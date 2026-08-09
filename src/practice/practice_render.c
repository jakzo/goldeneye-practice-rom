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
extern s32 modelFindNodeMtxIndex(ModelNode *node, s32 arg1);

typedef struct PracticeRenderJoint {
  void *model;
  void *node;
  f32 depth;
  struct PracticeRenderJoint *next;
  struct PracticeRenderJoint *prev;
} PracticeRenderJoint;

typedef struct PracticeRenderObject {
  Model *model;
  RenderPosView *render_pos;
} PracticeRenderObject;

extern PracticeRenderJoint *D_80036060;
extern u8 *g_GfxMemPos;
extern u8 *g_VtxBuffers[3];
extern u8 g_GfxActiveBufferIndex;
extern u32 D_80040990;
extern u32 watch_screen_index;
extern u32 controller_options_index;
extern u32 game_options_index;
extern s32 mission_brief_index;
extern s32 D_800409A4;
extern s32 watch_item_is_actively_selected;
extern s32 D_800409AC;
extern s32 D_800409B0;
extern s32 D_800409B4;
extern s32 g_curWatchItemIndex;
extern f32 D_800409BC;
extern s32 D_800409C0;
extern s32 D_800409C4;
extern f32 D_800409C8;
extern f32 D_800409CC;
extern s32 D_800409D0;
extern f32 D_800409D4;
extern s32 D_800409D8;

bool g_IsRenderOnly = FALSE;
static bool g_IsRenderStateInvalidated = FALSE;
static Mtxf g_LoadedCameraMatrix10CC;
static Mtxf g_LoadedCameraMatrix10D4;
static Mtxf g_LoadedCameraMatrix10E8;
static Mtxf g_LoadedPreviousCameraMatrix;
static Mtxf g_LoadedProjectionMatrix;
static Mtx g_LoadedRoomProjectionMatrix;
static bool g_HasLoadedProjectionMatrix;
static bool g_HasLoadedRoomProjectionMatrix;
static PracticeRenderWatchState g_PausedWatchState;

static void restore_model_matrices(Model *model);

void practice_set_loaded_camera_matrices(Mtxf *matrix10cc, Mtxf *matrix10d4,
                                         Mtxf *matrix10e8, Mtxf *matrix10ec) {
  if (matrix10cc != NULL) {
    matrix_4x4_copy(matrix10cc, &g_LoadedCameraMatrix10CC);
    g_CurrentPlayer->field_10CC = &g_LoadedCameraMatrix10CC;
  } else {
    g_CurrentPlayer->field_10CC = NULL;
  }
  if (matrix10d4 != NULL) {
    matrix_4x4_copy(matrix10d4, &g_LoadedCameraMatrix10D4);
    g_CurrentPlayer->field_10D4 = &g_LoadedCameraMatrix10D4;
  } else {
    g_CurrentPlayer->field_10D4 = NULL;
  }
  /* Loading occurs before the saved frame's render. The uninterrupted path
   * pushes each current camera matrix into its previous slot during that
   * render, before gameplay resumes. Reproduce that derived transition here. */
  if (matrix10cc != NULL) {
    matrix_4x4_copy(matrix10cc, &g_LoadedCameraMatrix10E8);
    g_CurrentPlayer->field_10E8 = &g_LoadedCameraMatrix10E8;
  } else if (matrix10e8 != NULL) {
    matrix_4x4_copy(matrix10e8, &g_LoadedCameraMatrix10E8);
    g_CurrentPlayer->field_10E8 = &g_LoadedCameraMatrix10E8;
  } else {
    g_CurrentPlayer->field_10E8 = NULL;
  }
  if (matrix10d4 != NULL) {
    matrix_4x4_copy(matrix10d4, &g_LoadedPreviousCameraMatrix);
    g_CurrentPlayer->field_10EC = &g_LoadedPreviousCameraMatrix;
  } else if (matrix10ec != NULL) {
    matrix_4x4_copy(matrix10ec, &g_LoadedPreviousCameraMatrix);
    g_CurrentPlayer->field_10EC = &g_LoadedPreviousCameraMatrix;
  } else {
    g_CurrentPlayer->field_10EC = NULL;
  }
}

void practice_set_loaded_projection_matrix(Mtxf *projection) {
  g_HasLoadedProjectionMatrix = projection != NULL;
  if (projection != NULL) {
    matrix_4x4_copy(projection, &g_LoadedProjectionMatrix);
    g_CurrentPlayer->projmatrixf = &g_LoadedProjectionMatrix;
  }
}

void practice_set_loaded_room_projection_matrix(Mtx *room_projection) {
  g_HasLoadedRoomProjectionMatrix = room_projection != NULL;
  if (room_projection != NULL) {
    bcopy(room_projection, &g_LoadedRoomProjectionMatrix,
          sizeof(g_LoadedRoomProjectionMatrix));
  }
}

static s32 equipped_weapon_matrix_index(Model *model) {
  s32 switch_index;

  if (model == NULL || model->obj == NULL)
    return -1;

  for (switch_index = 0;
       switch_index < model->obj->numSwitches && switch_index < 2;
       switch_index++) {
    s32 index = modelFindNodeMtxIndex(model->obj->Switches[switch_index], 0);

    if (index >= 0 && index < model->obj->numMatrices)
      return index;
  }

  return -1;
}

void practice_cache_equipped_weapon_matrix(PropRecord *weapon_prop) {
  ObjectRecord *obj = weapon_prop != NULL ? weapon_prop->obj : NULL;
  Model *model = obj != NULL ? obj->model : NULL;
  s32 index;

  /* A post-load refresh is render-only, but its rebuilt attachment matrix is
   * needed by the first live gameplay tick.  Ordinary held-pause renders must
   * not replace the gameplay cache. */
  if ((g_IsRenderOnly && !practice_is_render_state_invalidated()) ||
      weapon_prop == NULL ||
      weapon_prop->type != PROP_TYPE_WEAPON ||
      weapon_prop->parent == NULL ||
      weapon_prop->parent->type != PROP_TYPE_CHR || model == NULL ||
      model->render_pos == NULL)
    return;

  index = equipped_weapon_matrix_index(model);
  if (index >= 0) {
    Mtx fixed_matrix;

    /* Gameplay normally reads this matrix after the renderer has quantized it
     * to N64 fixed point. Keep the stable cache in float form, but round-trip
     * it through Mtx so the invalidated first tick sees the identical value. */
    guMtxF2L(model->render_pos[index].pos.m, &fixed_matrix);
    guMtxL2F(obj->mtx.m, &fixed_matrix);
  }
}

static void save_watch_state(PracticeRenderContext *context) {
  PracticeRenderWatchState *saved = &g_PausedWatchState;

  context->watch_state = saved;

  saved->unknown_40990 = D_80040990;
  saved->screen_index = watch_screen_index;
  saved->controller_options_index = controller_options_index;
  saved->game_options_index = game_options_index;
  saved->mission_brief_index = mission_brief_index;
  saved->unknown_409A4 = D_800409A4;
  saved->item_is_actively_selected = watch_item_is_actively_selected;
  saved->unknown_409AC = D_800409AC;
  saved->unknown_409B0 = D_800409B0;
  saved->unknown_409B4 = D_800409B4;
  saved->current_item_index = g_curWatchItemIndex;
  saved->unknown_409BC = D_800409BC;
  saved->unknown_409C0 = D_800409C0;
  saved->unknown_409C4 = D_800409C4;
  saved->unknown_409C8 = D_800409C8;
  saved->unknown_409CC = D_800409CC;
  saved->unknown_409D0 = D_800409D0;
  saved->unknown_409D4 = D_800409D4;
  saved->unknown_409D8 = D_800409D8;
}

static void restore_watch_state(PracticeRenderContext *context) {
  PracticeRenderWatchState *saved = context->watch_state;

  if (saved == NULL)
    return;

  D_80040990 = saved->unknown_40990;
  watch_screen_index = saved->screen_index;
  controller_options_index = saved->controller_options_index;
  game_options_index = saved->game_options_index;
  mission_brief_index = saved->mission_brief_index;
  D_800409A4 = saved->unknown_409A4;
  watch_item_is_actively_selected = saved->item_is_actively_selected;
  D_800409AC = saved->unknown_409AC;
  D_800409B0 = saved->unknown_409B0;
  D_800409B4 = saved->unknown_409B4;
  g_curWatchItemIndex = saved->current_item_index;
  D_800409BC = saved->unknown_409BC;
  D_800409C0 = saved->unknown_409C0;
  D_800409C4 = saved->unknown_409C4;
  D_800409C8 = saved->unknown_409C8;
  D_800409CC = saved->unknown_409CC;
  D_800409D0 = saved->unknown_409D0;
  D_800409D4 = saved->unknown_409D4;
  D_800409D8 = saved->unknown_409D8;
}

void practice_prepare_paused_render_state(PracticeRenderContext *context) {
  if (g_IsRenderStateInvalidated && g_HasLoadedProjectionMatrix &&
      g_HasLoadedRoomProjectionMatrix) {
    Mtx *projection = dynAllocateMatrix();
    Mtx *room_projection = dynAllocateMatrix();

    guMtxF2L(g_LoadedProjectionMatrix.m, projection);
    bcopy(&g_LoadedRoomProjectionMatrix, room_projection,
          sizeof(*room_projection));
    g_CurrentPlayer->projmatrix = projection;
    g_CurrentPlayer->field_10E0 = (s32)room_projection;
  }
  save_watch_state(context);
}

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
  return g_IsRenderStateInvalidated != FALSE;
}

bool practice_needs_refreshed_render(void) {
  return g_IsRenderStateInvalidated == TRUE;
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

static void save_model_render_positions(PracticeRenderContext *context,
                                        bool all_models) {
  PracticeRenderObject *saved;
  PropRecord *prop;
  s32 count = 0;

  for (prop = get_ptr_obj_pos_list_current_entry(); prop != NULL;
       prop = prop->prev) {
    bool include = all_models || (prop->flags & PROPFLAG_ONSCREEN);

    if (include &&
        (prop->type == PROP_TYPE_OBJ ||
         prop->type == PROP_TYPE_WEAPON ||
         prop->type == PROP_TYPE_DOOR) &&
        prop->obj != NULL && prop->obj->model != NULL) {
      count++;
    } else if (include &&
               (prop->type == PROP_TYPE_CHR ||
                prop->type == PROP_TYPE_VIEWER) &&
               prop->chr != NULL) {
      ChrRecord *chr = prop->chr;
      s32 i;

      if (chr->model != NULL)
        count++;

      for (i = 0; i < 3; i++) {
        PropRecord *held = chr->weapons_held[i];

        if (held != NULL && held->obj != NULL && held->obj->model != NULL)
          count++;
      }

      if (chr->handle_positiondata_hat != NULL &&
          chr->handle_positiondata_hat->obj != NULL &&
          chr->handle_positiondata_hat->obj->model != NULL) {
        count++;
      }
    }
  }

  context->model_render_positions = NULL;
  context->model_render_position_count = count;

  if (count == 0)
    return;

  saved = dynAllocate(count * sizeof(*saved));
  context->model_render_positions = saved;

  for (prop = get_ptr_obj_pos_list_current_entry(); prop != NULL;
       prop = prop->prev) {
    bool include = all_models || (prop->flags & PROPFLAG_ONSCREEN);

    if (include &&
        (prop->type == PROP_TYPE_OBJ ||
         prop->type == PROP_TYPE_WEAPON ||
         prop->type == PROP_TYPE_DOOR) &&
        prop->obj != NULL && prop->obj->model != NULL) {
      saved->model = prop->obj->model;
      saved->render_pos = prop->obj->model->render_pos;
      saved++;
    } else if (include &&
               (prop->type == PROP_TYPE_CHR ||
                prop->type == PROP_TYPE_VIEWER) &&
               prop->chr != NULL) {
      ChrRecord *chr = prop->chr;
      s32 i;

      if (chr->model != NULL) {
        saved->model = chr->model;
        saved->render_pos = chr->model->render_pos;
        saved++;
      }

      for (i = 0; i < 3; i++) {
        PropRecord *held = chr->weapons_held[i];

        if (held != NULL && held->obj != NULL && held->obj->model != NULL) {
          saved->model = held->obj->model;
          saved->render_pos = held->obj->model->render_pos;
          saved++;
        }
      }

      if (chr->handle_positiondata_hat != NULL &&
          chr->handle_positiondata_hat->obj != NULL &&
          chr->handle_positiondata_hat->obj->model != NULL) {
        saved->model = chr->handle_positiondata_hat->obj->model;
        saved->render_pos =
            chr->handle_positiondata_hat->obj->model->render_pos;
        saved++;
      }
    }
  }
}

static void restore_model_render_positions(PracticeRenderContext *context) {
  PracticeRenderObject *saved = context->model_render_positions;
  s32 i;

  for (i = 0; i < context->model_render_position_count; i++)
    saved[i].model->render_pos = saved[i].render_pos;
}

static void restore_refreshed_render_matrices(void) {
  PropRecord *prop;

  for (prop = get_ptr_obj_pos_list_current_entry(); prop != NULL;
       prop = prop->prev) {
    if (!(prop->flags & PROPFLAG_ONSCREEN)) {
      continue;
    }
    if ((prop->type == PROP_TYPE_CHR || prop->type == PROP_TYPE_VIEWER) &&
        prop->chr != NULL) {
      ChrRecord *chr = prop->chr;
      s32 hand;

      restore_model_matrices(chr->model);
      for (hand = 0; hand < 3; hand++) {
        PropRecord *held = chr->weapons_held[hand];
        s32 previous;
        bool duplicate = FALSE;

        for (previous = 0; previous < hand; previous++) {
          if (chr->weapons_held[previous] == held) {
            duplicate = TRUE;
            break;
          }
        }
        if (!duplicate && held != NULL && held->obj != NULL) {
          restore_model_matrices(held->obj->model);
        }
      }
      if (chr->handle_positiondata_hat != NULL &&
          chr->handle_positiondata_hat->obj != NULL) {
        restore_model_matrices(chr->handle_positiondata_hat->obj->model);
      }
    } else if ((prop->type == PROP_TYPE_OBJ ||
                prop->type == PROP_TYPE_WEAPON ||
                prop->type == PROP_TYPE_DOOR) &&
               (prop->parent == NULL ||
                (prop->parent->type != PROP_TYPE_CHR &&
                 prop->parent->type != PROP_TYPE_VIEWER)) &&
               prop->obj != NULL) {
      restore_model_matrices(prop->obj->model);
    }
  }
}

static void initialize_model_matrices(Model *model) {
  RenderPosView *new_render_pos;
  u32 render_pos_size;
  s32 matrix;

  if (model == NULL || model->obj == NULL)
    return;

  render_pos_size = model->obj->numMatrices * sizeof(*new_render_pos);
  new_render_pos = dynAllocate(render_pos_size);

  for (matrix = 0; matrix < model->obj->numMatrices; matrix++) {
    matrix_4x4_set_identity(&new_render_pos[matrix].pos);
  }

  model->render_pos = new_render_pos;
}

static void initialize_character_matrices(ChrRecord *chr) {
  s32 i;

  initialize_model_matrices(chr->model);

  for (i = 0; i < 3; i++) {
    PropRecord *held = chr->weapons_held[i];

    if (held != NULL && held->obj != NULL)
      initialize_model_matrices(held->obj->model);
  }

  if (chr->handle_positiondata_hat != NULL &&
      chr->handle_positiondata_hat->obj != NULL) {
    initialize_model_matrices(chr->handle_positiondata_hat->obj->model);
  }
}

void practice_prepare_character_render(PracticeRenderContext *context) {
  PropRecord *prop;
  s32 max_joint_count = 0;

  context->rendered_all_characters = FALSE;
  g_IsRenderOnly = TRUE;

  for (prop = get_ptr_obj_pos_list_current_entry(); prop != NULL;
       prop = prop->prev) {
    if ((prop->flags & PROPFLAG_ONSCREEN) &&
        (prop->type == PROP_TYPE_CHR || prop->type == PROP_TYPE_VIEWER)) {
      max_joint_count += count_character_render_joints(prop->chr);
    }
  }

  save_joint_pool(context, max_joint_count);
  save_model_render_positions(context, FALSE);

  for (prop = get_ptr_obj_pos_list_current_entry(); prop != NULL;
       prop = prop->prev) {
    if ((prop->flags & PROPFLAG_ONSCREEN) &&
        (prop->type == PROP_TYPE_OBJ ||
         prop->type == PROP_TYPE_WEAPON ||
         prop->type == PROP_TYPE_DOOR) &&
        prop->obj != NULL && prop->obj->model != NULL) {
      /* Visibility flags can survive a load even when the referenced matrix
       * buffer belonged to an older graphics-arena generation. Give the
       * object renderer unambiguous current-frame float storage. */
      initialize_model_matrices(prop->obj->model);
      continue;
    }

    if (!(prop->flags & PROPFLAG_ONSCREEN))
      continue;

    if (prop->type == PROP_TYPE_CHR || prop->type == PROP_TYPE_VIEWER) {
      ChrRecord *chr = prop->chr;
      f32 z_depth;

      if (chr == NULL || chr->model == NULL)
        continue;

      z_depth = prop->zDepth;

      /* Visibility can change while paused, so the previous render buffer may
       * contain either float or fixed-point matrices. Rebuild from model state
       * in an unambiguous current-arena float buffer. */
      initialize_character_matrices(chr);
      chrTickBeams(prop);

      prop->zDepth = z_depth;
    }
  }
}

void practice_prepare_refreshed_render(PracticeRenderContext *context) {
  PropRecord *prop;
  s32 max_joint_count = 0;
  s32 hand;

  /* Only visible models need matrix storage, but every restored character can
   * retain a joint pointer into the rewound arena. Clear those pointers in the
   * finish pass without rendering or allocating matrices for off-screen NPCs. */
  context->rendered_all_characters = TRUE;
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
      s32 render_pos_size = header->numMatrices * sizeof(RenderPosView);
      u8 *active_buffer = g_VtxBuffers[g_GfxActiveBufferIndex];
      RenderPosView *new_render_pos;
      s32 matrix;

      if ((u8 *)old_render_pos >= active_buffer &&
          (u8 *)old_render_pos + render_pos_size <= g_GfxMemPos) {
        new_render_pos = old_render_pos;
      } else {
        new_render_pos = dynAllocate(render_pos_size);
      }

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
    if ((prop->flags & PROPFLAG_ONSCREEN) &&
        (prop->type == PROP_TYPE_CHR || prop->type == PROP_TYPE_VIEWER) &&
        prop->chr != NULL) {
      max_joint_count += count_character_render_joints(prop->chr);
    }
  }

  save_joint_pool(context, max_joint_count);
  save_model_render_positions(context, FALSE);

  for (prop = get_ptr_obj_pos_list_current_entry(); prop != NULL;
       prop = prop->prev) {
    if (!(prop->flags & PROPFLAG_ONSCREEN))
      continue;

    if ((prop->type == PROP_TYPE_CHR || prop->type == PROP_TYPE_VIEWER) &&
        prop->chr != NULL && prop->chr->model != NULL) {
      initialize_character_matrices(prop->chr);
      chrTickBeams(prop);
    } else if ((prop->type == PROP_TYPE_OBJ ||
                prop->type == PROP_TYPE_WEAPON ||
                prop->type == PROP_TYPE_DOOR) &&
               prop->obj != NULL && prop->obj->model != NULL) {
      initialize_model_matrices(prop->obj->model);
      object_interaction(prop);
    }
  }

  chraiUpdateOnscreenPropCount();
}

void practice_finish_character_render(PracticeRenderContext *context) {
  PropRecord *prop;

  for (prop = get_ptr_obj_pos_list_current_entry(); prop != NULL;
       prop = prop->prev) {
    if ((context->rendered_all_characters ||
         (prop->flags & PROPFLAG_ONSCREEN)) &&
        (prop->type == PROP_TYPE_CHR || prop->type == PROP_TYPE_VIEWER) &&
        prop->chr != NULL) {
      /* A refreshed paused render ticks every character so newly restored
       * model matrices exist. Each tick also attaches its temporary render
       * joints to chr::field_20. Clear every such owner before putting the
       * saved joint pool back; otherwise an offscreen character retains a
       * pointer into the free list and creates a cycle on the next live tick.
       */
      prop->chr->field_20 = NULL;
    }
  }

  if (context->rendered_all_characters) {
    /* Retain the rebuilt allocations instead of restoring stale pre-load
     * arena pointers. Walk the live ownership graph because the saved pointer
     * list itself lives in that arena and may be overwritten by rendering. */
    restore_refreshed_render_matrices();
  } else {
    restore_model_render_positions(context);
  }
  restore_joint_pool(context);
  if (g_IsRenderStateInvalidated == TRUE) {
    /* Keep using refreshed matrices for subsequent held-pause renders until
     * the first live frame validates the reconstructed render state. */
    g_IsRenderStateInvalidated = 2;
  }
  restore_watch_state(context);
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
  s32 i;

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
