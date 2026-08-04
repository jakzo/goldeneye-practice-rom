#include "practice_frigate_fast_guard_death.h"
#include "bondaicommands.h"
#include "boss.h"
#include "initanitable.h"
#include "practice_config.h"
#include <bondconstants.h>

s32 practice_frigate_fast_guard_death_enabled(ChrRecord *guard) {
  return practice.frigate_fast_guard_death &&
         bossGetStageNum() == LEVELID_FRIGATE && guard != NULL;
}

s32 practice_frigate_guard_death_animation_index(
    ChrRecord *guard, struct struck_animation_table *animations, s32 count,
    s32 hit_part) {
  void *fastest_animation;
  s32 i;

  if (!practice_frigate_fast_guard_death_enabled(guard)) {
    return -1;
  }

  // Wiki animation #10 is fastest for gut and gun hits. Animation #2 is
  // fastest for every other regular hit group.
  if (hit_part == HIT_PELVIS || hit_part == HIT_GUN) {
    fastest_animation =
        &ptr_animation_table
             ->data[(s32)&ANIM_DATA_death_backward_fall_face_up1];
  } else {
    fastest_animation =
        &ptr_animation_table
             ->data[(s32)&ANIM_DATA_death_backward_spin_face_down_right];
  }

  for (i = 0; i < count; i++) {
    if (animations[i].anonymous_0 == fastest_animation) {
      return i;
    }
  }

  return 0;
}
