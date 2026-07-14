#include "practice_grenade_cam.h"
#include "bondconstants.h"
#include "bondtypes.h"
#include "bondview.h"
#include "chrlv.h"
#include "explosions.h"
#include "player.h"
#include "practice_config.h"
#include "practice_external_camera.h"
#include <fr.h>
#include <math.h>
#include <ultra64.h>

#define MAX_GRENADE_CAM_SLOTS 2
#define FREEZE_DURATION_SEC 5.0f
#define CAMERA_HEIGHT 300.0f
#define MIN_CAMERA_HEIGHT 300.0f
#define MAX_CAMERA_HEIGHT 700.0f
#define MAX_CAMERA_DIST 400.0f
#define MAX_CAMERA_DIST_SQR (MAX_CAMERA_DIST * MAX_CAMERA_DIST)

struct grenade_cam_slot {
  struct ObjectRecord *item_obj;
  coord3d item_last_pos;
  struct StandTile *item_last_stan;
  coord3d camera_pos;
  s32 freeze_ticks;
  bool active;
};

static struct grenade_cam_slot g_GrenadeCamSlots[MAX_GRENADE_CAM_SLOTS];

extern s32 g_ClockTimer;

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

static bool has_thrown_item_finished(struct ObjectRecord *obj) {
  if (obj->runtime_bitflags & RUNTIMEBITFLAG_EMBEDDED)
    return TRUE;

  // Mines, knives and other sticky items release their projectile after they
  // attach to level geometry. Grenades also release it when they come to rest,
  // but their camera should remain until the fuse finishes.
  return obj->obj != PROP_CHRGRENADE &&
         !(obj->runtime_bitflags & RUNTIMEBITFLAG_DEPOSIT);
}

static void submit_active_views(void) {
  s32 s;
  struct grenade_cam_slot *slot;
  struct PracticeExternalCameraView view;

  for (s = 0; s < MAX_GRENADE_CAM_SLOTS; s++) {
    slot = &g_GrenadeCamSlots[s];
    if (!slot->active)
      continue;

    view.position = slot->camera_pos;
    view.look.x = slot->item_last_pos.x - slot->camera_pos.x;
    view.look.y = slot->item_last_pos.y - slot->camera_pos.y;
    view.look.z = slot->item_last_pos.z - slot->camera_pos.z;
    if (view.look.x == 0.0f && view.look.z == 0.0f) {
      view.look.x = 0.001f;
    }
    view.up.x = 0.0f;
    view.up.y = 1.0f;
    view.up.z = 0.0f;
    view.stan = slot->item_last_stan;
    view.tracked_prop = slot->item_obj != NULL ? slot->item_obj->prop : NULL;
    view.forced_object = slot->item_obj;
    view.flags = PRACTICE_EXTERNAL_CAMERA_DRAW_CROSSHAIR |
                 PRACTICE_EXTERNAL_CAMERA_PRESERVE_GAMEPLAY_VISIBILITY;
    view.border_color = PRACTICE_EXTERNAL_CAMERA_BORDER_WHITE;
    practice_external_camera_add_view(&view);
  }
}

void practice_grenade_cam_tick(void) {
  s32 i;
  s32 s;
  struct PropRecord *player_prop = get_curplayer_positiondata();
  struct Projectile *p;
  struct grenade_cam_slot *slot;
  bool still_active;
  bool is_player_thrown_item;
  coord3d item_pos;
  f32 dx;
  f32 dz;
  f32 dist_sqr;
  f32 dist;
  f32 speed;

  if (g_CurrentPlayer == NULL || player_prop == NULL)
    return;

  if (!practice.grenade_cam) {
    for (s = 0; s < MAX_GRENADE_CAM_SLOTS; s++) {
      g_GrenadeCamSlots[s].active = FALSE;
      g_GrenadeCamSlots[s].item_obj = NULL;
    }
    return;
  }

  for (s = 0; s < MAX_GRENADE_CAM_SLOTS; s++) {
    slot = &g_GrenadeCamSlots[s];
    if (!slot->active)
      continue;

    if (slot->freeze_ticks > 0) {
      slot->freeze_ticks -= g_ClockTimer;
      if (slot->freeze_ticks <= 0) {
        slot->active = FALSE;
        slot->item_obj = NULL;
      }
    } else {
      still_active =
          slot->item_obj != NULL && slot->item_obj->prop != NULL &&
          is_supported_thrown_item(slot->item_obj) &&
          !has_thrown_item_finished(slot->item_obj) &&
          !(slot->item_obj->runtime_bitflags & RUNTIMEBITFLAG_REMOVE);

      if (still_active) {
        item_pos = slot->item_obj->prop->pos;
        slot->item_last_pos = item_pos;
        if (slot->item_obj->prop->stan != NULL) {
          slot->item_last_stan = slot->item_obj->prop->stan;
        }

        dx = slot->camera_pos.x - item_pos.x;
        dz = slot->camera_pos.z - item_pos.z;
        dist_sqr = dx * dx + dz * dz;
        if (dist_sqr > MAX_CAMERA_DIST_SQR) {
          dist = sqrtf(dist_sqr);
          slot->camera_pos.x = item_pos.x + dx / dist * MAX_CAMERA_DIST;
          slot->camera_pos.z = item_pos.z + dz / dist * MAX_CAMERA_DIST;
        }

        if (slot->camera_pos.y < item_pos.y + MIN_CAMERA_HEIGHT) {
          slot->camera_pos.y = item_pos.y + MIN_CAMERA_HEIGHT;
        } else if (slot->camera_pos.y > item_pos.y + MAX_CAMERA_HEIGHT) {
          slot->camera_pos.y = item_pos.y + MAX_CAMERA_HEIGHT;
        }
      } else {
        slot->freeze_ticks = (s32)(FREEZE_DURATION_SEC * CHRLV_FRAMERATE_F);
        slot->item_obj = NULL;
      }
    }
  }

  for (i = 0; i < PROJECTILES_ARR_MAX; i++) {
    p = &g_Projectiles[i];
    is_player_thrown_item =
        !(p->flags & PROJECTILEFLAG_FREE) && p->obj != NULL &&
        p->obj->prop != NULL && is_supported_thrown_item(p->obj) &&
        !has_thrown_item_finished(p->obj) && p->ownerprop == player_prop;
    if (!is_player_thrown_item)
      continue;

    for (s = 0; s < MAX_GRENADE_CAM_SLOTS; s++) {
      if (g_GrenadeCamSlots[s].active &&
          g_GrenadeCamSlots[s].item_obj == p->obj) {
        goto next_item;
      }
    }

    for (s = 0; s < MAX_GRENADE_CAM_SLOTS; s++) {
      slot = &g_GrenadeCamSlots[s];
      if (slot->active)
        continue;

      slot->active = TRUE;
      slot->item_obj = p->obj;
      slot->item_last_pos = p->obj->prop->pos;
      slot->item_last_stan = p->obj->prop->stan;
      slot->freeze_ticks = 0;

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

  submit_active_views();
}

void practice_grenade_cam_refresh(void) {
  s32 s;

  for (s = 0; s < MAX_GRENADE_CAM_SLOTS; s++) {
    g_GrenadeCamSlots[s].active = FALSE;
    g_GrenadeCamSlots[s].item_obj = NULL;
    g_GrenadeCamSlots[s].freeze_ticks = 0;
  }

  practice_grenade_cam_tick();
}
