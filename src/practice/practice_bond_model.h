#ifndef PRACTICE_BOND_MODEL_H
#define PRACTICE_BOND_MODEL_H

#include <bondtypes.h>
#include <ultra64.h>

/* Third-person Bond stand-in used by freecam and picture-in-picture views. */

void practice_bond_model_reset(void);
void practice_bond_model_tick(s32 pending_gfx_tasks);
void practice_bond_model_ensure(void);
void practice_bond_model_load_and_ensure(void);
void practice_bond_model_prepare_state_load(void);
void practice_bond_model_finish_state_load(void);
Gfx *practice_bond_model_render(Gfx *gdl, s32 force_room_visible);
Gfx *practice_bond_model_render_in_view(Gfx *gdl);

#endif /* PRACTICE_BOND_MODEL_H */
