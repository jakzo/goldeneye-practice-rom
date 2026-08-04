#ifndef PRACTICE_FRIGATE_FAST_GUARD_DEATH_H
#define PRACTICE_FRIGATE_FAST_GUARD_DEATH_H

#include "chr.h"

s32 practice_frigate_fast_guard_death_enabled(ChrRecord *guard);
s32 practice_frigate_guard_death_animation_index(
    ChrRecord *guard, struct struck_animation_table *animations, s32 count,
    s32 hit_part);

#endif /* PRACTICE_FRIGATE_FAST_GUARD_DEATH_H */
