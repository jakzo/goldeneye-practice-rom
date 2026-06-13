#ifndef PRACTICE_STATES_BOND_H
#define PRACTICE_STATES_BOND_H

#include <ultra64.h>
#include <bondtypes.h>

extern bool g_HasSavedState;

void save_bond_state(void);
void load_bond_state(void);

#endif /* PRACTICE_STATES_BOND_H */
