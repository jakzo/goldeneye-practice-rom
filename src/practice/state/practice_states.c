#include "practice_states.h"
#include "practice_states_bond.h"
#include "practice_states_globals.h"
#include "practice_ui.h"
#include "player.h"
#include <snd.h>
#include <music.h>
#include <bondconstants.h>
#include <ultra64.h>

static SaveState g_SaveState;
extern s32 g_CurrentStageToLoad;

bool g_HasSavedState = FALSE;

void save_game_state(void) {
  if (g_CurrentPlayer == NULL) {
    return;
  }

  g_SaveState.magic = SAVE_STATE_MAGIC;
  g_SaveState.version = SAVE_STATE_VERSION;
  g_SaveState.size = sizeof(SaveState);
  g_SaveState.level_id = g_CurrentStageToLoad;

  save_bond_state(&g_SaveState.bond_state);
  save_global_state(&g_SaveState.global_state);

  g_HasSavedState = TRUE;
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

  if (g_SaveState.magic != SAVE_STATE_MAGIC) {
    practiceLogWarn("Invalid save");
    return;
  }

  if (g_SaveState.version < SAVE_STATE_VERSION) {
    practiceLogWarn("Save was made with an older ROM version");
    return;
  }

  if (g_SaveState.version > SAVE_STATE_VERSION) {
    practiceLogWarn("Save was made with a newer ROM version");
    return;
  }

  if (g_SaveState.level_id != g_CurrentStageToLoad) {
    practiceLogWarn("Save does not match current level");
    return;
  }

  load_bond_state(&g_SaveState.bond_state);
  load_global_state(&g_SaveState.global_state);

  sndPlaySfx(g_musicSfxBufferPtr, CAMERA_BEEP1_SFX, 0);
  practiceLogInfo("State loaded");
}
