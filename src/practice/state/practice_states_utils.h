#ifndef PRACTICE_STATES_UTILS_H
#define PRACTICE_STATES_UTILS_H

#include <ultra64.h>
#include <bondtypes.h>

s32 get_prop_index(PropRecord *prop);
PropRecord *get_prop_by_index(s32 index);
PropRecord *get_safe_prop_by_index(s32 index);
s32 get_tile_offset(StandTile *tile);
StandTile *get_tile_by_offset(s32 offset);

#endif /* PRACTICE_STATES_UTILS_H */
