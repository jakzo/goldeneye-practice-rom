#ifndef _PRACTICE_GRENADE_CAM_H_
#define _PRACTICE_GRENADE_CAM_H_

#include "state/practice_states_stream.h"
#include <bondtypes.h>

void practice_grenade_cam_tick(void);
void practice_grenade_cam_refresh(void);
void practice_grenade_cam_reset(void);
void practice_grenade_cam_save_state(StateStream *stream);
bool practice_grenade_cam_load_state(StateStream *stream);

#endif
