#include "practice_states.h"
#include "bondinv.h"
#include "bondview.h"
#include "chr.h"
#include "gun.h"
#include "player.h"
#include "player_2.h"
#include "practice_ui.h"
#include <bondconstants.h>
#include <bondgame.h>
#include <joy.h>
#include <music.h>
#include <os_extension.h>
#include <snd.h>
#include <ultra64.h>

static bool g_HasSavedState = FALSE;

static void save_bond_state(void) {
  // TODO
}

static void load_bond_state(void) {
  // TODO
}

void save_game_state(void) {
  if (g_CurrentPlayer == NULL) {
    return;
  }

  save_bond_state();

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

  load_bond_state();

  sndPlaySfx(g_musicSfxBufferPtr, CAMERA_BEEP1_SFX, 0);
  practiceLogInfo("State loaded");
}
