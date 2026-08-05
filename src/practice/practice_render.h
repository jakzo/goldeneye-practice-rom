#ifndef PRACTICE_RENDER_H
#define PRACTICE_RENDER_H

typedef struct PracticeRenderContext {
  void **joint_pool_order;
  void *joint_pool_after;
  s32 joint_pool_count;
} PracticeRenderContext;

extern bool g_IsRenderOnly;

void practice_prepare_character_render(PracticeRenderContext *context);
void practice_prepare_refreshed_render(PracticeRenderContext *context);
void practice_finish_character_render(PracticeRenderContext *context);
void practice_restore_render_matrices(void);
void practice_invalidate_render_state(void);
bool practice_is_render_state_invalidated(void);
void practice_validate_render_state(void);

#endif /* PRACTICE_RENDER_H */
