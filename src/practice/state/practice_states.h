#ifndef PRACTICE_STATES_H
#define PRACTICE_STATES_H

#include "../practice_sram.h"
#include "practice_storage.h"
#include "practice_states_stream.h"
#include <bondtypes.h>
#include <ultra64.h>

#define SAVE_STATE_MAGIC 0x47455353 // "GESS"

/**
 * Increment this version number before releasing a new version of the ROM with
 * a breaking change to the save format.
 *
 * Safe changes:
 * - Adding extra fields/structs to the very end of the save state.
 * - Changing the data being stored inside an existing field (as long as
 * type/size is unchanged).
 *
 * Breaking changes:
 * - Modifying the size or layout of fields in the middle of the save state
 * (which alters offsets of all subsequent fields).
 */
#define SAVE_STATE_VERSION 8
typedef struct {
  u32 magic;
  u16 version;
  u16 size; /* Low 16 bits retained for compatibility. */
  s32 level_id;
  u32 full_size; /* Full serialized size; zero in older version-8 states. */
} SaveStateHeader;

void save_global_state(StateStream *stream);
void load_global_state_pre_props(StateStream *stream);
bool load_global_state_post_props(void);
void restore_global_rng_after_load(void);
bool save_viewer_players_state(StateStream *stream);
bool load_viewer_players_state(StateStream *stream,
                               bool force_model_rebuild);
bool practice_states_model_node_tree_is_valid(ModelNode *root);
bool save_props_state(StateStream *stream);
bool load_props_state(StateStream *stream);

extern bool g_HasSavedState;

void init_save_state_system(void);
void practice_states_on_stage_load(void);
bool practice_states_save_is_from_current_stage(void);
void save_game_state(void);
void load_game_state(void);
bool practice_states_set_storage_location(PracticeStorageLocation location);
PracticeStorageLocation practice_states_get_storage_location(void);
void practice_states_notify_sram_overwritten(void);
void practice_states_log_test_fixture(void);
#ifdef PRACTICE_TEST_ROM
#define TEST_SAVE_STATE_SLOT_SIZE 0x20000
void practice_states_set_test_slot(s32 slot);
s32 practice_states_get_test_slot(void);
#endif

#endif /* PRACTICE_STATES_H */
