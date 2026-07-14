#include "practice_external_camera.h"
#include "bg.h"
#include "bondconstants.h"
#include "bondgame.h"
#include "bondtypes.h"
#include "bondview.h"
#include "chrai.h"
#include "chrlv.h"
#include "chrobjhandler.h"
#include "dyn.h"
#include "explosions.h"
#include "lvl.h"
#include "objecthandler.h"
#include "player.h"
#include "player_2.h"
#include "stan.h"
#include <fr.h>
#include <math.h>
#include <os_extension.h>
#include <ultra64.h>

#define PIP_SIZE 75
#define PIP_SPACING 4
#define PIP_DYN_VTX_RESERVE 0x1000
#define PIP_CROSSHAIR_RADIUS 3
#define PROP_STATE_BITMAP_SIZE ((POS_DATA_ENTRY_LEN + 7) / 8)

struct PracticeExternalCameraPropState {
  u8 onscreen[PROP_STATE_BITMAP_SIZE];
  u8 offscreen_patrol[PROP_STATE_BITMAP_SIZE];
  u8 has_been_seen[PROP_STATE_BITMAP_SIZE];
};

static struct PracticeExternalCameraView
    g_ExternalCameraViews[PRACTICE_EXTERNAL_CAMERA_MAX_VIEWS];
static s32 g_ExternalCameraViewCount;
static bool g_IsRenderingExternalCamera;

extern s32 z_buffer;
extern s32 z_buffer_width;
extern s32 g_ClockTimer;
extern u8 g_ViBackIndex;
extern VideoSettings *g_ViBackData;
extern s32 g_viColorOutputMode;

#if defined(VERSION_EU)
extern void sub_GAME_7F0A46A0(Gfx **gdl, s32 arg1);
#else
extern void sub_GAME_7F0A4824(Gfx **gdl, s32 arg1);
#endif
extern Gfx *sub_GAME_7F0A2C44(Gfx *gdl);
extern void sub_GAME_7F0876C4(coord3d *cam_pos, coord3d *cam_look,
                              coord3d *cam_up);

extern coord3d flt_CODE_bss_80079940;
extern f32 flt_CODE_bss_8007994C;
extern coord3d flt_CODE_bss_80079950;
extern f32 flt_CODE_bss_8007995C;
extern coord3d flt_CODE_bss_80079960;
extern f32 flt_CODE_bss_8007996C;
extern coord3d flt_CODE_bss_80079970;
extern f32 flt_CODE_bss_8007997C;
extern f32 flt_CODE_bss_80079980;

s32 practice_external_camera_is_rendering(void) {
  return g_IsRenderingExternalCamera;
}

void practice_external_camera_begin_frame(void) {
  g_ExternalCameraViewCount = 0;
}

s32 practice_external_camera_add_view(
    const struct PracticeExternalCameraView *view) {
  if (view == NULL ||
      g_ExternalCameraViewCount >= PRACTICE_EXTERNAL_CAMERA_MAX_VIEWS) {
    return FALSE;
  }

  g_ExternalCameraViews[g_ExternalCameraViewCount++] = *view;
  return TRUE;
}

s32 practice_external_camera_make_npc_follow_view(
    ChrRecord *chr, const struct PracticeNpcFollowCameraParams *params,
    const coord3d *tracked_position, u32 flags, u32 border_color,
    struct PracticeExternalCameraView *view) {
  f32 heading;
  f32 forward_x;
  f32 forward_z;
  f32 right_x;
  f32 right_z;
  f32 horizontal_distance;
  coord3d target;
  StandTile *tracked_stan;

  if (chr == NULL || chr->prop == NULL || chr->model == NULL ||
      params == NULL || tracked_position == NULL || view == NULL ||
      params->distance <= 0.0f) {
    return FALSE;
  }

  heading = getsubroty(chr->model);
  forward_x = sinf(heading);
  forward_z = cosf(heading);
  right_x = cosf(heading);
  right_z = -sinf(heading);

  target = *tracked_position;
  target.x += right_x * params->offset.x + forward_x * params->offset.z;
  target.y += params->offset.y;
  target.z += right_z * params->offset.x + forward_z * params->offset.z;

  horizontal_distance = cosf(params->angle) * params->distance;
  view->position.x = target.x - forward_x * horizontal_distance;
  view->position.y = target.y + sinf(params->angle) * params->distance;
  view->position.z = target.z - forward_z * horizontal_distance;
  view->look.x = target.x - view->position.x;
  view->look.y = target.y - view->position.y;
  view->look.z = target.z - view->position.z;
  view->up.x = 0.0f;
  view->up.y = 1.0f;
  view->up.z = 0.0f;
  tracked_stan = chr->prop->stan;
  if (tracked_stan != NULL && (tracked_position->x != chr->prop->pos.x ||
                               tracked_position->z != chr->prop->pos.z)) {
    StandTile *walked_stan = tracked_stan;
    if (walkTilesBetweenPoints_NoCallback(&walked_stan, chr->prop->pos.x,
                                          chr->prop->pos.z, tracked_position->x,
                                          tracked_position->z)) {
      tracked_stan = walked_stan;
    }
  }
  view->stan = tracked_stan;
  view->tracked_prop = chr->prop;
  view->forced_object = NULL;
  view->flags = flags;
  view->border_color = border_color;
  return TRUE;
}

s32 practice_external_camera_add_npc_follow_view(
    ChrRecord *chr, const struct PracticeNpcFollowCameraParams *params,
    const coord3d *tracked_position, u32 flags, u32 border_color) {
  struct PracticeExternalCameraView view;

  if (!practice_external_camera_make_npc_follow_view(
          chr, params, tracked_position, flags, border_color, &view)) {
    return FALSE;
  }

  return practice_external_camera_add_view(&view);
}

// TODO: Get rid of this, let consumers choose the color directly
static u32 get_border_fill_color(u32 border_color) {
  if (border_color == PRACTICE_EXTERNAL_CAMERA_BORDER_RED) {
    return 0xF801F801;
  }
  if (border_color == PRACTICE_EXTERNAL_CAMERA_BORDER_GREEN) {
    return 0x07C107C1;
  }
  return 0xFFFFFFFF;
}

static void
save_prop_visibility_state(struct PracticeExternalCameraPropState *state) {
  ChrRecord *chr;
  PropRecord *prop;
  s32 byte;
  s32 i;
  u8 bit;

  for (byte = 0; byte < PROP_STATE_BITMAP_SIZE; byte++) {
    state->onscreen[byte] = 0;
    state->offscreen_patrol[byte] = 0;
    state->has_been_seen[byte] = 0;
  }

  for (i = 0; i < POS_DATA_ENTRY_LEN; i++) {
    prop = &pos_data_entry[i];
    byte = i >> 3;
    bit = 1 << (i & 7);

    if (prop->flags & PROPFLAG_ONSCREEN) {
      state->onscreen[byte] |= bit;
    }

    if (prop->type == PROP_TYPE_CHR && prop->chr != NULL) {
      chr = prop->chr;
      if (chr->hidden & CHRHIDDEN_OFFSCREEN_PATROL) {
        state->offscreen_patrol[byte] |= bit;
      }
      if (chr->chrflags & CHRFLAG_HAS_BEEN_ON_SCREEN) {
        state->has_been_seen[byte] |= bit;
      }
    }
  }
}

// TODO: Seems like there is a bug that a chr can never go off screen if they
// start the cam on screen
static void restore_prop_visibility_state(
    const struct PracticeExternalCameraPropState *state) {
  ChrRecord *chr;
  PropRecord *prop;
  s32 byte;
  s32 i;
  u8 bit;

  for (i = 0; i < POS_DATA_ENTRY_LEN; i++) {
    prop = &pos_data_entry[i];
    byte = i >> 3;
    bit = 1 << (i & 7);

    if (state->onscreen[byte] & bit) {
      prop->flags |= PROPFLAG_ONSCREEN;
    } else {
      prop->flags &= ~PROPFLAG_ONSCREEN;
    }

    if (prop->type == PROP_TYPE_CHR && prop->chr != NULL) {
      chr = prop->chr;
      if (state->offscreen_patrol[byte] & bit) {
        chr->hidden |= CHRHIDDEN_OFFSCREEN_PATROL;
      } else {
        chr->hidden &= ~CHRHIDDEN_OFFSCREEN_PATROL;
      }
      if (state->has_been_seen[byte] & bit) {
        chr->chrflags |= CHRFLAG_HAS_BEEN_ON_SCREEN;
      } else {
        chr->chrflags &= ~CHRFLAG_HAS_BEEN_ON_SCREEN;
      }
    }
  }

  // Keep the global list consistent with the restored real-camera flags.
  chraiUpdateOnscreenPropCount();
}

static void prepare_pip_props(void) {
  s32 saved_clock_timer = g_ClockTimer;
  struct player *saved_player_two = g_playerPointers[PLAYER_2];

  // The normal prop dispatcher also prepares each view's visibility, depth and
  // model matrices. Treat this as a zero-time second multiplayer view so its
  // retick/relist bookkeeping still runs without advancing simulation.
  //
  // Some dispatcher side paths iterate every reported player (for example,
  // prop sound attenuation). Give the synthetic view a valid player pointer
  // while it is reported, otherwise those paths dereference the empty P2 slot.
  g_playerPointers[PLAYER_2] = g_CurrentPlayer;
  g_ClockTimer = 0;

  determing_type_of_object_and_detection();
  chraiUpdateOnscreenPropCount();

  g_ClockTimer = saved_clock_timer;
  g_playerPointers[PLAYER_2] = saved_player_two;
}

Gfx *practice_external_camera_render(Gfx *gdl) {
  s32 i;
  s32 s;
  struct PracticeExternalCameraView *view;

  // Viewport layout variables
  s16 screen_w;
  s16 spacing;
  s16 top;
  s16 left;
  s16 marker_x;
  s16 marker_y;
  struct PropRecord *saved_tracked_props[PRACTICE_EXTERNAL_CAMERA_MAX_VIEWS];
  u8 saved_tracked_flags[PRACTICE_EXTERNAL_CAMERA_MAX_VIEWS];
  bool saved_tracked_flags_valid[PRACTICE_EXTERNAL_CAMERA_MAX_VIEWS];
  struct PracticeExternalCameraPropState saved_prop_state;
  bool preserve_gameplay_visibility = FALSE;
  struct ObjectRecord *render_item;
  struct PropRecord *render_item_prop;
  u32 saved_render_item_flags2;
  bool render_item_valid;

  // Saved player view/matrix state
  s16 saved_viewx;
  s16 saved_viewy;
  s16 saved_viewleft;
  s16 saved_viewtop;

  f32 saved_screenwidth;
  f32 saved_screenheight;
  f32 saved_halfwidth;
  f32 saved_halfheight;
  f32 saved_screenleft;
  f32 saved_screentop;
  f32 saved_scalex;
  f32 saved_scaley;

  f32 saved_fovy;
  f32 saved_aspect;
  f32 saved_perspnear;
  f32 saved_perspfovy;
  f32 saved_perspaspect;

  s32 saved_field_5C;
  s32 saved_field_60;
  s32 saved_field_64;
  s32 saved_field_68;

  coord3d saved_pos;
  coord3d saved_pos3;
  struct collision434 saved_field_488;
  StandTile *saved_room_pointer;
  coord3d saved_player_prop_pos;
  StandTile *saved_player_prop_stan;

  coord3d saved_current_model_pos;
  coord3d saved_previous_model_pos;
  coord3d saved_current_room_pos;
  s32 saved_curRoomIndex;

  Mtx *saved_field_10C4;
  Mtx *saved_field_10C8;
  Mtxf *saved_field_10CC;
  Mtxf *saved_field_10D4;
  s32 saved_field_10E0;
  s32 saved_field_10E4;
  Mtxf *saved_field_10E8;
  Mtxf *saved_field_10EC;

  coord3d saved_frustum_940;
  f32 saved_frustum_94C;
  coord3d saved_frustum_950;
  f32 saved_frustum_95C;
  coord3d saved_frustum_960;
  f32 saved_frustum_96C;
  coord3d saved_frustum_970;
  f32 saved_frustum_97C;
  f32 saved_frustum_980;

  Vp saved_vp;
  VideoSettings saved_vi;

  // Camera parameters
  coord3d cam_pos;
  coord3d cam_look;
  coord3d cam_up;
  StandTile *stan;

  if (g_ExternalCameraViewCount == 0 || g_CurrentPlayer == NULL ||
      g_CurrentPlayer->prop == NULL) {
    return gdl;
  }

  for (s = 0; s < g_ExternalCameraViewCount; s++) {
    saved_tracked_props[s] = g_ExternalCameraViews[s].tracked_prop;
    saved_tracked_flags_valid[s] = saved_tracked_props[s] != NULL;
    if (saved_tracked_flags_valid[s]) {
      saved_tracked_flags[s] = saved_tracked_props[s]->flags;
    }
    if (g_ExternalCameraViews[s].flags &
        PRACTICE_EXTERNAL_CAMERA_PRESERVE_GAMEPLAY_VISIBILITY) {
      preserve_gameplay_visibility = TRUE;
    }
  }

  // Save states
  saved_viewx = g_CurrentPlayer->viewx;
  saved_viewy = g_CurrentPlayer->viewy;
  saved_viewleft = g_CurrentPlayer->viewleft;
  saved_viewtop = g_CurrentPlayer->viewtop;

  saved_screenwidth = g_CurrentPlayer->c_screenwidth;
  saved_screenheight = g_CurrentPlayer->c_screenheight;
  saved_halfwidth = g_CurrentPlayer->c_halfwidth;
  saved_halfheight = g_CurrentPlayer->c_halfheight;
  saved_screenleft = g_CurrentPlayer->c_screenleft;
  saved_screentop = g_CurrentPlayer->c_screentop;
  saved_scalex = g_CurrentPlayer->c_scalex;
  saved_scaley = g_CurrentPlayer->c_scaley;

  saved_fovy = g_CurrentPlayer->fovy;
  saved_aspect = g_CurrentPlayer->aspect;
  saved_perspnear = g_CurrentPlayer->c_perspnear;
  saved_perspfovy = g_CurrentPlayer->c_perspfovy;
  saved_perspaspect = g_CurrentPlayer->c_perspaspect;

  saved_field_5C = g_CurrentPlayer->field_5C;
  saved_field_60 = g_CurrentPlayer->field_60;
  saved_field_64 = g_CurrentPlayer->field_64;
  saved_field_68 = g_CurrentPlayer->field_68;

  saved_pos = g_CurrentPlayer->pos;
  saved_pos3 = g_CurrentPlayer->pos3;
  saved_field_488 = g_CurrentPlayer->field_488;
  saved_room_pointer = g_CurrentPlayer->room_pointer;
  saved_player_prop_pos = g_CurrentPlayer->prop->pos;
  saved_player_prop_stan = g_CurrentPlayer->prop->stan;

  saved_current_model_pos = g_CurrentPlayer->current_model_pos;
  saved_previous_model_pos = g_CurrentPlayer->previous_model_pos;
  saved_current_room_pos = g_CurrentPlayer->current_room_pos;
  saved_curRoomIndex = g_CurrentPlayer->curRoomIndex;

  saved_field_10C4 = g_CurrentPlayer->field_10C4;
  saved_field_10C8 = g_CurrentPlayer->field_10C8;
  saved_field_10CC = g_CurrentPlayer->field_10CC;
  saved_field_10D4 = g_CurrentPlayer->field_10D4;
  saved_field_10E0 = g_CurrentPlayer->field_10E0;
  saved_field_10E4 = g_CurrentPlayer->field_10E4;
  saved_field_10E8 = g_CurrentPlayer->field_10E8;
  saved_field_10EC = g_CurrentPlayer->field_10EC;

  saved_frustum_940 = flt_CODE_bss_80079940;
  saved_frustum_94C = flt_CODE_bss_8007994C;
  saved_frustum_950 = flt_CODE_bss_80079950;
  saved_frustum_95C = flt_CODE_bss_8007995C;
  saved_frustum_960 = flt_CODE_bss_80079960;
  saved_frustum_96C = flt_CODE_bss_8007996C;
  saved_frustum_970 = flt_CODE_bss_80079970;
  saved_frustum_97C = flt_CODE_bss_8007997C;
  saved_frustum_980 = flt_CODE_bss_80079980;

  saved_vp = g_CurrentPlayer->viewports[g_ViBackIndex];
  saved_vi = *g_ViBackData;

  screen_w = viGetX();
  spacing = (screen_w - PIP_SIZE * g_ExternalCameraViewCount) /
            (g_ExternalCameraViewCount + 1);
  top = PIP_SPACING;

  for (s = 0; s < g_ExternalCameraViewCount; s++) {
    view = &g_ExternalCameraViews[s];

    // Re-rendering the scene consumes the same finite per-frame vertex pool as
    // the main view. Never start another pass if it would use the engine's
    // safety margin; effect renderers also stop allocating at this boundary.
    if (dynGetFreeVtx() < PIP_DYN_VTX_RESERVE)
      break;

    left = spacing + s * (PIP_SIZE + spacing);

    // 1. Clear Z-buffer inside scissor bounds
    gDPPipeSync(gdl++);
    gDPSetRenderMode(gdl++, G_RM_NOOP, G_RM_NOOP2);
    gDPSetColorImage(gdl++, G_IM_FMT_RGBA, G_IM_SIZ_16b, z_buffer_width,
                     OS_K0_TO_PHYSICAL(z_buffer));
    gDPSetCycleType(gdl++, G_CYC_FILL);
    gDPSetFillColor(gdl++,
                    (GPACK_ZDZ(G_MAXFBZ, 0) << 16 | GPACK_ZDZ(G_MAXFBZ, 0)));
    gDPSetScissor(gdl++, G_SC_NON_INTERLACE, left, top, left + PIP_SIZE,
                  top + PIP_SIZE);
    gDPFillRectangle(gdl++, left, top, left + PIP_SIZE - 1, top + PIP_SIZE - 1);
    gDPPipeSync(gdl++);

    // Restore color image
    if (g_viColorOutputMode != COLORMODE_32BIT) {
      gDPSetColorImage(gdl++, G_IM_FMT_RGBA, G_IM_SIZ_16b, g_ViBackData->bufx,
                       OS_K0_TO_PHYSICAL(g_ViBackData->framebuf));
    } else {
      gDPSetColorImage(gdl++, G_IM_FMT_RGBA, G_IM_SIZ_32b, g_ViBackData->bufx,
                       OS_K0_TO_PHYSICAL(cfb_16[0]));
    }

    // 2. Set viewport & projection
    g_CurrentPlayer->viewx = PIP_SIZE;
    g_CurrentPlayer->viewy = PIP_SIZE;
    g_CurrentPlayer->viewleft = left;
    g_CurrentPlayer->viewtop = top;
    g_CurrentPlayer->fovy = 60.0f;
    g_CurrentPlayer->aspect = 1.0f;

    currentPlayerSetScreenSize(PIP_SIZE, PIP_SIZE);
    currentPlayerSetScreenPosition(left, top);
    currentPlayerSetPerspective(g_ViBackData->znear, 60.0f, 1.0f);
    currentPlayerSetCameraScale();

    viSetViewSize(PIP_SIZE, PIP_SIZE);
    viSetViewPosition(left, top);
    viSetFovY(60.0f);
    viSetAspect(1.0f);

    gdl = video_related_F(gdl);

    // Load a dynamically allocated viewport in g_VtxBuffers (flushed to
    // physical RAM) because the player struct is in stage BSS/heap and CPU
    // modifications to it are not coherent with the RSP's physical memory
    // reads.
    {
      Vp *vp = (Vp *)dynAllocate(sizeof(Vp));
      vp->vp.vscale[0] = (g_ViBackData->viewx * 2);
      vp->vp.vtrans[0] =
          (g_ViBackData->viewx * 2) + (g_ViBackData->viewleft * 4);
      vp->vp.vscale[1] = (g_ViBackData->viewy * 2);
      vp->vp.vtrans[1] =
          (g_ViBackData->viewy * 2) + (g_ViBackData->viewtop * 4);
      vp->vp.vscale[2] = 0x1FF;
      vp->vp.vtrans[2] = 0x1FF;
      vp->vp.vscale[3] = 0;
      vp->vp.vtrans[3] = 0;
      osWritebackDCache(vp, sizeof(Vp));
      gSPViewport(gdl++, OS_K0_TO_PHYSICAL(vp));
    }

    gDPSetScissor(gdl++, G_SC_NON_INTERLACE, left, top, left + PIP_SIZE,
                  top + PIP_SIZE);

    // 3. Set camera transformation
    cam_pos = view->position;
    cam_look = view->look;
    cam_up = view->up;

    // The game's camera transform divides by the horizontal look magnitude.
    // Keep exact top-down views out of that gimbal lock.
    if (cam_look.x == 0.0f && cam_look.z == 0.0f) {
      cam_look.x = 0.001f;
    }

    if (cam_up.x == 0.0f && cam_up.y == 0.0f && cam_up.z == 0.0f) {
      cam_up.y = 1.0f;
    }

    stan = view->stan;
    if (stan == NULL)
      stan = saved_room_pointer;
    g_CurrentPlayer->room_pointer = stan;
    g_CurrentPlayer->field_488.current_tile_ptr_for_portals = stan;
    g_CurrentPlayer->pos = cam_pos;
    g_CurrentPlayer->pos3 = cam_pos;
    g_CurrentPlayer->field_488.pos = cam_pos;
    g_CurrentPlayer->field_488.pos3 = cam_pos;

    // A few prop render paths read the player prop directly instead of using
    // bondviewGetCurrentPlayersPosition. Make those paths observe this PIP's
    // camera, then restore the real player prop after all PIPs are rendered.
    g_CurrentPlayer->prop->pos = cam_pos;
    g_CurrentPlayer->prop->stan = stan;

    sub_GAME_7F0876C4(&cam_pos, &cam_look, &cam_up);

    // 4. Render visual scene
    gdl = skyRender(gdl);
    bgRoomVisibilityRelated();

    // Features can force a small object through the PIP's visibility
    // preparation. Characters use the normal room visibility path.
    // Without this, distant small items are culled and model->render_pos keeps
    // pointing at an old dyn buffer which may already contain non-matrix data.
    render_item = view->forced_object;
    render_item_prop = render_item != NULL ? render_item->prop : NULL;
    render_item_valid = render_item != NULL && render_item_prop != NULL &&
                        render_item->model != NULL;
    if (render_item_valid) {
      saved_render_item_flags2 = render_item->flags2;
      render_item->flags2 |= PROPFLAG2_04000000;
    }

    if (view->flags & PRACTICE_EXTERNAL_CAMERA_PRESERVE_GAMEPLAY_VISIBILITY) {
      save_prop_visibility_state(&saved_prop_state);
    }

    g_IsRenderingExternalCamera = TRUE;
    prepare_pip_props();

    if (render_item_valid) {
      render_item->flags2 = saved_render_item_flags2;
    }

    // Prop preparation marks objects visible in this synthetic view. The
    // thrown-item physics uses PROPFLAG_ONSCREEN to decide whether to test
    // object and door collisions, so restore the real-view flags below. This
    // preserves tricks which intentionally unload a door by looking away.
    gdl = bgLevelRender_practice(gdl);
    gdl = sub_GAME_7F049B58(gdl);
#if defined(VERSION_EU)
    sub_GAME_7F0A46A0(&gdl, 1);
#else
    sub_GAME_7F0A4824(&gdl, 1);
#endif
    gdl = sub_GAME_7F0A2C44(gdl);
    gdl = explosionRenderFlyingParticles(gdl);
    g_IsRenderingExternalCamera = FALSE;

    if (view->flags & PRACTICE_EXTERNAL_CAMERA_PRESERVE_GAMEPLAY_VISIBILITY) {
      restore_prop_visibility_state(&saved_prop_state);
    }

    for (i = 0; i < g_ExternalCameraViewCount; i++) {
      if (saved_tracked_flags_valid[i] &&
          g_ExternalCameraViews[i].tracked_prop == saved_tracked_props[i]) {
        saved_tracked_props[i]->flags = saved_tracked_flags[i];
      }
    }

    // 5. Mark the tracked item's screen position with a small red crosshair,
    // then draw a white slot border.
    gDPPipeSync(gdl++);
    gDPSetRenderMode(gdl++, G_RM_NOOP, G_RM_NOOP2);
    gDPSetCycleType(gdl++, G_CYC_FILL);

    if (view->flags & PRACTICE_EXTERNAL_CAMERA_DRAW_CROSSHAIR) {
      gDPSetFillColor(gdl++, 0xF801F801);
      gDPSetScissor(gdl++, G_SC_NON_INTERLACE, left, top, left + PIP_SIZE,
                    top + PIP_SIZE);

      marker_x = left + PIP_SIZE / 2;
      marker_y = top + PIP_SIZE / 2;
      gDPFillRectangle(gdl++, marker_x - PIP_CROSSHAIR_RADIUS, marker_y,
                       marker_x + PIP_CROSSHAIR_RADIUS, marker_y);
      gDPFillRectangle(gdl++, marker_x, marker_y - PIP_CROSSHAIR_RADIUS,
                       marker_x, marker_y + PIP_CROSSHAIR_RADIUS);
    }

    gDPSetFillColor(gdl++, get_border_fill_color(view->border_color));
    gDPSetScissor(gdl++, G_SC_NON_INTERLACE, 0, 0, screen_w, viGetY());

    gDPFillRectangle(gdl++, left, top, left + PIP_SIZE - 1, top); // Top
    gDPFillRectangle(gdl++, left, top + PIP_SIZE - 1, left + PIP_SIZE - 1,
                     top + PIP_SIZE - 1);                         // Bottom
    gDPFillRectangle(gdl++, left, top, left, top + PIP_SIZE - 1); // Left
    gDPFillRectangle(gdl++, left + PIP_SIZE - 1, top, left + PIP_SIZE - 1,
                     top + PIP_SIZE - 1); // Right
    gDPPipeSync(gdl++);
  }

  // Restore player state
  g_CurrentPlayer->viewx = saved_viewx;
  g_CurrentPlayer->viewy = saved_viewy;
  g_CurrentPlayer->viewleft = saved_viewleft;
  g_CurrentPlayer->viewtop = saved_viewtop;

  g_CurrentPlayer->c_screenwidth = saved_screenwidth;
  g_CurrentPlayer->c_screenheight = saved_screenheight;
  g_CurrentPlayer->c_halfwidth = saved_halfwidth;
  g_CurrentPlayer->c_halfheight = saved_halfheight;
  g_CurrentPlayer->c_screenleft = saved_screenleft;
  g_CurrentPlayer->c_screentop = saved_screentop;
  g_CurrentPlayer->c_scalex = saved_scalex;
  g_CurrentPlayer->c_scaley = saved_scaley;

  g_CurrentPlayer->fovy = saved_fovy;
  g_CurrentPlayer->aspect = saved_aspect;
  g_CurrentPlayer->c_perspnear = saved_perspnear;
  g_CurrentPlayer->c_perspfovy = saved_perspfovy;
  g_CurrentPlayer->c_perspaspect = saved_perspaspect;

  g_CurrentPlayer->field_5C = saved_field_5C;
  g_CurrentPlayer->field_60 = saved_field_60;
  g_CurrentPlayer->field_64 = saved_field_64;
  g_CurrentPlayer->field_68 = saved_field_68;

  g_CurrentPlayer->pos = saved_pos;
  g_CurrentPlayer->pos3 = saved_pos3;
  g_CurrentPlayer->field_488 = saved_field_488;
  g_CurrentPlayer->room_pointer = saved_room_pointer;
  g_CurrentPlayer->prop->pos = saved_player_prop_pos;
  g_CurrentPlayer->prop->stan = saved_player_prop_stan;

  g_CurrentPlayer->current_model_pos = saved_current_model_pos;
  g_CurrentPlayer->previous_model_pos = saved_previous_model_pos;
  g_CurrentPlayer->current_room_pos = saved_current_room_pos;
  g_CurrentPlayer->curRoomIndex = saved_curRoomIndex;

  g_CurrentPlayer->field_10C4 = saved_field_10C4;
  g_CurrentPlayer->field_10C8 = saved_field_10C8;
  g_CurrentPlayer->field_10CC = saved_field_10CC;
  g_CurrentPlayer->field_10D4 = saved_field_10D4;
  g_CurrentPlayer->field_10E0 = saved_field_10E0;
  g_CurrentPlayer->field_10E4 = saved_field_10E4;
  g_CurrentPlayer->field_10E8 = saved_field_10E8;
  g_CurrentPlayer->field_10EC = saved_field_10EC;

  flt_CODE_bss_80079940 = saved_frustum_940;
  flt_CODE_bss_8007994C = saved_frustum_94C;
  flt_CODE_bss_80079950 = saved_frustum_950;
  flt_CODE_bss_8007995C = saved_frustum_95C;
  flt_CODE_bss_80079960 = saved_frustum_960;
  flt_CODE_bss_8007996C = saved_frustum_96C;
  flt_CODE_bss_80079970 = saved_frustum_970;
  flt_CODE_bss_8007997C = saved_frustum_97C;
  flt_CODE_bss_80079980 = saved_frustum_980;

  g_CurrentPlayer->viewports[g_ViBackIndex] = saved_vp;
  osWritebackDCache(&g_CurrentPlayer->viewports[g_ViBackIndex], sizeof(Vp));
  *g_ViBackData = saved_vi;

  currentPlayerSetScreenSize(saved_screenwidth, saved_screenheight);
  currentPlayerSetScreenPosition(saved_screenleft, saved_screentop);
  currentPlayerSetPerspective(saved_perspnear, saved_perspfovy,
                              saved_perspaspect);
  currentPlayerSetCameraScale();

  gdl = video_related_F(gdl);

  // bgRoomVisibilityRelated above leaves the last PIP's rooms marked visible.
  // Rebuild them from Bond's restored camera so the next gameplay tick makes
  // the same loaded/off-screen decisions it would make without the PIPs.
  if (preserve_gameplay_visibility) {
    bgRoomVisibilityRelated();
  }

  return gdl;
}
