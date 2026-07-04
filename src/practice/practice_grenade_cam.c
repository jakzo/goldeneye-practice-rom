#include "practice_grenade_cam.h"
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
#include "player.h"
#include "player_2.h"
#include "practice_config.h"
#include "stan.h"
#include <fr.h>
#include <math.h>
#include <os_extension.h>
#include <ultra64.h>

// TODO: Cannot render more than 2 without game crashing
// TODO: Explosion and smoke textures get messed up after rendering pip
#define MAX_PIP_SLOTS 2
#define PIP_SIZE 75
#define PIP_SPACING 4
#define FREEZE_DURATION_SEC 5.0f
#define CAMERA_HEIGHT 300.0f
#define MIN_CAMERA_HEIGHT 225.0f
#define MAX_CAMERA_HEIGHT 450.0f
#define MAX_CAMERA_DIST 800.0f
#define MAX_CAMERA_DIST_SQR (MAX_CAMERA_DIST * MAX_CAMERA_DIST)
#define PIP_ITEM_SCALE 5.0f
#define PIP_MARKER_RADIUS 8

struct pip_slot {
  struct ObjectRecord *item_obj;
  coord3d item_last_pos;
  struct StandTile *item_last_stan;
  coord3d camera_pos;
  s32 freeze_ticks;
  bool active;
};

static struct pip_slot g_PipSlots[MAX_PIP_SLOTS];
static bool g_IsRenderingGrenadeCam;

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

s32 practice_grenade_cam_is_rendering(void) { return g_IsRenderingGrenadeCam; }

static bool is_supported_thrown_item(struct ObjectRecord *obj) {
  if (obj == NULL)
    return FALSE;

  switch (obj->obj) {
  case PROP_CHRGRENADE:
  case PROP_CHRKNIFE:
  case PROP_CHRREMOTEMINE:
  case PROP_CHRPROXIMITYMINE:
  case PROP_CHRTIMEDMINE:
  case PROP_CHRBOMBCASE:
  case PROP_CHRBUG:
  case PROP_CHRMICROCAMERA:
  case PROP_CHRGOLDENEYEKEY:
  case PROP_CHRPLASTIQUE:
    return TRUE;
  }

  return FALSE;
}

static void enlarge_item_render_model(struct ObjectRecord *obj) {
  Model *model;
  Mtxf *root;
  Mtxf *mtx;
  s32 i;
  s32 row;
  s32 col;

  if (obj == NULL || obj->model == NULL || obj->model->render_pos == NULL)
    return;

  model = obj->model;
  root = &model->render_pos[0].pos;

  for (i = 0; i < model->obj->numMatrices; i++) {
    mtx = &model->render_pos[i].pos;

    for (row = 0; row < 3; row++) {
      for (col = 0; col < 3; col++) {
        mtx->m[row][col] *= PIP_ITEM_SCALE;
      }
    }

    if (i != 0) {
      for (col = 0; col < 3; col++) {
        mtx->m[3][col] = root->m[3][col] +
                         (mtx->m[3][col] - root->m[3][col]) * PIP_ITEM_SCALE;
      }
    }
  }
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
  g_IsRenderingGrenadeCam = TRUE;
  g_ClockTimer = 0;

  determing_type_of_object_and_detection();
  chraiUpdateOnscreenPropCount();

  g_ClockTimer = saved_clock_timer;
  g_IsRenderingGrenadeCam = FALSE;
  g_playerPointers[PLAYER_2] = saved_player_two;
}

void practice_grenade_cam_tick(void) {
  s32 i, s;
  struct PropRecord *player_prop = get_curplayer_positiondata();
  struct Projectile *p;
  struct pip_slot *slot;
  bool still_active, is_player_thrown_item;
  coord3d g_pos;
  f32 dx, dz, dist_sqr, dist, speed;

  if (g_CurrentPlayer == NULL || player_prop == NULL)
    return;

  if (!practice.grenade_cam) {
    for (s = 0; s < MAX_PIP_SLOTS; s++) {
      g_PipSlots[s].active = FALSE;
      g_PipSlots[s].item_obj = NULL;
    }
    return;
  }

  // 1. Tick existing slots
  for (s = 0; s < MAX_PIP_SLOTS; s++) {
    slot = &g_PipSlots[s];
    if (!slot->active)
      continue;

    if (slot->freeze_ticks > 0) {
      // Freeze phase
      slot->freeze_ticks -= g_ClockTimer;
      if (slot->freeze_ticks <= 0) {
        slot->active = FALSE;
        slot->item_obj = NULL;
      }
    } else {
      // Tracking phase
      // Track the object rather than the Projectile. Projectile entries are
      // released when thrown items finish bouncing or stick to a surface.
      still_active =
          slot->item_obj != NULL && slot->item_obj->prop != NULL &&
          is_supported_thrown_item(slot->item_obj) &&
          !(slot->item_obj->runtime_bitflags & RUNTIMEBITFLAG_REMOVE);

      if (still_active) {
        g_pos = slot->item_obj->prop->pos;
        slot->item_last_pos = g_pos;
        if (slot->item_obj->prop->stan != NULL) {
          slot->item_last_stan = slot->item_obj->prop->stan;
        }

        // Camera follow logic
        dx = slot->camera_pos.x - g_pos.x;
        dz = slot->camera_pos.z - g_pos.z;
        dist_sqr = dx * dx + dz * dz;
        if (dist_sqr > MAX_CAMERA_DIST_SQR) {
          dist = sqrtf(dist_sqr);
          slot->camera_pos.x = g_pos.x + (dx / dist) * MAX_CAMERA_DIST;
          slot->camera_pos.z = g_pos.z + (dz / dist) * MAX_CAMERA_DIST;
        }

        if (slot->camera_pos.y < g_pos.y + MIN_CAMERA_HEIGHT) {
          slot->camera_pos.y = g_pos.y + MIN_CAMERA_HEIGHT;
        } else if (slot->camera_pos.y > g_pos.y + MAX_CAMERA_HEIGHT) {
          slot->camera_pos.y = g_pos.y + MAX_CAMERA_HEIGHT;
        }
      } else {
        // Start freeze phase
        slot->freeze_ticks = (s32)(FREEZE_DURATION_SEC * CHRLV_FRAMERATE_F);
        slot->item_obj = NULL;
      }
    }
  }

  // 2. Scan for new player-thrown items
  for (i = 0; i < PROJECTILES_ARR_MAX; i++) {
    p = &g_Projectiles[i];
    is_player_thrown_item = !(p->flags & PROJECTILEFLAG_FREE) &&
                            p->obj != NULL && p->obj->prop != NULL &&
                            is_supported_thrown_item(p->obj) &&
                            p->ownerprop == player_prop;
    if (!is_player_thrown_item)
      continue;

    // Check if already tracked
    for (s = 0; s < MAX_PIP_SLOTS; s++) {
      if (g_PipSlots[s].active && g_PipSlots[s].item_obj == p->obj)
        goto next_item;
    }

    // Find first available slot
    for (s = 0; s < MAX_PIP_SLOTS; s++) {
      slot = &g_PipSlots[s];
      if (slot->active)
        continue;

      slot->active = TRUE;
      slot->item_obj = p->obj;
      slot->item_last_pos = p->obj->prop->pos;
      slot->item_last_stan = p->obj->prop->stan;
      slot->freeze_ticks = 0;

      // Start behind the direction of travel. Starting directly above the
      // item put the model close to the near plane and produced an almost
      // vertical, poorly defined camera orientation.
      speed = sqrtf(p->speed.x * p->speed.x + p->speed.z * p->speed.z);
      if (speed > 0.001f) {
        slot->camera_pos.x =
            p->obj->prop->pos.x - p->speed.x / speed * MAX_CAMERA_DIST;
        slot->camera_pos.z =
            p->obj->prop->pos.z - p->speed.z / speed * MAX_CAMERA_DIST;
      } else {
        slot->camera_pos = *bondviewGetCurrentPlayersPosition();
      }
      slot->camera_pos.y = p->obj->prop->pos.y + CAMERA_HEIGHT;
      break;
    }
  next_item:;
  }
}

Gfx *practice_grenade_cam_render(Gfx *gdl) {
  s32 i;
  s32 s;
  s32 active_count = 0;
  struct pip_slot *slot;

  // Viewport layout variables
  s16 screen_w;
  s16 spacing;
  s16 top;
  s16 left;
  s16 marker_x;
  s16 marker_y;
  struct PropRecord *saved_item_props[MAX_PIP_SLOTS];
  u8 saved_item_flags[MAX_PIP_SLOTS];
  bool saved_item_flags_valid[MAX_PIP_SLOTS];
  rgba_u8 saved_item_shade;
  bool saved_item_shade_valid;
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
  coord3d target;
  coord3d cam_look;
  coord3d cam_up;
  StandTile *stan;

  for (s = 0; s < MAX_PIP_SLOTS; s++) {
    if (g_PipSlots[s].active) {
      active_count++;
    }

    saved_item_flags_valid[s] =
        g_PipSlots[s].item_obj != NULL && g_PipSlots[s].item_obj->prop != NULL;
    if (saved_item_flags_valid[s]) {
      saved_item_props[s] = g_PipSlots[s].item_obj->prop;
      saved_item_flags[s] = saved_item_props[s]->flags;
    }
  }

  if (active_count == 0) {
    return gdl;
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
  spacing = (screen_w - PIP_SIZE * MAX_PIP_SLOTS) / (MAX_PIP_SLOTS + 1);
  top = PIP_SPACING;

  for (s = 0; s < MAX_PIP_SLOTS; s++) {
    slot = &g_PipSlots[s];
    if (!slot->active)
      continue;

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
    cam_pos = slot->camera_pos;
    target = slot->item_last_pos;
    cam_look.x = target.x - cam_pos.x;
    cam_look.y = target.y - cam_pos.y;
    cam_look.z = target.z - cam_pos.z;

    if (cam_look.x == 0.0f && cam_look.z == 0.0f) {
      // Crashes with divide by 0 due to gimbal lock if we don't do this
      cam_look.x = 0.001f;
    }

    cam_up.x = 0.0f;
    cam_up.y = 1.0f;
    cam_up.z = 0.0f;

    stan = slot->item_last_stan;
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

    // Force only the tracked item through the PIP's visibility preparation.
    // Without this, distant small items are culled and model->render_pos keeps
    // pointing at an old dyn buffer which may already contain non-matrix data.
    render_item = slot->item_obj;
    render_item_prop = render_item != NULL ? render_item->prop : NULL;
    render_item_valid = render_item != NULL && render_item_prop != NULL &&
                        render_item->model != NULL;
    if (render_item_valid) {
      saved_render_item_flags2 = render_item->flags2;
      render_item->flags2 |= PROPFLAG2_04000000;
    }

    prepare_pip_props();

    if (render_item_valid) {
      render_item->flags2 = saved_render_item_flags2;
    }

    // Prop preparation marks objects visible in this synthetic view. The
    // thrown-item physics uses PROPFLAG_ONSCREEN to decide whether to test
    // object and door collisions, so restore the real-view flags below. This
    // preserves tricks which intentionally unload a door by looking away.
    render_item_valid =
        render_item_valid && render_item->prop == render_item_prop &&
        (render_item_prop->flags & PROPFLAG_ONSCREEN) &&
        render_item->model != NULL && render_item->model->render_pos != NULL;
    if (render_item_valid) {
      enlarge_item_render_model(render_item);
    }

    saved_item_shade_valid = render_item_valid;
    if (saved_item_shade_valid) {
      saved_item_shade = render_item->shadecol;
      render_item->shadecol.r = 0xff;
      render_item->shadecol.g = 0;
      render_item->shadecol.b = 0;
      render_item->shadecol.a = 0x80;
    }

    gdl = bgLevelRender_practice(gdl);
    gdl = sub_GAME_7F049B58(gdl);
#if defined(VERSION_EU)
    sub_GAME_7F0A46A0(&gdl, 1);
#else
    sub_GAME_7F0A4824(&gdl, 1);
#endif
    gdl = sub_GAME_7F0A2C44(gdl);
    gdl = explosionRenderFlyingParticles(gdl);

    if (saved_item_shade_valid) {
      render_item->shadecol = saved_item_shade;
    }

    for (i = 0; i < MAX_PIP_SLOTS; i++) {
      if (saved_item_flags_valid[i] && g_PipSlots[i].item_obj != NULL &&
          g_PipSlots[i].item_obj->prop == saved_item_props[i]) {
        saved_item_props[i]->flags = saved_item_flags[i];
      }
    }

    // 5. Draw an open red sight around the enlarged item, then a white slot
    // border. Keeping the centre clear avoids covering small models.
    gDPPipeSync(gdl++);
    gDPSetRenderMode(gdl++, G_RM_NOOP, G_RM_NOOP2);
    gDPSetCycleType(gdl++, G_CYC_FILL);
    gDPSetFillColor(gdl++, 0xF801F801);
    gDPSetScissor(gdl++, G_SC_NON_INTERLACE, left, top, left + PIP_SIZE,
                  top + PIP_SIZE);

    // marker_x = left + PIP_SIZE / 2;
    // marker_y = top + PIP_SIZE / 2;
    // gDPFillRectangle(gdl++, marker_x - PIP_MARKER_RADIUS,
    //                  marker_y - PIP_MARKER_RADIUS, marker_x +
    //                  PIP_MARKER_RADIUS, marker_y - PIP_MARKER_RADIUS);
    // gDPFillRectangle(gdl++, marker_x - PIP_MARKER_RADIUS,
    //                  marker_y + PIP_MARKER_RADIUS, marker_x +
    //                  PIP_MARKER_RADIUS, marker_y + PIP_MARKER_RADIUS);
    // gDPFillRectangle(gdl++, marker_x - PIP_MARKER_RADIUS,
    //                  marker_y - PIP_MARKER_RADIUS, marker_x -
    //                  PIP_MARKER_RADIUS, marker_y + PIP_MARKER_RADIUS);
    // gDPFillRectangle(gdl++, marker_x + PIP_MARKER_RADIUS,
    //                  marker_y - PIP_MARKER_RADIUS, marker_x +
    //                  PIP_MARKER_RADIUS, marker_y + PIP_MARKER_RADIUS);

    gDPSetFillColor(gdl++, 0xFFFFFFFF);
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

  return gdl;
}
