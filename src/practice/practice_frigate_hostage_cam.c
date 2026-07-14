#include "practice_frigate_hostage_cam.h"
#include "boss.h"
#include "chr.h"
#include "chrai.h"
#include "chrlv.h"
#include "game/objecthandler.h"
#include "game/textrelated.h"
#include "practice_config.h"
#include "practice_external_camera.h"
#include "stan.h"
#include <bondconstants.h>
#include <bondtypes.h>
#include <math.h>
#include <ultra64.h>

#define FRIGATE_HOSTAGE_AI_LIST_ID 0x0402
#define FRIGATE_HOSTAGE_ESCAPE_AI_LIST_ID 0x0404
// Offset 38 is the yield inside Frigate's post-rescue hostage loop.
#define FRIGATE_HOSTAGE_FREED_AI_OFFSET 38
#define FRIGATE_HOSTAGE_REACHED_AI_OFFSET 91
#define FRIGATE_HOSTAGE_ESCAPED_AI_OFFSET 224
#define FRIGATE_HOSTAGE_REACHED_DISTANCE 500.0f
#define FRIGATE_HOSTAGE_MAX_CAMERAS 2
#define HOSTAGE_PROGRESS_X 5
#define HOSTAGE_PROGRESS_Y 12
#define HOSTAGE_PROGRESS_LINE_HEIGHT 10
#define HOSTAGE_PROGRESS_GREEN 0x00FF00FF
#define HOSTAGE_PROGRESS_RED 0xFF3030FF
#define HOSTAGE_PROGRESS_OUTLINE 0x000000FF
#define HOSTAGE_PROGRESS_TEXT_MAX 32
#define ARRAY_COUNT(a) (sizeof(a) / sizeof((a)[0]))

static const s16 s_FrigateHostageChrIds[] = {44, 45, 48, 49, 52, 53};
static const s16 s_FrigateHostageEscapePadIds[] = {143, 145, 147,
                                                   148, 168, 169};

typedef enum FrigateHostageTerminalReason {
  FRIGATE_HOSTAGE_ACTIVE,
  FRIGATE_HOSTAGE_NO_PATH,
  FRIGATE_HOSTAGE_STOPPED,
  FRIGATE_HOSTAGE_DIED,
} FrigateHostageTerminalReason;

typedef struct FrigateHostageProgress {
  s16 pad_id;
  u8 order;
  u8 terminal_reason;
  u8 route_started;
} FrigateHostageProgress;

static FrigateHostageProgress
    s_FrigateHostageProgress[ARRAY_COUNT(s_FrigateHostageChrIds)];
static u8 s_FrigateHostageFreedCount;
static s32 s_FrigateHostageLastTimer;

static const struct PracticeNpcFollowCameraParams s_FrigateHostageCamera = {
    .distance = 190.0f,
    .offset = {0.0f, 95.0f, 0.0f},
    .angle = 0.16f,
};

static s32 is_freed_living_hostage(ChrRecord *hostage) {
  return hostage != NULL && hostage->prop != NULL &&
         hostage->aioffset >= FRIGATE_HOSTAGE_FREED_AI_OFFSET &&
         !(hostage->hidden & CHRHIDDEN_REMOVE);
}

extern f32 chrlvModelScaleAnimationRelated(ChrRecord *self);
extern s32 g_GlobalTimer;
extern int sprintf(char *dst, const char *fmt, ...);

static void reset_hostage_progress(void) {
  s32 i;

  for (i = 0; i < (s32)ARRAY_COUNT(s_FrigateHostageProgress); i++) {
    s_FrigateHostageProgress[i].pad_id = -1;
    s_FrigateHostageProgress[i].order = 0;
    s_FrigateHostageProgress[i].terminal_reason = FRIGATE_HOSTAGE_ACTIVE;
    s_FrigateHostageProgress[i].route_started = FALSE;
  }

  s_FrigateHostageFreedCount = 0;
}

static s32 hostage_pad_number(s16 pad_id) {
  s32 i;

  for (i = 0; i < (s32)ARRAY_COUNT(s_FrigateHostageEscapePadIds); i++) {
    if (s_FrigateHostageEscapePadIds[i] == pad_id) {
      return i + 1;
    }
  }

  return 0;
}

static f32 horizontal_distance(const coord3d *from, const coord3d *to) {
  f32 dx = to->x - from->x;
  f32 dz = to->z - from->z;
  return sqrtf(dx * dx + dz * dz);
}

static void adjust_virtual_position_y(ChrRecord *hostage,
                                      coord3d *virtual_position) {
  PropRecord *prop = hostage->prop;
  StandTile *virtual_stan;
  f32 prop_floor_y;
  f32 prop_floor_offset;

  if (prop->stan == NULL) {
    virtual_position->y = prop->pos.y;
    return;
  }

  virtual_stan = prop->stan;
  if (!walkTilesBetweenPoints_NoCallback(
          &virtual_stan, prop->pos.x, prop->pos.z, virtual_position->x,
          virtual_position->z)) {
    virtual_position->y = prop->pos.y;
    return;
  }

  prop_floor_y = stanGetPositionYValue(prop->stan, prop->pos.x, prop->pos.z);
  prop_floor_offset = prop->pos.y - prop_floor_y;
  virtual_position->y =
      stanGetPositionYValue(virtual_stan, virtual_position->x,
                            virtual_position->z) +
      prop_floor_offset;
}

static s32 hostage_route_targets_selected_pad(ChrRecord *hostage,
                                              s16 pad_id) {
  const coord3d *pad_position;

  if (hostage->actiontype != ACT_GOPOS || pad_id < 0) {
    return FALSE;
  }

  pad_position = &g_CurrentSetup.pads[pad_id].pos;
  return horizontal_distance(&hostage->act_gopos.targetpos, pad_position) <
         1.0f;
}

static const coord3d *hostage_waypoint_position(ChrRecord *hostage,
                                                s32 index) {
  waypoint *route_waypoint = hostage->act_gopos.waypoints[index];

  if (route_waypoint == NULL) {
    return NULL;
  }

  return &g_CurrentSetup.pads[route_waypoint->padID].pos;
}

/*
 * Estimate the remaining route length from the active waypoint queue. Magic
 * movement leaves the prop at the start of its current segment, so use the
 * engine's interpolated segment distance for that first leg.
 */
static f32 hostage_route_distance_remaining(ChrRecord *hostage) {
  const coord3d *from;
  const coord3d *to;
  f32 distance = 0.0f;
  s32 index;
  s32 appended_target = FALSE;

  if (hostage->actiontype != ACT_GOPOS || hostage->prop == NULL) {
    return -1.0f;
  }

  index = hostage->act_gopos.curindex;
  if (index >= MAX_CHRWAYPOINTS) {
    return -1.0f;
  }

  if (hostage->act_gopos.waydata.mode == WAYMODE_MAGIC) {
    f32 segment_remaining = hostage->act_gopos.waydata.segdisttotal -
                            hostage->act_gopos.waydata.segdistdone;
    if (segment_remaining > 0.0f) {
      distance += segment_remaining;
    }

    from = hostage_waypoint_position(hostage, index);
    if (from == NULL) {
      from = &hostage->act_gopos.targetpos;
      appended_target = TRUE;
    }
    index++;
  } else {
    from = &hostage->prop->pos;
  }

  for (; index < MAX_CHRWAYPOINTS; index++) {
    to = hostage_waypoint_position(hostage, index);
    if (to == NULL) {
      if (!appended_target) {
        distance += horizontal_distance(from, &hostage->act_gopos.targetpos);
        appended_target = TRUE;
      }
      break;
    }

    distance += horizontal_distance(from, to);
    from = to;
  }

  if (!appended_target) {
    distance += horizontal_distance(from, &hostage->act_gopos.targetpos);
  }

  return distance;
}

static f32 hostage_route_seconds_remaining(ChrRecord *hostage) {
  f32 distance = hostage_route_distance_remaining(hostage);
  f32 distance_per_tick;

  if (distance < 0.0f || hostage->model == NULL) {
    return -1.0f;
  }

  distance_per_tick = chrlvModelScaleAnimationRelated(hostage) *
                      modelGetAbsAnimSpeed(hostage->model);
  if (distance_per_tick <= 0.0f) {
    return -1.0f;
  }

  return distance / distance_per_tick / CHRLV_FRAMERATE_F;
}

static void update_hostage_progress(ChrRecord *hostage, s32 index,
                                    AIRecord *escape_ai) {
  FrigateHostageProgress *progress = &s_FrigateHostageProgress[index];

  if (hostage == NULL || hostage->prop == NULL) {
    if (progress->order != 0 &&
        progress->terminal_reason == FRIGATE_HOSTAGE_ACTIVE) {
      progress->terminal_reason = FRIGATE_HOSTAGE_DIED;
    }
    return;
  }

  if (hostage->ailist != escape_ai) {
    return;
  }

  if (progress->order == 0) {
    progress->order = ++s_FrigateHostageFreedCount;
  }

  if (hostage_pad_number(hostage->padpreset1) != 0 &&
      progress->pad_id != hostage->padpreset1) {
    progress->pad_id = hostage->padpreset1;
    progress->route_started = FALSE;
  }

  if (hostage_route_targets_selected_pad(hostage, progress->pad_id)) {
    progress->route_started = TRUE;
  }

  if (hostage->aioffset >= FRIGATE_HOSTAGE_ESCAPED_AI_OFFSET) {
    progress->terminal_reason = progress->route_started
                                    ? FRIGATE_HOSTAGE_STOPPED
                                    : FRIGATE_HOSTAGE_NO_PATH;
  } else if (chrIsDead(hostage)) {
    progress->terminal_reason = FRIGATE_HOSTAGE_DIED;
  }
}

void practice_frigate_hostage_cam_tick(void) {
  AIRecord *escape_ai;
  ChrRecord *hostage;
  coord3d virtual_position;
  s32 camera_count = 0;
  s32 i;

  if (bossGetStageNum() != LEVELID_FRIGATE) {
    reset_hostage_progress();
    s_FrigateHostageLastTimer = 0;
    return;
  }

  if (g_GlobalTimer < s_FrigateHostageLastTimer) {
    reset_hostage_progress();
  }
  s_FrigateHostageLastTimer = g_GlobalTimer;
  escape_ai = ailistFindById(FRIGATE_HOSTAGE_ESCAPE_AI_LIST_ID);

  for (i = 0; i < (s32)ARRAY_COUNT(s_FrigateHostageChrIds); i++) {
    hostage = chrFindByLiteralId(s_FrigateHostageChrIds[i]);
    update_hostage_progress(hostage, i, escape_ai);

    if (practice.frigate_hostage_cam &&
        camera_count < FRIGATE_HOSTAGE_MAX_CAMERAS &&
        is_freed_living_hostage(hostage)) {
      chrlvGetPatrolPercentOrPosition(hostage, &virtual_position);
      adjust_virtual_position_y(hostage, &virtual_position);
      if (practice_external_camera_add_npc_follow_view(
              hostage, &s_FrigateHostageCamera, &virtual_position,
              PRACTICE_EXTERNAL_CAMERA_PRESERVE_GAMEPLAY_VISIBILITY, 0)) {
        camera_count++;
      }
    }
  }
}

static FrigateHostageProgress *hostage_progress_for_order(s32 order,
                                                          s32 *index_out) {
  s32 i;

  for (i = 0; i < (s32)ARRAY_COUNT(s_FrigateHostageProgress); i++) {
    if (s_FrigateHostageProgress[i].order == order) {
      *index_out = i;
      return &s_FrigateHostageProgress[i];
    }
  }

  return NULL;
}

Gfx *practice_frigate_hostage_progress_render(Gfx *gdl) {
  FrigateHostageProgress *progress;
  ChrRecord *hostage;
  char text[HOSTAGE_PROGRESS_TEXT_MAX];
  const char *status;
  f32 seconds;
  f32 pad_distance;
  s32 hostage_index;
  s32 order;
  s32 pad_number;
  s32 x = HOSTAGE_PROGRESS_X;
  s32 y = HOSTAGE_PROGRESS_Y;
  u32 color;

  if (!practice.frigate_hostage_progress ||
      bossGetStageNum() != LEVELID_FRIGATE) {
    return gdl;
  }

  for (order = 1; order <= s_FrigateHostageFreedCount; order++) {
    progress = hostage_progress_for_order(order, &hostage_index);
    if (progress == NULL) {
      continue;
    }

    hostage = chrFindByLiteralId(s_FrigateHostageChrIds[hostage_index]);
    pad_number = hostage_pad_number(progress->pad_id);
    color = HOSTAGE_PROGRESS_RED;

    if (hostage != NULL && hostage->prop != NULL &&
        hostage->actiontype == ACT_GOPOS &&
        hostage->act_gopos.waydata.mode == WAYMODE_MAGIC) {
      color = HOSTAGE_PROGRESS_GREEN;
    }

    if (progress->terminal_reason == FRIGATE_HOSTAGE_NO_PATH) {
      status = "NO PATH";
      sprintf(text, "%d. PAD%d - %s", order, pad_number, status);
    } else if (progress->terminal_reason == FRIGATE_HOSTAGE_STOPPED) {
      status = "STOPPED";
      sprintf(text, "%d. PAD%d - %s", order, pad_number, status);
    } else if (progress->terminal_reason == FRIGATE_HOSTAGE_DIED) {
      status = "DEAD";
      sprintf(text, "%d. PAD%d - %s", order, pad_number, status);
    } else if (hostage == NULL || hostage->prop == NULL) {
      sprintf(text, "%d. PAD%d - GONE", order, pad_number);
    } else {
      pad_distance = progress->pad_id >= 0
                         ? chrGetDistanceToPad(hostage, progress->pad_id)
                         : -1.0f;
      if (hostage->aioffset == FRIGATE_HOSTAGE_REACHED_AI_OFFSET ||
          (pad_distance >= 0.0f &&
           pad_distance < FRIGATE_HOSTAGE_REACHED_DISTANCE)) {
        sprintf(text, "%d. PAD%d - REACHED", order, pad_number);
      } else {
        seconds = hostage_route_seconds_remaining(hostage);
        if (seconds >= 0.0f) {
          sprintf(text, "%d. PAD%d - %.1fs", order, pad_number, seconds);
        } else {
          sprintf(text, "%d. PAD%d - ROUTING", order, pad_number);
        }
      }
    }

    gdl = textRenderGlow(gdl, &x, &y, (s8 *)text, ptrFontBankGothicChars,
                         ptrFontBankGothic, color, HOSTAGE_PROGRESS_OUTLINE,
                         viGetX(), viGetY(), 0, 0);
    x = HOSTAGE_PROGRESS_X;
    y += HOSTAGE_PROGRESS_LINE_HEIGHT;
  }

  return gdl;
}
