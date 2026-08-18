#include "practice_freecam.h"
#include "bg.h"
#include "bondconstants.h"
#include "bondview.h"
#include "joy.h"
#include "math_atan2f.h"
#include "player.h"
#include "player_2.h"
#include "practice_bond_model.h"
#include "practice_external_camera.h"
#include "practice_render.h"
#include "stan.h"
#include "watch.h"
#include <math.h>
#include <os_extension.h>

#define FREECAM_STICK_DEADZONE 3
/* Same divisor bondview uses when converting a safe stick value to a speed. */
#define FREECAM_STICK_SCALE 70.0f
#define FREECAM_MOVE_SPEED 1000.0f
#define FREECAM_CPU_COUNTS_PER_SECOND ((f32)(OS_CLOCK_RATE * 3 / 4))
#define FREECAM_TURN_SPEED DegToRad(0.125f)
/* Just short of vertical so the look vector keeps a horizontal component. */
#define FREECAM_MAX_PITCH DegToRad(89.0f)
#define FREECAM_BOND_REVEAL_DISTANCE 50.0f
#define FREECAM_FORWARD_EPSILON 0.000001f
/* Vanilla bg.c unloads a room once model_bin_loaded has aged to 4. */
#define FREECAM_ROOM_AGE_UNLOAD 4
#define FREECAM_ROOM_BITMAP_SIZE ((MAXROOMCOUNT + 7) / 8)

extern void sub_GAME_7F0876C4(coord3d *cam_pos, coord3d *cam_look,
                              coord3d *cam_up);
extern void sub_GAME_7F0B6368(s32 room);

static struct {
  s32 active;
  s32 swallow_input;
  /* Physical pad used for look/move. May be outside getPlayerCount(). */
  s32 controller;
  /* Clamped player index for camera and render. */
  s32 player;
  s32 restore_player_camera;
  u8 room;
  StandTile *tile;
  coord3d tile_position;
  coord3d position;
  coord3d start_position;
  coord3d forward;
  coord3d up;
  f32 yaw;
  f32 pitch;
  u32 last_count;
} g_Freecam;

static u8 g_FreecamInitialRoomResidency[FREECAM_ROOM_BITMAP_SIZE];

static struct {
  s32 active;
  struct PracticeExternalCameraView view;
} g_FreecamPin;

static s32 room_is_valid(s32 room) {
  return room > 0 && room < g_MaxNumRooms && room < MAXROOMCOUNT;
}

static s32 room_has_geometry(const s_room_info *info) {
  return info->model_bin_loaded != 0 || info->ptr_point_index != NULL ||
         info->ptr_expanded_mapping_info != NULL;
}

static s32 is_pinned_room(s32 room) {
  return g_FreecamPin.active && g_FreecamPin.view.stan != NULL &&
         g_FreecamPin.view.stan->room == room;
}

static s32 was_room_resident_on_enable(s32 room) {
  return g_FreecamInitialRoomResidency[room >> 3] & (1 << (room & 7));
}

static void capture_initial_room_residency(void) {
  s32 room;

  bzero(g_FreecamInitialRoomResidency, sizeof(g_FreecamInitialRoomResidency));
  for (room = 1; room < g_MaxNumRooms && room < MAXROOMCOUNT; room++) {
    if (room_has_geometry(&g_BgRoomInfo[room]))
      g_FreecamInitialRoomResidency[room >> 3] |= 1 << (room & 7);
  }
}

static void restore_initial_room_residency(void) {
  s32 room;

  /* Free the temporary freecam set first so the original set has the same
   * amount of room-heap space available that it had when freecam started. */
  for (room = 1; room < g_MaxNumRooms && room < MAXROOMCOUNT; room++) {
    s_room_info *info = &g_BgRoomInfo[room];

    if (!was_room_resident_on_enable(room) && !is_pinned_room(room) &&
        room_has_geometry(info)) {
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
  f32 x = g_Freecam.position.x;
  f32 y = g_Freecam.position.y;
  f32 z = g_Freecam.position.z;

  if (!stanFindNearestValidTilePointIncremental(
          &x, &y, &z, 0.0f, g_Freecam.tile, g_Freecam.tile_position.x,
          g_Freecam.tile_position.z, &tile))
    return;

  if (tile != NULL) {
    g_Freecam.tile = tile;
    g_Freecam.tile_position.x = x;
    g_Freecam.tile_position.y = y;
    g_Freecam.tile_position.z = z;
    if (room_is_valid(tile->room) && tile->room != g_Freecam.room) {
      g_Freecam.room = tile->room;
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
  f32 cos_pitch = cosf(g_Freecam.pitch);
  f32 sin_pitch = sinf(g_Freecam.pitch);
  f32 cos_yaw = cosf(g_Freecam.yaw);
  f32 sin_yaw = sinf(g_Freecam.yaw);

  g_Freecam.forward.x = cos_pitch * sin_yaw;
  g_Freecam.forward.y = sin_pitch;
  g_Freecam.forward.z = -cos_pitch * cos_yaw;

  g_Freecam.up.x = -sin_pitch * sin_yaw;
  g_Freecam.up.y = cos_pitch;
  g_Freecam.up.z = sin_pitch * cos_yaw;
}

static void init_orientation_from_forward(void) {
  f32 forward_length_squared;
  f32 horizontal_length;

  forward_length_squared = g_Freecam.forward.x * g_Freecam.forward.x +
                           g_Freecam.forward.y * g_Freecam.forward.y +
                           g_Freecam.forward.z * g_Freecam.forward.z;
  if (forward_length_squared < FREECAM_FORWARD_EPSILON) {
    g_Freecam.forward.x = 0.0f;
    g_Freecam.forward.y = 0.0f;
    g_Freecam.forward.z = -1.0f;
  }

  horizontal_length = sqrtf(g_Freecam.forward.x * g_Freecam.forward.x +
                            g_Freecam.forward.z * g_Freecam.forward.z);
  g_Freecam.yaw = atan2f(g_Freecam.forward.x, -g_Freecam.forward.z);
  g_Freecam.pitch = atan2f(g_Freecam.forward.y, horizontal_length);
  if (g_Freecam.pitch > M_PI_F)
    g_Freecam.pitch -= M_TAU_F;
  update_orientation();
}

static void copy_player_start_position(void) {
  if (g_CurrentPlayer->unknown == 1)
    g_Freecam.start_position = g_CurrentPlayer->pos;
  else
    g_Freecam.start_position = g_CurrentPlayer->field_488.pos;
}

static void init_camera_from_pin(void) {
  /* Resume from the pinned PIP instead of Bond's current camera. Bond
   * reveal still uses his live position so a distant pin shows him
   * immediately. */
  g_Freecam.position = g_FreecamPin.view.position;
  g_Freecam.forward = g_FreecamPin.view.look;
  g_Freecam.tile = g_FreecamPin.view.stan;
  g_Freecam.room = g_Freecam.tile != NULL ? g_Freecam.tile->room
                                          : bondviewGetCurrentPlayersRoom();
  copy_player_start_position();
}

static void init_camera_from_player(void) {
  if (g_CurrentPlayer->unknown == 1) {
    g_Freecam.position = g_CurrentPlayer->pos;
    g_Freecam.forward.x = g_CurrentPlayer->pos2.x - g_CurrentPlayer->pos.x;
    g_Freecam.forward.y = g_CurrentPlayer->pos2.y - g_CurrentPlayer->pos.y;
    g_Freecam.forward.z = g_CurrentPlayer->pos2.z - g_CurrentPlayer->pos.z;
  } else {
    g_Freecam.position = g_CurrentPlayer->field_488.pos;
    g_Freecam.forward = g_CurrentPlayer->field_488.applied_view;
  }
  g_Freecam.room = bondviewGetCurrentPlayersRoom();
  g_Freecam.tile = g_CurrentPlayer->field_488.current_tile_ptr;
  g_Freecam.start_position = g_Freecam.position;
}

s32 practice_freecam_enable(s32 controller) {
  s32 previous_player;

  if (g_CurrentPlayer == NULL || controller < 0)
    return FALSE;

  previous_player = get_cur_playernum();
  g_Freecam.player = controller < getPlayerCount() ? controller : 0;
  set_cur_player(g_Freecam.player);

  if (g_FreecamPin.active)
    init_camera_from_pin();
  else
    init_camera_from_player();

  g_Freecam.tile_position = g_Freecam.position;
  stanFindNearestValidTilePointIncrementalReset();
  init_orientation_from_forward();

  capture_initial_room_residency();
  practice_bond_model_load_and_ensure();
  g_Freecam.controller = controller;
  g_Freecam.active = TRUE;
  g_Freecam.restore_player_camera = FALSE;
  g_Freecam.swallow_input = FALSE;
  g_Freecam.last_count = osGetCount();
  joySetInputSuppressed(TRUE);
  practice_invalidate_render_state();
  set_cur_player(previous_player);
  return TRUE;
}

void practice_freecam_disable(void) {
  restore_initial_room_residency();
  g_Freecam.active = FALSE;
  g_Freecam.restore_player_camera = TRUE;
  g_Freecam.swallow_input = TRUE;
  stanFindNearestValidTilePointIncrementalReset();
  practice_invalidate_render_state();
}

void practice_freecam_reset(void) {
  g_Freecam.active = FALSE;
  g_Freecam.swallow_input = FALSE;
  g_Freecam.controller = 0;
  g_Freecam.player = 0;
  g_Freecam.restore_player_camera = FALSE;
  g_Freecam.room = 0;
  g_Freecam.tile = NULL;
  g_FreecamPin.active = FALSE;
  g_Freecam.last_count = 0;
  stanFindNearestValidTilePointIncrementalReset();
  joySetInputSuppressed(FALSE);
}

s32 practice_freecam_is_active(void) { return g_Freecam.active; }

void practice_freecam_pin_camera(void) {
  if (!g_Freecam.active || g_Freecam.tile == NULL)
    return;

  g_FreecamPin.view.position = g_Freecam.position;
  g_FreecamPin.view.look = g_Freecam.forward;
  g_FreecamPin.view.up = g_Freecam.up;
  g_FreecamPin.view.stan = g_Freecam.tile;
  g_FreecamPin.view.tracked_prop = NULL;
  g_FreecamPin.view.forced_object = NULL;
  g_FreecamPin.view.flags =
      PRACTICE_EXTERNAL_CAMERA_PRESERVE_GAMEPLAY_VISIBILITY;
  g_FreecamPin.view.border_color = PRACTICE_FILL_COLOR(255, 51, 255);
  g_FreecamPin.active = TRUE;
  practice_invalidate_render_state();
}

void practice_freecam_clear_pinned_camera(void) { g_FreecamPin.active = FALSE; }

s32 practice_freecam_add_pinned_camera_view(void) {
  if (g_Freecam.active || !g_FreecamPin.active)
    return FALSE;

  return practice_external_camera_add_view(&g_FreecamPin.view);
}

Gfx *practice_freecam_render_bond(Gfx *gdl) {
  f32 dx;
  f32 dy;
  f32 dz;

  if (!g_Freecam.active || get_cur_playernum() != g_Freecam.player)
    return gdl;

  dx = g_Freecam.position.x - g_Freecam.start_position.x;
  dy = g_Freecam.position.y - g_Freecam.start_position.y;
  dz = g_Freecam.position.z - g_Freecam.start_position.z;
  if (dx * dx + dy * dy + dz * dz <
      FREECAM_BOND_REVEAL_DISTANCE * FREECAM_BOND_REVEAL_DISTANCE)
    return gdl;

  return practice_bond_model_render(gdl, FALSE);
}

void practice_freecam_finish_state_load(void) {
  s32 previous_player;

  if ((!g_Freecam.active && !g_FreecamPin.active) || g_CurrentPlayer == NULL)
    return;

  previous_player = get_cur_playernum();
  set_cur_player(g_Freecam.player);
  practice_bond_model_load_and_ensure();
  set_cur_player(previous_player);
}

void practice_freecam_age_rooms(void) {
  s32 room;

  if (!g_Freecam.active)
    return;

  for (room = 1; room < g_MaxNumRooms && room < MAXROOMCOUNT; room++) {
    s_room_info *info = &g_BgRoomInfo[room];

    if (info->field_35 != 0 || is_pinned_room(room))
      continue;

    if (info->model_bin_loaded == 0 &&
        (info->ptr_point_index != NULL ||
         info->ptr_expanded_mapping_info != NULL)) {
      delete_room_data(room);
    } else if (info->model_bin_loaded == FREECAM_ROOM_AGE_UNLOAD) {
      delete_room_data(room);
    } else if (info->model_bin_loaded != 0) {
      info->model_bin_loaded++;
    }
  }
}

static s32 freecam_control_style(void);
static s32 style_is_dual_stick(s32 style);

static void swallow_exit_input(void) {
  s32 pad2;

  if (!g_Freecam.swallow_input)
    return;
  if (joyGetButtonsRaw(g_Freecam.controller, ANY_BUTTON) != 0 ||
      apply_deadzone(joyGetStickXRaw(g_Freecam.controller)) != 0 ||
      apply_deadzone(joyGetStickYRaw(g_Freecam.controller)) != 0)
    return;

  if (style_is_dual_stick(freecam_control_style())) {
    pad2 = g_Freecam.player + getPlayerCount();
    if (joyGetButtonsRaw(pad2, ANY_BUTTON) != 0 ||
        apply_deadzone(joyGetStickXRaw(pad2)) != 0 ||
        apply_deadzone(joyGetStickYRaw(pad2)) != 0)
      return;
  }

  g_Freecam.swallow_input = FALSE;
  joySetInputSuppressed(FALSE);
}

static u16 hotkey_chord_buttons(void) {
  u16 buttons = A_BUTTON | B_BUTTON | START_BUTTON | U_JPAD | D_JPAD | L_JPAD |
                R_JPAD | D_CBUTTONS;
#if DEV
  buttons |= U_CBUTTONS;
#endif
  return buttons;
}

typedef struct FreecamAxes {
  s32 look_x;
  s32 look_y;
  f32 move;
  f32 strafe;
  f32 vertical;
} FreecamAxes;

static s32 freecam_control_style(void) {
  s32 previous_player;
  s32 style;

  previous_player = get_cur_playernum();
  set_cur_player(g_Freecam.player);
  style = cur_player_get_control_type();
  set_cur_player(previous_player);
  return style;
}

static s32 style_is_dual_stick(s32 style) {
  return style >= CONTROLLER_CONFIG_PLENTY && style <= CONTROLLER_CONFIG_GOODHEAD;
}

static s32 style_uses_stick_look(s32 style) {
  return style == CONTROLLER_CONFIG_SOLITARE ||
         style == CONTROLLER_CONFIG_GOODNIGHT;
}

static f32 analog_from_stick(s32 value) {
  f32 analog = (f32)value / FREECAM_STICK_SCALE;

  if (analog > 1.0f)
    return 1.0f;
  if (analog < -1.0f)
    return -1.0f;
  return analog;
}

static void apply_look_invert(FreecamAxes *axes) {
  /* Option 0 is Reverse, 1 is Upright. Upright keeps stick/C-up looking up. */
  if (!get_cur_player_look_vertical_inverted())
    axes->look_y = -axes->look_y;
}

static void read_freecam_axes(FreecamAxes *axes, u16 buttons,
                              u16 hotkey_trigger) {
  s32 style = freecam_control_style();
  s32 pad = g_Freecam.controller;
  s32 stick_x = apply_deadzone(joyGetStickXRaw(pad));
  s32 stick_y = apply_deadzone(joyGetStickYRaw(pad));

  axes->look_x = 0;
  axes->look_y = 0;
  axes->move = 0.0f;
  axes->strafe = 0.0f;
  axes->vertical = 0.0f;

  if (style_is_dual_stick(style)) {
    s32 pad2 = g_Freecam.player + getPlayerCount();
    s32 stick2_x;
    s32 stick2_y;

    stick_x = apply_deadzone(joyGetStickXRaw(g_Freecam.player));
    stick_y = apply_deadzone(joyGetStickYRaw(g_Freecam.player));
    stick2_x = apply_deadzone(joyGetStickXRaw(pad2));
    stick2_y = apply_deadzone(joyGetStickYRaw(pad2));
    if (joyGetButtonsRaw(g_Freecam.player, Z_TRIG))
      axes->vertical -= 1.0f;
    if (joyGetButtonsRaw(pad2, Z_TRIG))
      axes->vertical += 1.0f;

    /* First stick X always turns. Second stick X always strafes. */
    axes->look_x = stick_x;
    axes->strafe = analog_from_stick(stick2_x);

    if (style == CONTROLLER_CONFIG_PLENTY ||
        style == CONTROLLER_CONFIG_DOMINO) {
      /* 2.1 / 2.3: first Y walks, second Y looks. */
      axes->move = analog_from_stick(stick_y);
      axes->look_y = stick2_y;
    } else {
      /* 2.2 / 2.4: first Y looks, second Y walks. */
      axes->look_y = stick_y;
      axes->move = analog_from_stick(stick2_y);
    }
  } else {
    if (buttons & ((L_TRIG | R_TRIG) & ~hotkey_trigger))
      axes->vertical += 1.0f;
    if (buttons & Z_TRIG)
      axes->vertical -= 1.0f;

    if (style_uses_stick_look(style)) {
      /* 1.2 / 1.4: stick looks, C/D-pad walks and strafes. */
      axes->look_x = stick_x;
      axes->look_y = stick_y;
      if (buttons & (U_CBUTTONS | U_JPAD))
        axes->move += 1.0f;
      if (buttons & (D_CBUTTONS | D_JPAD))
        axes->move -= 1.0f;
      if (buttons & (L_CBUTTONS | L_JPAD))
        axes->strafe -= 1.0f;
      if (buttons & (R_CBUTTONS | R_JPAD))
        axes->strafe += 1.0f;
    } else {
      /* 1.1 / 1.3: stick walks and turns, C/D-pad looks and strafes. */
      axes->look_x = stick_x;
      axes->move = analog_from_stick(stick_y);
      if (buttons & (U_CBUTTONS | U_JPAD))
        axes->look_y += (s32)FREECAM_STICK_SCALE;
      if (buttons & (D_CBUTTONS | D_JPAD))
        axes->look_y -= (s32)FREECAM_STICK_SCALE;
      if (buttons & (L_CBUTTONS | L_JPAD))
        axes->strafe -= 1.0f;
      if (buttons & (R_CBUTTONS | R_JPAD))
        axes->strafe += 1.0f;
    }
  }

  apply_look_invert(axes);
}

static void apply_look(s32 look_x, s32 look_y) {
  g_Freecam.yaw += look_x * FREECAM_TURN_SPEED;
  g_Freecam.pitch += look_y * FREECAM_TURN_SPEED;

  if (g_Freecam.pitch > FREECAM_MAX_PITCH)
    g_Freecam.pitch = FREECAM_MAX_PITCH;
  if (g_Freecam.pitch < -FREECAM_MAX_PITCH)
    g_Freecam.pitch = -FREECAM_MAX_PITCH;
}

static s32 apply_move(f32 move, f32 strafe, f32 vertical, u32 elapsed_counts) {
  f32 direction_length;
  f32 scale;
  f32 distance;
  f32 cos_yaw;
  f32 sin_yaw;

  if (move == 0.0f && strafe == 0.0f && vertical == 0.0f)
    return FALSE;

  direction_length = sqrtf(move * move + strafe * strafe + vertical * vertical);
  /* Analog walk should scale with stick magnitude. Only clamp diagonals that
   * would otherwise exceed full speed. */
  scale = direction_length > 1.0f ? direction_length : 1.0f;
  cos_yaw = cosf(g_Freecam.yaw);
  sin_yaw = sinf(g_Freecam.yaw);
  distance = FREECAM_MOVE_SPEED * (f32)elapsed_counts /
             FREECAM_CPU_COUNTS_PER_SECOND / scale;
  g_Freecam.position.x += (move * sin_yaw + strafe * cos_yaw) * distance;
  g_Freecam.position.y += vertical * distance;
  g_Freecam.position.z += (-move * cos_yaw + strafe * sin_yaw) * distance;
  return TRUE;
}

void practice_freecam_tick(u16 hotkey_trigger) {
  u16 buttons;
  FreecamAxes axes;
  u32 current_count;
  u32 elapsed_counts;

  if (!g_Freecam.active) {
    swallow_exit_input();
    return;
  }

  current_count = osGetCount();
  elapsed_counts = current_count - g_Freecam.last_count;
  g_Freecam.last_count = current_count;

  buttons = joyGetButtonsRaw(g_Freecam.controller, ANY_BUTTON);
  if ((buttons & hotkey_trigger) && (buttons & hotkey_chord_buttons()))
    return;

  read_freecam_axes(&axes, buttons, hotkey_trigger);
  apply_look(axes.look_x, axes.look_y);

  if (!apply_move(axes.move, axes.strafe, axes.vertical, elapsed_counts)) {
    update_room();
    if (axes.look_x != 0 || axes.look_y != 0) {
      update_orientation();
      practice_invalidate_render_state();
    }
    return;
  }

  update_room();
  update_orientation();
  practice_invalidate_render_state();
}

s32 practice_freecam_apply_camera(void) {
  if (!g_Freecam.active || get_cur_playernum() != g_Freecam.player)
    return FALSE;

  sub_GAME_7F0876C4(&g_Freecam.position, &g_Freecam.forward, &g_Freecam.up);
  return TRUE;
}

s32 practice_freecam_consume_camera_restore(void) {
  if (g_Freecam.active || !g_Freecam.restore_player_camera ||
      get_cur_playernum() != g_Freecam.player) {
    return FALSE;
  }

  g_Freecam.restore_player_camera = FALSE;
  return TRUE;
}

s32 practice_freecam_get_render_context(u8 *room, coord3d **position,
                                        StandTile **tile) {
  if (!g_Freecam.active || get_cur_playernum() != g_Freecam.player)
    return FALSE;

  if (room != NULL)
    *room = g_Freecam.room;
  if (position != NULL)
    *position = &g_Freecam.position;
  if (tile != NULL)
    *tile = g_Freecam.tile;
  return TRUE;
}
