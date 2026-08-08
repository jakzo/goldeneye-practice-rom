#include "practice_states_props.h"
#include "bg.h"
#include "bondview.h"
#include "chr.h"
#include "chrai.h"
#include "chrlv.h"
#include "chrobjdata.h"
#include "chrobjhandler.h"
#include "dyn.h"
#include "emu_log.h"
#include "explosions.h"
#include "gun.h"
#include "initanitable.h"
#include "lightfixture.h"
#include "loadobjectmodel.h"
#include "lvl.h"
#include "matrixmath.h"
#include "objecthandler.h"
#include "player.h"
#include "practice_states.h"
#include "practice_states_chr.h"
#include "practice_states_props.h"
#include "practice_states_utils.h"
#include "practice_ui.h"
#include "unk_0A1DA0.h"
#include <assert.h>
#include <bondconstants.h>
#include <string.h>
#include <ultra64.h>

extern void doorActivatePortal(DoorRecord *door);
extern void doorDeactivatePortal(DoorRecord *door);
extern void door7F052B00(DoorRecord *door);
extern void sub_GAME_7F052D8C(DoorRecord *door);
extern union ModelRwData *modelGetNodeRwData(Model *Objinst, ModelNode *root);
extern void sub_GAME_7F050DE8(Model *model);
extern ModelNode *sub_GAME_7F04B478(ObjectRecord *obj);
extern bool sub_GAME_7F04B590(ModelFileHeader *header, ModelNode *node);
extern Vertex *sub_GAME_7F09BE4C(s32 vertexCount, s32 allocationType,
                                 ModelFileHeader *header, s32 destroyedLevel);
extern void sub_GAME_7F09C044(Vertex *vertices);
extern PathRecord *pathFindById(s32 ID);
extern s32 chraiGetAIListID(AIRecord *AIList, bool *isGlobalAIList);
extern AIRecord *ailistFindById(s32 ID);
extern void projectileFree(Projectile *projectile);
extern void embedmentFree(Embedment *embedment);
extern void projectileReset(Projectile *projectile);
extern s32 modelFindNodeMtxIndex(ModelNode *node, s32 arg1);

#define MAX_SAVED_EQUIPPED_WEAPON_MATRICES 256
#define MAX_SAVED_EQUIPPED_WEAPON_KEY_MATRICES 1
#define MAX_EQUIPPED_WEAPON_SWITCH_CANDIDATES 2
extern void chrpropDelist(PropRecord *prop);
extern void chrpropDetach(PropRecord *prop);
extern void redarken_lights_in_room(s32 room_index);
extern void sub_GAME_7F0B6368(s32 room);
extern s32 chrGetNumFree(void);
extern void clear_aircraft_model_obj(Model *model);
extern PropRecord *hatCreateForChr(ChrRecord *chr, s32 modelnum, u32 flags);
extern PropRecord *objInit(ObjectRecord *obj, ModelFileHeader *model_header,
                           PropRecord *prop, Model *model);
extern WeaponObjRecord *weaponCreate(bool musthaveprop, bool musthavemodel,
                                     ModelFileHeader *modeldef);
extern s32 setupGetCommandIndexByProp(PropRecord *prop);
extern WeaponObjRecord blank_08_object_preset_1;
extern bondstruct_unk_8007A170 dword_CODE_bss_8007A170[20];
extern struct sImageTableEntry *explosion_smokeimages;
extern struct sImageTableEntry *scattered_explosions;
extern struct sImageTableEntry *flareimage2;
extern CartridgeModelFileRecord ejected_cartridge[];
extern s32 dword_CODE_bss_80075DB0;
extern u32 num_obj_position_data_entries;
extern struct s_darkened_light darkened_light_table[];
extern s32 cur_entry_darkened_light_table;
extern bg_portal_data_entry *g_BgPortals;
#if !defined(VERSION_EU)
extern u8 dword_CODE_bss_8007A4E0[0xBB8];
#endif

#define BULLET_EFFECT_BUFFER_LEN 20
#define BULLET_SPARK_BUFFER_LEN 50
#define CASING_BUFFER_LEN 20
#define CASING_MODEL_COUNT 4
#define DARKENED_LIGHT_TABLE_MAX 0x200
#define BG_PORTAL_MAX 200

static ModelNode *get_model_node_by_index(ModelFileHeader *header,
                                          s16 targetIndex);
static void write_effect_zero_rle(StateStream *stream, const u8 *src,
                                  u32 size);
static void read_effect_zero_rle(StateStream *stream, u8 *dst, u32 size);

static bool is_rdram_range(const void *ptr, u32 size) {
  u32 addr = (u32)ptr;
  u32 end = addr + size;
  u32 rdram_end = 0x80000000 + osMemSize;

  return addr >= 0x80000000 && end >= addr && end <= rdram_end;
}

static u8 get_deformation_colour_mode(Vertex *saved, Vertex *base) {
  if (saved->r == base->r && saved->g == base->g &&
      saved->b == base->b && saved->a == base->a) {
    return 0;
  }
  if (saved->r == 0 && saved->g == 0 && saved->b == 0 &&
      saved->a == 0xff) {
    return 1;
  }
  if (saved->r == base->r && saved->g == base->g &&
      saved->b == base->b && saved->a == 0) {
    return 2;
  }
  if (saved->r == 0 && saved->g == 0 && saved->b == 0 && saved->a == 0) {
    return 3;
  }

  practiceLogError("Destroyed prop deformation colour invariant failed");
  assert(FALSE);
  return 0;
}

static void skip_object_deformation_vertices(StateStream *stream,
                                             u16 vertexCount) {
  s32 i;

  for (i = 0; i < vertexCount; i++) {
    read_u8(stream);
    read_u8(stream);
    if (read_u8(stream) == 0x80) {
      read_u16(stream);
    }
  }
  for (i = 0; i < (vertexCount + 3) / 4; i++) {
    read_u8(stream);
  }
}

static bool prop_is_inventory_item(PropRecord *prop) {
  InvItem *first;
  InvItem *item;

  if (prop == NULL || g_CurrentPlayer == NULL) {
    return FALSE;
  }

  first = g_CurrentPlayer->ptr_inventory_first_in_cycle;
  item = first;
  if (item == NULL) {
    return FALSE;
  }

  do {
    if (item->type == INV_ITEM_PROP &&
        item->type_inv_item.type_prop.prop == prop) {
      return TRUE;
    }
    item = item->next;
  } while (item != NULL && item != first);

  return FALSE;
}

static u8 save_portal_closed_state(s32 portal) {
  if (portal == -1) {
    return 0xff;
  }
  if (portal < 0 || portal >= BG_PORTAL_MAX) {
    practiceLogError("Tinted glass has invalid portal index %d", portal);
    assert(FALSE);
    return 0xff;
  }
  return bgGetDataPortalsControlBytes1Bit1(portal) != 0;
}

static s32 get_animation_offset(const ModelAnimation *animation) {
  if (animation == NULL) {
    return -1;
  }

  return (const u8 *)animation - &ptr_animation_table->data[0];
}

static ModelAnimation *get_animation_by_offset(s32 offset) {
  if (offset < 0) {
    return NULL;
  }

  return (ModelAnimation *)&ptr_animation_table->data[offset];
}

static void save_object_model_animation(StateStream *stream,
                                        const Model *model) {
  ModelRwData_HeaderRecord *root_data;
  bool has_root_data =
      model != NULL && model->obj != NULL && model->obj->RootNode != NULL &&
      (model->obj->RootNode->Opcode & 0xff) == MODELNODE_OPCODE_HEADER;

  write_u32(stream, model != NULL ? get_animation_offset(model->anim) : -1);
  write_u32(stream, model != NULL ? get_animation_offset(model->anim2) : -1);
  write_u8(stream, model != NULL ? (u8)model->gunhand : 0);
  write_u8(stream, model != NULL ? (u8)model->unk25 : 0);
  write_u8(stream, model != NULL ? (u8)model->animlooping : 0);
  write_u8(stream, model != NULL ? (u8)model->unk27 : 0);
  write_f32(stream, model != NULL ? model->unk28 : 0.0f);
  write_f32(stream, model != NULL ? model->unk2c : 0.0f);
  write_u16(stream, model != NULL ? (u16)model->framea : 0);
  write_u16(stream, model != NULL ? (u16)model->frameb : 0);
  write_f32(stream, model != NULL ? model->endframe : -1.0f);
  write_f32(stream, model != NULL ? model->speed : 0.0f);
  write_f32(stream, model != NULL ? model->newspeed : 0.0f);
  write_f32(stream, model != NULL ? model->oldspeed : 0.0f);
  write_f32(stream, model != NULL ? model->timespeed : 0.0f);
  write_f32(stream, model != NULL ? model->elapsespeed : 0.0f);
  write_f32(stream, model != NULL ? model->unk58 : 0.0f);
  write_f32(stream, model != NULL ? model->unk5c : 0.0f);
  write_u16(stream, model != NULL ? (u16)model->frame2a : 0);
  write_u16(stream, model != NULL ? (u16)model->frame2b : 0);
  write_f32(stream, model != NULL ? model->unk6c : -1.0f);
  write_f32(stream, model != NULL ? model->speed2 : 0.0f);
  write_u32(stream, model != NULL ? model->unk74 : 0);
  write_u32(stream, model != NULL ? model->unk78 : 0);
  write_f32(stream, model != NULL ? model->unk7c : 0.0f);
  write_u32(stream, model != NULL ? model->unk80 : 0);
  write_f32(stream, model != NULL ? model->unk84 : 0.0f);
  write_f32(stream, model != NULL ? model->unk88 : 0.0f);
  write_u32(stream, model != NULL ? model->unk8c : 0);
  write_f32(stream, model != NULL ? model->animloopframe : 0.0f);
  write_f32(stream, model != NULL ? model->animloopmerge : 0.0f);
  write_u32(stream, model != NULL ? model->unk9c : 0);
  write_u32(stream, model != NULL ? model->unka0 : 0);
  write_f32(stream, model != NULL ? model->playspeed : 1.0f);
  write_f32(stream, model != NULL ? model->animrate : 1.0f);
  write_f32(stream, model != NULL ? model->unkac : 0.0f);
  write_f32(stream, model != NULL ? model->unkb0 : 0.0f);
  write_f32(stream, model != NULL ? model->unkb4 : 0.0f);
  write_f32(stream, model != NULL ? model->unkb8 : 0.0f);
  write_u32(stream, model != NULL ? model->unkbc : 0);

  write_u8(stream, has_root_data);
  if (has_root_data) {
    root_data =
        (ModelRwData_HeaderRecord *)modelGetNodeRwData((Model *)model,
                                                       model->obj->RootNode);
    write_bytes(stream, root_data, sizeof(ModelRwData_HeaderRecord));
  }
}

static void load_object_model_animation(StateStream *stream, Model *model) {
  s32 anim_offset = read_u32(stream);
  s32 anim2_offset = read_u32(stream);
  s8 gunhand = (s8)read_u8(stream);
  s8 unk25 = (s8)read_u8(stream);
  s8 animlooping = (s8)read_u8(stream);
  s8 unk27 = (s8)read_u8(stream);
  f32 unk28 = read_f32(stream);
  f32 unk2c = read_f32(stream);
  s16 framea = (s16)read_u16(stream);
  s16 frameb = (s16)read_u16(stream);
  f32 endframe = read_f32(stream);
  f32 speed = read_f32(stream);
  f32 newspeed = read_f32(stream);
  f32 oldspeed = read_f32(stream);
  f32 timespeed = read_f32(stream);
  f32 elapsespeed = read_f32(stream);
  f32 unk58 = read_f32(stream);
  f32 unk5c = read_f32(stream);
  s16 frame2a = (s16)read_u16(stream);
  s16 frame2b = (s16)read_u16(stream);
  f32 unk6c = read_f32(stream);
  f32 speed2 = read_f32(stream);
  s32 unk74 = read_u32(stream);
  s32 unk78 = read_u32(stream);
  f32 unk7c = read_f32(stream);
  s32 unk80 = read_u32(stream);
  f32 unk84 = read_f32(stream);
  f32 unk88 = read_f32(stream);
  s32 unk8c = read_u32(stream);
  f32 animloopframe = read_f32(stream);
  f32 animloopmerge = read_f32(stream);
  s32 unk9c = read_u32(stream);
  s32 unka0 = read_u32(stream);
  f32 playspeed = read_f32(stream);
  f32 animrate = read_f32(stream);
  f32 unkac = read_f32(stream);
  f32 unkb0 = read_f32(stream);
  f32 unkb4 = read_f32(stream);
  f32 unkb8 = read_f32(stream);
  s32 unkbc = read_u32(stream);
  bool has_root_data = read_u8(stream);
  ModelRwData_HeaderRecord root_data;

  if (has_root_data) {
    read_bytes(stream, &root_data, sizeof(ModelRwData_HeaderRecord));
  }

  if (model == NULL) {
    return;
  }

  model->anim = get_animation_by_offset(anim_offset);
  model->anim2 = get_animation_by_offset(anim2_offset);
  model->gunhand = gunhand;
  model->unk25 = unk25;
  model->animlooping = animlooping;
  model->unk27 = unk27;
  model->unk28 = unk28;
  model->unk2c = unk2c;
  model->framea = framea;
  model->frameb = frameb;
  model->endframe = endframe;
  model->speed = speed;
  model->newspeed = newspeed;
  model->oldspeed = oldspeed;
  model->timespeed = timespeed;
  model->elapsespeed = elapsespeed;
  model->unk58 = unk58;
  model->unk5c = unk5c;
  model->frame2a = frame2a;
  model->frame2b = frame2b;
  model->unk6c = unk6c;
  model->speed2 = speed2;
  model->unk74 = unk74;
  model->unk78 = unk78;
  model->unk7c = unk7c;
  model->unk80 = unk80;
  model->unk84 = unk84;
  model->unk88 = unk88;
  model->unk8c = unk8c;
  model->animloopframe = animloopframe;
  model->animloopmerge = animloopmerge;
  model->animflipfunc = 0;
  model->unk9c = unk9c;
  model->unka0 = unka0;
  model->playspeed = playspeed;
  model->animrate = animrate;
  model->unkac = unkac;
  model->unkb0 = unkb0;
  model->unkb4 = unkb4;
  model->unkb8 = unkb8;
  model->unkbc = unkbc;

  if (has_root_data && model->obj != NULL && model->obj->RootNode != NULL &&
      (model->obj->RootNode->Opcode & 0xff) == MODELNODE_OPCODE_HEADER) {
    ModelRwData_HeaderRecord *dst =
        (ModelRwData_HeaderRecord *)modelGetNodeRwData(model,
                                                       model->obj->RootNode);
    *dst = root_data;
  }
}

static void load_portal_closed_state(s32 portal, u8 closed) {
  if (portal == -1 && closed == 0xff) {
    return;
  }
  if (portal < 0 || portal >= BG_PORTAL_MAX || closed > 1) {
    practiceLogError("Tinted glass portal state is invalid (%d, %d)", portal,
                     closed);
    assert(FALSE);
    return;
  }

  // A zero toggle sets the "closed" bit; a nonzero toggle clears it.
  bgToggleDataPortalsContrlBytes1Bit1(portal, closed == 0);
}

/*
 * The non-EU spark/dust pool uses 0x3c-byte entries. Its first 0x2c bytes have
 * the same layout as bondstruct_unk_8007A170; the final 0x10 bytes hold drift
 * and a vertical limit used by sub_GAME_7F0A46A0.
 */
typedef struct BulletSparkRecord {
  bondstruct_unk_8007A170 effect;
  coord3d drift;
  f32 vertical_limit;
} BulletSparkRecord;

static u8 get_bullet_effect_image_id(u32 image) {
  if (image == (u32)flareimage2) {
    return 0;
  }
  if (image == (u32)explosion_smokeimages) {
    return 1;
  }
  if (image == (u32)scattered_explosions) {
    return 2;
  }
  return 0xff;
}

static u32 get_bullet_effect_image_by_id(u8 id) {
  switch (id) {
  case 0:
    return (u32)flareimage2;
  case 1:
    return (u32)explosion_smokeimages;
  case 2:
    return (u32)scattered_explosions;
  default:
    return 0;
  }
}

static u8 get_casing_model_id(ModelFileHeader *header) {
  s32 i;

  for (i = 0; i < CASING_MODEL_COUNT; i++) {
    if (ejected_cartridge[i].header == header) {
      return i;
    }
  }

  practiceLogError("Active casing has unknown model header %08x", header);
  assert(FALSE);
  return 0xff;
}

static ModelFileHeader *get_casing_model_by_id(u8 id) {
  if (id >= CASING_MODEL_COUNT || ejected_cartridge[id].header == NULL) {
    practiceLogError("Saved casing has invalid model id %d", id);
    assert(FALSE);
    return NULL;
  }

  return ejected_cartridge[id].header;
}

// Allocation metadata serialized ahead of an object/door/weapon payload so the
// destination prop can be recreated before the payload is consumed. Mirrors
// ChrAllocationState for CHR props.
typedef struct ObjAllocationState {
  u16 modelnum;      /* ObjectRecord::obj model id, or 0xffff when absent. */
  u8 objtype;        /* PROPDEF_* object subtype. */
  s16 setupCmdIndex; /* Setup-command index owning this object, or -1 when the
                      * object was created dynamically (dropped/thrown). */
} ObjAllocationState;

typedef union {
  ObjectRecord base;
  KeyRecord key;
  CCTVRecord cctv;
  AmmoCrateRecord mag;
  WeaponObjRecord wpn;
  MonitorObjRecord mon;
  MultiMonitorObjRecord mmon;
  AutogunRecord agun;
  MultiAmmoCrateRecord ammo;
  BodyArmourRecord arm;
  VehichleRecord veh;
  AircraftRecord air;
  GlassRecord gl;
  TintedGlassRecord tgl;
  TankRecord tank;
} TempObjectRecord;

static void retain_prop_from_free_list(PropRecord *prop) {
  PropRecord *current = ptr_obj_pos_list_final_entry;
  PropRecord *previous = NULL;
  s32 guard = 0;

  while (current != NULL && guard++ < POS_DATA_ENTRY_LEN) {
    if (current < pos_data_entry ||
        current >= &pos_data_entry[POS_DATA_ENTRY_LEN]) {
      ptr_obj_pos_list_final_entry = NULL;
      return;
    }

    if (current == prop) {
      if (previous == NULL) {
        ptr_obj_pos_list_final_entry = current->prev;
      } else {
        previous->prev = current->prev;
      }
      current->prev = NULL;
      return;
    }
    previous = current;
    current = current->prev;
  }

  if (guard >= POS_DATA_ENTRY_LEN) {
    ptr_obj_pos_list_final_entry = NULL;
  }
}

static bool save_prop_free_list(StateStream *stream) {
  PropRecord *prop = ptr_obj_pos_list_final_entry;
  u32 count_offset = stream->base_address + stream->total_processed;
  u16 count = 0;

  write_u16(stream, 0);
  while (prop != NULL && count < POS_DATA_ENTRY_LEN) {
    s32 index = get_prop_index(prop);
    if (index < 0 || index >= POS_DATA_ENTRY_LEN) {
      practiceLogError("Prop free list contains invalid entry %08x", prop);
      return FALSE;
    }
    write_u16(stream, index);
    count++;
    prop = prop->prev;
  }
  if (prop != NULL) {
    practiceLogError("Prop free list contains a cycle");
    return FALSE;
  }

  {
    u32 end = stream->base_address + stream->total_processed;
    stream_seek(stream, count_offset);
    write_u16(stream, count);
    stream_seek(stream, end);
  }
  return TRUE;
}

static void forget_prop_rooms(PropRecord *prop) {
  if (prop != NULL && !(prop->flags & 0x10)) {
    prop->rooms[0] = 0xff;
  }
}

static void clear_plain_prop(PropRecord *prop, bool release_prop) {
  if (prop->flags & PROPFLAG_ENABLED) {
    forget_prop_rooms(prop);
    chrpropDelist(prop);
    chrpropDisable(prop);
  }

  prop->voidp = NULL;
  prop->parent = NULL;
  prop->child = NULL;
  prop->prev = NULL;
  prop->next = NULL;
  prop->stan = NULL;
  prop->rooms[0] = 0xff;

  if (release_prop) {
    chrpropFree(prop);
  }
}

static void release_explosion_prop(PropRecord *prop) {
  if (prop->explosion != NULL) {
    // Natural expiry leaves every part inactive before releasing the buffer
    // entry. Save-state loading can remove an explosion early, so explicitly
    // establish the same invariant before a later explosion reuses the entry.
    bzero(prop->explosion, sizeof(*prop->explosion));
  }
}

static void release_smoke_prop(PropRecord *prop) {
  if (prop->smoke != NULL) {
    // explosionCreateSmoke assumes a free entry has no live particles. That is
    // true after natural expiry, but not when loading forcibly removes smoke.
    // Without this clear, the next shot can render particles from after save.
    bzero(prop->smoke, sizeof(*prop->smoke));
  }
}

static void destroy_chr_prop(PropRecord *prop, bool release_prop) {
  if (prop == NULL || prop->type != PROP_TYPE_CHR) {
    return;
  }

  if (prop->chr != NULL && prop->chr->model != NULL) {
    disable_sounds_attached_to_player_then_something(prop);
  } else if (prop->flags & PROPFLAG_ENABLED) {
    forget_prop_rooms(prop);
  }

  if (prop->flags & PROPFLAG_ENABLED) {
    chrpropDelist(prop);
    chrpropDisable(prop);
  }

  clear_plain_prop(prop, release_prop);
}

static bool clear_prop_for_replacement(PropRecord *prop) {
  if (prop == NULL) {
    return TRUE;
  }

  if (!(prop->flags & PROPFLAG_ENABLED) && prop->parent == NULL) {
    return TRUE;
  }

  if (prop->type == PROP_TYPE_CHR) {
    destroy_chr_prop(prop, FALSE);
    return TRUE;
  }

  if ((prop->type == PROP_TYPE_OBJ || prop->type == PROP_TYPE_DOOR ||
       prop->type == PROP_TYPE_WEAPON) &&
      prop->obj != NULL) {
    // Keep the PropRecord itself out of the free list because the saved record
    // at this index is about to reuse it.
    objFreePermanently(prop->obj, FALSE);
    clear_plain_prop(prop, FALSE);
    return TRUE;
  }

  if (prop->type == PROP_TYPE_EXPLOSION && prop->explosion != NULL) {
    release_explosion_prop(prop);
    clear_plain_prop(prop, FALSE);
    return TRUE;
  }

  if (prop->type == PROP_TYPE_SMOKE && prop->smoke != NULL) {
    release_smoke_prop(prop);
    clear_plain_prop(prop, FALSE);
    return TRUE;
  }

  if (prop->type == PROP_TYPE_VIEWER || prop->type == PROP_TYPE_NUL) {
    clear_plain_prop(prop, FALSE);
    return TRUE;
  }

  // Future prop types need equivalent retained-slot teardown before the flag
  // can be enabled.
  return FALSE;
}

static PropRecord *create_chr_prop(PropRecord *prop,
                                   const ChrAllocationState *allocation,
                                   const coord3d *pos, s32 stan_offset) {
  Model *model;
  PropRecord *result;
  ChrRecord *target_chr;

  if (prop == NULL || allocation->bodynum < 0 ||
      allocation->slot_index < 0 ||
      allocation->slot_index >= g_NumChrSlots) {
    return NULL;
  }

  target_chr = &g_ChrSlots[allocation->slot_index];
  if (target_chr->model != NULL && target_chr->prop != prop) {
    destroy_chr_prop(target_chr->prop, TRUE);
  }
  if (chrGetNumFree() < 1) {
    return NULL;
  }

  model = retrieve_header_for_body_and_head(allocation->bodynum,
                                            allocation->headnum, 0);
  if (model == NULL) {
    return NULL;
  }

  retain_prop_from_free_list(prop);

  prop->flags = 0;
  prop->parent = NULL;
  prop->child = NULL;
  prop->prev = NULL;
  prop->next = NULL;
  prop->rooms[0] = 0xff;

  result = init_GUARDdata_with_set_values(
      prop, model, (coord3d *)pos, allocation->heading,
      get_tile_by_offset(stan_offset), NULL);
  if (result == NULL || result->chr == NULL) {
    clear_aircraft_model_obj(model);
    chrpropFree(prop);
    return NULL;
  }

  if (result->chr != target_chr) {
    ChrRecord *allocated_chr = result->chr;

    *target_chr = *allocated_chr;
    target_chr->prop = result;
    target_chr->model->chr = target_chr;
    result->chr = target_chr;
    bzero(allocated_chr, sizeof(*allocated_chr));
  }

  result->chr->headnum = allocation->headnum;
  result->chr->bodynum = allocation->bodynum;

  // Initialization calculates and registers rooms. The saved common prop
  // payload will install the authoritative room list immediately afterward.
  forget_prop_rooms(result);
  return result;
}

// Setup-backed objects can be destroyed permanently (notably the cuttable
// floor strips in Train), while runtime-created collectables can be dropped,
// thrown, or collected. Both kinds can therefore disappear from their saved
// prop slot and must be recreated on load.
static bool can_recreate_object(const ObjAllocationState *alloc) {
  return alloc->setupCmdIndex >= 0 || alloc->objtype == PROPDEF_COLLECTABLE ||
         alloc->objtype == PROPDEF_HAT;
}

// True when the prop currently at this slot already is the object the save
// describes AND is a live, fully-bound instance, so it can be restored in place
// rather than torn down and rebuilt. The obj->prop back-pointer check is
// essential: a collected pickup leaves its ObjectRecord at the slot with a
// freed model and obj->prop cleared to NULL, so it must not be reused (doing so
// re-enables a prop whose model has been released, crashing on the next tick).
static bool slot_matches_object(PropRecord *prop, u8 propType,
                                const ObjAllocationState *alloc) {
  return prop != NULL && prop->type == propType && prop->obj != NULL &&
         prop->obj->obj == alloc->modelnum &&
         prop->obj->type == alloc->objtype && prop->obj->prop == prop;
}

// Recreate an object/weapon prop in its exact saved slot. Level-defined objects
// reuse their persistent setup ObjectRecord; dynamically created weapons take a
// fresh record from the weapon pool. The caller has already cleared whatever
// occupied the slot. The serialized payload installs the authoritative object
// fields immediately afterward; this only establishes the prop/object/model
// relationship so those fields have somewhere to land.
static PropRecord *create_object_prop(PropRecord *prop, u8 propType,
                                      const ObjAllocationState *alloc) {
  ObjectRecord *obj;
  ModelFileHeader *header;
  PropRecord *result;

  if (prop == NULL || alloc->modelnum == 0xffff) {
    return NULL;
  }

  modelLoad(alloc->modelnum);
  header = PitemZ_entries[alloc->modelnum].header;
  if (header == NULL) {
    return NULL;
  }

  if (alloc->setupCmdIndex >= 0) {
    obj =
        setupCommandGetObject(lvlGetCurrentStageToLoad(), alloc->setupCmdIndex);
    // Only usable if it is the expected object and not already bound to a live
    // prop elsewhere.
    if (obj == NULL || obj->obj != alloc->modelnum || obj->prop != NULL) {
      return NULL;
    }
  } else if (alloc->objtype == PROPDEF_COLLECTABLE) {
    // No owning setup command: this was a dropped/thrown weapon created at
    // runtime. Take a fresh record from the weapon pool. Only collectables are
    // ever spawned this way; any other subtype without a setup origin is
    // unexpected and skipped by the caller.
    WeaponObjRecord *weapon = weaponCreate(FALSE, FALSE, header);
    if (weapon == NULL) {
      return NULL;
    }
    *weapon = blank_08_object_preset_1;
    obj = (ObjectRecord *)weapon;
    obj->obj = alloc->modelnum;
    obj->type = alloc->objtype;
  } else if (alloc->objtype == PROPDEF_HAT) {
    HatRecord *hat = hatCreate(FALSE, FALSE, header);
    if (hat == NULL) {
      return NULL;
    }
    bzero(hat, sizeof(*hat));
    obj = (ObjectRecord *)hat;
    obj->obj = alloc->modelnum;
    obj->type = alloc->objtype;
  } else {
    return NULL;
  }

  retain_prop_from_free_list(prop);
  prop->flags = 0;
  prop->parent = NULL;
  prop->child = NULL;
  prop->prev = NULL;
  prop->next = NULL;
  prop->rooms[0] = 0xff;

  // objInit binds the model, allocates collision metadata as flagged, and links
  // prop->obj/obj->prop. On failure it returns the prop to the free list.
  result = objInit(obj, header, prop, NULL);
  if (result == NULL) {
    return NULL;
  }

  result->type = propType;
  return result;
}

// Rebind a character's equipment object that is still parked, unbound, at its
// saved prop slot. When a character is killed (or disarmed) its held gun/hat
// detaches: the ObjectRecord stays at its slot but obj->prop is cleared and its
// model is released. Such an object is restored by rebinding its model and prop
// in place -- the same as a collected pickup -- which is essential for hats,
// whose objects come from level setup rather than the small runtime g_HatSlots
// pool that hatCreateForChr draws from (that pool holds the live guards' hats
// and is quickly exhausted). Returns NULL if the slot does not hold a matching
// unbound object, so the caller can fall back to pool allocation.
static PropRecord *rebind_chr_attachment(s16 index, s32 objtype, s16 modelnum) {
  PropRecord *prop = get_prop_by_index(index);
  ObjectRecord *obj;
  ModelFileHeader *header;
  PropRecord *result;
  u8 propType;

  if (prop == NULL ||
      (prop->type != PROP_TYPE_OBJ && prop->type != PROP_TYPE_WEAPON) ||
      (prop->flags & PROPFLAG_ENABLED) || prop->parent != NULL ||
      prop->obj == NULL || prop->obj->type != objtype ||
      prop->obj->obj != (u16)modelnum || prop->obj->prop != NULL) {
    return NULL;
  }

  propType = prop->type;
  obj = prop->obj;
  modelLoad(obj->obj);
  header = PitemZ_entries[obj->obj].header;
  if (header == NULL) {
    return NULL;
  }

  // The slot may have been returned to the free list when the item detached.
  retain_prop_from_free_list(prop);
  prop->flags = 0;
  prop->parent = NULL;
  prop->child = NULL;
  prop->prev = NULL;
  prop->next = NULL;
  prop->rooms[0] = 0xff;

  result = objInit(obj, header, prop, NULL);
  if (result == NULL) {
    return NULL;
  }

  result->type = propType;
  return result;
}

// Allocate an explosion prop into its exact saved slot using a free
// g_ExplosionBuffer entry. The serialized record fills the rest afterward.
static PropRecord *create_explosion_prop(PropRecord *prop, s32 buffer_index) {
  struct Explosion *explosion;

  if (prop == NULL || buffer_index < 0 ||
      buffer_index >= EXPLOSION_BUFFER_LEN) {
    return NULL;
  }

  explosion = &g_ExplosionBuffer[buffer_index];
  if (explosion->prop != NULL && explosion->prop != prop) {
    clear_prop_for_replacement(explosion->prop);
  }
  if (explosion->prop != NULL) {
    return NULL;
  }

  retain_prop_from_free_list(prop);
  prop->flags = 0;
  prop->parent = NULL;
  prop->child = NULL;
  prop->prev = NULL;
  prop->next = NULL;
  prop->rooms[0] = 0xff;
  prop->type = PROP_TYPE_EXPLOSION;
  prop->explosion = explosion;
  explosion->prop = prop;
  return prop;
}

// Allocate a smoke prop into its exact saved slot using a free g_SmokeBuffer
// entry. The serialized record fills the rest afterward.
static PropRecord *create_smoke_prop(PropRecord *prop, s32 buffer_index) {
  struct Smoke *smoke;

  if (prop == NULL || buffer_index < 0 || buffer_index >= SMOKE_BUFFER_LEN) {
    return NULL;
  }

  smoke = &g_SmokeBuffer[buffer_index];
  if (smoke->prop != NULL && smoke->prop != prop) {
    clear_prop_for_replacement(smoke->prop);
  }
  if (smoke->prop != NULL) {
    return NULL;
  }

  retain_prop_from_free_list(prop);
  prop->flags = 0;
  prop->parent = NULL;
  prop->child = NULL;
  prop->prev = NULL;
  prop->next = NULL;
  prop->rooms[0] = 0xff;
  prop->type = PROP_TYPE_SMOKE;
  prop->smoke = smoke;
  smoke->prop = prop;
  return prop;
}

static PropRecord *get_chr_attachment_prop(s16 index, s32 object_type) {
  PropRecord *prop = get_prop_by_index(index);

  if (prop == NULL ||
      (prop->type != PROP_TYPE_OBJ && prop->type != PROP_TYPE_WEAPON) ||
      prop->obj == NULL || prop->obj->type != object_type ||
      prop->obj->prop != prop || prop->obj->model == NULL) {
    return NULL;
  }

  return prop;
}

// Saved active-list and attachment-graph linkage for one prop slot, captured
// during the load loop and applied in a single pass afterward. The engine list
// helpers invoked while creating and removing props (chrpropActivate/Delist/
// Free, reparenting) mutate prev/next/parent/child of unrelated slots, so the
// saved links must be installed only once every such operation has completed.
typedef struct SavedPropLinks {
  u16 index;
  u16 parent;
  u16 child;
  u16 prev;
  u16 next;
  s16 attachmentNode;
  u32 objectRuntimeBitflags;
  void *objectProjectile;
} SavedPropLinks;

static bool saved_links_name_chr_child(const SavedPropLinks *savedLinks,
                                       s32 recordCount, u16 childIndex,
                                       u16 chrIndex) {
  s32 i;

  for (i = 0; i < recordCount; i++) {
    if (savedLinks[i].index == childIndex && savedLinks[i].parent == chrIndex) {
      return TRUE;
    }
  }

  return FALSE;
}

static bool prop_is_chr_attachment(PropRecord *prop,
                                   PropRecord *attachments[4]) {
  s32 i;

  for (i = 0; i < 4; i++) {
    if (attachments[i] == prop) {
      return TRUE;
    }
  }

  return FALSE;
}

/*
 * PROPFLAG_ENABLED does not mean that a slot is allocated. Respawning pickups
 * are disabled without being delisted, and collected/hidden setup objects can
 * remain bound to their ObjectRecord while inactive. Neither kind may be
 * returned to the prop free list.
 */
static bool prop_slot_has_live_owner(PropRecord *prop) {
  if (prop == NULL) {
    return FALSE;
  }

  switch ((PROP_TYPE)prop->type) {
  case PROP_TYPE_OBJ:
  case PROP_TYPE_DOOR:
  case PROP_TYPE_WEAPON:
    return prop->obj != NULL && prop->obj->prop == prop;
  case PROP_TYPE_CHR:
    return prop->chr != NULL && prop->chr->prop == prop;
  case PROP_TYPE_EXPLOSION:
    return prop->explosion != NULL && prop->explosion->prop == prop;
  case PROP_TYPE_SMOKE:
    return prop->smoke != NULL && prop->smoke->prop == prop;
  case PROP_TYPE_PLAYER:
  case PROP_TYPE_VIEWER:
    /* Player/viewer slots are owned outside the object/CHR backing-record
     * pools. They can be temporarily absent from the active list while the
     * player state is reconstructed, but must never become allocatable. */
    return TRUE;
  default:
    return FALSE;
  }
}

static bool prop_is_active_list_member(PropRecord *prop) {
  PropRecord *current = ptr_obj_pos_list_first_entry;
  s32 count = 0;

  while (current != NULL && count++ < POS_DATA_ENTRY_LEN) {
    if (current == prop) {
      return TRUE;
    }
    current = current->next;
  }

  return FALSE;
}

static u16 compact_prop_link_index(PropRecord *prop) {
  u16 index = get_prop_index(prop);

  if (index == (u16)-1) {
    return 0x3ff;
  }
  if (index >= POS_DATA_ENTRY_LEN) {
    practiceLogError("Prop link index is out of range: %d", index);
    assert(FALSE);
    return 0x3ff;
  }
  return index;
}

static void save_prop_links(StateStream *stream, PropRecord *prop) {
  u16 parent = compact_prop_link_index(prop->parent);
  u16 child = compact_prop_link_index(prop->child);
  u16 prev = compact_prop_link_index(prop->prev);
  u16 next = compact_prop_link_index(prop->next);

  write_u8(stream, parent >> 2);
  write_u8(stream, (parent << 6) | (child >> 4));
  write_u8(stream, (child << 4) | (prev >> 6));
  write_u8(stream, (prev << 2) | (next >> 8));
  write_u8(stream, next);
}

static void load_prop_links(StateStream *stream, u16 *parent, u16 *child,
                            u16 *prev, u16 *next) {
  u8 packed[5];
  s32 i;

  for (i = 0; i < 5; i++) {
    packed[i] = read_u8(stream);
  }
  *parent = ((u16)packed[0] << 2) | (packed[1] >> 6);
  *child = ((u16)(packed[1] & 0x3f) << 4) | (packed[2] >> 4);
  *prev = ((u16)(packed[2] & 0x0f) << 6) | (packed[3] >> 2);
  *next = ((u16)(packed[3] & 0x03) << 8) | packed[4];
  if (*parent == 0x3ff)
    *parent = (u16)-1;
  if (*child == 0x3ff)
    *child = (u16)-1;
  if (*prev == 0x3ff)
    *prev = (u16)-1;
  if (*next == 0x3ff)
    *next = (u16)-1;
}

static void reset_room_prop_lists(void) {
  s32 i;
  s32 j;

  if (ptr_list_object_lookup_indices != NULL) {
    ptr_list_object_lookup_indices[0] = -1;
  }
  num_obj_position_data_entries = 1;

  if (RoomPropListChunkIndexes != NULL) {
    volatile s16 *indexes = RoomPropListChunkIndexes;
    for (i = 0; i < g_MaxNumRooms; i++) {
      indexes[i] = -1;
    }
  }

  if (RoomPropListChunks != NULL) {
    for (i = 0; i < BSS_8007161C_LEN; i++) {
      volatile s16 *propnums = RoomPropListChunks[i].propnums;
      propnums[0] = -2;
      for (j = 1; j < BSS_8007161C_DATA_LEN; j++) {
        propnums[j] = -1;
      }
    }
  }
}

static void rebuild_room_prop_lists_from_active_props(void) {
  PropRecord *prop;
  s32 guard = 0;

  reset_room_prop_lists();

  for (prop = ptr_obj_pos_list_first_entry;
       prop != NULL && guard++ < POS_DATA_ENTRY_LEN; prop = prop->next) {
    if (prop->parent == NULL && prop->rooms[0] != 0xff) {
      chrpropRegisterRooms(prop);
    }
  }

  if (guard >= POS_DATA_ENTRY_LEN) {
    practiceLogWarn("Active prop list loop detected while rebuilding rooms");
  }
}

static void save_room_prop_lists(StateStream *stream) {
  u16 occupied_count = 0;
  s32 i;

  write_u16(stream, g_MaxNumRooms);
  write_bytes(stream, RoomPropListChunkIndexes,
              g_MaxNumRooms * sizeof(*RoomPropListChunkIndexes));

  for (i = 0; i < BSS_8007161C_LEN; i++) {
    if (RoomPropListChunks[i].propnums[0] != -2) {
      occupied_count++;
    }
  }

  write_u16(stream, occupied_count);
  for (i = 0; i < BSS_8007161C_LEN; i++) {
    if (RoomPropListChunks[i].propnums[0] != -2) {
      write_u8(stream, i);
      write_bytes(stream, &RoomPropListChunks[i],
                  sizeof(RoomPropListChunks[i]));
    }
  }
}

static bool load_room_prop_lists(StateStream *stream) {
  u16 room_count = read_u16(stream);
  u16 occupied_count;
  s32 i;

  if (room_count != g_MaxNumRooms || RoomPropListChunkIndexes == NULL ||
      RoomPropListChunks == NULL) {
    practiceLogError("Saved room prop list count %d does not match level %d",
                     room_count, g_MaxNumRooms);
    assert(FALSE);
    return FALSE;
  }

  read_bytes(stream, RoomPropListChunkIndexes,
             g_MaxNumRooms * sizeof(*RoomPropListChunkIndexes));

  for (i = 0; i < BSS_8007161C_LEN; i++) {
    RoomPropListChunks[i].propnums[0] = -2;
    RoomPropListChunks[i].propnums[1] = -1;
  }

  occupied_count = read_u16(stream);
  if (occupied_count > BSS_8007161C_LEN) {
    practiceLogError("Saved room prop list has invalid chunk count %d",
                     occupied_count);
    assert(FALSE);
    return FALSE;
  }

  for (i = 0; i < occupied_count; i++) {
    u8 chunk_index = read_u8(stream);

    if (RoomPropListChunks[chunk_index].propnums[0] != -2) {
      practiceLogError("Saved room prop list repeats chunk %d", chunk_index);
      assert(FALSE);
      return FALSE;
    }
    read_bytes(stream, &RoomPropListChunks[chunk_index],
               sizeof(RoomPropListChunks[chunk_index]));
  }
  return TRUE;
}

/* The render pass builds this depth-sorted list, but player bullet collision
 * consumes it during the following gameplay tick. Preserve the exact order so
 * the first tick after loading does not use the later timeline's candidates. */
static void save_onscreen_prop_list(StateStream *stream) {
  s32 i;
  s32 count = g_LastOnScreenProp - g_OnScreenPropList;

  if (count < 0 || count >= ONSCREEN_PROP_LIST_LEN ||
      count != g_OnScreenPropCount) {
    practiceLogError("On-screen prop list invariant failed (%d, %d)", count,
                     g_OnScreenPropCount);
    assert(FALSE);
  }

  write_u16(stream, (u16)count);
  for (i = 0; i < count; i++) {
    write_u16(stream, get_prop_index(g_OnScreenPropList[i]));
  }
}

static void load_onscreen_prop_list(StateStream *stream) {
  s32 i;
  u16 count = read_u16(stream);

  if (count >= ONSCREEN_PROP_LIST_LEN) {
    practiceLogError("Saved on-screen prop count is invalid (%d)", count);
    assert(FALSE);
  }

  for (i = 0; i < count; i++) {
    s16 index = (s16)read_u16(stream);
    PropRecord *prop = get_prop_by_index(index);

    if (prop == NULL) {
      practiceLogError("Saved on-screen prop index is invalid (%d)", index);
      assert(FALSE);
    }
    g_OnScreenPropList[i] = prop;
  }
  g_OnScreenPropList[count] = NULL;
  g_OnScreenPropCount = count;
  g_LastOnScreenProp = &g_OnScreenPropList[count];
}

/*
 * Child objects are disabled and removed from the active list while held,
 * worn, embedded, or concealed. They still own real ObjectRecords/Models and
 * their prev/next fields are the child-sibling graph, so serialize them as full
 * prop records instead of reconstructing them from convenience pointers.
 */
static bool prop_is_saved_child_object(PropRecord *prop) {
  if (prop == NULL || prop->parent == NULL ||
      (prop->type != PROP_TYPE_OBJ && prop->type != PROP_TYPE_WEAPON) ||
      prop->obj == NULL) {
    return FALSE;
  }

  if ((prop->parent->type == PROP_TYPE_CHR && prop->parent->chr != NULL) ||
      prop->parent->type == PROP_TYPE_PLAYER ||
      prop->parent->type == PROP_TYPE_VIEWER) {
    return TRUE;
  }

  // Contained weapons and sticky objects are also disabled and delisted, but
  // remain in the parent object's recursively-ticked child graph. They must be
  // saved even when they are not embedded: destroying a crate, for example,
  // turns its child weapon into a projectile on the following object tick.
  return (prop->parent->type == PROP_TYPE_OBJ ||
          prop->parent->type == PROP_TYPE_DOOR ||
          prop->parent->type == PROP_TYPE_WEAPON) &&
         prop->parent->obj != NULL;
}

/*
 * A pristine object hidden inside another object does not use its own model
 * transform, projectile/embedment state, or deformation until it is dropped.
 * Its containing prop supplies the drop position. Keep the logical object and
 * subtype state, but omit that dormant rendering/attachment data so levels
 * with many crate contents still fit in the save-state SRAM partition.
 */
static bool prop_uses_compact_contained_object_state(PropRecord *prop) {
  return prop_is_saved_child_object(prop) && prop->parent->type != PROP_TYPE_CHR &&
         prop->obj->projectile == NULL &&
         !(prop->obj->runtime_bitflags &
           (RUNTIMEBITFLAG_DEPOSIT | RUNTIMEBITFLAG_EMBEDDED)) &&
         objGetDestroyedLevel(prop->obj) == 0;
}

static void detach_old_chr_attachment(ChrRecord *chr, PropRecord *prop,
                                      PropRecord *saved_attachments[4]) {
  if (prop == NULL || prop_is_chr_attachment(prop, saved_attachments) ||
      prop->parent != chr->prop || prop->obj == NULL) {
    return;
  }

  // This attachment was acquired after the save. It is absent from the saved
  // world, so remove it instead of dropping it as a new standalone prop.
  // Dropping it here also appends it to the active list while its prev/next
  // fields are still used by the CHR child chain, splitting the active list.
  objFreePermanently(prop->obj, TRUE);
}

static bool attach_prop_to_chr(ChrRecord *chr, PropRecord *prop,
                               s32 switch_index) {
  ObjectRecord *obj;

  if (prop == NULL || chr->prop == NULL || chr->model == NULL ||
      chr->model->obj == NULL || switch_index >= chr->model->obj->numSwitches ||
      chr->model->obj->Switches[switch_index] == NULL) {
    return FALSE;
  }

  obj = prop->obj;

  // Link the prop as the head of the character's child list. The caller resets
  // chr->prop->child before the first attach, so the list is rebuilt cleanly
  // here rather than reparented onto a possibly-stale chain. Reparenting via
  // the engine helper is avoided because it sets prop->prev = host->child,
  // which produces a self-referential link (and an infinite child-list walk)
  // whenever the prop is still named by a stale child pointer -- exactly what
  // happens to equipment that is detached and reattached across repeated
  // save/load cycles. A prop that is currently a live standalone item must
  // leave the active list first; a prop that was already a child simply has its
  // stale sibling links overwritten.
  if (prop->parent == NULL && (prop->flags & PROPFLAG_ENABLED)) {
    forget_prop_rooms(prop);
    chrpropDelist(prop);
  } else if (prop->parent != NULL && prop->parent != chr->prop) {
    objDetach(prop);
  }
  prop->parent = chr->prop;
  prop->next = NULL;
  prop->prev = chr->prop->child;
  if (chr->prop->child != NULL) {
    chr->prop->child->next = prop;
  }
  chr->prop->child = prop;

  // Held equipment is neither a deposited projectile nor an embedded object.
  // The projectile/embedment pools were already restored from the save, so a
  // pointer retained by a weapon dropped after the save must not be reused.
  obj->projectile = NULL;
  obj->runtime_bitflags &= ~(RUNTIMEBITFLAG_DEPOSIT | RUNTIMEBITFLAG_EMBEDDED);
  obj->runtime_bitflags |= RUNTIMEBITFLAG_HASOWNER;
  obj->model->attachedto = chr->model;
  obj->model->attachedto_objinst = chr->model->obj->Switches[switch_index];
  chrpropDisable(prop);
  return TRUE;
}

static void restore_chr_attachments(PropRecord *chr_prop,
                                    ChrAttachmentIndices *indices,
                                    const SavedPropLinks *savedLinks,
                                    s32 recordCount) {
  ChrRecord *chr = chr_prop->chr;
  PropRecord *saved[4];
  PropRecord *old[4];
  bool savedChild[4];
  u16 chrIndex;
  s32 i;

  if (chr == NULL || chr->prop != chr_prop) {
    return;
  }

  chrIndex = get_prop_index(chr_prop);
  for (i = 0; i < 3; i++) {
    savedChild[i] = saved_links_name_chr_child(
        savedLinks, recordCount, indices->weapons_held[i], chrIndex);
  }
  savedChild[3] = saved_links_name_chr_child(savedLinks, recordCount,
                                             indices->hat, chrIndex);

  saved[0] = savedChild[0]
                 ? get_chr_attachment_prop(indices->weapons_held[GUNRIGHT],
                                           PROPDEF_COLLECTABLE)
                 : NULL;
  saved[1] = savedChild[1]
                 ? get_chr_attachment_prop(indices->weapons_held[GUNLEFT],
                                           PROPDEF_COLLECTABLE)
                 : NULL;
  saved[2] = savedChild[2] ? get_prop_by_index(indices->weapons_held[2]) : NULL;
  saved[3] = savedChild[3]
                 ? get_chr_attachment_prop(indices->hat, PROPDEF_HAT)
                 : NULL;

  // Restore held equipment that no longer has a live standalone prop to
  // reattach (e.g. the character was killed and dropped it). Prefer rebinding
  // the original object still parked, unbound, at its saved slot; only allocate
  // a fresh one from the runtime pools when that slot is gone.
  if (savedChild[GUNRIGHT] && saved[GUNRIGHT] == NULL &&
      indices->weapon_model[GUNRIGHT] >= 0) {
    saved[GUNRIGHT] = rebind_chr_attachment(indices->weapons_held[GUNRIGHT],
                                            PROPDEF_COLLECTABLE,
                                            indices->weapon_model[GUNRIGHT]);
    if (saved[GUNRIGHT] == NULL) {
      saved[GUNRIGHT] = chrGiveWeapon(chr, indices->weapon_model[GUNRIGHT],
                                      indices->weaponnum[GUNRIGHT],
                                      indices->weapon_flags[GUNRIGHT]);
    }
  }
  if (savedChild[GUNLEFT] && saved[GUNLEFT] == NULL &&
      indices->weapon_model[GUNLEFT] >= 0) {
    saved[GUNLEFT] = rebind_chr_attachment(indices->weapons_held[GUNLEFT],
                                           PROPDEF_COLLECTABLE,
                                           indices->weapon_model[GUNLEFT]);
    if (saved[GUNLEFT] == NULL) {
      saved[GUNLEFT] = chrGiveWeapon(chr, indices->weapon_model[GUNLEFT],
                                     indices->weaponnum[GUNLEFT],
                                     indices->weapon_flags[GUNLEFT]);
    }
  }
  if (savedChild[3] && saved[3] == NULL && indices->hat_model >= 0) {
    saved[3] =
        rebind_chr_attachment(indices->hat, PROPDEF_HAT, indices->hat_model);
    if (saved[3] == NULL) {
      saved[3] = hatCreateForChr(chr, indices->hat_model, indices->hat_flags);
    }
  }

  if (saved[0] == saved[1]) {
    saved[1] = NULL;
  }
  if (saved[3] == saved[0] || saved[3] == saved[1]) {
    saved[3] = NULL;
  }

  old[0] = chr->weapons_held[0];
  old[1] = chr->weapons_held[1];
  old[2] = chr->weapons_held[2];
  old[3] = chr->handle_positiondata_hat;

  for (i = 0; i < 4; i++) {
    detach_old_chr_attachment(chr, old[i], saved);
  }

  chr->weapons_held[0] = NULL;
  chr->weapons_held[1] = NULL;
  chr->weapons_held[2] = NULL;
  chr->handle_positiondata_hat = NULL;

  // Reset the child-list head so attach_prop_to_chr rebuilds it from scratch.
  // The retained/reused character may carry a stale (even self-referential)
  // child chain from earlier save/load cycles; discarding it here guarantees
  // the equipment below forms a clean, acyclic sibling list.
  chr_prop->child = NULL;

  if (!attach_prop_to_chr(chr, saved[GUNRIGHT], 3)) {
    saved[GUNRIGHT] = NULL;
  }
  if (!attach_prop_to_chr(chr, saved[GUNLEFT], 5)) {
    saved[GUNLEFT] = NULL;
  }
  if (!attach_prop_to_chr(chr, saved[3], 6)) {
    saved[3] = NULL;
  }

  chr->weapons_held[GUNRIGHT] = saved[GUNRIGHT];
  chr->weapons_held[GUNLEFT] = saved[GUNLEFT];
  // The third legacy slot has no readers or attachment-node semantics in the
  // current decompilation. Preserve it only when it already names this CHR's
  // child; normal runtime state leaves it NULL.
  if (saved[2] != NULL && saved[2]->parent == chr_prop) {
    chr->weapons_held[2] = saved[2];
  }
  chr->handle_positiondata_hat = saved[3];

  // Restore the muzzle flash (GUNFIRE node) visibility last: weapon creation
  // and (re)attachment reset it to hidden, so the saved firing flash would be
  // lost otherwise. weaponSetGunfireVisible is a no-op on non-weapon props.
  if (chr->weapons_held[GUNRIGHT] != NULL) {
    weaponSetGunfireVisible(chr->weapons_held[GUNRIGHT],
                            indices->gunfire_visible[GUNRIGHT]);
  }
  if (chr->weapons_held[GUNLEFT] != NULL) {
    weaponSetGunfireVisible(chr->weapons_held[GUNLEFT],
                            indices->gunfire_visible[GUNLEFT]);
  }
}

static void restore_concealed_chr_item(PropRecord *prop, PropRecord *chr_prop) {
  ObjectRecord *obj;
  ChrRecord *chr;

  if (prop == NULL || chr_prop == NULL || chr_prop->type != PROP_TYPE_CHR ||
      chr_prop->chr == NULL ||
      (prop->type != PROP_TYPE_OBJ && prop->type != PROP_TYPE_WEAPON) ||
      prop->obj == NULL) {
    return;
  }

  chr = chr_prop->chr;
  if (prop == chr->weapons_held[GUNRIGHT] ||
      prop == chr->weapons_held[GUNLEFT] ||
      prop == chr->handle_positiondata_hat) {
    return;
  }

  obj = prop->obj;

  if (prop->parent == NULL && (prop->flags & PROPFLAG_ENABLED)) {
    forget_prop_rooms(prop);
    chrpropDelist(prop);
  } else if (prop->parent != NULL) {
    chrpropDetach(prop);
  }

  chrpropReparent(prop, chr_prop);
  chrpropDisable(prop);
  obj->runtime_bitflags &= ~(RUNTIMEBITFLAG_DEPOSIT | RUNTIMEBITFLAG_EMBEDDED);
  obj->runtime_bitflags |= RUNTIMEBITFLAG_HASOWNER;
  obj->projectile = NULL;
}

static void restore_player_child_object(PropRecord *prop,
                                        PropRecord *player_prop) {
  if (prop == NULL || player_prop == NULL ||
      (player_prop->type != PROP_TYPE_PLAYER &&
       player_prop->type != PROP_TYPE_VIEWER) ||
      (prop->type != PROP_TYPE_OBJ && prop->type != PROP_TYPE_WEAPON) ||
      prop->obj == NULL) {
    return;
  }

  if (prop->parent == NULL && (prop->flags & PROPFLAG_ENABLED)) {
    forget_prop_rooms(prop);
    chrpropDelist(prop);
  } else if (prop->parent != NULL) {
    chrpropDetach(prop);
  }

  chrpropReparent(prop, player_prop);
  chrpropDisable(prop);
  prop->obj->runtime_bitflags |= RUNTIMEBITFLAG_HASOWNER;
  prop->obj->runtime_bitflags &=
      ~(RUNTIMEBITFLAG_DEPOSIT | RUNTIMEBITFLAG_EMBEDDED);
  prop->obj->projectile = NULL;
}

static bool restore_contained_object(PropRecord *prop, PropRecord *parent) {
  if (prop == NULL || parent == NULL ||
      (prop->type != PROP_TYPE_OBJ && prop->type != PROP_TYPE_WEAPON) ||
      (parent->type != PROP_TYPE_OBJ && parent->type != PROP_TYPE_DOOR &&
       parent->type != PROP_TYPE_WEAPON) ||
      prop->obj == NULL || parent->obj == NULL) {
    practiceLogError(
        "Contained prop has invalid restored parent relationship "
        "(prop=%d type=%d obj=%x parent=%d type=%d obj=%x)",
        prop != NULL ? get_prop_index(prop) : -1,
        prop != NULL ? prop->type : -1,
        prop != NULL ? prop->obj : NULL,
        parent != NULL ? get_prop_index(parent) : -1,
        parent != NULL ? parent->type : -1,
        parent != NULL ? parent->obj : NULL);
    assert(FALSE);
    return FALSE;
  }

  if (prop->parent == NULL && (prop->flags & PROPFLAG_ENABLED)) {
    forget_prop_rooms(prop);
    chrpropDelist(prop);
  } else if (prop->parent != NULL) {
    chrpropDetach(prop);
  }

  chrpropReparent(prop, parent);
  return TRUE;
}

static bool restore_embedded_object(PropRecord *prop, PropRecord *parent,
                                    s16 attachmentNodeIndex) {
  ObjectRecord *obj;
  ObjectRecord *parentObj;
  ModelNode *attachmentNode;

  if (prop == NULL || parent == NULL ||
      (prop->type != PROP_TYPE_OBJ && prop->type != PROP_TYPE_WEAPON) ||
      (parent->type != PROP_TYPE_OBJ && parent->type != PROP_TYPE_DOOR &&
       parent->type != PROP_TYPE_WEAPON) ||
      prop->obj == NULL || parent->obj == NULL) {
    practiceLogError("Embedded prop has invalid restored parent relationship");
    assert(FALSE);
    return FALSE;
  }

  obj = prop->obj;
  parentObj = parent->obj;
  if (obj->embedment == NULL || obj->model == NULL ||
      parentObj->model == NULL || parentObj->model->obj == NULL) {
    practiceLogError("Embedded prop has incomplete restored state");
    assert(FALSE);
    return FALSE;
  }

  attachmentNode =
      get_model_node_by_index(parentObj->model->obj, attachmentNodeIndex);
  if (attachmentNode == NULL) {
    practiceLogError("Embedded prop attachment node index %d is invalid",
                     attachmentNodeIndex);
    assert(FALSE);
    return FALSE;
  }

  if (prop->parent == NULL && (prop->flags & PROPFLAG_ENABLED)) {
    forget_prop_rooms(prop);
    chrpropDelist(prop);
  } else if (prop->parent != NULL) {
    objDetach(prop);
  }

  chrpropReparent(prop, parent);
  chrpropDisable(prop);
  obj->model->attachedto = parentObj->model;
  obj->model->attachedto_objinst = attachmentNode;
  return TRUE;
}

static void removePropAtIndex(s16 index) {
  PropRecord *toClear = get_prop_by_index(index);
  if (toClear == NULL)
    return;

  // Embedded mines are disabled and absent from the active list. They still
  // need removing when their slot is absent from the save.
  if (!(toClear->flags & PROPFLAG_ENABLED) && toClear->parent == NULL &&
      !prop_slot_has_live_owner(toClear)) {
    return;
  }

  // Player/viewer presence props are owned by the bond/viewer-player loaders,
  // not the generic prop add/remove path. They are always saved records (so a
  // genuine gap never contains one); never free one here even if a divergent
  // state leaves one in an unexpected slot.
  if (toClear->type == PROP_TYPE_VIEWER || toClear->type == PROP_TYPE_PLAYER) {
    return;
  }

  if (toClear->type == PROP_TYPE_CHR) {
    destroy_chr_prop(toClear, TRUE);
  } else if (toClear->type == PROP_TYPE_OBJ ||
             toClear->type == PROP_TYPE_DOOR ||
             toClear->type == PROP_TYPE_WEAPON) {
    objFreePermanently(toClear->obj, TRUE);
  } else if (toClear->type == PROP_TYPE_EXPLOSION) {
    release_explosion_prop(toClear);
    clear_plain_prop(toClear, TRUE);
  } else if (toClear->type == PROP_TYPE_SMOKE) {
    release_smoke_prop(toClear);
    clear_plain_prop(toClear, TRUE);
  } else {
    clear_plain_prop(toClear, TRUE);
  }
}

static s16 get_prop_index_for_object(ObjectRecord *obj) {
  s32 i;

  if (obj == NULL) {
    return -1;
  }

  // TODO: Is there a faster way to check than looping over all prop entries?
  for (i = 0; i < POS_DATA_ENTRY_LEN; i++) {
    PropRecord *prop = get_enabled_prop_by_index(i);
    if (prop != NULL &&
        // TODO: Can we remove this type check?
        (prop->type == PROP_TYPE_OBJ || prop->type == PROP_TYPE_DOOR ||
         prop->type == PROP_TYPE_WEAPON) &&
        prop->obj == obj) {
      return i;
    }
  }

  return -1;
}

static bool find_model_node_index(ModelNode *node, ModelNode *target,
                                  s32 *nextIndex, s32 limit, s16 *result) {
  while (node != NULL && *nextIndex < limit) {
    s32 currentIndex = (*nextIndex)++;

    if (node == target) {
      *result = currentIndex;
      return TRUE;
    }
    if (find_model_node_index(node->Child, target, nextIndex, limit, result)) {
      return TRUE;
    }
    node = node->Next;
  }

  return FALSE;
}

static s16 get_model_node_index(ModelFileHeader *header, ModelNode *target) {
  s16 result = -1;
  s32 nextIndex = 0;

  if (header == NULL || header->RootNode == NULL || target == NULL) {
    return -1;
  }

  // numRecords counts runtime-data records, not every structural ModelNode, so
  // it cannot bound this traversal. Use the s16 index range as a cycle guard.
  find_model_node_index(header->RootNode, target, &nextIndex, 0x7fff, &result);
  return result;
}

static ModelNode *find_model_node_by_index(ModelNode *node, s16 targetIndex,
                                           s32 *nextIndex, s32 limit) {
  while (node != NULL && *nextIndex < limit) {
    ModelNode *result;
    s32 currentIndex = (*nextIndex)++;

    if (currentIndex == targetIndex) {
      return node;
    }
    result =
        find_model_node_by_index(node->Child, targetIndex, nextIndex, limit);
    if (result != NULL) {
      return result;
    }
    node = node->Next;
  }

  return NULL;
}

static ModelNode *get_model_node_by_index(ModelFileHeader *header,
                                          s16 targetIndex) {
  s32 nextIndex = 0;

  if (header == NULL || header->RootNode == NULL || targetIndex < 0) {
    return NULL;
  }

  return find_model_node_by_index(header->RootNode, targetIndex, &nextIndex,
                                  0x7fff);
}

static void save_projectile(StateStream *stream, Projectile *proj) {
  write_u32(stream, proj->flags);
  write_bytes(stream, &proj->speed, sizeof(coord3d));
  write_bytes(stream, &proj->unk10, sizeof(coord3d));
  write_f32(stream, proj->unk1C);
  write_bytes(stream, &proj->mtx, sizeof(Mtxf));
  write_f32(stream, proj->unk60);
  write_f32(stream, proj->unk64);
  write_bytes(stream, proj->unk68, sizeof(proj->unk68));
  write_bytes(stream, proj->unk78, sizeof(proj->unk78));
  write_u16(stream, get_prop_index(proj->ownerprop));
  write_f32(stream, proj->unk8C);
  write_u32(stream, proj->unk90);
  write_f32(stream, proj->unk94);
  write_u32(stream, proj->unkA0);
  write_u32(stream, proj->unkA4);
  write_u32(stream, proj->unkA8);
  write_u32(stream, proj->unkAC);
  write_f32(stream, proj->unkB0);
  write_f32(stream, proj->unkB4);
  write_u32(stream, proj->droptype);
  write_u32(stream, proj->refreshrate);
  write_f32(stream, proj->unkC0);
  write_f32(stream, proj->unkC4);
  write_f32(stream, proj->unkC8);
  write_u8(stream, proj->unkCC);
  write_u8(stream, proj->unkCD);
  write_u8(stream, proj->unkCE);
  write_u8(stream, proj->unkCF);
  write_u32(stream, proj->unkD0);
  write_f32(stream, proj->unkd4);
  write_u32(stream, proj->unkD8);
  write_u32(stream, proj->unkDC);
  write_u32(stream, proj->unkE0);
  write_u16(stream, get_prop_index_for_object(proj->obj));
  write_u32(stream, proj->unkE8);
}

static void load_projectile(StateStream *stream, Projectile *proj,
                            s16 *ownerprop_idx, s16 *obj_idx) {
  proj->flags = read_u32(stream);
  read_bytes(stream, &proj->speed, sizeof(coord3d));
  read_bytes(stream, &proj->unk10, sizeof(coord3d));
  proj->unk1C = read_f32(stream);
  read_bytes(stream, &proj->mtx, sizeof(Mtxf));
  proj->unk60 = read_f32(stream);
  proj->unk64 = read_f32(stream);
  read_bytes(stream, proj->unk68, sizeof(proj->unk68));
  read_bytes(stream, proj->unk78, sizeof(proj->unk78));
  *ownerprop_idx = (s16)read_u16(stream);
  proj->ownerprop = NULL;
  proj->unk8C = read_f32(stream);
  proj->unk90 = read_u32(stream);
  proj->unk94 = read_f32(stream);
  proj->sound1 = NULL;
  proj->sound2 = NULL;
  proj->unkA0 = read_u32(stream);
  proj->unkA4 = read_u32(stream);
  proj->unkA8 = read_u32(stream);
  proj->unkAC = read_u32(stream);
  proj->unkB0 = read_f32(stream);
  proj->unkB4 = read_f32(stream);
  proj->droptype = read_u32(stream);
  proj->refreshrate = read_u32(stream);
  proj->unkC0 = read_f32(stream);
  proj->unkC4 = read_f32(stream);
  proj->unkC8 = read_f32(stream);
  proj->unkCC = read_u8(stream);
  proj->unkCD = read_u8(stream);
  proj->unkCE = read_u8(stream);
  proj->unkCF = read_u8(stream);
  proj->unkD0 = read_u32(stream);
  proj->unkd4 = read_f32(stream);
  proj->unkD8 = read_u32(stream);
  proj->unkDC = read_u32(stream);
  proj->unkE0 = read_u32(stream);
  *obj_idx = (s16)read_u16(stream);
  proj->obj = NULL;
  proj->unkE8 = read_u32(stream);
}

static void save_embedment(StateStream *stream, Embedment *emb) {
  write_u32(stream, emb->flags);
  write_bytes(stream, &emb->matrix, sizeof(Mtxf));
  write_u16(stream,
            emb->projectile ? (u16)(emb->projectile - g_Projectiles) : -1);
}

static void load_embedment(StateStream *stream, Embedment *emb) {
  emb->flags = read_u32(stream);
  read_bytes(stream, &emb->matrix, sizeof(Mtxf));
  {
    s16 proj_idx = (s16)read_u16(stream);
    emb->projectile = (proj_idx != -1) ? &g_Projectiles[proj_idx] : NULL;
  }
}

static void save_obj_allocation_state(StateStream *stream, PropRecord *prop) {
  ObjectRecord *obj = prop->obj;
  s16 setup_index = obj != NULL ? setupGetCommandIndexByProp(prop) : -1;

  write_u16(stream, obj != NULL ? (u16)obj->obj : (u16)0xffff);
  write_u8(stream, obj != NULL ? (u8)obj->type : (u8)0);
  write_u16(stream, (u16)setup_index);
}

static void load_obj_allocation_state(StateStream *stream,
                                      ObjAllocationState *alloc) {
  alloc->modelnum = read_u16(stream);
  alloc->objtype = read_u8(stream);
  alloc->setupCmdIndex = (s16)read_u16(stream);
}

/*
 * objDeform replaces the first collision display-list node's vertex pointer
 * with a randomly deformed copy. Replaying objDeform on load cannot reproduce
 * the saved mesh (and may deform an already-deformed mesh), so preserve the
 * allocated vertices themselves.
 */
static void save_object_deformation(StateStream *stream, ObjectRecord *obj) {
  ModelNode *node = NULL;
  struct ModelRoData_DisplayList_CollisionRecord *rodata = NULL;
  struct ModelRwData_DisplayList_CollisionRecord *rwdata = NULL;
  u16 vertexCount = 0;

  if (obj != NULL && objGetDestroyedLevel(obj) > 0 && obj->model != NULL &&
      obj->model->obj != NULL) {
    node = sub_GAME_7F04B478(obj);
  }
  if (node != NULL && (node->Opcode & 0xff) == MODELNODE_OPCODE_DLCOLLISION &&
      sub_GAME_7F04B590(obj->model->obj, node)) {
    rodata = (struct ModelRoData_DisplayList_CollisionRecord *)node->Data;
    rwdata =
        (struct ModelRwData_DisplayList_CollisionRecord *)modelGetNodeRwData(
            obj->model, node);
    if (rodata != NULL && rwdata != NULL && rodata->numVertices > 0 &&
        rwdata->Vertices != NULL && rwdata->Vertices != rodata->Vertices) {
      vertexCount = (u16)rodata->numVertices;
    }
  }

  write_u16(stream, vertexCount);
  if (vertexCount > 0) {
    s32 i;

    for (i = 0; i < vertexCount; i++) {
      Vertex *saved = &rwdata->Vertices[i];
      Vertex *base = &rodata->Vertices[i];
      s32 dx = saved->coord.x - base->coord.x;
      s32 dy = saved->coord.y - base->coord.y;
      s32 dz = saved->coord.z - base->coord.z;

      if (saved->index != base->index || saved->s != base->s ||
          saved->t != base->t || dx < -128 || dx > 127 || dz < -128 ||
          dz > 127 || dy < -32768 || dy > 32767) {
        practiceLogError("Destroyed prop deformation invariant failed");
        assert(FALSE);
      }

      write_u8(stream, dx);
      write_u8(stream, dz);
      if (dy >= -127 && dy <= 127) {
        write_u8(stream, dy);
      } else {
        write_u8(stream, 0x80);
        write_u16(stream, dy);
      }
    }
    for (i = 0; i < vertexCount; i += 4) {
      u8 modes = 0;
      s32 j;

      for (j = 0; j < 4 && i + j < vertexCount; j++) {
        modes |= get_deformation_colour_mode(&rwdata->Vertices[i + j],
                                             &rodata->Vertices[i + j])
                 << (j * 2);
      }
      write_u8(stream, modes);
    }
  }
}

static void load_object_deformation(StateStream *stream, ObjectRecord *obj,
                                    bool clearCurrent) {
  u16 vertexCount = read_u16(stream);
  ModelNode *node = NULL;
  struct ModelRoData_DisplayList_CollisionRecord *rodata = NULL;
  struct ModelRwData_DisplayList_CollisionRecord *rwdata = NULL;
  Vertex *vertices = NULL;

  if (obj != NULL && obj->model != NULL && obj->model->obj != NULL) {
    node = sub_GAME_7F04B478(obj);
  }
  if (node != NULL && (node->Opcode & 0xff) == MODELNODE_OPCODE_DLCOLLISION &&
      sub_GAME_7F04B590(obj->model->obj, node)) {
    rodata = (struct ModelRoData_DisplayList_CollisionRecord *)node->Data;
    rwdata =
        (struct ModelRwData_DisplayList_CollisionRecord *)modelGetNodeRwData(
            obj->model, node);
  }

  if ((clearCurrent || vertexCount > 0) && rwdata != NULL && rodata != NULL &&
      rwdata->Vertices != rodata->Vertices) {
    sub_GAME_7F09C044(rwdata->Vertices);
    rwdata->Vertices = rodata->Vertices;
  }

  if (vertexCount == 0) {
    return;
  }

  if (rodata == NULL || rwdata == NULL || rodata->numVertices != vertexCount) {
    skip_object_deformation_vertices(stream, vertexCount);
    practiceLogError(
        "Destroyed prop deformation does not match model (saved=%d, live=%d)",
        vertexCount, rodata != NULL ? rodata->numVertices : -1);
    assert(FALSE);
    return;
  }

  vertices = sub_GAME_7F09BE4C(vertexCount, 0xB0B, obj->model->obj,
                               objGetDestroyedLevel(obj));
  if (vertices == NULL) {
    skip_object_deformation_vertices(stream, vertexCount);
    practiceLogError("Could not allocate destroyed prop deformation vertices");
    assert(FALSE);
    return;
  }

  {
    s32 i;

    for (i = 0; i < vertexCount; i++) {
      Vertex *base = &rodata->Vertices[i];
      s32 dx = (s8)read_u8(stream);
      s32 dz = (s8)read_u8(stream);
      s32 dy = (s8)read_u8(stream);

      if (dy == -128) {
        dy = (s16)read_u16(stream);
      }

      vertices[i] = *base;
      vertices[i].coord.x += dx;
      vertices[i].coord.y += dy;
      vertices[i].coord.z += dz;
    }
    for (i = 0; i < vertexCount; i += 4) {
      u8 modes = read_u8(stream);
      s32 j;

      for (j = 0; j < 4 && i + j < vertexCount; j++) {
        Vertex *vertex = &vertices[i + j];
        u8 colourMode = (modes >> (j * 2)) & 3;

        if (colourMode == 1 || colourMode == 3) {
          vertex->r = 0;
          vertex->g = 0;
          vertex->b = 0;
        }
        if (colourMode == 1) {
          vertex->a = 0xff;
        } else if (colourMode == 2 || colourMode == 3) {
          vertex->a = 0;
        }
      }
    }
  }
  rwdata->Vertices = vertices;
}

/* Offscreen guards can aim using their equipped weapon's matrices from the
 * previous render. Those matrices live in the rotating graphics arena and are
 * not recreated by the paused post-load render when the guard is culled. */
static void save_equipped_weapon_matrices(StateStream *stream,
                                          ObjectRecord *obj) {
  PropRecord *prop = obj != NULL ? obj->prop : NULL;
  Model *model = obj != NULL ? obj->model : NULL;
  s32 indices[MAX_SAVED_EQUIPPED_WEAPON_KEY_MATRICES];
  s32 count = 0;
  s32 switch_index;

  if (prop != NULL && prop->type == PROP_TYPE_WEAPON &&
      prop->parent != NULL && prop->parent->type == PROP_TYPE_CHR &&
      model != NULL && model->obj != NULL && model->render_pos != NULL &&
      model->obj->numMatrices > 0 &&
      model->obj->numMatrices <= MAX_SAVED_EQUIPPED_WEAPON_MATRICES &&
      is_rdram_range(model->render_pos,
                     model->obj->numMatrices * sizeof(RenderPosView))) {
    for (switch_index = 0;
         switch_index < model->obj->numSwitches &&
         switch_index < MAX_EQUIPPED_WEAPON_SWITCH_CANDIDATES && count == 0;
         switch_index++) {
      s32 index = modelFindNodeMtxIndex(
          model->obj->Switches[switch_index], 0);

      if (index >= 0 && index < model->obj->numMatrices) {
        indices[count++] = index;
      }
    }
  }

  write_u8(stream, count);
  for (switch_index = 0; switch_index < count; switch_index++) {
    Mtx saved_matrix;

    write_u8(stream, indices[switch_index]);
    bcopy(&model->render_pos[indices[switch_index]], &saved_matrix,
          sizeof(saved_matrix));
    write_bytes(stream, &saved_matrix, sizeof(saved_matrix));
  }
}

static void load_equipped_weapon_matrices(StateStream *stream,
                                          ObjectRecord *obj) {
  s32 saved_count = read_u8(stream);
  Model *model = obj != NULL ? obj->model : NULL;
  bool can_restore =
      saved_count <= MAX_SAVED_EQUIPPED_WEAPON_KEY_MATRICES &&
      model != NULL && model->obj != NULL && model->obj->numMatrices > 0 &&
      model->obj->numMatrices <= MAX_SAVED_EQUIPPED_WEAPON_MATRICES;
  RenderPosView *render_pos = NULL;
  s32 matrix;

  if (can_restore && saved_count > 0) {
    Mtxf identity_float;
    Mtx identity_fixed;

    matrix_4x4_set_identity(&identity_float);
    guMtxF2L(identity_float.m, &identity_fixed);
    render_pos = dynAllocate(model->obj->numMatrices * sizeof(RenderPosView));
    for (matrix = 0; matrix < model->obj->numMatrices; matrix++) {
      bcopy(&identity_fixed, &render_pos[matrix], sizeof(identity_fixed));
    }
    model->render_pos = render_pos;
  }

  for (matrix = 0; matrix < saved_count; matrix++) {
    s32 index = read_u8(stream);
    Mtx saved_matrix;

    read_bytes(stream, &saved_matrix, sizeof(saved_matrix));
    if (render_pos != NULL && index < model->obj->numMatrices) {
      bcopy(&saved_matrix, &render_pos[index], sizeof(saved_matrix));
    }
  }
}

static void save_affine_matrix(StateStream *stream, const Mtxf *matrix,
                               const coord3d *position) {
  bool is_affine = matrix->m[0][3] == 0.0f && matrix->m[1][3] == 0.0f &&
                   matrix->m[2][3] == 0.0f && matrix->m[3][3] == 1.0f;
  u8 mode = is_affine ? 1 : 0;
  s32 row;

  if (is_affine && matrix->m[3][0] == position->f[0] &&
      matrix->m[3][1] == position->f[1] &&
      matrix->m[3][2] == position->f[2]) {
    mode = 2;
  } else if (is_affine && matrix->m[3][0] == 0.0f &&
             matrix->m[3][1] == 0.0f && matrix->m[3][2] == 0.0f) {
    mode = 3;
  }
  write_u8(stream, mode);
  if (!is_affine) {
    write_bytes(stream, matrix, sizeof(*matrix));
    return;
  }
  for (row = 0; row < (mode >= 2 ? 3 : 4); row++) {
    write_f32(stream, matrix->m[row][0]);
    write_f32(stream, matrix->m[row][1]);
    write_f32(stream, matrix->m[row][2]);
  }
}

static u8 load_affine_matrix(StateStream *stream, Mtxf *matrix) {
  u8 mode = read_u8(stream);
  s32 row;

  if (mode == 0) {
    read_bytes(stream, matrix, sizeof(*matrix));
    return mode;
  }
  if (mode > 3) {
    practiceLogError("Saved object matrix mode is invalid (%d)", mode);
    assert(FALSE);
    mode = 1;
  }
  for (row = 0; row < (mode >= 2 ? 3 : 4); row++) {
    matrix->m[row][0] = read_f32(stream);
    matrix->m[row][1] = read_f32(stream);
    matrix->m[row][2] = read_f32(stream);
    matrix->m[row][3] = 0.0f;
  }
  if (mode >= 2) {
    matrix->m[3][0] = 0.0f;
    matrix->m[3][1] = 0.0f;
    matrix->m[3][2] = 0.0f;
  }
  matrix->m[3][3] = 1.0f;
  return mode;
}

static void save_object_base(StateStream *stream, ObjectRecord *obj) {
  s16 projIdx = -1;
  s16 embIdx = -1;
  s16 attachmentNodeIdx = -1;
  u32 switchStates = 0;

  write_u16(stream, obj->extrascale);
  write_u8(stream, obj->state);
  write_u8(stream, obj->type);
  write_u16(stream, obj->obj);
  write_u16(stream, obj->pad);
  write_u32(stream, obj->flags);
  write_u32(stream, obj->flags2);
  save_affine_matrix(stream, &obj->mtx, &obj->runtime_pos);
  write_bytes(stream, &obj->runtime_pos, sizeof(coord3d));
  write_u32(stream, obj->runtime_bitflags);
  write_f32(stream, obj->maxdamage);
  write_f32(stream, obj->damage);
  write_u32(stream, *(u32 *)&obj->shadecol);
  write_u32(stream, *(u32 *)&obj->nextcol);

  if ((obj->runtime_bitflags & RUNTIMEBITFLAG_DEPOSIT) &&
      obj->projectile != NULL) {
    projIdx = (s16)(obj->projectile - g_Projectiles);
  }
  write_u16(stream, projIdx);

  if ((obj->runtime_bitflags & RUNTIMEBITFLAG_EMBEDDED) &&
      obj->embedment != NULL) {
    embIdx = (s16)(obj->embedment - g_Embedments);
  }
  write_u16(stream, embIdx);

  if (obj->runtime_bitflags & RUNTIMEBITFLAG_EMBEDDED) {
    if (obj->prop == NULL || obj->prop->parent == NULL || obj->model == NULL ||
        obj->model->attachedto == NULL ||
        obj->model->attachedto_objinst == NULL) {
      practiceLogError("Embedded prop has incomplete attachment pointers");
      assert(FALSE);
    } else {
      attachmentNodeIdx = get_model_node_index(obj->model->attachedto->obj,
                                               obj->model->attachedto_objinst);
      if (attachmentNodeIdx < 0) {
        practiceLogError(
            "Embedded prop attachment node is not in parent model");
        assert(FALSE);
      }
    }
  }
  write_u16(stream, attachmentNodeIdx);

  if (obj->model != NULL && obj->model->obj != NULL) {
    s32 i;
    s32 numSw = obj->model->obj->numSwitches;
    if (numSw > 32) {
      numSw = 32;
    }
    for (i = 0; i < numSw; i++) {
      ModelNode *switchNode = obj->model->obj->Switches[i];
      if (switchNode != NULL &&
          (switchNode->Opcode & 0xFF) == MODELNODE_OPCODE_SWITCH) {
        union ModelRwData *rwdata = modelGetNodeRwData(obj->model, switchNode);
        if (rwdata != NULL && rwdata->Switch.visible) {
          switchStates |= (1 << i);
        }
      }
    }
  }
  write_u32(stream, switchStates);
  save_object_deformation(stream, obj);
}

static bool load_object_base(StateStream *stream, ObjectRecord *obj,
                             PropRecord *prop, s16 *attachmentNodeIdx) {
  PropDefHeaderRecord *pdhr = (PropDefHeaderRecord *)obj;
  bool liveWasDestroyed = prop != NULL && objGetDestroyedLevel(obj) > 0;
  s16 projectileIdx;
  s16 embedmentIdx;
  u32 switchStates;
  u8 matrixMode;

  pdhr->extrascale = read_u16(stream);
  pdhr->state = read_u8(stream);
  pdhr->type = read_u8(stream);
  obj->obj = read_u16(stream);
  obj->pad = read_u16(stream);
  obj->flags = read_u32(stream);
  obj->flags2 = read_u32(stream);
  matrixMode = load_affine_matrix(stream, &obj->mtx);
  read_bytes(stream, &obj->runtime_pos, sizeof(coord3d));
  if (matrixMode == 2) {
    obj->mtx.m[3][0] = obj->runtime_pos.f[0];
    obj->mtx.m[3][1] = obj->runtime_pos.f[1];
    obj->mtx.m[3][2] = obj->runtime_pos.f[2];
  }
  obj->runtime_bitflags = read_u32(stream);
  obj->maxdamage = read_f32(stream);
  obj->damage = read_f32(stream);
  *(u32 *)&obj->shadecol = read_u32(stream);
  *(u32 *)&obj->nextcol = read_u32(stream);

  projectileIdx = (s16)read_u16(stream);
  embedmentIdx = (s16)read_u16(stream);
  if (attachmentNodeIdx != NULL) {
    *attachmentNodeIdx = (s16)read_u16(stream);
  } else {
    read_u16(stream);
  }
  switchStates = read_u32(stream);

  // Projectile and embedment occupy the same union slot. Restore only the
  // member selected by the runtime flags; writing both would overwrite the
  // first pointer with the second.
  if (prop != NULL && (obj->runtime_bitflags & RUNTIMEBITFLAG_DEPOSIT) &&
      projectileIdx >= 0 && projectileIdx < PROJECTILES_ARR_MAX) {
    obj->projectile = &g_Projectiles[projectileIdx];
    obj->projectile->obj = obj;
  } else if (prop != NULL &&
             (obj->runtime_bitflags & RUNTIMEBITFLAG_EMBEDDED) &&
             embedmentIdx >= 0 && embedmentIdx < EMBEDMENT_ARR_MAX) {
    obj->embedment = &g_Embedments[embedmentIdx];
  } else {
    obj->projectile = NULL;
    obj->runtime_bitflags &=
        ~(RUNTIMEBITFLAG_DEPOSIT | RUNTIMEBITFLAG_EMBEDDED);
  }

  if (prop != NULL && obj->model != NULL && obj->model->obj != NULL) {
    s32 i;
    s32 numSw = obj->model->obj->numSwitches;
    if (numSw > 32) {
      numSw = 32;
    }
    for (i = 0; i < numSw; i++) {
      ModelNode *switchNode = obj->model->obj->Switches[i];
      if (switchNode != NULL &&
          (switchNode->Opcode & 0xFF) == MODELNODE_OPCODE_SWITCH) {
        union ModelRwData *rwdata = modelGetNodeRwData(obj->model, switchNode);
        if (rwdata != NULL) {
          rwdata->Switch.visible = (switchStates & (1 << i)) != 0;
        }
      }
    }
    sub_GAME_7F050DE8(obj->model);
  }

  // Toggle-relation rebuilding can reset display-list runtime data, so install
  // the saved vertex buffer only after the model's switches are reapplied.
  load_object_deformation(stream, prop != NULL ? obj : NULL, liveWasDestroyed);

  return TRUE;
}

static void save_compact_contained_object_base(StateStream *stream,
                                               ObjectRecord *obj) {
  u32 switchStates = 0;
  s32 i;

  write_u16(stream, obj->extrascale);
  write_u8(stream, obj->state);
  write_u8(stream, obj->type);
  write_u16(stream, obj->obj);
  write_u16(stream, obj->pad);
  write_u32(stream, obj->flags);
  write_u32(stream, obj->flags2);
  write_u32(stream, obj->runtime_bitflags);
  write_f32(stream, obj->maxdamage);
  write_f32(stream, obj->damage);
  write_u32(stream, *(u32 *)&obj->shadecol);
  write_u32(stream, *(u32 *)&obj->nextcol);

  if (obj->model != NULL && obj->model->obj != NULL) {
    s32 numSwitches = obj->model->obj->numSwitches;
    if (numSwitches > 32) {
      numSwitches = 32;
    }
    for (i = 0; i < numSwitches; i++) {
      ModelNode *node = obj->model->obj->Switches[i];
      if (node != NULL &&
          (node->Opcode & 0xff) == MODELNODE_OPCODE_SWITCH) {
        union ModelRwData *rwdata = modelGetNodeRwData(obj->model, node);
        if (rwdata != NULL && rwdata->Switch.visible) {
          switchStates |= 1 << i;
        }
      }
    }
  }
  write_u32(stream, switchStates);
}

static void load_compact_contained_object_base(StateStream *stream,
                                               ObjectRecord *obj,
                                               PropRecord *prop) {
  PropDefHeaderRecord *pdhr = (PropDefHeaderRecord *)obj;
  u32 switchStates;
  s32 i;

  pdhr->extrascale = read_u16(stream);
  pdhr->state = read_u8(stream);
  pdhr->type = read_u8(stream);
  obj->obj = read_u16(stream);
  obj->pad = read_u16(stream);
  obj->flags = read_u32(stream);
  obj->flags2 = read_u32(stream);
  obj->runtime_bitflags = read_u32(stream) &
                          ~(RUNTIMEBITFLAG_DEPOSIT | RUNTIMEBITFLAG_EMBEDDED);
  obj->maxdamage = read_f32(stream);
  obj->damage = read_f32(stream);
  *(u32 *)&obj->shadecol = read_u32(stream);
  *(u32 *)&obj->nextcol = read_u32(stream);
  switchStates = read_u32(stream);
  obj->projectile = NULL;

  if (prop != NULL && obj->model != NULL && obj->model->obj != NULL) {
    s32 numSwitches = obj->model->obj->numSwitches;
    if (numSwitches > 32) {
      numSwitches = 32;
    }
    for (i = 0; i < numSwitches; i++) {
      ModelNode *node = obj->model->obj->Switches[i];
      if (node != NULL &&
          (node->Opcode & 0xff) == MODELNODE_OPCODE_SWITCH) {
        union ModelRwData *rwdata = modelGetNodeRwData(obj->model, node);
        if (rwdata != NULL) {
          rwdata->Switch.visible = (switchStates & (1 << i)) != 0;
        }
      }
    }
    sub_GAME_7F050DE8(obj->model);
  }
}

static void save_monitor_record(StateStream *stream,
                                struct MonitorRecord *mon) {
  write_u32(stream, (u32)mon->cmdlist);
  write_u16(stream, mon->offset);
  write_u16(stream, mon->pause60);
  write_u32(stream, (u32)mon->tconfig);
  write_f32(stream, mon->rot);
  write_f32(stream, mon->xscale);
  write_f32(stream, mon->xscalefrac);
  write_f32(stream, mon->xscaleinc);
  write_f32(stream, mon->xscaleold);
  write_f32(stream, mon->xscalenew);
  write_f32(stream, mon->yscale);
  write_f32(stream, mon->yscalefrac);
  write_f32(stream, mon->yscaleinc);
  write_f32(stream, mon->yscaleold);
  write_f32(stream, mon->yscalenew);
  write_f32(stream, mon->xmid);
  write_f32(stream, mon->xmidfrac);
  write_f32(stream, mon->xmidinc);
  write_f32(stream, mon->xmidold);
  write_f32(stream, mon->xmidnew);
  write_f32(stream, mon->ymid);
  write_f32(stream, mon->ymidfrac);
  write_f32(stream, mon->ymidinc);
  write_f32(stream, mon->ymidold);
  write_f32(stream, mon->ymidnew);
  write_u8(stream, mon->red);
  write_u8(stream, mon->redold);
  write_u8(stream, mon->rednew);
  write_u8(stream, mon->green);
  write_u8(stream, mon->greenold);
  write_u8(stream, mon->greennew);
  write_u8(stream, mon->blue);
  write_u8(stream, mon->blueold);
  write_u8(stream, mon->bluenew);
  write_u8(stream, mon->alpha);
  write_u8(stream, mon->alphaold);
  write_u8(stream, mon->alphanew);
  write_f32(stream, mon->colfrac);
  write_f32(stream, mon->colinc);
}

static void load_monitor_record(StateStream *stream,
                                struct MonitorRecord *mon) {
  mon->cmdlist = (u32 *)read_u32(stream);
  mon->offset = read_u16(stream);
  mon->pause60 = read_u16(stream);
  mon->tconfig = (void *)read_u32(stream);
  mon->rot = read_f32(stream);
  mon->xscale = read_f32(stream);
  mon->xscalefrac = read_f32(stream);
  mon->xscaleinc = read_f32(stream);
  mon->xscaleold = read_f32(stream);
  mon->xscalenew = read_f32(stream);
  mon->yscale = read_f32(stream);
  mon->yscalefrac = read_f32(stream);
  mon->yscaleinc = read_f32(stream);
  mon->yscaleold = read_f32(stream);
  mon->yscalenew = read_f32(stream);
  mon->xmid = read_f32(stream);
  mon->xmidfrac = read_f32(stream);
  mon->xmidinc = read_f32(stream);
  mon->xmidold = read_f32(stream);
  mon->xmidnew = read_f32(stream);
  mon->ymid = read_f32(stream);
  mon->ymidfrac = read_f32(stream);
  mon->ymidinc = read_f32(stream);
  mon->ymidold = read_f32(stream);
  mon->ymidnew = read_f32(stream);
  mon->red = read_u8(stream);
  mon->redold = read_u8(stream);
  mon->rednew = read_u8(stream);
  mon->green = read_u8(stream);
  mon->greenold = read_u8(stream);
  mon->greennew = read_u8(stream);
  mon->blue = read_u8(stream);
  mon->blueold = read_u8(stream);
  mon->bluenew = read_u8(stream);
  mon->alpha = read_u8(stream);
  mon->alphaold = read_u8(stream);
  mon->alphanew = read_u8(stream);
  mon->colfrac = read_f32(stream);
  mon->colinc = read_f32(stream);
}

static void load_door_record(StateStream *stream, DoorRecord *door) {
  door->maxFrac = read_f32(stream);
  door->perimFrac = read_f32(stream);
  door->accel = read_f32(stream);
  door->decel = read_f32(stream);
  door->maxSpeed = read_f32(stream);
  door->doorFlags = read_u16(stream);
  door->doorType = read_u16(stream);
  door->keyflags = read_u32(stream);
  door->autoCloseFrames = read_u32(stream);
  door->doorOpenSound = read_u32(stream);
  door->frac = read_f32(stream);
  door->unkac = read_f32(stream);
  door->unkb0 = read_f32(stream);
  door->openPosition = read_f32(stream);
  door->speed = read_f32(stream);
  door->openstate = read_u8(stream);
  door->unkbd = read_u8(stream);
  door->calculatedopacity = read_u16(stream);
  door->TintDist = read_u32(stream);
  door->CullDist = read_u16(stream);
  door->soundType = read_u8(stream);
  door->fadeTime60 = read_u8(stream);
  read_bytes(stream, &door->bbox, sizeof(struct ModelRoData_BoundingBoxRecord));
  door->openedTime = read_u32(stream);
  door->portalNumber = read_u32(stream);
  door->openSoundState = NULL;
  door->closeSoundState = NULL;
  door->lastcalc60i = read_u32(stream);
}

static void write_effect_zero_rle(StateStream *stream, const u8 *src,
                                  u32 size) {
  u32 offset = 0;

  while (offset < size) {
    u32 zero_count = 0;
    u32 literal_start;
    u32 literal_count;

    while (offset + zero_count < size && src[offset + zero_count] == 0 &&
           zero_count < 128) {
      zero_count++;
    }
    if (zero_count >= 2) {
      write_u8(stream, 0x80 | (zero_count - 1));
      offset += zero_count;
      continue;
    }
    literal_start = offset;
    literal_count = 0;
    while (offset < size && literal_count < 128) {
      zero_count = 0;
      while (offset + zero_count < size && src[offset + zero_count] == 0 &&
             zero_count < 2) {
        zero_count++;
      }
      if (zero_count >= 2) {
        break;
      }
      offset++;
      literal_count++;
    }
    write_u8(stream, literal_count - 1);
    write_bytes(stream, src + literal_start, literal_count);
  }
}

static void read_effect_zero_rle(StateStream *stream, u8 *dst, u32 size) {
  u32 offset = 0;

  while (offset < size) {
    u8 control = read_u8(stream);
    u32 count = (control & 0x7f) + 1;

    if (count > size - offset) {
      practiceLogError("Effect state RLE exceeds destination size");
      assert(FALSE);
      count = size - offset;
    }
    if (control & 0x80) {
      bzero(dst + offset, count);
    } else {
      read_bytes(stream, dst + offset, count);
    }
    offset += count;
  }
}

static void save_explosion_record(StateStream *stream,
                                  struct Explosion *explosion) {
  write_u16(stream, get_prop_index(explosion->source));
  write_effect_zero_rle(stream, (u8 *)explosion->parts,
                        sizeof(explosion->parts));
  write_u16(stream, explosion->age);
  write_u16(stream, explosion->unk3CA);
  write_u8(stream, explosion->explosion_type);
  write_u8(stream, explosion->unk3CD);
  write_u8(stream, explosion->player);
  write_u8(stream, explosion->unk3CF);
  write_bytes(stream, &explosion->pos, sizeof(explosion->pos));
  write_u16(stream, explosion->room);
  write_u16(stream, explosion->unk3DE);
}

static void load_explosion_record(StateStream *stream,
                                  struct Explosion *explosion) {
  s16 source_index = read_u16(stream);

  explosion->source = get_enabled_prop_by_index(source_index);
  read_effect_zero_rle(stream, (u8 *)explosion->parts,
                       sizeof(explosion->parts));
  explosion->age = read_u16(stream);
  explosion->unk3CA = read_u16(stream);
  explosion->explosion_type = read_u8(stream);
  explosion->unk3CD = read_u8(stream);
  explosion->player = read_u8(stream);
  explosion->unk3CF = read_u8(stream);
  read_bytes(stream, &explosion->pos, sizeof(explosion->pos));
  explosion->room = read_u16(stream);
  explosion->unk3DE = read_u16(stream);
}

static void save_smoke_record(StateStream *stream, struct Smoke *smoke) {
  write_u16(stream, smoke->duration);
  write_u16(stream, smoke->smoke_type);
  write_effect_zero_rle(stream, (u8 *)smoke->parts, sizeof(smoke->parts));
}

static void load_smoke_record(StateStream *stream, struct Smoke *smoke) {
  smoke->duration = read_u16(stream);
  smoke->smoke_type = read_u16(stream);
  read_effect_zero_rle(stream, (u8 *)smoke->parts, sizeof(smoke->parts));
}

/* Free effect-buffer entries retain particle data in the original engine.
 * Future effects reuse that data, so clearing a later-timeline effect while
 * loading must be followed by restoring the saved free-entry contents. */
static void save_free_effect_entries(StateStream *stream) {
  s32 i;

  for (i = 0; i < EXPLOSION_BUFFER_LEN; i++) {
    struct Explosion *explosion = &g_ExplosionBuffer[i];
    bool is_free = explosion->prop == NULL;
    write_u8(stream, is_free);
    if (is_free) {
      write_effect_zero_rle(stream, (u8 *)explosion->parts,
                            sizeof(*explosion) - 2 * sizeof(void *));
    }
  }
  for (i = 0; i < SMOKE_BUFFER_LEN; i++) {
    struct Smoke *smoke = &g_SmokeBuffer[i];
    bool is_free = smoke->prop == NULL;
    write_u8(stream, is_free);
    if (is_free) {
      write_effect_zero_rle(stream, (u8 *)&smoke->duration,
                            sizeof(*smoke) - sizeof(void *));
    }
  }
}

static void load_free_effect_entries(StateStream *stream) {
  s32 i;

  for (i = 0; i < EXPLOSION_BUFFER_LEN; i++) {
    if (read_u8(stream)) {
      struct Explosion *explosion = &g_ExplosionBuffer[i];
      if (explosion->prop != NULL) {
        practiceLogError("Saved free explosion entry %d is still active", i);
        assert(FALSE);
      }
      explosion->source = NULL;
      read_effect_zero_rle(stream, (u8 *)explosion->parts,
                           sizeof(*explosion) - 2 * sizeof(void *));
    }
  }
  for (i = 0; i < SMOKE_BUFFER_LEN; i++) {
    if (read_u8(stream)) {
      struct Smoke *smoke = &g_SmokeBuffer[i];
      if (smoke->prop != NULL) {
        practiceLogError("Saved free smoke entry %d is still active", i);
        assert(FALSE);
      }
      read_effect_zero_rle(stream, (u8 *)&smoke->duration,
                           sizeof(*smoke) - sizeof(void *));
    }
  }
}

// Scorch marks (explosion burn circles) and bullet holes live in global ring
// buffers, not in PropRecords. Both are saved sparsely: the ring cursor plus
// the active entries (identified by a non-negative room id) at their exact
// buffer index, so future scorches/impacts keep filling the ring in the same
// order.
//
// Scorch geometry is entirely world space; its `model` pointer is unused by the
// renderer. Bullet impacts attached to an object/door reference it through its
// prop index (resolved after every prop is restored); world-surface impacts
// have no prop. Must run after the prop table is rebuilt so those indices
// resolve.
static void save_decals_state(StateStream *stream) {
  s32 i;
  u16 count;

  write_u32(stream, g_NumScorchEntries);
  count = 0;
  if (g_ScorchBuffer != NULL) {
    for (i = 0; i < SCORCH_BUFFER_LEN; i++) {
      if (g_ScorchBuffer[i].roomid >= 0) {
        count++;
      }
    }
  }
  write_u16(stream, count);
  if (g_ScorchBuffer != NULL) {
    for (i = 0; i < SCORCH_BUFFER_LEN; i++) {
      struct Scorch *scorch = &g_ScorchBuffer[i];
      if (scorch->roomid < 0) {
        continue;
      }
      write_u16(stream, (u16)i);
      write_u16(stream, scorch->roomid);
      write_u8(stream, scorch->unk02);
      write_u8(stream, scorch->unk03);
      write_bytes(stream, &scorch->pos, sizeof(coord3d));
      write_f32(stream, scorch->explosion_size);
      write_effect_zero_rle(stream, (u8 *)scorch->vertex_list,
                            sizeof(scorch->vertex_list));
    }
  }

  write_u32(stream, g_NumImpactEntries);
  count = 0;
  if (g_BulletImpactBuffer != NULL) {
    for (i = 0; i < BULLET_IMPACT_BUFFER_LEN; i++) {
      if (g_BulletImpactBuffer[i].room >= 0) {
        count++;
      }
    }
  }
  write_u16(stream, count);
  if (g_BulletImpactBuffer != NULL) {
    for (i = 0; i < BULLET_IMPACT_BUFFER_LEN; i++) {
      struct BulletImpact *impact = &g_BulletImpactBuffer[i];
      if (impact->room < 0) {
        continue;
      }
      write_u16(stream, (u16)i);
      write_u16(stream, impact->room);
      write_u16(stream, impact->impact_type);
      write_u32(stream, impact->unk04);
      write_effect_zero_rle(stream, (u8 *)impact->vertex_list,
                            sizeof(impact->vertex_list));
      write_u16(stream, get_prop_index(impact->prop));
      write_u8(stream, impact->model_render_pos_index);
      write_u8(stream, impact->room_clear_flag);
      write_u16(stream, impact->unk4E);
    }
  }
}

static void load_decals_state(StateStream *stream) {
  s32 i;
  u16 count;

  g_NumScorchEntries = read_u32(stream);
  if (g_ScorchBuffer != NULL) {
    for (i = 0; i < SCORCH_BUFFER_LEN; i++) {
      g_ScorchBuffer[i].roomid = -1;
    }
  }
  count = read_u16(stream);
  for (i = 0; i < count; i++) {
    struct Scorch tmp;
    u16 index = read_u16(stream);
    tmp.roomid = read_u16(stream);
    tmp.unk02 = read_u8(stream);
    tmp.unk03 = read_u8(stream);
    read_bytes(stream, &tmp.pos, sizeof(coord3d));
    tmp.explosion_size = read_f32(stream);
    read_effect_zero_rle(stream, (u8 *)tmp.vertex_list,
                         sizeof(tmp.vertex_list));
    tmp.model = NULL;
    if (g_ScorchBuffer != NULL && index < SCORCH_BUFFER_LEN) {
      g_ScorchBuffer[index] = tmp;
    }
  }

  g_NumImpactEntries = read_u32(stream);
  if (g_BulletImpactBuffer != NULL) {
    for (i = 0; i < BULLET_IMPACT_BUFFER_LEN; i++) {
      g_BulletImpactBuffer[i].room = -1;
    }
  }
  count = read_u16(stream);
  for (i = 0; i < count; i++) {
    struct BulletImpact tmp;
    u16 index = read_u16(stream);
    s16 propIdx;
    tmp.room = read_u16(stream);
    tmp.impact_type = read_u16(stream);
    tmp.unk04 = read_u32(stream);
    read_effect_zero_rle(stream, (u8 *)tmp.vertex_list,
                         sizeof(tmp.vertex_list));
    propIdx = (s16)read_u16(stream);
    tmp.model_render_pos_index = read_u8(stream);
    tmp.room_clear_flag = read_u8(stream);
    tmp.unk4E = read_u16(stream);

    // Drop an impact whose owning prop is gone: its stale
    // model_render_pos_index would index a freed/replaced model on the next
    // render.
    if (propIdx >= 0) {
      tmp.prop = get_enabled_prop_by_index(propIdx);
      if (tmp.prop == NULL) {
        tmp.room = -1;
      }
    } else {
      tmp.prop = NULL;
    }

    if (g_BulletImpactBuffer != NULL && index < BULLET_IMPACT_BUFFER_LEN) {
      g_BulletImpactBuffer[index] = tmp;
    }
  }
}

// Airborne explosion shrapnel/debris (`g_FlyingParticlesBuffer`, ring cursor
// `g_NumParticleEntries`, capacity `max_particles`). Each entry is a fully
// world-space, pointer-free quad; `unk00 > 0` marks it live and counts down its
// lifetime, while `vertex_list` holds the per-particle local geometry/colour
// set once at spawn (the renderer rebuilds the world matrix from
// position/rotation each frame). An interleaved occupancy bitmap preserves the
// exact buffer slots without spending a byte on every live particle index.
static void save_flying_particles_state(StateStream *stream) {
  s32 block;

  write_u32(stream, g_NumParticleEntries);
  for (block = 0; block < max_particles; block += 8) {
    u8 occupied = 0;
    s32 bit;

    if (g_FlyingParticlesBuffer != NULL) {
      for (bit = 0; bit < 8 && block + bit < max_particles; bit++) {
        if (g_FlyingParticlesBuffer[block + bit].unk00 > 0) {
          occupied |= 1 << bit;
        }
      }
    }
    write_u8(stream, occupied);

    if (g_FlyingParticlesBuffer != NULL) {
      for (bit = 0; bit < 8 && block + bit < max_particles; bit++) {
        s32 i = block + bit;

        if (g_FlyingParticlesBuffer[i].unk00 > 0) {
          struct FlyingParticles *particle = &g_FlyingParticlesBuffer[i];
          Vtx *vertices = particle->vertex_list;
          s32 vertex;
          u32 geometry = 0;
          u32 shades = 0;
          u32 appearance;
          bool bright_shades;
          bool vertices_are_compact = TRUE;

          for (vertex = 0; vertex < 4; vertex++) {
            vertices_are_compact &= vertices[vertex].v.ob[1] == 0;
            vertices_are_compact &= vertices[vertex].v.ob[0] >= -15 &&
                                    vertices[vertex].v.ob[0] <= 15 &&
                                    vertices[vertex].v.ob[2] >= -15 &&
                                    vertices[vertex].v.ob[2] <= 15;
            vertices_are_compact &=
                vertices[vertex].v.cn[0] == vertices[vertex].v.cn[1] &&
                vertices[vertex].v.cn[0] == vertices[vertex].v.cn[2] &&
                vertices[vertex].v.cn[3] == vertices[0].v.cn[3];
          }
          vertices_are_compact &=
              particle->unk00 > 0 && particle->unk00 <= 0xffff &&
              vertices[0].v.ob[0] >= 0 && vertices[0].v.ob[2] >= 0 &&
              vertices[1].v.ob[0] >= 0 && vertices[1].v.ob[2] <= 0 &&
              vertices[2].v.ob[0] <= 0 && vertices[2].v.ob[2] <= 0 &&
              vertices[3].v.ob[0] <= 0 && vertices[3].v.ob[2] >= 0 &&
              vertices[2].v.tc[0] >= 0 && vertices[2].v.tc[0] <= 0x300 &&
              vertices[2].v.tc[1] >= 0 && vertices[2].v.tc[1] <= 0x300 &&
              (vertices[2].v.tc[0] & 0xff) == 0 &&
              (vertices[2].v.tc[1] & 0xff) == 0 &&
              vertices[0].v.cn[3] == 0xdc &&
              vertices[0].v.tc[0] == (s16)(vertices[2].v.tc[0] + 0xe0) &&
              vertices[0].v.tc[1] == (s16)(vertices[2].v.tc[1] + 0xe0) &&
              vertices[1].v.tc[0] == (s16)(vertices[2].v.tc[0] + 0xe0) &&
              vertices[1].v.tc[1] == vertices[2].v.tc[1] &&
              vertices[3].v.tc[0] == vertices[2].v.tc[0] &&
              vertices[3].v.tc[1] == (s16)(vertices[2].v.tc[1] + 0xe0);
          bright_shades = vertices[0].v.cn[0] >= 0xc0;
          for (vertex = 0; vertex < 4; vertex++) {
            vertices_are_compact &=
                bright_shades ? (vertices[vertex].v.cn[0] >= 0xc0)
                              : (vertices[vertex].v.cn[0] <= 0x3f);
          }
          if (!vertices_are_compact) {
            practiceLogError("Flying particle %d vertex invariant failed", i);
            assert(FALSE);
          }

          write_bytes(stream, &particle->position, sizeof(coord3d));
          write_bytes(stream, &particle->rotation, sizeof(coord3d));
          write_bytes(stream, &particle->position_drift, sizeof(coord3d));
          write_bytes(stream, &particle->rotation_drift, sizeof(coord3d));
          for (vertex = 0; vertex < 4; vertex++) {
            s32 x = vertices[vertex].v.ob[0];
            s32 z = vertices[vertex].v.ob[2];

            geometry |= (x < 0 ? -x : x) << (vertex * 8);
            geometry |= (z < 0 ? -z : z) << (vertex * 8 + 4);
          }
          write_u32(stream, geometry);

          appearance = particle->unk00 & 0xff;
          appearance |= (vertices[2].v.tc[0] >> 8) << 8;
          appearance |= (vertices[2].v.tc[1] >> 8) << 10;
          appearance |= bright_shades << 12;
          appearance |= (particle->unk00 > 0xff) << 13;
          for (vertex = 0; vertex < 4; vertex++) {
            shades |= (vertices[vertex].v.cn[0] & 0x3f) << (vertex * 6);
          }
          write_u16(stream, appearance);
          if (particle->unk00 > 0xff) {
            write_u8(stream, particle->unk00 >> 8);
          }
          write_u8(stream, shades);
          write_u8(stream, shades >> 8);
          write_u8(stream, shades >> 16);
        }
      }
    }
  }
}

static void load_flying_particles_state(StateStream *stream) {
  s32 block;

  g_NumParticleEntries = read_u32(stream);
  if (g_FlyingParticlesBuffer != NULL) {
    for (block = 0; block < max_particles; block++) {
      g_FlyingParticlesBuffer[block].unk00 = 0;
    }
  }
  for (block = 0; block < max_particles; block += 8) {
    u8 occupied = read_u8(stream);
    s32 bit;

    for (bit = 0; bit < 8 && block + bit < max_particles; bit++) {
      struct FlyingParticles tmp;
      s32 index = block + bit;
      s16 texture_s;
      s16 texture_t;
      s32 vertex;
      u32 geometry;
      u32 shades;
      u32 appearance;
      bool bright_shades;

      if (!(occupied & (1 << bit))) {
        continue;
      }

      read_bytes(stream, &tmp.position, sizeof(coord3d));
      read_bytes(stream, &tmp.rotation, sizeof(coord3d));
      read_bytes(stream, &tmp.position_drift, sizeof(coord3d));
      read_bytes(stream, &tmp.rotation_drift, sizeof(coord3d));
      tmp.unk34 = 0;
      geometry = read_u32(stream);
      for (vertex = 0; vertex < 4; vertex++) {
        s32 x = (geometry >> (vertex * 8)) & 0xf;
        s32 z = (geometry >> (vertex * 8 + 4)) & 0xf;

        tmp.vertex_list[vertex].v.ob[0] = vertex >= 2 ? -x : x;
        tmp.vertex_list[vertex].v.ob[1] = 0;
        tmp.vertex_list[vertex].v.ob[2] =
            vertex == 1 || vertex == 2 ? -z : z;
        tmp.vertex_list[vertex].v.flag = 0;
      }
      appearance = read_u16(stream);
      tmp.unk00 = appearance & 0xff;
      if (appearance & (1 << 13)) {
        tmp.unk00 |= read_u8(stream) << 8;
      }
      shades = read_u8(stream);
      shades |= (u32)read_u8(stream) << 8;
      shades |= (u32)read_u8(stream) << 16;
      texture_s = ((appearance >> 8) & 3) << 8;
      texture_t = ((appearance >> 10) & 3) << 8;
      bright_shades = (appearance >> 12) & 1;
      tmp.vertex_list[0].v.tc[0] = texture_s + 0xe0;
      tmp.vertex_list[0].v.tc[1] = texture_t + 0xe0;
      tmp.vertex_list[1].v.tc[0] = texture_s + 0xe0;
      tmp.vertex_list[1].v.tc[1] = texture_t;
      tmp.vertex_list[2].v.tc[0] = texture_s;
      tmp.vertex_list[2].v.tc[1] = texture_t;
      tmp.vertex_list[3].v.tc[0] = texture_s;
      tmp.vertex_list[3].v.tc[1] = texture_t + 0xe0;
      for (vertex = 0; vertex < 4; vertex++) {
        u8 shade = (shades >> (vertex * 6)) & 0x3f;

        if (bright_shades) {
          shade |= 0xc0;
        }
        tmp.vertex_list[vertex].v.cn[0] = shade;
        tmp.vertex_list[vertex].v.cn[1] = shade;
        tmp.vertex_list[vertex].v.cn[2] = shade;
      }
      for (vertex = 0; vertex < 4; vertex++) {
        tmp.vertex_list[vertex].v.cn[3] = 0xdc;
      }
      if (g_FlyingParticlesBuffer != NULL && index < (u32)max_particles) {
        g_FlyingParticlesBuffer[index] = tmp;
      }
    }
  }
}

/*
 * Shattered window/light shards use the older broken-window pool in
 * unk_0A1DA0.c, separate from explosion flying particles. `piece > 0` marks a
 * live shard and doubles as its age.
 */
static void save_shattered_window_pieces_state(StateStream *stream) {
  s32 i;
  u16 count = 0;

  write_u32(stream, g_NextShardNum);

  if (ptr_shattered_window_pieces != NULL) {
    for (i = 0; i < SHATTERED_WINDOW_PIECES_BUFFER_LEN; i++) {
      if (ptr_shattered_window_pieces[i].piece > 0 &&
          ptr_shattered_window_pieces[i].piece < 0x96) {
        count++;
      }
    }
  }

  write_u16(stream, count);
  if (ptr_shattered_window_pieces != NULL) {
    for (i = 0; i < SHATTERED_WINDOW_PIECES_BUFFER_LEN; i++) {
      if (ptr_shattered_window_pieces[i].piece > 0 &&
          ptr_shattered_window_pieces[i].piece < 0x96) {
        write_u16(stream, (u16)i);
        write_bytes(stream, &ptr_shattered_window_pieces[i],
                    sizeof(s_shattered_window_piece));
      }
    }
  }
}

static void clear_shattered_window_pieces_state(void) {
  s32 i;

  if (ptr_shattered_window_pieces != NULL) {
    for (i = 0; i < SHATTERED_WINDOW_PIECES_BUFFER_LEN; i++) {
      ptr_shattered_window_pieces[i].piece = 0;
    }
  }
  g_NextShardNum = 0;
}

static void load_shattered_window_pieces_state(StateStream *stream) {
  s32 i;
  s32 saved_next_shard_num;
  u16 saved_count;

  saved_next_shard_num = read_u32(stream);
  if (saved_next_shard_num < 0 ||
      saved_next_shard_num >= SHATTERED_WINDOW_PIECES_BUFFER_LEN) {
    practiceLogError("Saved window-shard cursor is invalid (%d)",
                     saved_next_shard_num);
    assert(FALSE);
    saved_next_shard_num = 0;
  }

  clear_shattered_window_pieces_state();
  g_NextShardNum = saved_next_shard_num;

  saved_count = read_u16(stream);
  if (saved_count > SHATTERED_WINDOW_PIECES_BUFFER_LEN) {
    practiceLogError("Saved window-shard count is invalid (%d)", saved_count);
    assert(FALSE);
  }

  for (i = 0; i < saved_count; i++) {
    s_shattered_window_piece piece;
    u16 index = read_u16(stream);

    read_bytes(stream, &piece, sizeof(s_shattered_window_piece));
    if (ptr_shattered_window_pieces != NULL &&
        saved_count <= SHATTERED_WINDOW_PIECES_BUFFER_LEN &&
        index < SHATTERED_WINDOW_PIECES_BUFFER_LEN) {
      ptr_shattered_window_pieces[index] = piece;
    }
  }
}

/*
 * Shot light fixtures are tracked by background room vertex index, not by
 * props. The live room vertex colours are darkened in place with `>>= 2`.
 * Avoid reloading BG room memory here: a frame can still be using the room's
 * display lists when state is loaded, and the BG loader uses the tail of the
 * existing room allocation as scratch.
 */
static void save_darkened_lights_state(StateStream *stream) {
  s32 i;
  u16 count = 0;

  write_u32(stream, cur_entry_darkened_light_table);

  for (i = 0; i < DARKENED_LIGHT_TABLE_MAX; i++) {
    if (darkened_light_table[i].room_index > 0 &&
        darkened_light_table[i].room_index < g_MaxNumRooms) {
      count++;
    }
  }

  write_u16(stream, count);
  for (i = 0; i < DARKENED_LIGHT_TABLE_MAX; i++) {
    if (darkened_light_table[i].room_index > 0 &&
        darkened_light_table[i].room_index < g_MaxNumRooms) {
      write_u16(stream, (u16)i);
      write_u16(stream, darkened_light_table[i].room_index);
      write_u16(stream, darkened_light_table[i].vtx_index);
    }
  }
}

static bool bg_room_vertices_loaded(s32 room) {
  return room > 0 && room < g_MaxNumRooms &&
         g_BgRoomInfo[room].model_bin_loaded != 0 &&
         g_BgRoomInfo[room].ptr_point_index != NULL;
}

static bool light_vertex_loaded(s32 room, u16 vtx_index) {
  if (!bg_room_vertices_loaded(room)) {
    return FALSE;
  }

  /* Bounds check the saved vertex index before redarken_lights_in_room scans. */
  return ((u32)vtx_index * sizeof(Vtx)) <
         (u32)g_BgRoomInfo[room].usize_point_index_binary;
}

static void undarken_light_vertex(s32 room, u16 vtx_index) {
  Vtx *vertex;

  if (!light_vertex_loaded(room, vtx_index)) {
    return;
  }

  vertex = &g_BgRoomInfo[room].ptr_point_index[vtx_index];
  vertex->v.cn[0] <<= 2;
  vertex->v.cn[1] <<= 2;
  vertex->v.cn[2] <<= 2;
  vertex->v.cn[3] <<= 2;
}

static void load_darkened_lights_state(StateStream *stream) {
  s32 i;
  u16 count;
  u8 rooms_to_reload[MAXROOMCOUNT];

  bzero(rooms_to_reload, sizeof(rooms_to_reload));

  for (i = 0; i < DARKENED_LIGHT_TABLE_MAX; i++) {
    s32 room = darkened_light_table[i].room_index;
    if (room > 0 && room < g_MaxNumRooms) {
      undarken_light_vertex(room, darkened_light_table[i].vtx_index);
      if (light_vertex_loaded(room, darkened_light_table[i].vtx_index)) {
        rooms_to_reload[room] = TRUE;
      }
    }
    darkened_light_table[i].room_index = 0;
    darkened_light_table[i].vtx_index = 0;
  }

  cur_entry_darkened_light_table = read_u32(stream);
  if (cur_entry_darkened_light_table < 0 ||
      cur_entry_darkened_light_table >= DARKENED_LIGHT_TABLE_MAX) {
    practiceLogError("Saved darkened-light cursor is invalid (%d)",
                     cur_entry_darkened_light_table);
    assert(FALSE);
    cur_entry_darkened_light_table = 0;
  }

  count = read_u16(stream);
  if (count > DARKENED_LIGHT_TABLE_MAX) {
    practiceLogError("Saved darkened-light count is invalid (%d)", count);
    assert(FALSE);
    count = DARKENED_LIGHT_TABLE_MAX;
  }

  for (i = 0; i < count; i++) {
    u16 index = read_u16(stream);
    u16 room = read_u16(stream);
    u16 vtx_index = read_u16(stream);

    if (index >= DARKENED_LIGHT_TABLE_MAX || room == 0 ||
        room >= g_MaxNumRooms) {
      practiceLogError("Saved darkened light is invalid (%d, %d, %d)", index,
                       room, vtx_index);
      assert(FALSE);
      continue;
    }

    darkened_light_table[index].room_index = room;
    darkened_light_table[index].vtx_index = vtx_index;
    if (light_vertex_loaded(room, vtx_index)) {
      rooms_to_reload[room] = TRUE;
    }
  }

  for (i = 1; i < g_MaxNumRooms && i < MAXROOMCOUNT; i++) {
    if (rooms_to_reload[i] && bg_room_vertices_loaded(i)) {
      redarken_lights_in_room(i);
    }
  }
}

/* Room residency is a render cache, but collision and visibility queries read
 * its logical flags during gameplay. Rewind those flags without immediately
 * freeing later-timeline allocations that may still be referenced by a render
 * task. bgCheckIfRoomModelNeedsLoad reuses such cached allocations, while the
 * normal room-aging pass frees unrequested ones at its safe point. */
static void save_bg_room_cache_state(StateStream *stream) {
  s32 room;
  u16 portal_count = 0;

  write_u16(stream, (u16)g_MaxNumRooms);
  for (room = 0; room < g_MaxNumRooms; room++) {
    s_room_info *info = &g_BgRoomInfo[room];
    write_u8(stream, info->room_rendered);
    write_u8(stream, info->room_neighbor_to_rendered);
    write_u8(stream, info->model_bin_loaded);
    write_u8(stream, info->portals_to_room_count);
    write_u8(stream, info->room_loaded_mask);
    write_u8(stream, info->field_35);
  }

  while (portal_count < BG_PORTAL_MAX &&
         g_BgPortals[portal_count].offset_portal != NULL) {
    portal_count++;
  }
  if (portal_count == BG_PORTAL_MAX) {
    practiceLogError("BG portal list has no terminator");
    assert(FALSE);
  }
  write_u16(stream, portal_count);
  for (room = 0; room < portal_count; room++) {
    write_u8(stream, g_BgPortals[room].controlbytes1);
    write_u8(stream, g_BgPortals[room].controlbytes2);
  }
}

static void load_bg_room_cache_state(StateStream *stream) {
  u8 room_rendered[MAXROOMCOUNT];
  u8 room_neighbor[MAXROOMCOUNT];
  u8 model_loaded[MAXROOMCOUNT];
  u8 portal_count[MAXROOMCOUNT];
  u8 loaded_mask[MAXROOMCOUNT];
  u8 field_35[MAXROOMCOUNT];
  u16 count = read_u16(stream);
  u16 bg_portal_count;
  s32 room;

  if (count != g_MaxNumRooms || count > MAXROOMCOUNT) {
    practiceLogError("Saved BG room count %d does not match level %d", count,
                     g_MaxNumRooms);
    assert(FALSE);
  }

  for (room = 0; room < count; room++) {
    room_rendered[room] = read_u8(stream);
    room_neighbor[room] = read_u8(stream);
    model_loaded[room] = read_u8(stream);
    portal_count[room] = read_u8(stream);
    loaded_mask[room] = read_u8(stream);
    field_35[room] = read_u8(stream);
  }

  bg_portal_count = read_u16(stream);
  if (bg_portal_count > BG_PORTAL_MAX) {
    practiceLogError("Saved BG portal count is invalid (%d)", bg_portal_count);
    assert(FALSE);
    return;
  }
  for (room = 0; room < bg_portal_count; room++) {
    u8 control1 = read_u8(stream);
    u8 control2 = read_u8(stream);

    if (g_BgPortals[room].offset_portal == NULL) {
      practiceLogError("Saved BG portal count exceeds level portals (%d)",
                       bg_portal_count);
      assert(FALSE);
      return;
    }
    g_BgPortals[room].controlbytes1 = control1;
    g_BgPortals[room].controlbytes2 = control2;
  }

  if (count != g_MaxNumRooms || count > MAXROOMCOUNT) {
    return;
  }

  for (room = 0; room < count; room++) {
    s_room_info *info = &g_BgRoomInfo[room];

    if (model_loaded[room] != 0 && info->model_bin_loaded == 0 &&
        info->ptr_point_index == NULL &&
        info->ptr_expanded_mapping_info == NULL) {
      sub_GAME_7F0B6368(room);
    }
    info->room_rendered = room_rendered[room];
    info->room_neighbor_to_rendered = room_neighbor[room];
    info->model_bin_loaded = model_loaded[room];
    info->portals_to_room_count = portal_count[room];
    info->room_loaded_mask = loaded_mask[room];
    info->field_35 = field_35[room];
  }

  if (bg_portal_count < BG_PORTAL_MAX &&
      g_BgPortals[bg_portal_count].offset_portal != NULL) {
    practiceLogError("Saved BG portal count does not match level (%d)",
                     bg_portal_count);
    assert(FALSE);
  }
}

/*
 * Short-lived gun effects are not props:
 *
 * - dword_CODE_bss_8007A170 contains impact flares/smoke.
 * - On non-EU builds dword_CODE_bss_8007A4E0 contains the larger moving
 *   spark/dust records.
 *
 * Effect image pointers are stored as small stable IDs, never as absolute
 * addresses. Player and NPC tracer beams are held in hand/ChrRecord firing
 * state already serialized by practice_states_bond.c and
 * practice_states_chr.c.
 */
static void save_gun_effects_state(StateStream *stream) {
  s32 i;
  u16 count = 0;

  for (i = 0; i < BULLET_EFFECT_BUFFER_LEN; i++) {
    if (dword_CODE_bss_8007A170[i].unk04 > 0) {
      count++;
    }
  }
  write_u16(stream, count);
  for (i = 0; i < BULLET_EFFECT_BUFFER_LEN; i++) {
    if (dword_CODE_bss_8007A170[i].unk04 > 0) {
      bondstruct_unk_8007A170 effect = dword_CODE_bss_8007A170[i];
      u8 image_id = get_bullet_effect_image_id(effect.unk0C);
      write_u8(stream, i);
      write_u8(stream, image_id);
      effect.unk0C = 0;
      write_bytes(stream, &effect, sizeof(bondstruct_unk_8007A170));
    }
  }

#if !defined(VERSION_EU)
  {
    BulletSparkRecord *sparks = (BulletSparkRecord *)dword_CODE_bss_8007A4E0;

    count = 0;
    for (i = 0; i < BULLET_SPARK_BUFFER_LEN; i++) {
      if (sparks[i].effect.unk04 > 0) {
        count++;
      }
    }
    write_u16(stream, count);
    for (i = 0; i < BULLET_SPARK_BUFFER_LEN; i++) {
      if (sparks[i].effect.unk04 > 0) {
        BulletSparkRecord spark = sparks[i];
        u8 image_id = get_bullet_effect_image_id(spark.effect.unk0C);
        write_u8(stream, i);
        write_u8(stream, image_id);
        spark.effect.unk0C = 0;
        write_bytes(stream, &spark, sizeof(BulletSparkRecord));
      }
    }
  }
#endif
}

static void load_gun_effects_state(StateStream *stream) {
  s32 i;
  u16 count;

  for (i = 0; i < BULLET_EFFECT_BUFFER_LEN; i++) {
    dword_CODE_bss_8007A170[i].unk04 = 0;
  }
  count = read_u16(stream);
  for (i = 0; i < count; i++) {
    bondstruct_unk_8007A170 effect;
    u8 index = read_u8(stream);
    u8 image_id = read_u8(stream);
    read_bytes(stream, &effect, sizeof(bondstruct_unk_8007A170));
    effect.unk0C = get_bullet_effect_image_by_id(image_id);
    if (effect.unk0C == 0) {
      effect.unk04 = 0;
    }
    if (index < BULLET_EFFECT_BUFFER_LEN) {
      dword_CODE_bss_8007A170[index] = effect;
    }
  }

#if !defined(VERSION_EU)
  {
    BulletSparkRecord *sparks = (BulletSparkRecord *)dword_CODE_bss_8007A4E0;

    for (i = 0; i < BULLET_SPARK_BUFFER_LEN; i++) {
      sparks[i].effect.unk04 = 0;
    }
    count = read_u16(stream);
    for (i = 0; i < count; i++) {
      BulletSparkRecord spark;
      u8 index = read_u8(stream);
      u8 image_id = read_u8(stream);
      read_bytes(stream, &spark, sizeof(BulletSparkRecord));
      spark.effect.unk0C = get_bullet_effect_image_by_id(image_id);
      if (spark.effect.unk0C == 0) {
        spark.effect.unk04 = 0;
      }
      if (index < BULLET_SPARK_BUFFER_LEN) {
        sparks[index] = spark;
      }
    }
  }
#endif
}

/*
 * Ejected first-person cartridge casings live in a fixed global pool rather
 * than the prop table. The model headers are level-lifetime allocations loaded
 * by init_ejected_cartridges, but store a stable table index rather than an
 * absolute pointer so a state remains valid if their addresses ever move.
 */
static void save_casings_state(StateStream *stream) {
  s32 i;
  u8 count = 0;

  for (i = 0; i < CASING_BUFFER_LEN; i++) {
    if (g_Casings[i].header != NULL) {
      count++;
    }
  }

  write_u8(stream, count);
  for (i = 0; i < CASING_BUFFER_LEN; i++) {
    CasingRecord casing;

    if (g_Casings[i].header == NULL) {
      continue;
    }

    casing = g_Casings[i];
    write_u8(stream, i);
    write_u8(stream, get_casing_model_id(casing.header));
    casing.header = NULL;
    write_bytes(stream, &casing, sizeof(CasingRecord));
  }
}

static void load_casings_state(StateStream *stream) {
  s32 i;
  u8 count;

  bzero(g_Casings, sizeof(g_Casings));
  count = read_u8(stream);
  if (count > CASING_BUFFER_LEN) {
    practiceLogError("Saved casing count is invalid (%d)", count);
    assert(FALSE);
    return;
  }

  for (i = 0; i < count; i++) {
    CasingRecord casing;
    u8 index = read_u8(stream);
    u8 model_id = read_u8(stream);

    read_bytes(stream, &casing, sizeof(CasingRecord));
    casing.header = get_casing_model_by_id(model_id);

    if (index >= CASING_BUFFER_LEN) {
      practiceLogError("Saved casing has invalid pool index %d", index);
      assert(FALSE);
      continue;
    }
    g_Casings[index] = casing;
  }

  // The casing impact sound graph was stopped before loading. Do not retain
  // its now-defunct ALSoundState handle in this global slot.
  dword_CODE_bss_80075DB0 = 0;
}

static void load_object_subtype(StateStream *stream, ObjectRecord *obj) {
  switch (obj->type) {
  case PROPDEF_PROP:
  case PROPDEF_ALARM:
  case PROPDEF_RACK:
  case PROPDEF_HAT:
  case PROPDEF_UNK41:
  case PROPDEF_GAS_RELEASING:
    break;
  case PROPDEF_KEY: {
    KeyRecord *key = (KeyRecord *)obj;
    key->keyflags = read_u32(stream);
    break;
  }
  case PROPDEF_CCTV: {
    CCTVRecord *cctv = (CCTVRecord *)obj;
    cctv->cctv_pad = read_u32(stream);
    read_bytes(stream, &cctv->unk84, sizeof(Mtxf));
    cctv->unkC4 = read_f32(stream);
    cctv->unkC8 = read_f32(stream);
    cctv->unkCC = read_f32(stream);
    cctv->unkD0 = read_f32(stream);
    cctv->unkD4 = read_u32(stream);
    cctv->unkD8 = read_f32(stream);
    cctv->unkDC = read_f32(stream);
    cctv->timer = read_u32(stream);
    cctv->convert_to_f32 = read_u32(stream);
    cctv->unkE8 = read_f32(stream);
    cctv->unkEC = read_u32(stream);
    cctv->unkF0 = read_u32(stream);
    cctv->unkF4 = read_u32(stream);
    cctv->unkF8 = read_u32(stream);
    break;
  }
  case PROPDEF_MAGAZINE: {
    AmmoCrateRecord *mag = (AmmoCrateRecord *)obj;
    mag->ammoType = read_u32(stream);
    break;
  }
  case PROPDEF_COLLECTABLE: {
    WeaponObjRecord *wpn = (WeaponObjRecord *)obj;
    PropRecord *dualProp;
    wpn->weaponnum = read_u8(stream);
    wpn->LinkedWeaponType = read_u8(stream);
    wpn->timer = read_u16(stream);
    s16 dualIdx = read_u16(stream);
    dualProp = get_enabled_prop_by_index(dualIdx);
    wpn->dualweapon = dualProp != NULL &&
                              (dualProp->type == PROP_TYPE_OBJ ||
                               dualProp->type == PROP_TYPE_WEAPON) &&
                              dualProp->obj != NULL &&
                              dualProp->obj->type == PROPDEF_COLLECTABLE
                          ? dualProp->weapon
                          : NULL;
    break;
  }
  case PROPDEF_MONITOR: {
    MonitorObjRecord *mon = (MonitorObjRecord *)obj;
    load_monitor_record(stream, &mon->Monitor);
    mon->OwnerOffset = read_u32(stream);
    mon->OwnerPart = read_u32(stream);
    mon->ImageNum = read_u32(stream);
    break;
  }
  case PROPDEF_MULTI_MONITOR: {
    MultiMonitorObjRecord *mmon = (MultiMonitorObjRecord *)obj;
    s32 m;
    for (m = 0; m < 4; m++) {
      load_monitor_record(stream, &mmon->Monitor[m]);
    }
    mmon->ImageNums[0] = read_u8(stream);
    mmon->ImageNums[1] = read_u8(stream);
    mmon->ImageNums[2] = read_u8(stream);
    mmon->ImageNums[3] = read_u8(stream);
    break;
  }
  case PROPDEF_AUTOGUN: {
    AutogunRecord *agun = (AutogunRecord *)obj;
    agun->padID = read_u32(stream);
    agun->rot_related = read_f32(stream);
    agun->unk88 = read_f32(stream);
    agun->unk8C = read_f32(stream);
    agun->unk90 = read_f32(stream);
    agun->unk94 = read_f32(stream);
    agun->unk98 = read_f32(stream);
    agun->unk9C = read_f32(stream);
    agun->unkA0 = read_f32(stream);
    agun->speed = read_f32(stream);
    agun->aimdist = read_f32(stream);
    agun->unkAC = read_u32(stream);
    agun->unkB0 = read_f32(stream);
    agun->unkB4 = read_f32(stream);
    agun->unkB8 = read_u32(stream);
    agun->unkBC = read_u32(stream);
    agun->unkC0 = read_u32(stream);

    struct beam b;
    read_bytes(stream, &b, sizeof(struct beam));
    agun->unkC4 = NULL;
    agun->unkC8 = NULL;
    if (agun->beam != NULL) {
      *agun->beam = b;
    }

    agun->is_active = read_u32(stream);
    agun->unkD4 = read_f32(stream);
    break;
  }
  case PROPDEF_AMMO: {
    MultiAmmoCrateRecord *ammo = (MultiAmmoCrateRecord *)obj;
    ammo->unk80 = read_u16(stream);
    s32 a;
    for (a = 0; a < AMMOTYPE_GLOBAL_MAX; a++) {
      ammo->quantities[a] = read_u16(stream);
    }
    break;
  }
  case PROPDEF_ARMOUR: {
    BodyArmourRecord *arm = (BodyArmourRecord *)obj;
    arm->initialamount = read_f32(stream);
    arm->amount = read_f32(stream);
    break;
  }
  case PROPDEF_VEHICHLE: {
    VehichleRecord *veh = (VehichleRecord *)obj;
    u32 ailistID = read_u32(stream);
    veh->ailist = (ailistID != 0) ? ailistFindById(ailistID) : NULL;
    veh->aioffset = read_u16(stream);
    veh->aireturnlist = read_u16(stream);
    veh->speed = read_f32(stream);
    veh->wheelxrot = read_f32(stream);
    veh->wheelyrot = read_f32(stream);
    veh->speedaim = read_f32(stream);
    veh->speedtime60 = read_f32(stream);
    veh->turnrot60 = read_f32(stream);
    veh->roty = read_f32(stream);
    u32 pathID = read_u32(stream);
    veh->path = (pathID != 0) ? pathFindById(pathID) : NULL;
    veh->nextstep = read_u32(stream);
    veh->Sound = NULL;
    break;
  }
  case PROPDEF_AIRCRAFT: {
    AircraftRecord *air = (AircraftRecord *)obj;
    u32 ailistID = read_u32(stream);
    air->ailist = (ailistID != 0) ? ailistFindById(ailistID) : NULL;
    air->aioffset = read_u16(stream);
    air->aireturnlist = read_u16(stream);
    air->rotoryrot = read_f32(stream);
    air->rotaryspeed = read_f32(stream);
    air->rotaryspeedaim = read_f32(stream);
    air->rotaryspeedtime = read_f32(stream);
    air->speed = read_f32(stream);
    air->speedaim = read_f32(stream);
    air->speedtime60 = read_f32(stream);
    air->yrot = read_f32(stream);
    air->nextstep = read_u32(stream);
    u32 pathID = read_u32(stream);
    air->path = (pathID != 0) ? pathFindById(pathID) : NULL;
    air->Sound = NULL;
    load_object_model_animation(stream, air->model);
    break;
  }
  case PROPDEF_GLASS:
  case PROPDEF_SAFE: {
    break;
  }
  case PROPDEF_TINTED_GLASS: {
    TintedGlassRecord *tgl = (TintedGlassRecord *)obj;
    tgl->TintDist = read_u32(stream);
    tgl->CullDist = read_u32(stream);
    tgl->calculatedopacity = read_u32(stream);
    tgl->portalnum = read_u32(stream);
    tgl->unk90 = read_f32(stream);
    load_portal_closed_state(tgl->portalnum, read_u8(stream));
    break;
  }
  case PROPDEF_TANK: {
    TankRecord *tank = (TankRecord *)obj;
    read_bytes(stream, &tank->rect, sizeof(rect4f));
    tank->unkA4 = read_u32(stream);
    tank->unkA8 = read_u32(stream);
    tank->unkAC = read_u32(stream);
    tank->unkB0 = read_u32(stream);
    tank->unkB4 = read_u32(stream);
    tank->unkB8 = read_u32(stream);
    tank->unkBC = read_u32(stream);
    tank->unkC0 = read_u32(stream);
    tank->is_firing_tank = read_u32(stream);
    tank->turret_vertical_angle = read_f32(stream);
    tank->turret_orientation_angle = read_f32(stream);
    tank->unkD0 = read_f32(stream);
    tank->stan_y = read_f32(stream);
    tank->unkD8 = read_u32(stream);
    tank->tank_orientation_angle = read_f32(stream);
    break;
  }
  default:
    break;
  }
}

static void skip_prop_data(StateStream *stream, u8 type,
                           bool compactContainedObject) {
  if (type == PROP_TYPE_DOOR) {
    ObjectRecord temp_obj;
    DoorRecord temp_door;
    bzero(&temp_obj, sizeof(temp_obj));
    bzero(&temp_door, sizeof(temp_door));
    load_object_base(stream, &temp_obj, NULL, NULL);
    load_door_record(stream, &temp_door);
  } else if (type == PROP_TYPE_OBJ || type == PROP_TYPE_WEAPON) {
    TempObjectRecord temp_obj;
    bzero(&temp_obj, sizeof(temp_obj));
    if (compactContainedObject) {
      load_compact_contained_object_base(stream, &temp_obj.base, NULL);
      load_object_subtype(stream, &temp_obj.base);
    } else if (load_object_base(stream, &temp_obj.base, NULL, NULL)) {
      if (type == PROP_TYPE_WEAPON) {
        load_equipped_weapon_matrices(stream, NULL);
      }
      load_object_subtype(stream, &temp_obj.base);
    }
  } else if (type == PROP_TYPE_EXPLOSION) {
    struct Explosion temp_explosion;
    bzero(&temp_explosion, sizeof(temp_explosion));
    load_explosion_record(stream, &temp_explosion);
  } else if (type == PROP_TYPE_SMOKE) {
    struct Smoke temp_smoke;
    bzero(&temp_smoke, sizeof(temp_smoke));
    load_smoke_record(stream, &temp_smoke);
  } else if (type == PROP_TYPE_CHR) {
    ChrRecord temp_chr;
    bzero(&temp_chr, sizeof(temp_chr));
    load_chr_record(stream, &temp_chr, NULL);
  }
}

bool save_props_state(StateStream *stream) {
  u32 headerOffset;
  u32 dataStart;
  u16 recordCount = 0;
  s32 i;
  s32 previousPropIndex = -1;
  u32 pi;

  headerOffset = stream->base_address + stream->total_processed;

  // Placeholder header fields
  write_u32(stream, 0); // size
  write_u16(stream, 0); // recordCount
  write_u16(stream, get_prop_index(ptr_obj_pos_list_first_entry));
  write_u16(stream, get_prop_index(ptr_obj_pos_list_current_entry));
  write_u16(stream, get_prop_index(ptr_obj_pos_list_final_entry));

  dataStart = stream->base_address + stream->total_processed;

  write_u32(stream, g_NumExplosionEntries);
  write_u32(stream, g_NumSmokeEntries);

  /* Save active projectiles. */
  {
    u16 activeProjCount = 0;
    for (pi = 0; pi < PROJECTILES_ARR_MAX; pi++) {
      if (!(g_Projectiles[pi].flags & PROJECTILEFLAG_FREE)) {
        activeProjCount++;
      }
    }
    write_u16(stream, activeProjCount);
    for (pi = 0; pi < PROJECTILES_ARR_MAX; pi++) {
      if (!(g_Projectiles[pi].flags & PROJECTILEFLAG_FREE)) {
        write_u16(stream, pi);
        save_projectile(stream, &g_Projectiles[pi]);
      }
    }
  }

  /* Save active embedments. */
  {
    u16 activeEmbCount = 0;
    for (pi = 0; pi < EMBEDMENT_ARR_MAX; pi++) {
      if (!(g_Embedments[pi].flags & EMBEDMENTFLAG_FREE)) {
        activeEmbCount++;
      }
    }
    write_u16(stream, activeEmbCount);
    for (pi = 0; pi < EMBEDMENT_ARR_MAX; pi++) {
      if (!(g_Embedments[pi].flags & EMBEDMENTFLAG_FREE)) {
        write_u16(stream, pi);
        save_embedment(stream, &g_Embedments[pi]);
      }
    }
  }

  for (i = 0; i < POS_DATA_ENTRY_LEN; i++) {
    PropRecord *prop = get_prop_by_index(i);
    bool compactContainedObject;

    if (prop == NULL ||
        (!prop_is_active_list_member(prop) &&
         !prop_is_saved_child_object(prop) && !prop_is_inventory_item(prop))) {
      continue;
    }

    compactContainedObject =
        prop_uses_compact_contained_object_state(prop);
    // Write base prop record fields
    {
      u32 delta = i - previousPropIndex;
      if (delta < 0xff) {
        write_u8(stream, delta);
      } else {
        write_u8(stream, 0xff);
        write_u16(stream, delta);
      }
      previousPropIndex = i;
    }
    write_u8(stream,
             prop->type | (compactContainedObject ? 0x80 : 0));
    write_u8(stream, prop->flags);
    write_u16(stream, prop->timetoregen);
    if (!compactContainedObject) {
      write_bytes(stream, &prop->pos, sizeof(coord3d));
      write_u32(stream, get_tile_offset(prop->stan));
      write_f32(stream, prop->zDepth);
    }
    save_prop_links(stream, prop);
    if (!compactContainedObject) {
      u8 roomCount = 0;
      while (roomCount < 4 && prop->rooms[roomCount] != 0xff) {
        roomCount++;
      }
      write_u8(stream, roomCount);
      write_bytes(stream, prop->rooms, roomCount);
      write_u32(stream, prop->unk30);
    }
    if (prop->type == PROP_TYPE_EXPLOSION) {
      if (prop->explosion < g_ExplosionBuffer ||
          prop->explosion >= g_ExplosionBuffer + EXPLOSION_BUFFER_LEN) {
        practiceLogError("Explosion prop %d has invalid buffer entry", i);
        return FALSE;
      }
      write_u8(stream, prop->explosion - g_ExplosionBuffer);
    } else if (prop->type == PROP_TYPE_SMOKE) {
      if (prop->smoke < g_SmokeBuffer ||
          prop->smoke >= g_SmokeBuffer + SMOKE_BUFFER_LEN) {
        practiceLogError("Smoke prop %d has invalid buffer entry", i);
        return FALSE;
      }
      write_u8(stream, prop->smoke - g_SmokeBuffer);
    }

    switch ((PROP_TYPE)prop->type) {
    case PROP_TYPE_DOOR: {
      DoorRecord *door = prop->door;
      save_obj_allocation_state(stream, prop);
      if (door != NULL) {
        save_object_base(stream, (ObjectRecord *)door);

        write_f32(stream, door->maxFrac);
        write_f32(stream, door->perimFrac);
        write_f32(stream, door->accel);
        write_f32(stream, door->decel);
        write_f32(stream, door->maxSpeed);
        write_u16(stream, door->doorFlags);
        write_u16(stream, door->doorType);
        write_u32(stream, door->keyflags);
        write_u32(stream, door->autoCloseFrames);
        write_u32(stream, door->doorOpenSound);
        write_f32(stream, door->frac);
        write_f32(stream, door->unkac);
        write_f32(stream, door->unkb0);
        write_f32(stream, door->openPosition);
        write_f32(stream, door->speed);
        write_u8(stream, door->openstate);
        write_u8(stream, door->unkbd);
        write_u16(stream, door->calculatedopacity);
        write_u32(stream, door->TintDist);
        write_u16(stream, door->CullDist);
        write_u8(stream, door->soundType);
        write_u8(stream, door->fadeTime60);
        write_bytes(stream, &door->bbox,
                    sizeof(struct ModelRoData_BoundingBoxRecord));
        write_u32(stream, door->openedTime);
        write_u32(stream, door->portalNumber);
        write_u32(stream, door->lastcalc60i);
      }
      break;
    }

    case PROP_TYPE_OBJ:
    case PROP_TYPE_WEAPON: {
      ObjectRecord *obj = prop->obj;
      save_obj_allocation_state(stream, prop);
      if (obj != NULL) {
        if (compactContainedObject) {
          save_compact_contained_object_base(stream, obj);
        } else {
          save_object_base(stream, obj);
        }
        if (prop->type == PROP_TYPE_WEAPON && !compactContainedObject) {
          save_equipped_weapon_matrices(stream, obj);
        }

        switch (obj->type) {
        case PROPDEF_PROP:
        case PROPDEF_ALARM:
        case PROPDEF_RACK:
        case PROPDEF_HAT:
        case PROPDEF_UNK41:
        case PROPDEF_GAS_RELEASING:
          break;
        case PROPDEF_KEY: {
          KeyRecord *key = (KeyRecord *)obj;
          write_u32(stream, key->keyflags);
          break;
        }
        case PROPDEF_CCTV: {
          CCTVRecord *cctv = (CCTVRecord *)obj;
          write_u32(stream, cctv->cctv_pad);
          write_bytes(stream, &cctv->unk84, sizeof(Mtxf));
          write_f32(stream, cctv->unkC4);
          write_f32(stream, cctv->unkC8);
          write_f32(stream, cctv->unkCC);
          write_f32(stream, cctv->unkD0);
          write_u32(stream, cctv->unkD4);
          write_f32(stream, cctv->unkD8);
          write_f32(stream, cctv->unkDC);
          write_u32(stream, cctv->timer);
          write_u32(stream, cctv->convert_to_f32);
          write_f32(stream, cctv->unkE8);
          write_u32(stream, cctv->unkEC);
          write_u32(stream, cctv->unkF0);
          write_u32(stream, cctv->unkF4);
          write_u32(stream, cctv->unkF8);
          break;
        }
        case PROPDEF_MAGAZINE: {
          AmmoCrateRecord *mag = (AmmoCrateRecord *)obj;
          write_u32(stream, mag->ammoType);
          break;
        }
        case PROPDEF_COLLECTABLE: {
          WeaponObjRecord *wpn = (WeaponObjRecord *)obj;
          write_u8(stream, wpn->weaponnum);
          write_u8(stream, wpn->LinkedWeaponType);
          write_u16(stream, wpn->timer);
          s16 dualIdx = (wpn->dualweapon && wpn->dualweapon->prop)
                            ? (s16)get_prop_index(wpn->dualweapon->prop)
                            : -1;
          write_u16(stream, dualIdx);
          break;
        }
        case PROPDEF_MONITOR: {
          MonitorObjRecord *mon = (MonitorObjRecord *)obj;
          save_monitor_record(stream, &mon->Monitor);
          write_u32(stream, mon->OwnerOffset);
          write_u32(stream, mon->OwnerPart);
          write_u32(stream, mon->ImageNum);
          break;
        }
        case PROPDEF_MULTI_MONITOR: {
          MultiMonitorObjRecord *mmon = (MultiMonitorObjRecord *)obj;
          s32 m;
          for (m = 0; m < 4; m++) {
            save_monitor_record(stream, &mmon->Monitor[m]);
          }
          write_u8(stream, mmon->ImageNums[0]);
          write_u8(stream, mmon->ImageNums[1]);
          write_u8(stream, mmon->ImageNums[2]);
          write_u8(stream, mmon->ImageNums[3]);
          break;
        }
        case PROPDEF_AUTOGUN: {
          AutogunRecord *agun = (AutogunRecord *)obj;
          write_u32(stream, agun->padID);
          write_f32(stream, agun->rot_related);
          write_f32(stream, agun->unk88);
          write_f32(stream, agun->unk8C);
          write_f32(stream, agun->unk90);
          write_f32(stream, agun->unk94);
          write_f32(stream, agun->unk98);
          write_f32(stream, agun->unk9C);
          write_f32(stream, agun->unkA0);
          write_f32(stream, agun->speed);
          write_f32(stream, agun->aimdist);
          write_u32(stream, agun->unkAC);
          write_f32(stream, agun->unkB0);
          write_f32(stream, agun->unkB4);
          write_u32(stream, agun->unkB8);
          write_u32(stream, agun->unkBC);
          write_u32(stream, agun->unkC0);

          struct beam b;
          if (agun->beam != NULL) {
            b = *agun->beam;
          } else {
            b.age = -1;
            b.weaponnum = 0;
            b.from.x = 0.0f;
            b.from.y = 0.0f;
            b.from.z = 0.0f;
            b.dir.x = 0.0f;
            b.dir.y = 0.0f;
            b.dir.z = 0.0f;
            b.maxdist = 0.0f;
            b.speed = 0.0f;
            b.mindist = 0.0f;
            b.dist = 0.0f;
          }
          write_bytes(stream, &b, sizeof(struct beam));

          write_u32(stream, agun->is_active);
          write_f32(stream, agun->unkD4);
          break;
        }
        case PROPDEF_AMMO: {
          MultiAmmoCrateRecord *ammo = (MultiAmmoCrateRecord *)obj;
          write_u16(stream, ammo->unk80);
          s32 a;
          for (a = 0; a < AMMOTYPE_GLOBAL_MAX; a++) {
            write_u16(stream, ammo->quantities[a]);
          }
          break;
        }
        case PROPDEF_ARMOUR: {
          BodyArmourRecord *arm = (BodyArmourRecord *)obj;
          write_f32(stream, arm->initialamount);
          write_f32(stream, arm->amount);
          break;
        }
        case PROPDEF_VEHICHLE: {
          VehichleRecord *veh = (VehichleRecord *)obj;
          bool isGlobal;
          write_u32(stream,
                    veh->ailist ? chraiGetAIListID(veh->ailist, &isGlobal) : 0);
          write_u16(stream, veh->aioffset);
          write_u16(stream, veh->aireturnlist);
          write_f32(stream, veh->speed);
          write_f32(stream, veh->wheelxrot);
          write_f32(stream, veh->wheelyrot);
          write_f32(stream, veh->speedaim);
          write_f32(stream, veh->speedtime60);
          write_f32(stream, veh->turnrot60);
          write_f32(stream, veh->roty);
          write_u32(stream, veh->path ? veh->path->ID : 0);
          write_u32(stream, veh->nextstep);
          break;
        }
        case PROPDEF_AIRCRAFT: {
          AircraftRecord *air = (AircraftRecord *)obj;
          bool isGlobal;
          write_u32(stream,
                    air->ailist ? chraiGetAIListID(air->ailist, &isGlobal) : 0);
          write_u16(stream, air->aioffset);
          write_u16(stream, air->aireturnlist);
          write_f32(stream, air->rotoryrot);
          write_f32(stream, air->rotaryspeed);
          write_f32(stream, air->rotaryspeedaim);
          write_f32(stream, air->rotaryspeedtime);
          write_f32(stream, air->speed);
          write_f32(stream, air->speedaim);
          write_f32(stream, air->speedtime60);
          write_f32(stream, air->yrot);
          write_u32(stream, air->nextstep);
          write_u32(stream, air->path ? air->path->ID : 0);
          save_object_model_animation(stream, air->model);
          break;
        }
        case PROPDEF_GLASS:
        case PROPDEF_SAFE: {
          break;
        }
        case PROPDEF_TINTED_GLASS: {
          TintedGlassRecord *tgl = (TintedGlassRecord *)obj;
          write_u32(stream, tgl->TintDist);
          write_u32(stream, tgl->CullDist);
          write_u32(stream, tgl->calculatedopacity);
          write_u32(stream, tgl->portalnum);
          write_f32(stream, tgl->unk90);
          write_u8(stream, save_portal_closed_state(tgl->portalnum));
          break;
        }
        case PROPDEF_TANK: {
          TankRecord *tank = (TankRecord *)obj;
          write_bytes(stream, &tank->rect, sizeof(rect4f));
          write_u32(stream, tank->unkA4);
          write_u32(stream, tank->unkA8);
          write_u32(stream, tank->unkAC);
          write_u32(stream, tank->unkB0);
          write_u32(stream, tank->unkB4);
          write_u32(stream, tank->unkB8);
          write_u32(stream, tank->unkBC);
          write_u32(stream, tank->unkC0);
          write_u32(stream, tank->is_firing_tank);
          write_f32(stream, tank->turret_vertical_angle);
          write_f32(stream, tank->turret_orientation_angle);
          write_f32(stream, tank->unkD0);
          write_f32(stream, tank->stan_y);
          write_u32(stream, tank->unkD8);
          write_f32(stream, tank->tank_orientation_angle);
          break;
        }
        default:
          break;
        }
      }
      break;
    }

    case PROP_TYPE_SMOKE:
      if (prop->smoke != NULL) {
        save_smoke_record(stream, prop->smoke);
      }
      break;

    case PROP_TYPE_EXPLOSION:
      if (prop->explosion != NULL) {
        save_explosion_record(stream, prop->explosion);
      }
      break;

    case PROP_TYPE_NUL:
      practiceLogDebug(
          "INVARIANT VIOLATION: Enabled PROP_TYPE_NUL at prop index %d", i);
      break;
    case PROP_TYPE_CHR:
      if (prop->chr == NULL) {
        practiceLogWarn("Enabled CHR prop at index %d has no ChrRecord", i);
        return FALSE;
      }
      save_chr_record(stream, prop->chr);
      break;
    case PROP_TYPE_PLAYER:
      practiceLogDebug(
          "INVARIANT VIOLATION: Enabled PROP_TYPE_PLAYER at prop index %d", i);
      break;
    case PROP_TYPE_VIEWER:
    default:
      break;
    }

    recordCount++;
  }

  // Player payloads follow all prop records so they can safely refer to any
  // restored prop, regardless of prop array order.
  if (!save_viewer_players_state(stream)) {
    return FALSE;
  }

  // Scorch marks and bullet holes reference props by index, so they are saved
  // after every prop record (and the players) has been written.
  save_decals_state(stream);
  save_free_effect_entries(stream);
  save_flying_particles_state(stream);
  save_gun_effects_state(stream);
  save_casings_state(stream);
  save_darkened_lights_state(stream);
  save_bg_room_cache_state(stream);
  save_shattered_window_pieces_state(stream);
  if (!save_prop_free_list(stream)) {
    return FALSE;
  }
  save_room_prop_lists(stream);
  save_onscreen_prop_list(stream);

  /* Patch the props header with the real size and record count. */
  u32 totalPropsSize =
      stream->total_processed - (dataStart - stream->base_address);
  stream_seek(stream, headerOffset);
  write_u32(stream, totalPropsSize);
  write_u16(stream, recordCount);
  write_u16(stream, get_prop_index(ptr_obj_pos_list_first_entry));
  write_u16(stream, get_prop_index(ptr_obj_pos_list_current_entry));
  write_u16(stream, get_prop_index(ptr_obj_pos_list_final_entry));

  // Resume stream offset at the end
  stream_seek(stream, dataStart + totalPropsSize);

  return TRUE;
}

typedef struct PendingChrAttachments {
  u16 prop_index;
  ChrAttachmentIndices attachments;
} PendingChrAttachments;

static void cleanup_live_chr_children(PropRecord *chr_prop,
                                      const SavedPropLinks *savedLinks,
                                      s32 recordCount) {
  PropRecord *child;
  u16 chrIndex;

  if (chr_prop == NULL || chr_prop->type != PROP_TYPE_CHR ||
      chr_prop->chr == NULL) {
    return;
  }

  chrIndex = get_prop_index(chr_prop);
  child = chr_prop->child;

  while (child != NULL) {
    PropRecord *next = child->prev;
    u16 childIndex = get_prop_index(child);

    if (saved_links_name_chr_child(savedLinks, recordCount, childIndex,
                                   chrIndex)) {
      chrpropDetach(child);
    } else if ((child->type == PROP_TYPE_OBJ ||
                child->type == PROP_TYPE_WEAPON) &&
               child->obj != NULL) {
      objFreePermanently(child->obj, TRUE);
    } else {
      chrpropDetach(child);
    }

    child = next;
  }

  chr_prop->child = NULL;
}

static bool saved_link_names_child(const SavedPropLinks *savedLinks,
                                   s32 recordCount, u16 childIndex,
                                   u16 parentIndex) {
  s32 i;

  for (i = 0; i < recordCount; i++) {
    if (savedLinks[i].index == childIndex &&
        savedLinks[i].parent == parentIndex) {
      return TRUE;
    }
  }

  return FALSE;
}

static PropRecord *get_saved_child_link(const SavedPropLinks *savedLinks,
                                        s32 recordCount, u16 childIndex,
                                        u16 parentIndex) {
  if ((s16)childIndex < 0 || !saved_link_names_child(savedLinks, recordCount,
                                                     childIndex, parentIndex)) {
    return NULL;
  }

  return get_prop_by_index(childIndex);
}

static bool prop_can_own_saved_children(PropRecord *prop) {
  if (prop == NULL) {
    return FALSE;
  }
  if (prop->type == PROP_TYPE_CHR) {
    return prop->chr != NULL && prop->chr->prop == prop;
  }
  if (prop->type == PROP_TYPE_PLAYER || prop->type == PROP_TYPE_VIEWER) {
    return TRUE;
  }
  if (prop->type == PROP_TYPE_OBJ || prop->type == PROP_TYPE_DOOR ||
      prop->type == PROP_TYPE_WEAPON) {
    return prop->obj != NULL && prop->obj->prop == prop;
  }
  return FALSE;
}

static void rebuild_saved_child_links(const SavedPropLinks *savedLinks,
                                      s32 recordCount) {
  s32 i;

  for (i = 0; i < recordCount; i++) {
    PropRecord *parent;

    if ((s16)savedLinks[i].parent >= 0) {
      continue;
    }

    parent = get_prop_by_index(savedLinks[i].index);
    if (prop_can_own_saved_children(parent)) {
      parent->child = get_saved_child_link(
          savedLinks, recordCount, savedLinks[i].child, savedLinks[i].index);
    }
  }

  for (i = 0; i < recordCount; i++) {
    PropRecord *prop;
    PropRecord *parent;

    if ((s16)savedLinks[i].parent < 0) {
      continue;
    }

    prop = get_prop_by_index(savedLinks[i].index);
    parent = get_prop_by_index(savedLinks[i].parent);
    if (prop == NULL || !prop_can_own_saved_children(parent) ||
        prop->parent != parent) {
      continue;
    }

    prop->prev = get_saved_child_link(savedLinks, recordCount,
                                      savedLinks[i].prev, savedLinks[i].parent);
    prop->next = get_saved_child_link(savedLinks, recordCount,
                                      savedLinks[i].next, savedLinks[i].parent);
  }
}

bool load_props_state(StateStream *stream) {
  u32 dataStart;
  s32 i;
  u16 nextIndexToRemove = 0;
  s32 c;
  u32 pi;
  s16 projectileOwnerPropIndices[PROJECTILES_ARR_MAX];
  s16 projectileObjPropIndices[PROJECTILES_ARR_MAX];
  s32 previousSavedPropIndex = -1;
  u32 totalPropsSize = read_u32(stream);
  u16 recordCount = read_u16(stream);
  s16 indexOfFirstEntry = read_u16(stream);
  s16 indexOfCurrentEntry = read_u16(stream);
  s16 indexOfFinalEntry = read_u16(stream);
  s32 pendingChrCount = 0;
  s32 pendingChrCapacity = g_NumChrSlots > 0 ? g_NumChrSlots : 1;

  if (recordCount > POS_DATA_ENTRY_LEN) {
    practiceLogWarn("Invalid prop record count %d", recordCount);
    return FALSE;
  }
  if (pendingChrCapacity > POS_DATA_ENTRY_LEN) {
    pendingChrCapacity = POS_DATA_ENTRY_LEN;
  }

  SavedPropLinks savedLinks[recordCount > 0 ? recordCount : 1];
  PendingChrAttachments
      pendingChrAttachments[pendingChrCapacity > 0 ? pendingChrCapacity : 1];

  dataStart = stream->base_address + stream->total_processed;

  /* Release every live blood clone before recreating or loading any CHR.
   * Loading one CHR at a time can otherwise temporarily require both the
   * post-save and saved allocations, exhausting the shared vertex pool. */
  for (i = 0; i < POS_DATA_ENTRY_LEN; i++) {
    PropRecord *live_prop = get_prop_by_index(i);

    if (live_prop != NULL && live_prop->type == PROP_TYPE_CHR &&
        live_prop->chr != NULL && live_prop->chr->prop == live_prop) {
      clear_chr_model_blood_patches(live_prop->chr);
    }
  }
  /* The 0xCCCC vertex pool is dedicated to CHR blood patches. Reinitialize
   * its allocation metadata after releasing every live patch so repeated
   * state loads cannot accumulate fragmentation or duplicated allocations. */
  reset_chr_blood_vertex_pool();
  g_NumExplosionEntries = read_u32(stream);
  g_NumSmokeEntries = read_u32(stream);

  // The current room registration tables may belong to a different lifecycle
  // moment (for example, a level-end cutscene). Rebuild from the restored prop
  // records instead of walking stale chunk links during replacement.
  reset_room_prop_lists();

  /* Free the old projectile/embedment pools before installing the saved
   * entries. Prop teardown later in this function must not be allowed to free
   * those newly restored entries through a stale live ObjectRecord pointer. */
  for (pi = 0; pi < PROJECTILES_ARR_MAX; pi++) {
    projectileOwnerPropIndices[pi] = -1;
    projectileObjPropIndices[pi] = -1;
    if (!(g_Projectiles[pi].flags & PROJECTILEFLAG_FREE)) {
      projectileFree(&g_Projectiles[pi]);
    }
  }

  for (pi = 0; pi < EMBEDMENT_ARR_MAX; pi++) {
    if (!(g_Embedments[pi].flags & EMBEDMENTFLAG_FREE)) {
      embedmentFree(&g_Embedments[pi]);
    }
  }

  for (i = 0; i < POS_DATA_ENTRY_LEN; i++) {
    PropRecord *live_prop = get_prop_by_index(i);

    if (live_prop != NULL &&
        (live_prop->type == PROP_TYPE_OBJ ||
         live_prop->type == PROP_TYPE_DOOR ||
         live_prop->type == PROP_TYPE_WEAPON) &&
        live_prop->obj != NULL &&
        (live_prop->obj->runtime_bitflags &
         (RUNTIMEBITFLAG_DEPOSIT | RUNTIMEBITFLAG_EMBEDDED))) {
      live_prop->obj->projectile = NULL;
      live_prop->obj->runtime_bitflags &=
          ~(RUNTIMEBITFLAG_DEPOSIT | RUNTIMEBITFLAG_EMBEDDED);
    }
  }

  /* Restore active projectiles. */
  {
    u16 activeProjCount = read_u16(stream);
    for (pi = 0; pi < activeProjCount; pi++) {
      u16 index = read_u16(stream);
      load_projectile(stream, &g_Projectiles[index],
                      &projectileOwnerPropIndices[index],
                      &projectileObjPropIndices[index]);
    }
  }

  /* Restore active embedments. */
  {
    u16 activeEmbCount = read_u16(stream);
    for (pi = 0; pi < activeEmbCount; pi++) {
      u16 index = read_u16(stream);
      load_embedment(stream, &g_Embedments[index]);
    }
  }

  for (i = 0; i < recordCount; i++) {
    u32 savedPropDelta = read_u8(stream);
    u16 savedPropIndex;
    if (savedPropDelta == 0xff) {
      savedPropDelta = read_u16(stream);
    }
    savedPropIndex = previousSavedPropIndex + savedPropDelta;
    previousSavedPropIndex = savedPropIndex;
    u8 savedPropTypeAndFlags = read_u8(stream);
    bool compactContainedObject = (savedPropTypeAndFlags & 0x80) != 0;
    u8 savedPropType = savedPropTypeAndFlags & 0x7f;
    u8 savedPropFlags = read_u8(stream);
    s16 savedPropTimetoregen = read_u16(stream);
    coord3d savedPropPos = {0};
    s32 savedPropStanOffset = -1;
    f32 savedPropZDepth = 0.0f;
    if (!compactContainedObject) {
      read_bytes(stream, &savedPropPos, sizeof(coord3d));
      savedPropStanOffset = read_u32(stream);
      savedPropZDepth = read_f32(stream);
    }
    u16 savedPropParentIdx;
    u16 savedPropChildIdx;
    u16 savedPropPrevIdx;
    u16 savedPropNextIdx;
    load_prop_links(stream, &savedPropParentIdx, &savedPropChildIdx,
                    &savedPropPrevIdx, &savedPropNextIdx);
    u8 savedPropRooms[4] = {0xff, 0xff, 0xff, 0xff};
    u8 savedPropRoomCount = 0;
    s32 savedPropUnk30 = 0;
    if (!compactContainedObject) {
      savedPropRoomCount = read_u8(stream);
      if (savedPropRoomCount > 4) {
        practiceLogWarn("Invalid room count %d for saved prop %d",
                        savedPropRoomCount, savedPropIndex);
        return FALSE;
      }
      read_bytes(stream, savedPropRooms, savedPropRoomCount);
      savedPropUnk30 = read_u32(stream);
    }
    ChrAllocationState savedChrAllocation;
    ObjAllocationState savedObjAllocation;
    bool hasChrAllocation = savedPropType == PROP_TYPE_CHR;
    bool hasObjAllocation = savedPropType == PROP_TYPE_DOOR ||
                            savedPropType == PROP_TYPE_OBJ ||
                            savedPropType == PROP_TYPE_WEAPON;
    bool createdObjProp = FALSE;
    bool shouldRegisterObjectRooms = FALSE;
    u8 savedEffectBufferIndex = 0xff;

    for (c = 0; c < i; c++) {
      if (savedLinks[c].index == savedPropIndex) {
        practiceLogWarn("Duplicate saved prop index %d", savedPropIndex);
        return FALSE;
      }
    }
    if (savedPropIndex >= POS_DATA_ENTRY_LEN) {
      practiceLogWarn("Invalid saved prop index %d", savedPropIndex);
      return FALSE;
    }
    savedLinks[i].index = savedPropIndex;
    savedLinks[i].attachmentNode = -1;
    savedLinks[i].objectRuntimeBitflags = 0;
    savedLinks[i].objectProjectile = NULL;

    if (hasChrAllocation) {
      load_chr_allocation_state(stream, &savedChrAllocation);
      if (savedChrAllocation.slot_index < 0 ||
          savedChrAllocation.slot_index >= g_NumChrSlots) {
        practiceLogError("Saved CHR slot %d is invalid",
                         savedChrAllocation.slot_index);
        assert(FALSE);
        return FALSE;
      }
    }
    if (hasObjAllocation) {
      load_obj_allocation_state(stream, &savedObjAllocation);
    }
    if (savedPropType == PROP_TYPE_EXPLOSION ||
        savedPropType == PROP_TYPE_SMOKE) {
      savedEffectBufferIndex = read_u8(stream);
      if ((savedPropType == PROP_TYPE_EXPLOSION &&
           savedEffectBufferIndex >= EXPLOSION_BUFFER_LEN) ||
          (savedPropType == PROP_TYPE_SMOKE &&
           savedEffectBufferIndex >= SMOKE_BUFFER_LEN)) {
        practiceLogError("Prop %d has invalid effect buffer entry %d",
                         savedPropIndex, savedEffectBufferIndex);
        return FALSE;
      }
    }

    PropRecord *prop = get_prop_by_index(savedPropIndex);

    // Remove any enabled props occupying slots the save skipped over so the
    // prop array is rebuilt to exactly match the saved set.
    for (c = nextIndexToRemove; c < savedPropIndex; c++) {
      removePropAtIndex(c);
    }
    nextIndexToRemove = savedPropIndex + 1;

    // Establish the destination prop in this exact slot, recreating it when the
    // current world no longer has a compatible prop there. Slots are kept 1:1
    // with the save so every index-based reference stays valid.
    switch ((PROP_TYPE)savedPropType) {
    case PROP_TYPE_CHR:
      // If the same character still owns this slot, reuse it in place. Body and
      // head IDs alone are insufficient: a freed/reused prop can retain a stale
      // ChrRecord pointer whose actual chr->prop owner is another slot. Reusing
      // that alias restores collision at this prop while leaving the model at
      // the owner's position, producing an invisible blocking guard.
      if (prop->type == PROP_TYPE_CHR && prop->chr != NULL &&
          (prop->flags & PROPFLAG_ENABLED) && prop->chr->prop == prop &&
          prop->chr == &g_ChrSlots[savedChrAllocation.slot_index] &&
          prop->chr->model != NULL && prop->chr->model->chr == prop->chr &&
          prop->chr->bodynum == savedChrAllocation.bodynum &&
          prop->chr->headnum == savedChrAllocation.headnum) {
        break;
      }

      if (prop->type == PROP_TYPE_CHR &&
          (prop->chr != NULL || (prop->flags & PROPFLAG_ENABLED))) {
        if (prop->chr != NULL && prop->chr->prop != prop) {
          // Only this PropRecord is stale. The ChrRecord and model belong to a
          // different slot which may also be present in the save, so tearing
          // them down here would destroy that legitimate character.
          clear_plain_prop(prop, FALSE);
        } else {
          destroy_chr_prop(prop, FALSE);
        }
      } else if (!clear_prop_for_replacement(prop)) {
        practiceLogWarn(
            "Cannot retain prop slot %d while replacing type %d with CHR",
            savedPropIndex, prop->type);
        skip_prop_data(stream, savedPropType, compactContainedObject);
        return FALSE;
      }

      prop = create_chr_prop(prop, &savedChrAllocation, &savedPropPos,
                             savedPropStanOffset);
      if (prop == NULL) {
        practiceLogWarn("Could not recreate CHR prop at index %d",
                        savedPropIndex);
        skip_prop_data(stream, savedPropType, compactContainedObject);
        return FALSE;
      }
      break;

    case PROP_TYPE_OBJ:
    case PROP_TYPE_WEAPON:
      if (!slot_matches_object(prop, savedPropType, &savedObjAllocation) &&
          can_recreate_object(&savedObjAllocation)) {
        if (!clear_prop_for_replacement(prop)) {
          practiceLogWarn(
              "Cannot retain prop slot %d while replacing type %d with object",
              savedPropIndex, prop->type);
          skip_prop_data(stream, savedPropType, compactContainedObject);
          return FALSE;
        }
        prop = create_object_prop(prop, savedPropType, &savedObjAllocation);
        if (prop == NULL) {
          practiceLogWarn("Could not recreate object prop at index %d",
                          savedPropIndex);
          prop = get_prop_by_index(savedPropIndex);
          skip_prop_data(stream, savedPropType, compactContainedObject);
          continue;
        }
        createdObjProp = TRUE;
      } else if (!slot_matches_object(prop, savedPropType,
                                      &savedObjAllocation)) {
        // Runtime object subtypes without a setup origin or creation path
        // cannot be restored safely into an unbound slot.
        practiceLogWarn("Cannot recreate object at prop slot %d (type %d)",
                        savedPropIndex, savedObjAllocation.objtype);
        skip_prop_data(stream, savedPropType, compactContainedObject);
        continue;
      }
      break;

    case PROP_TYPE_EXPLOSION:
      if (prop->type != PROP_TYPE_EXPLOSION || prop->explosion == NULL ||
          prop->explosion->prop != prop ||
          prop->explosion != &g_ExplosionBuffer[savedEffectBufferIndex]) {
        if (!clear_prop_for_replacement(prop)) {
          skip_prop_data(stream, savedPropType, compactContainedObject);
          return FALSE;
        }
        prop = create_explosion_prop(prop, savedEffectBufferIndex);
        if (prop == NULL) {
          practiceLogWarn("Could not recreate explosion prop at index %d",
                          savedPropIndex);
          prop = get_prop_by_index(savedPropIndex);
          skip_prop_data(stream, savedPropType, compactContainedObject);
          continue;
        }
      }
      break;

    case PROP_TYPE_SMOKE:
      if (prop->type != PROP_TYPE_SMOKE || prop->smoke == NULL ||
          prop->smoke->prop != prop ||
          prop->smoke != &g_SmokeBuffer[savedEffectBufferIndex]) {
        if (!clear_prop_for_replacement(prop)) {
          skip_prop_data(stream, savedPropType, compactContainedObject);
          return FALSE;
        }
        prop = create_smoke_prop(prop, savedEffectBufferIndex);
        if (prop == NULL) {
          practiceLogWarn("Could not recreate smoke prop at index %d",
                          savedPropIndex);
          prop = get_prop_by_index(savedPropIndex);
          skip_prop_data(stream, savedPropType, compactContainedObject);
          continue;
        }
      }
      break;

    default:
      break;
    }

    if (savedPropType == PROP_TYPE_VIEWER ||
        ((savedPropType == PROP_TYPE_OBJ ||
          savedPropType == PROP_TYPE_WEAPON) &&
         prop->parent == NULL && prop_is_active_list_member(prop))) {
      forget_prop_rooms(prop);
      shouldRegisterObjectRooms =
          savedPropType != PROP_TYPE_VIEWER && (s16)savedPropParentIdx < 0;
    }

    // Apply the common PropRecord fields. The active-list/attachment-graph
    // linkage is deferred: record the saved indices now and install them in a
    // final pass, after every create/remove/attach operation has finished
    // mutating the lists. Setting them here would let later operations on other
    // slots clobber this prop's links.
    // A concealed item may currently be a dropped active prop. Remove that live
    // registration before replacing its room list and flags with the saved
    // concealed state.
    if ((s16)savedPropParentIdx >= 0 && prop->parent == NULL &&
        prop_is_active_list_member(prop)) {
      forget_prop_rooms(prop);
      chrpropDelist(prop);
    }
    prop->type = savedPropType;
    prop->flags = savedPropFlags;
    prop->timetoregen = savedPropTimetoregen;
    if (!compactContainedObject) {
      prop->pos = savedPropPos;
      prop->stan = get_tile_by_offset(savedPropStanOffset);
      prop->zDepth = savedPropZDepth;
    }
    // Active-list records were standalone roots at save time, while concealed
    // CHR items are reparented in a final pass. Detach any live parent retained
    // after the save (for example, an object collected into Bond's inventory)
    // before either relationship is rebuilt.
    if (prop->parent != NULL) {
      chrpropDetach(prop);
    }
    savedLinks[i].index = savedPropIndex;
    savedLinks[i].parent = savedPropParentIdx;
    savedLinks[i].child = savedPropChildIdx;
    savedLinks[i].prev = savedPropPrevIdx;
    savedLinks[i].next = savedPropNextIdx;
    if (!compactContainedObject) {
      prop->rooms[0] = savedPropRooms[0];
      prop->rooms[1] = savedPropRooms[1];
      prop->rooms[2] = savedPropRooms[2];
      prop->rooms[3] = savedPropRooms[3];
      prop->unk30 = savedPropUnk30;
    }

    switch ((PROP_TYPE)savedPropType) {
    case PROP_TYPE_DOOR: {
      DoorRecord *door = prop->door;
      ObjectRecord *obj = prop->obj;

      if (prop->type != PROP_TYPE_DOOR || prop->door == NULL) {
        practiceLogWarn("Prop at index %d is not a door (type=%d)",
                        savedPropIndex, savedPropType);
        return FALSE;
      }

      if (!load_object_base(stream, obj, prop, &savedLinks[i].attachmentNode)) {
        return FALSE;
      }

      load_door_record(stream, door);

      door7F052B00(door);

      if (door->doorFlags & DOORFLAG_0004) {
        Model *model = obj->model;
        ModelNode *node = model->obj->RootNode->Child->Child;
        struct ModelRoData_DisplayList_CollisionRecord *dlcRecord =
            (struct ModelRoData_DisplayList_CollisionRecord *)node->Data;
        struct ModelRwData_DisplayList_CollisionRecord *dst =
            (struct ModelRwData_DisplayList_CollisionRecord *)
                modelGetNodeRwData(model, node);

        if (door->unkcc != NULL && dlcRecord != NULL &&
            dlcRecord->Vertices != NULL) {
          s32 var_a0;
          for (var_a0 = 0; var_a0 < dlcRecord->numVertices; var_a0++) {
            door->unkcc[var_a0] = dlcRecord->Vertices[var_a0];
          }
        }

        if (door->openPosition <= 0.0f) {
          if (door->unkcc != NULL) {
            dst->Vertices = door->unkcc;
          }
        } else {
          sub_GAME_7F052D8C(door);
        }
      }

      if (door->openPosition > 0.0f) {
        doorActivatePortal(door);
      } else {
        doorDeactivatePortal(door);
      }
      break;
    }

    case PROP_TYPE_OBJ:
    case PROP_TYPE_WEAPON: {
      ObjectRecord *obj = prop->obj;
      if (obj == NULL) {
        // Just skip the rest
        TempObjectRecord temp_obj;
        bzero(&temp_obj, sizeof(temp_obj));
        if (compactContainedObject) {
          load_compact_contained_object_base(stream, &temp_obj.base, NULL);
          load_object_subtype(stream, &temp_obj.base);
        } else if (load_object_base(stream, &temp_obj.base, NULL, NULL)) {
          if (savedPropType == PROP_TYPE_WEAPON) {
            load_equipped_weapon_matrices(stream, NULL);
          }
          load_object_subtype(stream, &temp_obj.base);
        }
        break;
      }

      if (obj->model != NULL && obj->model->obj != NULL) {
        s32 swIdx;
        for (swIdx = 0; swIdx < obj->model->obj->numSwitches; swIdx++) {
          ModelNode *switchNode = obj->model->obj->Switches[swIdx];
          if (switchNode != NULL &&
              (switchNode->Opcode & 0xFF) == MODELNODE_OPCODE_SWITCH) {
            union ModelRwData *rwdata =
                modelGetNodeRwData(obj->model, switchNode);
            if (rwdata != NULL) {
              rwdata->Switch.visible = TRUE;
            }
          }
        }
        sub_GAME_7F050DE8(obj->model);
      }
      if (compactContainedObject) {
        load_compact_contained_object_base(stream, obj, prop);
      } else {
        if (!load_object_base(stream, obj, prop,
                              &savedLinks[i].attachmentNode)) {
          return FALSE;
        }
        if (savedPropType == PROP_TYPE_WEAPON) {
          load_equipped_weapon_matrices(stream, obj);
        }
      }

      load_object_subtype(stream, obj);

      (void)createdObjProp;
      (void)shouldRegisterObjectRooms;

      // Dynamic collision geometry is still rebuilt from the restored
      // transform; this is independent of room-list registration.
      if (obj->ptr_allocated_collisiondata_block != NULL) {
        chrobjCollisionRelated(obj);
      }
      break;
    }

    case PROP_TYPE_SMOKE:
      // The slot now holds the saved smoke prop (reused or recreated above);
      // load straight into its buffer entry. A failed recreation falls back to
      // a throwaway record so the stream stays aligned.
      if (prop->smoke != NULL && prop->smoke->prop == prop) {
        load_smoke_record(stream, prop->smoke);
      } else {
        struct Smoke temp_smoke;
        bzero(&temp_smoke, sizeof(temp_smoke));
        load_smoke_record(stream, &temp_smoke);
      }
      break;

    case PROP_TYPE_EXPLOSION:
      // The slot now holds the saved explosion prop (reused or recreated
      // above); load straight into its buffer entry. A failed recreation falls
      // back to a throwaway record so the stream stays aligned.
      if (prop->explosion != NULL && prop->explosion->prop == prop) {
        load_explosion_record(stream, prop->explosion);
      } else {
        struct Explosion temp_explosion;
        bzero(&temp_explosion, sizeof(temp_explosion));
        load_explosion_record(stream, &temp_explosion);
      }
      break;

    case PROP_TYPE_CHR:
      // Replacement mode rebuilt the PropRecord, ChrRecord, and Model before
      // applying common fields. Testing mode reaches this only for a matching
      // existing active CHR. In replacement mode the saved body and head were
      // used to allocate the model; in testing mode model allocation is
      // intentionally retained, but the immutable configuration IDs are still
      // restored with the rest of the ChrRecord.
      if (prop->chr == NULL) {
        skip_prop_data(stream, PROP_TYPE_CHR, FALSE);
      } else {
        if (pendingChrCount >= pendingChrCapacity) {
          practiceLogWarn("Too many CHR props in save state");
          return FALSE;
        }
        prop->chr->headnum = savedChrAllocation.headnum;
        prop->chr->bodynum = savedChrAllocation.bodynum;
        load_chr_prop_spatial_state(prop, &savedPropPos, savedPropStanOffset,
                                    savedPropRooms);
        pendingChrAttachments[pendingChrCount].prop_index = savedPropIndex;
        load_chr_record(stream, prop->chr,
                        &pendingChrAttachments[pendingChrCount].attachments);
        pendingChrCount++;
      }
      break;

    case PROP_TYPE_NUL:
    case PROP_TYPE_PLAYER:
    // Viewer state loaded after rest of props
    case PROP_TYPE_VIEWER:
    default:
      break;
    }

    if ((savedPropType == PROP_TYPE_OBJ ||
         savedPropType == PROP_TYPE_DOOR ||
         savedPropType == PROP_TYPE_WEAPON) &&
        prop->obj != NULL) {
      savedLinks[i].objectRuntimeBitflags = prop->obj->runtime_bitflags;
      savedLinks[i].objectProjectile = prop->obj->projectile;
    }
  }

  // Resolve CHR equipment only after every referenced prop has loaded. Indices
  // refer to the rebuilt prop table.
  for (i = 0; i < pendingChrCount; i++) {
    PropRecord *chr_prop =
        get_prop_by_index(pendingChrAttachments[i].prop_index);
    if (chr_prop != NULL && chr_prop->type == PROP_TYPE_CHR) {
      cleanup_live_chr_children(chr_prop, savedLinks, recordCount);
      restore_chr_attachments(chr_prop, &pendingChrAttachments[i].attachments,
                              savedLinks, recordCount);
    }
  }

  // Concealed CHR inventory is not represented by weapons_held or the hat
  // pointer. Reattach those saved child props after normal equipment has
  // rebuilt the CHR child chain.
  for (i = 0; i < recordCount; i++) {
    PropRecord *prop;
    PropRecord *parent;

    if ((s16)savedLinks[i].parent < 0) {
      continue;
    }

    prop = get_prop_by_index(savedLinks[i].index);
    parent = get_prop_by_index(savedLinks[i].parent);
    if (prop == NULL ||
        (prop->type != PROP_TYPE_OBJ && prop->type != PROP_TYPE_WEAPON)) {
      continue;
    }

    if (prop->obj != NULL &&
        (prop->obj->runtime_bitflags & RUNTIMEBITFLAG_EMBEDDED)) {
      if (!restore_embedded_object(prop, parent,
                                   savedLinks[i].attachmentNode)) {
        return FALSE;
      }
    } else if (parent != NULL && parent->type == PROP_TYPE_CHR) {
      restore_concealed_chr_item(prop, parent);
    } else if (parent != NULL &&
               (parent->type == PROP_TYPE_PLAYER ||
                parent->type == PROP_TYPE_VIEWER)) {
      restore_player_child_object(prop, parent);
    } else if (parent != NULL &&
               (parent->type == PROP_TYPE_OBJ ||
                parent->type == PROP_TYPE_DOOR ||
                parent->type == PROP_TYPE_WEAPON) &&
               !restore_contained_object(prop, parent)) {
      return FALSE;
    }
  }

  // Attachment restoration uses engine helpers that freely rewrite prev/next
  // while moving objects between active-list and child-list states. Reinstall
  // the saved CHR child graph afterward so prev remains the child-walk link and
  // next remains its reverse link for drop/teardown code.
  rebuild_saved_child_links(savedLinks, recordCount);

  /* Reparenting helpers normalize ownership flags. Preserve transitional
   * states too, such as a held item already queued to drop with a projectile. */
  for (i = 0; i < recordCount; i++) {
    PropRecord *prop = get_prop_by_index(savedLinks[i].index);

    if (prop != NULL &&
        (prop->type == PROP_TYPE_OBJ || prop->type == PROP_TYPE_DOOR ||
         prop->type == PROP_TYPE_WEAPON) &&
        prop->obj != NULL) {
      prop->obj->runtime_bitflags = savedLinks[i].objectRuntimeBitflags;
      prop->obj->projectile = savedLinks[i].objectProjectile;
    }
  }

  /* Resolve projectile references only after all saved props are processed. */
  for (pi = 0; pi < PROJECTILES_ARR_MAX; pi++) {
    Projectile *proj = &g_Projectiles[pi];
    PropRecord *objProp;

    if (proj->flags & PROJECTILEFLAG_FREE) {
      continue;
    }

    proj->ownerprop = get_enabled_prop_by_index(projectileOwnerPropIndices[pi]);
    objProp = get_enabled_prop_by_index(projectileObjPropIndices[pi]);

    if (projectileObjPropIndices[pi] == -1) {
      // Projectile::obj is optional. In particular, dropped objects created by
      // objBounce point to the projectile from ObjectRecord::projectile while
      // this reverse pointer remains NULL.
      proj->obj = NULL;
    } else if (objProp != NULL &&
               (objProp->type == PROP_TYPE_OBJ ||
                objProp->type == PROP_TYPE_DOOR ||
                objProp->type == PROP_TYPE_WEAPON) &&
               objProp->obj != NULL) {
      proj->obj = objProp->obj;
    } else {
      proj->obj = NULL;
      practiceLogError("Projectile %d has invalid saved object prop %d", pi,
                       projectileObjPropIndices[pi]);
      assert(FALSE);
    }
  }

  // Clear any props which were not saved.
  for (c = nextIndexToRemove; c < POS_DATA_ENTRY_LEN; c++) {
    removePropAtIndex(c);
  }

  // Rebuild the active list now that all prop creation, teardown, and
  // attachments are complete. Only standalone records receive their saved
  // prev/next links; concealed items and equipment already use those fields
  // for the CHR child-sibling graph.
  //
  // prev/next are overloaded: they are the active-list links for standalone
  // props, but also the child-sibling links for attached equipment.
  // A prop that was an enabled standalone record at save time (so it appears in
  // savedLinks) can be re-attached as a character's child by
  // restore_chr_attachments above, which repurposes its prev/next as sibling
  // links. Restoring the saved active-list neighbours onto such a prop would
  // splice it back into the active list while it is still a child, so the
  // character teardown walk (disable_sounds_attached_to_player_then_something
  // follows child -> prev) wanders out of the sibling chain into the active
  // list and frees an unrelated prop's record as an object. Only re-link props
  // that are still standalone. This intentionally includes disabled,
  // respawning pickups, which remain active so timetoregen keeps ticking.
  for (i = 0; i < recordCount; i++) {
    PropRecord *p = get_prop_by_index(savedLinks[i].index);
    if (p != NULL && p->parent == NULL) {
      p->prev = get_prop_by_index(savedLinks[i].prev);
      p->next = get_prop_by_index(savedLinks[i].next);
    }
  }

  ptr_obj_pos_list_first_entry = get_prop_by_index(indexOfFirstEntry);
  ptr_obj_pos_list_current_entry = get_prop_by_index(indexOfCurrentEntry);

  // Rebuild the free list (final_entry -> prev -> ... chain) from scratch
  // rather than trusting the saved head. A slot is in use when it belongs to
  // the saved active list (including disabled respawning props), is attached,
  // or remains bound to a live backing record. The latter covers inactive
  // setup objects which are not active-list records. The free-list prev links
  // are not serialized, so the saved head is intentionally ignored.
  (void)indexOfFinalEntry;
  ptr_obj_pos_list_final_entry = NULL;
  for (i = 0; i < POS_DATA_ENTRY_LEN; i++) {
    PropRecord *p = get_prop_by_index(i);
    bool wasSavedActive = FALSE;
    for (c = 0; c < recordCount; c++) {
      if (savedLinks[c].index == i) {
        wasSavedActive = TRUE;
        break;
      }
    }
    if (p != NULL && !wasSavedActive && p->parent == NULL &&
        !prop_slot_has_live_owner(p)) {
      p->prev = ptr_obj_pos_list_final_entry;
      p->next = NULL;
      p->stan = NULL;
      ptr_obj_pos_list_final_entry = p;
    }
  }

  if (!load_viewer_players_state(stream)) {
    return FALSE;
  }

  rebuild_room_prop_lists_from_active_props();

  // Restore scorch marks and bullet holes now that the prop table is rebuilt,
  // so prop-attached impacts can resolve their saved prop index.
  load_decals_state(stream);
  load_free_effect_entries(stream);
  load_flying_particles_state(stream);
  load_gun_effects_state(stream);
  load_casings_state(stream);
  load_darkened_lights_state(stream);
  load_bg_room_cache_state(stream);
  if (stream->base_address + stream->total_processed + sizeof(u32) +
          sizeof(u16) <=
      dataStart + totalPropsSize) {
    load_shattered_window_pieces_state(stream);
  } else {
    clear_shattered_window_pieces_state();
  }

  /* Allocation order is gameplay state: explosions, smoke, dropped weapons,
   * and other dynamic props take the free-list head. Rebuilding by slot index
   * changes the prop selected by the next allocation and can alter update
   * ordering and RNG use. Restore the exact saved head-to-tail chain. */
  {
    u16 free_count = read_u16(stream);
    PropRecord *first_free = NULL;
    PropRecord *previous_free = NULL;

    ptr_obj_pos_list_final_entry = NULL;
    for (i = 0; i < free_count; i++) {
      u16 index = read_u16(stream);
      PropRecord *free_prop = get_prop_by_index(index);
      bool was_saved = FALSE;

      for (c = 0; c < recordCount; c++) {
        if (savedLinks[c].index == index) {
          was_saved = TRUE;
          break;
        }
      }

      // The saved free list is authoritative. Loading can retain a stale
      // convenience pointer (for example, a CHR weapons_held entry) whose prop
      // was already free when the state was captured. If restoration attached
      // that current-runtime occupant, remove it before rebuilding the exact
      // saved allocation chain.
      if (free_prop != NULL && !was_saved &&
          (free_prop->parent != NULL || prop_slot_has_live_owner(free_prop))) {
        removePropAtIndex(index);
        free_prop = get_prop_by_index(index);
      }

      if (free_prop == NULL || was_saved || free_prop->parent != NULL ||
          prop_slot_has_live_owner(free_prop)) {
        practiceLogError(
            "Invalid saved prop free-list entry %d (saved=%d type=%d "
            "flags=%02x parent=%x owner=%d)",
            index, was_saved, free_prop != NULL ? free_prop->type : -1,
            free_prop != NULL ? free_prop->flags : 0,
            free_prop != NULL ? free_prop->parent : NULL,
            prop_slot_has_live_owner(free_prop));
        assert(FALSE);
        return FALSE;
      }

      free_prop->next = NULL;
      free_prop->stan = NULL;
      if (previous_free == NULL) {
        first_free = free_prop;
      } else {
        previous_free->prev = free_prop;
      }
      previous_free = free_prop;
    }
    if (previous_free != NULL) {
      previous_free->prev = NULL;
    }
    /* removePropAtIndex may call chrpropFree while stale occupants are being
     * torn down, which temporarily changes the global head. The links above
     * are authoritative, so publish their saved head only after cleanup is
     * complete. */
    ptr_obj_pos_list_final_entry = first_free;
  }

  if (!load_room_prop_lists(stream)) {
    return FALSE;
  }
  load_onscreen_prop_list(stream);

  return TRUE;
}
