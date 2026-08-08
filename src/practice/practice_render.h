#ifndef PRACTICE_RENDER_H
#define PRACTICE_RENDER_H

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
  void *equipped_weapon_matrices;
  s32 equipped_weapon_matrix_count;
  s32 rendered_all_characters;
  PracticeRenderWatchState *watch_state;
} PracticeRenderContext;

extern bool g_IsRenderOnly;

void practice_cache_equipped_weapon_matrix(PropRecord *weapon_prop);
void practice_prepare_paused_render_state(PracticeRenderContext *context);
void practice_prepare_character_render(PracticeRenderContext *context);
void practice_prepare_refreshed_render(PracticeRenderContext *context);
void practice_finish_character_render(PracticeRenderContext *context);
void practice_restore_render_matrices(void);
void practice_set_loaded_camera_matrices(Mtxf *matrix10cc, Mtxf *matrix10d4,
                                         Mtxf *matrix10e8, Mtxf *matrix10ec);
void practice_set_loaded_projection_matrix(Mtxf *projection);
void practice_set_loaded_room_projection_matrix(Mtx *room_projection);
void practice_invalidate_render_state(void);
bool practice_is_render_state_invalidated(void);
void practice_validate_render_state(void);

#endif /* PRACTICE_RENDER_H */
