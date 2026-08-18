#ifndef PRACTICE_FREECAM_H
#define PRACTICE_FREECAM_H

#include <ultra64.h>
#include <bondtypes.h>

s32 practice_freecam_enable(s32 controller);
void practice_freecam_disable(void);
void practice_freecam_reset(void);
s32 practice_freecam_is_active(void);
void practice_freecam_pin_camera(void);
void practice_freecam_clear_pinned_camera(void);
s32 practice_freecam_add_pinned_camera_view(void);
void practice_freecam_ensure_bond_for_external_cameras(void);
void practice_freecam_sync_bond_for_external_cameras(s32 needed);
void practice_freecam_process_pending_bond_ensure(s32 pending_gfx_tasks);
Gfx *practice_freecam_render_bond(Gfx *gdl);
Gfx *practice_freecam_render_bond_in_external_camera(Gfx *gdl);
void practice_freecam_release_deferred_model(s32 pending_gfx_tasks);
void practice_freecam_prepare_state_load(void);
void practice_freecam_finish_state_load(void);
void practice_freecam_age_rooms(void);
void practice_freecam_tick(u16 hotkey_trigger);
s32 practice_freecam_apply_camera(void);
s32 practice_freecam_consume_camera_restore(void);
s32 practice_freecam_get_render_context(u8 *room, coord3d **position,
                                        StandTile **tile);

#endif /* PRACTICE_FREECAM_H */
