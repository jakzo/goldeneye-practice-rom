#include "ultratypes.h"
#include <ultra64.h>

f32 g_TimeScale = 1.0f;
f32 g_TimeScaleFinal = 1.0f;
f32 g_FractionalClockTimerAcc = 0.0f;
s32 g_IsTimeScaleChanged = FALSE;
s32 g_IsTimePaused = FALSE;

void set_final_time_scale(f32 scale) {
  g_TimeScaleFinal = scale;
  g_IsTimeScaleChanged = scale != 1.0f;
}

// TODO: Gun fire rate by bond is unaffected by time scale.
//       Look into gunTickGameplay and chrlvTriggerFireWeapon.
void set_time_scale(f32 scale) {
  if (scale < 0.0f)
    scale = 0.0f;
  g_TimeScale = scale;
  if (!g_IsTimePaused)
    set_final_time_scale(scale);
}

void pause() {
  g_IsTimePaused = TRUE;
  set_final_time_scale(0.0f);
}

void unpause() {
  g_IsTimePaused = FALSE;
  set_final_time_scale(g_TimeScale);
}
