#ifndef PRACTICE_STATES_GLOBALS_H
#define PRACTICE_STATES_GLOBALS_H

#include "practice_states_stream.h"
#include <ultra64.h>

void save_global_state(StateStream *stream);
void load_global_state(StateStream *stream);

#endif /* PRACTICE_STATES_GLOBALS_H */
