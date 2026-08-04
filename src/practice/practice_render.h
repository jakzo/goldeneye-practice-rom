#ifndef PRACTICE_RENDER_H
#define PRACTICE_RENDER_H

typedef struct PracticeRenderContext {
  void **joint_pool_order;
  void *joint_pool_after;
  s32 joint_pool_count;
} PracticeRenderContext;

extern bool g_IsRenderOnly;

void practice_prepare_character_render(PracticeRenderContext *context);
void practice_finish_character_render(PracticeRenderContext *context);
void practice_restore_render_matrices(void);

#endif /* PRACTICE_RENDER_H */
