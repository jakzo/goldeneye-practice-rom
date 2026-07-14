#ifndef PRACTICE_EXTERNAL_CAMERA_H
#define PRACTICE_EXTERNAL_CAMERA_H

#include <bondtypes.h>
#include <ultra64.h>

#define PRACTICE_EXTERNAL_CAMERA_MAX_VIEWS 3

enum PracticeExternalCameraFlags {
  PRACTICE_EXTERNAL_CAMERA_DRAW_CROSSHAIR = 1 << 0,
  // Render the view without making its rooms and props count as visible to
  // gameplay. In particular, NPCs retain their off-screen virtual movement.
  PRACTICE_EXTERNAL_CAMERA_PRESERVE_GAMEPLAY_VISIBILITY = 1 << 1
};

enum PracticeExternalCameraBorderColor {
  PRACTICE_EXTERNAL_CAMERA_BORDER_WHITE,
  PRACTICE_EXTERNAL_CAMERA_BORDER_RED,
  PRACTICE_EXTERNAL_CAMERA_BORDER_GREEN
};

struct PracticeExternalCameraView {
  coord3d position;
  // The look and up vectors define the camera rotation basis.
  coord3d look;
  coord3d up;
  StandTile *stan;
  struct PropRecord *tracked_prop;
  struct ObjectRecord *forced_object;
  u32 flags;
  u32 border_color;
};

struct PracticeNpcFollowCameraParams {
  // Distance from the offset target point to the camera.
  f32 distance;
  // Target offset in NPC-local coordinates: X right, Y up and Z forward.
  coord3d offset;
  // Camera elevation above the NPC's forward axis, in radians.
  f32 angle;
};

void practice_external_camera_begin_frame(void);
s32 practice_external_camera_add_view(
    const struct PracticeExternalCameraView *view);
s32 practice_external_camera_make_npc_follow_view(
    ChrRecord *chr, const struct PracticeNpcFollowCameraParams *params,
    const coord3d *tracked_position, u32 flags, u32 border_color,
    struct PracticeExternalCameraView *view);
s32 practice_external_camera_add_npc_follow_view(
    ChrRecord *chr, const struct PracticeNpcFollowCameraParams *params,
    const coord3d *tracked_position, u32 flags, u32 border_color);
Gfx *practice_external_camera_render(Gfx *gdl);
s32 practice_external_camera_is_rendering(void);

#endif /* PRACTICE_EXTERNAL_CAMERA_H */
