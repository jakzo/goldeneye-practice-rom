#ifndef PRACTICE_RENDER_H
#define PRACTICE_RENDER_H

#include <bondtypes.h>
#include <ultra64.h>

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
} PracticeRenderContext;

extern bool g_IsRenderOnly;

bool practice_has_paused_framebuffer(void);
void practice_capture_paused_framebuffer(void);
Gfx *practice_cache_ui_background(Gfx *gdl, s32 left, s32 top, s32 right,
                                  s32 bottom);
Gfx *practice_cache_paused_framebuffer(Gfx *gdl);
Gfx *practice_restore_paused_framebuffer(Gfx *gdl);
void practice_cache_equipped_weapon_matrix(PropRecord *weapon_prop);
void practice_prepare_paused_render_state(PracticeRenderContext *context);
void practice_prepare_character_render(PracticeRenderContext *context);
void practice_prepare_refreshed_render(PracticeRenderContext *context);
void practice_finish_character_render(PracticeRenderContext *context);
void practice_restore_render_matrices(void);
void practice_begin_live_render(void);
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
bool practice_is_render_state_invalidated(void);
bool practice_needs_refreshed_render(void);
void practice_validate_render_state(void);

#endif /* PRACTICE_RENDER_H */
