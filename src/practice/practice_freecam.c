#include "practice_freecam.h"
#include "bg.h"
#include "bondconstants.h"
#include "bondview.h"
#include "joy.h"
#include "math_atan2f.h"
#include "player.h"
#include "player_2.h"
#include "practice_render.h"
#include "stan.h"
#include <math.h>
#include <os_extension.h>

#define FREECAM_STICK_DEADZONE 3
#define FREECAM_MOVE_SPEED 1000.0f
#define FREECAM_CPU_COUNTS_PER_SECOND 46875000.0f
#define FREECAM_TURN_SPEED 0.0021816616f
#define FREECAM_MAX_PITCH 1.55334306f

static s32 g_FreecamActive;
static s32 g_FreecamSwallowInput;
static s32 g_FreecamController;
static s32 g_FreecamPlayer;
static u8 g_FreecamRoom;
static StandTile *g_FreecamTile;
static coord3d g_FreecamTilePosition;
static coord3d g_FreecamPosition;
static coord3d g_FreecamForward;
static coord3d g_FreecamUp;
static f32 g_FreecamYaw;
static f32 g_FreecamPitch;
static u32 g_FreecamLastCount;

extern void sub_GAME_7F0876C4(coord3d *cam_pos, coord3d *cam_look,
                              coord3d *cam_up);

static void update_room(void) {
  StandTile *tile;
  f32 x = g_FreecamPosition.x;
  f32 y = g_FreecamPosition.y;
  f32 z = g_FreecamPosition.z;

  if (!stanFindNearestValidTilePointIncremental(&x, &y, &z, 0.0f,
                                                g_FreecamTile,
                                                g_FreecamTilePosition.x,
                                                g_FreecamTilePosition.z,
                                                &tile))
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
  g_FreecamTilePosition = g_FreecamPosition;
  stanFindNearestValidTilePointIncrementalReset();

  forward_length_squared =
      g_FreecamForward.x * g_FreecamForward.x +
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

  g_FreecamController = controller;
  g_FreecamActive = TRUE;
  g_FreecamSwallowInput = FALSE;
  g_FreecamLastCount = osGetCount();
  joySetInputSuppressed(TRUE);
  practice_invalidate_render_state();
  set_cur_player(previous_player);
  return TRUE;
}

void practice_freecam_disable(void) {
  g_FreecamActive = FALSE;
  g_FreecamSwallowInput = TRUE;
  stanFindNearestValidTilePointIncrementalReset();
}

void practice_freecam_reset(void) {
  g_FreecamActive = FALSE;
  g_FreecamSwallowInput = FALSE;
  g_FreecamController = 0;
  g_FreecamPlayer = 0;
  g_FreecamRoom = 0;
  g_FreecamTile = NULL;
  g_FreecamLastCount = 0;
  stanFindNearestValidTilePointIncrementalReset();
  joySetInputSuppressed(FALSE);
}

s32 practice_freecam_is_active(void) {
  return g_FreecamActive;
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
      (buttons & (A_BUTTON | START_BUTTON | U_JPAD | D_JPAD | L_JPAD |
                  R_JPAD | D_CBUTTONS
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
  direction_length = sqrtf(move * move + strafe * strafe +
                           vertical * vertical);
  distance = FREECAM_MOVE_SPEED * (f32)elapsed_counts /
             FREECAM_CPU_COUNTS_PER_SECOND / direction_length;
  g_FreecamPosition.x +=
      (move * sin_yaw + strafe * cos_yaw) * distance;
  g_FreecamPosition.y += vertical * distance;
  g_FreecamPosition.z +=
      (-move * cos_yaw + strafe * sin_yaw) * distance;
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

s32 practice_freecam_get_render_context(u8 *room, coord3d **position) {
  if (!g_FreecamActive || get_cur_playernum() != g_FreecamPlayer)
    return FALSE;

  if (room != NULL)
    *room = g_FreecamRoom;
  if (position != NULL)
    *position = &g_FreecamPosition;
  return TRUE;
}
