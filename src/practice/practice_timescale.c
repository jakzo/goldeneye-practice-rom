#include "game/lvl.h"
#include "practice_music.h"
#include "practice_sfx.h"
#include "ultratypes.h"
#include <ultra64.h>

extern u64 g_randomSeed;
extern u64 g_chrObjRandomSeed;
extern s32 speedgraphframes;
extern void store_osgetcount(void);

f32 g_TimeScale = 1.0f;
f32 g_TimeScaleFinal = 1.0f;
f32 g_FractionalClockTimerAcc = 0.0f;
s32 g_IsTimeScaleChanged = FALSE;
s32 g_IsTimePaused = FALSE;

s32 g_TimeScaleDeltaFrames = 1;
u64 g_FrozenFrameRngSeed = 0;
u64 g_FrozenFrameChrObjRngSeed = 0;
s32 g_PrevFrameTimeScaleDropped = FALSE;
s32 g_ForcedDeltaFrames = -1;

void set_final_time_scale(f32 scale) {
  g_TimeScaleFinal = scale;
  g_IsTimeScaleChanged = scale != 1.0f;
}

void set_time_scale(f32 scale) {
  if (scale < 0.0f)
    scale = 0.0f;
  g_TimeScale = scale;
  if (!g_IsTimePaused)
    set_final_time_scale(scale);
}

void pause() {
  if (!g_IsTimePaused) {
    g_ForcedDeltaFrames = speedgraphframes;
    /* The pause hotkey is handled after the frame counter update. A paused
     * render can therefore run before restore_rng_if_frame_dropped gets a
     * chance to establish this frame's baseline. Snapshot it here so render
     * cleanup never restores the RNG state from an older paused frame. */
    g_FrozenFrameRngSeed = g_randomSeed;
    g_FrozenFrameChrObjRngSeed = g_chrObjRandomSeed;
    practice_sfx_pause();
  }
  g_IsTimePaused = TRUE;
  set_final_time_scale(0.0f);
  practice_music_pause();
}

void unpause() {
  practice_music_resume();
  practice_sfx_resume();
  if (g_IsTimePaused) {
    // Drop the wall-clock time spent paused so it isn't counted as elapsed
    // game frames (catch-up) when time resumes
    store_osgetcount();
  }
  g_IsTimePaused = FALSE;
  set_final_time_scale(g_TimeScale);
}

// Restores RNG while time is frozen by holding the hotkey trigger. Needed
// because rendering frames consumes RNG (eg. muzzle flash) even when no physics
// are happening. Call this at the very top of the per-frame tick, before any
// RNG is consumed.
void restore_rng_if_frame_dropped(void) {
  if (g_PrevFrameTimeScaleDropped) {
    g_randomSeed = g_FrozenFrameRngSeed;
    g_chrObjRandomSeed = g_FrozenFrameChrObjRngSeed;
  } else {
    g_FrozenFrameRngSeed = g_randomSeed;
    g_FrozenFrameChrObjRngSeed = g_chrObjRandomSeed;
  }
  g_PrevFrameTimeScaleDropped = (g_TimeScaleDeltaFrames == 0);
}

void restore_rng_after_paused_render(void) {
  if (speedgraphframes == 0) {
    g_randomSeed = g_FrozenFrameRngSeed;
    g_chrObjRandomSeed = g_FrozenFrameChrObjRngSeed;
  }
}

void sync_frozen_rng_after_load(void) {
  g_FrozenFrameRngSeed = g_randomSeed;
  g_FrozenFrameChrObjRngSeed = g_chrObjRandomSeed;
}

void freeze_current_frame_after_load(s32 resume_delta_frames) {
  if (!g_IsTimePaused)
    return;

  /*
   * The pause hotkey is processed after this frame's delta has already been
   * calculated. A state loaded by that same hotkey must render immediately,
   * but must not run the restored simulation once with the old frame's delta.
   */
  g_ForcedDeltaFrames = resume_delta_frames;
  g_TimeScaleDeltaFrames = 0;
  speedgraphframes = 0;
  g_ClockTimer = 0;
  g_GlobalTimerDelta = 0.0f;
}
