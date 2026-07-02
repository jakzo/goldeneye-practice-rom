#ifndef PRACTICE_DIALOG_H
#define PRACTICE_DIALOG_H

#include "state/practice_states_stream.h"
#include <ultra64.h>

void practice_dialog_reset(void);
void practice_dialog_enqueue(s32 buffer_index, u16 text_id);
void practice_dialog_remove(s32 buffer_index);
void practice_dialog_save(StateStream *stream);
void practice_dialog_load(StateStream *stream);

#endif /* PRACTICE_DIALOG_H */
