# Save/Load States

This document logs our findings, analysis, and architecture specifications for the practice ROM's save/load state functionality, serving as a guide for future developers and AI agents.

You should update this document whenever new information is learned. Keep it concise and information-dense.

If you have been told to continue work on this feature, read the "current goal" section below to understand what we want to achieve. Also look at the key learnings and struct analysis for helpful information and leads to continue investigating.

Above all, make sure you investigate thoroughly to fully understand where pieces of data are used, their purpose and the implications of setting them to certain values on load. Once confident about a piece of data, add it to the save/load system, document your findings in this file, and stop and tell the user about what you found and how they can test it. Do not continue work unless the user explicitly tells you to.

## Current Goal

Everything should now be implemented. Right now bugs and missing bits are being found through manual testing and then added to the list below for fixing.

## Prompt

Read through [INSTRUCTIONS.md](src/practice/state/docs/INSTRUCTIONS.md) and implement or fix save/load state support for the first item in "Remaining".

## Remaining

- When saving during the intro camera swirl then loading again, the Bond player model is loaded in the position the camera was saved at, causing you to start in a different position after loading
- After dying the game shows a replay of your death and turns the sfx volume down, but when loading state back to alive the volume stays lowered
- Saving object deformation vertices can cost 20kb+, can we save the seeds used to deform them instead?
- Lighting that slowly changes regardless of time scale (not sure if part of state or time scale bug?)

## Key Learnings

Add any general advice helpful for future agents working on this feature here. Be sure to read and understand these before starting work on handling more state in the system.

- **Implement one feature at a time then manually test**: Since crashes and hangs are so common and can be unexpectedly caused by even seemingly innocuous changes, implement a small number of safe properties in the save/load code. After this, update this file and report back to the user what was added, what was learned and what they should now do in the game to test that the newly added properties are loaded correctly.
- **Stale Pointers**: Any struct member ending in `*` (e.g., `ALSoundState *`, `ObjectRecord *`, `PropRecord *`) is an absolute memory address. If the game engine deallocates or reallocates the target object, loading a saved state that retains the old pointer will cause a crash. All such pointers must either be relocated (mapped back to correct indices) or it will not affect gameplay nullified (set to `NULL`).
- **EU Player Layout Differs After the Hand Headers**: `struct player` embeds
  two `ModelFileHeader` values before `hands`. The EU header omits the
  non-EU `isLoaded` field, so `hands` and every later player field start eight
  bytes earlier in EU (`0x0868` instead of `0x0870`). Any raw player snapshot
  ranges after this point must use EU-adjusted offsets; applying US offsets to
  an EU save corrupts restored hand state and can crash when the first-person
  weapon model next updates. Prefer named members such as
  `g_CurrentPlayer->hands` over hardcoded addresses where possible.
- **Relocate Symbols in Practice-Build Assembly**: Practice code changes the
  final BSS layout, so original absolute addresses embedded in unmatched
  assembly are not necessarily valid. Express both halves of an address as
  `%hi(symbol)` and `%lo(symbol)` even when the original immediate values are
  known. The EU firing path previously used literal halves of
  `g_CurrentPlayer`; practice builds then dereferenced the original address and
  crashed on their first weapon update. Symbolic operands still produce the
  original bytes when building against the original layout.
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
- **Shot Light Fixtures**: Shootable background lights are not props. The
  `lightfixture.c` darkened-light ring stores room id and BG vertex index, and
  the actual vertex colours are destructively shifted when the light is shot.
  Serialize the ring cursor and live entries by exact ring index. On load,
  left-shift resident vertices named by the live table to undo current
  darkening, restore the saved table, then call `redarken_lights_in_room` only
  for rooms whose BG vertex allocation is currently resident. Treat
  `g_BgRoomInfo[room].model_bin_loaded` as the authoritative residency flag;
  `ptr_point_index` should also be non-null before touching vertices. Leave
  entries for unloaded rooms in the table; the normal BG load path calls
  `redarken_lights_in_room` when those vertices become resident again. Do not
  reload the BG room allocation while loading state: an in-flight render task
  can still reference its display lists, and the BG point-table loader uses the
  room allocation tail as scratch. Airborne glass shards from these lights are
  emitted through `sub_GAME_7F0A2160` into the broken-window shard pool
  (`ptr_shattered_window_pieces` and `g_NextShardNum`), not the flying-particle
  pool. Serialize this as an optional tail after the pre-existing props tail
  data, using the props section byte count to detect whether the shard payload
  is present; when absent, clear the live shard pool rather than preserving
  post-save debris. When present, serialize live shard entries at exact pool
  indices.
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
- **Saved Prop Rooms Are Authoritative**: Deregister a reused root prop from its
  live rooms, restore its serialized room list, then register that exact list.
  Do not call `setupUpdateObjectRoomPosition` during load. Its spatial search
  depends on current portal visibility; a closed tinted-glass portal can make
  the glass omit the room on the viewer's side, so the pane is then culled
  before it can render or reopen its own portal. Control's vertically stacked
  windows expose this clearly.
- **Room Prop Lists Are Derived State**: `RoomPropListChunkIndexes` and
  `RoomPropListChunks` are room-to-prop lookup indexes built by
  `chrpropRegisterRooms`, not authoritative saved state. Loading gameplay from
  a different lifecycle moment, such as an ending cutscene, can leave these
  chunk tables stale or corrupted even when each restored `PropRecord::rooms`
  list is correct. Clear the chunk tables before destructive prop replacement,
  then rebuild them after props, attachments, and the player/viewer prop have
  been restored by walking the active root prop list and registering each
  restored room list. Otherwise `roomGetProps`/LOS collision can read garbage
  prop indices and crash on the first post-load tick.
  Do not deregister/register individual props while the save-state loader is
  still replacing props: after the reset, the chunk tables are intentionally
  discarded, and mid-load registration can rebuild them from transient
  active-list or parent/child relationships. The load path should restore each
  prop's local `rooms[]` payload only, then perform one final room-list rebuild
  from the completed active root list.
  Prod GCC can also lower sentinel-fill loops such as `s16_array[i] = -1` into
  `memset(dst, 0xff, len)`. In this codebase `memset` is an alias of libultra's
  `bzero` implementation and ignores the fill byte, so nonzero generated
  `memset` calls zero the table instead. Use explicit `volatile s16 *` stores
  (or another verified no-`memset` pattern) for nonzero sentinel fills in
  GCC-built practice code, and confirm prod disassembly when adding similar
  bulk initialization.
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
- **Aircraft Model Animation State**: Aircraft props such as Runway's plane are
  driven by their `Model` animation controller, not only by `AircraftRecord`.
  Restore `Model::anim`, frame, speed, loop, play-rate, and root RW data for
  aircraft objects; otherwise loading across the plane cutscene keeps stale or
  reset animation state and the plane can disappear or desynchronize. Clear
  `AircraftRecord::Sound`/`VehichleRecord::Sound` on load because those
  `ALSoundState` handles are dynamic and all SFX are stopped before restoring.
- **Texture Inflation Must Fit the Main-Thread Stack**: Save-state model
  recreation calls the texture loader from a much deeper stack than normal
  level setup. `texLoad` and `texInflateNonZlib` already use about 16 KiB of
  local buffers; the original `texInflateHuffman` adds another fixed 12 KiB
  even when its alphabet has only 16 entries. The full GCC migration changed
  `image.c` from IDO to GCC, so stack-frame behavior here must be audited rather
  than assumed equivalent. In practice builds, size the Huffman frequency/tree
  arrays from the actual `chansize`. Otherwise the 32 KiB main-thread stack can
  grow below `sp_main`; initializing the overflowing node table to `-1` then
  fills the adjacent scheduler stack with `0xffffffff`, making thread 2 return
  to that address.

## Struct Analysis

Keep all the properties of the structs and global variables we save and load in files in this directory. Document them and their properties, along with whether they affect gameplay, their status in the save/load code and any other important information. Use this as a guide when reading source code to understand what each property does.

If you are missing any information about a struct or variable, mark with a TODO comment what should still be investigated and documented.
