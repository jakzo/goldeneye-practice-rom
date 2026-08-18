#ifndef PRACTICE_EXTERNAL_CAMERA_H
#define PRACTICE_EXTERNAL_CAMERA_H

#include <bondtypes.h>
#include <ultra64.h>

#define PRACTICE_EXTERNAL_CAMERA_MAX_VIEWS 2

enum PracticeExternalCameraFlags {
  PRACTICE_EXTERNAL_CAMERA_DRAW_CROSSHAIR = 1 << 0,
  // Render the view without making its rooms and props count as visible to
  // gameplay. In particular, NPCs retain their off-screen virtual movement.
  PRACTICE_EXTERNAL_CAMERA_PRESERVE_GAMEPLAY_VISIBILITY = 1 << 1
};

/* Packed gDPSetFillColor value: two identical RGBA5551 pixels. */
#define PRACTICE_FILL_COLOR(r, g, b)                                           \
  (((u32)GPACK_RGBA5551((r), (g), (b), 1) << 16) |                             \
   (u32)GPACK_RGBA5551((r), (g), (b), 1))

struct PracticeExternalCameraView {
  coord3d position;
  // The look and up vectors define the camera rotation basis.
  coord3d look;
  coord3d up;
  StandTile *stan;
  struct PropRecord *tracked_prop;
  struct ObjectRecord *forced_object;
  u32 flags;
  u32 border_color; /* PRACTICE_FILL_COLOR(r, g, b) */
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
s32 practice_external_camera_max_views(void);
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
void practice_external_camera_set_rendering(s32 rendering);
void practice_external_camera_prepare_props(s32 force_visible_frame);

#endif /* PRACTICE_EXTERNAL_CAMERA_H */
