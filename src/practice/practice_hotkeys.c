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

bool practice_check_hotkeys(void) {
  u16 jgbptf;
  u16 jgb;
  u16 trigger = hotkey_trigger();
  bool finish_replay_frame = FALSE;

  // Switch to real controller temporarily for hotkeys during replay
  if (g_ReplayIsPlaying)
    joySetContDataIndex(0);

  jgb = joyGetButtons(get_cur_playernum(), ANY_BUTTON);
  jgbptf = joyGetButtonsPressedThisFrame(get_cur_playernum(), ANY_BUTTON);

  if (g_ReplayIsPlaying)
    joySetContDataIndex(1);

  if (!(jgb & trigger)) {
    if (g_IsTimePaused)
      unpause();
    return FALSE;
  }

  if ((jgbptf & trigger) && !g_IsTimePaused) {
    pause();
    // Timing and replay input are selected before hotkeys are checked. If the
    // trigger arrived on an advancing replay frame, let that selected frame's
    // gameplay finish before the held pause starts on the following frame.
    finish_replay_frame =
        g_ReplayIsPlaying && !practice_replay_is_paused_frame();
  }

  if (jgbptf & D_JPAD) {
    save_game_state();
    return TRUE;
  }
  if (jgbptf & U_JPAD) {
    load_game_state();
    return TRUE;
  }

  if (jgbptf & L_JPAD) {
    f32 new_time_scale = ((s32)(g_TimeScale * 10.0f + 0.5f) - 1) * 0.1f;
    set_time_scale(new_time_scale);
    practiceLogInfo("Time scale: %.1f", g_TimeScale);
    return TRUE;
  }
  if (jgbptf & R_JPAD) {
    f32 new_time_scale = ((s32)(g_TimeScale * 10.0f + 0.5f) + 1) * 0.1f;
    set_time_scale(new_time_scale);
    practiceLogInfo("Time scale: %.1f", g_TimeScale);
    return TRUE;
  }

  if (jgbptf & D_CBUTTONS) {
    splits_log_position();
    return TRUE;
  }

  if (jgbptf & START_BUTTON) {
    unpause();
    bossSetLoadedStage(g_CurrentStageToLoad);
    practiceLogInfo("Level restarted");
    return TRUE;
  }

#if DEV
  // Log the room the player is currently in (useful for finding room IDs).
  if (jgbptf & U_CBUTTONS) {
    practiceLogInfo("Room: %d", g_BgCurrentRoom);
    return TRUE;
  }
#endif

  return !finish_replay_frame;
}
