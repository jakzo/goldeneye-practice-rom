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

- Saving object deformation vertices can cost 20kb+, can we save the seeds used to deform them instead?
- Lighting that slowly changes regardless of time scale (not sure if part of state or time scale bug?)

## Key Learnings

Add any general advice helpful for future agents working on this feature here. Be sure to read and understand these before starting work on handling more state in the system.

- **Implement one feature at a time then manually test**: Since crashes and hangs are so common and can be unexpectedly caused by even seemingly innocuous changes, implement a small number of safe properties in the save/load code. After this, update this file and report back to the user what was added, what was learned and what they should now do in the game to test that the newly added properties are loaded correctly.
- **Stale Pointers**: Any struct member ending in `*` (e.g., `ALSoundState *`, `ObjectRecord *`, `PropRecord *`) is an absolute memory address. If the game engine deallocates or reallocates the target object, loading a saved state that retains the old pointer will cause a crash. All such pointers must either be relocated (mapped back to correct indices) or it will not affect gameplay nullified (set to `NULL`).
- **Cross-Stage Teardown Must Precede Allocator Reset**: After a replay/level
  hop, prop convenience pointers can still name allocations from the previous
  stage lifecycle even though the new stage pool is already in place. Validate
  ownership and RDRAM range before following them, and destroy all stale CHR
  models before resetting the shared `0xCCCC` blood-vertex allocator. Model
  destruction normally releases blood clones; resetting first lets those old
  frees corrupt the fresh allocator. The same rule applies to object cleanup:
  never clear projectile/embedment fields through stale `ObjectRecord` pointers
  after a stage hop, because their addresses can alias the new setup's AI data.
- **Packed STAN Ends at Its Own Terminator**: `D_80040F60` is relocation
  scratch and can point at a different STAN processed later during stage load.
  Derive the mutable packed collision payload by walking from
  `standTileStart + 0x80` with `list_of_tilesizes` until the eight-byte zero
  terminator. Ignore the mutable visited bit when testing the first terminator
  word. Saving only through the scratch pointer omitted valid Frigate tiles and
  changed guard collision/line walks after a hop.
- **Pathfinder Lists Are Mutable State**: AI path searches mutate waypoint and
  waygroup distances, group assignments, neighbour lists, and waypoint lists.
  Some searches destructively move a `-1` terminator earlier, so restoring only
  the surviving entries leaves a permanently truncated graph. Serialize every
  list's values and saved terminator position, then restore this state after
  props because CHR reconstruction can invoke navigation helpers.
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
  controls, and timers. The frame containing a deferred load is not equivalent
  to the saved frame's ordinary timing setup, so restore `g_GlobalTimer`,
  `g_ClockTimer`, `g_GlobalTimerDelta`, `g_JP_GlobalTimerDelta` when present,
  the practice time-scale accumulator `g_FractionalClockTimerAcc`, and
  `D_80048380` exactly. CHR perception timestamps, weapon timers, movement,
  effects, and AI all consume this shared timing state before it is recomputed.
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
- **Sky and Fog State**: Cloud and water geometry is rebuilt every frame from
  the player camera and `CurrentEnvironmentRecord`; there are no persistent
  vertex or shape buffers to restore. `g_SkyCloudOffset` is the independently
  advancing cloud/water texture phase. Fog additionally depends on its derived
  scaled distances/intensities, `g_CurFogDetails`, the VI Z range, near-fog
  selection, and the active main/alternate environment pair. Restore all of
  these together with the complete pointer-free current environment (including
  any interpolated alternative sky) and `g_FogSkyIsEnabled`; restoring only the
  visible record leaves later intro/swirl fog calculations active after load.
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
- **SFX Volume and Practice Pause**: Sound effects advance on the audio clock,
  not `g_ClockTimer`. Practice pause snapshots and mutes the six game SFX slot
  volumes, leaving the seventh slot for practice feedback, then restores each
  game slot on resume. The active sound graph keeps running silently, so
  continuous effects such as sliding doors, alarms, vehicles, and toxic gas
  remain available without maintaining an owner-by-owner list. The death
  replay sets the separate global `g_sndSfxVolumeScale` to `0.5`; level setup
  normally resets it to `1.0`. Save and restore that scale. During a paused
  hotkey load, assign the restored scale without applying it to the temporarily
  zeroed natural slot volumes; `practice_sfx_resume` applies it when restoring
  those volumes.
- **A Practice-Paused Frame Is Render-Only**: Loading through the hotkey is
  deferred until graphics tasks have drained, then the restored state is shown
  while practice time remains paused. Do not run gameplay side effects during
  this frame merely because their timer delta is zero. In particular, an
  explosive weapon can trigger when its timer equals a threshold without first
  decrementing it, so `chrobjWeaponTick` and paused NPC object-hit detonation
  must wait for the first resumed gameplay tick. This guard is specific to
  `g_IsTimePaused`; ordinary base-game zero-delta frames remain part of normal
  simulation.
- **Render-Only Refresh Must Be Transactional**: Rendering mutates gameplay-
  visible caches even with a zero clock delta: prop onscreen flags, CHR onscreen
  counts, watch UI state, model `render_pos` pointers and root RW data, the
  render-joint free list, room-transform mappings, and float matrices converted
  in place to N64 fixed point. Snapshot and restore these around practice-only
  renders. After a load, preserve the serialized current and previous camera
  matrices plus projection matrices until the live frame consumes them; do not
  replace a CHR weapon's saved gameplay matrix from a render-only refresh.
  When a gameplay matrix must be reconstructed, use the game's 65535-based
  `matrix_4x4_f32_to_s32`/`sub_GAME_7F058E78` round trip, not libultra's 16.16
  `guMtxF2L`/`guMtxL2F` pair.
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
  `redarken_lights_in_room` when those vertices become resident again. Hotkey
  loads are deferred until all graphics tasks have drained, so it is then safe
  to reconcile BG allocations: free rooms absent from the save, discard partial
  allocations, and synchronously reload every saved-resident room. Keeping all
  later-timeline allocations can exhaust the BG allocator; restoring only the
  logical `model_bin_loaded` flag leaves blue voids, unloaded rooms, or invalid
  display-list/texture metadata. Airborne glass shards from these lights are
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
- **Room Prop Lists Need Safe Rebuild and Exact Final State**:
  `RoomPropListChunkIndexes` and `RoomPropListChunks` are room-to-prop lookup
  indexes built by `chrpropRegisterRooms`. Loading gameplay from a different
  lifecycle moment, such as an ending cutscene, can leave the live tables stale
  or corrupted even when each restored `PropRecord::rooms` list is correct.
  Clear the tables before destructive prop replacement and rebuild a safe
  intermediate table after props, attachments, and the player/viewer prop have
  been restored. Then restore the saved tables exactly: chunk allocation and
  entry order can affect `roomGetProps` traversal and downstream gameplay.
  The serialized tables use signed variable-length entries because their
  `-1` sentinels and small prop/chunk indices otherwise waste scarce SRAM.
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
- **Clearing Attached Props Must Unlink Them First**: The fallback path for an
  invalid or already-released object cannot call `objDetach`, because that
  helper dereferences the missing object/model. It must still use
  `chrpropDetach` before clearing or freeing the `PropRecord`, and clear any
  matching CHR weapon/hat convenience pointer. Otherwise the parent retains a
  child whose object pointer is `NULL`, and normal CHR teardown later crashes
  in `objFree`.
- **Model Frame Cache Pointers Need Their Shared Buffer**: `Model::unk34`,
  `unk38`, `unk64`, and `unk68` point into the 720-byte
  `animations_frame_buffer`. Offscreen movement can consume these decoded
  frames before the next matrix calculation. Restore the pointers and the
  exact buffer bytes, and restore the buffer only after prop/model recreation
  has finished decoding its own frames. Clearing the pointers loses root
  motion; restoring pointers alone reads unrelated destination-stage data.
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
- **Character Model Definitions Are Shared Mutable State**: Body and head
  `ModelFileHeader`/`ModelNode` definitions are shared across all CHR instances.
  `modelAttachPart` mutates their parent/head links, RW-data indices, and
  `numRecords`, so rebuilding guards in a different order can change the global
  definition graph even when each instance appears valid. Serialize the exact
  definition state and restore it only after every CHR has been allocated.
  `Model::Type`, the `datas` allocation, attached-head record/offset, root RW
  data, and collision bounds remain per-instance state. Never validate a saved
  `Model::Type` against the final shared header count or re-run per-instance
  parent-link restoration after installing the saved global indices. The model
  pool slot must have at least the saved `Type` capacity. Finally rebuild all
  display-list base addresses and writable pointers from the current loaded
  body/head definitions, including hidden switch branches.
- **Never Restore the Prop Free List From Saved Indices — Rebuild It**: Free-list entries have no saved record, so their `prev` links — which chain the free list together — are never written. Restoring `ptr_obj_pos_list_final_entry` from the saved head index therefore points the free list into stale pre-load links. `chrpropAllocate` can then hand out slots that are actually in use, corrupting the prop graph and eventually crashing. Rebuild the free list after props and attachments are restored, but do not equate disabled with free: respawning pickups are disabled while remaining on the active list, and inactive setup objects can remain bound through `obj->prop`. Serialize every active-list prop regardless of `PROPFLAG_ENABLED`; a slot is free only when it was not saved active, has no parent, and has no live CHR/object/explosion/smoke owner. The `prev`-chain integer round-trip through `get_prop_index`/`get_prop_by_index` cannot preserve a misaligned pointer, so any misalignment seen in memory is live corruption rather than saved data.
- **Active Records Must Clear Live Parents**: Every serialized prop is reached
  through the active list and therefore had `parent == NULL` at save time. If
  the prop is later collected, its live record is delisted and reparented to
  the player. Loading its common fields without detaching that live parent
  makes the final active-list rebuild skip it; the preceding prop keeps
  `next == NULL`, orphaning every later prop from rendering. Detach any current
  parent before restoring a saved active record.
- **Player/Viewer Children Need Allocation Reconciliation**: Bond's third-
  person/viewer model and first-person weapon buffers can be rebuilt while
  loading a different lifecycle point. Detach saved children before that
  rebuild, remove any replacement children created by `solo_char_load`, then
  transfer the generated model allocation to the saved weapon prop and rebuild
  the saved attachment graph. Reusing only the old prop/object payload leaves
  stale model-buffer pointers; keeping both children duplicates the held weapon
  and corrupts ownership links. Do not include player/viewer parents in the
  earlier ordinary-prop child-graph pass. Their saved child props are only
  placeholders until the hand buffers have been regenerated. Assert that every
  child belongs to its saved parent before installing the link.
- **Character Slot Ownership Includes Viewers**: When normalizing stale
  `g_ChrSlots`, accept two-way ownership from `PROP_TYPE_CHR`,
  `PROP_TYPE_PLAYER`, and `PROP_TYPE_VIEWER`. A live third-person Bond model
  uses a viewer-owned CHR slot. Marking that slot free while the viewer still
  references it lets a later `chrAllocate` overwrite `chr->prop`, causing the
  next player collision-room update to follow an unrelated pointer.
- **Validate Complete Model Node Graphs Before Reuse**: A `Model`, its header,
  and its root node can remain in RDRAM while a descendant node still contains
  stale `Data`, `Parent`, sibling, or child pointers into an overwritten hand
  buffer. Validate every reachable node and link with a traversal bound before
  treating a live third-person Bond model as reusable. If validation fails,
  discard and rebuild the model rather than passing the graph to model setup,
  whose pointer-promotion and RW-data walks assume every descendant is valid.
- **Dynamic Collision Coefficients Are Authoritative**: Object collision
  geometry is rebuilt from the restored transform, but the cached coefficient
  words at `collision_data::unk24` are not reproduced bit-identically. Save the
  0x20-byte coefficient tail when dynamic collision data exists and apply it
  after `chrobjCollisionRelated`; otherwise the next hit/LOS query can diverge
  despite matching visible geometry.
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
- **Storage Is Independent From Serialization**: Save-state fields serialize
  through `StateStream`; the concrete `StorageStream` can target cartridge SRAM,
  volatile Expansion Pak RAM, or a flashcart SD card. Select it with
  `practice_states_set_storage_location`. SRAM is the default and starts at
  `SAVE_STATE_SRAM_OFFSET`, leaving exactly
  `SRAM_SIZE_BYTES - SAVE_STATE_SRAM_OFFSET` bytes for the serialized state.
  Expansion Pak storage uses the upper 2 MiB starting at uncached alias
  `0xa0600000` (physical `0x00600000`), clear of the framebuffer boundary and
  graphics scratch use, and is available only when `osMemSize` reports the full
  8 MiB. Flashcart storage streams each state to a numbered file under
  `/goldeneye_practice_rom/save_states` and retains the configured maximum of
  1–99 files. The stream reports an error rather than overrunning a fixed-size
  backend. Prefer stable IDs, seeds, occupancy maps, compact deltas, and state
  from which larger buffers can be reproduced; do not serialize allocator
  padding or renderer output merely because a larger backend is available.
  Replay-file save-state tests select Expansion Pak RAM so their state data
  cannot overwrite the replay fixture. The much larger host `.state` replay
  sidecars are per-frame diagnostic timelines and do not consume runtime save
  storage.

## Struct Analysis

Keep all the properties of the structs and global variables we save and load in files in this directory. Document them and their properties, along with whether they affect gameplay, their status in the save/load code and any other important information. Use this as a guide when reading source code to understand what each property does.

If you are missing any information about a struct or variable, mark with a TODO comment what should still be investigated and documented.
