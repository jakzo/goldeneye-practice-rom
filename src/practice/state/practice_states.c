#include "practice_states.h"
#include "player.h"
#include "practice_sram.h"
#include "practice_states_bond.h"
#include "practice_states_globals.h"
#include "practice_ui.h"
#include <bondconstants.h>
#include <music.h>
#include <snd.h>
#include <ultra64.h>

static union {
  SaveState state;
  // Force g_SaveStateUnion (and therefore state) to be 8-byte aligned
  u64 align_dummy;
} g_SaveStateUnion;
#define g_SaveState g_SaveStateUnion.state

extern s32 g_CurrentStageToLoad;

bool g_HasSavedState = FALSE;

void init_save_state_system(void) {
  SaveStateHeader header;
  bool isValidSave;

  sram_read(SAVE_STATE_SRAM_OFFSET, &header, sizeof(SaveStateHeader));

  isValidSave =
      header.magic == SAVE_STATE_MAGIC && header.size <= sizeof(SaveState);
  if (isValidSave) {
    sram_read(SAVE_STATE_SRAM_OFFSET, &g_SaveState, header.size);
    g_HasSavedState = TRUE;
  } else {
    g_HasSavedState = FALSE;
  }
}

void save_game_state(void) {
  u8 *start;
  u8 *prop_data_start;
  s32 data_size;

  if (g_CurrentPlayer == NULL)
    return;

  g_SaveState.header.magic = SAVE_STATE_MAGIC;
  g_SaveState.header.version = SAVE_STATE_VERSION;
  g_SaveState.header.level_id = g_CurrentStageToLoad;

  save_global_state(&g_SaveState.global_state);
  save_bond_state(&g_SaveState.bond_state);
  if (!save_props_state(&g_SaveState.props_state)) {
    practiceLogWarn("Failed to save state");
    return;
  }

  g_HasSavedState = TRUE;

  start = (u8 *)&g_SaveState;
  prop_data_start = (u8 *)g_SaveState.props_state.data;
  data_size = prop_data_start + g_SaveState.props_state.size - start;
  g_SaveState.header.size = data_size;

  sram_write(SAVE_STATE_SRAM_OFFSET, &g_SaveState, g_SaveState.header.size);

  sndPlaySfx(g_musicSfxBufferPtr, CAMERA_BEEP1_SFX, 0);
  practiceLogInfo("State saved");
}

void load_game_state(void) {
  if (g_CurrentPlayer == NULL || !g_HasSavedState) {
    if (!g_HasSavedState) {
      practiceLogWarn("No saved state");
    }
    return;
  }

  if (g_SaveState.header.magic != SAVE_STATE_MAGIC) {
    practiceLogWarn("Invalid save");
    return;
  }

  if (g_SaveState.header.version < SAVE_STATE_VERSION) {
    practiceLogWarn("Save was made with an older ROM version");
    return;
  }

  if (g_SaveState.header.version > SAVE_STATE_VERSION) {
    practiceLogWarn("Save was made with a newer ROM version");
    return;
  }

  if (g_SaveState.header.level_id != g_CurrentStageToLoad) {
    practiceLogWarn("Save does not match current level");
    return;
  }

  // Stop all active sound effects before loading state
  sndDeactivateAllSfxByFlag_1();

  load_global_state(&g_SaveState.global_state);
  load_bond_state(&g_SaveState.bond_state);
  if (!load_props_state(&g_SaveState.props_state)) {
    practiceLogWarn("Failed to load state");
    return;
  }

  sndPlaySfx(g_musicSfxBufferPtr, CAMERA_BEEP1_SFX, 0);
  practiceLogInfo("State loaded");
}
