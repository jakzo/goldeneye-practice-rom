#include "boss.h"
#include "os.h"
#include "player_2.h"
#include "practice_config.h"
#include "practice_replay.h"
#include "practice_splits.h"
#include "practice_timescale.h"
#include "practice_ui.h"
#include "state/practice_states.h"
#include <bondconstants.h>
#include <bondgame.h>
#include <joy.h>
#include <os_extension.h>
#include <ultra64.h>

extern s32 g_CurrentStageToLoad;
extern s32 g_BgCurrentRoom; // player's current room (defined in bg.c)

extern int sprintf(char *dst, const char *fmt, ...);

u16 hotkey_trigger(void) {
  return practice.left_trigger_hotkeys ? L_TRIG : R_TRIG;
}

void practice_check_hotkeys(void) {
  s32 controller;
  s32 action_controller = -1;
  bool trigger_held = FALSE;
  u16 trigger = hotkey_trigger();

  // Switch to real controller temporarily for hotkeys during replay
  if (g_ReplayIsPlaying)
    joySetContDataIndex(0);

  for (controller = 0; controller < MAXCONTROLLERS; controller++) {
    u16 buttons = joyGetButtons(controller, ANY_BUTTON);
    u16 pressed = joyGetButtonsPressedThisFrame(controller, ANY_BUTTON);

    if (buttons & trigger) {
      trigger_held = TRUE;
      if (action_controller < 0 && (pressed & ~trigger))
        action_controller = controller;
    }

    if ((pressed & trigger) && !g_IsTimePaused)
      pause();
  }

  if (g_ReplayIsPlaying)
    joySetContDataIndex(1);

  if (!trigger_held) {
    if (g_IsTimePaused)
      unpause();
    return;
  }

  if (action_controller >= 0) {
    u16 pressed;

    if (g_ReplayIsPlaying)
      joySetContDataIndex(0);
    pressed = joyGetButtonsPressedThisFrame(action_controller, ANY_BUTTON);
    if (g_ReplayIsPlaying)
      joySetContDataIndex(1);

    if (pressed & D_JPAD) {
      save_game_state();
      return;
    }
    if (pressed & U_JPAD) {
      load_game_state();
      return;
    }

    if (pressed & L_JPAD) {
      f32 new_time_scale = ((s32)(g_TimeScale * 10.0f + 0.5f) - 1) * 0.1f;
      set_time_scale(new_time_scale);
      practiceLogInfo("Time scale: %.1f", g_TimeScale);
      return;
    }
    if (pressed & R_JPAD) {
      f32 new_time_scale = ((s32)(g_TimeScale * 10.0f + 0.5f) + 1) * 0.1f;
      set_time_scale(new_time_scale);
      practiceLogInfo("Time scale: %.1f", g_TimeScale);
      return;
    }

    if (pressed & D_CBUTTONS) {
      s32 previous_player = get_cur_playernum();
      if (action_controller < getPlayerCount())
        set_cur_player(action_controller);
      splits_log_position();
      set_cur_player(previous_player);
      return;
    }

    if (pressed & START_BUTTON) {
      unpause();
      bossSetLoadedStage(g_CurrentStageToLoad);
      practiceLogInfo("Level restarted");
      return;
    }

#if DEV
    // Log the room the player is currently in (useful for finding room IDs).
    if (pressed & U_CBUTTONS) {
      s32 previous_player = get_cur_playernum();
      if (action_controller < getPlayerCount())
        set_cur_player(action_controller);
      practiceLogInfo("Room: %d", g_BgCurrentRoom);
      set_cur_player(previous_player);
      return;
    }
#endif
  }
}
