#include "practice_states.h"
#include "../practice_replay.h"
#include "../practice_grenade_cam.h"
#include "../practice_render.h"
#include "../practice_sfx.h"
#include "../practice_sram.h"
#include "../practice_timescale.h"
#include "player.h"
#include "practice_ui.h"
#include "emu_log.h"
#include <snd.h>
#include <ultra64.h>

extern s32 g_CurrentStageToLoad;
extern void store_osgetcount(void);

/* Small header cache so we can validate without re-reading SRAM. */
static SaveStateHeader g_SavedHeader __attribute__((aligned(16)));
bool g_HasSavedState = FALSE;
static u32 g_SaveStateSramOffset = SAVE_STATE_SRAM_OFFSET;
static bool g_PreserveReplayForTest = FALSE;

static u32 get_saved_state_size(void) {
  return g_SavedHeader.full_size != 0 ? g_SavedHeader.full_size
                                      : g_SavedHeader.size;
}

void practice_states_set_test_storage(bool enabled) {
  g_SaveStateSramOffset =
      enabled ? TEST_SAVE_STATE_SRAM_OFFSET : SAVE_STATE_SRAM_OFFSET;
  g_PreserveReplayForTest = enabled;
  init_save_state_system();
}

void practice_states_log_test_fixture(void) {
  static const char hex[] = "0123456789abcdef";
  SramStream stream;
  u8 bytes[128];
  char encoded[sizeof(bytes) * 2 + 1];
  u32 offset = 0;

  if (!g_HasSavedState)
    return;

  sram_stream_init_read(&stream, g_SaveStateSramOffset);
  emu_log("RUNWAY_STATE_BEGIN size=%d", get_saved_state_size());
  while (offset < get_saved_state_size()) {
    u32 chunk = get_saved_state_size() - offset;
    u32 i;

    if (chunk > sizeof(bytes))
      chunk = sizeof(bytes);
    read_bytes(&stream.base, bytes, chunk);
    for (i = 0; i < chunk; i++) {
      encoded[i * 2] = hex[bytes[i] >> 4];
      encoded[i * 2 + 1] = hex[bytes[i] & 0xf];
    }
    encoded[chunk * 2] = '\0';
    emu_log("RUNWAY_STATE_DATA %s", encoded);
    offset += chunk;
  }
  emu_log("RUNWAY_STATE_END");
}

void init_save_state_system(void) {
  sram_read(g_SaveStateSramOffset, &g_SavedHeader, sizeof(g_SavedHeader));

  g_HasSavedState = g_SavedHeader.magic == SAVE_STATE_MAGIC &&
                    g_SavedHeader.version == SAVE_STATE_VERSION;
}

void save_game_state(void) {
  SramStream stream;

  if (g_CurrentPlayer == NULL)
    return;

  if (!g_PreserveReplayForTest) {
    practice_replay_stop_recording();
    practice_replay_stop_playback();
    practice_replay_invalidate_saved();
  }

  sram_stream_init_write(&stream, g_SaveStateSramOffset);

  /* 1. Write placeholder header (magic, version, level_id, size=0). */
  {
    SaveStateHeader header;
    header.magic = SAVE_STATE_MAGIC;
    header.version = SAVE_STATE_VERSION;
    header.level_id = g_CurrentStageToLoad;
    header.size = 0; /* patched below */
    header.full_size = 0;
    write_bytes(&stream.base, &header, sizeof(header));
  }

  /* 2. Write globals. */
  save_global_state(&stream.base);

  /* 3. Write props and their associated player state. */
  if (!save_props_state(&stream.base)) {
    practiceLogWarn("Failed to save state");
    return;
  }

  /* Flush the remaining bytes in the buffer to SRAM. */
  stream_flush(&stream.base);

  if (stream.error) {
    g_HasSavedState = FALSE;
    practiceLogWarn("Save state is too large for SRAM");
    return;
  }

  /* 4. Patch the header size field in g_SavedHeader. */
  g_SavedHeader.magic = SAVE_STATE_MAGIC;
  g_SavedHeader.version = SAVE_STATE_VERSION;
  g_SavedHeader.level_id = g_CurrentStageToLoad;
  g_SavedHeader.size = stream.base.total_processed;
  g_SavedHeader.full_size = stream.base.total_processed;

  sram_write(g_SaveStateSramOffset, &g_SavedHeader, sizeof(g_SavedHeader));

  g_HasSavedState = TRUE;

  practice_sfx_play_save_state_sound();
  practiceLogInfo("State saved (%dKB)",
                  (get_saved_state_size() + 1023) / 1024);
}

void load_game_state(void) {
  SramStream stream;
  s32 paused_resume_delta = g_IsTimePaused ? g_ForcedDeltaFrames : -1;

  if (g_CurrentPlayer == NULL || !g_HasSavedState) {
    if (!g_HasSavedState) {
      practiceLogWarn("No saved state");
    }
    return;
  }

  if (g_SavedHeader.magic != SAVE_STATE_MAGIC) {
    practiceLogWarn("Invalid save");
    return;
  }

  if (g_SavedHeader.version < SAVE_STATE_VERSION) {
    practiceLogWarn("Save was made with an older ROM version");
    return;
  }

  if (g_SavedHeader.version > SAVE_STATE_VERSION) {
    practiceLogWarn("Save was made with a newer ROM version");
    return;
  }

  if (g_SavedHeader.level_id != g_CurrentStageToLoad) {
    practiceLogWarn("Save does not match current level");
    return;
  }

  if (get_saved_state_size() < sizeof(SaveStateHeader) ||
      get_saved_state_size() > SRAM_SIZE_BYTES - g_SaveStateSramOffset) {
    practiceLogWarn("Invalid save size %d", get_saved_state_size());
    return;
  }

  /* Stop all active sound effects before loading state. */
  sndDeactivateAllSfxByFlag_1();

  sram_stream_init_read(&stream, g_SaveStateSramOffset);

  /* 1. Skip header (already validated from g_SavedHeader). */
  stream_seek(&stream.base, g_SaveStateSramOffset + sizeof(SaveStateHeader));

  /* 2. Load scalar globals and cache prop-dependent global references. */
  load_global_state_pre_props(&stream.base);

  /* 3. Load props, followed by their associated player state. */
  if (!load_props_state(&stream.base)) {
    practiceLogWarn("Failed to load state");
    return;
  }

  /* 4. Resolve prop-dependent globals and restore the current player. */
  if (!load_global_state_post_props()) {
    practiceLogWarn("Failed to restore post-prop globals");
    return;
  }

  freeze_current_frame_after_load(paused_resume_delta);

  practice_grenade_cam_refresh();

  /* The current paused frame did not tick the newly restored model graph. */
  practice_invalidate_render_state();

  // Re-baseline frame timer so time isn't dumped into the next deltaFrames
  store_osgetcount();

  practice_sfx_play_save_state_sound();
  /* Loading, model reconstruction, and refresh helpers may consume randomness.
   * The resumed gameplay frame must start from the serialized sequence. */
  restore_global_rng_after_load();
  sync_frozen_rng_after_load();
  practiceLogInfo("State loaded");
}
