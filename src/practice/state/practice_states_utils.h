#ifndef PRACTICE_STATES_UTILS_H
#define PRACTICE_STATES_UTILS_H

#include <ultra64.h>
#include <bondtypes.h>

s32 get_prop_index(PropRecord *prop);
PropRecord *get_prop_by_index(s32 index);
PropRecord *get_enabled_prop_by_index(s32 index);
s32 get_tile_offset(StandTile *tile);
StandTile *get_tile_by_offset(s32 offset);
s32 get_ai_list_reference(AIRecord *ailist);
AIRecord *get_ai_list_by_reference(s32 reference);

#endif /* PRACTICE_STATES_UTILS_H */
