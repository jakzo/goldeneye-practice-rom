#include "practice_states_globals.h"
#include "bg.h"
#include "bondview.h"
#include "chr.h"
#include "chrai.h"
#include "chrobjhandler.h"
#include "fog.h"
#include "fr.h"
#include "loadobjectmodel.h"
#include "lvl.h"
#include "objective_status.h"
#include "player.h"
#include "player_2.h"
#include "practice_dialog.h"
#include "practice_lag.h"
#include "practice_states_music.h"
#include "practice_states_utils.h"
#include "practice_timescale.h"
#include "practice_ui.h"
#include "unk_092E50.h"
#include <assert.h>
#include <ultra64.h>

#define BONDVIEW_HUD_MSG_BOTTOM_BUFFER_LENGTH 0x65
#ifdef VERSION_EU
#define BG_VISIBLE_ROOM_LIST_LENGTH 0x8c
#else
#define BG_VISIBLE_ROOM_LIST_LENGTH 0x98
#endif
#ifdef VERSION_EU
#define ROOM_TRANSFORM_CACHE_LENGTH 100
#else
#define ROOM_TRANSFORM_CACHE_LENGTH 300
#endif

extern char stringbuffer_lowerleft[0x5][BONDVIEW_HUD_MSG_BOTTOM_BUFFER_LENGTH];
extern s32 status_bar_text_buffer_index;
extern s32 display_statusbar;
extern s32 g_EnterTankAudioState;
extern struct coord3d g_EnterTankCoord;
extern f32 g_TankEnterBondHorizAngleDeg;
extern f32 g_TankEnterBondVertAngleDeg;
extern s32 g_TankEngineSfxVolume;
extern f32 g_TankEnteringSitHeight;
extern f32 g_TankEnteringSitHeightRemain;
extern struct coord3d g_TankModelPositionOffset;
extern s32 g_GlobalTimer;
extern s32 mission_timer;
extern s32 g_ViShakeIntensity;
extern u32 g_ViShakeTimer;
extern s32 g_BgCurrentRoom;
extern char
    list_visible_rooms_in_cur_global_vis_packet[BG_VISIBLE_ROOM_LIST_LENGTH];
extern s32 num_visible_rooms_in_cur_global_vis_packet;
extern u8 roomStatusFlags[ROOM_TRANSFORM_CACHE_LENGTH];
extern s32 roomIndices[ROOM_TRANSFORM_CACHE_LENGTH];
extern s32 roomOwners[ROOM_TRANSFORM_CACHE_LENGTH];
extern Mtx roomMatrices[ROOM_TRANSFORM_CACHE_LENGTH];
extern u32 D_80040990;
extern u32 watch_screen_index;
extern u32 controller_options_index;
extern u32 game_options_index;
extern s32 mission_brief_index;
extern s32 D_800409A4;
extern s32 watch_item_is_actively_selected;
extern s32 D_800409AC;
extern s32 D_800409B0;
extern s32 D_800409B4;
extern s32 g_curWatchItemIndex;
extern f32 D_800409BC;
extern s32 D_800409C0;
extern s32 D_800409C4;
extern f32 D_800409C8;
extern f32 D_800409CC;
extern s32 D_800409D0;
extern f32 D_800409D4;
extern s32 D_800409D8;
extern u32 controlstick_lr_enabled;
extern u32 D_80040AE0;
extern u32 D_80040AE4;
extern f32 D_80040AE8;
extern f32 D_80040AEC;
extern f32 D_80040AF0;
extern u32 D_80040AF4;
extern u32 D_80040AF8;
extern u32 D_80040AFC;
extern u32 D_80040B00;
extern s32 g_WatchBackgroundGreen;
extern u32 D_80040B08;
extern u32 D_80040B0C;
extern u32 D_80040B10;
extern f32 D_80040B14;
extern f32 D_80040B18;
extern f32 D_80040B1C;
extern f32 D_80040B20;
extern f32 D_80040B24;
extern f32 D_80040B28;
extern u32 D_80040B2C;
extern f32 D_80040B30;
extern f32 D_80040B34;
extern f32 D_80040B38;
extern u32 D_80040B3C;
extern s32 D_80040B40;
extern u16 D_80040B44;
extern u32 D_80040B48;
extern u32 D_80040B4C;
extern u32 D_80040B50;
extern u32 D_80040B54;
extern struct coord3d flt_CODE_bss_800799E8;
extern PropRecord *dword_CODE_bss_800799F4;
extern PadRecord *dword_CODE_bss_800799F8;
extern CutsceneRecord *gBondViewCutscene;
extern f32 flt_CODE_bss_80079A00;
extern f32 flt_CODE_bss_80079A04;
extern f32 flt_CODE_bss_80079A08;
extern f32 flt_CODE_bss_80079A0C;
extern f32 flt_CODE_bss_80079A10;
extern s32 dword_CODE_bss_80079A14;
extern enum CAMERAMODE dword_CODE_bss_80079A18;
extern s32 dword_CODE_bss_80079A1C;
extern f32 watch_transition_time;
extern u64 g_randomSeed;
extern u64 g_chrObjRandomSeed;
extern NearFogRecord *g_NearFogValuesP;
extern f32 g_FarFogIntensity;
extern f32 g_DifferenceFromFarFogIntensity;
extern f32 g_ScaledFarFogIntensity;
extern f32 g_ScaledDifferenceFromFarFogIntensity;
extern EnvironmentRecord *g_EnvironmentFoundp;
extern EnvironmentRecord *g_EnvironmentMainp;
extern EnvironmentRecord *g_EnvironmentAltp;
extern EnvironmentRecord fog_tables[];
#if defined(VERSION_EU)
extern NearFogRecordF eu_loadCurrentNearFog;
#endif
struct PracticeFogDetails {
  f32 current;
  f32 scaled_far;
  u32 unk08;
  u32 unk0c;
  f32 far_scaled;
  f32 near_scaled;
};
extern struct PracticeFogDetails g_CurFogDetails;
extern void sub_GAME_7F0BD8FC(s32 arg0);

static s32 saved_player_tank_prop_index;
static s32 saved_world_tank_prop_index;
static s32 saved_current_player_index;
static s32 saved_death_camera_prop_index;
static u64 saved_random_seed;
static u64 saved_chr_obj_random_seed;
static s32 saved_player1_guard_id;

static s32 get_camera_pad_id(PadRecord *saved_pad) {
  s32 i;

  if (saved_pad == NULL) {
    return -1;
  }

  if (g_CurrentSetup.pads != NULL) {
    for (i = 0; g_CurrentSetup.pads[i].plink != NULL; i++) {
      if (saved_pad == &g_CurrentSetup.pads[i]) {
        return i;
      }
    }
  }

  if (g_CurrentSetup.boundpads != NULL) {
    for (i = 0; g_CurrentSetup.boundpads[i].plink != NULL; i++) {
      if (saved_pad == (PadRecord *)&g_CurrentSetup.boundpads[i]) {
        return i + 10000;
      }
    }
  }

  practiceLogWarn("Could not identify frozen camera pad %08x", saved_pad);
  return -1;
}

static PadRecord *get_camera_pad_by_id(s32 pad_id) {
  if (pad_id < 0) {
    return NULL;
  }
  if (isNotBoundPad(pad_id)) {
    return &g_CurrentSetup.pads[pad_id];
  }
  return (PadRecord *)&g_CurrentSetup.boundpads[getBoundPadNum(pad_id)];
}

static void save_background_ai_state(StateStream *stream) {
  s32 i;

  write_u32(stream, g_ActiveChrsCount);

  for (i = 0; i < g_ActiveChrsCount; i++) {
    ChrRecord *chr = &g_ActiveChrs[i];
    s32 ailist_reference = get_ai_list_reference(chr->ailist);

    write_u16(stream, (u16)chr->chrnum);
    write_u8(stream, (u8)chr->actiontype);
    write_u8(stream, (u8)chr->sleep);
    write_u16(stream, chr->hidden);
    write_u32(stream, chr->chrflags);
    write_u8(stream, chr->morale);
    write_u8(stream, chr->alertness);
    write_u8(stream, chr->flags2);
    write_u8(stream, chr->random);
    write_u32(stream, chr->timer60);
    write_u16(stream, (u16)chr->padpreset1);
    write_u16(stream, (u16)chr->chrpreset1);
    write_u16(stream, (u16)chr->chrseeshot);
    write_u16(stream, (u16)chr->chrseedie);
    write_u32(stream, ailist_reference);
    write_u16(stream, chr->aioffset);
    write_u16(stream, (u16)chr->aireturnlist);
  }
}

static void load_background_ai_state(StateStream *stream) {
  s32 saved_count = read_u32(stream);
  s32 i;

  for (i = 0; i < saved_count; i++) {
    s16 chrnum = (s16)read_u16(stream);
    u8 actiontype = read_u8(stream);
    s8 sleep = (s8)read_u8(stream);
    u16 hidden = read_u16(stream);
    u32 chrflags = read_u32(stream);
    u8 morale = read_u8(stream);
    u8 alertness = read_u8(stream);
    u8 flags2 = read_u8(stream);
    u8 random = read_u8(stream);
    s32 timer60 = read_u32(stream);
    s16 padpreset1 = (s16)read_u16(stream);
    s16 chrpreset1 = (s16)read_u16(stream);
    s16 chrseeshot = (s16)read_u16(stream);
    s16 chrseedie = (s16)read_u16(stream);
    s32 ailist_reference = read_u32(stream);
    u16 aioffset = read_u16(stream);
    s16 aireturnlist = (s16)read_u16(stream);

    if (i < g_ActiveChrsCount) {
      ChrRecord *chr = &g_ActiveChrs[i];
      AIRecord *ailist = get_ai_list_by_reference(ailist_reference);

      chr->chrnum = chrnum;
      chr->actiontype = actiontype;
      chr->sleep = sleep;
      chr->hidden = hidden;
      chr->chrflags = chrflags;
      chr->morale = morale;
      chr->alertness = alertness;
      chr->flags2 = flags2;
      chr->random = random;
      chr->timer60 = timer60;
      chr->padpreset1 = padpreset1;
      chr->chrpreset1 = chrpreset1;
      chr->chrseeshot = chrseeshot;
      chr->chrseedie = chrseedie;
      chr->ailist = ailist;
      chr->aioffset = ailist != NULL ? aioffset : 0;
      chr->aireturnlist = ailist != NULL ? aireturnlist : -1;
    }
  }

  if (saved_count != g_ActiveChrsCount) {
    practiceLogWarn("Background AI count changed (%d saved, %d live)",
                    saved_count, g_ActiveChrsCount);
  }
}

static void save_sky_state(StateStream *stream) {
  f32 zrange[2];

  viGetZRange(zrange);
  write_f32(stream, g_SkyCloudOffset);
  write_u32(stream, g_FogSkyIsEnabled);
  write_bytes(stream, fogGetCurrentEnvironmentp(),
              sizeof(CurrentEnvironmentRecord));
  write_bytes(stream, &g_CurFogDetails, sizeof(g_CurFogDetails));
  write_f32(stream, g_FarFogIntensity);
  write_f32(stream, g_DifferenceFromFarFogIntensity);
  write_f32(stream, g_ScaledFarFogIntensity);
  write_f32(stream, g_ScaledDifferenceFromFarFogIntensity);
  write_f32(stream, zrange[0]);
  write_f32(stream, zrange[1]);
  write_u32(stream, g_EnvironmentMainp != NULL ? g_EnvironmentMainp->Id : 0);
  write_u8(stream, g_EnvironmentFoundp != NULL);
  write_u8(stream, g_NearFogValuesP != NULL);
}

static void load_sky_state(StateStream *stream) {
  f32 zrange[2];
  s32 environment_id;
  bool had_environment;
  bool had_near_fog;
  EnvironmentRecord *environment;

  g_SkyCloudOffset = read_f32(stream);
  g_FogSkyIsEnabled = read_u32(stream);
  read_bytes(stream, fogGetCurrentEnvironmentp(),
             sizeof(CurrentEnvironmentRecord));
  read_bytes(stream, &g_CurFogDetails, sizeof(g_CurFogDetails));
  g_FarFogIntensity = read_f32(stream);
  g_DifferenceFromFarFogIntensity = read_f32(stream);
  g_ScaledFarFogIntensity = read_f32(stream);
  g_ScaledDifferenceFromFarFogIntensity = read_f32(stream);
  zrange[0] = read_f32(stream);
  zrange[1] = read_f32(stream);
  environment_id = read_u32(stream);
  had_environment = read_u8(stream);
  had_near_fog = read_u8(stream);

  environment = NULL;
  if (environment_id != 0) {
    for (environment = fog_tables; environment->Id != 0; environment++) {
      if (environment->Id == environment_id) {
        break;
      }
    }
    if (environment->Id == 0) {
      practiceLogWarn("Could not restore fog environment %d", environment_id);
      environment = NULL;
    }
  }

  g_EnvironmentMainp = environment;
  g_EnvironmentAltp = environment != NULL ? environment + 1 : NULL;
  g_EnvironmentFoundp = had_environment ? environment : NULL;
  g_NearFogValuesP = NULL;
  if (had_near_fog) {
    if (environment == NULL) {
      practiceLogWarn("Fog state has near values without an environment");
    } else {
#if defined(VERSION_EU)
      eu_loadCurrentNearFog.NearFog = environment->Visibility.Nfd.NearFog;
      eu_loadCurrentNearFog.MaxVisRange =
          environment->Visibility.Nfd.MaxVisRange;
      eu_loadCurrentNearFog.MaxObfuscationRange =
          environment->Visibility.Nfd.MaxObfuscationRange;
      g_NearFogValuesP = (NearFogRecord *)&eu_loadCurrentNearFog;
#else
      g_NearFogValuesP = &environment->Visibility.Nfd;
#endif
    }
  }
  viSetZRange(zrange[0], zrange[1]);
}

static void save_level_timer_state(StateStream *stream) {
  write_u32(stream, clock_drawn_flag);
  write_u32(stream, clock_enable);
  write_f32(stream, clock_time);
}

static void load_level_timer_state(StateStream *stream) {
  clock_drawn_flag = read_u32(stream);
  clock_enable = read_u32(stream);
  clock_time = read_f32(stream);
}

static void save_toxic_gas_state(StateStream *stream) {
  write_f32(stream, toxic_gas_sound_timer);
  write_u32(stream, activate_gas_sound_timer);
  write_bytes(stream, &D_80030AD0, sizeof(D_80030AD0));
  write_u32(stream, D_80030ADC);
  write_f32(stream, D_80030AE0);
  write_f32(stream, gasTimeToFullOpacity);
  write_u32(stream, gasDoesDamageFlag);
}

static void load_toxic_gas_state(StateStream *stream) {
  toxic_gas_sound_timer = read_f32(stream);
  activate_gas_sound_timer = read_u32(stream);
  read_bytes(stream, &D_80030AD0, sizeof(D_80030AD0));
  D_80030ADC = read_u32(stream);
  D_80030AE0 = read_f32(stream);
  gasTimeToFullOpacity = read_f32(stream);
  gasDoesDamageFlag = read_u32(stream);
  // SFX handles are dynamically allocated and all SFX are stopped before
  // loading. Let the gas update create a fresh handle when it next runs.
  ptr_gas_sound = NULL;
}

static s32 count_room_objective_criteria(void) {
  struct criteria_roomentered *criteria;
  s32 count = 0;

  for (criteria = ptr_last_enter_room_subobject_entry_type20; criteria != NULL;
       criteria = criteria->next) {
    count++;
  }

  return count;
}

static s32 count_deposit_objective_criteria(void) {
  struct criteria_deposit *criteria;
  s32 count = 0;

  for (criteria = ptr_last_deposit_in_room_subobject_entry_type21;
       criteria != NULL; criteria = criteria->next) {
    count++;
  }

  return count;
}

static s32 count_picture_objective_criteria(void) {
  struct criteria_picture *criteria;
  s32 count = 0;

  for (criteria = ptr_last_photo_obj_in_room_subobject_entry_type1E;
       criteria != NULL; criteria = criteria->next) {
    count++;
  }

  return count;
}

static void save_objective_state(StateStream *stream) {
  struct criteria_roomentered *room_criteria;
  struct criteria_deposit *deposit_criteria;
  struct criteria_picture *picture_criteria;
  s32 i;

  for (i = 0; i < OBJECTIVES_MAX; i++) {
    write_u32(stream, objectiveStatuses[i]);
  }

  write_u32(stream, count_room_objective_criteria());
  for (room_criteria = ptr_last_enter_room_subobject_entry_type20;
       room_criteria != NULL; room_criteria = room_criteria->next) {
    write_u32(stream, room_criteria->status);
  }

  write_u32(stream, count_deposit_objective_criteria());
  for (deposit_criteria = ptr_last_deposit_in_room_subobject_entry_type21;
       deposit_criteria != NULL; deposit_criteria = deposit_criteria->next) {
    write_u32(stream, deposit_criteria->flag);
  }

  write_u32(stream, count_picture_objective_criteria());
  for (picture_criteria = ptr_last_photo_obj_in_room_subobject_entry_type1E;
       picture_criteria != NULL; picture_criteria = picture_criteria->next) {
    write_u32(stream, picture_criteria->flag);
  }

  for (i = 0; i < MAX_PLAYER_COUNT; i++) {
    write_u32(stream, g_playerPlayerData[i].killed_civilians);
  }
}

static void load_objective_state(StateStream *stream) {
  struct criteria_roomentered *room_criteria;
  struct criteria_deposit *deposit_criteria;
  struct criteria_picture *picture_criteria;
  s32 saved_count;
  s32 current_count;
  s32 i;

  for (i = 0; i < OBJECTIVES_MAX; i++) {
    objectiveStatuses[i] = read_u32(stream);
  }

  saved_count = read_u32(stream);
  current_count = count_room_objective_criteria();
  room_criteria = ptr_last_enter_room_subobject_entry_type20;
  for (i = 0; i < saved_count; i++) {
    u32 status = read_u32(stream);
    if (room_criteria != NULL) {
      room_criteria->status = status;
      room_criteria = room_criteria->next;
    }
  }
  while (room_criteria != NULL) {
    room_criteria->status = 0;
    room_criteria = room_criteria->next;
  }
  if (saved_count != current_count) {
    practiceLogWarn("Room objective criteria count changed (%d saved, %d live)",
                    saved_count, current_count);
  }

  saved_count = read_u32(stream);
  current_count = count_deposit_objective_criteria();
  deposit_criteria = ptr_last_deposit_in_room_subobject_entry_type21;
  for (i = 0; i < saved_count; i++) {
    u32 flag = read_u32(stream);
    if (deposit_criteria != NULL) {
      deposit_criteria->flag = flag;
      deposit_criteria = deposit_criteria->next;
    }
  }
  while (deposit_criteria != NULL) {
    deposit_criteria->flag = 0;
    deposit_criteria = deposit_criteria->next;
  }
  if (saved_count != current_count) {
    practiceLogWarn(
        "Deposit objective criteria count changed (%d saved, %d live)",
        saved_count, current_count);
  }

  saved_count = read_u32(stream);
  current_count = count_picture_objective_criteria();
  picture_criteria = ptr_last_photo_obj_in_room_subobject_entry_type1E;
  for (i = 0; i < saved_count; i++) {
    u32 flag = read_u32(stream);
    if (picture_criteria != NULL) {
      picture_criteria->flag = flag;
      picture_criteria = picture_criteria->next;
    }
  }
  while (picture_criteria != NULL) {
    picture_criteria->flag = 0;
    picture_criteria = picture_criteria->next;
  }
  if (saved_count != current_count) {
    practiceLogWarn(
        "Picture objective criteria count changed (%d saved, %d live)",
        saved_count, current_count);
  }

  for (i = 0; i < MAX_PLAYER_COUNT; i++) {
    g_playerPlayerData[i].killed_civilians = read_u32(stream);
  }
}

#if defined(VERSION_JP) || defined(VERSION_EU)
extern s32 dword_CODE_bss_jp80079CEC[0x05];
extern s32 dword_CODE_bss_jp80079Cd8[0x05];
#else
extern s32 copy_1stfonttable;
extern s32 copy_2ndfonttable;
#endif

void save_global_state(StateStream *stream) {
  // HUD
  write_u32(stream, status_bar_text_buffer_index);
  write_u32(stream, display_statusbar);
  write_bytes(stream, stringbuffer_lowerleft, sizeof(stringbuffer_lowerleft));
  write_u32(stream, upper_text_buffer_index);
  write_u32(stream, display_upper_text_window);
  write_u32(stream, upper_text_window_timer);
  write_u32(stream, g_UpperTextDisplayFlag);
  practice_dialog_save(stream);

  // Font
#if defined(VERSION_JP) || defined(VERSION_EU)
  write_bytes(stream, dword_CODE_bss_jp80079CEC,
              sizeof(dword_CODE_bss_jp80079CEC));
  write_bytes(stream, dword_CODE_bss_jp80079Cd8,
              sizeof(dword_CODE_bss_jp80079Cd8));
#else
  write_u32(stream, copy_1stfonttable);
  write_u32(stream, copy_2ndfonttable);
#endif

  // Tank
  write_u32(stream, in_tank_flag);
  write_u32(stream, get_prop_index(g_PlayerTankProp));
  write_u32(stream, get_prop_index(g_WorldTankProp));
  write_f32(stream, g_PlayerTankYOffset);
  write_f32(stream, g_TankTurnSpeed);
  write_f32(stream, g_TankOrientationAngle);
  write_f32(stream, tank_turret_unused_angle);
  write_f32(stream, g_TankTurretVerticalAngle);
  write_f32(stream, g_TankTurretVerticalAngleRelated);
  write_f32(stream, g_TankTurretOrientationAngleRad);
  write_f32(stream, g_TankTurretOrientationAngleDeg);
  write_f32(stream, tank_turret_turn_speed);
  write_u32(stream, g_BondCanEnterTank);
  write_f32(stream, g_TankTurretAngle);
  write_f32(stream, g_TankTurretTurn);
  write_u32(stream, g_ExplodeTankOnDeathFlag);
  write_u32(stream, g_TankDamagePenaltyTicks);
  write_u32(stream, g_EnterTankAudioState);
  write_bytes(stream, &g_TankModelPositionOffset,
              sizeof(g_TankModelPositionOffset));
  write_u32(stream, g_TankEngineSfxVolume);
  write_f32(stream, g_TankEnteringSitHeight);
  write_f32(stream, g_TankEnteringSitHeightRemain);
  write_f32(stream, g_TankEnterBondHorizAngleDeg);
  write_f32(stream, g_TankEnterBondVertAngleDeg);
  write_bytes(stream, &g_EnterTankCoord, sizeof(g_EnterTankCoord));

  // Camera
  write_u32(stream, g_CameraMode);
  write_u32(stream, camera_mode);
  write_u32(stream, g_CameraAfterCinema);
  write_f32(stream, camera_transition_timer);
  write_u32(stream, camera_fade_active);
  write_u32(stream, stop_time_flag);
  write_u32(stream, intro_camera_index);
  write_u32(stream, is_timer_active);
  write_u32(stream, g_PlayerInvincible);
  write_u32(stream, g_bondviewForceDisarm);
  write_bytes(stream, &flt_CODE_bss_800799E8,
              sizeof(flt_CODE_bss_800799E8));
  write_u32(stream, get_prop_index(dword_CODE_bss_800799F4));
  write_u32(stream, get_camera_pad_id(dword_CODE_bss_800799F8));
  write_u32(stream, gBondViewCutscene != NULL
                        ? tagGetCommandIndex((ObjectRecord *)gBondViewCutscene)
                        : -1);
  write_f32(stream, flt_CODE_bss_80079A00);
  write_f32(stream, flt_CODE_bss_80079A04);
  write_f32(stream, flt_CODE_bss_80079A08);
  write_f32(stream, flt_CODE_bss_80079A0C);
  write_f32(stream, flt_CODE_bss_80079A10);
  write_u32(stream, dword_CODE_bss_80079A14);
  write_u32(stream, dword_CODE_bss_80079A18);
  write_u32(stream, dword_CODE_bss_80079A1C);
  write_f32(stream, watch_transition_time);
  write_u32(stream, g_ViShakeIntensity);
  write_u32(stream, g_ViShakeTimer);

  // Level activity
  write_u32(stream, sub_GAME_7F0BD8F0());
  write_u32(stream, lvlGetControlsLockedFlag());

  // Values
  write_u32(stream, g_GlobalTimer);
  write_u32(stream, g_ClockTimer);
  write_f32(stream, g_GlobalTimerDelta);
#if defined(BUGFIX_R1)
  write_f32(stream, g_JP_GlobalTimerDelta);
#endif
  write_u32(stream, D_80048380);
  write_u32(stream, mission_timer);
#if defined(VERSION_JP) || defined(VERSION_EU)
  write_f32(stream, watch_time_0);
#else
  write_u32(stream, watch_time_0);
#endif
  write_u32(stream, D_80040990);
  write_u32(stream, watch_screen_index);
  write_u32(stream, controller_options_index);
  write_u32(stream, game_options_index);
  write_u32(stream, mission_brief_index);
  write_u32(stream, D_800409A4);
  write_u32(stream, watch_item_is_actively_selected);
  write_u32(stream, D_800409AC);
  write_u32(stream, D_800409B0);
  write_u32(stream, D_800409B4);
  write_u32(stream, g_curWatchItemIndex);
  write_f32(stream, D_800409BC);
  write_u32(stream, D_800409C0);
  write_u32(stream, D_800409C4);
  write_f32(stream, D_800409C8);
  write_f32(stream, D_800409CC);
  write_u32(stream, D_800409D0);
  write_f32(stream, D_800409D4);
  write_u32(stream, D_800409D8);
  write_u32(stream, controlstick_lr_enabled);
  write_u32(stream, D_80040AE0);
  write_u32(stream, D_80040AE4);
  write_f32(stream, D_80040AE8);
  write_f32(stream, D_80040AEC);
  write_f32(stream, D_80040AF0);
  write_u32(stream, D_80040AF4);
  write_u32(stream, D_80040AF8);
  write_u32(stream, D_80040AFC);
  write_u32(stream, D_80040B00);
  write_u32(stream, g_WatchBackgroundGreen);
  write_u32(stream, D_80040B08);
  write_u32(stream, D_80040B0C);
  write_u32(stream, D_80040B10);
  write_f32(stream, D_80040B14);
  write_f32(stream, D_80040B18);
  write_f32(stream, D_80040B1C);
  write_f32(stream, D_80040B20);
  write_f32(stream, D_80040B24);
  write_f32(stream, D_80040B28);
  write_u32(stream, D_80040B2C);
  write_f32(stream, D_80040B30);
  write_f32(stream, D_80040B34);
  write_f32(stream, D_80040B38);
  write_u32(stream, D_80040B3C);
  write_u32(stream, D_80040B40);
  write_u16(stream, D_80040B44);
  write_u32(stream, D_80040B48);
  write_u32(stream, D_80040B4C);
  write_u32(stream, D_80040B50);
  write_u32(stream, D_80040B54);
  write_bytes(stream, &g_randomSeed, sizeof(g_randomSeed));
  write_bytes(stream, &g_chrObjRandomSeed, sizeof(g_chrObjRandomSeed));
  write_u32(stream, player1_guardID);
  write_u32(stream, g_ForcedDeltaFrames);
  write_u32(stream, get_cur_playernum());
  write_u32(stream, g_BgCurrentRoom);
  write_u32(stream, num_visible_rooms_in_cur_global_vis_packet);
  write_bytes(stream, list_visible_rooms_in_cur_global_vis_packet,
              sizeof(list_visible_rooms_in_cur_global_vis_packet));
  write_bytes(stream, roomStatusFlags, sizeof(roomStatusFlags));
  write_bytes(stream, roomIndices, sizeof(roomIndices));
  write_bytes(stream, roomOwners, sizeof(roomOwners));
  {
    u16 matrix_count = 0;
    s32 matrix;

    for (matrix = 0; matrix < ROOM_TRANSFORM_CACHE_LENGTH; matrix++) {
      if (roomIndices[matrix] >= 0) {
        matrix_count++;
      }
    }
    write_u16(stream, matrix_count);
    for (matrix = 0; matrix < ROOM_TRANSFORM_CACHE_LENGTH; matrix++) {
      if (roomIndices[matrix] >= 0) {
        write_u16(stream, matrix);
        write_bytes(stream, &roomMatrices[matrix], sizeof(Mtx));
      }
    }
  }

  // Alarm
  write_u32(stream, alarm_timer);
  write_u32(stream, objectiveregisters1);

  // Level timers
  save_level_timer_state(stream);

  // Toxic gas
  save_toxic_gas_state(stream);

  // Background AI
  save_background_ai_state(stream);

  // Sky
  save_sky_state(stream);

  // Objectives
  save_objective_state(stream);

  // Music
  save_music_state(stream);

  // Practice lag impact accumulator
  practice_lag_save(stream);
}

void load_global_state_pre_props(StateStream *stream) {
  // HUD
  status_bar_text_buffer_index = read_u32(stream);
  display_statusbar = read_u32(stream);
  read_bytes(stream, stringbuffer_lowerleft, sizeof(stringbuffer_lowerleft));
  upper_text_buffer_index = read_u32(stream);
  display_upper_text_window = read_u32(stream);
  upper_text_window_timer = read_u32(stream);
  g_UpperTextDisplayFlag = read_u32(stream);
  practice_dialog_load(stream);

  // Font
#if defined(VERSION_JP) || defined(VERSION_EU)
  read_bytes(stream, dword_CODE_bss_jp80079CEC,
             sizeof(dword_CODE_bss_jp80079CEC));
  read_bytes(stream, dword_CODE_bss_jp80079Cd8,
             sizeof(dword_CODE_bss_jp80079Cd8));
#else
  copy_1stfonttable = read_u32(stream);
  copy_2ndfonttable = read_u32(stream);
#endif

  // Tank
  in_tank_flag = read_u32(stream);
  saved_player_tank_prop_index = read_u32(stream);
  saved_world_tank_prop_index = read_u32(stream);
  g_PlayerTankYOffset = read_f32(stream);
  g_TankTurnSpeed = read_f32(stream);
  g_TankOrientationAngle = read_f32(stream);
  tank_turret_unused_angle = read_f32(stream);
  g_TankTurretVerticalAngle = read_f32(stream);
  g_TankTurretVerticalAngleRelated = read_f32(stream);
  g_TankTurretOrientationAngleRad = read_f32(stream);
  g_TankTurretOrientationAngleDeg = read_f32(stream);
  tank_turret_turn_speed = read_f32(stream);
  g_BondCanEnterTank = read_u32(stream);
  g_TankTurretAngle = read_f32(stream);
  g_TankTurretTurn = read_f32(stream);
  g_ExplodeTankOnDeathFlag = read_u32(stream);
  g_TankDamagePenaltyTicks = read_u32(stream);
  g_EnterTankAudioState = read_u32(stream);
  read_bytes(stream, &g_TankModelPositionOffset,
             sizeof(g_TankModelPositionOffset));
  g_TankEngineSfxVolume = read_u32(stream);
  g_TankEnteringSitHeight = read_f32(stream);
  g_TankEnteringSitHeightRemain = read_f32(stream);
  g_TankEnterBondHorizAngleDeg = read_f32(stream);
  g_TankEnterBondVertAngleDeg = read_f32(stream);
  read_bytes(stream, &g_EnterTankCoord, sizeof(g_EnterTankCoord));

  // Camera
  g_CameraMode = read_u32(stream);
  camera_mode = read_u32(stream);
  g_CameraAfterCinema = read_u32(stream);
  camera_transition_timer = read_f32(stream);
  camera_fade_active = read_u32(stream);
  stop_time_flag = read_u32(stream);
  intro_camera_index = read_u32(stream);
  is_timer_active = read_u32(stream);
  g_PlayerInvincible = read_u32(stream);
  g_bondviewForceDisarm = read_u32(stream);
  read_bytes(stream, &flt_CODE_bss_800799E8,
             sizeof(flt_CODE_bss_800799E8));
  saved_death_camera_prop_index = read_u32(stream);
  dword_CODE_bss_800799F8 = get_camera_pad_by_id(read_u32(stream));
  {
    s32 command_index = read_u32(stream);
    gBondViewCutscene = command_index >= 0
                           ? (CutsceneRecord *)setupGetPtrToCommandByIndex(
                                 command_index)
                           : NULL;
    if (gBondViewCutscene != NULL &&
        gBondViewCutscene->type != PROPDEF_CAMERAPOS) {
      practiceLogWarn("Invalid frozen camera command %d (type=%d)",
                      command_index, gBondViewCutscene->type);
      gBondViewCutscene = NULL;
    }
  }
  flt_CODE_bss_80079A00 = read_f32(stream);
  flt_CODE_bss_80079A04 = read_f32(stream);
  flt_CODE_bss_80079A08 = read_f32(stream);
  flt_CODE_bss_80079A0C = read_f32(stream);
  flt_CODE_bss_80079A10 = read_f32(stream);
  dword_CODE_bss_80079A14 = read_u32(stream);
  dword_CODE_bss_80079A18 = read_u32(stream);
  dword_CODE_bss_80079A1C = read_u32(stream);
  watch_transition_time = read_f32(stream);
  g_ViShakeIntensity = read_u32(stream);
  g_ViShakeTimer = read_u32(stream);

  // Level activity
  sub_GAME_7F0BD8FC(read_u32(stream));
  lvlSetControlsLockedFlag(read_u32(stream));

  // Values
  g_GlobalTimer = read_u32(stream);
  g_ClockTimer = read_u32(stream);
  g_GlobalTimerDelta = read_f32(stream);
#if defined(BUGFIX_R1)
  g_JP_GlobalTimerDelta = read_f32(stream);
#endif
  D_80048380 = read_u32(stream);
  mission_timer = read_u32(stream);
#if defined(VERSION_JP) || defined(VERSION_EU)
  watch_time_0 = read_f32(stream);
#else
  watch_time_0 = read_u32(stream);
#endif
  D_80040990 = read_u32(stream);
  watch_screen_index = read_u32(stream);
  controller_options_index = read_u32(stream);
  game_options_index = read_u32(stream);
  mission_brief_index = read_u32(stream);
  D_800409A4 = read_u32(stream);
  watch_item_is_actively_selected = read_u32(stream);
  D_800409AC = read_u32(stream);
  D_800409B0 = read_u32(stream);
  D_800409B4 = read_u32(stream);
  g_curWatchItemIndex = read_u32(stream);
  D_800409BC = read_f32(stream);
  D_800409C0 = read_u32(stream);
  D_800409C4 = read_u32(stream);
  D_800409C8 = read_f32(stream);
  D_800409CC = read_f32(stream);
  D_800409D0 = read_u32(stream);
  D_800409D4 = read_f32(stream);
  D_800409D8 = read_u32(stream);
  controlstick_lr_enabled = read_u32(stream);
  D_80040AE0 = read_u32(stream);
  D_80040AE4 = read_u32(stream);
  D_80040AE8 = read_f32(stream);
  D_80040AEC = read_f32(stream);
  D_80040AF0 = read_f32(stream);
  D_80040AF4 = read_u32(stream);
  D_80040AF8 = read_u32(stream);
  D_80040AFC = read_u32(stream);
  D_80040B00 = read_u32(stream);
  g_WatchBackgroundGreen = read_u32(stream);
  D_80040B08 = read_u32(stream);
  D_80040B0C = read_u32(stream);
  D_80040B10 = read_u32(stream);
  D_80040B14 = read_f32(stream);
  D_80040B18 = read_f32(stream);
  D_80040B1C = read_f32(stream);
  D_80040B20 = read_f32(stream);
  D_80040B24 = read_f32(stream);
  D_80040B28 = read_f32(stream);
  D_80040B2C = read_u32(stream);
  D_80040B30 = read_f32(stream);
  D_80040B34 = read_f32(stream);
  D_80040B38 = read_f32(stream);
  D_80040B3C = read_u32(stream);
  D_80040B40 = read_u32(stream);
  D_80040B44 = read_u16(stream);
  D_80040B48 = read_u32(stream);
  D_80040B4C = read_u32(stream);
  D_80040B50 = read_u32(stream);
  D_80040B54 = read_u32(stream);
  read_bytes(stream, &saved_random_seed, sizeof(saved_random_seed));
  read_bytes(stream, &saved_chr_obj_random_seed,
             sizeof(saved_chr_obj_random_seed));
  saved_player1_guard_id = read_u32(stream);
  g_ForcedDeltaFrames = read_u32(stream);
  saved_current_player_index = read_u32(stream);
  g_BgCurrentRoom = read_u32(stream);
  num_visible_rooms_in_cur_global_vis_packet = read_u32(stream);
  read_bytes(stream, list_visible_rooms_in_cur_global_vis_packet,
             sizeof(list_visible_rooms_in_cur_global_vis_packet));
  read_bytes(stream, roomStatusFlags, sizeof(roomStatusFlags));
  read_bytes(stream, roomIndices, sizeof(roomIndices));
  read_bytes(stream, roomOwners, sizeof(roomOwners));
  {
    s32 matrix;
    s32 room;

    /* roomIndices and s_room_info::field_36 are the two directions of the
     * room-transform lookup. Only the compact index array needs to be stored;
     * rebuild the reverse mapping before the renderer can reuse a slot. */
    for (room = 0; room < g_MaxNumRooms; room++) {
      g_BgRoomInfo[room].field_36 = -1;
    }
    for (matrix = 0; matrix < ROOM_TRANSFORM_CACHE_LENGTH; matrix++) {
      room = roomIndices[matrix];
      if (room < 0) {
        continue;
      }
      if (room >= g_MaxNumRooms || g_BgRoomInfo[room].field_36 != -1) {
        practiceLogError("Saved room transform mapping is invalid (%d -> %d)",
                         matrix, room);
        assert(FALSE);
        roomIndices[matrix] = -1;
        roomOwners[matrix] = -1;
        roomStatusFlags[matrix] = 2;
        continue;
      }
      g_BgRoomInfo[room].field_36 = matrix;
    }
  }
  {
    u16 matrix_count = read_u16(stream);
    s32 matrix;

    if (matrix_count > ROOM_TRANSFORM_CACHE_LENGTH) {
      practiceLogError("Saved room transform count is invalid (%d)",
                       matrix_count);
      assert(FALSE);
    }
    for (matrix = 0; matrix < matrix_count; matrix++) {
      u16 index = read_u16(stream);
      if (index >= ROOM_TRANSFORM_CACHE_LENGTH) {
        practiceLogError("Saved room transform index is invalid (%d)", index);
        assert(FALSE);
        return;
      }
      read_bytes(stream, &roomMatrices[index], sizeof(Mtx));
    }
  }
  if (num_visible_rooms_in_cur_global_vis_packet < 0 ||
      num_visible_rooms_in_cur_global_vis_packet >
          BG_VISIBLE_ROOM_LIST_LENGTH) {
    practiceLogError("Saved visible room count is invalid (%d)",
                     num_visible_rooms_in_cur_global_vis_packet);
    assert(FALSE);
  }

  // Alarm
  alarm_timer = read_u32(stream);
  objectiveregisters1 = read_u32(stream);
  // Sound states are dynamically allocated and all SFX are stopped before
  // loading. Let the alarm update create a fresh handle when it next runs.
  ptr_alarm_sfx = NULL;

  // Level timers
  load_level_timer_state(stream);

  // Toxic gas
  load_toxic_gas_state(stream);

  // Background AI
  load_background_ai_state(stream);

  // Sky
  load_sky_state(stream);

  // Objectives
  load_objective_state(stream);

  // Music
  load_music_state(stream);

  // Practice lag impact accumulator
  practice_lag_load(stream);

  // TODO: We should save the RNG state needed for restoring each prop
  // individually but for now just use the final RNG state when restoring props
  g_randomSeed = saved_random_seed;
  g_chrObjRandomSeed = saved_chr_obj_random_seed;
}

bool load_global_state_post_props(void) {
  if (saved_current_player_index < 0 ||
      saved_current_player_index >= getPlayerCount() ||
      g_playerPointers[saved_current_player_index] == NULL) {
    practiceLogWarn("Invalid saved current player index %d",
                    saved_current_player_index);
    return FALSE;
  }

  g_PlayerTankProp = get_enabled_prop_by_index(saved_player_tank_prop_index);
  g_WorldTankProp = get_enabled_prop_by_index(saved_world_tank_prop_index);
  dword_CODE_bss_800799F4 =
      get_enabled_prop_by_index(saved_death_camera_prop_index);

  set_cur_player(saved_current_player_index);

  // Prop and player restoration can consume randomness. Apply the saved RNG
  // state last so the next gameplay tick resumes the saved sequence.
  g_randomSeed = saved_random_seed;
  g_chrObjRandomSeed = saved_chr_obj_random_seed;
  player1_guardID = saved_player1_guard_id;

  return TRUE;
}

void restore_global_rng_after_load(void) {
  g_randomSeed = saved_random_seed;
  g_chrObjRandomSeed = saved_chr_obj_random_seed;
}
