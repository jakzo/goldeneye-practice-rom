#ifndef PRACTICE_RENDER_H
#define PRACTICE_RENDER_H

#include <bondtypes.h>
#include <ultra64.h>

/* Leave this many free dyn bytes after a synthetic camera or Bond draw. */
#define PRACTICE_DYN_VTX_RESERVE 0x1000

typedef struct PracticeSavedPlayerPose {
  coord3d pos;
  coord3d pos3;
  coord3d collision_pos;
  coord3d collision_pos3;
  coord3d prop_pos;
  StandTile *room_pointer;
  StandTile *portal_tile;
  StandTile *prop_stan;
} PracticeSavedPlayerPose;

typedef struct PracticeRenderWatchState {
  u32 unknown_40990;
  u32 screen_index;
  u32 controller_options_index;
  u32 game_options_index;
  s32 mission_brief_index;
  s32 unknown_409A4;
  s32 item_is_actively_selected;
  s32 unknown_409AC;
  s32 unknown_409B0;
  s32 unknown_409B4;
  s32 current_item_index;
  f32 unknown_409BC;
  s32 unknown_409C0;
  s32 unknown_409C4;
  f32 unknown_409C8;
  f32 unknown_409CC;
  s32 unknown_409D0;
  f32 unknown_409D4;
  s32 unknown_409D8;
} PracticeRenderWatchState;

typedef struct PracticeRenderContext {
  void **joint_pool_order;
  void *joint_pool_after;
  s32 joint_pool_count;
  void *model_render_positions;
  s32 model_render_position_count;
  s32 rendered_all_characters;
  PracticeRenderWatchState *watch_state;
  coord3d current_model_pos;
  coord3d previous_model_pos;
  coord3d current_room_pos;
  s32 cur_room_index;
  void *prop_visibility_state;
  void *monitor_states;
  s32 monitor_state_count;
  bool freecam_render;
  PracticeSavedPlayerPose saved_player_pose;
} PracticeRenderContext;

extern bool g_IsRenderOnly;

bool practice_has_paused_framebuffer(void);
void practice_capture_paused_framebuffer(void);
Gfx *practice_cache_ui_background(Gfx *gdl, s32 left, s32 top, s32 right,
                                  s32 bottom);
Gfx *practice_cache_paused_framebuffer(Gfx *gdl);
Gfx *practice_restore_paused_framebuffer(Gfx *gdl);
void practice_cache_equipped_weapon_matrix(PropRecord *weapon_prop);
void practice_save_player_pose(PracticeSavedPlayerPose *saved);
void practice_apply_camera_pose(const coord3d *position, StandTile *tile);
void practice_restore_player_pose(const PracticeSavedPlayerPose *saved);
void practice_prepare_paused_render_state(PracticeRenderContext *context);
void practice_prepare_character_render(PracticeRenderContext *context);
void practice_prepare_refreshed_render(PracticeRenderContext *context);
void practice_finish_character_render(PracticeRenderContext *context);
void practice_begin_external_camera_prop_preparation(void);
void practice_finish_external_camera_prop_preparation(void);
void practice_initialize_external_camera_model(Model *model);
void practice_restore_render_matrices(void);
void practice_begin_live_render(void);
void practice_render_reset(void);
void practice_mark_converted_render_matrices(RenderPosView *render_pos);
bool practice_hand_render_matrices_are_fixed(s32 hand);
bool practice_prop_render_matrices_are_fixed(PropRecord *prop);
void practice_clear_loaded_hand_matrices_float(void);
void practice_mark_loaded_hand_matrices_float(s32 hand);
void practice_set_loaded_camera_matrices(Mtxf *matrix10cc, Mtxf *matrix10d4,
                                         Mtxf *matrix10e8, Mtxf *matrix10ec);
void practice_set_loaded_projection_matrix(Mtxf *projection);
void practice_set_loaded_room_projection_matrix(Mtx *room_projection);
void practice_invalidate_render_state(void);
void practice_defer_render_state_refresh(void);
bool practice_is_render_state_invalidated(void);
void practice_clear_model_render_positions(void);
bool practice_needs_refreshed_render(void);
void practice_validate_render_state(void);
void practice_persist_clipped_door_vertices(PropRecord *prop);
void practice_persist_all_clipped_door_vertices(void);

#endif /* PRACTICE_RENDER_H */
