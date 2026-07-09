#ifndef PRACTICE_LAG_H
#define PRACTICE_LAG_H

#include <ultra64.h>

/*
 * Strafing lag impact helper.
 *
 * Uses the experimentally-fitted TEST_MOVE_SPEED curve (see
 * docs/test_strafe_speed_results.html) to estimate how much extra wall-clock/
 * mission time lag cost while Bond was diagonal-strafing.
 */

/* Reset accumulated lag impact. Call on stage load. */
void practice_lag_reset(void);

/*
 * Call once per rendered gameplay frame after player input/movement have been
 * processed. Accumulates total diagonal-strafing time and predicted lag-added
 * time while the mission timer is running.
 */
void practice_lag_tick(void);

/*
 * Total seconds spent diagonal-strafing this run (including lag frames).
 * Zero when no diagonal strafing was recorded.
 */
f32 practice_lag_get_running_seconds(void);

/*
 * Seconds of that runtime attributable to lag (how much faster the same
 * distance could have been covered with 0 dropped frames).
 */
f32 practice_lag_get_time_added_seconds(void);

/*
 * Instantaneous 100m-strafe lag cost for a constant number of dropped frames
 * (speedgraphframes - 1). Shared by the HUD lag estimate.
 */
f32 practice_lag_strafe_100m_time_added(s32 dropped_frames);

/*
 * RGBA color for the lag-ratio portion of the impact summary.
 * Green at <=10% lag/run, orange near 35%, red at >=50%, with gradients
 * between.
 */
u32 practice_lag_impact_color(f32 running_seconds, f32 lag_seconds);

/* Save/load `.bss`-scale accumulators for save states. */
void practice_lag_save(void *stream);
void practice_lag_load(void *stream);

#endif /* PRACTICE_LAG_H */
