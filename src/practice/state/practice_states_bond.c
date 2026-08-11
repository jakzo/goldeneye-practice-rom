#include "../practice_render.h"
#include "bondinv.h"
#include "bondview.h"
#include "chrai.h"
#include "chrobjdata.h"
#include "dyn.h"
#include "matrixmath.h"
#include "objecthandler.h"
#include "player.h"
#include "player_2.h"
#include "practice_states.h"
#include "practice_states_chr.h"
#include "practice_states_utils.h"
#include "practice_ui.h"
#include "watch.h"
#include <assert.h>
#include <bondconstants.h>
#include <bondgame.h>
#include <joy.h>
#include <music.h>
#include <os_extension.h>
#include <snd.h>
#include <ultra64.h>

extern void used_to_load_1st_person_model_on_demand(enum GUNHAND hand);
extern void sub_GAME_7F05E978(Model *model, s32 value);
extern void sub_GAME_7F05EA94(Model *model, s32 value);
extern void sub_GAME_7F07E7CC(void);

extern void bondviewApplyVertaTheta(void);
extern void chrpropDelist(PropRecord *prop);
extern void disable_sounds_attached_to_player_then_something(PropRecord *prop);
extern int bondinvAddPropToInv(PropRecord *prop);
extern void solo_char_load(void);
extern void sub_GAME_7F07DE9C(struct player *player);
extern s32 g_EnterTankAudioState;
extern void *memcpy(void *dst, const void *src, size_t count);
extern u8 *g_GfxMemPos;
extern u8 *g_VtxBuffers[3];
extern u8 g_GfxActiveBufferIndex;

#define MAX_SAVED_HAND_RENDER_MATRICES 256

static const struct {
  u32 srcoff;
  u32 size;
} player_blocks[6] = {
#if defined(VERSION_EU)
    {0x0000, 0x0598}, {0x0658, 0x01B0}, {0x0868, 0x0854},
    {0x1120, 0x0080}, {0x11B0, 0x0100}, {0x29B0, 0x00B8},
#else
    {0x0000, 0x0598}, {0x0658, 0x01B0}, {0x0870, 0x0854},
    {0x1128, 0x0080}, {0x11B8, 0x0100}, {0x29B8, 0x00B8},
#endif
};

#define PLAYER_BLOCK_COUNT (sizeof(player_blocks) / sizeof(player_blocks[0]))

static bool is_rdram_range(const void *ptr, u32 size) {
  u32 addr = (u32)ptr;
  u32 end = addr + size;
  u32 rdram_end = 0x80000000 + osMemSize;

  return addr >= 0x80000000 && end >= addr && end <= rdram_end;
}

static bool is_active_gfx_range(const void *ptr, u32 size) {
  u32 addr = (u32)ptr;
  u32 end = addr + size;
  u32 start = (u32)g_VtxBuffers[g_GfxActiveBufferIndex];

  return addr >= start && end >= addr && end <= (u32)g_GfxMemPos;
}

static bool player_model_is_valid_for_chr(Model *model, ChrRecord *chr) {
  if (!is_rdram_range(chr, sizeof(ChrRecord)) ||
      !is_rdram_range(model, sizeof(Model)) || model->chr != chr ||
      !is_rdram_range(model->obj, sizeof(ModelFileHeader))) {
    return FALSE;
  }

  return is_rdram_range(model->obj->RootNode, sizeof(ModelNode));
}

static bool current_player_model_is_valid(void) {
  PropRecord *prop;
  ChrRecord *chr;
  Model *model;

  if (g_CurrentPlayer == NULL ||
      !is_rdram_range(g_CurrentPlayer->ptr_char_objectinstance,
                      sizeof(Model))) {
    return FALSE;
  }

  prop = g_CurrentPlayer->prop;
  model = g_CurrentPlayer->ptr_char_objectinstance;
  if (prop == NULL || prop->chr == NULL) {
    return FALSE;
  }

  chr = prop->chr;
  if (chr->prop != prop || chr->model != model) {
    return FALSE;
  }

  return player_model_is_valid_for_chr(model, chr);
}

static bool player_presence_prop_is_active(PropRecord *target) {
  PropRecord *prop = ptr_obj_pos_list_first_entry;
  s32 guard = 0;

  while (prop != NULL && guard++ < POS_DATA_ENTRY_LEN) {
    if (prop == target) {
      return TRUE;
    }
    prop = prop->next;
  }

  return FALSE;
}

static void release_player_presence_prop(PropRecord *prop) {
  if (prop == NULL) {
    return;
  }

  if ((prop->type != PROP_TYPE_VIEWER && prop->type != PROP_TYPE_PLAYER) ||
      prop->parent != NULL || !player_presence_prop_is_active(prop)) {
    practiceLogError("Invalid stale player prop index=%d type=%d parent=%d",
                     get_prop_index(prop), prop->type,
                     get_prop_index(prop->parent));
    assert(FALSE);
    return;
  }

  chrpropDeregisterRooms(prop);
  chrpropDelist(prop);
  chrpropDisable(prop);
  chrpropFree(prop);
}

static ModelNode *next_model_node(ModelNode *root, ModelNode *node) {
  if (node->Child != NULL) {
    return node->Child;
  }
  while (node != NULL && node != root) {
    if (node->Next != NULL) {
      return node->Next;
    }
    node = node->Parent;
  }
  return NULL;
}

static void write_zero_rle(StateStream *stream, const u8 *src, u32 size);
static void read_zero_rle(StateStream *stream, u8 *dst, u32 size);

static void save_hand_gunfire_nodes(StateStream *stream, Model *model) {
  ModelNode *root = model->obj->RootNode;
  ModelNode *node;
  u8 count = 0;

  for (node = root; node != NULL; node = next_model_node(root, node)) {
    if ((node->Opcode & 0xff) == MODELNODE_OPCODE_GUNFIRE) {
      count++;
    }
  }
  write_u8(stream, count);
  for (node = root; node != NULL; node = next_model_node(root, node)) {
    if ((node->Opcode & 0xff) == MODELNODE_OPCODE_GUNFIRE) {
      ModelRwData_GunfireRecord *rwdata =
          (ModelRwData_GunfireRecord *)modelGetNodeRwData(model, node);
      write_u32(stream, *(u32 *)rwdata);
    }
  }
}

static void load_hand_gunfire_nodes(StateStream *stream, Model *model) {
  ModelNode *root = model != NULL ? model->obj->RootNode : NULL;
  ModelNode *node = root;
  u8 count = read_u8(stream);
  u8 i;

  for (i = 0; i < count; i++) {
    u32 value = read_u32(stream);
    while (node != NULL && (node->Opcode & 0xff) != MODELNODE_OPCODE_GUNFIRE) {
      node = next_model_node(root, node);
    }
    if (node != NULL) {
      ModelRwData_GunfireRecord *rwdata =
          (ModelRwData_GunfireRecord *)modelGetNodeRwData(model, node);
      *(u32 *)rwdata = value;
      node = next_model_node(root, node);
    }
  }
}

#ifdef DEV

static void validate_current_player_model_invariant(bool saved_had_model) {
  PropRecord *prop;
  ChrRecord *chr;
  Model *model;

  if (g_CurrentPlayer == NULL) {
    return;
  }

  prop = g_CurrentPlayer->prop;
  model = g_CurrentPlayer->ptr_char_objectinstance;

  if (saved_had_model != (model != NULL)) {
    practiceLogError("Bond model presence mismatch saved=%d live=%08x",
                     saved_had_model, model);
  }

  if (model == NULL) {
    return;
  }

  if (prop == NULL) {
    practiceLogError("Bond model live without viewer prop model=%08x", model);
    return;
  }

  if (prop->chr == NULL) {
    practiceLogError("Bond model live without viewer chr model=%08x prop=%08x",
                     model, prop);
    return;
  }

  if (!is_rdram_range(model, sizeof(Model))) {
    practiceLogError("Bond model pointer outside RDRAM model=%08x", model);
    return;
  }

  chr = prop->chr;

  if (chr->model != model || model->chr != chr) {
    practiceLogError("Bond model/chr mismatch model=%08x chr=%08x "
                     "chrmodel=%08x modelchr=%08x",
                     model, chr, chr->model, model->chr);
  }

  if (!is_rdram_range(model->obj, sizeof(ModelFileHeader))) {
    practiceLogError("Bond model header invalid model=%08x obj=%08x", model,
                     model->obj);
    return;
  }

  if (!is_rdram_range(model->obj->RootNode, sizeof(ModelNode))) {
    practiceLogError("Bond model root invalid model=%08x obj=%08x root=%08x",
                     model, model->obj, model->obj->RootNode);
  }
}
#endif

static void reconcile_current_player_model_presence(bool saved_had_model,
                                                    bool force_rebuild) {
  bool live_has_model;

  if (g_CurrentPlayer == NULL) {
    return;
  }

  live_has_model = current_player_model_is_valid();
  if (force_rebuild && live_has_model && g_CurrentPlayer->prop != NULL &&
      g_CurrentPlayer->prop->chr != NULL) {
    disable_sounds_attached_to_player_then_something(g_CurrentPlayer->prop);
    g_CurrentPlayer->prop->chr = NULL;
    sub_GAME_7F07DE9C(g_CurrentPlayer);
    g_CurrentPlayer->ptr_char_objectinstance = NULL;
    live_has_model = FALSE;
  }
  if (g_CurrentPlayer->ptr_char_objectinstance != NULL && !live_has_model) {
    Model *stale_model = g_CurrentPlayer->ptr_char_objectinstance;

    practiceLogDebug("Rebuilding invalid Bond model %08x", stale_model);
    if (g_CurrentPlayer->prop != NULL && g_CurrentPlayer->prop->chr != NULL) {
      ChrRecord *chr = g_CurrentPlayer->prop->chr;

      if (chr->prop == g_CurrentPlayer->prop) {
        if (player_model_is_valid_for_chr(chr->model, chr)) {
          disable_sounds_attached_to_player_then_something(
              g_CurrentPlayer->prop);
        } else {
          /* Loading after the replay has ended restores the saved player
           * pointers after the live Bond model has already been released.
           * The ChrRecord can still belong to the viewer, but its model graph
           * is no longer safe to walk. Establish the same free-slot markers
           * as normal character teardown without touching the stale model. */
          chr->model = NULL;
          chr->chrnum = -1;
        }
      }
      g_CurrentPlayer->prop->chr = NULL;
      sub_GAME_7F07DE9C(g_CurrentPlayer);
    }
    g_CurrentPlayer->ptr_char_objectinstance = NULL;
  }
  if (saved_had_model) {
    if (live_has_model) {
      return;
    }
    if (g_CurrentPlayer->prop != NULL) {
      // A teardown can leave the viewer prop holding a ChrRecord whose model
      // was cleared. solo_char_load() only rebuilds when prop->chr is NULL.
      if (g_CurrentPlayer->prop->chr != NULL &&
          g_CurrentPlayer->prop->chr->model == NULL) {
        g_CurrentPlayer->prop->chr = NULL;
      }
      solo_char_load();
    }
    return;
  }

  if (!live_has_model) {
    return;
  }

  if (g_CurrentPlayer->prop != NULL && g_CurrentPlayer->prop->chr != NULL) {
    // Mirror the game's viewer-model teardown, but do not set
    // g_bondviewForceDisarm because this is state reconciliation, not gameplay.
    disable_sounds_attached_to_player_then_something(g_CurrentPlayer->prop);
    g_CurrentPlayer->prop->chr = NULL;
    sub_GAME_7F07DE9C(g_CurrentPlayer);
  }
  g_CurrentPlayer->ptr_char_objectinstance = NULL;
}

static void write_zero_rle(StateStream *stream, const u8 *src, u32 size) {
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

static void read_zero_rle(StateStream *stream, u8 *dst, u32 size) {
  u32 offset = 0;

  while (offset < size) {
    u8 control = read_u8(stream);
    u32 count = (control & 0x7f) + 1;

    if (count > size - offset) {
      practiceLogError("Player state RLE exceeds block size");
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

static void write_sparse_matrix(StateStream *stream, const Mtxf *matrix) {
  const u32 *words = (const u32 *)matrix;
  u16 present = 0;
  s32 word;

  for (word = 0; word < 16; word++) {
    if (words[word] != 0) {
      present |= 1 << word;
    }
  }
  write_u16(stream, present);
  for (word = 0; word < 16; word++) {
    if (present & (1 << word)) {
      write_u32(stream, words[word]);
    }
  }
}

static void read_sparse_matrix(StateStream *stream, Mtxf *matrix) {
  u32 *words = (u32 *)matrix;
  u16 present = read_u16(stream);
  s32 word;

  for (word = 0; word < 16; word++) {
    words[word] = present & (1 << word) ? read_u32(stream) : 0;
  }
}

static void write_delta_matrix(StateStream *stream, const Mtxf *matrix,
                               const Mtxf *base) {
  const u32 *words = (const u32 *)matrix;
  const u32 *base_words = (const u32 *)base;
  u32 codes = 0;
  s32 word;

  for (word = 0; word < 16; word++) {
    u32 delta = words[word] ^ base_words[word];
    u32 code = delta == 0 ? 0 : delta <= 0xff ? 1 : delta <= 0xffff ? 2 : 3;
    codes |= code << (word * 2);
  }
  write_u32(stream, codes);
  for (word = 0; word < 16; word++) {
    u32 delta = words[word] ^ base_words[word];
    u32 code = (codes >> (word * 2)) & 3;

    if (code == 1) {
      write_u8(stream, delta);
    } else if (code == 2) {
      write_u16(stream, delta);
    } else if (code == 3) {
      write_u32(stream, delta);
    }
  }
}

static void read_delta_matrix(StateStream *stream, Mtxf *matrix,
                              const Mtxf *base) {
  u32 *words = (u32 *)matrix;
  const u32 *base_words = (const u32 *)base;
  u32 codes = read_u32(stream);
  s32 word;

  for (word = 0; word < 16; word++) {
    u32 code = (codes >> (word * 2)) & 3;
    u32 delta = code == 0   ? 0
                : code == 1 ? read_u8(stream)
                : code == 2 ? read_u16(stream)
                            : read_u32(stream);
    words[word] = base_words[word] ^ delta;
  }
}

static void save_player_state_direct(StateStream *stream, struct player *src) {
  u8 *src_bytes = (u8 *)src;
  s32 i;
  for (i = 0; i < PLAYER_BLOCK_COUNT; i++) {
    write_zero_rle(stream, src_bytes + player_blocks[i].srcoff,
                   player_blocks[i].size);
  }
}

static bool load_player_state_direct(StateStream *stream, struct player *dst) {
  u8 *dst_bytes = (u8 *)dst;
  s32 backup_field_5C = dst->field_5C;
  s32 backup_field_60 = dst->field_60;
  s32 backup_field_64 = dst->field_64;
  s32 backup_field_68 = dst->field_68;
  PropRecord *backup_prop = dst->prop;
  struct Model *backup_ptr_char_objectinstance = dst->ptr_char_objectinstance;
  Model backup_head_model = *(Model *)&dst->model;
  ObjectRecord *backup_hand_rocket[2];
  s32 backup_hand_model_header[2];
  s32 backup_hand_render_pos[2];
  s32 backup_hand_rw_data[2];
  InvItem *backup_ptr_inventory_first_in_cycle =
      dst->ptr_inventory_first_in_cycle;
  InvItem *backup_p_itemcur = dst->p_itemcur;
  textoverride *backup_textoverrides = dst->textoverrides;
  bool saved_had_model;
  s32 i;

  backup_hand_rocket[0] = dst->hands[0].rocket;
  backup_hand_rocket[1] = dst->hands[1].rocket;
  for (i = 0; i < 2; i++) {
    backup_hand_model_header[i] = dst->hands[i].field_B70;
    backup_hand_render_pos[i] = dst->hands[i].field_B74;
    backup_hand_rw_data[i] = dst->hands[i].field_B78;
  }

  for (i = 0; i < PLAYER_BLOCK_COUNT; i++) {
    read_zero_rle(stream, dst_bytes + player_blocks[i].srcoff,
                  player_blocks[i].size);
  }

  saved_had_model = dst->ptr_char_objectinstance != NULL;

  dst->field_5C = backup_field_5C;
  dst->field_60 = backup_field_60;
  dst->field_64 = backup_field_64;
  dst->field_68 = backup_field_68;
  dst->prop = backup_prop;
  dst->ptr_char_objectinstance = backup_ptr_char_objectinstance;
  /* `player::model` is the misleadingly typed first word of an embedded Model
   * spanning 0x598-0x657. Preserve its complete live allocation graph; the
   * dedicated animation payload below restores its logical saved state. */
  *(Model *)&dst->model = backup_head_model;
  dst->hands[0].rocket = backup_hand_rocket[0];
  dst->hands[1].rocket = backup_hand_rocket[1];
  for (i = 0; i < 2; i++) {
    dst->hands[i].field_B70 = backup_hand_model_header[i];
    dst->hands[i].field_B74 = backup_hand_render_pos[i];
    dst->hands[i].field_B78 = backup_hand_rw_data[i];
  }
  dst->ptr_inventory_first_in_cycle = backup_ptr_inventory_first_in_cycle;
  dst->p_itemcur = backup_p_itemcur;
  dst->textoverrides = backup_textoverrides;

  return saved_had_model;
}

static void save_current_player_state(StateStream *stream) {
  if (g_CurrentPlayer == NULL) {
    return;
  }

  /* 1. Player blocks — direct to storage. */
  save_player_state_direct(stream, g_CurrentPlayer);

  /* 2. Bond helper section (sparse fields). */
  write_u32(stream, get_tile_offset(g_CurrentPlayer->room_pointer));
  write_u32(stream,
            get_tile_offset(g_CurrentPlayer->field_488.current_tile_ptr));
  write_u32(
      stream,
      get_tile_offset(g_CurrentPlayer->field_488.current_tile_ptr_for_portals));
  write_u32(stream,
            get_tile_offset(
                g_CurrentPlayer->previous_collision_info.current_tile_ptr));
  write_u32(stream, get_tile_offset(g_CurrentPlayer->previous_collision_info
                                        .current_tile_ptr_for_portals));
  write_u32(stream, get_tile_offset(g_CurrentPlayer->field_2A70));
  write_u32(stream, get_prop_index(g_CurrentPlayer->autoaim_target_y));
  write_u32(stream, get_prop_index(g_CurrentPlayer->autoaim_target_x));

  if (g_CurrentPlayer->prop != NULL) {
    write_u8(stream, TRUE);
    write_bytes(stream, &g_CurrentPlayer->prop->pos, sizeof(coord3d));
    write_u32(stream, get_tile_offset(g_CurrentPlayer->prop->stan));
    write_u8(stream, g_CurrentPlayer->prop->rooms[0]);
    write_u8(stream, g_CurrentPlayer->prop->rooms[1]);
    write_u8(stream, g_CurrentPlayer->prop->rooms[2]);
    write_u8(stream, g_CurrentPlayer->prop->rooms[3]);
  } else {
    write_u8(stream, FALSE);
  }

  /* 3. Inventory section. */
  {
    InvItem *first = g_CurrentPlayer->ptr_inventory_first_in_cycle;
    InvItem *item = first;
    s32 count = 0;
    s32 i;

    if (item != NULL) {
      do {
        count++;
        item = item->next;
      } while (item != first && item != NULL);
    }

    write_u32(stream, count);

    item = first;
    for (i = 0; i < count; i++) {
      s32 weapon_right = -1;
      s32 weapon_left = -1;
      s32 prop_index = -1;

      write_u32(stream, item->type);
      if (item->type == INV_ITEM_WEAPON) {
        weapon_right = item->type_inv_item.type_weap.weapon;
      } else if (item->type == INV_ITEM_DUAL) {
        weapon_right = item->type_inv_item.type_dual.weapon_right;
        weapon_left = item->type_inv_item.type_dual.weapon_left;
      } else if (item->type == INV_ITEM_PROP) {
        prop_index = get_prop_index(item->type_inv_item.type_prop.prop);
      }
      write_u32(stream, weapon_right);
      write_u32(stream, weapon_left);
      write_u32(stream, prop_index);

      item = item->next;
    }
  }

  /* 4. First-person hand matrices. Paused rendering restores these buffers to
   * float while a live render leaves them packed for the RSP, so normalize
   * either representation to exact floats in storage. */
  {
    s32 hand;

    for (hand = 0; hand < 2; hand++) {
      ModelFileHeader *header =
          (ModelFileHeader *)g_CurrentPlayer->hands[hand].field_B70;
      RenderPosView *matrices =
          (RenderPosView *)g_CurrentPlayer->hands[hand].field_B74;
      s32 count = header != NULL && matrices != NULL ? header->numMatrices : 0;

      if (count < 0 || count > MAX_SAVED_HAND_RENDER_MATRICES) {
        count = 0;
      }

      write_u16(stream, count);
      if (count > 0) {
        s32 matrix;
        for (matrix = 0; matrix < count; matrix++) {
          Mtxf value;
          if (practice_hand_render_matrices_are_fixed(hand)) {
            guMtxL2F(value.m, (Mtx *)&matrices[matrix].pos);
          } else {
            matrix_4x4_copy(&matrices[matrix].pos, &value);
          }
          write_bytes(stream, &value, sizeof(value));
        }
      }
    }
  }

  /* 5. Watch model animation. The containing player block only preserves its
   * scalar fields; its model/header/RW-data pointers are rebuilt on load. */
  {
    Model *watch_model =
        (Model *)&g_CurrentPlayer->something_with_watch_object_instance;
    ModelFileHeader *watch_header =
        &g_CurrentPlayer->copy_of_body_obj_header[GUNLEFT];
    bool has_watch_model =
        g_CurrentPlayer->hand_item[GUNLEFT] == ITEM_SUIT_LF_HAND &&
        watch_model->obj == watch_header && watch_header->RootNode != NULL;

    write_u8(stream, has_watch_model);
    if (has_watch_model) {
      practice_states_save_model_animation(stream, watch_model);
    }
  }

  /* 6. First-person weapon model animations. Rebuilding a hand model restores
   * its allocation graph but resets animation progress; the firing state
   * machine consults that progress when deciding whether a held trigger starts
   * another shot. */
  {
    s32 hand;

    for (hand = 0; hand < 2; hand++) {
      Model *model = (Model *)&g_CurrentPlayer->hands[hand].field_B68;
      ModelFileHeader *header = &g_CurrentPlayer->copy_of_body_obj_header[hand];
      bool has_model = g_CurrentPlayer->hand_item[hand] != ITEM_UNARMED &&
                       g_CurrentPlayer->hand_item[hand] != ITEM_TANKSHELLS &&
                       g_CurrentPlayer->hand_item[hand] != ITEM_SUIT_LF_HAND &&
                       model->obj == header && header->RootNode != NULL;

      write_u8(stream, has_model);
      if (has_model) {
        practice_states_save_model_animation(stream, model);
        save_hand_gunfire_nodes(stream, model);
      }
    }
  }

  /* 7. Head-bob and third-person Bond model animation/root transforms. */
  write_u8(stream, g_CurrentPlayer->model != NULL);
  if (g_CurrentPlayer->model != NULL) {
    practice_states_save_model_animation(stream,
                                         (Model *)&g_CurrentPlayer->model);
  }
  write_u8(stream, g_CurrentPlayer->ptr_char_objectinstance != NULL);
  if (g_CurrentPlayer->ptr_char_objectinstance != NULL) {
    practice_states_save_model_animation(
        stream, g_CurrentPlayer->ptr_char_objectinstance);
  }

  /* Gameplay runs before rendering, and reads both current camera matrices for
   * bullet tracing/culling plus the previous matrices for aim correction. The
   * pointers target the rotating graphics arena, so save their values. */
  {
    Mtxf *matrices[4] = {
        g_CurrentPlayer->field_10CC, g_CurrentPlayer->field_10D4,
        g_CurrentPlayer->field_10E8, g_CurrentPlayer->field_10EC};
    s32 matrix;

    for (matrix = 0; matrix < 4; matrix++) {
      write_u8(stream, matrices[matrix] != NULL);
      if (matrices[matrix] != NULL) {
        if (matrix >= 2 && matrices[matrix - 2] != NULL) {
          write_delta_matrix(stream, matrices[matrix], matrices[matrix - 2]);
        } else {
          write_sparse_matrix(stream, matrices[matrix]);
        }
      }
    }
    write_u8(stream, g_CurrentPlayer->projmatrixf != NULL);
    if (g_CurrentPlayer->projmatrixf != NULL) {
      write_sparse_matrix(stream, g_CurrentPlayer->projmatrixf);
    }
    write_u8(stream, g_CurrentPlayer->field_10E0 != 0);
    if (g_CurrentPlayer->field_10E0 != 0) {
      write_zero_rle(stream, (u8 *)g_CurrentPlayer->field_10E0, sizeof(Mtx));
    }
  }
}

static void load_current_player_state(StateStream *stream,
                                      bool force_model_rebuild) {
  textoverride *live_textoverrides;
  RenderPosView *live_hand_render_pos[2];
  s32 live_hand_matrix_count[2];
  ITEM_IDS live_hand_item[2];
  s32 saved_curblendpos[2];
  f32 saved_dampt[2];
  f32 saved_blendscale1[2];
  s32 saved_sideflag[2];
  f32 saved_gunsync;
  f32 saved_syncchange;
  f32 saved_synccount;
  s32 saved_syncoffset;
  f32 saved_field_107C;
  f32 saved_field_1080;
  s32 saved_equipcuritem;
  bool saved_had_model;
  s32 hand;

  if (g_CurrentPlayer == NULL) {
    return;
  }

  /* 1. Backup live-only text overrides. */
  live_textoverrides = g_CurrentPlayer->textoverrides;
  for (hand = 0; hand < 2; hand++) {
    ModelFileHeader *header =
        (ModelFileHeader *)g_CurrentPlayer->hands[hand].field_B70;
    live_hand_render_pos[hand] =
        (RenderPosView *)g_CurrentPlayer->hands[hand].field_B74;
    live_hand_matrix_count[hand] = header != NULL ? header->numMatrices : 0;
    live_hand_item[hand] = g_CurrentPlayer->hand_item[hand];
  }

  /* 2. Read player blocks directly into live struct. */
  saved_had_model = load_player_state_direct(stream, g_CurrentPlayer);
  saved_equipcuritem = g_CurrentPlayer->equipcuritem;
  saved_gunsync = g_CurrentPlayer->gunsync;
  saved_syncchange = g_CurrentPlayer->syncchange;
  saved_synccount = g_CurrentPlayer->synccount;
  saved_syncoffset = g_CurrentPlayer->syncoffset;
  saved_field_107C = g_CurrentPlayer->field_107C;
  saved_field_1080 = g_CurrentPlayer->field_1080;
  for (hand = 0; hand < 2; hand++) {
    saved_curblendpos[hand] = g_CurrentPlayer->hands[hand].curblendpos;
    saved_dampt[hand] = g_CurrentPlayer->hands[hand].dampt;
    saved_blendscale1[hand] = g_CurrentPlayer->hands[hand].blendscale1;
    saved_sideflag[hand] = g_CurrentPlayer->hands[hand].sideflag;
  }

  /* 3. Read and apply bond helper section. */
  {
    g_CurrentPlayer->room_pointer = get_tile_by_offset(read_u32(stream));
    g_CurrentPlayer->field_488.current_tile_ptr =
        get_tile_by_offset(read_u32(stream));
    g_CurrentPlayer->field_488.current_tile_ptr_for_portals =
        get_tile_by_offset(read_u32(stream));
    g_CurrentPlayer->previous_collision_info.current_tile_ptr =
        get_tile_by_offset(read_u32(stream));
    g_CurrentPlayer->previous_collision_info.current_tile_ptr_for_portals =
        get_tile_by_offset(read_u32(stream));
    g_CurrentPlayer->field_2A70 = get_tile_by_offset(read_u32(stream));

    g_CurrentPlayer->autoaim_target_y =
        get_enabled_prop_by_index(read_u32(stream));
    g_CurrentPlayer->autoaim_target_x =
        get_enabled_prop_by_index(read_u32(stream));

    /* Recalculate derived look parameters. */
    bondviewApplyVertaTheta();

    /* Sync player's world PropRecord. */
    if (read_u8(stream)) {
      coord3d prop_pos;
      s32 prop_stan_offset;
      u8 prop_rooms[4];

      read_bytes(stream, &prop_pos, sizeof(coord3d));
      prop_stan_offset = read_u32(stream);
      prop_rooms[0] = read_u8(stream);
      prop_rooms[1] = read_u8(stream);
      prop_rooms[2] = read_u8(stream);
      prop_rooms[3] = read_u8(stream);

      if (g_CurrentPlayer->prop != NULL) {
        g_CurrentPlayer->prop->pos = prop_pos;
        g_CurrentPlayer->prop->stan = get_tile_by_offset(prop_stan_offset);
        g_CurrentPlayer->prop->rooms[0] = prop_rooms[0];
        g_CurrentPlayer->prop->rooms[1] = prop_rooms[1];
        g_CurrentPlayer->prop->rooms[2] = prop_rooms[2];
        g_CurrentPlayer->prop->rooms[3] = prop_rooms[3];
      } else {
        g_CurrentPlayer->prop = chrpropAllocate();
        if (g_CurrentPlayer->prop != NULL) {
          g_CurrentPlayer->prop->obj = NULL;
          g_CurrentPlayer->prop->type = PROP_TYPE_VIEWER;
          g_CurrentPlayer->prop->pos = prop_pos;
          g_CurrentPlayer->prop->stan = get_tile_by_offset(prop_stan_offset);
          g_CurrentPlayer->prop->rooms[0] = prop_rooms[0];
          g_CurrentPlayer->prop->rooms[1] = prop_rooms[1];
          g_CurrentPlayer->prop->rooms[2] = prop_rooms[2];
          g_CurrentPlayer->prop->rooms[3] = prop_rooms[3];
          chrpropActivate(g_CurrentPlayer->prop);
          chrpropEnable(g_CurrentPlayer->prop);
        }
      }
    } else {
      if (g_CurrentPlayer->prop != NULL) {
        release_player_presence_prop(g_CurrentPlayer->prop);
        g_CurrentPlayer->prop = NULL;
      }
    }
  }

  /* 4. Re-initialize inventory, then read and apply inventory section. */
  {
    InvItem *first_item = NULL;
    InvItem *previous_item = NULL;
    s32 num_inv_items;
    s32 i;

    bondinvReinitInv();
    g_CurrentPlayer->textoverrides = live_textoverrides;

    num_inv_items = read_u32(stream);

    for (i = 0; i < num_inv_items; i++) {
      InvItem *item = NULL;
      s32 type = read_u32(stream);
      s32 weapon_right = read_u32(stream);
      s32 weapon_left = read_u32(stream);
      s32 prop_index = read_u32(stream);

      if (i < g_CurrentPlayer->equipmaxitems) {
        item = &g_CurrentPlayer->p_itemcur[i];
      }

      if (item == NULL) {
        practiceLogError("Inventory state has too many items (%d > %d)",
                         num_inv_items, g_CurrentPlayer->equipmaxitems);
        continue;
      }

      item->type = type;
      if (type == INV_ITEM_WEAPON) {
        item->type_inv_item.type_weap.weapon = weapon_right;
      } else if (type == INV_ITEM_DUAL) {
        item->type_inv_item.type_dual.weapon_right = weapon_right;
        item->type_inv_item.type_dual.weapon_left = weapon_left;
      } else if (type == INV_ITEM_PROP) {
        PropRecord *prop = get_prop_by_index(prop_index);
        item->type_inv_item.type_prop.prop = prop;
        if (prop == NULL) {
          practiceLogError("Could not restore inventory prop %d", prop_index);
        }
      } else {
        practiceLogError("Invalid inventory item type %d", type);
        item->type = -1;
        continue;
      }

      item->prev = previous_item;
      if (previous_item != NULL) {
        previous_item->next = item;
      } else {
        first_item = item;
      }
      previous_item = item;
    }

    if (first_item != NULL) {
      first_item->prev = previous_item;
      previous_item->next = first_item;
    }
    g_CurrentPlayer->ptr_inventory_first_in_cycle = first_item;

    /* bondinvReinitInv resets this to unarmed. Restore the exact saved index
     * after rebuilding the list in its saved cyclic order. Recomputing it from
     * the current weapon loses transient selection state and can make a held
     * trigger fire twice. */
    g_CurrentPlayer->equipcuritem = saved_equipcuritem;
  }

  /* 5. Restore hand weapon model loading and logical animation triggers. */
  {
    for (hand = 0; hand < 2; hand++) {
      ITEM_IDS saved_model_item = g_CurrentPlayer->hand_item[hand];
      s32 saved_hand_invisible = g_CurrentPlayer->hand_invisible[hand];
      ITEM_IDS saved_pending_item = g_CurrentPlayer->field_2A44[hand];
      s32 saved_lock_hand_model = g_CurrentPlayer->lock_hand_model[hand];
      s32 saved_weapon_animation =
          g_CurrentPlayer->hands[hand].weapon_current_animation;
      s32 saved_watch_animation_step =
          g_CurrentPlayer->step_in_view_watch_animation;
      Model *live_model = (Model *)&g_CurrentPlayer->hands[hand].field_B68;
      ModelFileHeader *live_header =
          &g_CurrentPlayer->copy_of_body_obj_header[hand];
      bool can_reuse_model = saved_model_item == live_hand_item[hand] &&
                             live_model->obj == live_header &&
                             live_header->RootNode != NULL;

      /* Tank shells select the tank firing path but have no standalone
       * first-person hand model that can be rebuilt with the generic item
       * loader. The tank renderer owns its visuals. */
      if (saved_model_item != ITEM_UNARMED &&
          saved_model_item != ITEM_TANKSHELLS && !can_reuse_model) {
        g_CurrentPlayer->hand_invisible[hand] = -3;
        g_CurrentPlayer->field_2A44[hand] = saved_model_item;
        g_CurrentPlayer->hand_item[hand] = ITEM_UNARMED;
        g_CurrentPlayer->hands[hand].weapon_current_animation = 0;
        g_CurrentPlayer->lock_hand_model[hand] = 0;
        used_to_load_1st_person_model_on_demand(hand);
        if (saved_model_item == ITEM_SUIT_LF_HAND) {
          /* The left suit hand is the watch model at player offset 0x230, not
           * the ordinary first-person model embedded in hands[GUNLEFT]. */
          sub_GAME_7F07E7CC();
        } else {
          Model *model = (Model *)&g_CurrentPlayer->hands[hand].field_B68;
          ModelFileHeader *header =
              &g_CurrentPlayer->copy_of_body_obj_header[hand];
          u32 *rw_data = (u32 *)&g_CurrentPlayer->hands[hand].field_B88;
          RenderPosView *render_pos;
          s32 matrix;

          modelCalculateRwDataLen(header);
          modelInit(model, header, rw_data);
          if (live_hand_render_pos[hand] != NULL &&
              live_hand_matrix_count[hand] == header->numMatrices &&
              is_active_gfx_range(live_hand_render_pos[hand],
                                  header->numMatrices *
                                      sizeof(RenderPosView))) {
            render_pos = live_hand_render_pos[hand];
          } else {
            render_pos =
                dynAllocate(header->numMatrices * sizeof(RenderPosView));
          }
          for (matrix = 0; matrix < header->numMatrices; matrix++) {
            matrix_4x4_set_identity(&render_pos[matrix].pos);
          }
          model->render_pos = render_pos;
          sub_GAME_7F05E978(model, 1);
          sub_GAME_7F05EA94(model, g_CurrentPlayer->hands[hand].field_87E);
        }
      }

      /* Model loading mutates the hand transition state to make the new model
       * immediately visible. Restore the saved logical state so the next gun
       * tick continues from the exact saved transition. */
      g_CurrentPlayer->hand_invisible[hand] = saved_hand_invisible;
      g_CurrentPlayer->hand_item[hand] = saved_model_item;
      g_CurrentPlayer->field_2A44[hand] = saved_pending_item;
      g_CurrentPlayer->lock_hand_model[hand] = saved_lock_hand_model;
      g_CurrentPlayer->hands[hand].weapon_current_animation =
          saved_weapon_animation;
      g_CurrentPlayer->step_in_view_watch_animation =
          saved_watch_animation_step;
      g_CurrentPlayer->hands[hand].audioHandle = NULL;
    }
  }

  /* First-person model loading reinitializes gun-sway accumulators that were
   * already restored by the direct player blocks. These values control when
   * bgunCalculateBlend consumes RNG, so reinstall the saved values after the
   * model allocation graph has been rebuilt. */
  g_CurrentPlayer->gunsync = saved_gunsync;
  g_CurrentPlayer->syncchange = saved_syncchange;
  g_CurrentPlayer->synccount = saved_synccount;
  g_CurrentPlayer->syncoffset = saved_syncoffset;
  g_CurrentPlayer->field_107C = saved_field_107C;
  g_CurrentPlayer->field_1080 = saved_field_1080;
  for (hand = 0; hand < 2; hand++) {
    g_CurrentPlayer->hands[hand].curblendpos = saved_curblendpos[hand];
    g_CurrentPlayer->hands[hand].dampt = saved_dampt[hand];
    g_CurrentPlayer->hands[hand].blendscale1 = saved_blendscale1[hand];
    g_CurrentPlayer->hands[hand].sideflag = saved_sideflag[hand];
  }

  /* 6. Restore canonical float hand matrices into the current live buffers. */
  {
    s32 hand;

    practice_clear_loaded_hand_matrices_float();
    for (hand = 0; hand < 2; hand++) {
      ModelFileHeader *header =
          (ModelFileHeader *)g_CurrentPlayer->hands[hand].field_B70;
      RenderPosView *matrices =
          (RenderPosView *)g_CurrentPlayer->hands[hand].field_B74;
      s32 saved_count = read_u16(stream);
      u32 size = saved_count * sizeof(Mtx);

      if (saved_count > 0 && saved_count <= MAX_SAVED_HAND_RENDER_MATRICES &&
          header != NULL && matrices != NULL &&
          header->numMatrices == saved_count) {
        read_bytes(stream, matrices, size);
        practice_mark_loaded_hand_matrices_float(hand);
      } else if (size > 0) {
        stream_seek(stream,
                    stream->base_address + stream->total_processed + size);
      }
    }
  }

  /* 7. Restore the logical watch animation after rebuilding its allocation
   * graph in the hand-model section above. */
  {
    bool has_watch_model = read_u8(stream);
    Model *watch_model =
        (Model *)&g_CurrentPlayer->something_with_watch_object_instance;

    if (has_watch_model) {
      practice_states_load_model_animation(
          stream, g_CurrentPlayer->hand_item[GUNLEFT] == ITEM_SUIT_LF_HAND
                      ? watch_model
                      : NULL);
    }
  }

  /* 8. Restore first-person weapon animation progress after rebuilding the
   * corresponding embedded Model and its RW data. */
  {
    s32 hand;

    for (hand = 0; hand < 2; hand++) {
      bool has_model = read_u8(stream);
      Model *model = (Model *)&g_CurrentPlayer->hands[hand].field_B68;
      ModelFileHeader *header = &g_CurrentPlayer->copy_of_body_obj_header[hand];

      if (has_model) {
        Model *valid_model =
            model->obj == header && header->RootNode != NULL ? model : NULL;
        practice_states_load_model_animation(stream, valid_model);
        load_hand_gunfire_nodes(stream, valid_model);
      }
    }
  }

  /* 9. Restore head-bob and third-person model animation/root transforms.
   * This must follow hand-model loading because both use the player's weapon
   * buffers; rebuilding a hand model can invalidate the intro's Bond model. */
  reconcile_current_player_model_presence(saved_had_model, force_model_rebuild);
#ifdef DEV
  validate_current_player_model_invariant(saved_had_model);
#endif
  if (read_u8(stream)) {
    /* The intro can reuse the first-person weapon buffer that backs the live
     * gait model before a late-level state is loaded. Rebuild the embedded
     * Model's fixed allocation graph, then apply the saved logical animation
     * and root transform below. */
    animInit((Model *)&g_CurrentPlayer->model, &player_gait_object_header,
             &g_CurrentPlayer->field_654);
    modelSetScale((Model *)&g_CurrentPlayer->model, IDO_POINT_ONE);
    practice_states_load_model_animation(stream,
                                         (Model *)&g_CurrentPlayer->model);
  }
  if (read_u8(stream)) {
    practice_states_load_model_animation(
        stream, g_CurrentPlayer->ptr_char_objectinstance);
  }
  if (g_CurrentPlayer->ptr_char_objectinstance != NULL) {
    /* The animation payload includes root position data, so bind the restored
     * third-person model to the physical viewer prop after loading it. During
     * intro cameras player->pos is the camera, not Bond's world position. */
    setsuboffset(g_CurrentPlayer->ptr_char_objectinstance,
                 &g_CurrentPlayer->prop->pos);
    setsubroty(g_CurrentPlayer->ptr_char_objectinstance,
               get_curplay_horizontal_rotation_in_degrees());
  }

  {
    Mtxf matrices[4];
    Mtxf *present[4];
    s32 matrix;

    for (matrix = 0; matrix < 4; matrix++) {
      present[matrix] = read_u8(stream) ? &matrices[matrix] : NULL;
      if (present[matrix] != NULL) {
        if (matrix >= 2 && present[matrix - 2] != NULL) {
          read_delta_matrix(stream, present[matrix], present[matrix - 2]);
        } else {
          read_sparse_matrix(stream, present[matrix]);
        }
      }
    }
    practice_set_loaded_camera_matrices(present[0], present[1], present[2],
                                        present[3]);

    if (read_u8(stream)) {
      read_sparse_matrix(stream, &matrices[0]);
      practice_set_loaded_projection_matrix(&matrices[0]);
    } else {
      practice_set_loaded_projection_matrix(NULL);
    }
    if (read_u8(stream)) {
      read_zero_rle(stream, (u8 *)&matrices[0], sizeof(Mtx));
      practice_set_loaded_room_projection_matrix((Mtx *)&matrices[0]);
    } else {
      practice_set_loaded_room_projection_matrix(NULL);
    }
  }

  /* 9. Re-generate watch menu GDLs. */
  {
    extern Gfx *sub_GAME_7F0A3330(Gfx * arg0, void *arg1, s32 arg2);
    extern void sub_GAME_7F0A2F30(struct damage_display_parent * arg0, s32 arg1,
                                  s32 arg2, f32 arg3);
    extern struct WatchVertex *setup_watch_rectangles(
        struct WatchVertex * vtx, s32 startx, s32 startz, s32 width, s32 height,
        s32 horizontal_offset, s32 vertical_offset);
    extern Gfx *sub_GAME_7F0A3B40(Gfx * gdl, s32 * arg1);

    sub_GAME_7F0A2F30(g_CurrentPlayer->armor_display_values, 0x2e, 1,
                      get_BONDdata_watch_armor());
    sub_GAME_7F0A2F30(g_CurrentPlayer->health_display_values, 0x2e, -1,
                      bondviewGetCurrentPlayerHealth());
    sub_GAME_7F0A3330(
        (Gfx *)&g_CurrentPlayer->watch_body_armor_bar_gdl,
        (void *)OS_K0_TO_PHYSICAL(&g_CurrentPlayer->armor_display_values),
        0x2E);
    sub_GAME_7F0A3330(
        (Gfx *)&g_CurrentPlayer->watch_health_bar_gdl,
        (void *)OS_K0_TO_PHYSICAL(&g_CurrentPlayer->health_display_values),
        0x2E);

    {
      Gfx *ptr_b = g_CurrentPlayer->buffer_for_watch_greenbackdrop_DL;
      struct WatchVertex *ptr_a =
          &g_CurrentPlayer->buffer_for_watch_greenbackdrop_vertices->vtx[0];
      s32 i;
      for (i = 0;
           i < (WATCH_NUMBER_SCREENS * WATCH_SCREEN_SELECT_RECTANGLE_HSTEP);
           i += WATCH_SCREEN_SELECT_RECTANGLE_HSTEP) {
        struct WatchVertex *ptr_copy = ptr_a;
        ptr_a = setup_watch_rectangles(ptr_a, i, 0, 0x64, 0x14, -0x12B, 0x136);
        ptr_b = sub_GAME_7F0A3B40(ptr_b, (s32 *)OS_K0_TO_PHYSICAL(ptr_copy));
      }
      gSPEndDisplayList(ptr_b);

      {
        struct WatchVertex *ptr_copy =
            (struct WatchVertex *)
                g_CurrentPlayer->buffer_for_watch_static_vertices;
        ptr_b = g_CurrentPlayer->buffer_for_watch_static_DL;
        setup_watch_rectangles(
            &g_CurrentPlayer->buffer_for_watch_static_vertices->vtx[0], 0, 0,
            0x398, 0x14, -0x1CC, 0);
        ptr_b = sub_GAME_7F0A3B40(ptr_b, (s32 *)OS_K0_TO_PHYSICAL(ptr_copy));
        gSPEndDisplayList(ptr_b);
      }
    }
  }
}

bool save_viewer_players_state(StateStream *stream) {
  s32 original_player = get_cur_playernum();
  s32 player_count = getPlayerCount();
  s32 saved_count = 0;
  s32 i;

  for (i = 0; i < player_count; i++) {
    if (g_playerPointers[i] != NULL) {
      saved_count++;
    }
  }

  write_u8(stream, saved_count);

  for (i = 0; i < player_count; i++) {
    struct player *player = g_playerPointers[i];
    s16 viewer_prop_index;

    if (player == NULL) {
      continue;
    }

    viewer_prop_index = get_prop_index(player->prop);
    if (player->prop != NULL && player->prop->type != PROP_TYPE_VIEWER) {
      practiceLogWarn("Player %d has non-viewer prop type %d", i,
                      player->prop->type);
      set_cur_player(original_player);
      return FALSE;
    }

    write_u8(stream, i);
    write_u16(stream, viewer_prop_index);

    set_cur_player(i);
    save_current_player_state(stream);
    write_u8(stream, current_player_model_is_valid());
    if (current_player_model_is_valid()) {
      ChrRecord *viewer_chr = g_CurrentPlayer->prop->chr;
      save_chr_record(stream, viewer_chr);
    }
  }

  set_cur_player(original_player);
  return TRUE;
}

bool load_viewer_players_state(StateStream *stream, bool force_model_rebuild) {
  s32 live_player_count = getPlayerCount();
  s32 saved_count = read_u8(stream);
  s32 i;

  for (i = 0; i < saved_count; i++) {
    s32 player_index = read_u8(stream);
    s16 viewer_prop_index = (s16)read_u16(stream);
    PropRecord *viewer_prop = NULL;

    if (player_index < 0 || player_index >= live_player_count ||
        g_playerPointers[player_index] == NULL) {
      practiceLogWarn("Invalid saved player index %d", player_index);
      return FALSE;
    }

    if (viewer_prop_index >= 0) {
      viewer_prop = get_enabled_prop_by_index(viewer_prop_index);
      if (viewer_prop != NULL && viewer_prop->type != PROP_TYPE_VIEWER) {
        practiceLogWarn("Player %d prop %d has unexpected type %d",
                        player_index, viewer_prop_index, viewer_prop->type);
        return FALSE;
      }

      /*
       * While prop addition/removal is disabled, a saved viewer can be absent
       * when loading from a cutscene. Leave the live pointer in place so the
       * player loader can update it or allocate a replacement from its saved
       * position data.
       */
      if (viewer_prop != NULL) {
        PropRecord *live_prop = g_playerPointers[player_index]->prop;
        s32 other_player;

        if (live_prop != NULL && live_prop != viewer_prop) {
          for (other_player = 0; other_player < live_player_count;
               other_player++) {
            if (other_player != player_index &&
                g_playerPointers[other_player] != NULL &&
                g_playerPointers[other_player]->prop == live_prop) {
              practiceLogError(
                  "Player %d stale prop %d is shared with player %d",
                  player_index, get_prop_index(live_prop), other_player);
              assert(FALSE);
              return FALSE;
            }
          }
          release_player_presence_prop(live_prop);
        }
        g_playerPointers[player_index]->prop = viewer_prop;
      }
    }

    set_cur_player(player_index);
    load_current_player_state(stream, force_model_rebuild);

    if (read_u8(stream)) {
      ChrRecord *viewer_chr =
          g_CurrentPlayer->prop != NULL ? g_CurrentPlayer->prop->chr : NULL;
      ChrAllocationState allocation;
      ChrAttachmentIndices attachments;

      if (viewer_chr == NULL || viewer_chr->model == NULL) {
        practiceLogError("Viewer CHR missing player=%d chr=%08x", player_index,
                         viewer_chr);
        return FALSE;
      }
      load_chr_allocation_state(stream, &allocation);
      if (allocation.slot_index < 0 ||
          allocation.slot_index >= g_NumChrSlots) {
        practiceLogError("Viewer CHR slot invalid player=%d saved=%d",
                         player_index, allocation.slot_index);
        return FALSE;
      }
      if (allocation.slot_index != viewer_chr - g_ChrSlots) {
        ChrRecord *target_chr = &g_ChrSlots[allocation.slot_index];

        if (target_chr->model != NULL) {
          practiceLogError(
              "Viewer CHR slot occupied player=%d saved=%d live=%d",
              player_index, allocation.slot_index, viewer_chr - g_ChrSlots);
          return FALSE;
        }
        *target_chr = *viewer_chr;
        target_chr->prop = g_CurrentPlayer->prop;
        target_chr->model->chr = target_chr;
        g_CurrentPlayer->prop->chr = target_chr;
        bzero(viewer_chr, sizeof(*viewer_chr));
        viewer_chr->unk180[0].unk00 = -1;
        viewer_chr->unk180[1].unk00 = -1;
        viewer_chr = target_chr;
      }
      load_chr_record(stream, viewer_chr, &attachments);
    }
  }

  return TRUE;
}
