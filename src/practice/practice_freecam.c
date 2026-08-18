#include "practice_freecam.h"
#include "bg.h"
#include "bondconstants.h"
#include "bondview.h"
#include "chr.h"
#include "chrlv.h"
#include "chrobjhandler.h"
#include "joy.h"
#include "math_atan2f.h"
#include "objecthandler.h"
#include "player.h"
#include "player_2.h"
#include "practice_external_camera.h"
#include "practice_render.h"
#include "stan.h"
#include <math.h>
#include <os_extension.h>

#define FREECAM_STICK_DEADZONE 3
#define FREECAM_MOVE_SPEED 1000.0f
#define FREECAM_CPU_COUNTS_PER_SECOND 46875000.0f
#define FREECAM_TURN_SPEED 0.0021816616f
#define FREECAM_MAX_PITCH 1.55334306f
#define FREECAM_BOND_REVEAL_DISTANCE 50.0f
#define FREECAM_ROOM_BITMAP_SIZE ((MAXROOMCOUNT + 7) / 8)

static s32 g_FreecamActive;
static s32 g_FreecamSwallowInput;
static s32 g_FreecamController;
static s32 g_FreecamPlayer;
static s32 g_FreecamRestorePlayerCamera;
static u8 g_FreecamRoom;
static StandTile *g_FreecamTile;
static coord3d g_FreecamTilePosition;
static coord3d g_FreecamPosition;
static coord3d g_FreecamStartPosition;
static coord3d g_FreecamForward;
static coord3d g_FreecamUp;
static f32 g_FreecamYaw;
static f32 g_FreecamPitch;
static u32 g_FreecamLastCount;
static u8 g_FreecamInitialRoomResidency[FREECAM_ROOM_BITMAP_SIZE];
static s32 g_FreecamPinnedCameraActive;
static struct PracticeExternalCameraView g_FreecamPinnedCamera;
static PropRecord g_FreecamBondProp;
static ChrRecord g_FreecamBondChr;
static coord3d g_FreecamBondModelPosition;
static f32 g_FreecamBondHeading;
static Model *g_FreecamBondDeferredModel;

extern void sub_GAME_7F0876C4(coord3d *cam_pos, coord3d *cam_look,
                              coord3d *cam_up);
extern void sub_GAME_7F0B6368(s32 room);
extern void clear_aircraft_model_obj(Model *model);

static void destroy_freecam_bond(void) {
  Model *model = g_FreecamBondChr.model;

  if (g_FreecamBondProp.chr == &g_FreecamBondChr &&
      g_FreecamBondChr.prop == &g_FreecamBondProp) {
    /* The display list may still reference this model's matrices when the
     * hotkey exits freecam. Forget the render wrapper now, but keep the model
     * instance reserved until the graphics task has completed. */
    g_FreecamBondChr.field_20 = NULL;
    if (model != NULL)
      g_FreecamBondDeferredModel = model;
  }

  bzero(&g_FreecamBondChr, sizeof(g_FreecamBondChr));
  bzero(&g_FreecamBondProp, sizeof(g_FreecamBondProp));
}

static s32 player_has_third_person_bond(void) {
  return g_CurrentPlayer != NULL && g_CurrentPlayer->prop != NULL &&
         g_CurrentPlayer->prop->chr != NULL &&
         g_CurrentPlayer->prop->chr->model != NULL &&
         g_CurrentPlayer->prop->chr != &g_FreecamBondChr;
}

static void apply_freecam_bond_animation(Model *model) {
  Model *player_model;
  ModelRwData_HeaderRecord *root_data;

  player_model = (Model *)&g_CurrentPlayer->model;
  if (objecthandlerGetModelAnim(player_model) == NULL)
    return;

  /* The first modelSetAnimation stores hip height in unk34.y. Later calls
   * rewrite that as pos.y - ground and will lift the model if they run
   * every live frame. */
  modelSetAnimation(model, objecthandlerGetModelAnim(player_model),
                    objecthandlerGetModelGunhand(player_model),
                    objecthandlerGetModelField28(player_model), 0.5f, 0.0f);

  root_data = (ModelRwData_HeaderRecord *)modelGetNodeRwData(
      model, model->obj->RootNode);
  root_data->ground = g_CurrentPlayer->field_70;
}

static void sync_freecam_bond(void) {
  Model *model;
  ModelRwData_HeaderRecord *root_data;

  if (g_CurrentPlayer == NULL || g_CurrentPlayer->prop == NULL ||
      g_FreecamBondProp.chr != &g_FreecamBondChr)
    return;

  g_FreecamBondProp.pos = g_CurrentPlayer->prop->pos;
  g_FreecamBondProp.pos.y = g_CurrentPlayer->field_70;
  g_FreecamBondProp.stan = g_CurrentPlayer->prop->stan;
  g_FreecamBondModelPosition = g_FreecamBondProp.pos;
  g_FreecamBondProp.rooms[0] = g_CurrentPlayer->prop->stan != NULL
                                   ? g_CurrentPlayer->prop->stan->room
                                   : 0xff;
  g_FreecamBondProp.rooms[1] = 0xff;
  g_FreecamBondHeading = get_curplay_horizontal_rotation_in_degrees();
  g_FreecamBondChr.ground = g_CurrentPlayer->field_70;
  g_FreecamBondChr.prevpos = g_FreecamBondModelPosition;
  set_color_shading_from_tile(&g_FreecamBondProp, &g_FreecamBondChr.nextcol);
  g_FreecamBondChr.shadecol = g_FreecamBondChr.nextcol;

  model = g_FreecamBondChr.model;
  if (model == NULL)
    return;

  setsuboffset(model, &g_FreecamBondModelPosition);
  setsubroty(model, g_FreecamBondHeading);
  root_data = (ModelRwData_HeaderRecord *)modelGetNodeRwData(
      model, model->obj->RootNode);
  root_data->ground = g_CurrentPlayer->field_70;
}

static void create_freecam_bond(void) {
  Model *model;

  if (g_CurrentPlayer == NULL || g_CurrentPlayer->prop == NULL)
    return;

  if (g_FreecamBondProp.chr == &g_FreecamBondChr &&
      g_FreecamBondChr.model != NULL) {
    sync_freecam_bond();
    return;
  }

  /* Intros, outros, death animations and other third-person cameras attach the
   * real Bond model to the player prop. A second body would overlap it. */
  if (player_has_third_person_bond())
    return;

  /* A previous PIP/freecam exit may still be holding the instance until the
   * last display list completes. Reuse it instead of failing the next create. */
  if (g_FreecamBondDeferredModel != NULL) {
    model = g_FreecamBondDeferredModel;
    g_FreecamBondDeferredModel = NULL;
  } else {
    model = retrieve_header_for_body_and_head(BODY_Brosnan_Tuxedo,
                                              HEAD_Male_Brosnan_Tuxedo, 0);
    if (model == NULL)
      return;
  }

  bzero(&g_FreecamBondProp, sizeof(g_FreecamBondProp));
  bzero(&g_FreecamBondChr, sizeof(g_FreecamBondChr));
  g_FreecamBondProp.type = PROP_TYPE_CHR;
  g_FreecamBondProp.chr = &g_FreecamBondChr;

  /* This is only the subset of character state consumed by rendering. It is
   * intentionally not created with init_GUARDdata_with_set_values: that
   * initializer performs collision placement and allocates a gameplay CHR. */
  g_FreecamBondChr.chrnum = -1;
  g_FreecamBondChr.headnum = HEAD_Male_Brosnan_Tuxedo;
  g_FreecamBondChr.bodynum = BODY_Brosnan_Tuxedo;
  g_FreecamBondChr.actiontype = ACT_STAND;
  g_FreecamBondChr.fadealpha = 0xff;
  g_FreecamBondChr.flinchcnt = -1;
  g_FreecamBondChr.chrflags = CHRFLAG_04000000;
  g_FreecamBondChr.prop = &g_FreecamBondProp;
  g_FreecamBondChr.model = model;
  g_FreecamBondChr.chrwidth = 20.0f;
  g_FreecamBondChr.chrheight = 185.0f;
  g_FreecamBondChr.unk180[0].unk00 = -1;
  g_FreecamBondChr.unk180[1].unk00 = -1;

  /* The normal CHR callback derives root translation from gameplay movement
   * state and rewrites this model back to the player's eye-height position.
   * The synthetic body already has an explicit floor-level origin. */
  sub_GAME_7F06FF5C(model, 0);
  model->unk00 = 0x0a;
  model->chr = &g_FreecamBondChr;
  modelSetAnimPlaySpeed(model, animation_rate, 0.0f);
  sync_freecam_bond();
  apply_freecam_bond_animation(model);
}

static s32 was_room_resident_on_enable(s32 room) {
  return g_FreecamInitialRoomResidency[room >> 3] & (1 << (room & 7));
}

static void capture_initial_room_residency(void) {
  s32 room;

  bzero(g_FreecamInitialRoomResidency, sizeof(g_FreecamInitialRoomResidency));
  for (room = 1; room < g_MaxNumRooms && room < MAXROOMCOUNT; room++) {
    s_room_info *info = &g_BgRoomInfo[room];

    if (info->model_bin_loaded != 0 || info->ptr_point_index != NULL ||
        info->ptr_expanded_mapping_info != NULL) {
      g_FreecamInitialRoomResidency[room >> 3] |= 1 << (room & 7);
    }
  }
}

static void restore_initial_room_residency(void) {
  s32 room;

  /* Free the temporary freecam set first so the original set has the same
   * amount of room-heap space available that it had when freecam started. */
  for (room = 1; room < g_MaxNumRooms && room < MAXROOMCOUNT; room++) {
    s_room_info *info = &g_BgRoomInfo[room];
    s32 pinned_room =
        g_FreecamPinnedCameraActive && g_FreecamPinnedCamera.stan != NULL &&
        g_FreecamPinnedCamera.stan->room == room;

    if (!was_room_resident_on_enable(room) && !pinned_room &&
        (info->model_bin_loaded != 0 || info->ptr_point_index != NULL ||
         info->ptr_expanded_mapping_info != NULL)) {
      delete_room_data(room);
    }
  }

  for (room = 1; room < g_MaxNumRooms && room < MAXROOMCOUNT; room++) {
    s_room_info *info = &g_BgRoomInfo[room];

    if (!was_room_resident_on_enable(room) || info->model_bin_loaded != 0)
      continue;

    if (info->ptr_point_index != NULL ||
        info->ptr_expanded_mapping_info != NULL) {
      info->model_bin_loaded = 1;
    } else {
      sub_GAME_7F0B6368(room);
    }
  }
}

static void update_room(void) {
  StandTile *tile;
  f32 x = g_FreecamPosition.x;
  f32 y = g_FreecamPosition.y;
  f32 z = g_FreecamPosition.z;

  if (!stanFindNearestValidTilePointIncremental(&x, &y, &z, 0.0f, g_FreecamTile,
                                                g_FreecamTilePosition.x,
                                                g_FreecamTilePosition.z, &tile))
    return;

  if (tile != NULL) {
    g_FreecamTile = tile;
    g_FreecamTilePosition.x = x;
    g_FreecamTilePosition.y = y;
    g_FreecamTilePosition.z = z;
    if (tile->room > 0 && tile->room < g_MaxNumRooms &&
        tile->room < MAXROOMCOUNT && tile->room != g_FreecamRoom) {
      g_FreecamRoom = tile->room;
      practice_invalidate_render_state();
    }
  }
}

static s32 apply_deadzone(s32 value) {
  if (value < -FREECAM_STICK_DEADZONE)
    return value + FREECAM_STICK_DEADZONE;
  if (value > FREECAM_STICK_DEADZONE)
    return value - FREECAM_STICK_DEADZONE;
  return 0;
}

static void update_orientation(void) {
  f32 cos_pitch = cosf(g_FreecamPitch);
  f32 sin_pitch = sinf(g_FreecamPitch);
  f32 cos_yaw = cosf(g_FreecamYaw);
  f32 sin_yaw = sinf(g_FreecamYaw);

  g_FreecamForward.x = cos_pitch * sin_yaw;
  g_FreecamForward.y = sin_pitch;
  g_FreecamForward.z = -cos_pitch * cos_yaw;

  g_FreecamUp.x = -sin_pitch * sin_yaw;
  g_FreecamUp.y = cos_pitch;
  g_FreecamUp.z = sin_pitch * cos_yaw;
}

s32 practice_freecam_enable(s32 controller) {
  s32 previous_player;
  f32 forward_length_squared;
  f32 horizontal_length;

  if (g_CurrentPlayer == NULL || controller < 0)
    return FALSE;

  previous_player = get_cur_playernum();
  g_FreecamPlayer = controller < getPlayerCount() ? controller : 0;
  set_cur_player(g_FreecamPlayer);

  if (g_FreecamPinnedCameraActive) {
    /* Resume from the pinned PIP instead of Bond's current camera. Bond
     * reveal still uses his live position so a distant pin shows him
     * immediately. */
    g_FreecamPosition = g_FreecamPinnedCamera.position;
    g_FreecamForward = g_FreecamPinnedCamera.look;
    g_FreecamTile = g_FreecamPinnedCamera.stan;
    g_FreecamRoom = g_FreecamTile != NULL ? g_FreecamTile->room
                                          : bondviewGetCurrentPlayersRoom();
    if (g_CurrentPlayer->unknown == 1)
      g_FreecamStartPosition = g_CurrentPlayer->pos;
    else
      g_FreecamStartPosition = g_CurrentPlayer->field_488.pos;
  } else {
    if (g_CurrentPlayer->unknown == 1) {
      g_FreecamPosition = g_CurrentPlayer->pos;
      g_FreecamForward.x = g_CurrentPlayer->pos2.x - g_CurrentPlayer->pos.x;
      g_FreecamForward.y = g_CurrentPlayer->pos2.y - g_CurrentPlayer->pos.y;
      g_FreecamForward.z = g_CurrentPlayer->pos2.z - g_CurrentPlayer->pos.z;
    } else {
      g_FreecamPosition = g_CurrentPlayer->field_488.pos;
      g_FreecamForward = g_CurrentPlayer->field_488.applied_view;
    }
    g_FreecamRoom = bondviewGetCurrentPlayersRoom();
    g_FreecamTile = g_CurrentPlayer->field_488.current_tile_ptr;
    g_FreecamStartPosition = g_FreecamPosition;
  }
  g_FreecamTilePosition = g_FreecamPosition;
  stanFindNearestValidTilePointIncrementalReset();

  forward_length_squared = g_FreecamForward.x * g_FreecamForward.x +
                           g_FreecamForward.y * g_FreecamForward.y +
                           g_FreecamForward.z * g_FreecamForward.z;
  if (forward_length_squared < 0.000001f) {
    g_FreecamForward.x = 0.0f;
    g_FreecamForward.y = 0.0f;
    g_FreecamForward.z = -1.0f;
  }

  horizontal_length = sqrtf(g_FreecamForward.x * g_FreecamForward.x +
                            g_FreecamForward.z * g_FreecamForward.z);
  g_FreecamYaw = atan2f(g_FreecamForward.x, -g_FreecamForward.z);
  g_FreecamPitch = atan2f(g_FreecamForward.y, horizontal_length);
  if (g_FreecamPitch > M_PI_F)
    g_FreecamPitch -= M_TAU_F;
  update_orientation();

  capture_initial_room_residency();
  create_freecam_bond();
  g_FreecamController = controller;
  g_FreecamActive = TRUE;
  g_FreecamRestorePlayerCamera = FALSE;
  g_FreecamSwallowInput = FALSE;
  g_FreecamLastCount = osGetCount();
  joySetInputSuppressed(TRUE);
  practice_invalidate_render_state();
  set_cur_player(previous_player);
  return TRUE;
}

void practice_freecam_disable(void) {
  restore_initial_room_residency();
  g_FreecamActive = FALSE;
  g_FreecamRestorePlayerCamera = TRUE;
  g_FreecamSwallowInput = TRUE;
  stanFindNearestValidTilePointIncrementalReset();
  practice_invalidate_render_state();
}

void practice_freecam_reset(void) {
  /* Level setup has already rebuilt the character/model pools, so old-stage
   * pointers must only be forgotten here. Normal freecam exit performs the
   * explicit release above. */
  bzero(&g_FreecamBondProp, sizeof(g_FreecamBondProp));
  bzero(&g_FreecamBondChr, sizeof(g_FreecamBondChr));
  g_FreecamBondDeferredModel = NULL;
  g_FreecamActive = FALSE;
  g_FreecamSwallowInput = FALSE;
  g_FreecamController = 0;
  g_FreecamPlayer = 0;
  g_FreecamRestorePlayerCamera = FALSE;
  g_FreecamRoom = 0;
  g_FreecamTile = NULL;
  g_FreecamPinnedCameraActive = FALSE;
  g_FreecamLastCount = 0;
  stanFindNearestValidTilePointIncrementalReset();
  joySetInputSuppressed(FALSE);
}

s32 practice_freecam_is_active(void) { return g_FreecamActive; }

void practice_freecam_pin_camera(void) {
  if (!g_FreecamActive || g_FreecamTile == NULL)
    return;

  g_FreecamPinnedCamera.position = g_FreecamPosition;
  g_FreecamPinnedCamera.look = g_FreecamForward;
  g_FreecamPinnedCamera.up = g_FreecamUp;
  g_FreecamPinnedCamera.stan = g_FreecamTile;
  g_FreecamPinnedCamera.tracked_prop = NULL;
  g_FreecamPinnedCamera.forced_object = NULL;
  g_FreecamPinnedCamera.flags =
      PRACTICE_EXTERNAL_CAMERA_PRESERVE_GAMEPLAY_VISIBILITY;
  g_FreecamPinnedCamera.border_color = 0xff33ffff;
  g_FreecamPinnedCameraActive = TRUE;
  practice_invalidate_render_state();
}

void practice_freecam_clear_pinned_camera(void) {
  g_FreecamPinnedCameraActive = FALSE;
}

s32 practice_freecam_add_pinned_camera_view(void) {
  if (g_FreecamActive || !g_FreecamPinnedCameraActive)
    return FALSE;

  return practice_external_camera_add_view(&g_FreecamPinnedCamera);
}

static s32 bond_room_id(void) {
  if (g_FreecamBondProp.rooms[0] != 0xff)
    return g_FreecamBondProp.rooms[0];
  if (g_FreecamBondProp.stan != NULL)
    return g_FreecamBondProp.stan->room;
  return -1;
}

static u8 force_bond_room_visible(void) {
  s32 room = bond_room_id();
  u8 saved;

  if (room <= 0 || room >= g_MaxNumRooms || room >= MAXROOMCOUNT)
    return 0xff;

  saved = g_BgRoomInfo[room].room_rendered;
  g_BgRoomInfo[room].room_rendered = 1;
  return saved;
}

static void restore_bond_room_visible(u8 saved) {
  s32 room = bond_room_id();

  if (saved == 0xff || room <= 0 || room >= g_MaxNumRooms ||
      room >= MAXROOMCOUNT)
    return;

  g_BgRoomInfo[room].room_rendered = saved;
}

static Gfx *render_freecam_bond(Gfx *gdl, s32 force_room_visible) {
  ChrRecord *chr;
  Model *model;
  ModelRwData_HeaderRecord *root_data;
  u8 saved_room_rendered = 0xff;

  if (player_has_third_person_bond() || g_FreecamBondProp.chr == NULL)
    return gdl;

  chr = g_FreecamBondProp.chr;
  model = chr->model;
  if (model == NULL || chr->prop != &g_FreecamBondProp)
    return gdl;

  setsuboffset(model, &g_FreecamBondModelPosition);
  setsubroty(model, g_FreecamBondHeading);
  root_data = (ModelRwData_HeaderRecord *)modelGetNodeRwData(
      model, model->obj->RootNode);
  root_data->ground = g_FreecamBondChr.ground;
  subcalcpos(model);

  /* Grenade cam sits well above the floor, so Bond's room is often not in
   * the PIP's rendered set. Mark it visible so the frustum test can run. */
  if (force_room_visible)
    saved_room_rendered = force_bond_room_visible();

  chrTickBeams(&g_FreecamBondProp);

  if (force_room_visible)
    restore_bond_room_visible(saved_room_rendered);

  if (chr->field_20 != NULL) {
    gdl = chrRenderProp(&g_FreecamBondProp, gdl, FALSE);
    gdl = chrRenderProp(&g_FreecamBondProp, gdl, TRUE);
  }

  return gdl;
}

Gfx *practice_freecam_render_bond(Gfx *gdl) {
  f32 dx;
  f32 dy;
  f32 dz;

  if (!g_FreecamActive || get_cur_playernum() != g_FreecamPlayer)
    return gdl;

  dx = g_FreecamPosition.x - g_FreecamStartPosition.x;
  dy = g_FreecamPosition.y - g_FreecamStartPosition.y;
  dz = g_FreecamPosition.z - g_FreecamStartPosition.z;
  if (dx * dx + dy * dy + dz * dz <
      FREECAM_BOND_REVEAL_DISTANCE * FREECAM_BOND_REVEAL_DISTANCE)
    return gdl;

  return render_freecam_bond(gdl, FALSE);
}

void practice_freecam_sync_bond_for_external_cameras(s32 needed) {
  if (needed)
    create_freecam_bond();
}

Gfx *practice_freecam_render_bond_in_external_camera(Gfx *gdl) {
  return render_freecam_bond(gdl, TRUE);
}

void practice_freecam_release_deferred_model(s32 pending_gfx_tasks) {
  s32 previous_player;

  if (pending_gfx_tasks != 0 || g_FreecamBondDeferredModel == NULL)
    return;

  clear_aircraft_model_obj(g_FreecamBondDeferredModel);
  g_FreecamBondDeferredModel = NULL;

  /* A rapid re-entry can occur before the previous model was safe to release.
   * Create the replacement as soon as the pool instance becomes available. */
  if (g_CurrentPlayer != NULL && g_FreecamBondProp.chr == NULL) {
    previous_player = get_cur_playernum();
    set_cur_player(g_FreecamPlayer);
    create_freecam_bond();
    set_cur_player(previous_player);
  }
}

void practice_freecam_prepare_state_load(void) {
  if (g_FreecamActive || g_FreecamBondProp.chr != NULL) {
    destroy_freecam_bond();
    practice_freecam_release_deferred_model(0);
  }
}

void practice_freecam_finish_state_load(void) {
  s32 previous_player;

  if ((!g_FreecamActive && !g_FreecamPinnedCameraActive) ||
      g_CurrentPlayer == NULL)
    return;

  previous_player = get_cur_playernum();
  set_cur_player(g_FreecamPlayer);
  create_freecam_bond();
  set_cur_player(previous_player);
}

void practice_freecam_age_rooms(void) {
  s32 room;

  if (!g_FreecamActive)
    return;

  for (room = 1; room < g_MaxNumRooms && room < MAXROOMCOUNT; room++) {
    s_room_info *info = &g_BgRoomInfo[room];

    if (info->field_35 != 0 ||
        (g_FreecamPinnedCameraActive && g_FreecamPinnedCamera.stan != NULL &&
         g_FreecamPinnedCamera.stan->room == room))
      continue;

    if (info->model_bin_loaded == 0 &&
        (info->ptr_point_index != NULL ||
         info->ptr_expanded_mapping_info != NULL)) {
      delete_room_data(room);
    } else if (info->model_bin_loaded == 4) {
      delete_room_data(room);
    } else if (info->model_bin_loaded != 0) {
      info->model_bin_loaded++;
    }
  }
}

void practice_freecam_tick(u16 hotkey_trigger) {
  u16 buttons;
  s32 stick_x;
  s32 stick_y;
  f32 move;
  f32 strafe;
  f32 vertical;
  f32 direction_length;
  f32 distance;
  f32 cos_yaw;
  f32 sin_yaw;
  u32 current_count;
  u32 elapsed_counts;

  if (!g_FreecamActive) {
    if (g_FreecamSwallowInput &&
        joyGetButtonsRaw(g_FreecamController, ANY_BUTTON) == 0 &&
        apply_deadzone(joyGetStickXRaw(g_FreecamController)) == 0 &&
        apply_deadzone(joyGetStickYRaw(g_FreecamController)) == 0) {
      g_FreecamSwallowInput = FALSE;
      joySetInputSuppressed(FALSE);
    }
    return;
  }

  current_count = osGetCount();
  elapsed_counts = current_count - g_FreecamLastCount;
  g_FreecamLastCount = current_count;

  buttons = joyGetButtonsRaw(g_FreecamController, ANY_BUTTON);
  if ((buttons & hotkey_trigger) &&
      (buttons & (A_BUTTON | B_BUTTON | START_BUTTON | U_JPAD | D_JPAD |
                  L_JPAD | R_JPAD | D_CBUTTONS
#if DEV
                  | U_CBUTTONS
#endif
                  ))) {
    return;
  }

  stick_x = apply_deadzone(joyGetStickXRaw(g_FreecamController));
  stick_y = apply_deadzone(joyGetStickYRaw(g_FreecamController));
  move = 0.0f;
  strafe = 0.0f;
  vertical = 0.0f;

  if (buttons & (U_CBUTTONS | U_JPAD))
    move += 1.0f;
  if (buttons & (D_CBUTTONS | D_JPAD))
    move -= 1.0f;
  if (buttons & (L_CBUTTONS | L_JPAD))
    strafe -= 1.0f;
  if (buttons & (R_CBUTTONS | R_JPAD))
    strafe += 1.0f;
  if (buttons & ((L_TRIG | R_TRIG) & ~hotkey_trigger))
    vertical += 1.0f;
  if (buttons & Z_TRIG)
    vertical -= 1.0f;

  g_FreecamYaw += stick_x * FREECAM_TURN_SPEED;
  g_FreecamPitch += stick_y * FREECAM_TURN_SPEED;

  if (g_FreecamPitch > FREECAM_MAX_PITCH)
    g_FreecamPitch = FREECAM_MAX_PITCH;
  if (g_FreecamPitch < -FREECAM_MAX_PITCH)
    g_FreecamPitch = -FREECAM_MAX_PITCH;

  if (move == 0.0f && strafe == 0.0f && vertical == 0.0f) {
    update_room();
    if (stick_x != 0 || stick_y != 0) {
      update_orientation();
      practice_invalidate_render_state();
    }
    return;
  }

  cos_yaw = cosf(g_FreecamYaw);
  sin_yaw = sinf(g_FreecamYaw);
  direction_length = sqrtf(move * move + strafe * strafe + vertical * vertical);
  distance = FREECAM_MOVE_SPEED * (f32)elapsed_counts /
             FREECAM_CPU_COUNTS_PER_SECOND / direction_length;
  g_FreecamPosition.x += (move * sin_yaw + strafe * cos_yaw) * distance;
  g_FreecamPosition.y += vertical * distance;
  g_FreecamPosition.z += (-move * cos_yaw + strafe * sin_yaw) * distance;
  update_room();
  update_orientation();
  practice_invalidate_render_state();
}

s32 practice_freecam_apply_camera(void) {
  if (!g_FreecamActive || get_cur_playernum() != g_FreecamPlayer)
    return FALSE;

  sub_GAME_7F0876C4(&g_FreecamPosition, &g_FreecamForward, &g_FreecamUp);
  return TRUE;
}

s32 practice_freecam_consume_camera_restore(void) {
  if (g_FreecamActive || !g_FreecamRestorePlayerCamera ||
      get_cur_playernum() != g_FreecamPlayer) {
    return FALSE;
  }

  g_FreecamRestorePlayerCamera = FALSE;
  return TRUE;
}

s32 practice_freecam_get_render_context(u8 *room, coord3d **position,
                                        StandTile **tile) {
  if (!g_FreecamActive || get_cur_playernum() != g_FreecamPlayer)
    return FALSE;

  if (room != NULL)
    *room = g_FreecamRoom;
  if (position != NULL)
    *position = &g_FreecamPosition;
  if (tile != NULL)
    *tile = g_FreecamTile;
  return TRUE;
}
