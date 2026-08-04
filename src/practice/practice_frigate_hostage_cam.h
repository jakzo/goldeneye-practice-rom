#ifndef PRACTICE_FRIGATE_HOSTAGE_CAM_H
#define PRACTICE_FRIGATE_HOSTAGE_CAM_H

#include <ultra64.h>
#include <bondtypes.h>

u16 practice_frigate_hostage_escape_pad(ChrRecord *hostage, u16 pad_id);
void practice_frigate_hostage_cam_tick(void);
Gfx *practice_frigate_hostage_progress_render(Gfx *gdl);

#endif /* PRACTICE_FRIGATE_HOSTAGE_CAM_H */
