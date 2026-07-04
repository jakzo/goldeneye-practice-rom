# Save/Load States

This document logs our findings, analysis, and architecture specifications for the practice ROM's save/load state functionality, serving as a guide for future developers and AI agents.

You should update this document whenever new information is learned. Keep it concise and information-dense.

If you have been told to continue work on this feature, read the "current goal" section below to understand what we want to achieve. Also look at the key learnings and struct analysis for helpful information and leads to continue investigating.

Above all, make sure you investigate thoroughly to fully understand where pieces of data are used, their purpose and the implications of setting them to certain values on load. Once confident about a piece of data, add it to the save/load system, document your findings in this file, and stop and tell the user about what you found and how they can test it. Do not continue work unless the user explicitly tells you to.

## Current Goal

Props are fully implemented for save and restore, including adding props that
the save contains but the current world does not, and removing props that exist
but are absent from the save. The next goal is global state.

### Next Goals

- [done] Bond
- [done] Props (PropRecord type-by-type, including add/remove on load)
- [done] NPCs (position, animation, AI, etc.)
- [current] Global state (mission timer, objectives, etc.)

## Prompt

Read through [INSTRUCTIONS.md](src/practice/state/docs/INSTRUCTIONS.md) and implement support for saving/loading state of the first item in "Remaining State to Restore". Make sure to:

- See `practice_states_*.c` files for the current state of the code
- Remove the item from the "Remaining State to Restore" section once implemented
- No need to update the changelog or SAVE_STATE_VERSION
- If you notice some other global state that is not supported yet, implement it immediately if simple, otherwise add it to the list of remaining state to restore to do it later
- If there is a case that might need to be considered but you are not sure (for example, if a field is normally null or a pointer but theoretically the object pointed to could have been freed, though there is no evidence of this) then add an invariant assertion and emit an error log if it fails, no need to handle the case gracefully

## Remaining State to Restore

- Lighting that slowly changes regardless of time scale (not sure if part of state or time scale bug?)

## Key Learnings

Add any general advice helpful for future agents working on this feature here. Be sure to read and understand these before starting work on handling more state in the system.

- **Implement one feature at a time then manually test**: Since crashes and hangs are so common and can be unexpectedly caused by even seemingly innocuous changes, implement a small number of safe properties in the save/load code. After this, update this file and report back to the user what was added, what was learned and what they should now do in the game to test that the newly added properties are loaded correctly.
- **Stale Pointers**: Any struct member ending in `*` (e.g., `ALSoundState *`, `ObjectRecord *`, `PropRecord *`) is an absolute memory address. If the game engine deallocates or reallocates the target object, loading a saved state that retains the old pointer will cause a crash. All such pointers must either be relocated (mapped back to correct indices) or it will not affect gameplay nullified (set to `NULL`).
- **Sound System Crashing**: Sound structures (`ALSoundState`) are allocated dynamically. Nullifying properties like `audioHandle`, `openSoundState`, and `closeSoundState` upon loading prevents the sound engine from trying to modify a defunct sound node.
- **Alarm State**: `alarm_timer` is both the active flag and the elapsed alarm
  duration, so it must be restored exactly rather than reduced to a Boolean.
  `ptr_alarm_sfx` is a dynamically allocated sound handle; clear it on load so
  the next alarm update starts a fresh sound for an active restored alarm.
  Alarm consequences can also be latched in the global
  `objectiveregisters1` stage-script bitfield. For example, Bunker 1 sets bit
  `0x100` when the alarm activates, then a separate controller uses it to
  dispatch reinforcements later. Restore the complete bitfield so delayed
  stage-script actions agree with the restored alarm and mission state. Level
  background AI runs in `g_ActiveChrs`, whose entries are fake `ChrRecord`s
  without models or props and are therefore absent from prop serialization.
  Restore their AI list/offset, timer, flags, random value, presets, and other
  interpreter state as global state. Otherwise Bunker 1's reinforcement script
  retains its post-alarm timer and spawns guards again after loading a
  pre-alarm state.
- **Objective State**: Objective completion is split across several owners.
  `objectiveregisters1` contains mission-wide AI/script flags. Enter-room,
  deposit-item, and photograph progress is stored in mutable criteria records
  linked from the loaded stage setup; serialize their values in stable list
  order, not their pointers. `objectiveStatuses` caches the last displayed
  status and must agree with restored progress to avoid false HUD transition
  messages. Civilian casualties live in `g_playerPlayerData`, outside the main
  player struct, and can drive mission-failure AI.
- **Mission Dialogue State**: Top-of-screen mission dialogue is a two-entry
  circular text queue populated exclusively by the `AI_TextPrintTop` command.
  Track its stable 16-bit language text IDs alongside the runtime buffers,
  clearing slots on dequeue, display reset, and level start. Restore the text
  through `langGet` together with `upper_text_buffer_index`,
  `display_upper_text_window`, `upper_text_window_timer`, and
  `g_UpperTextDisplayFlag`; the last two control message lifetime and temporary
  suppression independently of the queue.
- **Watch Clock Hands**: The analog watch hands are derived from the global
  `watch_time_0`, which advances every gameplay tick and is initialized from an
  optional stage `INTROTYPE_WATCH` record. Its representation is version
  dependent: `s32` in US and `f32` in JP/EU, so serialize it with matching
  integer/float stream operations.
- **Watch Level Activity**: The watch menu changes two globals outside the
  player struct. `D_800483C0` gates world rendering through
  `sub_GAME_7F0BD8F0`, while `g_ControlsLockedFlag` makes `lvlManageMpGame` set
  `g_ClockTimer` to zero. Restore both through their accessors so loading a
  gameplay state while the watch is open immediately resumes rendering,
  controls, and timers. `g_ClockTimer` itself is derived each frame and does not
  need serialization.
- **Level Exit State**: `stop_time_flag` is the global latch used by
  `AI_TriggerFadeAndExitLevelOnButtonPress`: `0` is inactive, `1` waits for
  player input, and `2` is fading to the title stage. Restore its exact value;
  otherwise loading a state from before the end sequence leaves a later exit
  armed and the next button press fades out the level.
- **Level Countdown Timer State**: Scripted HUD countdowns used by Train,
  Streets, and any other `AI_HudCountdown*` command all share the globals
  `clock_drawn_flag`, `clock_enable`, and `clock_time` in `chrobjhandler.c`.
  Restore all three together; `clock_drawn_flag == 0` means visible,
  `clock_enable` controls ticking, and `clock_time` is the current tick value.
- **Toxic Gas State**: Toxic gas is another small global effect in
  `chrobjhandler.c`. Restore `toxic_gas_sound_timer`,
  `activate_gas_sound_timer`, `D_80030AD0`, `D_80030ADC`, `D_80030AE0`,
  `gasTimeToFullOpacity`, and `gasDoesDamageFlag`. `ptr_gas_sound` is a
  dynamic sound handle; clear it on load so active restored gas creates a fresh
  sound.
- **Sky State**: Cloud and water geometry is rebuilt every frame from the
  player camera and `CurrentEnvironmentRecord`; there are no persistent vertex
  or shape buffers to restore. `g_SkyCloudOffset` is the independently advancing
  cloud/water texture phase. Restore it together with the complete pointer-free
  current environment (including any interpolated alternative sky) and
  `g_FogSkyIsEnabled`.
- **Music State**: The three compact-sequence players advance independently of
  the gameplay clock. Temporary time-scale pause explicitly stops the players
  and remembers which were active, resumes only those players, and suspends
  music fades. Save states restore each track ID, sequence tick position,
  logical playing state, volume/fade progress, tempo, all per-channel
  instrument/controller state, mission music state, and the four scripted
  music-slot timers. Sequence positions are rebuilt as `ALCSeqMarker`s after
  reloading track data; never serialize marker or instrument pointers. Seeking
  past earlier MIDI program changes without restoring channel state makes every
  channel use the bank's default instrument.
- **SFX During Practice Pause**: Sound effects advance on the audio clock, not
  `g_ClockTimer`. Practice pause snapshots and mutes all seven SFX slot volumes,
  then restores each slot on resume. The active sound graph keeps running
  silently, so continuous effects such as sliding doors, alarms, vehicles, and
  toxic gas remain available without maintaining an owner-by-owner list.
- **Transient Gun Effects**: The global impact-flare/spark/dust pools
  (`dword_CODE_bss_8007A170`, plus
  `dword_CODE_bss_8007A4E0` outside EU) are independent of props. Serialize
  their live entries at exact pool indices and clear all live markers before
  loading. Map effect-image pointers to stable IDs instead of persisting
  absolute addresses. Player/NPC tracer beams are held in the restored
  hand/CHR records, but their rendered age must not advance while
  `g_ClockTimer == 0`.
- **Muzzle Flash (GUNFIRE Node)**: A firing weapon's muzzle flash is the
  GUNFIRE-node `visible` flag (`ModelRwData_GunfireRecord`) on the weapon's
  `Model`, set/read by `weaponSetGunfireVisible`/`weaponIsGunfireVisible`. It is
  _latched_, not per-frame: `chrlvFireWeaponRelated` → `chrSetFiring` raises it
  on a fire trigger and it is only cleared by stop-firing, so it survives across
  ticks and is **not** re-derived on load. NPC/CHR-held weapons are disabled
  child props that the prop loop never serializes, so the flag is saved per held
  weapon inside the CHR record (next to its model/weaponnum/flags) and reapplied
  in `restore_chr_attachments` _after_ each weapon is reattached — weapon
  (re)creation resets the node to hidden, so restoring it earlier would be lost.
  Standalone (enabled) weapon props on the ground never fire, so their flag is
  always clear and is not serialized. The player's own first-person muzzle flash
  is a separate view-model effect driven each frame from the saved hand firing
  state, not this prop flag.
- **Concealed CHR Items**: Stage scripts can equip non-weapon objects (such as
  the starting-room key in Archives) by disabling the object prop, removing it
  from the active list, and parenting it to a CHR. These objects are not named
  by `weapons_held` or the hat pointer; `chrDropItems` discovers them by walking
  the CHR child chain. Serialize concealed child props alongside active-list
  props, then reattach them after held weapons and hats rebuild that chain.
  Otherwise loading erases the ownership link and the item is not dropped when
  the guard dies.
- **Ending Cutscene CHR Lifecycle State**: Some level-end AI queues normal CHRs
  for teardown or hides them while the exit cutscene is active. Loading an
  earlier state during that cutscene must overwrite the live `CHRHIDDEN_REMOVE`
  and `CHRFLAG_HIDDEN` bits; otherwise restored CHRs are removed or left
  invisible on the next tick. These bits are pointer-free and are restored with
  the CHR record rather than global state.
- **Object Projectile/Embedment Union**: `ObjectRecord::projectile` and `ObjectRecord::embedment` occupy the same union slot. On load, restore only the member selected by `RUNTIMEBITFLAG_DEPOSIT` or `RUNTIMEBITFLAG_EMBEDDED`. Restoring one and then clearing the other overwrites the shared pointer; a deposited object will retain its flag and crash on the next tick when the engine dereferences the null projectile.
- **Resolve Projectile Prop Indices After Loading All Props**: Do not temporarily store saved prop indices in `Projectile::ownerprop` or `Projectile::obj`. If a referenced prop was collected or otherwise removed after the save, its record is skipped and the integer remains disguised as a pointer; a later tick or second save will dereference it and crash. Keep indices in separate arrays, resolve them after all prop records are processed, and free projectiles whose object prop no longer exists.
- **Adding/Removing Props on Load**: The loader rebuilds the prop array to match the save exactly. Props are processed in ascending slot order; before each saved record, every enabled prop in the skipped slots is removed (`removePropAtIndex`), and after the last record all trailing enabled slots are removed. Each saved prop is restored into its _exact_ original slot so all index-based references (parent/child/prev/next, `weapons_held`, projectile `obj`) stay valid. When the current world has no compatible prop in a slot, it is recreated there:
    - CHR: torn down and rebuilt via `create_chr_prop` (body/head allocation metadata).
    - Setup-backed objects and weapon props are rebuilt via `create_object_prop`. Level-defined objects reuse their persistent setup `ObjectRecord` (found through a serialized setup-command index from `setupGetCommandIndexByProp`); dynamically dropped/thrown collectables take a fresh record from the weapon pool (`weaponCreate`). The common fields then install the authoritative object data and rooms are registered.
    - Explosions/smoke: rebuilt via `create_explosion_prop`/`create_smoke_prop` using a free `g_ExplosionBuffer`/`g_SmokeBuffer` entry.
    - Do not assume setup geometry persists. Destroyable setup objects can be freed completely; Train's 20 cuttable floor strips are ordinary `PROPDEF_PROP` records whose slots can then be reused. Recreate any missing setup-backed object from its saved command index. Doors still rely on their persistent live prop.
      Each object/door/weapon record carries an `ObjAllocationState` (model id, subtype, setup-command index) ahead of its payload so the destination prop can be built before the payload is consumed, mirroring `ChrAllocationState` for CHRs. The active-list links (`first`/`current`) and the per-prop `prev/next` are rebuilt from the restored indices; the attachment graph (`parent`/`child`) is owned by `restore_chr_attachments`. Disabled mid-regeneration props remain on the active list and are serialized; disabled attached equipment is restored through its owning CHR. **This is new, hard-to-test code** — the dominant test is save → play (collect items, kill guards, throw grenades, open doors) → load and confirm the saved world is faithfully restored without crashes.
- **Discard Post-Save CHR Equipment**: A guard can acquire a different weapon
  or hat after saving. When loading, an old attachment not named by the save
  must be freed, not detached and activated as a dropped item. Activating it
  preserves state that did not exist at save time and can splice its overloaded
  child-sibling `prev`/`next` links into the active prop list.
- **Validate CHR Ownership Before Reuse**: Matching body/head IDs do not prove
  that a prop still owns its `ChrRecord`. A reused prop slot can retain a stale
  pointer to a same-model character owned by another slot. Require
  `chr->prop == prop` and `model->chr == chr`; otherwise clear only the stale
  prop alias and recreate its saved CHR. Destroying the shared `ChrRecord`
  would also destroy the legitimate owner. Reusing it separates collision
  (`prop->pos`) from rendering (`model` root position), creating an invisible
  blocking guard.
- **Never Restore the Prop Free List From Saved Indices — Rebuild It**: Free-list entries have no saved record, so their `prev` links — which chain the free list together — are never written. Restoring `ptr_obj_pos_list_final_entry` from the saved head index therefore points the free list into stale pre-load links. `chrpropAllocate` can then hand out slots that are actually in use, corrupting the prop graph and eventually crashing. Rebuild the free list after props and attachments are restored, but do not equate disabled with free: respawning pickups are disabled while remaining on the active list, and inactive setup objects can remain bound through `obj->prop`. Serialize every active-list prop regardless of `PROPFLAG_ENABLED`; a slot is free only when it was not saved active, has no parent, and has no live CHR/object/explosion/smoke owner. The `prev`-chain integer round-trip through `get_prop_index`/`get_prop_by_index` cannot preserve a misaligned pointer, so any misalignment seen in memory is live corruption rather than saved data.
- **Active Records Must Clear Live Parents**: Every serialized prop is reached
  through the active list and therefore had `parent == NULL` at save time. If
  the prop is later collected, its live record is delisted and reparented to
  the player. Loading its common fields without detaching that live parent
  makes the final active-list rebuild skip it; the preceding prop keeps
  `next == NULL`, orphaning every later prop from rendering. Detach any current
  parent before restoring a saved active record.
- **NULL Prop Handling (Cutscenes vs Gameplay)**: The player's world physical presence `g_CurrentPlayer->prop` is `NULL` during intro cutscenes, level loading, or death. When loading a saved state:
    - Track whether the saved state had a valid prop using a `has_prop` boolean flag.
    - If the saved state has a prop but the current player does not (e.g., loading gameplay into a cutscene), allocate a new prop using `chrpropAllocate()`, initialize its fields, activate it, enable it, and register it to its rooms.
    - If the saved state does not have a prop but the current player does (e.g., loading a cutscene into active gameplay), deregister, delist, disable, and free the current player prop using `chrpropDeregisterRooms`, `chrpropDelist`, `chrpropDisable`, and `chrpropFree`.
    - If both have a prop, update its coordinates and rooms safely using room deregistration/registration.

## Struct Analysis

Keep all the properties of the structs and global variables we save and load in files in this directory. Document them and their properties, along with whether they affect gameplay, their status in the save/load code and any other important information. Use this as a guide when reading source code to understand what each property does.

If you are missing any information about a struct or variable, mark with a TODO comment what should still be investigated and documented.
