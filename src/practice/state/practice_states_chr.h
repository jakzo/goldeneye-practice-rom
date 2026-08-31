#ifndef PRACTICE_STATES_CHR_H
#define PRACTICE_STATES_CHR_H

#include "practice_states_stream.h"
#include <bondtypes.h>

typedef struct ChrAttachmentIndices {
  s16 weapons_held[3];
  s16 hat;
  s16 weapon_model[2];
  s8 weaponnum[2];
  u32 weapon_flags[2];
  s8 gunfire_visible[2];
  s16 hat_model;
  u32 hat_flags;
} ChrAttachmentIndices;

typedef struct ChrAllocationState {
  s16 slot_index;
  s8 headnum;
  s8 bodynum;
  f32 heading;
  u16 model_type;
  u16 head_record_index;
  u16 head_data_offset;
} ChrAllocationState;

void save_chr_record(StateStream *stream, const ChrRecord *chr);
void load_chr_allocation_state(StateStream *stream,
                               ChrAllocationState *allocation);
void load_chr_record(StateStream *stream, ChrRecord *chr,
                     ChrAttachmentIndices *attachments,
                     u32 *saved_root_data_offset);
bool practice_states_reload_model_root_data(StateStream *stream, Model *model,
                                            u32 saved_root_data_offset);
void practice_states_save_model_animation(StateStream *stream,
                                          const Model *model);
void practice_states_load_model_animation(StateStream *stream, Model *model);
void practice_states_save_animation_frame_buffer(StateStream *stream);
void practice_states_load_animation_frame_buffer(StateStream *stream);
void practice_states_save_chr_model_definitions(StateStream *stream);
bool practice_states_canonicalize_chr_model_definitions(void);
bool practice_states_load_chr_model_definitions(StateStream *stream);
bool practice_states_restore_model_parent_links(Model *model);
bool practice_states_rebuild_chr_model_allocation(ChrRecord *chr);
bool practice_states_restore_chr_model_allocation(
    ChrRecord *chr, const ChrAllocationState *allocation);
bool practice_states_save_chr_action(StateStream *stream,
                                     const ChrRecord *chr);
void practice_states_load_chr_action(StateStream *stream, ChrRecord *chr);
void clear_chr_model_blood_patches(ChrRecord *chr);
void reset_chr_blood_vertex_pool(void);
void load_chr_prop_spatial_state(PropRecord *prop, const coord3d *pos,
                                 s32 stan_offset, const u8 rooms[4]);

#endif /* PRACTICE_STATES_CHR_H */
