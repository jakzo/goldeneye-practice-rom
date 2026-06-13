#include "practice_states_globals.h"
#include "practice_states_utils.h"
#include "bondview.h"
#include <ultra64.h>

#define BONDVIEW_HUD_MSG_BOTTOM_BUFFER_LENGTH 0x65

extern char stringbuffer_lowerleft[0x5][BONDVIEW_HUD_MSG_BOTTOM_BUFFER_LENGTH];
extern s32 status_bar_text_buffer_index;
extern s32 display_statusbar;
extern s32 g_EnterTankAudioState;

#if defined(VERSION_JP) || defined(VERSION_EU)
extern s32 dword_CODE_bss_jp80079CEC[0x05];
extern s32 dword_CODE_bss_jp80079Cd8[0x05];
#else
extern s32 copy_1stfonttable;
extern s32 copy_2ndfonttable;
#endif

extern void *memcpy(void *dst, const void *src, size_t count);

void save_tank_state(SavedTankState *dst) {
    dst->in_tank_flag = in_tank_flag;
    dst->g_PlayerTankProp_index = get_prop_index(g_PlayerTankProp);
    dst->g_WorldTankProp_index = get_prop_index(g_WorldTankProp);
    dst->g_PlayerTankYOffset = g_PlayerTankYOffset;
    dst->g_TankTurnSpeed = g_TankTurnSpeed;
    dst->g_TankOrientationAngle = g_TankOrientationAngle;
    dst->tank_turret_unused_angle = tank_turret_unused_angle;
    dst->g_TankTurretVerticalAngle = g_TankTurretVerticalAngle;
    dst->g_TankTurretVerticalAngleRelated = g_TankTurretVerticalAngleRelated;
    dst->g_TankTurretOrientationAngleRad = g_TankTurretOrientationAngleRad;
    dst->g_TankTurretOrientationAngleDeg = g_TankTurretOrientationAngleDeg;
    dst->tank_turret_turn_speed = tank_turret_turn_speed;
    dst->g_BondCanEnterTank = g_BondCanEnterTank;
    dst->g_TankTurretAngle = g_TankTurretAngle;
    dst->g_TankTurretTurn = g_TankTurretTurn;
    dst->g_ExplodeTankOnDeathFlag = g_ExplodeTankOnDeathFlag;
    dst->g_TankDamagePenaltyTicks = g_TankDamagePenaltyTicks;
    dst->g_EnterTankAudioState = g_EnterTankAudioState;
}

void load_tank_state(SavedTankState *src) {
    in_tank_flag = src->in_tank_flag;
    g_PlayerTankProp = get_safe_prop_by_index(src->g_PlayerTankProp_index);
    g_WorldTankProp = get_safe_prop_by_index(src->g_WorldTankProp_index);
    g_PlayerTankYOffset = src->g_PlayerTankYOffset;
    g_TankTurnSpeed = src->g_TankTurnSpeed;
    g_TankOrientationAngle = src->g_TankOrientationAngle;
    tank_turret_unused_angle = src->tank_turret_unused_angle;
    g_TankTurretVerticalAngle = src->g_TankTurretVerticalAngle;
    g_TankTurretVerticalAngleRelated = src->g_TankTurretVerticalAngleRelated;
    g_TankTurretOrientationAngleRad = src->g_TankTurretOrientationAngleRad;
    g_TankTurretOrientationAngleDeg = src->g_TankTurretOrientationAngleDeg;
    tank_turret_turn_speed = src->tank_turret_turn_speed;
    g_BondCanEnterTank = src->g_BondCanEnterTank;
    g_TankTurretAngle = src->g_TankTurretAngle;
    g_TankTurretTurn = src->g_TankTurretTurn;
    g_ExplodeTankOnDeathFlag = src->g_ExplodeTankOnDeathFlag;
    g_TankDamagePenaltyTicks = src->g_TankDamagePenaltyTicks;
    g_EnterTankAudioState = src->g_EnterTankAudioState;
}

void save_camera_state(SavedCameraState *dst) {
    dst->g_CameraMode = g_CameraMode;
    dst->camera_mode = camera_mode;
    dst->g_CameraAfterCinema = g_CameraAfterCinema;
    dst->camera_transition_timer = camera_transition_timer;
    dst->camera_fade_active = camera_fade_active;
    dst->intro_camera_index = intro_camera_index;
    dst->is_timer_active = is_timer_active;
    dst->g_PlayerInvincible = g_PlayerInvincible;
}

void load_camera_state(SavedCameraState *src) {
    g_CameraMode = src->g_CameraMode;
    camera_mode = src->camera_mode;
    g_CameraAfterCinema = src->g_CameraAfterCinema;
    camera_transition_timer = src->camera_transition_timer;
    camera_fade_active = src->camera_fade_active;
    intro_camera_index = src->intro_camera_index;
    is_timer_active = src->is_timer_active;
    g_PlayerInvincible = src->g_PlayerInvincible;
}
void save_hud_state(SavedHudState *dst) {
    dst->status_bar_text_buffer_index = status_bar_text_buffer_index;
    dst->display_statusbar = display_statusbar;
    memcpy(dst->stringbuffer_lowerleft, stringbuffer_lowerleft, sizeof(stringbuffer_lowerleft));
}

void load_hud_state(SavedHudState *src) {
    status_bar_text_buffer_index = src->status_bar_text_buffer_index;
    display_statusbar = src->display_statusbar;
    memcpy(stringbuffer_lowerleft, src->stringbuffer_lowerleft, sizeof(stringbuffer_lowerleft));
}

void save_font_state(SavedFontState *dst) {
#if defined(VERSION_JP) || defined(VERSION_EU)
    memcpy(dst->dword_CODE_bss_jp80079CEC, dword_CODE_bss_jp80079CEC, sizeof(dword_CODE_bss_jp80079CEC));
    memcpy(dst->dword_CODE_bss_jp80079Cd8, dword_CODE_bss_jp80079Cd8, sizeof(dword_CODE_bss_jp80079Cd8));
#else
    dst->copy_1stfonttable = copy_1stfonttable;
    dst->copy_2ndfonttable = copy_2ndfonttable;
#endif
}

void load_font_state(SavedFontState *src) {
#if defined(VERSION_JP) || defined(VERSION_EU)
    memcpy(dword_CODE_bss_jp80079CEC, src->dword_CODE_bss_jp80079CEC, sizeof(dword_CODE_bss_jp80079CEC));
    memcpy(dword_CODE_bss_jp80079Cd8, src->dword_CODE_bss_jp80079Cd8, sizeof(dword_CODE_bss_jp80079Cd8));
#else
    copy_1stfonttable = src->copy_1stfonttable;
    copy_2ndfonttable = src->copy_2ndfonttable;
#endif
}

void save_global_state(SavedGlobals *dst) {
    save_hud_state(&dst->hud);
    save_font_state(&dst->font);
    save_tank_state(&dst->tank);
    save_camera_state(&dst->camera);
}

void load_global_state(SavedGlobals *src) {
    load_hud_state(&src->hud);
    load_font_state(&src->font);
    load_tank_state(&src->tank);
    load_camera_state(&src->camera);
}
