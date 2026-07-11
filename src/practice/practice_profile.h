#ifndef PRACTICE_PROFILE_H
#define PRACTICE_PROFILE_H

#include <ultra64.h>

typedef enum PracticeProfileArea {
  PRACTICE_PROFILE_TICK,
  PRACTICE_PROFILE_PHYSICS,
  PRACTICE_PROFILE_RENDER,
  PRACTICE_PROFILE_UI,
  PRACTICE_PROFILE_PHYSICS_SETUP,
  PRACTICE_PROFILE_PHYSICS_CORE,
  PRACTICE_PROFILE_PHYSICS_PRACTICE,
  PRACTICE_PROFILE_RENDER_SETUP,
  PRACTICE_PROFILE_RENDER_VISIBILITY,
  PRACTICE_PROFILE_RENDER_BG,
  PRACTICE_PROFILE_RENDER_ACTORS,
  PRACTICE_PROFILE_RENDER_HUD,
  PRACTICE_PROFILE_RENDER_PRACTICE,
  PRACTICE_PROFILE_AREA_COUNT
} PracticeProfileArea;

#ifdef PROFILE_PRACTICE
void practice_profile_set_enabled(s32 enabled);
void practice_profile_frame_start(s32 delta_frames);
void practice_profile_begin(PracticeProfileArea area);
void practice_profile_end(PracticeProfileArea area);
void practice_profile_frame_end(void);
#else
#define practice_profile_set_enabled(enabled) ((void)0)
#define practice_profile_frame_start(delta_frames) ((void)0)
#define practice_profile_begin(area) ((void)0)
#define practice_profile_end(area) ((void)0)
#define practice_profile_frame_end() ((void)0)
#endif

#endif
