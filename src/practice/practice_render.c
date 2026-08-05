#include <ultra64.h>
#include <bondconstants.h>
#include <bondtypes.h>

#include "game/chrai.h"
#include "game/chr.h"
#include "game/chrobjhandler.h"
#include "game/dyn.h"
#include "game/matrixmath.h"
#include "game/player.h"
#include "practice_render.h"

extern s32 object_interaction(PropRecord *prop);

typedef struct PracticeRenderJoint {
  void *model;
  void *node;
  f32 depth;
  struct PracticeRenderJoint *next;
  struct PracticeRenderJoint *prev;
} PracticeRenderJoint;

extern PracticeRenderJoint *D_80036060;

bool g_IsRenderOnly = FALSE;
static bool g_IsRenderStateInvalidated = FALSE;

static s32 count_model_render_joints(Model *model) {
  ModelNode *node;
  ModelNode *parent;
  s32 count = 0;

  if (model == NULL || model->obj == NULL)
    return 0;

  node = model->obj->RootNode;
  parent = node != NULL ? node->Parent : NULL;

  while (node != NULL) {
    s32 type = node->Opcode & 0xff;

    if (type <= 3 || (type >= 11 && type <= 16) || type == 21)
      count++;

    if (node->Child != NULL) {
      node = node->Child;
    } else {
      while (node != NULL) {
        if (node == parent) {
          node = NULL;
          break;
        }

        if (node->Next != NULL) {
          node = node->Next;
          break;
        }

        node = node->Parent;
      }
    }
  }

  return count;
}

static s32 count_character_render_joints(ChrRecord *chr) {
  s32 count;
  s32 i;

  if (chr == NULL)
    return 0;

  count = count_model_render_joints(chr->model);

  for (i = 0; i < 3; i++) {
    PropRecord *held = chr->weapons_held[i];

    if (held != NULL && held->obj != NULL)
      count += count_model_render_joints(held->obj->model);
  }

  if (chr->handle_positiondata_hat != NULL &&
      chr->handle_positiondata_hat->obj != NULL) {
    count += count_model_render_joints(
        chr->handle_positiondata_hat->obj->model);
  }

  return count;
}

void practice_invalidate_render_state(void) {
  g_IsRenderStateInvalidated = TRUE;
}

bool practice_is_render_state_invalidated(void) {
  return g_IsRenderStateInvalidated;
}

void practice_validate_render_state(void) {
  g_IsRenderStateInvalidated = FALSE;
}

static void save_joint_pool(PracticeRenderContext *context, s32 max_count) {
  PracticeRenderJoint *joint;
  PracticeRenderJoint **order;
  s32 count = 0;
  s32 i;

  context->joint_pool_order = NULL;
  context->joint_pool_after = D_80036060;
  context->joint_pool_count = 0;

  for (joint = D_80036060;
       joint != NULL && count < max_count;
       joint = joint->next) {
    count++;
  }

  if (count == 0)
    return;

  order = dynAllocate(count * sizeof(*order));
  joint = D_80036060;

  for (i = 0; i < count; i++) {
    order[i] = joint;
    joint = joint->next;
  }

  context->joint_pool_order = (void **)order;
  context->joint_pool_after = joint;
  context->joint_pool_count = count;
}

static void restore_joint_pool(PracticeRenderContext *context) {
  PracticeRenderJoint **order =
      (PracticeRenderJoint **)context->joint_pool_order;
  PracticeRenderJoint *after =
      (PracticeRenderJoint *)context->joint_pool_after;
  s32 count = context->joint_pool_count;
  s32 i;

  for (i = 0; i < count; i++) {
    order[i]->next = i + 1 < count ? order[i + 1] : after;
    order[i]->prev = i > 0 ? order[i - 1] : NULL;
  }

  if (after != NULL)
    after->prev = count > 0 ? order[count - 1] : NULL;

  D_80036060 = count > 0 ? order[0] : after;
}

void practice_prepare_character_render(PracticeRenderContext *context) {
  PropRecord *prop;
  s32 max_joint_count = 0;

  g_IsRenderOnly = TRUE;

  for (prop = get_ptr_obj_pos_list_current_entry(); prop != NULL;
       prop = prop->prev) {
    if ((prop->flags & PROPFLAG_ONSCREEN) &&
        (prop->type == PROP_TYPE_CHR || prop->type == PROP_TYPE_VIEWER)) {
      max_joint_count += count_character_render_joints(prop->chr);
    }
  }

  save_joint_pool(context, max_joint_count);

  for (prop = get_ptr_obj_pos_list_current_entry(); prop != NULL;
       prop = prop->prev) {
    if (!(prop->flags & PROPFLAG_ONSCREEN))
      continue;

    if (prop->type == PROP_TYPE_CHR || prop->type == PROP_TYPE_VIEWER) {
      ChrRecord *chr = prop->chr;
      RenderPosView *render_pos;
      RenderPosView *held_render_pos[3] = {NULL, NULL, NULL};
      RenderPosView *hat_render_pos = NULL;
      f32 z_depth;
      s32 i;

      if (chr == NULL || chr->model == NULL)
        continue;

      render_pos = chr->model->render_pos;
      z_depth = prop->zDepth;

      for (i = 0; i < 3; i++) {
        PropRecord *held = chr->weapons_held[i];

        if (held != NULL && held->obj != NULL && held->obj->model != NULL)
          held_render_pos[i] = held->obj->model->render_pos;
      }

      if (chr->handle_positiondata_hat != NULL &&
          chr->handle_positiondata_hat->obj != NULL &&
          chr->handle_positiondata_hat->obj->model != NULL) {
        hat_render_pos = chr->handle_positiondata_hat->obj->model->render_pos;
      }

      chrTickBeams(prop);

      chr->model->render_pos = render_pos;
      prop->zDepth = z_depth;

      for (i = 0; i < 3; i++) {
        PropRecord *held = chr->weapons_held[i];

        if (held_render_pos[i] != NULL && held != NULL && held->obj != NULL &&
            held->obj->model != NULL) {
          held->obj->model->render_pos = held_render_pos[i];
        }
      }

      if (hat_render_pos != NULL && chr->handle_positiondata_hat != NULL &&
          chr->handle_positiondata_hat->obj != NULL &&
          chr->handle_positiondata_hat->obj->model != NULL) {
        chr->handle_positiondata_hat->obj->model->render_pos = hat_render_pos;
      }
    }
  }
}

void practice_prepare_refreshed_render(PracticeRenderContext *context) {
  PropRecord *prop;
  s32 max_joint_count = 0;
  s32 hand;

  g_IsRenderOnly = TRUE;

  /*
   * Hand render matrices live in the previous frame's dynamic arena. Reserve
   * new-frame storage and convert the saved fixed-point matrices into it
   * before any other render allocation can reuse their old addresses.
   */
  for (hand = 0; hand < 2; hand++) {
    struct hand *hand_state = &g_CurrentPlayer->hands[hand];
    ModelFileHeader *header = (ModelFileHeader *)hand_state->field_B70;
    RenderPosView *old_render_pos =
        (RenderPosView *)hand_state->field_B74;

    if (header != NULL && old_render_pos != NULL) {
      RenderPosView *new_render_pos =
          dynAllocate(header->numMatrices * sizeof(RenderPosView));
      s32 matrix;

      for (matrix = 0; matrix < header->numMatrices; matrix++) {
        Mtxf restored_matrix;

        /*
         * Rewinding the paused render arena can make new_render_pos equal
         * old_render_pos.  guMtxL2F is not safe when its input and output
         * overlap, so finish reading the fixed matrix into stack storage
         * before writing the restored float matrix back to the arena.
         */
        guMtxL2F(restored_matrix.m,
                 (Mtx *)&old_render_pos[matrix].pos);
        matrix_4x4_copy(&restored_matrix,
                        &new_render_pos[matrix].pos);
      }

      hand_state->field_B74 = (s32)new_render_pos;
    }
  }

  /*
   * A load can replace models and their dynamic matrix buffers after the
   * normal tick has already been skipped for a paused frame.  In that case
   * there is no previous fixed-point render matrix to convert back to float.
   * Build fresh matrices from the restored state instead.
   */
  for (prop = get_ptr_obj_pos_list_current_entry(); prop != NULL;
       prop = prop->prev) {
    if ((prop->type == PROP_TYPE_CHR || prop->type == PROP_TYPE_VIEWER) &&
        prop->chr != NULL) {
      max_joint_count += count_character_render_joints(prop->chr);
    }
  }

  save_joint_pool(context, max_joint_count);

  for (prop = get_ptr_obj_pos_list_current_entry(); prop != NULL;
       prop = prop->prev) {
    if ((prop->type == PROP_TYPE_CHR || prop->type == PROP_TYPE_VIEWER) &&
        prop->chr != NULL && prop->chr->model != NULL) {
      chrTickBeams(prop);
    } else if ((prop->type == PROP_TYPE_OBJ ||
                prop->type == PROP_TYPE_WEAPON ||
                prop->type == PROP_TYPE_DOOR) &&
               prop->obj != NULL && prop->obj->model != NULL) {
      object_interaction(prop);
    }
  }

  chraiUpdateOnscreenPropCount();
}

void practice_finish_character_render(PracticeRenderContext *context) {
  PropRecord *prop;

  for (prop = get_ptr_obj_pos_list_current_entry(); prop != NULL;
       prop = prop->prev) {
    if ((prop->flags & PROPFLAG_ONSCREEN) &&
        (prop->type == PROP_TYPE_CHR || prop->type == PROP_TYPE_VIEWER) &&
        prop->chr != NULL) {
      prop->chr->field_20 = NULL;
    }
  }

  restore_joint_pool(context);
  g_IsRenderOnly = FALSE;
}

static void restore_matrices(RenderPosView *render_pos, s32 count) {
  Mtxf matrix;
  s32 i;

  if (render_pos == NULL)
    return;

  for (i = 0; i < count; i++) {
    guMtxL2F(matrix.m, (Mtx *)&render_pos[i].pos);
    matrix_4x4_copy(&matrix, &render_pos[i].pos);
  }
}

static void restore_model_matrices(Model *model) {
  if (model == NULL || model->obj == NULL)
    return;

  restore_matrices(model->render_pos, model->obj->numMatrices);
}

void practice_restore_render_matrices(void) {
  PropRecord *prop;
  s32 i;

  for (prop = get_ptr_obj_pos_list_current_entry(); prop != NULL;
       prop = prop->prev) {
    if (!(prop->flags & PROPFLAG_ONSCREEN))
      continue;

    if (prop->type == PROP_TYPE_CHR || prop->type == PROP_TYPE_VIEWER) {
      ChrRecord *chr = prop->chr;

      if (chr == NULL)
        continue;

      restore_model_matrices(chr->model);

      for (i = 0; i < 3; i++) {
        PropRecord *held = chr->weapons_held[i];

        if (held != NULL && held->obj != NULL)
          restore_model_matrices(held->obj->model);
      }

      if (chr->handle_positiondata_hat != NULL &&
          chr->handle_positiondata_hat->obj != NULL) {
        restore_model_matrices(chr->handle_positiondata_hat->obj->model);
      }
    } else if (prop->type == PROP_TYPE_OBJ ||
               prop->type == PROP_TYPE_WEAPON ||
               prop->type == PROP_TYPE_DOOR) {
      if (prop->obj != NULL)
        restore_model_matrices(prop->obj->model);
    }
  }

  for (i = 0; i < 2; i++) {
    struct hand *hand = &g_CurrentPlayer->hands[i];
    ModelFileHeader *header = (ModelFileHeader *)hand->field_B70;

    if (header != NULL) {
      restore_matrices((RenderPosView *)hand->field_B74,
                       header->numMatrices);
    }

    if (hand->rocket != NULL)
      restore_model_matrices(hand->rocket->model);
  }
}
