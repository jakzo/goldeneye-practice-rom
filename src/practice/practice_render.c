#include <ultra64.h>
#include <bondconstants.h>
#include <bondtypes.h>

#include "game/chrai.h"
#include "game/chr.h"
#include "game/chrobjhandler.h"
#include "game/dyn.h"
#include "game/fog.h"
#include "game/matrixmath.h"
#include "game/objecthandler.h"
#include "game/player.h"
#include "game/viewport.h"
#ifdef PRACTICE_TEST_ROM
#include <assert.h>
#include "emu_log.h"
#endif
#include "fr.h"
#include "practice_external_camera.h"
#include "practice_freecam.h"
#include "practice_render.h"

extern s32 object_interaction(PropRecord *prop);
extern void sub_GAME_7F052D8C(DoorRecord *door);
extern s32 modelFindNodeMtxIndex(ModelNode *node, s32 arg1);
extern s32 g_viColorOutputMode;

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
  bool has_root_data;
  ModelRwData_HeaderRecord root_data;
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
static u8 g_ConvertedPropMatrices[(POS_DATA_ENTRY_LEN + 7) / 8];
static u8 g_ConvertedHandMatrices;
static u8 g_LoadedFloatHandMatrices;
static bool g_HasPausedFramebuffer;

#define PRACTICE_PROP_STATE_BITMAP_SIZE ((POS_DATA_ENTRY_LEN + 7) / 8)

typedef struct PracticePausedPropState {
  u8 onscreen[PRACTICE_PROP_STATE_BITMAP_SIZE];
  u8 offscreen_patrol[PRACTICE_PROP_STATE_BITMAP_SIZE];
  u8 has_been_seen[PRACTICE_PROP_STATE_BITMAP_SIZE];
} PracticePausedPropState;

typedef struct PracticePausedMonitorState {
  MonitorRecord *monitor;
  MonitorRecord state;
} PracticePausedMonitorState;

static void clear_converted_render_matrices(void);
static void refresh_object_render_state(PropRecord *prop);
#ifdef PRACTICE_TEST_ROM
static void assert_visible_clipped_door_vertices_persistent(void);
#endif

#define PRACTICE_UI_BACKGROUND_RECT_COUNT 32
typedef struct PracticeUiBackgroundRect {
  s16 left;
  s16 top;
  s16 right;
  s16 bottom;
} PracticeUiBackgroundRect;
static PracticeUiBackgroundRect
    g_PracticeUiBackgroundRects[PRACTICE_UI_BACKGROUND_RECT_COUNT];
static s32 g_PracticeUiBackgroundRectCount;

static void restore_matrices(RenderPosView *render_pos, s32 count);
static void restore_model_matrices(Model *model);

#define PAUSED_FRAMEBUFFER_TILE_HEIGHT 6

bool practice_has_paused_framebuffer(void) {
  return g_HasPausedFramebuffer;
}

Gfx *practice_cache_ui_background(Gfx *gdl, s32 left, s32 top, s32 right,
                                  s32 bottom) {
  u8 *source = viGetFrameBuf2();
  s32 y;

  if (z_buffer == 0 || g_viColorOutputMode == COLORMODE_32BIT ||
      z_buffer_width * z_buffer_height < SCREEN_WIDTH * SCREEN_HEIGHT ||
      g_PracticeUiBackgroundRectCount >= PRACTICE_UI_BACKGROUND_RECT_COUNT) {
    return gdl;
  }
  if (left < 0)
    left = 0;
  if (top < 0)
    top = 0;
  if (right > SCREEN_WIDTH)
    right = SCREEN_WIDTH;
  if (bottom > SCREEN_HEIGHT)
    bottom = SCREEN_HEIGHT;
  if (left >= right || top >= bottom)
    return gdl;

  g_PracticeUiBackgroundRects[g_PracticeUiBackgroundRectCount].left = left;
  g_PracticeUiBackgroundRects[g_PracticeUiBackgroundRectCount].top = top;
  g_PracticeUiBackgroundRects[g_PracticeUiBackgroundRectCount].right = right;
  g_PracticeUiBackgroundRects[g_PracticeUiBackgroundRectCount].bottom = bottom;
  g_PracticeUiBackgroundRectCount++;

  gDPPipeSync(gdl++);
  gDPSetCycleType(gdl++, G_CYC_COPY);
  gDPSetRenderMode(gdl++, G_RM_NOOP, G_RM_NOOP2);
  gDPSetTexturePersp(gdl++, G_TP_NONE);
  gDPSetTextureFilter(gdl++, G_TF_POINT);
  gDPSetAlphaCompare(gdl++, G_AC_NONE);
  gDPSetScissor(gdl++, G_SC_NON_INTERLACE, 0, 0, SCREEN_WIDTH,
                SCREEN_HEIGHT);
  gDPSetColorImage(gdl++, G_IM_FMT_RGBA, G_IM_SIZ_16b, SCREEN_WIDTH,
                   OS_K0_TO_PHYSICAL(z_buffer));

  for (y = top; y < bottom; y += PAUSED_FRAMEBUFFER_TILE_HEIGHT) {
    s32 tile_bottom = y + PAUSED_FRAMEBUFFER_TILE_HEIGHT;

    if (tile_bottom > bottom)
      tile_bottom = bottom;
    gDPLoadTextureTile(
        gdl++, OS_K0_TO_PHYSICAL(source), G_IM_FMT_RGBA, G_IM_SIZ_16b,
        SCREEN_WIDTH, SCREEN_HEIGHT, left, y, right - 1, tile_bottom - 1, 0,
        G_TX_CLAMP, G_TX_CLAMP, G_TX_NOMASK, G_TX_NOMASK, G_TX_NOLOD,
        G_TX_NOLOD);
    gSPTextureRectangle(gdl++, left << 2, y << 2, (right - 1) << 2,
                        (tile_bottom - 1) << 2, G_TX_RENDERTILE, left << 5,
                        y << 5, 4 << 10, 1 << 10);
    gDPPipeSync(gdl++);
  }

  gDPSetColorImage(gdl++, G_IM_FMT_RGBA, G_IM_SIZ_16b, SCREEN_WIDTH,
                   OS_K0_TO_PHYSICAL(source));
  return gdl;
}

void practice_capture_paused_framebuffer(void) {
  u8 *source = viGetFrameBuf1();
  u8 *destination = viGetFrameBuf2();
  u8 *background = (u8 *)z_buffer;
  u32 size = SCREEN_WIDTH * SCREEN_HEIGHT * 2;
  s32 rect_index;

  if (background == NULL || g_viColorOutputMode == COLORMODE_32BIT ||
      z_buffer_width * z_buffer_height < SCREEN_WIDTH * SCREEN_HEIGHT ||
      g_PracticeUiBackgroundRectCount == 0) {
    g_HasPausedFramebuffer = FALSE;
    return;
  }

  osInvalDCache(source, size);
  osInvalDCache(background, size);
  if (source != destination)
    bcopy(source, destination, size);
  /* Later UI elements may overlap earlier ones, so peel their saved
   * backgrounds in reverse draw order. */
  for (rect_index = g_PracticeUiBackgroundRectCount - 1; rect_index >= 0;
       rect_index--) {
    PracticeUiBackgroundRect *rect =
        &g_PracticeUiBackgroundRects[rect_index];
    s32 width = (rect->right - rect->left) * 2;
    s32 y;

    for (y = rect->top; y < rect->bottom; y++) {
      u32 offset = (y * SCREEN_WIDTH + rect->left) * 2;
      bcopy(background + offset, destination + offset, width);
    }
  }
  osWritebackDCache(destination, size);
  bcopy(destination, background, size);
  osWritebackDCache(background, size);
  g_HasPausedFramebuffer = TRUE;
}

Gfx *practice_cache_paused_framebuffer(Gfx *gdl) {
  u8 *source = viGetFrameBuf2();
  s32 y;

  if (z_buffer == 0 || g_viColorOutputMode == COLORMODE_32BIT ||
      z_buffer_width * z_buffer_height < SCREEN_WIDTH * SCREEN_HEIGHT) {
    g_HasPausedFramebuffer = FALSE;
    return gdl;
  }

  /* Preserve the completed paused scene before practice UI is composited. The
   * RDP copies six scanlines at a time so each RGBA16 tile fits in its 4 KiB
   * TMEM. */
  gDPPipeSync(gdl++);
  gDPSetCycleType(gdl++, G_CYC_COPY);
  gDPSetRenderMode(gdl++, G_RM_NOOP, G_RM_NOOP2);
  gDPSetTexturePersp(gdl++, G_TP_NONE);
  gDPSetTextureFilter(gdl++, G_TF_POINT);
  gDPSetAlphaCompare(gdl++, G_AC_NONE);
  gDPSetScissor(gdl++, G_SC_NON_INTERLACE, 0, 0, SCREEN_WIDTH,
                SCREEN_HEIGHT);
  gDPSetColorImage(gdl++, G_IM_FMT_RGBA, G_IM_SIZ_16b, SCREEN_WIDTH,
                   OS_K0_TO_PHYSICAL(z_buffer));

  for (y = 0; y < SCREEN_HEIGHT; y += PAUSED_FRAMEBUFFER_TILE_HEIGHT) {
    s32 bottom = y + PAUSED_FRAMEBUFFER_TILE_HEIGHT - 1;

    if (bottom >= SCREEN_HEIGHT)
      bottom = SCREEN_HEIGHT - 1;

    gDPLoadTextureTile(
        gdl++, OS_K0_TO_PHYSICAL(source), G_IM_FMT_RGBA, G_IM_SIZ_16b,
        SCREEN_WIDTH, SCREEN_HEIGHT, 0, y, SCREEN_WIDTH - 1, bottom, 0,
        G_TX_CLAMP, G_TX_CLAMP, G_TX_NOMASK, G_TX_NOMASK, G_TX_NOLOD,
        G_TX_NOLOD);
    gSPTextureRectangle(gdl++, 0, y << 2, (SCREEN_WIDTH - 1) << 2,
                        bottom << 2, G_TX_RENDERTILE, 0, y << 5, 4 << 10,
                        1 << 10);
    gDPPipeSync(gdl++);
  }

  gDPSetColorImage(gdl++, G_IM_FMT_RGBA, G_IM_SIZ_16b, SCREEN_WIDTH,
                   OS_K0_TO_PHYSICAL(source));
  g_HasPausedFramebuffer = TRUE;
  return gdl;
}

Gfx *practice_restore_paused_framebuffer(Gfx *gdl) {
  u8 *source = viGetFrameBuf1();
  u8 *destination = viGetFrameBuf2();
  u32 size = SCREEN_WIDTH * SCREEN_HEIGHT * 2;

  if (g_HasPausedFramebuffer && z_buffer != 0 &&
      z_buffer_width * z_buffer_height >= SCREEN_WIDTH * SCREEN_HEIGHT) {
    source = (u8 *)z_buffer;
  }

  if (source != destination) {
    osInvalDCache(source, size);
    bcopy(source, destination, size);
    osWritebackDCache(destination, size);
  }

  /* UI-only display lists do not call video_related_F, so explicitly retarget
   * the RDP when VI advances to the other framebuffer while paused. */
  gDPPipeSync(gdl++);
  gDPSetColorImage(gdl++, G_IM_FMT_RGBA, G_IM_SIZ_16b, SCREEN_WIDTH,
                   OS_K0_TO_PHYSICAL(destination));
  return gdl;
}

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
  /* Gameplay can consume the previous-frame matrices before another render
   * replaces them, so restore their saved values exactly. */
  if (matrix10e8 != NULL) {
    matrix_4x4_copy(matrix10e8, &g_LoadedCameraMatrix10E8);
    g_CurrentPlayer->field_10E8 = &g_LoadedCameraMatrix10E8;
  } else if (matrix10cc != NULL) {
    matrix_4x4_copy(matrix10cc, &g_LoadedCameraMatrix10E8);
    g_CurrentPlayer->field_10E8 = &g_LoadedCameraMatrix10E8;
  } else {
    g_CurrentPlayer->field_10E8 = NULL;
  }
  if (matrix10ec != NULL) {
    matrix_4x4_copy(matrix10ec, &g_LoadedPreviousCameraMatrix);
    g_CurrentPlayer->field_10EC = &g_LoadedPreviousCameraMatrix;
  } else if (matrix10d4 != NULL) {
    matrix_4x4_copy(matrix10d4, &g_LoadedPreviousCameraMatrix);
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

  /* Render-only work must not replace the gameplay cache restored by a save
   * state. The first live gameplay tick consumes that exact cached matrix. */
  if (g_IsRenderOnly || weapon_prop == NULL ||
      weapon_prop->type != PROP_TYPE_WEAPON ||
      weapon_prop->parent == NULL ||
      weapon_prop->parent->type != PROP_TYPE_CHR || model == NULL ||
      model->render_pos == NULL)
    return;

  index = equipped_weapon_matrix_index(model);
  if (index >= 0) {
    Mtx fixed_matrix;

    /* Gameplay reads this matrix after the renderer has quantized it with the
     * game's 65535-based conversion, not libultra's standard 16.16 helpers.
     * Use the same pack/unpack pair so the invalidated first tick receives the
     * bit-identical transform used by the ordinary gameplay path. */
    matrix_4x4_f32_to_s32(&model->render_pos[index].pos,
                          (Mtxf *)&fixed_matrix);
    sub_GAME_7F058E78((Mtxf *)&fixed_matrix, &obj->mtx);
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

static void save_prop_visibility_state(PracticeRenderContext *context) {
  PracticePausedPropState *state = dynAllocate(sizeof(*state));
  s32 i;

  bzero(state, sizeof(*state));
  for (i = 0; i < POS_DATA_ENTRY_LEN; i++) {
    PropRecord *prop = &pos_data_entry[i];
    u8 bit = 1 << (i & 7);

    if (prop->flags & PROPFLAG_ONSCREEN)
      state->onscreen[i >> 3] |= bit;
    if (prop->type == PROP_TYPE_CHR && prop->chr != NULL) {
      if (prop->chr->hidden & CHRHIDDEN_OFFSCREEN_PATROL)
        state->offscreen_patrol[i >> 3] |= bit;
      if (prop->chr->chrflags & CHRFLAG_HAS_BEEN_ON_SCREEN)
        state->has_been_seen[i >> 3] |= bit;
    }
  }

  context->prop_visibility_state = state;
}

static void restore_prop_visibility_state(PracticeRenderContext *context) {
  PracticePausedPropState *state = context->prop_visibility_state;
  s32 i;

  if (state == NULL)
    return;

  for (i = 0; i < POS_DATA_ENTRY_LEN; i++) {
    PropRecord *prop = &pos_data_entry[i];
    u8 bit = 1 << (i & 7);

    if (state->onscreen[i >> 3] & bit)
      prop->flags |= PROPFLAG_ONSCREEN;
    else
      prop->flags &= ~PROPFLAG_ONSCREEN;

    if (prop->type == PROP_TYPE_CHR && prop->chr != NULL) {
      if (state->offscreen_patrol[i >> 3] & bit)
        prop->chr->hidden |= CHRHIDDEN_OFFSCREEN_PATROL;
      else
        prop->chr->hidden &= ~CHRHIDDEN_OFFSCREEN_PATROL;
      if (state->has_been_seen[i >> 3] & bit)
        prop->chr->chrflags |= CHRFLAG_HAS_BEEN_ON_SCREEN;
      else
        prop->chr->chrflags &= ~CHRFLAG_HAS_BEEN_ON_SCREEN;
    }
  }

  chraiUpdateOnscreenPropCount();
}

static s32 count_monitor_states_for_prop(PropRecord *prop) {
  PropRecord *child;
  s32 count = 0;

  if ((prop->type == PROP_TYPE_OBJ || prop->type == PROP_TYPE_WEAPON ||
       prop->type == PROP_TYPE_DOOR) &&
      prop->obj != NULL) {
    if (prop->obj->type == PROPDEF_MONITOR)
      count++;
    else if (prop->obj->type == PROPDEF_MULTI_MONITOR)
      count += 4;
  }
  for (child = prop->child; child != NULL; child = child->prev)
    count += count_monitor_states_for_prop(child);
  return count;
}

static void save_monitor_states_for_prop(PracticePausedMonitorState *states,
                                         s32 *count, PropRecord *prop) {
  PropRecord *child;
  s32 i;

  if ((prop->type == PROP_TYPE_OBJ || prop->type == PROP_TYPE_WEAPON ||
       prop->type == PROP_TYPE_DOOR) &&
      prop->obj != NULL) {
    if (prop->obj->type == PROPDEF_MONITOR) {
      MonitorObjRecord *monitor = (MonitorObjRecord *)prop->obj;
      states[*count].monitor = &monitor->Monitor;
      states[*count].state = monitor->Monitor;
      (*count)++;
    } else if (prop->obj->type == PROPDEF_MULTI_MONITOR) {
      MultiMonitorObjRecord *monitor = (MultiMonitorObjRecord *)prop->obj;
      for (i = 0; i < 4; i++) {
        states[*count].monitor = &monitor->Monitor[i];
        states[*count].state = monitor->Monitor[i];
        (*count)++;
      }
    }
  }
  for (child = prop->child; child != NULL; child = child->prev)
    save_monitor_states_for_prop(states, count, child);
}

static void save_monitor_states(PracticeRenderContext *context) {
  PracticePausedMonitorState *states;
  PropRecord *prop;
  s32 count = 0;

  for (prop = get_ptr_obj_pos_list_current_entry(); prop != NULL;
       prop = prop->prev)
    count += count_monitor_states_for_prop(prop);

  context->monitor_states = NULL;
  context->monitor_state_count = 0;
  if (count == 0)
    return;

  states = dynAllocate(count * sizeof(*states));
  for (prop = get_ptr_obj_pos_list_current_entry(); prop != NULL;
       prop = prop->prev)
    save_monitor_states_for_prop(states, &context->monitor_state_count, prop);
  context->monitor_states = states;
}

static void restore_monitor_states(PracticeRenderContext *context) {
  PracticePausedMonitorState *states = context->monitor_states;
  s32 i;

  for (i = 0; states != NULL && i < context->monitor_state_count; i++)
    *states[i].monitor = states[i].state;
}

static void prepare_freecam_player_state(PracticeRenderContext *context) {
  coord3d *position;
  StandTile *tile;

  context->freecam_render = FALSE;
  context->monitor_states = NULL;
  context->monitor_state_count = 0;

  if (g_CurrentPlayer == NULL || g_CurrentPlayer->prop == NULL ||
      !practice_freecam_get_render_context(NULL, &position, &tile) ||
      position == NULL || tile == NULL) {
    return;
  }

  context->saved_player_pos = g_CurrentPlayer->pos;
  context->saved_player_pos3 = g_CurrentPlayer->pos3;
  context->saved_collision_pos = g_CurrentPlayer->field_488.pos;
  context->saved_collision_pos3 = g_CurrentPlayer->field_488.pos3;
  context->saved_room_pointer = g_CurrentPlayer->room_pointer;
  context->saved_portal_tile =
      g_CurrentPlayer->field_488.current_tile_ptr_for_portals;
  context->saved_player_prop_pos = g_CurrentPlayer->prop->pos;
  context->saved_player_prop_stan = g_CurrentPlayer->prop->stan;

  g_CurrentPlayer->pos = *position;
  g_CurrentPlayer->pos3 = *position;
  g_CurrentPlayer->field_488.pos = *position;
  g_CurrentPlayer->field_488.pos3 = *position;
  g_CurrentPlayer->room_pointer = tile;
  g_CurrentPlayer->field_488.current_tile_ptr_for_portals = tile;
  g_CurrentPlayer->prop->pos = *position;
  g_CurrentPlayer->prop->stan = tile;

  context->freecam_render = TRUE;
  save_monitor_states(context);
}

static void prepare_freecam_prop_visibility(PracticeRenderContext *context) {
  if (!context->freecam_render)
    return;

  practice_external_camera_set_rendering(TRUE);
  practice_external_camera_prepare_props(TRUE);
  chraiUpdateOnscreenPropCount();
}

static void restore_freecam_player_state(PracticeRenderContext *context) {
  if (!context->freecam_render)
    return;

  g_CurrentPlayer->pos = context->saved_player_pos;
  g_CurrentPlayer->pos3 = context->saved_player_pos3;
  g_CurrentPlayer->field_488.pos = context->saved_collision_pos;
  g_CurrentPlayer->field_488.pos3 = context->saved_collision_pos3;
  g_CurrentPlayer->room_pointer = context->saved_room_pointer;
  g_CurrentPlayer->field_488.current_tile_ptr_for_portals =
      context->saved_portal_tile;
  g_CurrentPlayer->prop->pos = context->saved_player_prop_pos;
  g_CurrentPlayer->prop->stan = context->saved_player_prop_stan;
  practice_external_camera_set_rendering(FALSE);
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
  bzero(g_ConvertedPropMatrices, sizeof(g_ConvertedPropMatrices));
  g_ConvertedHandMatrices = 0;
  context->current_model_pos = g_CurrentPlayer->current_model_pos;
  context->previous_model_pos = g_CurrentPlayer->previous_model_pos;
  context->current_room_pos = g_CurrentPlayer->current_room_pos;
  save_prop_visibility_state(context);
  prepare_freecam_player_state(context);
  save_watch_state(context);
}

void practice_invalidate_render_state(void) {
  /* Loading can rebuild a model into a fresh float matrix buffer while the
   * prop slot still carries the previous live render's fixed-point marker.
   * The refreshed render never consumes the old buffers, so discard all of
   * their representation metadata as soon as the state is invalidated. */
  clear_converted_render_matrices();
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

static void save_joint_pool(PracticeRenderContext *context) {
  PracticeRenderJoint *joint;
  PracticeRenderJoint **order;
  s32 count = 0;
  s32 i;

  context->joint_pool_order = NULL;
  context->joint_pool_after = D_80036060;
  context->joint_pool_count = 0;

  for (joint = D_80036060;
       joint != NULL && count < 600;
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

      for (i = 0; i < 2; i++) {
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

      for (i = 0; i < 2; i++) {
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

  saved = context->model_render_positions;
  {
    s32 i;
    for (i = 0; i < context->model_render_position_count; i++) {
      Model *model = saved[i].model;
      ModelNode *root = model != NULL && model->obj != NULL
                            ? model->obj->RootNode
                            : NULL;
      saved[i].has_root_data =
          root != NULL && (root->Opcode & 0xff) == MODELNODE_OPCODE_HEADER;
      if (saved[i].has_root_data) {
        ModelRwData_HeaderRecord *root_data =
            (ModelRwData_HeaderRecord *)modelGetNodeRwData(model, root);
        saved[i].root_data = *root_data;
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

static void restore_model_root_data(PracticeRenderContext *context) {
  PracticeRenderObject *saved = context->model_render_positions;
  s32 i;

  for (i = 0; i < context->model_render_position_count; i++) {
    if (saved[i].has_root_data) {
      ModelNode *root = saved[i].model->obj->RootNode;
      ModelRwData_HeaderRecord *root_data =
          (ModelRwData_HeaderRecord *)modelGetNodeRwData(saved[i].model, root);
      *root_data = saved[i].root_data;
    }
  }
}

static void restore_refreshed_render_matrices(void) {
  s32 prop_index;

  for (prop_index = 0; prop_index < POS_DATA_ENTRY_LEN; prop_index++) {
    PropRecord *prop = &pos_data_entry[prop_index];

    if (!(g_ConvertedPropMatrices[prop_index >> 3] &
          (1 << (prop_index & 7)))) {
      continue;
    }

    if ((prop->type == PROP_TYPE_CHR || prop->type == PROP_TYPE_VIEWER) &&
        prop->chr != NULL && prop->chr->model != NULL) {
      restore_model_matrices(prop->chr->model);
    } else if ((prop->type == PROP_TYPE_OBJ ||
                prop->type == PROP_TYPE_WEAPON ||
                prop->type == PROP_TYPE_DOOR) &&
               prop->obj != NULL && prop->obj->model != NULL) {
      restore_model_matrices(prop->obj->model);
    }
  }
}

static Model *get_live_prop_model(PropRecord *prop) {
  if (prop == NULL)
    return NULL;

  if ((prop->type == PROP_TYPE_CHR || prop->type == PROP_TYPE_VIEWER) &&
      prop->chr != NULL) {
    return prop->chr->model;
  }
  if ((prop->type == PROP_TYPE_OBJ || prop->type == PROP_TYPE_WEAPON ||
       prop->type == PROP_TYPE_DOOR) &&
      prop->obj != NULL) {
    return prop->obj->model;
  }
  return NULL;
}

static bool mark_converted_prop_tree(PropRecord *prop,
                                     RenderPosView *render_pos, s32 depth) {
  PropRecord *child;
  Model *model;
  s32 child_guard = 0;

  if (prop == NULL || depth >= POS_DATA_ENTRY_LEN)
    return FALSE;

  model = get_live_prop_model(prop);
  if (model != NULL && model->render_pos == render_pos) {
    s32 prop_index = prop - pos_data_entry;
    if (prop_index >= 0 && prop_index < POS_DATA_ENTRY_LEN) {
      g_ConvertedPropMatrices[prop_index >> 3] |= 1 << (prop_index & 7);
      return TRUE;
    }
  }

  for (child = prop->child;
       child != NULL && child_guard++ < POS_DATA_ENTRY_LEN;
       child = child->prev) {
    if (mark_converted_prop_tree(child, render_pos, depth + 1))
      return TRUE;
  }
  return FALSE;
}

void practice_mark_converted_render_matrices(RenderPosView *render_pos) {
  s32 hand;
  PropRecord *prop;

  if (render_pos == NULL)
    return;

  for (hand = 0; hand < 2; hand++) {
    if ((RenderPosView *)g_CurrentPlayer->hands[hand].field_B74 == render_pos) {
      g_ConvertedHandMatrices |= 1 << hand;
      return;
    }
  }

  for (prop = get_ptr_obj_pos_list_current_entry(); prop != NULL;
       prop = prop->prev) {
    if (mark_converted_prop_tree(prop, render_pos, 0))
      return;
  }
}

bool practice_hand_render_matrices_are_fixed(s32 hand) {
  return hand >= 0 && hand < 2 &&
         (g_ConvertedHandMatrices & (1 << hand)) != 0;
}

bool practice_prop_render_matrices_are_fixed(PropRecord *prop) {
  s32 prop_index = prop != NULL ? prop - pos_data_entry : -1;

  return prop_index >= 0 && prop_index < POS_DATA_ENTRY_LEN &&
         (g_ConvertedPropMatrices[prop_index >> 3] &
          (1 << (prop_index & 7))) != 0;
}

void practice_clear_loaded_hand_matrices_float(void) {
  g_LoadedFloatHandMatrices = 0;
}

void practice_mark_loaded_hand_matrices_float(s32 hand) {
  if (hand >= 0 && hand < 2)
    g_LoadedFloatHandMatrices |= 1 << hand;
}

static void restore_converted_hand_matrices(void) {
  s32 hand;

  for (hand = 0; hand < 2; hand++) {
    struct hand *hand_state = &g_CurrentPlayer->hands[hand];
    ModelFileHeader *header = (ModelFileHeader *)hand_state->field_B70;

    if ((g_ConvertedHandMatrices & (1 << hand)) && header != NULL) {
      restore_matrices((RenderPosView *)hand_state->field_B74,
                       header->numMatrices);
    }
  }
}

static void clear_converted_render_matrices(void) {
  bzero(g_ConvertedPropMatrices, sizeof(g_ConvertedPropMatrices));
  g_ConvertedHandMatrices = 0;
}

#ifdef PRACTICE_TEST_ROM
static void assert_visible_clipped_door_vertices_persistent(void) {
  PropRecord *prop;

  for (prop = get_ptr_obj_pos_list_current_entry(); prop != NULL;
       prop = prop->prev) {
    DoorRecord *door;
    ModelNode *node;
    struct ModelRwData_DisplayList_CollisionRecord *rwdata;

    if (!(prop->flags & PROPFLAG_ONSCREEN) ||
        prop->type != PROP_TYPE_DOOR || prop->door == NULL ||
        !(prop->door->doorFlags & DOORFLAG_0004) ||
        prop->door->unkcc == NULL || prop->door->model == NULL ||
        prop->door->model->obj == NULL) {
      continue;
    }

    door = prop->door;
    node = door->model->obj->RootNode->Child->Child;
    rwdata = (struct ModelRwData_DisplayList_CollisionRecord *)
        modelGetNodeRwData(door->model, node);

    if (rwdata->Vertices != door->unkcc) {
      emu_log(
          "PAUSED_DOOR_RETAINED_FRAME_VERTICES prop=%d vertices=%x cache=%x",
          prop - pos_data_entry, rwdata->Vertices, door->unkcc);
      emu_log("TEST_FAILED");
      assert(FALSE);
    }
  }
}
#endif

void practice_begin_live_render(void) {
  /* Gameplay/model ticks rebuild float matrices before each live render, so
   * only conversions from this render can describe the buffers a following
   * paused frame inherits. */
  clear_converted_render_matrices();
  g_HasPausedFramebuffer = FALSE;
  g_PracticeUiBackgroundRectCount = 0;
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

  for (i = 0; i < 2; i++) {
    PropRecord *held = chr->weapons_held[i];

    if (held != NULL && held->obj != NULL)
      initialize_model_matrices(held->obj->model);
  }

  if (chr->handle_positiondata_hat != NULL &&
      chr->handle_positiondata_hat->obj != NULL) {
    initialize_model_matrices(chr->handle_positiondata_hat->obj->model);
  }
}

static bool prop_or_ancestor_is_onscreen(PropRecord *prop) {
  s32 guard = 0;

  while (prop != NULL && guard++ < POS_DATA_ENTRY_LEN) {
    if (prop->flags & PROPFLAG_ONSCREEN)
      return TRUE;
    prop = prop->parent;
  }

  return FALSE;
}

static void initialize_visible_object_tree(PropRecord *prop, s32 depth) {
  PropRecord *child;
  s32 child_guard = 0;

  if (prop == NULL || depth >= POS_DATA_ENTRY_LEN)
    return;

  if ((prop->type == PROP_TYPE_OBJ || prop->type == PROP_TYPE_WEAPON ||
       prop->type == PROP_TYPE_DOOR) &&
      prop->obj != NULL && prop->obj->model != NULL &&
      prop_or_ancestor_is_onscreen(prop)) {
    initialize_model_matrices(prop->obj->model);
  }

  for (child = prop->child;
       child != NULL && child_guard++ < POS_DATA_ENTRY_LEN;
       child = child->prev) {
    initialize_visible_object_tree(child, depth + 1);
  }
}

static void refresh_object_render_state(PropRecord *prop) {
  object_interaction(prop);

  if (prop->type == PROP_TYPE_DOOR &&
      (prop->door->doorFlags & DOORFLAG_0004)) {
    DoorRecord *door = prop->door;
    Model *model = door->model;
    ModelNode *node = model->obj->RootNode->Child->Child;
    struct ModelRoData_DisplayList_CollisionRecord *rodata =
        (struct ModelRoData_DisplayList_CollisionRecord *)node->Data;
    struct ModelRwData_DisplayList_CollisionRecord *rwdata =
        (struct ModelRwData_DisplayList_CollisionRecord *)
            modelGetNodeRwData(model, node);
    s32 vertex;

    /* object_interaction's render-only path rebuilds door matrices but skips
     * the clipped vertex buffer normally produced by the gameplay tick. The
     * paused arena is rewound, so that buffer must be current-frame data too.
     * Copy the result into the door's stage-pool cache immediately: otherwise
     * the next live stationary-door tick can read the retained arena pointer
     * after the arena has been reused, then preserve that corruption in
     * unkcc. */
    sub_GAME_7F052D8C(door);

    for (vertex = 0; vertex < rodata->numVertices; vertex++) {
      door->unkcc[vertex] = rwdata->Vertices[vertex];
    }
    rwdata->Vertices = door->unkcc;
  }
}

void practice_prepare_character_render(PracticeRenderContext *context) {
  PropRecord *prop;

  context->rendered_all_characters = FALSE;
  g_IsRenderOnly = TRUE;

  /* A render-only pass can change its visible set while culling. Preserve the
   * complete free list: limiting the snapshot to the initially visible model
   * estimate lets newly visible characters consume unsaved nodes and corrupt
   * the pool when it is restored. */
  save_joint_pool(context);
  save_model_render_positions(context, FALSE);
  prepare_freecam_prop_visibility(context);

  /* Attached child props are delisted, but a visible ancestor can still
   * render their models recursively. Give those models current-arena storage
   * without allocating matrices for every dormant object in the level. */
  for (prop = get_ptr_obj_pos_list_current_entry(); prop != NULL;
       prop = prop->prev) {
    initialize_visible_object_tree(prop, 0);
  }

  for (prop = get_ptr_obj_pos_list_current_entry(); prop != NULL;
       prop = prop->prev) {
    if ((prop->flags & PROPFLAG_ONSCREEN) &&
        (prop->type == PROP_TYPE_OBJ ||
         prop->type == PROP_TYPE_WEAPON ||
         prop->type == PROP_TYPE_DOOR) &&
        prop->obj != NULL && prop->obj->model != NULL) {
      refresh_object_render_state(prop);
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

#ifdef PRACTICE_TEST_ROM
  assert_visible_clipped_door_vertices_persistent();
#endif
}

void practice_prepare_refreshed_render(PracticeRenderContext *context) {
  PropRecord *prop;
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
        if (g_LoadedFloatHandMatrices & (1 << hand)) {
          matrix_4x4_copy(&old_render_pos[matrix].pos, &restored_matrix);
        } else {
          guMtxL2F(restored_matrix.m,
                   (Mtx *)&old_render_pos[matrix].pos);
        }
        matrix_4x4_copy(&restored_matrix,
                        &new_render_pos[matrix].pos);
      }

      hand_state->field_B74 = (s32)new_render_pos;
      g_LoadedFloatHandMatrices &= ~(1 << hand);
    }
  }

  /*
   * A load can replace models and their dynamic matrix buffers after the
   * normal tick has already been skipped for a paused frame.  In that case
   * there is no previous fixed-point render matrix to convert back to float.
   * Build fresh matrices from the restored state instead.
   */
  save_joint_pool(context);
  save_model_render_positions(context, FALSE);
  prepare_freecam_prop_visibility(context);

  for (prop = get_ptr_obj_pos_list_current_entry(); prop != NULL;
       prop = prop->prev) {
    initialize_visible_object_tree(prop, 0);
  }

  for (prop = get_ptr_obj_pos_list_current_entry(); prop != NULL;
       prop = prop->prev) {
    if ((prop->type == PROP_TYPE_OBJ ||
         prop->type == PROP_TYPE_WEAPON ||
         prop->type == PROP_TYPE_DOOR) &&
        prop->obj != NULL && prop->obj->model != NULL) {
      if (prop->flags & PROPFLAG_ONSCREEN) {
        refresh_object_render_state(prop);
      }
      continue;
    }

    if (!(prop->flags & PROPFLAG_ONSCREEN))
      continue;

    if ((prop->type == PROP_TYPE_CHR || prop->type == PROP_TYPE_VIEWER) &&
        prop->chr != NULL && prop->chr->model != NULL) {
      initialize_character_matrices(prop->chr);
      chrTickBeams(prop);
    }
  }

#ifdef PRACTICE_TEST_ROM
  assert_visible_clipped_door_vertices_persistent();
#endif

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

  /* The current display list still points at the fixed-point matrices emitted
   * by the paused render. Do not convert those buffers back to floats before
   * the RSP has consumed them; doing so stretches arbitrary prop triangles
   * across the frame. Restore only gameplay-owned pointers and root data here.
   * The first live tick rebuilds float matrices before its render. */
  restore_monitor_states(context);
  restore_model_root_data(context);
  if (!context->rendered_all_characters) {
    restore_model_render_positions(context);
  }
  restore_joint_pool(context);
  if (g_IsRenderStateInvalidated == TRUE) {
    /* Keep using refreshed matrices for subsequent held-pause renders until
     * the first live frame validates the reconstructed render state. */
    g_IsRenderStateInvalidated = 2;
  }
  /* Visibility is gameplay state. A render-only paused frame may calculate a
   * different visible set while rebuilding matrices, but it must not expose
   * that transient set to the next live tick. */
  restore_prop_visibility_state(context);
  g_CurrentPlayer->current_model_pos = context->current_model_pos;
  g_CurrentPlayer->previous_model_pos = context->previous_model_pos;
  g_CurrentPlayer->current_room_pos = context->current_room_pos;
  restore_watch_state(context);
  restore_freecam_player_state(context);
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
  /* A live alpha pass leaves every matrix it submits in packed fixed-point
   * form. Restore exactly those buffers before a zero-tick paused render tries
   * to reuse them as floats, including nested prop models. */
  restore_refreshed_render_matrices();
  restore_converted_hand_matrices();
  clear_converted_render_matrices();
}
