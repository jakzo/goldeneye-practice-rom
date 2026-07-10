#ifndef _PRACTICE_GRENADE_CAM_H_
#define _PRACTICE_GRENADE_CAM_H_

#include <ultra64.h>

void practice_grenade_cam_tick(void);
void practice_grenade_cam_refresh(void);
Gfx *practice_grenade_cam_render(Gfx *gdl);
s32 practice_grenade_cam_is_rendering(void);

#endif
