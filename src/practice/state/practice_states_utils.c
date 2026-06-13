#include "practice_states_utils.h"
#include "chrai.h"
#include <bondconstants.h>
#include <ultra64.h>

extern PropRecord pos_data_entry[];
extern struct StandTile *standTileStart;

s32 get_prop_index(PropRecord *prop) {
    if (prop == NULL) {
        return -1;
    }
    return prop - pos_data_entry;
}

PropRecord *get_prop_by_index(s32 index) {
    if (index < 0 || index >= POS_DATA_ENTRY_LEN) {
        return NULL;
    }
    return &pos_data_entry[index];
}

PropRecord *get_safe_prop_by_index(s32 index) {
    PropRecord *prop = get_prop_by_index(index);
    if (prop != NULL && (prop->flags & PROPFLAG_ENABLED)) {
        return prop;
    }
    return NULL;
}

s32 get_tile_offset(StandTile *tile) {
    if (tile == NULL || standTileStart == NULL) {
        return -1;
    }
    return (s32)((u8 *)tile - (u8 *)standTileStart);
}

StandTile *get_tile_by_offset(s32 offset) {
    if (offset < 0 || standTileStart == NULL) {
        return NULL;
    }
    return (StandTile *)((u8 *)standTileStart + offset);
}
