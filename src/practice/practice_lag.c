#include "practice_lag.h"
#include "bondview.h"
#include "player.h"
#include "practice_config.h"
#include "practice_states_stream.h"
#include "unk_0C0A70.h"
#include <ultra64.h>

/*
 * Median no-lag 100m diagonal strafe time from TEST_MOVE_SPEED (deltaFrames=1
 * samples in docs/test_strafe_speed_results.html). Used to convert per-frame
 * lag into an equivalent "seconds lost over the distance just covered".
 */
#define STRAFE_100M_BASELINE_SECONDS 12.17f

/*
 * Minimum |speedforwards| / |speedsideways| before a frame counts as
 * diagonal-strafing. Both directions are held digitally once ramped past ~1;
 * 0.5 catches the ramp and near-full-speed region without counting pure
 * forward hang or pure strafe.
 */
#define STRAFE_SPEED_THRESHOLD 0.5f

/* Ratio of lag / total running time for the color gradient stops. */
#define LAG_RATIO_GREEN 0.01f
#define LAG_RATIO_ORANGE 0.05f
#define LAG_RATIO_RED 0.10f

#define LAG_COLOR_GREEN 0x116611FF
#define LAG_COLOR_ORANGE 0x997711FF
#define LAG_COLOR_RED 0x991111FF

extern s32 is_timer_active;
extern s32 g_ClockTimer;

/* Total seconds spent diagonal-strafing (including lag frames). */
static f32 s_RunningSeconds = 0.0f;

/* Accumulated predicted extra mission time spent due to lag while strafing. */
static f32 s_LagTimeAddedSeconds = 0.0f;

static s32 clamp_byte(s32 value) {
  if (value < 0) {
    return 0;
  }
  if (value > 255) {
    return 255;
  }
  return value;
}

static u32 lerp_rgba(u32 a, u32 b, f32 t) {
  s32 ar = (a >> 24) & 0xFF;
  s32 ag = (a >> 16) & 0xFF;
  s32 ab = (a >> 8) & 0xFF;
  s32 aa = a & 0xFF;
  s32 br = (b >> 24) & 0xFF;
  s32 bg = (b >> 16) & 0xFF;
  s32 bb = (b >> 8) & 0xFF;
  s32 ba = b & 0xFF;
  s32 r;
  s32 g;
  s32 blue;
  s32 alpha;

  if (t < 0.0f) {
    t = 0.0f;
  } else if (t > 1.0f) {
    t = 1.0f;
  }

  r = clamp_byte(ar + (s32)((f32)(br - ar) * t + 0.5f));
  g = clamp_byte(ag + (s32)((f32)(bg - ag) * t + 0.5f));
  blue = clamp_byte(ab + (s32)((f32)(bb - ab) * t + 0.5f));
  alpha = clamp_byte(aa + (s32)((f32)(ba - aa) * t + 0.5f));

  return ((u32)r << 24) | ((u32)g << 16) | ((u32)blue << 8) | (u32)alpha;
}

f32 practice_lag_strafe_100m_time_added(s32 dropped_frames) {
  f32 d;
  f32 added_seconds;

  if (dropped_frames <= 0) {
    return 0.0f;
  }

  d = (f32)dropped_frames;

  /*
   * Fourth-order least-squares fit of the TEST_MOVE_SPEED strafe medians in
   * docs/test_strafe_speed_results.html, expressed as added 100m time vs
   * dropped frames and forced through 0.
   */
  added_seconds =
      d * (0.247800872f +
           d * (0.017377249f + d * (-0.001018621f + d * 0.000013940f)));

  return added_seconds < 0.0f ? 0.0f : added_seconds;
}

void practice_lag_reset(void) {
  s_RunningSeconds = 0.0f;
  s_LagTimeAddedSeconds = 0.0f;
}

void practice_lag_tick(void) {
  s32 dropped_frames;
  f32 poly_added;
  f32 lagged_100m_time;
  f32 delta_seconds;

  if (g_CurrentPlayer == NULL || !is_timer_active || g_ClockTimer <= 0) {
    return;
  }

  /*
   * Require both forward/back and left/right movement intent. The experimental
   * curve models diagonal (strafe+forward) max-speed movement; pure forward
   * or pure strafe use a different speed profile.
   */
  if (g_CurrentPlayer->speedforwards > -STRAFE_SPEED_THRESHOLD &&
      g_CurrentPlayer->speedforwards < STRAFE_SPEED_THRESHOLD) {
    return;
  }
  if (g_CurrentPlayer->speedsideways > -STRAFE_SPEED_THRESHOLD &&
      g_CurrentPlayer->speedsideways < STRAFE_SPEED_THRESHOLD) {
    return;
  }

  delta_seconds = (f32)g_ClockTimer / 60.0f;
  s_RunningSeconds += delta_seconds;

  dropped_frames = speedgraphframes - 1;
  if (dropped_frames <= 0) {
    return;
  }

  poly_added = practice_lag_strafe_100m_time_added(dropped_frames);
  if (poly_added <= 0.0f) {
    return;
  }

  lagged_100m_time = STRAFE_100M_BASELINE_SECONDS + poly_added;

  /*
   * While traveling distance at lag d for delta_seconds, the no-lag time for
   * the same distance is delta_seconds * T0 / T(d). Extra time is therefore:
   *   delta_seconds * (1 - T0/T(d)) = delta_seconds * poly / T(d)
   */
  s_LagTimeAddedSeconds += delta_seconds * (poly_added / lagged_100m_time);
}

f32 practice_lag_get_running_seconds(void) { return s_RunningSeconds; }

f32 practice_lag_get_time_added_seconds(void) { return s_LagTimeAddedSeconds; }

u32 practice_lag_impact_color(f32 running_seconds, f32 lag_seconds) {
  f32 ratio = lag_seconds / running_seconds;

  if (ratio <= LAG_RATIO_GREEN) {
    return LAG_COLOR_GREEN;
  }
  if (ratio >= LAG_RATIO_RED) {
    return LAG_COLOR_RED;
  }
  if (ratio <= LAG_RATIO_ORANGE) {
    return lerp_rgba(LAG_COLOR_GREEN, LAG_COLOR_ORANGE,
                     (ratio - LAG_RATIO_GREEN) /
                         (LAG_RATIO_ORANGE - LAG_RATIO_GREEN));
  }
  return lerp_rgba(LAG_COLOR_ORANGE, LAG_COLOR_RED,
                   (ratio - LAG_RATIO_ORANGE) /
                       (LAG_RATIO_RED - LAG_RATIO_ORANGE));
}

void practice_lag_save(void *stream) {
  write_f32((StateStream *)stream, s_RunningSeconds);
  write_f32((StateStream *)stream, s_LagTimeAddedSeconds);
}

void practice_lag_load(void *stream) {
  s_RunningSeconds = read_f32((StateStream *)stream);
  s_LagTimeAddedSeconds = read_f32((StateStream *)stream);
}
