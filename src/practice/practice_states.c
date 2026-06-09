#include "practice_states.h"
#include "assets/obseg/text/LmiscE.h"
#include "bondinv.h"
#include "bondview.h"
#include "chr.h"
#include "chrobjhandler.h"
#include "debugmenu_handler.h"
#include "file2.h"
#include "front.h"
#include "gun.h"
#include "lvl_text.h"
#include "objecthandler.h"
#include "player.h"
#include "player_2.h"
#include "practice_ui.h"
#include <bondconstants.h>
#include <bondgame.h>
#include <joy.h>
#include <music.h>
#include <os_extension.h>
#include <snd.h>
#include <ultra64.h>

// === TODO LIST OF THINGS NOT YET SAVED/RESTORED ===
// Bond:
// - momentum
// - blood on death
// - camera rotation on death
// - crashes when restoring during death replay
// Props:
// - respawn on destroy
// - reset health

extern void *memcpy(void *dst, const void *src, size_t count);
extern s32 get_numguards(void);
extern PropRecord pos_data_entry[];
extern void place_item_in_hand_swap_and_make_visible(s32 hand, s32 item);

static bool g_HasSavedState = FALSE;

// ==========================================
// 1. BOND STATE SAVING AND LOADING
// ==========================================

static struct player g_SavedPlayerState;
static s32 g_SavedMissionTimer = 0;
static s32 g_SavedInTankFlag = 0;
static struct PropRecord *g_SavedPlayerTankProp = NULL;
static s32 g_SavedCameraMode = 0;

#define MAX_SAVED_INVENTORY_ITEMS 128
static InvItem g_SavedInventoryItems[MAX_SAVED_INVENTORY_ITEMS];
static s32 g_SavedInventoryItemsCount = 0;
static InvItem *g_SavedPtrInventoryFirstInCycle = NULL;
static InvItem *g_SavedPItemCur = NULL;

static InvItem *relocate_inv_pointer(InvItem *p, InvItem *saved_base,
                                     InvItem *active_base, s32 max_count) {
  s32 index;
  if (p == NULL || saved_base == NULL || active_base == NULL)
    return NULL;
  index = p - saved_base;
  if (index >= 0 && index < max_count) {
    return active_base + index;
  }
  return NULL;
}

static void save_bond_state(void) {
  /* Save Player Structure */
  memcpy(&g_SavedPlayerState, g_CurrentPlayer, sizeof(struct player));
  g_SavedMissionTimer = mission_timer;
  g_SavedInTankFlag = in_tank_flag;
  g_SavedPlayerTankProp = g_PlayerTankProp;
  g_SavedCameraMode = g_CameraMode;

  /* Save Inventory items */
  g_SavedInventoryItemsCount = g_CurrentPlayer->equipmaxitems;
  if (g_SavedInventoryItemsCount > MAX_SAVED_INVENTORY_ITEMS) {
    g_SavedInventoryItemsCount = MAX_SAVED_INVENTORY_ITEMS;
  }
  if (g_CurrentPlayer->p_itemcur != NULL) {
    memcpy(g_SavedInventoryItems, g_CurrentPlayer->p_itemcur,
           g_SavedInventoryItemsCount * sizeof(InvItem));
  }
  g_SavedPtrInventoryFirstInCycle =
      g_CurrentPlayer->ptr_inventory_first_in_cycle;
  g_SavedPItemCur = g_CurrentPlayer->p_itemcur;
}

static void load_bond_state(void) {
  s32 i;

  // Restore health & armor
  g_CurrentPlayer->bondhealth = g_SavedPlayerState.bondhealth;
  g_CurrentPlayer->bondarmour = g_SavedPlayerState.bondarmour;
  g_CurrentPlayer->oldhealth = g_SavedPlayerState.oldhealth;
  g_CurrentPlayer->oldarmour = g_SavedPlayerState.oldarmour;
  g_CurrentPlayer->apparenthealth = g_SavedPlayerState.apparenthealth;
  g_CurrentPlayer->apparentarmour = g_SavedPlayerState.apparentarmour;

  // Restore health/armor show timers
  g_CurrentPlayer->damageshowtime = g_SavedPlayerState.damageshowtime;
  g_CurrentPlayer->healthshowtime = g_SavedPlayerState.healthshowtime;
  g_CurrentPlayer->healthshowmode = g_SavedPlayerState.healthshowmode;

  if (g_SavedPlayerState.damageshowtime < 0) {
    gunSetGunAmmoVisible(GUNAMMOREASON_DAMAGE, TRUE);
    gunSetSightVisible(GUNSIGHTREASON_DAMAGE, TRUE);
    hudmsgsSetOn(4);
    bondviewClearUpperTextDisplayFlag(PLAYERFLAG_NOTIMER);
    countdownTimerSetVisible(8, 1);
  }

  // Restore dead state and death animation markers (important for reviving)
  g_CurrentPlayer->bonddead = g_SavedPlayerState.bonddead;
  g_CurrentPlayer->redbloodfinished = g_SavedPlayerState.redbloodfinished;
  g_CurrentPlayer->deathanimfinished = g_SavedPlayerState.deathanimfinished;

  // Restore cheat invincibility status
  g_CurrentPlayer->bondinvincible = g_SavedPlayerState.bondinvincible;

  // Restore ammo counts
  memcpy(g_CurrentPlayer->ammoheldarr, g_SavedPlayerState.ammoheldarr,
         sizeof(g_CurrentPlayer->ammoheldarr));

  /* Restore Inventory List */
  if (g_CurrentPlayer->p_itemcur != NULL && g_SavedPItemCur != NULL) {
    s32 count = g_CurrentPlayer->equipmaxitems;
    if (count > g_SavedInventoryItemsCount) {
      count = g_SavedInventoryItemsCount;
    }
    memcpy(g_CurrentPlayer->p_itemcur, g_SavedInventoryItems,
           count * sizeof(InvItem));

    // Relocate next and prev pointers inside the new p_itemcur bank
    for (i = 0; i < count; i++) {
      g_CurrentPlayer->p_itemcur[i].next = relocate_inv_pointer(
          g_CurrentPlayer->p_itemcur[i].next, g_SavedPItemCur,
          g_CurrentPlayer->p_itemcur, count);
      g_CurrentPlayer->p_itemcur[i].prev = relocate_inv_pointer(
          g_CurrentPlayer->p_itemcur[i].prev, g_SavedPItemCur,
          g_CurrentPlayer->p_itemcur, count);
    }

    g_CurrentPlayer->ptr_inventory_first_in_cycle =
        relocate_inv_pointer(g_SavedPtrInventoryFirstInCycle, g_SavedPItemCur,
                             g_CurrentPlayer->p_itemcur, count);
  }
  g_CurrentPlayer->equipallguns = g_SavedPlayerState.equipallguns;
  g_CurrentPlayer->equipcuritem = g_SavedPlayerState.equipcuritem;

  // Restore weapon selection and magazine ammo counts (keeping pointers safe)
  memcpy(&g_CurrentPlayer->hands[0], &g_SavedPlayerState.hands[0],
         sizeof(struct hand));
  memcpy(&g_CurrentPlayer->hands[1], &g_SavedPlayerState.hands[1],
         sizeof(struct hand));
  g_CurrentPlayer->hands[0].audioHandle = NULL;
  g_CurrentPlayer->hands[0].rocket = NULL;
  g_CurrentPlayer->hands[1].audioHandle = NULL;
  g_CurrentPlayer->hands[1].rocket = NULL;

  // Request the engine to load the 1st person weapon models on demand
  g_CurrentPlayer->lock_hand_model[GUNRIGHT] =
      g_SavedPlayerState.lock_hand_model[GUNRIGHT];
  g_CurrentPlayer->lock_hand_model[GUNLEFT] =
      g_SavedPlayerState.lock_hand_model[GUNLEFT];

  place_item_in_hand_swap_and_make_visible(
      GUNRIGHT, g_SavedPlayerState.hand_item[GUNRIGHT]);
  place_item_in_hand_swap_and_make_visible(
      GUNLEFT, g_SavedPlayerState.hand_item[GUNLEFT]);
  g_CurrentPlayer->cur_item_weapon_getname =
      g_SavedPlayerState.cur_item_weapon_getname;

  // Restore look angles and trigonometric caches
  g_CurrentPlayer->vv_theta = g_SavedPlayerState.vv_theta;
  g_CurrentPlayer->vv_costheta = g_SavedPlayerState.vv_costheta;
  g_CurrentPlayer->vv_sintheta = g_SavedPlayerState.vv_sintheta;
  g_CurrentPlayer->vv_verta = g_SavedPlayerState.vv_verta;
  g_CurrentPlayer->vv_verta360 = g_SavedPlayerState.vv_verta360;

  // Safely sync top-level coordinates
  g_CurrentPlayer->pos = g_SavedPlayerState.pos;
  g_CurrentPlayer->pos2 = g_SavedPlayerState.pos2;
  g_CurrentPlayer->offset = g_SavedPlayerState.offset;
  g_CurrentPlayer->pos3 = g_SavedPlayerState.pos3;
  g_CurrentPlayer->current_model_pos = g_SavedPlayerState.current_model_pos;
  g_CurrentPlayer->previous_model_pos = g_SavedPlayerState.previous_model_pos;
  g_CurrentPlayer->current_room_pos = g_SavedPlayerState.current_room_pos;
  g_CurrentPlayer->bondprevpos = g_SavedPlayerState.bondprevpos;
  if (g_CurrentPlayer->prop != NULL) {
    g_CurrentPlayer->prop->pos =
        g_SavedPlayerState.field_488.collision_position;
    g_CurrentPlayer->prop->stan = g_SavedPlayerState.field_488.current_tile_ptr;
  }
  g_CurrentPlayer->stanHeight = g_SavedPlayerState.stanHeight;
  g_CurrentPlayer->vertical_bounce_adjust =
      g_SavedPlayerState.vertical_bounce_adjust;
  g_CurrentPlayer->crouchpos = g_SavedPlayerState.crouchpos;
  g_CurrentPlayer->ducking_height_offset =
      g_SavedPlayerState.ducking_height_offset;

  // Direct restoration of full collision, room/portal pointers, and camera
  // smoothing
  g_CurrentPlayer->field_488 = g_SavedPlayerState.field_488;
  g_CurrentPlayer->previous_collision_info =
      g_SavedPlayerState.previous_collision_info;
  g_CurrentPlayer->room_pointer = g_SavedPlayerState.room_pointer;

  g_CurrentPlayer->field_3B8 = g_SavedPlayerState.field_3B8;
  g_CurrentPlayer->field_3C4 = g_SavedPlayerState.field_3C4;
  g_CurrentPlayer->field_3C8 = g_SavedPlayerState.field_3C8;
  g_CurrentPlayer->field_3CC = g_SavedPlayerState.field_3CC;

  in_tank_flag = g_SavedInTankFlag;
  g_PlayerTankProp = g_SavedPlayerTankProp;
  g_CameraMode = g_SavedCameraMode;

  mission_timer = g_SavedMissionTimer;
}

// ==========================================
// 2. GUARDS STATE SAVING AND LOADING
// ==========================================

#define MAX_SAVED_GUARDS 64

typedef struct {
  s16 chrnum;
  coord3d pos;
  StandTile *stan;
  f32 damage;
  ACT_TYPE actiontype;
  CHRFLAG chrflags;
  f32 theta;

  // Physics / movement
  coord3d prevpos;
  coord3d fallspeed;
  f32 ground;
  f32 sumground;
  f32 manground;

  // AI
  u8 alertness;
  u8 morale;
  s8 sleep;
  s32 timer60;
  u16 aioffset;
  s16 aireturnlist;
  AIRecord *ailist;
  u8 action_union[120];

  bool active;
} SavedGuardState;

static SavedGuardState g_SavedGuards[MAX_SAVED_GUARDS];
static s32 g_SavedGuardsCount = 0;

static void save_guards_state(void) {
  s32 i;
  ChrRecord *guard;
  s32 numslots;

  g_SavedGuardsCount = 0;
  numslots = get_numguards();
  if (numslots > MAX_SAVED_GUARDS) {
    numslots = MAX_SAVED_GUARDS;
  }

  for (i = 0; i < numslots; i++) {
    guard = &g_ChrSlots[i];
    if (guard->model != NULL && guard->prop != NULL) {
      g_SavedGuards[g_SavedGuardsCount].chrnum = guard->chrnum;
      g_SavedGuards[g_SavedGuardsCount].pos = guard->prop->pos;
      g_SavedGuards[g_SavedGuardsCount].stan = guard->prop->stan;
      g_SavedGuards[g_SavedGuardsCount].damage = guard->damage;
      g_SavedGuards[g_SavedGuardsCount].actiontype = guard->actiontype;
      g_SavedGuards[g_SavedGuardsCount].chrflags = guard->chrflags;
      g_SavedGuards[g_SavedGuardsCount].theta = getsubroty(guard->model);

      g_SavedGuards[g_SavedGuardsCount].prevpos = guard->prevpos;
      g_SavedGuards[g_SavedGuardsCount].fallspeed = guard->fallspeed;
      g_SavedGuards[g_SavedGuardsCount].ground = guard->ground;
      g_SavedGuards[g_SavedGuardsCount].sumground = guard->sumground;
      g_SavedGuards[g_SavedGuardsCount].manground = guard->manground;

      g_SavedGuards[g_SavedGuardsCount].alertness = guard->alertness;
      g_SavedGuards[g_SavedGuardsCount].morale = guard->morale;
      g_SavedGuards[g_SavedGuardsCount].sleep = guard->sleep;
      g_SavedGuards[g_SavedGuardsCount].timer60 = guard->timer60;
      g_SavedGuards[g_SavedGuardsCount].aioffset = guard->aioffset;
      g_SavedGuards[g_SavedGuardsCount].aireturnlist = guard->aireturnlist;
      g_SavedGuards[g_SavedGuardsCount].ailist = guard->ailist;
      memcpy(g_SavedGuards[g_SavedGuardsCount].action_union, &guard->act_init,
             120);

      g_SavedGuards[g_SavedGuardsCount].active = TRUE;
      g_SavedGuardsCount++;
    }
  }
}

static void load_guards_state(void) {
  s32 i, j;
  ChrRecord *guard;
  s32 numslots;

  numslots = get_numguards();
  for (i = 0; i < numslots; i++) {
    guard = &g_ChrSlots[i];
    if (guard->model != NULL && guard->prop != NULL) {
      for (j = 0; j < g_SavedGuardsCount; j++) {
        if (g_SavedGuards[j].active &&
            g_SavedGuards[j].chrnum == guard->chrnum) {
          guard->prop->pos = g_SavedGuards[j].pos;
          guard->prop->stan = g_SavedGuards[j].stan;
          guard->damage = g_SavedGuards[j].damage;
          guard->actiontype = g_SavedGuards[j].actiontype;
          guard->chrflags = g_SavedGuards[j].chrflags;

          guard->prevpos = g_SavedGuards[j].prevpos;
          guard->fallspeed = g_SavedGuards[j].fallspeed;
          guard->ground = g_SavedGuards[j].ground;
          guard->sumground = g_SavedGuards[j].sumground;
          guard->manground = g_SavedGuards[j].manground;

          guard->alertness = g_SavedGuards[j].alertness;
          guard->morale = g_SavedGuards[j].morale;
          guard->sleep = g_SavedGuards[j].sleep;
          guard->timer60 = g_SavedGuards[j].timer60;
          guard->aioffset = g_SavedGuards[j].aioffset;
          guard->aireturnlist = g_SavedGuards[j].aireturnlist;
          guard->ailist = g_SavedGuards[j].ailist;
          memcpy(&guard->act_init, g_SavedGuards[j].action_union, 120);

          // Sync visual model position and Y-rotation
          setsuboffset(guard->model, &g_SavedGuards[j].pos);
          setsubroty(guard->model, g_SavedGuards[j].theta);
          break;
        }
      }
    }
  }
}

// ==========================================
// 2b. PROP RECOVERY: ID <-> INDEX
// ==========================================

#define MAX_SAVED_PROPS 600

static s16 idToIndex[MAX_SAVED_PROPS];
static s32 propRecoveryIdCounter = 0;

static s16 propAddToRecovery(PropRecord *prop) {
  s16 id = propRecoveryIdCounter;
  if (id < MAX_SAVED_PROPS) {
    idToIndex[id] = prop - pos_data_entry;
    propRecoveryIdCounter++;
  }
  return id;
}

static PropRecord *propRecoverByIndex(s16 prop_index) {
  if (prop_index >= 0 && prop_index < POS_DATA_ENTRY_LEN) {
    return &pos_data_entry[prop_index];
  }
  return NULL;
}

// ==========================================
// 3. PROP STATE SAVING AND LOADING
// ==========================================

typedef struct {
  s16 id;
  s16 prop_def_type; /* PROPDEF_* type */
  bool active;

  /* Saved object-level state */
  u32 object_flags;
  u32 object_flags2;
  u32 object_runtime_bitflags;
  rgba_u8 shadecol;
  rgba_u8 nextcol;

  /* Saved prop-level state */
  u8 prop_flags;
  u8 prop_state;
  f32 prop_damage;
  f32 prop_maxdamage;
  coord3d prop_pos;

  /* Saved door-specific state */
  f32 door_openPosition; /* 0=closed, 1=open */
  f32 door_speed;
  s8  door_openstate;    /* DOORSTATE enum */
  s16 linkedDoorId;      /* Prop recovery ID of linked door (or -1) */

  /* Saved ammo-specific state (MultiAmmoCrateRecord extras) */
  u16 ammo_unk80;
  u16 ammo_quantities[AMMOTYPE_GLOBAL_MAX];
} SavedPropState;

static SavedPropState g_SavedProps[MAX_SAVED_PROPS];
static s32 g_SavedPropsCount = 0;

// ==========================================
// 4. PROP STATE SAVING AND LOADING
// ==========================================

static void save_props_state(void) {
  s32 i;

  g_SavedPropsCount = 0;
  propRecoveryIdCounter = 0;

  for (i = 0; i < POS_DATA_ENTRY_LEN; i++) {
    PropRecord *prop = &pos_data_entry[i];
    ObjectRecord *obj;
    s16 propdef_type;

    // Only process props that have an associated object
    if (prop->obj == NULL) {
      continue;
    }
    obj = prop->obj;
    propdef_type = obj->type;

    g_SavedProps[g_SavedPropsCount].id = propAddToRecovery(prop);
    g_SavedProps[g_SavedPropsCount].prop_def_type = propdef_type;
    g_SavedProps[g_SavedPropsCount].active = TRUE;
    g_SavedProps[g_SavedPropsCount].object_flags = obj->flags;
    g_SavedProps[g_SavedPropsCount].object_flags2 = obj->flags2;
    g_SavedProps[g_SavedPropsCount].object_runtime_bitflags =
        obj->runtime_bitflags;
    g_SavedProps[g_SavedPropsCount].shadecol = obj->shadecol;
    g_SavedProps[g_SavedPropsCount].nextcol = obj->nextcol;
    g_SavedProps[g_SavedPropsCount].prop_flags = prop->flags;
    g_SavedProps[g_SavedPropsCount].prop_state = obj->state;
    g_SavedProps[g_SavedPropsCount].prop_damage = obj->damage;
    g_SavedProps[g_SavedPropsCount].prop_maxdamage = obj->maxdamage;
    g_SavedProps[g_SavedPropsCount].prop_pos = obj->runtime_pos;

    /* Save door-specific state for PROPDEF_DOOR */
    if (propdef_type == PROPDEF_DOOR) {
      DoorRecord *door = (DoorRecord *)obj;
      g_SavedProps[g_SavedPropsCount].door_openPosition = door->openPosition;
      g_SavedProps[g_SavedPropsCount].door_speed = door->speed;
      g_SavedProps[g_SavedPropsCount].door_openstate = door->openstate;
      if (door->linkedDoor != NULL) {
        g_SavedProps[g_SavedPropsCount].linkedDoorId = propAddToRecovery(door->linkedDoor->prop);
      } else {
        g_SavedProps[g_SavedPropsCount].linkedDoorId = -1;
      }
      g_SavedProps[g_SavedPropsCount].ammo_unk80 = 0;
      {
        s32 _k;
        for (_k = 0; _k < AMMOTYPE_GLOBAL_MAX; _k++)
          g_SavedProps[g_SavedPropsCount].ammo_quantities[_k] = 0;
      }
    } else if (propdef_type == PROPDEF_AMMO) {
      MultiAmmoCrateRecord *ammo = (MultiAmmoCrateRecord *)obj;
      g_SavedProps[g_SavedPropsCount].linkedDoorId = -1;
      g_SavedProps[g_SavedPropsCount].door_openPosition = 0;
      g_SavedProps[g_SavedPropsCount].door_speed = 0;
      g_SavedProps[g_SavedPropsCount].door_openstate = 0;
      g_SavedProps[g_SavedPropsCount].ammo_unk80 = ammo->unk80;
      memcpy(g_SavedProps[g_SavedPropsCount].ammo_quantities, ammo->quantities, sizeof(ammo->quantities));
    } else {
      g_SavedProps[g_SavedPropsCount].linkedDoorId = -1;
      g_SavedProps[g_SavedPropsCount].door_openPosition = 0;
      g_SavedProps[g_SavedPropsCount].door_speed = 0;
      g_SavedProps[g_SavedPropsCount].door_openstate = 0;
      g_SavedProps[g_SavedPropsCount].ammo_unk80 = 0;
      {
        s32 _k;
        for (_k = 0; _k < AMMOTYPE_GLOBAL_MAX; _k++)
          g_SavedProps[g_SavedPropsCount].ammo_quantities[_k] = 0;
      }
    }

    g_SavedPropsCount++;
    if (g_SavedPropsCount >= MAX_SAVED_PROPS) {
      break;
    }
  }
}

static void load_props_state(void) {
  s32 i;

  for (i = 0; i < g_SavedPropsCount; i++) {
    PropRecord *prop;
    ObjectRecord *obj;

    prop = propRecoverByIndex(idToIndex[g_SavedProps[i].id]);
    if (prop == NULL || !g_SavedProps[i].active) {
      continue;
    }
    obj = prop->obj;
    if (obj == NULL) {
      continue;
    }

    switch (obj->type) {
    // ----------------------------------------------------------------
    // PROPDEF_DOOR (1) - SAVE: door movement state + linked door ref.
    // ----------------------------------------------------------------
    // case PROPDEF_DOOR: {
    //   DoorRecord *door = (DoorRecord *)obj;
    //   const SavedPropState *saved = &g_SavedProps[i];

    //   /* Restore door movement state */
    //   door->openPosition = saved->door_openPosition;
    //   door->speed = saved->door_speed;
    //   door->openstate = saved->door_openstate;

    //   /* Re-resolve linked door pointer from saved prop recovery ID */
    //   if (saved->linkedDoorId >= 0) {
    //     PropRecord *linkedProp = propRecoverByIndex(idToIndex[saved->linkedDoorId]);
    //     if (linkedProp != NULL && linkedProp->obj != NULL) {
    //       door->linkedDoor = (DoorRecord *)linkedProp->obj;
    //     }
    //   }

    //   /* Sound pointers are runtime-allocated (ALSoundState*).
    //    * They cannot be resolved after load - set to NULL to avoid
    //    * stale pointer access (sndGetPlayingState on stale ptr crashes).
    //    * The door's sound system will recreate them on next movement.
    //    */
    //   door->openSoundState = NULL;
    //   door->closeSoundState = NULL;
    //   break;
    // }
    // ----------------------------------------------------------------
    // PROPDEF_PROP (3) - SAVE: self-contained state.
    // ----------------------------------------------------------------
    case PROPDEF_PROP:
    // ----------------------------------------------------------------
    // PROPDEF_KEY (4) - SAVE: self-contained (same as standard prop).
    // ----------------------------------------------------------------
    case PROPDEF_KEY:
    // ----------------------------------------------------------------
    // PROPDEF_ALARM (5) - SAVE: self-contained state (state/flags).
    // ----------------------------------------------------------------
    case PROPDEF_ALARM:
    // ----------------------------------------------------------------
    // PROPDEF_CCTV (6) - SAVE: self-contained state (state/flags).
    // ----------------------------------------------------------------
    case PROPDEF_CCTV:
    // ----------------------------------------------------------------
    // PROPDEF_MAGAZINE (7) - SAVE: self-contained state (state/flags).
    // ----------------------------------------------------------------
    case PROPDEF_MAGAZINE:
    // ----------------------------------------------------------------
    // PROPDEF_COLLECTABLE (8) - SAVE: self-contained state (state/flags).
    // ----------------------------------------------------------------
    case PROPDEF_COLLECTABLE:
    // ----------------------------------------------------------------
    // PROPDEF_GUARD (9) - SKIP: complex AI system (ailist,
    //                       action_union, ChrRecord cross-references).
    // ----------------------------------------------------------------
    // ----------------------------------------------------------------
    // PROPDEF_MONITOR (10) - SKIP: references global animation
    //                       controller blocks (monitor_ani_control_blocks).
    // ----------------------------------------------------------------
    // ----------------------------------------------------------------
    // PROPDEF_MULTI_MONITOR (11) - SKIP: references global animation
    //                       controller blocks.
    // ----------------------------------------------------------------
    // ----------------------------------------------------------------
    // PROPDEF_RACK (12) - SKIP: complex rack data.
    // ----------------------------------------------------------------
    // ----------------------------------------------------------------
    // PROPDEF_AUTOGUN (13) - SKIP: references audio (snd), has
    //                       complex internal state.
    // ----------------------------------------------------------------
    // ----------------------------------------------------------------
    // PROPDEF_SWITCH (19) - SKIP: bidirectional cross-references
    //                       via LinkRecord -> LinkRecord next/prev.
    // ----------------------------------------------------------------
    // ----------------------------------------------------------------
    // PROPDEF_LINK (14) - SKIP: bidirectional cross-references
    //                       via LinkRecord -> ObjectRecord -> prop.
    // ----------------------------------------------------------------
    // ----------------------------------------------------------------
    // PROPDEF_HAT (17) - SKIP: references g_HatSlots[].
    // ----------------------------------------------------------------
    // ----------------------------------------------------------------
    // PROPDEF_GUARD_ATTRIBUTE (18) - SKIP: references ChrRecord.
    // ----------------------------------------------------------------
    // ----------------------------------------------------------------
    // PROPDEF_SWITCH (19) - SKIP: bidirectional cross-references
    //                       via LinkRecord -> LinkRecord next/prev.
    // ----------------------------------------------------------------
    // ----------------------------------------------------------------
    // PROPDEF_AMMO (20) - SAVE: state + quantities array from
    //                       MultiAmmoCrateRecord. Must restore
    //                       quantities or the spawning loop hangs
    //                       when all slots are 0.
    // ----------------------------------------------------------------
    // case PROPDEF_AMMO: {
    //   const SavedPropState *saved = &g_SavedProps[i];
    //   MultiAmmoCrateRecord *ammo = (MultiAmmoCrateRecord *)obj;

    //   obj->flags = saved->object_flags;
    //   obj->flags2 = saved->object_flags2;
    //   obj->runtime_bitflags = saved->object_runtime_bitflags;
    //   obj->shadecol = saved->shadecol;
    //   obj->nextcol = saved->nextcol;
    //   obj->state = saved->prop_state;
    //   obj->damage = saved->prop_damage;
    //   obj->maxdamage = saved->prop_maxdamage;
    //   prop->flags = saved->prop_flags;
    //   obj->runtime_pos = saved->prop_pos;

    //   // Restore MultiAmmoCrateRecord-specific fields
    //   ammo->unk80 = saved->ammo_unk80;
    //   memcpy(ammo->quantities, saved->ammo_quantities, sizeof(ammo->quantities));

    //   // Sync visual model position from saved coordinates
    //   setsuboffset(obj->model, &saved->prop_pos);
    //   break;
    // }
    // ----------------------------------------------------------------
    // PROPDEF_ARMOUR (21) - SAVE: self-contained state (state/flags).
    // ----------------------------------------------------------------
    case PROPDEF_ARMOUR:
    // ----------------------------------------------------------------
    // PROPDEF_TAG (22) - SAVE: self-contained (state/flags).
    // ----------------------------------------------------------------
    case PROPDEF_TAG:
      // ----------------------------------------------------------------
      // Restore the prop and object state:
      //   prop->flags  - enabled, onscreen, damaged
      //   obj->state   - destroyed, respawn, damaged
      //   obj->flags / flags2 - control/response flags
      //   obj->runtime_bitflags - removed, activated, deposited
      //   obj->damage / maxdamage - health system
      //   obj->shadecol / nextcol - visual color state
      //   obj->runtime_pos - world position (also sets model offset)
      // ----------------------------------------------------------------
      {
        const SavedPropState *saved = &g_SavedProps[i];
        obj->flags = saved->object_flags;
        obj->flags2 = saved->object_flags2;
        obj->runtime_bitflags = saved->object_runtime_bitflags;
        obj->shadecol = saved->shadecol;
        obj->nextcol = saved->nextcol;
        obj->state = saved->prop_state;
        obj->damage = saved->prop_damage;
        obj->maxdamage = saved->prop_maxdamage;
        prop->flags = saved->prop_flags;
        obj->runtime_pos = saved->prop_pos;

        // Sync visual model position from saved coordinates
        setsuboffset(obj->model, &saved->prop_pos);
      }
      break;
    // ----------------------------------------------------------------
    // PROPDEF_OBJECTIVE_* (23-34) - SKIP: objective metadata,
    //                          may reference objects by index.
    // ----------------------------------------------------------------
    // ----------------------------------------------------------------
    // PROPDEF_GAS_RELEASING (36) - SKIP: complex gas logic.
    // ----------------------------------------------------------------
    // ----------------------------------------------------------------
    // PROPDEF_RENAME (37) - SKIP: references PropDefHeaderRecord.
    // ----------------------------------------------------------------
    // ----------------------------------------------------------------
    // PROPDEF_LOCK_DOOR (38) - SAVE: lock object health is handled
    //                       by generic prop mechanism above. The
    //                       LockDoorRecord chain pointers remain
    //                       valid since the level is not reloaded.
    // ----------------------------------------------------------------
    case PROPDEF_LOCK_DOOR:
      /* Lock health already saved/restored via generic mechanism
       * when the lock object itself is processed as a regular prop.
       * The door's PADLOCKEDDOOR flag is saved via object_runtime_bitflags.
       * No extra work needed - chain pointers persist across save/load.
       */
      break;
    // ----------------------------------------------------------------
    // PROPDEF_TRUCK (39) - SKIP: complex vehicle state.
    // ----------------------------------------------------------------
    // ----------------------------------------------------------------
    // PROPDEF_HELI (40) - SKIP: complex vehicle state.
    // ----------------------------------------------------------------
    // ----------------------------------------------------------------
    // PROPDEF_SAFE (42) / PROPDEF_SAFE_ITEM (42) - SKIP: references
    //                       door openPosition cross-references.
    // ----------------------------------------------------------------
    // ----------------------------------------------------------------
    // PROPDEF_TANK (45) - SKIP: complex tank state.
    // ----------------------------------------------------------------
    // ----------------------------------------------------------------
    // PROPDEF_CAMERAPOS (46) - SKIP: save/restore in bondview.
    // ----------------------------------------------------------------
    default:
      break;
    }
  }
}

// ==========================================
// 5. MAIN PUBLIC ENTRY POINTS
// ==========================================

void save_game_state(void) {
  if (g_CurrentPlayer == NULL) {
    return;
  }

  save_bond_state();
  save_guards_state();
  save_props_state();

  g_HasSavedState = TRUE;
  sndPlaySfx(g_musicSfxBufferPtr, CAMERA_BEEP1_SFX, 0);
  practiceLogInfo("STATE SAVED");
}

void load_game_state(void) {
  if (g_CurrentPlayer == NULL || !g_HasSavedState) {
    if (!g_HasSavedState) {
      practiceLogWarn("NO SAVED STATE");
    }
    return;
  }

  load_bond_state();
  load_guards_state();
  load_props_state();

  sndPlaySfx(g_musicSfxBufferPtr, CAMERA_BEEP1_SFX, 0);
  practiceLogInfo("STATE LOADED");
}
