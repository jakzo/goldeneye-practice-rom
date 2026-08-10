#include "practice_states_utils.h"
#include "chrai.h"
#include "chraidata.h"
#include "practice_ui.h"
#include <bondconstants.h>
#include <assert.h>
#include <ultra64.h>

#define AI_LIST_REFERENCE_SETUP_FLAG 0x80000000U
#define AI_LIST_REFERENCE_INDEX_MASK 0x7fffffffU
#define AI_LIST_REFERENCE_NULL (-1)

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

PropRecord *get_enabled_prop_by_index(s32 index) {
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
  /* -1 is the only serialized NULL sentinel. A non-NULL tile-like field can
   * temporarily contain an address below standTileStart; its signed byte
   * offset is negative, but adding it back still reconstructs the exact value.
   * Collapsing every negative offset to NULL changes upstream collision state
   * on load and can perturb Bond's next movement calculation. */
  if (offset == -1 || standTileStart == NULL) {
    return NULL;
  }
  return (StandTile *)((u8 *)standTileStart + offset);
}

s32 get_ai_list_reference(AIRecord *ailist) {
  s32 i;

  if (ailist == NULL) {
    return AI_LIST_REFERENCE_NULL;
  }

  if (g_CurrentSetup.ailists != NULL) {
    for (i = 0; g_CurrentSetup.ailists[i].ailist != NULL; i++) {
      if (g_CurrentSetup.ailists[i].ailist == ailist) {
        return AI_LIST_REFERENCE_SETUP_FLAG | i;
      }
    }
  }

  for (i = 0; g_GlobalAILists[i].ailist != NULL; i++) {
    if (g_GlobalAILists[i].ailist == ailist) {
      return g_GlobalAILists[i].ID;
    }
  }

  practiceLogError("Could not identify AI list %08x", ailist);
  assert(FALSE);
  return AI_LIST_REFERENCE_NULL;
}

AIRecord *get_ai_list_by_reference(s32 reference) {
  if (reference == AI_LIST_REFERENCE_NULL) {
    return NULL;
  }

  if ((u32)reference & AI_LIST_REFERENCE_SETUP_FLAG) {
    u32 target = (u32)reference & AI_LIST_REFERENCE_INDEX_MASK;
    u32 i;

    if (g_CurrentSetup.ailists != NULL) {
      for (i = 0; g_CurrentSetup.ailists[i].ailist != NULL; i++) {
        if (i == target) {
          return g_CurrentSetup.ailists[i].ailist;
        }
      }
    }

    practiceLogError("Invalid setup AI list index %d", target);
    assert(FALSE);
    return NULL;
  }

  if (!isGlobalAIListID(reference)) {
    practiceLogError("Invalid global AI list reference %d", reference);
    assert(FALSE);
    return NULL;
  }

  return ailistFindById(reference);
}
