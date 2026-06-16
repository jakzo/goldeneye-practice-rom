#include "practice_replay.h"
#include "file2.h"
#include "game/lvl.h"
#include "joy.h"
#include "memp.h"
#include "practice_ui.h"
#include "watch.h"

extern s32 g_CurrentStageToLoad;
extern u64 g_randomSeed;
extern u64 g_chrObjRandomSeed;
extern s32 g_ClockTimer;
extern s32 g_GlobalTimer;

ReplaySession *g_ReplaySession = NULL;
bool g_ReplayIsRecording = FALSE;

static s32 g_LastGlobalTimer = 0;
static s32 g_LastRecordedSettingsControlStyle = 0;
static u16 g_LastRecordedSettingsFlags = 0;

static u16 get_current_settings_flags(void) {
  // Logic copied from fileSaveSettingsForFolder
  u16 flags = 0;
  if (get_cur_player_look_vertical_inverted())
    flags |= OPTION_INVERTLOOK;
  if (cur_player_get_autoaim())
    flags |= OPTION_AUTOAIM;
  if (cur_player_get_aim_control())
    flags |= OPTION_AIMCONTROL;
  if (cur_player_get_sight_onscreen_control())
    flags |= OPTION_SIGHTONSCREEN;
  if (cur_player_get_lookahead())
    flags |= OPTION_LOOKAHEAD;
  if (cur_player_get_ammo_onscreen_setting())
    flags |= OPTION_DISPLAYAMMO;
  if (cur_player_get_screen_setting() == SCREEN_SIZE_WIDESCREEN)
    flags |= OPTION_SCREENWIDE;
  else if (cur_player_get_screen_setting() == SCREEN_SIZE_CINEMA)
    flags |= OPTION_SCREENCINEMA;
  if (get_screen_ratio() != SCREEN_RATIO_NORMAL)
    flags |= OPTION_SCREENRATIO;
  flags |= ((u16)(cur_player_get_control_type() << 8)) & OPTION_CONTROLTYPE;
  return flags;
}

static bool practice_replay_is_dual_controller(s32 control_style) {
  return (control_style == CONTROLLER_CONFIG_PLENTY ||
          control_style == CONTROLLER_CONFIG_GALORE ||
          control_style == CONTROLLER_CONFIG_DOMINO ||
          control_style == CONTROLLER_CONFIG_GOODHEAD);
}

void practice_replay_on_stage_load(void) {
  // TODO: This is unfinished
  return;

  g_ReplaySession = NULL;
  g_ReplayIsRecording = FALSE;
  joySetRecordFunc(NULL);

  /* Auto-start recording for gameplay stages */
  if (g_CurrentStageToLoad != LEVELID_TITLE) {
    practice_replay_start_recording();
  }
}

void practice_replay_start_recording(void) {
  s32 control_style;
  bool is_dual;
  ReplayEvent *first_event;

  control_style = cur_player_get_control_type();
  is_dual = practice_replay_is_dual_controller(control_style);

  g_ReplaySession = (ReplaySession *)mempAllocBytesInBank(sizeof(ReplaySession),
                                                          MEMPOOL_STAGE);
  if (g_ReplaySession == NULL) {
    practiceLogWarn("Replay: Failed to alloc buffer");
    return;
  }

  g_ReplaySession->header.magic = REPLAY_MAGIC;
  g_ReplaySession->header.version = REPLAY_VERSION;
  g_ReplaySession->header.region = REPLAY_REGION;
  g_ReplaySession->header.stage_id = g_CurrentStageToLoad;
  g_ReplaySession->header.difficulty = lvlGetSelectedDifficulty();
  g_ReplaySession->header.random_seed = g_randomSeed;
  g_ReplaySession->header.chr_obj_random_seed = g_chrObjRandomSeed;
  g_ReplaySession->header.event_count = 0;

  g_LastRecordedSettingsControlStyle = control_style;
  g_LastRecordedSettingsFlags = 0;

  /* Record starting settings as the first event */
  first_event = &g_ReplaySession->events[0];
  g_ReplaySession->header.event_count = 1;

  g_LastGlobalTimer = g_GlobalTimer;
  g_ReplayIsRecording = TRUE;

  joySetRecordFunc(practice_replay_record_callback);
  practiceLogInfo("Replay: Recording started");
}

void practice_replay_stop_recording(void) {
  if (!g_ReplayIsRecording || g_ReplaySession == NULL)
    return;
  g_ReplayIsRecording = FALSE;
  joySetRecordFunc(NULL);
  practiceLogInfo("Replay: Stopped. Events: %d",
                  g_ReplaySession->header.event_count);
}

void practice_replay_record_callback(struct contsample *samples, s32 start_idx,
                                     s32 end_idx) {
  // TODO
}
