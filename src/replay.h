#ifndef REPLAY_H
#define REPLAY_H

#include <ultra64.h>

void replay_init(void);
s32 replay_get_stage(void);
void replay_before_stage_load(s32 stage);
void replay_on_stage_load(void);
s32 replay_override_delta(s32 delta_frames);
void replay_frame_start(void);

#endif
