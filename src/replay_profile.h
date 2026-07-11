#ifndef REPLAY_PROFILE_H
#define REPLAY_PROFILE_H
#include <ultra64.h>
typedef enum ReplayProfileArea {
    REPLAY_PROFILE_TICK,
    REPLAY_PROFILE_PHYSICS,
    REPLAY_PROFILE_RENDER,
    REPLAY_PROFILE_PHYSICS_SETUP,
    REPLAY_PROFILE_PHYSICS_CORE,
    REPLAY_PROFILE_PHYSICS_PRACTICE,
    REPLAY_PROFILE_RENDER_SETUP,
    REPLAY_PROFILE_RENDER_VISIBILITY,
    REPLAY_PROFILE_RENDER_BG,
    REPLAY_PROFILE_RENDER_ACTORS,
    REPLAY_PROFILE_RENDER_HUD,
    REPLAY_PROFILE_RENDER_PRACTICE,
    REPLAY_PROFILE_AREA_COUNT
} ReplayProfileArea;
void replay_profile_init(void);
void replay_profile_before_stage_load(s32 stage);
void replay_profile_on_stage_load(void);
s32 replay_profile_override_delta(s32 delta_frames);
void replay_profile_frame_start(s32 delta_frames);
void replay_profile_begin(ReplayProfileArea area);
void replay_profile_end(ReplayProfileArea area);
void replay_profile_frame_end(void);
#endif
