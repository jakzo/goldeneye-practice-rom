#ifndef REPLAY_H
#define REPLAY_H

#include <ultra64.h>

void replay_init(void);
#ifdef REPLAY_PLAYBACK
s32 replay_get_stage(void);
#endif
void replay_before_stage_load(s32 stage);
void replay_on_stage_load(void);
#ifdef REPLAY_PLAYBACK
s32 replay_override_delta(s32 delta_frames);
void replay_frame_start(void);
#endif
#ifdef REPLAY_RECORD
void replay_init_profile(void);
s32 replay_sram_read(u32 offset, void *destination, u32 size);
s32 replay_sram_write(u32 offset, void *source, u32 size);
#endif

#endif
