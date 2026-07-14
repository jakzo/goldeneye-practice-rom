#include "practice_dam_guard_cam.h"
#include "boss.h"
#include "chr.h"
#include "chrlv.h"
#include "practice_config.h"
#include "practice_external_camera.h"
#include <bondconstants.h>
#include <bondtypes.h>
#include <ultra64.h>

#define DAM_GATE_GUARD_CHR_ID 2

static const struct PracticeNpcFollowCameraParams s_DamGuardCamera = {
    .distance = 300.0f,
    .offset = {0.0f, 105.0f, 0.0f},
    .angle = 0.35f,
};

extern s32 g_BgCurrentRoom;

s32 practice_dam_gate_guard_area_active(void) {
  return bossGetStageNum() == LEVELID_DAM &&
         ((g_BgCurrentRoom >= 110 && g_BgCurrentRoom <= 111) ||
          (g_BgCurrentRoom >= 124 && g_BgCurrentRoom <= 131));
}

void practice_dam_guard_cam_tick(void) {
  ChrRecord *guard;
  coord3d virtual_position;

  if (!practice.dam_guard_cam || !practice_dam_gate_guard_area_active()) {
    return;
  }

  guard = chrFindByLiteralId(DAM_GATE_GUARD_CHR_ID);
  if (guard == NULL || guard->prop == NULL || guard->model == NULL) {
    return;
  }

  chrlvGetPatrolPercentOrPosition(guard, &virtual_position);
  practice_external_camera_add_npc_follow_view(
      guard, &s_DamGuardCamera, &virtual_position,
      PRACTICE_EXTERNAL_CAMERA_PRESERVE_GAMEPLAY_VISIBILITY, 0);
}
