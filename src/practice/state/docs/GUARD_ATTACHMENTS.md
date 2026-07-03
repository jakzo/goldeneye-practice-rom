# Guard Attachments

This document describes how GoldenEye's prop, object, model, and character
records combine to represent guard attachments. It is written for save/load
work: restoring an attachment means restoring the prop-tree relationship, the
object/model relationship, the guard convenience pointers, and the enabled/list
state consistently.

## Source Map

- `PropRecord`, `ChrRecord`, `ObjectRecord`, and `WeaponObjRecord` live in [`src/bondtypes.h`](../../../bondtypes.h#L2176).
- Prop flags, runtime flags, drop types, prop types, and propdef types live in [`src/bondconstants.h`](../../../bondconstants.h#L328).
- Prop allocation, active-list membership, parent/child links, enable/disable,
  and room registration live in [`src/game/chrprop.c`](../../../game/chrprop.c#L254).
- Guard allocation and guard-side attachment fields are initialized in
  [`src/game/chr.c`](../../../game/chr.c#L2600).
- Setup-time object and weapon assignment is in [`src/game/prop.c`](../../../game/prop.c#L137).
- Object init, embed, drop, detach, hat assignment, and weapon assignment are in
  [`src/game/chrobjhandler.c`](../../../game/chrobjhandler.c#L1374).
- AI commands that equip/drop attached objects are handled in
  [`src/game/chrai.c`](../../../game/chrai.c#L2332).

## Mental Model

There are four independent layers:

1. `PropRecord`: the live world handle. It can be on the global active list, or
   attached under another prop through `parent/child/prev/next`.
2. `ObjectRecord` / `WeaponObjRecord` / `HatRecord`: the setup/runtime entity
   backing the prop. This owns `flags`, `flags2`, `state`, `runtime_bitflags`,
   `model`, `prop`, position, matrix, health, projectile/embedment pointers, and
   type-specific fields.
3. `Model`: the render/animation instance. A model can be attached to another
   model with `attachedto` and `attachedto_objinst`.
4. `ChrRecord`: the guard state. It stores convenience pointers for visible
   held weapons and hats: `weapons_held[GUNRIGHT]`, `weapons_held[GUNLEFT]`,
   and `handle_positiondata_hat`.

The prop tree and model attachment are related but not the same thing. A
concealed weapon is a child prop of a guard, but it may not have a hand model
attachment. A held weapon is both a child prop and a model attached to a hand
switch. An embedded object is a child prop and a model attached to a specific
node, but normal concealed/held item drop paths intentionally skip embedded
items.

## Core Struct Fields

### `PropRecord`

Defined at [`src/bondtypes.h`](../../../bondtypes.h#L2176).

- `type`: runtime prop kind. Relevant values are `PROP_TYPE_CHR`,
  `PROP_TYPE_VIEWER`, `PROP_TYPE_OBJ`, and `PROP_TYPE_WEAPON` from
  [`src/bondconstants.h`](../../../bondconstants.h#L3076).
- `flags`: runtime prop flags. `PROPFLAG_ENABLED` controls ticking/world
  participation, and `PROPFLAG_ONSCREEN` is a frame-derived visibility/render
  flag. Both are defined in [`src/bondconstants.h`](../../../bondconstants.h#L328).
- `timetoregen`: respawn countdown. This can tick on a disabled but still active
  prop.
- `chr` / `obj` / `weapon`: union pointer to the backing entity.
- `pos`: world position. For attached props this is not the authoritative
  rendered position while attached; model attachment supplies render placement.
- `stan`: stand tile. `chrpropReparent` clears this to `NULL` when an object is
  attached to another prop.
- `parent`: non-`NULL` when attached under another prop.
- `child`: newest child in the attachment chain.
- `prev` / `next`: overloaded. For root props they are the global active-list
  links. For attached props they are sibling links.
- `rooms`: room registration used for culling/collision on active world props.
  Attached props are usually deregistered or have `stan == NULL`.

Important routines:

- `chrpropAllocate` initializes a free prop with no parent/child/list links,
  zero flags, no stan, and `rooms[0] = 0xff`
  ([`src/game/chrprop.c`](../../../game/chrprop.c#L279)).
- `chrpropActivate` appends a root prop to the active list
  ([`src/game/chrprop.c`](../../../game/chrprop.c#L313)).
- `chrpropActivateThisFrame` prepends a root prop to the active list
  ([`src/game/chrprop.c`](../../../game/chrprop.c#L335)).
- `chrpropDelist` removes a root prop from the active list and clears
  `prev/next` ([`src/game/chrprop.c`](../../../game/chrprop.c#L353)).
- `chrpropEnable` and `chrpropDisable` only set/clear `PROPFLAG_ENABLED`;
  they do not change list membership
  ([`src/game/chrprop.c`](../../../game/chrprop.c#L254)).
- `chrpropReparent` sets `newChild->parent = host`, links it as `host->child`,
  clears `newChild->stan`, and reuses `prev/next` as sibling links
  ([`src/game/chrprop.c`](../../../game/chrprop.c#L384)).
- `chrpropDetach` unlinks from the parent child chain and clears
  `parent/prev/next` ([`src/game/chrprop.c`](../../../game/chrprop.c#L402)).

### `Model`

Defined at [`src/bondtypes.h`](../../../bondtypes.h#L1445).

- `chr`: owning guard for body models, otherwise usually `NULL`.
- `obj`: model file header.
- `render_pos` and `datas`: per-frame/per-node model runtime data.
- `scale`: current model scale.
- `attachedto`: parent model when attached.
- `attachedto_objinst`: parent model node/switch used as the attachment point.

For guard attachments, `attachedto` usually points at `chr->model`.
`attachedto_objinst` is commonly one of the guard model switches:

- right hand: `chr->model->obj->Switches[3]`
- left hand: `chr->model->obj->Switches[5]`
- hat/head: `chr->model->obj->Switches[6]`

Those hand/head assignments are made in `chrEquipWeapon` and `hatApplyToChr`
([weapon](../../../game/chrobjhandler.c#L37208),
[hat](../../../game/chrobjhandler.c#L36632)).

### `ChrRecord`

Defined at [`src/bondtypes.h`](../../../bondtypes.h#L2275). Guard attachment
fields are initialized in [`init_GUARDdata_with_set_values`](../../../game/chr.c#L2600):

- `prop`: guard's own prop.
- `model`: guard body/head model.
- `hidden`: includes `CHRHIDDEN_DROP_HELD_ITEMS`, which schedules pending
  detach/drop processing after `propobjSetDropped` marks a child projectile
  drop type. The flag is defined in
  [`src/bondconstants.h`](../../../bondconstants.h#L205).
- `weapons_held[3]`: right, left, and a third slot noted as hat in comments.
  Runtime code uses indices `GUNRIGHT` and `GUNLEFT` for visible held weapons.
- `handle_positiondata_hat`: prop for the worn hat.

The key distinction is that `weapons_held[]` is not the complete list of
attached weapons. Concealed weapons are children of `chr->prop` but are not in
`weapons_held[]`.

### `ObjectRecord`

Defined at [`src/bondtypes.h`](../../../bondtypes.h#L2508).

- Header: `extrascale`, `state`, and setup type. `state` includes damage,
  respawn, activated, and destroyed bits
  ([`src/bondconstants.h`](../../../bondconstants.h#L532)).
- `obj`: model/object ID.
- `pad`: setup pad, guard ID, or relative object offset depending on flags.
- `flags`: setup/configuration. Relevant attachment bits:
  `PROPFLAG_ASSIGNEDTOCHR`, `PROPFLAG_INSIDEANOTHEROBJ`,
  `PROPFLAG_CONCEAL_GUN`, `PROPFLAG_WEAPON_LEFTHANDED`,
  `PROPFLAG_AIUNDROPPABLE`, and `PROPFLAG_IS_DOUBLE`
  ([`src/bondconstants.h`](../../../bondconstants.h#L328)).
- `flags2`: load filters and interaction/collision behavior.
- `prop`: back pointer to the live prop.
- `model`: backing model.
- `mtx`: scale/rotation matrix.
- `runtime_pos`: authoritative world position when not attached.
- `runtime_bitflags`: dynamic object state, including `RUNTIMEBITFLAG_HASOWNER`,
  `RUNTIMEBITFLAG_EMBEDDED`, `RUNTIMEBITFLAG_DEPOSIT`, and
  `RUNTIMEBITFLAG_REMOVE` ([`src/bondconstants.h`](../../../bondconstants.h#L553)).
- projectile/embedment pointers: used when an object is thrown, dropped, or
  embedded. The struct fields are lower in `ObjectRecord`; projectile/drop code
  treats them as `obj->projectile` and `obj->embedment`.

### `WeaponObjRecord`

Defined at [`src/bondtypes.h`](../../../bondtypes.h#L3073). It inherits
`ObjectRecord` and adds:

- `weaponnum`: item ID used for inventory/fire behavior.
- `LinkedWeaponType`: secondary relationship.
- `timer`: mine/grenade/other timer-like field.
- `dualweapon`: pointer to paired weapon object when dual-wielded.

Weapon props use `PROP_TYPE_WEAPON`, not `PROP_TYPE_OBJ`, after
`sub_GAME_7F051DD8` or `complete_object_data_block_return_position_entry`
sets the prop type and hides gunfire
([`src/game/chrobjhandler.c`](../../../game/chrobjhandler.c#L37181)).

## Attachment States

### Root, Active, Enabled

A normal object on the ground has:

- `prop->parent == NULL`
- `prop` is linked into the active prop list through `prev/next`
- `prop->flags & PROPFLAG_ENABLED`
- `obj->prop == prop`
- `obj->model` allocated
- `obj->runtime_pos` and `prop->pos` match
- rooms registered

`domakedefaultobj` creates this state by initializing the model/prop, placing it,
calling `setupUpdateObjectRoomPosition`, then activating and enabling it
([`src/game/prop.c`](../../../game/prop.c#L137),
[`src/game/prop.c`](../../../game/prop.c#L472)).

### Root, Active, Disabled

Disabled is not the same as free or delisted. `chrpropDisable` only clears
`PROPFLAG_ENABLED` ([`src/game/chrprop.c`](../../../game/chrprop.c#L261)).
Respawning pickups and temporarily inactive props can still have an `obj->prop`
binding and active-list membership so future code can re-enable or regenerate
them. Save/load must preserve disabled-but-live props instead of treating
`PROPFLAG_ENABLED == 0` as free.

### Attached To Guard, Concealed

A concealed guard attachment has:

- `prop->parent == chr->prop`
- linked in `chr->prop->child` chain
- not on the global active list
- usually `PROPFLAG_ENABLED` cleared if it came from `AI_ChrEquipObject`
  world-to-guard flow
- `model->attachedto_objinst == NULL` unless another path set it
- not referenced by `chr->weapons_held[]`

`chrEquipWeapon` always reparents the weapon prop to the guard, but it only sets
the hand model attachment and `weapons_held[hand]` when
`PROPFLAG_CONCEAL_GUN` is not set and the hand is free
([`src/game/chrobjhandler.c`](../../../game/chrobjhandler.c#L37222)).

Concealed items are dropped by `chrDropItems`, which iterates `self->prop->child`
and excludes the hat plus currently held left/right weapons
([`src/game/chr.c`](../../../game/chr.c#L6716)).

### Attached To Guard, Held In Hand

A held weapon has all concealed properties plus:

- `chr->weapons_held[GUNRIGHT] == prop` or `chr->weapons_held[GUNLEFT] == prop`
- `prop->obj->model->attachedto == chr->model`
- `attachedto_objinst` is switch 3 for right hand or switch 5 for left hand
- gunfire visibility is controlled through weapon helpers

`chrEquipWeapon` refuses to occupy a hand that already has a held weapon and
returns `FALSE`, allowing callers to fall back to concealed attachment
([`src/game/chrobjhandler.c`](../../../game/chrobjhandler.c#L37227)).

### Attached Hat

A hat is an object prop attached to the guard's head:

- `chr->handle_positiondata_hat == prop`
- `prop->parent == chr->prop`
- `hat->model->attachedto == chr->model`
- `hat->model->attachedto_objinst == chr->model->obj->Switches[6]`

This is set by `hatApplyToChr`
([`src/game/chrobjhandler.c`](../../../game/chrobjhandler.c#L36632)).
Head hit logic can mark it for `DROPTYPE_HAT` and set
`CHRHIDDEN_DROP_HELD_ITEMS` ([`src/game/chrlv.c`](../../../game/chrlv.c#L2638)).

### Embedded In Object Or Character

An embedded object has:

- `obj->runtime_bitflags & RUNTIMEBITFLAG_EMBEDDED`
- `obj->embedment` allocated
- `prop` deregistered, delisted, and disabled
- `obj->model->attachedto` and `attachedto_objinst` set to the hit parent model
  and node
- `prop->parent` set to the parent prop

`objEmbed` creates this state
([`src/game/chrobjhandler.c`](../../../game/chrobjhandler.c#L4624)). Embedded
props are rendered/ticked through their parent when the parent is onscreen; the
embedded render path computes the child model matrix from the parent node matrix
and saved embedment matrix
([`src/game/chrobjhandler.c`](../../../game/chrobjhandler.c#L5000)).

AI comments correctly warn that embedded items are not dropped by normal
held/concealed item drop commands. The drop functions operate on projectile
drop type and later detach/drop behavior; embedded objects without a projectile
are not normal held items.

### Dropped / Projectile Deposit

When a guard drop is requested, the first step is usually `propobjSetDropped`.
That does not immediately detach the prop. It sets `projectile->droptype` on an
embedded/deposit projectile if one exists
([`src/game/chrobjhandler.c`](../../../game/chrobjhandler.c#L29389)). Callers
then set `chr->hidden |= CHRHIDDEN_DROP_HELD_ITEMS` so the character logic later
performs the actual drop processing.

`objDrop` converts an embedded projectile to a deposited projectile, computes
velocity/orientation based on `DROPTYPE_*`, calls `objDetach`, activates and
enables the prop as a root world prop, writes `prop->pos`/`obj->runtime_pos`,
and leaves it in projectile/deposit state
([`src/game/chrobjhandler.c`](../../../game/chrobjhandler.c#L29452)).

Drop types are defined in [`src/bondconstants.h`](../../../bondconstants.h#L489):

- `DROPTYPE_DEFAULT`
- `DROPTYPE_SURRENDER`
- `DROPTYPE_THROWGRENADE`
- `DROPTYPE_HAT`

## Setup-Time Paths

### Guard Creation

Guards are loaded before assigned objects in the main setup pass. `setup_props`
dispatches `PROPDEF_GUARD` to `expand_09_characters`
([`src/game/prop.c`](../../../game/prop.c#L1608)). Runtime spawned guards go
through `chrSpawnAtCoord`, which calls `chrAllocate`, activates the guard prop
this frame, and enables it
([`src/game/chrlv.c`](../../../game/chrlv.c#L11220)).

`chrAllocate` calls `init_GUARDdata_with_set_values`
([`src/game/chr.c`](../../../game/chr.c#L2737)). That initializer:

- assigns `prop->type = PROP_TYPE_CHR`
- links `prop->chr`, `chr->prop`, `chr->model`, and `model->chr`
- sets position, stan, heading, animation speed, collision defaults, AI fields,
  health, shading, and all attachment convenience pointers
- clears `weapons_held[0]`, `weapons_held[1]`, `weapons_held[2]`, and
  `handle_positiondata_hat`

### Assigned Object To Guard

An object with `PROPFLAG_ASSIGNEDTOCHR` uses `pad` as a literal guard ID. In
`domakedefaultobj`, this path initializes the object prop/model, scales it, and
calls `chrpropReparent(var_v0, chr->prop)`
([`src/game/prop.c`](../../../game/prop.c#L191)). It does not activate or enable
the prop as a root world prop.

### Assigned Weapon To Guard

Setup `PROPDEF_COLLECTABLE` records are handled by `weaponAssignToHome`
([`src/game/prop.c`](../../../game/prop.c#L501)). If
`PROPFLAG_ASSIGNEDTOCHR` is set:

1. Find the guard by literal ID from `weapon->pad`.
2. Load projectile models.
3. Call `sub_GAME_7F052030`, which normalizes damage then calls weapon load/apply.
4. `sub_GAME_7F051F30` initializes the weapon prop/model, scales it, and calls
   `chrEquipWeapon`.

`chrEquipWeapon` is the central classification point: no conceal flag plus free
hand means visible held weapon; conceal flag or occupied hand means child prop
without hand ownership
([`src/game/chrobjhandler.c`](../../../game/chrobjhandler.c#L37208)).

### Assigned Hat To Guard

Setup `PROPDEF_HAT` records go through `setupHat`. With
`PROPFLAG_ASSIGNEDTOCHR`, it finds the guard and calls `hatAssignToChr`
([`src/game/prop.c`](../../../game/prop.c#L620)). The actual link is made by
`hatApplyToChr` as described above.

### Inside Another Object

Objects with `PROPFLAG_INSIDEANOTHEROBJ` are created first but only fully linked
in a second setup pass. The second pass finds the target object by relative
setup index, sets `RUNTIMEBITFLAG_HASOWNER`, rescales, and reparents the prop
under the owner prop ([`src/game/prop.c`](../../../game/prop.c#L1907)).

This is object-contained attachment, not necessarily guard-contained attachment,
but the same prop tree rules apply.

## Runtime / AI Paths

### Drop Tagged Object From Character

`AI_DropObject` finds the tagged object and requires its parent to be a
character prop. It calls `propobjSetDropped(obj->prop, DROPTYPE_SURRENDER)` and
sets `CHRHIDDEN_DROP_HELD_ITEMS`
([`src/game/chrai.c`](../../../game/chrai.c#L2332)).

This path marks the drop; it does not immediately remove the prop from the
child chain.

### Drop All Concealed Items

`AI_ChrDropAllConcealedItems` calls `chrDropItems`
([`src/game/chrai.c`](../../../game/chrai.c#L2345)). `chrDropItems` walks
`self->prop->child`, skipping `handle_positiondata_hat` and the two
`weapons_held[]` props, and calls `propobjSetDropped(childprop,
DROPTYPE_DEFAULT)` unless `PROPFLAG_AIUNDROPPABLE` is set
([`src/game/chr.c`](../../../game/chr.c#L6716)). It then sets
`CHRHIDDEN_DROP_HELD_ITEMS`.

### Drop All Held Items

`AI_ChrDropAllHeldItems` only checks `weapons_held[GUNRIGHT]` and
`weapons_held[GUNLEFT]`, sets each to `DROPTYPE_DEFAULT`, and sets
`CHRHIDDEN_DROP_HELD_ITEMS`
([`src/game/chrai.c`](../../../game/chrai.c#L2356)).

### Equip Tagged Object To Character

`AI_ChrEquipObject` handles moving a tagged object from the world or another
parent to a guard ([`src/game/chrai.c`](../../../game/chrai.c#L2388)):

1. If `obj->prop->parent` exists, call `objDetach`.
2. Otherwise deregister rooms, delist from the active list, and disable.
3. If it is a collectable and `chrEquipWeapon` succeeds, that function handles
   held/concealed classification.
4. Otherwise directly `chrpropReparent(obj->prop, chr->prop)`.

This is the clearest example of a prop that is disabled but still attached and
potentially active later after a drop.

### Give Weapon To Character

AI `TRYGiveMeItem`/related command paths call `chrGiveWeapon`
([`src/game/chrai.c`](../../../game/chrai.c#L3549)), which creates or reuses a
weapon object slot and eventually calls the same `sub_GAME_7F051F30` and
`chrEquipWeapon` path ([`src/game/chrobjhandler.c`](../../../game/chrobjhandler.c#L37677)).

### Surrender / Death / Hat Knockoff

Surrender sets held weapons to `DROPTYPE_SURRENDER`, sets
`CHRHIDDEN_DROP_HELD_ITEMS`, then calls `chrDropItems` for concealed items
([`src/game/chrlv.c`](../../../game/chrlv.c#L800)).

Damage/death paths can mark held weapons or hats for dropping:

- held weapons during hit/death logic
  ([`src/game/chrlv.c`](../../../game/chrlv.c#L2308))
- hat knockoff as `DROPTYPE_HAT`
  ([`src/game/chrlv.c`](../../../game/chrlv.c#L2638))

## Detach And Drop Mechanics

`objDetach` is the authoritative detach helper for object props
([`src/game/chrobjhandler.c`](../../../game/chrobjhandler.c#L29411)):

1. If there is no parent, do nothing.
2. Call `chrpropDetach` to unlink from the parent child chain.
3. Clear `model->attachedto_objinst`.
4. Clear `RUNTIMEBITFLAG_HASOWNER`.
5. If the parent is a character/viewer, clear guard convenience pointers:
   `handle_positiondata_hat`, `weapons_held[GUNRIGHT]`, or
   `weapons_held[GUNLEFT]`, and stop firing for a cleared weapon hand.

Notice that `objDetach` clears `attachedto_objinst` but not `attachedto`.
Code that checks actual attachment generally uses `attachedto_objinst`; save/load
should still restore or clear both fields coherently to avoid stale parent
model pointers becoming misleading.

`objDrop` handles converting from attached to root projectile/world state
([`src/game/chrobjhandler.c`](../../../game/chrobjhandler.c#L29452)). After
matrix/collision work, it calls `objDetach`, then `chrpropActivate` and
`chrpropEnable`, writes world position, and leaves the object active as a root
prop.

## Render And Tick Behavior

Root props are considered for rendering when enabled and onscreen.
`chraiUpdateOnscreenPropCount` adds props whose flags contain both
`PROPFLAG_ENABLED` and `PROPFLAG_ONSCREEN`
([`src/game/chrprop.c`](../../../game/chrprop.c#L193)).

Rendering dispatch uses `prop->type`:

- characters go to `chrRenderProp`
  ([`src/game/chrprop.c`](../../../game/chrprop.c#L434))
- objects/weapons/doors go to `chrobjRenderProp`
  ([`src/game/chrprop.c`](../../../game/chrprop.c#L444))

Guard rendering reads held weapon and hat pointers from `ChrRecord`
([`src/game/chr.c`](../../../game/chr.c#L6777)). Object rendering uses model
state and object opacity/damage state
([`src/game/chrobjhandler.c`](../../../game/chrobjhandler.c#L27924)).

Recursive object tick/render helper paths walk child chains using `child` then
`prev`, not `next`, because `host->child` is the newest child and `prev` points
toward older siblings. Examples:

- offscreen/weapon tick recursion
  ([`src/game/chrobjhandler.c`](../../../game/chrobjhandler.c#L4974))
- embedded onscreen recursion
  ([`src/game/chrobjhandler.c`](../../../game/chrobjhandler.c#L5000))

## Defined Versus Calculated State

### Mostly Defined / Must Restore

- Backing object identity: setup index or generated slot identity.
- `prop->type`, union target (`chr`/`obj`/`weapon`), `obj->prop`, `obj->model`,
  `chr->prop`, `chr->model`.
- Attachment relationship: `prop->parent`, parent `child`, sibling order
  through `prev`/`next`.
- Guard convenience pointers: `weapons_held[]`, `handle_positiondata_hat`.
- Model attachment: `model->attachedto`, `model->attachedto_objinst`.
- Setup/runtime flags that choose behavior: object `flags`, `flags2`, `state`,
  `runtime_bitflags`, weapon fields, projectile/embedment state.
- Enabled/list state: whether the prop is a root active-list prop, an attached
  child, disabled-but-live, or free.
- `CHRHIDDEN_DROP_HELD_ITEMS` and pending projectile `droptype` if a drop is
  already scheduled but not processed.

### Calculated / Rebuildable

- Room registration for root props can be recalculated from object matrix and
  position by `setupUpdateObjectRoomPosition`
  ([`src/game/loadobjectmodel.c`](../../../game/loadobjectmodel.c#L250)).
  Attached props usually clear `stan` and are not normal room-registered roots.
- `PROPFLAG_ONSCREEN` is per-frame visibility. Restoring it exactly is usually
  less important than letting the next visibility pass recalculate it, except
  embedded render/drop code can branch on it during the same frame.
- Model relation data can be recalculated with `modelUpdateRelationsQuick`
  ([`src/game/model.c`](../../../game/model.c#L2969)) after model matrices or
  attachment matrices are restored.
- Object shade/fog/render alpha are partly recalculated during render, though
  saved object shade fields may still matter for visual continuity.

### Hybrid / Handle Carefully

- `prop->pos` and `obj->runtime_pos`: authoritative for root world props, but
  not enough for an attached model. For attached props, restore the attachment
  link and model/embedment matrix too.
- `prop->stan` and `rooms`: authoritative for root collision/culling, often
  intentionally `NULL`/deregistered for attachments.
- `PROPFLAG_ENABLED`: behavior state, not allocation state. A disabled attached
  prop can be valid and later dropped/enabled.
- `runtime_bitflags`: some bits are setup/runtime facts, some are transient
  physics/render flags. `EMBEDDED`, `DEPOSIT`, `HASOWNER`, `REMOVE`, and owner
  bits strongly affect attachment/drop behavior.

## Save/Load Implications

To restore guard attachments correctly:

1. Restore or recreate all participating guard and object/weapon/hat records
   first, with their `prop` and `model` pointers valid.
2. Clear stale parent/child and active-list links for the affected props before
   relinking. `prev/next` cannot be interpreted without knowing whether the prop
   is root or attached.
3. Rebuild root active-list membership only for root props. Attached props
   should not also be in the active list.
4. Restore `PROPFLAG_ENABLED` independently from active-list membership.
5. Rebuild the child chain for each parent prop in saved sibling order. Because
   engine traversal walks newest-to-oldest with `child` then `prev`, preserve
   order if deterministic behavior matters.
6. Restore `chr->weapons_held[GUNRIGHT]`, `chr->weapons_held[GUNLEFT]`, and
   `chr->handle_positiondata_hat` from child classification, not merely from
   serialized raw pointers.
7. Restore each attached object's `model->attachedto` and
   `model->attachedto_objinst`. For guard hands/head, use the guard model
   switches. For embedded props, use the saved parent model/node and embedment
   matrix.
8. Restore or recompute room registration. Root props should be registered;
   attached props are normally deregistered and have `stan == NULL`.
9. Preserve pending drop state. If `CHRHIDDEN_DROP_HELD_ITEMS` is set and child
   projectile `droptype` is saved, load must not accidentally complete or clear
   the scheduled drop.
10. Never infer free/dead props solely from `PROPFLAG_ENABLED == 0`. Check
    object/guard ownership, active-list membership, parent, and backing record
    pointers.

The practice loader resolves CHR equipment in two phases. First it restores
held weapon and hat convenience pointers and model attachment nodes, then it
reattaches concealed children. Those helpers necessarily rewrite `child`,
`prev`, and `next`, and can produce a coherent but different child order. A
final graph pass must therefore reinstall the serialized CHR child links after
all equipment and concealed-item helpers have run. This keeps `parent->child`
and every child `prev`/`next` pair identical to the save while preserving the
already-restored `weapons_held[]`, hat pointer, and model attachments.

## Minimal Attachment Classifier

For a child prop `p` whose parent is a guard prop `gprop`:

- `p == gprop->chr->weapons_held[GUNRIGHT]`: right-hand held weapon.
- `p == gprop->chr->weapons_held[GUNLEFT]`: left-hand held weapon.
- `p == gprop->chr->handle_positiondata_hat`: worn hat.
- `p->obj->runtime_bitflags & RUNTIMEBITFLAG_EMBEDDED`: embedded child.
- `p->type == PROP_TYPE_WEAPON` and neither held pointer matches: concealed
  weapon/item.
- `p->type == PROP_TYPE_OBJ` and not hat/embedded: generic attached object.

For restoration, verify the inverse relationships:

- held right must have `model->attachedto == chr->model` and
  `attachedto_objinst == chr->model->obj->Switches[3]`.
- held left must use switch 5.
- hat must use switch 6.
- concealed non-embedded child should generally have no hand/head
  `attachedto_objinst`.
- embedded child must have `RUNTIMEBITFLAG_EMBEDDED` and an `Embedment` record.

## Common Failure Modes

- Restoring `parent` but leaving the prop in the active list corrupts both the
  active list and the sibling chain because `prev/next` are shared.
- Restoring the child chain but not `weapons_held[]` makes held weapons render
  or drop as concealed items.
- Restoring `weapons_held[]` but not model attachment makes the guard think the
  weapon is held while the model has no hand node.
- Clearing disabled attached props loses concealed items that can later be
  dropped or enabled.
- Re-enabling an attached prop as a root prop makes it tick/render in world
  space while still owned by the guard.
- Rebuilding a valid child chain in a different order changes `child`/`prev`
  traversal. Drop and teardown code walks that order, so save/load must restore
  the serialized graph rather than relying on helper insertion order.
- Ignoring `RUNTIMEBITFLAG_EMBEDDED` treats stuck projectiles as ordinary
  concealed items, but the engine uses embedment matrices and intentionally
  excludes embedded items from normal drop behavior.
- Clearing `CHRHIDDEN_DROP_HELD_ITEMS` or projectile `droptype` can cancel a
  drop that was already scheduled before the save.
