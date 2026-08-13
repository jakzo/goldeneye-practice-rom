#ifndef PRACTICE_BUNKER1_H
#define PRACTICE_BUNKER1_H

#include <bondtypes.h>
#include <ultra64.h>

u8 practice_bunker1_guard_grenade_probability(s32 stage_id, s32 chrnum,
                                              u8 probability);
void practice_bunker1_guard_pulled_grenade(ChrRecord *guard);

#endif /* PRACTICE_BUNKER1_H */
