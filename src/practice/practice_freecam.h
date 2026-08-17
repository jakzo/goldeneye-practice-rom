#ifndef PRACTICE_FREECAM_H
#define PRACTICE_FREECAM_H

#include <ultra64.h>
#include <bondtypes.h>

s32 practice_freecam_enable(s32 controller);
void practice_freecam_disable(void);
void practice_freecam_reset(void);
s32 practice_freecam_is_active(void);
void practice_freecam_age_rooms(void);
void practice_freecam_tick(u16 hotkey_trigger);
s32 practice_freecam_apply_camera(void);
s32 practice_freecam_get_render_context(u8 *room, coord3d **position,
                                        StandTile **tile);

#endif /* PRACTICE_FREECAM_H */
