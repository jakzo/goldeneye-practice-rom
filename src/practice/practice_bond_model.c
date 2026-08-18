#include "practice_bond_model.h"
#include "bg.h"
#include "bondconstants.h"
#include "chr.h"
#include "chr_b.h"
#include "chrlv.h"
#include "chrobjdata.h"
#include "chrobjhandler.h"
#include "dyn.h"
#include "memp.h"
#include "objecthandler.h"
#include "player.h"
#include "player_2.h"
#include "practice_config.h"
#include "practice_render.h"
#include <os_extension.h>

#define BOND_REVEAL_BODY BODY_Brosnan_Tuxedo
#define BOND_REVEAL_HEAD HEAD_Male_Brosnan_Tuxedo
#define BOND_CHR_WIDTH 20.0f
#define BOND_CHR_HEIGHT 185.0f
#define BOND_MODEL_UNK00 0x0a
#define BOND_ANIM_RATE 0.5f
/* Uncompressed sizes of CdjbondZ / CheadbrosnanZ (the .bin assets, before
 * 1172 compression). fileLoad decompresses into leftover and then shrinks
 * the stage-pool entry to this size. */
#define BOND_BODY_UNCOMPRESSED 28704
#define BOND_HEAD_UNCOMPRESSED 9024
/* sub_GAME_7F0762E0 rewrites display lists after decompress and grows the
 * allocation with fileSetSize. */
#define BOND_GDL_PAD 0x4000
/* Leave leftover for room streaming and other first-time fileLoad. */
#define BOND_LEFTOVER_RESERVE 0x20000

extern MemoryPool g_mempPools[];
extern void sub_GAME_7F06B248(void *arg0);
extern void clear_aircraft_model_obj(Model *model);

static struct {
  PropRecord prop;
  ChrRecord chr;
  coord3d model_position;
  f32 heading;
  Model *deferred;
  s32 player;
} g_Bond;

static s32 bond_enabled(void) { return practice.show_bond_in_cam; }

static s32 bond_instance_linked(void) {
  return g_Bond.prop.chr == &g_Bond.chr && g_Bond.chr.prop == &g_Bond.prop;
}

static s32 bond_instance_ready(void) {
  return bond_instance_linked() && g_Bond.chr.model != NULL;
}

static s32 room_is_valid(s32 room) {
  return room > 0 && room < g_MaxNumRooms && room < MAXROOMCOUNT;
}

static s32 align16(s32 value) { return (value + 0xf) & ~0xf; }

static s32 player_has_third_person_bond(void) {
  return g_CurrentPlayer != NULL && g_CurrentPlayer->prop != NULL &&
         g_CurrentPlayer->prop->chr != NULL &&
         g_CurrentPlayer->prop->chr->model != NULL &&
         g_CurrentPlayer->prop->chr != &g_Bond.chr;
}

static void apply_bond_animation(Model *model) {
  Model *player_model;
  ModelRwData_HeaderRecord *root_data;

  player_model = (Model *)&g_CurrentPlayer->model;
  if (objecthandlerGetModelAnim(player_model) == NULL)
    return;

  /* The first modelSetAnimation stores hip height in unk34.y. Later calls
   * rewrite that as pos.y - ground and will lift the model if they run
   * every live frame. */
  modelSetAnimation(model, objecthandlerGetModelAnim(player_model),
                    objecthandlerGetModelGunhand(player_model),
                    objecthandlerGetModelField28(player_model), BOND_ANIM_RATE,
                    0.0f);

  root_data = (ModelRwData_HeaderRecord *)modelGetNodeRwData(
      model, model->obj->RootNode);
  root_data->ground = g_CurrentPlayer->field_70;
}

static void sync_bond(void) {
  Model *model;
  ModelRwData_HeaderRecord *root_data;

  if (g_CurrentPlayer == NULL || g_CurrentPlayer->prop == NULL ||
      g_Bond.prop.chr != &g_Bond.chr)
    return;

  g_Bond.prop.pos = g_CurrentPlayer->prop->pos;
  g_Bond.prop.pos.y = g_CurrentPlayer->field_70;
  g_Bond.prop.stan = g_CurrentPlayer->prop->stan;
  g_Bond.model_position = g_Bond.prop.pos;
  g_Bond.prop.rooms[0] = g_CurrentPlayer->prop->stan != NULL
                             ? g_CurrentPlayer->prop->stan->room
                             : 0xff;
  g_Bond.prop.rooms[1] = 0xff;
  g_Bond.heading = get_curplay_horizontal_rotation_in_degrees();
  g_Bond.chr.ground = g_CurrentPlayer->field_70;
  g_Bond.chr.prevpos = g_Bond.model_position;
  set_color_shading_from_tile(&g_Bond.prop, &g_Bond.chr.nextcol);
  g_Bond.chr.shadecol = g_Bond.chr.nextcol;

  model = g_Bond.chr.model;
  if (model == NULL)
    return;

  setsuboffset(model, &g_Bond.model_position);
  setsubroty(model, g_Bond.heading);
  root_data = (ModelRwData_HeaderRecord *)modelGetNodeRwData(
      model, model->obj->RootNode);
  root_data->ground = g_CurrentPlayer->field_70;
}

static s32 bond_body_headers_resident(void) {
  return c_item_entries[BOND_REVEAL_BODY].header != NULL &&
         c_item_entries[BOND_REVEAL_BODY].header->RootNode != NULL &&
         c_item_entries[BOND_REVEAL_HEAD].header != NULL &&
         c_item_entries[BOND_REVEAL_HEAD].header->RootNode != NULL;
}

static s32 stage_pool_ready(void) {
  return g_mempPools[MEMPOOL_STAGE].start != g_mempPools[MEMPOOL_STAGE].end &&
         g_mempPools[MEMPOOL_STAGE].pos != 0;
}

static s32 bond_load_bytes_needed(void) {
  return align16(BOND_BODY_UNCOMPRESSED + BOND_GDL_PAD) +
         align16(BOND_HEAD_UNCOMPRESSED + BOND_GDL_PAD) +
         BOND_LEFTOVER_RESERVE;
}

/* Vanilla fileLoad takes leftover as decompress workspace, then
 * mempAddEntryOfSizeToBank / fileSetSize shrink it back to the uncompressed
 * model plus rewritten display lists. Only call this when leftover can hold
 * both files and still leave a reserve; a mid-mission load with ~16KB left
 * overlaps the compressed source and dest and hangs in decompressdata. */
static s32 try_load_bond_headers(void) {
  s32 leftover;

  if (!bond_enabled())
    return FALSE;

  if (bond_body_headers_resident())
    return TRUE;

  if (!stage_pool_ready() || g_CameraMode == CAMERAMODE_SWIRL)
    return FALSE;

  leftover = mempGetBankSizeLeft(MEMPOOL_STAGE);
  if (leftover < bond_load_bytes_needed())
    return FALSE;

  load_body_head_if_not_loaded(BOND_REVEAL_BODY);
  leftover = mempGetBankSizeLeft(MEMPOOL_STAGE);
  if (leftover < align16(BOND_HEAD_UNCOMPRESSED + BOND_GDL_PAD) +
                     BOND_LEFTOVER_RESERVE)
    return FALSE;

  load_body_head_if_not_loaded(BOND_REVEAL_HEAD);
  return bond_body_headers_resident();
}

static void create_bond(void) {
  Model *model;

  if (!bond_enabled() || g_CurrentPlayer == NULL ||
      g_CurrentPlayer->prop == NULL)
    return;

  if (bond_instance_ready()) {
    sync_bond();
    return;
  }

  /* Intros, outros, death animations and other third-person cameras attach the
   * real Bond model to the player prop. A second body would overlap it. */
  if (player_has_third_person_bond())
    return;

  /* A previous PIP/freecam exit may still be holding the instance until the
   * last display list completes. Reuse it instead of failing the next create. */
  if (g_Bond.deferred != NULL) {
    model = g_Bond.deferred;
    g_Bond.deferred = NULL;
  } else if (bond_body_headers_resident()) {
    model = retrieve_header_for_body_and_head(BOND_REVEAL_BODY, BOND_REVEAL_HEAD,
                                              0);
    if (model == NULL)
      return;
  } else {
    return;
  }

  bzero(&g_Bond.prop, sizeof(g_Bond.prop));
  bzero(&g_Bond.chr, sizeof(g_Bond.chr));
  g_Bond.prop.type = PROP_TYPE_CHR;
  g_Bond.prop.chr = &g_Bond.chr;

  /* This is only the subset of character state consumed by rendering. It is
   * intentionally not created with init_GUARDdata_with_set_values: that
   * initializer performs collision placement and allocates a gameplay CHR. */
  g_Bond.chr.chrnum = -1;
  g_Bond.chr.headnum = BOND_REVEAL_HEAD;
  g_Bond.chr.bodynum = BOND_REVEAL_BODY;
  g_Bond.chr.actiontype = ACT_STAND;
  g_Bond.chr.fadealpha = 0xff;
  g_Bond.chr.flinchcnt = -1;
  g_Bond.chr.chrflags = CHRFLAG_04000000;
  g_Bond.chr.prop = &g_Bond.prop;
  g_Bond.chr.model = model;
  g_Bond.chr.chrwidth = BOND_CHR_WIDTH;
  g_Bond.chr.chrheight = BOND_CHR_HEIGHT;
  g_Bond.chr.unk180[0].unk00 = -1;
  g_Bond.chr.unk180[1].unk00 = -1;
  g_Bond.player = get_cur_playernum();

  /* The normal CHR callback derives root translation from gameplay movement
   * state and rewrites this model back to the player's eye-height position.
   * The synthetic body already has an explicit floor-level origin. */
  sub_GAME_7F06FF5C(model, 0);
  model->unk00 = BOND_MODEL_UNK00;
  model->chr = &g_Bond.chr;
  modelSetAnimPlaySpeed(model, animation_rate, 0.0f);
  sync_bond();
  apply_bond_animation(model);
}

static void destroy_bond(void) {
  Model *model = g_Bond.chr.model;

  if (bond_instance_linked()) {
    /* The display list may still reference this model's matrices when the
     * hotkey exits freecam. Forget the render wrapper now, but keep the model
     * instance reserved until the graphics task has completed. */
    g_Bond.chr.field_20 = NULL;
    if (model != NULL)
      g_Bond.deferred = model;
  }

  bzero(&g_Bond.chr, sizeof(g_Bond.chr));
  bzero(&g_Bond.prop, sizeof(g_Bond.prop));
}

static void release_deferred_model(s32 pending_gfx_tasks) {
  s32 previous_player;

  if (pending_gfx_tasks != 0 || g_Bond.deferred == NULL)
    return;

  clear_aircraft_model_obj(g_Bond.deferred);
  g_Bond.deferred = NULL;

  /* A rapid re-entry can occur before the previous model was safe to release.
   * Create the replacement as soon as the pool instance becomes available. */
  if (!bond_enabled() || g_CurrentPlayer == NULL || g_Bond.prop.chr != NULL)
    return;

  previous_player = get_cur_playernum();
  if (g_Bond.player >= 0 && g_Bond.player < getPlayerCount())
    set_cur_player(g_Bond.player);
  try_load_bond_headers();
  create_bond();
  set_cur_player(previous_player);
}

static s32 bond_room_id(void) {
  if (g_Bond.prop.rooms[0] != 0xff)
    return g_Bond.prop.rooms[0];
  if (g_Bond.prop.stan != NULL)
    return g_Bond.prop.stan->room;
  return -1;
}

static u8 force_bond_room_visible(void) {
  s32 room = bond_room_id();
  u8 saved;

  if (!room_is_valid(room))
    return 0xff;

  saved = g_BgRoomInfo[room].room_rendered;
  g_BgRoomInfo[room].room_rendered = 1;
  return saved;
}

static void restore_bond_room_visible(u8 saved) {
  s32 room = bond_room_id();

  if (saved == 0xff || !room_is_valid(room))
    return;

  g_BgRoomInfo[room].room_rendered = saved;
}

void practice_bond_model_reset(void) {
  /* Level setup has already rebuilt the character/model pools, so old-stage
   * pointers must only be forgotten here. */
  bzero(&g_Bond.prop, sizeof(g_Bond.prop));
  bzero(&g_Bond.chr, sizeof(g_Bond.chr));
  g_Bond.deferred = NULL;
  g_Bond.player = 0;
}

void practice_bond_model_tick(s32 pending_gfx_tasks) {
  release_deferred_model(pending_gfx_tasks);
  try_load_bond_headers();
}

void practice_bond_model_ensure(void) {
  if (!bond_enabled())
    return;

  if (bond_instance_ready()) {
    sync_bond();
    return;
  }

  /* Instantiates only when headers are already resident. fileLoad belongs in
   * try_load_bond_headers (after swirl / idle tick), never on the PIP render
   * path that calls this. */
  create_bond();
}

void practice_bond_model_load_and_ensure(void) {
  try_load_bond_headers();
  create_bond();
}

void practice_bond_model_prepare_state_load(void) {
  if (g_Bond.prop.chr != NULL || g_Bond.deferred != NULL) {
    destroy_bond();
    release_deferred_model(0);
  }
}

Gfx *practice_bond_model_render(Gfx *gdl, s32 force_room_visible) {
  ChrRecord *chr;
  Model *model;
  ModelRwData_HeaderRecord *root_data;
  u8 saved_room_rendered = 0xff;

  if (!bond_enabled() || player_has_third_person_bond() ||
      g_Bond.prop.chr == NULL)
    return gdl;

  chr = g_Bond.prop.chr;
  model = chr->model;
  if (model == NULL || chr->prop != &g_Bond.prop)
    return gdl;

  setsuboffset(model, &g_Bond.model_position);
  setsubroty(model, g_Bond.heading);
  root_data = (ModelRwData_HeaderRecord *)modelGetNodeRwData(
      model, model->obj->RootNode);
  root_data->ground = g_Bond.chr.ground;
  subcalcpos(model);

  /* Grenade cam sits well above the floor, so Bond's room is often not in
   * the PIP's rendered set. Mark it visible so the frustum test can run. */
  if (force_room_visible)
    saved_room_rendered = force_bond_room_visible();

  chrTickBeams(&g_Bond.prop);

  if (force_room_visible)
    restore_bond_room_visible(saved_room_rendered);

  if (chr->field_20 != NULL &&
      dynGetFreeVtx() >= PRACTICE_DYN_VTX_RESERVE) {
    gdl = chrRenderProp(&g_Bond.prop, gdl, FALSE);
    gdl = chrRenderProp(&g_Bond.prop, gdl, TRUE);
  } else if (chr->field_20 != NULL) {
    sub_GAME_7F06B248(chr->field_20);
    chr->field_20 = NULL;
  }

  return gdl;
}

Gfx *practice_bond_model_render_in_view(Gfx *gdl) {
  s32 room;
  s32 room_already_visible;

  if (!bond_enabled() || g_CurrentPlayer == NULL || g_Bond.prop.chr == NULL)
    return gdl;

  if (dynGetFreeVtx() < PRACTICE_DYN_VTX_RESERVE)
    return gdl;

  room = bond_room_id();
  room_already_visible = room_is_valid(room) &&
                         g_BgRoomInfo[room].room_rendered != 0;
  /* Force Bond's room into the PIP visibility set when it is not already
   * portal-visible (grenade cam looking down, or a distant follow cam). */
  return practice_bond_model_render(gdl, !room_already_visible);
}
