#ifndef PRACTICE_FREECAM_H
#define PRACTICE_FREECAM_H

#include <ultra64.h>

s32 practice_freecam_enable(s32 controller);
void practice_freecam_disable(void);
void practice_freecam_reset(void);
s32 practice_freecam_is_active(void);
void practice_freecam_tick(u16 hotkey_trigger);
s32 practice_freecam_apply_camera(void);

#endif /* PRACTICE_FREECAM_H */
