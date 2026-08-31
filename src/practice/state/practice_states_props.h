#ifndef PRACTICE_STATES_PROPS_H
#define PRACTICE_STATES_PROPS_H

#include "practice_states_stream.h"
#include <bondtypes.h>
#include <ultra64.h>

bool save_props_state(StateStream *stream);
bool load_props_state(StateStream *stream);
bool practice_states_restore_chr_model_display_lists(void);
void practice_states_sanitize_chr_attachments(ChrRecord *chr);

#endif /* PRACTICE_STATES_PROPS_H */
