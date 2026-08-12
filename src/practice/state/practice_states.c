#include "practice_states.h"
#include "../practice_config.h"
#include "../practice_grenade_cam.h"
#include "../practice_level.h"
#include "../practice_render.h"
#include "../practice_replay.h"
#include "../practice_sfx.h"
#include "../practice_sram.h"
#include "../practice_tests.h"
#include "../practice_timescale.h"
#include "emu_log.h"
#include "game/lvl.h"
#include "player.h"
#include "practice_ui.h"
#include <snd.h>
#include <ultra64.h>

extern s32 g_CurrentStageToLoad;
extern void store_osgetcount(void);

/* Small header cache so validation does not need to re-read storage. */
static SaveStateHeader g_SavedHeader __attribute__((aligned(16)));
bool g_HasSavedState = FALSE;
static PracticeStorageLocation g_SaveStateStorage = PRACTICE_STORAGE_SRAM;

static u32 get_save_state_storage_offset(void) {
  return g_SaveStateStorage == PRACTICE_STORAGE_SRAM ? SAVE_STATE_SRAM_OFFSET
                                                     : 0;
}

static u32 get_saved_state_size(void) {
  return g_SavedHeader.full_size != 0 ? g_SavedHeader.full_size
                                      : g_SavedHeader.size;
}

bool practice_states_set_storage_location(PracticeStorageLocation location) {
  g_SaveStateStorage = location;
  init_save_state_system();
  return storage_location_is_available(location);
}

PracticeStorageLocation practice_states_get_storage_location(void) {
  return g_SaveStateStorage;
}

void practice_states_notify_sram_overwritten(void) {
  if (g_SaveStateStorage == PRACTICE_STORAGE_SRAM) {
    g_SavedHeader.magic = 0;
    g_HasSavedState = FALSE;
  }
}

void practice_states_log_test_fixture(void) {
  static const char hex[] = "0123456789abcdef";
  StorageCursor cursor;
  u8 bytes[128] __attribute__((aligned(16)));
  char encoded[sizeof(bytes) * 2 + 1];
  u32 offset = 0;

  if (!g_HasSavedState)
    return;

  if (!storage_begin_load(
          g_SaveStateStorage, practice_level_short_name(g_CurrentStageToLoad),
          practice_difficulty_short_name(lvlGetSelectedDifficulty())))
    return;

  storage_cursor_init(&cursor, g_SaveStateStorage,
                      get_save_state_storage_offset());
  emu_log("RUNWAY_STATE_BEGIN size=%d", get_saved_state_size());
  while (offset < get_saved_state_size()) {
    u32 chunk = get_saved_state_size() - offset;
    u32 i;

    if (chunk > sizeof(bytes))
      chunk = sizeof(bytes);
    storage_read(&cursor, bytes, chunk);
    if (cursor.error)
      break;
    for (i = 0; i < chunk; i++) {
      encoded[i * 2] = hex[bytes[i] >> 4];
      encoded[i * 2 + 1] = hex[bytes[i] & 0xf];
    }
    encoded[chunk * 2] = '\0';
    emu_log("RUNWAY_STATE_DATA %s", encoded);
    offset += chunk;
  }
  emu_log("RUNWAY_STATE_END");
  storage_finish_load(g_SaveStateStorage);
}

typedef struct SaveSerializationResult {
  u32 size;
  bool props_saved;
  bool stream_error;
} SaveSerializationResult;

static SaveSerializationResult serialize_game_state(StateStream *stream,
                                                    bool *stream_error) {
  SaveSerializationResult result;

  result.size = 0;
  result.props_saved = FALSE;
  result.stream_error = FALSE;

  /* 1. Write placeholder header (magic, version, level_id, size=0). */
  {
    SaveStateHeader header;
    header.magic = SAVE_STATE_MAGIC;
    header.version = SAVE_STATE_VERSION;
    header.level_id = g_CurrentStageToLoad;
    header.size = 0; /* patched below */
    header.full_size = 0;
    write_bytes(stream, &header, sizeof(header));
  }

  /* 2. Write globals. */
  save_global_state(stream);

  /* 3. Write props and their associated player state. */
  if (!save_props_state(stream)) {
    result.stream_error = *stream_error;
    return result;
  }
  result.props_saved = TRUE;

  /* Flush the remaining bytes in the storage stream. */
  stream_flush(stream);
  result.size = stream->total_processed;
  result.stream_error = *stream_error;
  return result;
}

static SaveSerializationResult serialize_game_state_to_memory(void) {
  MemoryStream stream;
  memory_stream_init_write(&stream, g_SaveStateStorage,
                           get_save_state_storage_offset());
  return serialize_game_state(&stream.base, &stream.error);
}

static SaveSerializationResult serialize_game_state_to_storage(void) {
  StorageStream stream;
  storage_stream_init_write(&stream, g_SaveStateStorage,
                            get_save_state_storage_offset());
  return serialize_game_state(&stream.base, &stream.error);
}

void init_save_state_system(void) {
  StorageCursor cursor;

  if (!storage_location_is_available(g_SaveStateStorage)) {
    g_SavedHeader.magic = 0;
    g_HasSavedState = FALSE;
    return;
  }

  if (!storage_begin_load(
          g_SaveStateStorage, practice_level_short_name(g_CurrentStageToLoad),
          practice_difficulty_short_name(lvlGetSelectedDifficulty()))) {
    g_SavedHeader.magic = 0;
    g_HasSavedState = FALSE;
    return;
  }

  storage_cursor_init(&cursor, g_SaveStateStorage,
                      get_save_state_storage_offset());
  storage_read(&cursor, &g_SavedHeader, sizeof(g_SavedHeader));
  storage_finish_load(g_SaveStateStorage);

  g_HasSavedState = !cursor.error && g_SavedHeader.magic == SAVE_STATE_MAGIC &&
                    g_SavedHeader.version == SAVE_STATE_VERSION;
}

void save_game_state(void) {
  SaveSerializationResult result;
  StorageCursor header_cursor;

  if (g_CurrentPlayer == NULL)
    return;

  if (!storage_location_is_available(g_SaveStateStorage)) {
    practiceLogWarn("Selected save state storage is not available");
    return;
  }

  if (g_SaveStateStorage == PRACTICE_STORAGE_SRAM &&
      g_practice_test_case != PRACTICE_TEST_REPLAY_RUNWAY_SAVE_STATES) {
    practice_replay_stop_recording();
    practice_replay_stop_playback();
    practice_replay_invalidate_saved();
  }

  if (!storage_begin_save(
          g_SaveStateStorage, practice_level_short_name(g_CurrentStageToLoad),
          practice_difficulty_short_name(lvlGetSelectedDifficulty()),
          practice.max_save_states)) {
    practiceLogWarn("Could not start save state write");
    return;
  }

  result = g_SaveStateStorage == PRACTICE_STORAGE_EXPANSION_RAM
               ? serialize_game_state_to_memory()
               : serialize_game_state_to_storage();

  if (!result.props_saved) {
    storage_finish_save(g_SaveStateStorage, FALSE);
    g_HasSavedState = FALSE;
    practiceLogWarn("Failed to save state");
    return;
  }

  if (result.stream_error) {
    storage_finish_save(g_SaveStateStorage, FALSE);
    g_HasSavedState = FALSE;
    if (g_SaveStateStorage == PRACTICE_STORAGE_FLASHCART_SD) {
      practiceLogWarn("Failed while writing save state data");
    } else {
      practiceLogWarn("Save state is too large for selected storage");
    }
    return;
  }

  /* 4. Patch the header size field in g_SavedHeader. */
  g_SavedHeader.magic = SAVE_STATE_MAGIC;
  g_SavedHeader.version = SAVE_STATE_VERSION;
  g_SavedHeader.level_id = g_CurrentStageToLoad;
  g_SavedHeader.size = result.size;
  g_SavedHeader.full_size = result.size;

  storage_cursor_init(&header_cursor, g_SaveStateStorage,
                      get_save_state_storage_offset());
  storage_write(&header_cursor, &g_SavedHeader, sizeof(g_SavedHeader));
  if (header_cursor.error) {
    storage_finish_save(g_SaveStateStorage, FALSE);
    g_HasSavedState = FALSE;
    practiceLogWarn("Failed to write save state header");
    return;
  }

  if (!storage_finish_save(g_SaveStateStorage, TRUE)) {
    g_HasSavedState = FALSE;
    practiceLogWarn("Failed to finish save state write");
    return;
  }

  g_HasSavedState = TRUE;

  practice_sfx_play_save_state_sound();
  practiceLogInfo("State saved (%dKB)", (get_saved_state_size() + 1023) / 1024);
}

enum LoadSerializationResult {
  LOAD_SERIALIZATION_OK,
  LOAD_SERIALIZATION_DATA_FAILED,
  LOAD_SERIALIZATION_POST_PROPS_FAILED
};

static s32 deserialize_game_state(StateStream *stream, bool *stream_error) {
  /* 1. Skip header (already validated from g_SavedHeader). */
  stream_seek(stream,
              get_save_state_storage_offset() + sizeof(SaveStateHeader));

  /* 2. Load scalar globals and cache prop-dependent global references. */
  load_global_state_pre_props(stream);

  /* 3. Load props, followed by their associated player state. */
  if (!load_props_state(stream) || *stream_error) {
    return LOAD_SERIALIZATION_DATA_FAILED;
  }

  /* 4. Resolve prop-dependent globals and restore the current player. */
  if (!load_global_state_post_props()) {
    return LOAD_SERIALIZATION_POST_PROPS_FAILED;
  }

  return LOAD_SERIALIZATION_OK;
}

static s32 deserialize_game_state_from_memory(u32 storage_offset, u32 size) {
  MemoryStream stream;
  memory_stream_init_read(&stream, g_SaveStateStorage, storage_offset, size);
  return deserialize_game_state(&stream.base, &stream.error);
}

static s32 deserialize_game_state_from_storage(u32 storage_offset, u32 size) {
  StorageStream stream;
  storage_stream_init_read(&stream, g_SaveStateStorage, storage_offset, size);
  return deserialize_game_state(&stream.base, &stream.error);
}

void load_game_state(void) {
  s32 load_result;
  s32 paused_resume_delta = g_IsTimePaused ? g_ForcedDeltaFrames : -1;
  u32 storage_offset = get_save_state_storage_offset();

  if (g_CurrentPlayer == NULL) {
    return;
  }

  if (g_SaveStateStorage == PRACTICE_STORAGE_FLASHCART_SD) {
    /* The latest save is selected independently for the current level, so
     * refresh the cached header whenever the load hotkey is used. */
    init_save_state_system();
  }

  if (!g_HasSavedState) {
    practiceLogWarn("No saved state");
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
      storage_offset > storage_location_size(g_SaveStateStorage) ||
      get_saved_state_size() >
          storage_location_size(g_SaveStateStorage) - storage_offset) {
    practiceLogWarn("Invalid save size %d", get_saved_state_size());
    return;
  }

  if (!storage_begin_load(
          g_SaveStateStorage, practice_level_short_name(g_CurrentStageToLoad),
          practice_difficulty_short_name(lvlGetSelectedDifficulty()))) {
    practiceLogWarn("Could not open save state");
    return;
  }

  /* Stop all active sound effects before loading state. */
  sndDeactivateAllSfxByFlag_1();

  load_result = g_SaveStateStorage == PRACTICE_STORAGE_EXPANSION_RAM
                    ? deserialize_game_state_from_memory(storage_offset,
                                                         get_saved_state_size())
                    : deserialize_game_state_from_storage(
                          storage_offset, get_saved_state_size());

  if (load_result == LOAD_SERIALIZATION_DATA_FAILED) {
    storage_finish_load(g_SaveStateStorage);
    practiceLogWarn("Failed to load state");
    return;
  }
  if (load_result == LOAD_SERIALIZATION_POST_PROPS_FAILED) {
    storage_finish_load(g_SaveStateStorage);
    practiceLogWarn("Failed to restore post-prop globals");
    return;
  }

  storage_finish_load(g_SaveStateStorage);

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
