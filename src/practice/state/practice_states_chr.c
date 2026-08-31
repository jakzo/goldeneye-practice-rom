#include "practice_states_chr.h"
#include "bondhead.h"
#include "chr.h"
#include "chrai.h"
#include "chrobjhandler.h"
#include "initanitable.h"
#include "objecthandler.h"
#include "practice_states_utils.h"
#include "practice_ui.h"
#include <assert.h>
#include <bondconstants.h>
#include <snd.h>
#include <string.h>

extern PathRecord *pathFindById(s32 ID);
extern Vertex *sub_GAME_7F09BE4C(s32 vertexCount, s32 allocationType,
                                 void *allocationData, s32 arg3);
extern void sub_GAME_7F09C044(Vertex *vertices);
extern s32 dword_CODE_bss_8007A0D0;
extern s32 dword_CODE_bss_8007A0D4;
extern Vertex *dword_CODE_bss_8007A0E0;
extern s16 word_CODE_bss_8007A0F0;

typedef struct BloodVertexAllocation {
  Vertex *vertices;
  s32 unk04;
  s32 unk08;
  s16 count;
  s16 ref_count;
  s16 next;
  s16 previous;
} BloodVertexAllocation;

extern BloodVertexAllocation *dword_CODE_bss_8007A0E8;

#define CHR_COMBAT_HIDDEN_MASK                                                 \
  (CHRHIDDEN_FIRE_WEAPON_LEFT | CHRHIDDEN_FIRE_WEAPON_RIGHT |                  \
   CHRHIDDEN_FIRE_TRACER | CHRHIDDEN_MOVING | CHRHIDDEN_OFFSCREEN_PATROL)

#define CHR_LIFECYCLE_HIDDEN_MASK                                              \
  (CHRHIDDEN_DROP_HELD_ITEMS | CHRHIDDEN_ALERT_GUARD_RELATED |                 \
   CHRHIDDEN_REMOVE | CHRHIDDEN_BACKGROUND_AI | CHRHIDDEN_0400 |               \
   CHRHIDDEN_FREEZE)

#define CHR_FLINCH_HIDDEN_MASK CHRHIDDEN_RAND_FLINCH_MASK

#define CHR_DAMAGE_FLAGS_MASK CHRFLAG_INVINCIBLE

#define MAX_MODEL_BLOOD_NODES 128
#define MAX_MODEL_TRAVERSAL_DEPTH 64
#define MAX_MODEL_TRAVERSAL_NODES 512
#define CHR_COMMON_BUFFER_SIZE 512

typedef struct ChrCommonStream {
  StateStream base;
  u8 bytes[CHR_COMMON_BUFFER_SIZE];
  u32 size;
} ChrCommonStream;

static void chr_common_write(StateStream *stream, const void *src, u32 size) {
  ChrCommonStream *common = (ChrCommonStream *)stream;
  if (stream->total_processed + size > sizeof(common->bytes)) {
    practiceLogError("CHR common state exceeds its temporary buffer");
    assert(FALSE);
    return;
  }
  memcpy(common->bytes + stream->total_processed, src, size);
  stream->total_processed += size;
  if (stream->total_processed > common->size) {
    common->size = stream->total_processed;
  }
}

static void chr_common_read(StateStream *stream, void *dst, u32 size) {
  ChrCommonStream *common = (ChrCommonStream *)stream;
  if (stream->total_processed + size > common->size) {
    practiceLogError("CHR common state read exceeds its temporary buffer");
    assert(FALSE);
    return;
  }
  memcpy(dst, common->bytes + stream->total_processed, size);
  stream->total_processed += size;
}

static void chr_common_seek(StateStream *stream, u32 absolute_offset) {
  stream->total_processed = absolute_offset;
}

static void chr_common_flush(StateStream *stream) { (void)stream; }

static void chr_common_stream_init(ChrCommonStream *stream, u32 size) {
  stream->base.write_bytes = chr_common_write;
  stream->base.read_bytes = chr_common_read;
  stream->base.seek = chr_common_seek;
  stream->base.flush = chr_common_flush;
  stream->base.total_processed = 0;
  stream->base.base_address = 0;
  stream->size = size;
}

static void write_chr_common_rle(StateStream *stream, const u8 *src, u32 size) {
  u32 offset = 0;

  write_u16(stream, size);
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

static void read_chr_common_rle(StateStream *stream, ChrCommonStream *common) {
  u32 size = read_u16(stream);
  u32 offset = 0;

  if (size > sizeof(common->bytes)) {
    practiceLogError("Saved CHR common state is too large (%d)", size);
    assert(FALSE);
    size = sizeof(common->bytes);
  }
  chr_common_stream_init(common, size);
  while (offset < size) {
    u8 control = read_u8(stream);
    u32 count = (control & 0x7f) + 1;

    if (count > size - offset) {
      practiceLogError("CHR common state RLE exceeds destination size");
      assert(FALSE);
      count = size - offset;
    }
    if (control & 0x80) {
      bzero(common->bytes + offset, count);
    } else {
      read_bytes(stream, common->bytes + offset, count);
    }
    offset += count;
  }
}

static f32 normalize_chr_heading(f32 heading) {
  union {
    f32 value;
    u32 bits;
  } raw;
  u32 exponent;

  raw.value = heading;
  exponent = raw.bits & 0x7f800000;

  /*
   * Intro camguards can leave the root heading as a denormal (Runway uses
   * 0x000000a4).  It is effectively zero, but setsubroty's subtraction traps
   * on denormal input on the N64 FPU when the character is recreated.
   */
  if (exponent == 0)
    return 0.0f;

  if (exponent == 0x7f800000) {
    practiceLogError("CHR model heading is not finite: %08x", raw.bits);
    assert(FALSE);
    return 0.0f;
  }

  return heading;
}

#define CHR_BEHAVIOR_FLAGS_MASK 0xffffffffU

typedef struct FiringAnimationTableRef {
  struct weapon_firing_animation_table *table;
  u8 count;
} FiringAnimationTableRef;

static const FiringAnimationTableRef firing_animation_tables[] = {
    {rifle_firing_animation_group1, 1},
    {rifle_firing_animation_group2, 2},
    {rifle_firing_animation_group5, 2},
    {rifle_firing_animation_group3, 1},
    {rifle_firing_animation_group4, 1},
    {pistol_firing_animation_group1, 4},
    {pistol_firing_animation_group2, 2},
    {pistol_firing_animation_group3, 4},
    {pistol_firing_animation_group6, 4},
    {pistol_firing_animation_group4, 1},
    {pistol_firing_animation_group5, 1},
    {doubles_firing_animation_group1, 1},
    {doubles_firing_animation_group2, 2},
    {doubles_firing_animation_group3, 2},
    {crouched_rifle_firing_animation_group1, 1},
    {crouched_rifle_firing_animation_groupA, 1},
    {crouched_rifle_firing_animation_group2, 1},
    {crouched_rifle_firing_animation_group3, 1},
    {crouched_pistol_firing_animation_group1, 2},
    {crouched_pistol_firing_animation_group2, 3},
    {crouched_pistol_firing_animation_group3, 2},
    {crouched_doubles_firing_animation_group1, 2},
    {crouched_doubles_firing_animation_group2, 2},
    {crouched_doubles_firing_animation_group3, 2},
    {D_80030078, 20},
    {D_80030660, 10},
};

static s16
get_firing_animation_id(const struct weapon_firing_animation_table *animation) {
  s32 table;
  s32 row;

  if (animation == NULL) {
    return -1;
  }

  for (table = 0; table < (s32)(sizeof(firing_animation_tables) /
                                sizeof(firing_animation_tables[0]));
       table++) {
    for (row = 0; row < firing_animation_tables[table].count; row++) {
      if (animation == &firing_animation_tables[table].table[row]) {
        return (s16)((table << 8) | row);
      }
    }
  }

  return -1;
}

static struct weapon_firing_animation_table *
get_firing_animation_by_id(s16 id) {
  u8 table;
  u8 row;

  if (id < 0) {
    return NULL;
  }

  table = (u16)id >> 8;
  row = id & 0xff;

  if (table >= sizeof(firing_animation_tables) /
                   sizeof(firing_animation_tables[0]) ||
      row >= firing_animation_tables[table].count) {
    return NULL;
  }

  return &firing_animation_tables[table].table[row];
}

static bool is_supported_action(ACT_TYPE actiontype) {
  switch (actiontype) {
  case ACT_INIT:
  case ACT_STAND:
  case ACT_KNEEL:
  case ACT_ANIM:
  case ACT_DIE:
  case ACT_DEAD:
  case ACT_ARGH:
  case ACT_PREARGH:
  case ACT_ATTACK:
  case ACT_ATTACKWALK:
  case ACT_ATTACKROLL:
  case ACT_SIDESTEP:
  case ACT_JUMPOUT:
  case ACT_RUNPOS:
  case ACT_PATROL:
  case ACT_GOPOS:
  case ACT_SURRENDER:
  case ACT_LOOKATTARGET:
  case ACT_SURPRISED:
  case ACT_STARTALARM:
  case ACT_THROWGRENADE:
  case ACT_TURNDIR:
  case ACT_TEST:
  case ACT_BONDINTRO:
  case ACT_BONDDIE:
  case ACT_BONDMULTI:
  case ACT_NULL:
    return TRUE;
  default:
    return FALSE;
  }
}

static bool is_supported_chr_action(const ChrRecord *chr) {
  if (!is_supported_action(chr->actiontype)) {
    return FALSE;
  }

  switch (chr->actiontype) {
  case ACT_ATTACK:
    return get_firing_animation_id(chr->act_attack.animfloats) >= 0;
  case ACT_ATTACKWALK:
    return get_firing_animation_id(chr->act_attackwalk.animfloats) >= 0;
  case ACT_ATTACKROLL:
    return get_firing_animation_id(chr->act_attackroll.animfloats) >= 0;
  case ACT_BONDMULTI:
    return chr->act_bondmulti.unk2c == NULL ||
           get_firing_animation_id((struct weapon_firing_animation_table *)
                                       chr->act_bondmulti.unk2c) >= 0;
  default:
    return TRUE;
  }
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

static s16 get_waypoint_index(const waypoint *point) {
  s16 index;

  if (point == NULL || g_CurrentSetup.pathwaypoints == NULL) {
    return -1;
  }

  /* waypointFindRoute writes a NULL terminator but leaves later route-array
   * slots untouched. Some ACT_GOPOS records therefore contain stack junk in
   * unused slots; only exact pointers into the live waypoint table are valid.
   */
  for (index = 0; g_CurrentSetup.pathwaypoints[index].padID >= 0; index++) {
    if (point == &g_CurrentSetup.pathwaypoints[index]) {
      return index;
    }
  }
  return -1;
}

static waypoint *get_waypoint_by_index(s16 index) {
  if (index < 0 || g_CurrentSetup.pathwaypoints == NULL) {
    return NULL;
  }

  return &g_CurrentSetup.pathwaypoints[index];
}

static s16 get_patrol_path_id(const struct patrol_path *path) {
  const PathRecord *record = (const PathRecord *)path;
  return record != NULL ? record->ID : -1;
}

static struct patrol_path *get_patrol_path_by_id(s16 id) {
  return id >= 0 ? (struct patrol_path *)pathFindById(id) : NULL;
}

static void save_waydata(StateStream *stream, const struct waydata *waydata) {
  write_u8(stream, (u8)waydata->mode);
  write_u8(stream, (u8)waydata->unk01);
  write_u8(stream, (u8)waydata->unk02);
  write_u8(stream, (u8)waydata->unk03);
  write_bytes(stream, &waydata->pos, sizeof(coord3d));
  write_bytes(stream, &waydata->pos2, sizeof(coord3d));
  write_bytes(stream, &waydata->pos3, sizeof(coord3d));
  write_u32(stream, waydata->age);
  write_bytes(stream, &waydata->pos_copy, sizeof(coord3d));
  write_f32(stream, waydata->segdistdone);
  write_f32(stream, waydata->segdisttotal);
}

static void load_waydata(StateStream *stream, struct waydata *waydata) {
  waydata->mode = (s8)read_u8(stream);
  waydata->unk01 = (s8)read_u8(stream);
  waydata->unk02 = (s8)read_u8(stream);
  waydata->unk03 = (s8)read_u8(stream);
  read_bytes(stream, &waydata->pos, sizeof(coord3d));
  read_bytes(stream, &waydata->pos2, sizeof(coord3d));
  read_bytes(stream, &waydata->pos3, sizeof(coord3d));
  waydata->age = read_u32(stream);
  read_bytes(stream, &waydata->pos_copy, sizeof(coord3d));
  waydata->segdistdone = read_f32(stream);
  waydata->segdisttotal = read_f32(stream);
}

static void save_supported_action(StateStream *stream, const ChrRecord *chr) {
  s32 i;

  write_u8(stream, (u8)chr->actiontype);

  switch (chr->actiontype) {
  case ACT_STAND:
    write_u32(stream, chr->act_stand.prestand);
    write_u32(stream, chr->act_stand.face_entitytype);
    write_u32(stream, chr->act_stand.face_entityid);
    write_u32(stream, chr->act_stand.reaim);
    write_u32(stream, chr->act_stand.turning);
    write_u32(stream, chr->act_stand.checkfacingwall);
    write_u32(stream, chr->act_stand.wallcount);
    write_f32(stream, chr->act_stand.mergetime);
    write_u8(stream, chr->act_stand.face_target);
    break;
  case ACT_ANIM:
    write_u8(stream, chr->act_anim.unk02c != 0);
    write_u8(stream, chr->act_anim.holdLastFrame != 0);
    write_u8(stream, chr->act_anim.playSfx != 0);
    write_u8(stream, chr->act_anim.idleOnEnd != 0);
    write_u8(stream, chr->act_anim.noTranslate != 0);
    write_u8(stream, (chr->chrflags & CHRFLAG_02000000) != 0);
    break;
  case ACT_DIE:
    // notifychrindex (offset 0) is reused by chrlvIterateGuardSeeShotDie as the
    // guard-notification scan cursor. The thud frames fire one-shot SFX and are
    // set to -1.0 once played, so restoring them prevents replay. The remaining
    // fields drive the corpse's extra movement in sub_GAME_7F01FC10 and must
    // remain intact to avoid invalid floating-point divisions after loading.
    write_u32(stream, chr->act_die.notifychrindex);
    write_f32(stream, chr->act_die.thudframe1);
    write_f32(stream, chr->act_die.thudframe2);
    write_f32(stream, chr->act_die.timeextra);
    write_f32(stream, chr->act_die.elapseextra);
    write_bytes(stream, &chr->act_die.extraspeed, sizeof(coord3d));
    write_u16(stream, (u16)chr->act_die.drcarollimagedelay);
    break;
  case ACT_DEAD:
    // The corpse fade/reap timer lives in the offset-0 word, which the engine
    // reads as act_init.padding[0] (aliasing the bool act_dead.allowfade): -1
    // means "just entered", and >=0 is the elapsed fade timer that fadealpha is
    // derived from. The other named act_dead fields are never read.
    write_u32(stream, chr->act_init.padding[0]);
    break;
  case ACT_ARGH:
    // notifychrindex (offset 0) is reused by chrlvIterateGuardSeeShotDie as the
    // guard-notification scan cursor while ACT_ARGH/ACT_DIE are active.
    write_u32(stream, chr->act_argh.notifychrindex);
    write_u32(stream, chr->act_argh.unk30);
    break;
  case ACT_PREARGH:
    // unk044 has no reader or writer in GoldenEye and is omitted.
    write_bytes(stream, &chr->act_preargh.pos, sizeof(coord3d));
    write_f32(stream, chr->act_preargh.unk038);
    write_u32(stream, chr->act_preargh.unk03c);
    write_u32(stream, chr->act_preargh.unk040);
    break;
  case ACT_ATTACK:
    write_u16(stream, (u16)get_firing_animation_id(chr->act_attack.animfloats));
    write_u8(stream, (u8)chr->act_attack.unk30);
    write_u8(stream, (u8)chr->act_attack.unk31);
    write_u8(stream, (u8)chr->act_attack.unk32);
    write_u8(stream, (u8)chr->act_attack.unk33);
    write_u8(stream, (u8)chr->act_attack.unk34);
    write_u8(stream, (u8)chr->act_attack.unk36);
    write_u8(stream, (u8)chr->act_attack.unk37);
    write_bytes(stream, chr->act_attack.unk38, sizeof(chr->act_attack.unk38));
    write_bytes(stream, chr->act_attack.unk3a, sizeof(chr->act_attack.unk3a));
    write_bytes(stream, chr->act_attack.unk3c, sizeof(chr->act_attack.unk3c));
    write_u32(stream, chr->act_attack.unk40);
    write_u32(stream, chr->act_attack.unk44);
    write_u32(stream, chr->act_attack.attack_time);
    write_u32(stream, chr->act_attack.attacktype);
    write_u32(stream, chr->act_attack.entityid);
    write_u32(stream, chr->act_attack.unk54);
    write_u32(stream, chr->act_attack.type_of_motion);
    write_u8(stream, (u8)chr->act_attack.attack_item);
    break;
  case ACT_ATTACKWALK:
    write_u32(stream, chr->act_attackwalk.clock_timer30);
    write_u32(stream, chr->act_attackwalk.clock_timer34);
    write_u32(stream, chr->act_attackwalk.unk038);
    write_u16(stream,
              (u16)get_firing_animation_id(chr->act_attackwalk.animfloats));
    write_u32(stream, chr->act_attackwalk.timer40);
    write_u32(stream, chr->act_attackwalk.unk044);
    write_bytes(stream, chr->act_attackwalk.unk48,
                sizeof(chr->act_attackwalk.unk48));
    write_bytes(stream, chr->act_attackwalk.unk4a,
                sizeof(chr->act_attackwalk.unk4a));
    write_bytes(stream, chr->act_attackwalk.unk4C,
                sizeof(chr->act_attackwalk.unk4C));
    write_f32(stream, chr->act_attackwalk.speed);
    write_u8(stream, (u8)chr->act_attackwalk.attack_item);
    break;
  case ACT_ATTACKROLL:
    write_u16(stream,
              (u16)get_firing_animation_id(chr->act_attackroll.animfloats));
    write_u8(stream, (u8)chr->act_attackroll.unk30);
    write_u8(stream, (u8)chr->act_attackroll.unk31);
    write_u8(stream, (u8)chr->act_attackroll.unk32);
    write_u8(stream, (u8)chr->act_attackroll.unk33);
    write_u8(stream, (u8)chr->act_attackroll.unk34);
    write_u8(stream, (u8)chr->act_attackroll.unk35);
    write_u8(stream, (u8)chr->act_attackroll.unk36);
    write_u8(stream, (u8)chr->act_attackroll.unk37);
    write_bytes(stream, chr->act_attackroll.unk38,
                sizeof(chr->act_attackroll.unk38));
    write_bytes(stream, chr->act_attackroll.unk3a,
                sizeof(chr->act_attackroll.unk3a));
    write_bytes(stream, chr->act_attackroll.unk3c,
                sizeof(chr->act_attackroll.unk3c));
    write_u32(stream, chr->act_attackroll.unk40);
    write_u32(stream, chr->act_attackroll.unk44);
    write_u32(stream, chr->act_attackroll.attack_time);
    write_u32(stream, chr->act_attackroll.unk4c[0]);
    write_u32(stream, chr->act_attackroll.unk4c[1]);
    write_u32(stream, chr->act_attackroll.unk54[0]);
    write_u32(stream, chr->act_attackroll.unk54[1]);
    write_u8(stream, (u8)chr->act_attackroll.attack_item);
    break;
  case ACT_RUNPOS:
    write_bytes(stream, &chr->act_runpos.pos, sizeof(coord3d));
    write_f32(stream, chr->act_runpos.neardist);
    write_u32(stream, chr->act_runpos.eta60);
    write_f32(stream, chr->act_runpos.turnspeed);
    break;
  case ACT_PATROL:
    write_u16(stream, (u16)get_patrol_path_id(chr->act_patrol.path));
    write_u32(stream, chr->act_patrol.nextstep);
    write_u8(stream, chr->act_patrol.forward);
    save_waydata(stream, &chr->act_patrol.waydata);
    write_u32(stream, chr->act_patrol.lastvisible60);
    write_f32(stream, chr->act_patrol.speed);
    break;
  case ACT_GOPOS:
    write_bytes(stream, &chr->act_gopos.targetpos, sizeof(coord3d));
    write_u32(stream, get_tile_offset(chr->act_gopos.target));
    write_u16(stream, (u16)get_waypoint_index(chr->act_gopos.target_path));
    for (i = 0; i < MAX_CHRWAYPOINTS; i++) {
      write_u16(stream, (u16)get_waypoint_index(chr->act_gopos.waypoints[i]));
    }
    write_u8(stream, chr->act_gopos.curindex);
    write_u8(stream, chr->act_gopos.unk59);
    write_u16(stream, chr->act_gopos.unk5a);
    save_waydata(stream, &chr->act_gopos.waydata);
    write_u32(stream, chr->act_gopos.unk9c);
    write_f32(stream, chr->act_gopos.speed);
    break;
  case ACT_BONDMULTI:
    write_u16(
        stream,
        (u16)get_firing_animation_id(
            (struct weapon_firing_animation_table *)chr->act_bondmulti.unk2c));
    break;
  default:
    // Payload-free actions are driven entirely by their discriminator, Model
    // state, and (where applicable) restored equipment attachments.
    break;
  }

  /* The final four bytes of the action union survive action transitions. In
   * particular, ACT_STAND retains the weapon ID here and ACT_ATTACK consumes
   * it later without reinitializing it. Preserve the compact shared tail for
   * every action rather than saving the whole inactive union. */
  write_bytes(stream, &chr->act_attack.attack_item, 4);
}

static void load_supported_action(StateStream *stream, ChrRecord *chr) {
  ACT_TYPE actiontype = (ACT_TYPE)read_u8(stream);
  u8 action_tail[4];
  s32 i;

  if (chr != NULL) {
    chr->actiontype = actiontype;
  }

  switch (actiontype) {
  case ACT_STAND: {
    u32 prestand = read_u32(stream);
    u32 face_entitytype = read_u32(stream);
    u32 face_entityid = read_u32(stream);
    u32 reaim = read_u32(stream);
    u32 turning = read_u32(stream);
    u32 checkfacingwall = read_u32(stream);
    u32 wallcount = read_u32(stream);
    f32 mergetime = read_f32(stream);
    s8 face_target = read_u8(stream);
    if (chr != NULL) {
      chr->act_stand.prestand = prestand;
      chr->act_stand.face_entitytype = face_entitytype;
      chr->act_stand.face_entityid = face_entityid;
      chr->act_stand.reaim = reaim;
      chr->act_stand.turning = turning;
      chr->act_stand.checkfacingwall = checkfacingwall;
      chr->act_stand.wallcount = wallcount;
      chr->act_stand.mergetime = mergetime;
      chr->act_stand.face_target = face_target;
    }
    break;
  }
  case ACT_ANIM: {
    u32 unk02c = read_u8(stream);
    u32 holdLastFrame = read_u8(stream);
    u32 playSfx = read_u8(stream);
    u32 idleOnEnd = read_u8(stream);
    u32 noTranslate = read_u8(stream);
    bool sfxPlayed = read_u8(stream);

    if (chr != NULL) {
      chr->act_anim.unk02c = unk02c;
      chr->act_anim.holdLastFrame = holdLastFrame;
      chr->act_anim.playSfx = playSfx;
      chr->act_anim.idleOnEnd = idleOnEnd;
      chr->act_anim.noTranslate = noTranslate;
      if (sfxPlayed) {
        chr->chrflags |= CHRFLAG_02000000;
      } else {
        chr->chrflags &= ~CHRFLAG_02000000;
      }
    }
    break;
  }
  case ACT_DIE: {
    s32 notifychrindex = read_u32(stream);
    f32 thudframe1 = read_f32(stream);
    f32 thudframe2 = read_f32(stream);
    f32 timeextra = read_f32(stream);
    f32 elapseextra = read_f32(stream);
    coord3d extraspeed;
    s16 drcarollimagedelay;

    read_bytes(stream, &extraspeed, sizeof(coord3d));
    drcarollimagedelay = (s16)read_u16(stream);

    if (chr != NULL) {
      chr->act_die.notifychrindex = notifychrindex;
      chr->act_die.thudframe1 = thudframe1;
      chr->act_die.thudframe2 = thudframe2;
      chr->act_die.timeextra = timeextra;
      chr->act_die.elapseextra = elapseextra;
      chr->act_die.extraspeed = extraspeed;
      chr->act_die.drcarollimagedelay = drcarollimagedelay;
    }
    break;
  }
  case ACT_DEAD: {
    s32 fade_timer = read_u32(stream);

    if (chr != NULL) {
      chr->act_init.padding[0] = fade_timer;
    }
    break;
  }
  case ACT_ARGH: {
    s32 notifychrindex = read_u32(stream);
    s32 unk30 = read_u32(stream);

    if (chr != NULL) {
      chr->act_argh.notifychrindex = notifychrindex;
      chr->act_argh.unk30 = unk30;
    }
    break;
  }
  case ACT_PREARGH: {
    coord3d pos;
    f32 unk038;
    s32 unk03c;
    s32 unk040;

    read_bytes(stream, &pos, sizeof(coord3d));
    unk038 = read_f32(stream);
    unk03c = read_u32(stream);
    unk040 = read_u32(stream);

    if (chr != NULL) {
      chr->act_preargh.pos = pos;
      chr->act_preargh.unk038 = unk038;
      chr->act_preargh.unk03c = unk03c;
      chr->act_preargh.unk040 = unk040;
    }
    break;
  }
  case ACT_ATTACK: {
    s16 anim_id = (s16)read_u16(stream);
    s8 unk30 = (s8)read_u8(stream);
    s8 unk31 = (s8)read_u8(stream);
    s8 unk32 = (s8)read_u8(stream);
    s8 unk33 = (s8)read_u8(stream);
    s8 unk34 = (s8)read_u8(stream);
    s8 unk36 = (s8)read_u8(stream);
    s8 unk37 = (s8)read_u8(stream);
    s8 unk38[2];
    s8 unk3a[2];
    s8 unk3c[2];
    u32 unk40;
    s32 unk44;
    s32 attack_time;
    u32 attacktype;
    u32 entityid;
    u32 unk54;
    s32 type_of_motion;
    s8 attack_item;

    read_bytes(stream, unk38, sizeof(unk38));
    read_bytes(stream, unk3a, sizeof(unk3a));
    read_bytes(stream, unk3c, sizeof(unk3c));
    unk40 = read_u32(stream);
    unk44 = read_u32(stream);
    attack_time = read_u32(stream);
    attacktype = read_u32(stream);
    entityid = read_u32(stream);
    unk54 = read_u32(stream);
    type_of_motion = read_u32(stream);
    attack_item = (s8)read_u8(stream);

    if (chr != NULL) {
      chr->act_attack.animfloats = get_firing_animation_by_id(anim_id);
      chr->act_attack.unk30 = unk30;
      chr->act_attack.unk31 = unk31;
      chr->act_attack.unk32 = unk32;
      chr->act_attack.unk33 = unk33;
      chr->act_attack.unk34 = unk34;
      chr->act_attack.unk36 = unk36;
      chr->act_attack.unk37 = unk37;
      chr->act_attack.unk38[0] = unk38[0];
      chr->act_attack.unk38[1] = unk38[1];
      chr->act_attack.unk3a[0] = unk3a[0];
      chr->act_attack.unk3a[1] = unk3a[1];
      chr->act_attack.unk3c[0] = unk3c[0];
      chr->act_attack.unk3c[1] = unk3c[1];
      chr->act_attack.unk40 = unk40;
      chr->act_attack.unk44 = unk44;
      chr->act_attack.attack_time = attack_time;
      chr->act_attack.attacktype = attacktype;
      chr->act_attack.entityid = entityid;
      chr->act_attack.unk54 = unk54;
      chr->act_attack.type_of_motion = type_of_motion;
      chr->act_attack.attack_item = attack_item;
    }
    break;
  }
  case ACT_ATTACKWALK: {
    s32 clock_timer30 = read_u32(stream);
    s32 clock_timer34 = read_u32(stream);
    u32 unk038 = read_u32(stream);
    s16 anim_id = (s16)read_u16(stream);
    s32 timer40 = read_u32(stream);
    s32 unk044 = read_u32(stream);
    s8 unk48[2];
    s8 unk4a[2];
    s8 unk4C[2];
    f32 speed;
    s8 attack_item;

    read_bytes(stream, unk48, sizeof(unk48));
    read_bytes(stream, unk4a, sizeof(unk4a));
    read_bytes(stream, unk4C, sizeof(unk4C));
    speed = read_f32(stream);
    attack_item = (s8)read_u8(stream);

    if (chr != NULL) {
      chr->act_attackwalk.clock_timer30 = clock_timer30;
      chr->act_attackwalk.clock_timer34 = clock_timer34;
      chr->act_attackwalk.unk038 = unk038;
      chr->act_attackwalk.animfloats = get_firing_animation_by_id(anim_id);
      chr->act_attackwalk.timer40 = timer40;
      chr->act_attackwalk.unk044 = unk044;
      chr->act_attackwalk.unk48[0] = unk48[0];
      chr->act_attackwalk.unk48[1] = unk48[1];
      chr->act_attackwalk.unk4a[0] = unk4a[0];
      chr->act_attackwalk.unk4a[1] = unk4a[1];
      chr->act_attackwalk.unk4C[0] = unk4C[0];
      chr->act_attackwalk.unk4C[1] = unk4C[1];
      chr->act_attackwalk.speed = speed;
      chr->act_attackwalk.attack_item = attack_item;
    }
    break;
  }
  case ACT_ATTACKROLL: {
    s16 anim_id = (s16)read_u16(stream);
    s8 unk30 = (s8)read_u8(stream);
    s8 unk31 = (s8)read_u8(stream);
    s8 unk32 = (s8)read_u8(stream);
    s8 unk33 = (s8)read_u8(stream);
    s8 unk34 = (s8)read_u8(stream);
    s8 unk35 = (s8)read_u8(stream);
    s8 unk36 = (s8)read_u8(stream);
    s8 unk37 = (s8)read_u8(stream);
    s8 unk38[2];
    s8 unk3a[2];
    s8 unk3c[2];
    u32 unk40;
    s32 unk44;
    s32 attack_time;
    s32 unk4c[2];
    s32 unk54[2];
    s8 attack_item;

    read_bytes(stream, unk38, sizeof(unk38));
    read_bytes(stream, unk3a, sizeof(unk3a));
    read_bytes(stream, unk3c, sizeof(unk3c));
    unk40 = read_u32(stream);
    unk44 = read_u32(stream);
    attack_time = read_u32(stream);
    unk4c[0] = read_u32(stream);
    unk4c[1] = read_u32(stream);
    unk54[0] = read_u32(stream);
    unk54[1] = read_u32(stream);
    attack_item = (s8)read_u8(stream);

    if (chr != NULL) {
      chr->act_attackroll.animfloats = get_firing_animation_by_id(anim_id);
      chr->act_attackroll.unk30 = unk30;
      chr->act_attackroll.unk31 = unk31;
      chr->act_attackroll.unk32 = unk32;
      chr->act_attackroll.unk33 = unk33;
      chr->act_attackroll.unk34 = unk34;
      chr->act_attackroll.unk35 = unk35;
      chr->act_attackroll.unk36 = unk36;
      chr->act_attackroll.unk37 = unk37;
      chr->act_attackroll.unk38[0] = unk38[0];
      chr->act_attackroll.unk38[1] = unk38[1];
      chr->act_attackroll.unk3a[0] = unk3a[0];
      chr->act_attackroll.unk3a[1] = unk3a[1];
      chr->act_attackroll.unk3c[0] = unk3c[0];
      chr->act_attackroll.unk3c[1] = unk3c[1];
      chr->act_attackroll.unk40 = unk40;
      chr->act_attackroll.unk44 = unk44;
      chr->act_attackroll.attack_time = attack_time;
      chr->act_attackroll.unk4c[0] = unk4c[0];
      chr->act_attackroll.unk4c[1] = unk4c[1];
      chr->act_attackroll.unk54[0] = unk54[0];
      chr->act_attackroll.unk54[1] = unk54[1];
      chr->act_attackroll.attack_item = attack_item;
    }
    break;
  }
  case ACT_RUNPOS: {
    coord3d pos;
    f32 neardist;
    s32 eta60;
    f32 turnspeed;

    read_bytes(stream, &pos, sizeof(coord3d));
    neardist = read_f32(stream);
    eta60 = read_u32(stream);
    turnspeed = read_f32(stream);

    if (chr != NULL) {
      chr->act_runpos.pos = pos;
      chr->act_runpos.neardist = neardist;
      chr->act_runpos.eta60 = eta60;
      chr->act_runpos.turnspeed = turnspeed;
    }
    break;
  }
  case ACT_PATROL: {
    s16 path_id = (s16)read_u16(stream);
    s32 nextstep = read_u32(stream);
    bool forward = read_u8(stream);
    struct waydata waydata;
    s32 lastvisible60;
    f32 speed;

    load_waydata(stream, &waydata);
    lastvisible60 = read_u32(stream);
    speed = read_f32(stream);

    if (chr != NULL) {
      chr->act_patrol.path = get_patrol_path_by_id(path_id);
      chr->act_patrol.nextstep = nextstep;
      chr->act_patrol.forward = forward;
      chr->act_patrol.waydata = waydata;
      chr->act_patrol.lastvisible60 = lastvisible60;
      chr->act_patrol.speed = speed;
    }
    break;
  }
  case ACT_GOPOS: {
    coord3d targetpos;
    s32 target_offset;
    s16 target_path_index;
    s16 waypoint_indices[MAX_CHRWAYPOINTS];
    u8 curindex;
    u8 unk59;
    u16 unk5a;
    struct waydata waydata;
    s32 unk9c;
    f32 speed;

    read_bytes(stream, &targetpos, sizeof(coord3d));
    target_offset = read_u32(stream);
    target_path_index = (s16)read_u16(stream);
    for (i = 0; i < MAX_CHRWAYPOINTS; i++) {
      waypoint_indices[i] = (s16)read_u16(stream);
    }
    curindex = read_u8(stream);
    unk59 = read_u8(stream);
    unk5a = read_u16(stream);
    load_waydata(stream, &waydata);
    unk9c = read_u32(stream);
    speed = read_f32(stream);

    if (chr != NULL) {
      chr->act_gopos.targetpos = targetpos;
      chr->act_gopos.target = get_tile_by_offset(target_offset);
      chr->act_gopos.target_path = get_waypoint_by_index(target_path_index);
      for (i = 0; i < MAX_CHRWAYPOINTS; i++) {
        chr->act_gopos.waypoints[i] =
            get_waypoint_by_index(waypoint_indices[i]);
      }
      chr->act_gopos.curindex = curindex;
      chr->act_gopos.unk59 = unk59;
      chr->act_gopos.unk5a = unk5a;
      chr->act_gopos.waydata = waydata;
      chr->act_gopos.unk9c = unk9c;
      chr->act_gopos.speed = speed;
    }
    break;
  }
  case ACT_BONDMULTI: {
    s16 animation_id = (s16)read_u16(stream);

    if (chr != NULL) {
      chr->act_bondmulti.unk2c =
          (f32 *)get_firing_animation_by_id(animation_id);
    }
    break;
  }
  default:
    break;
  }

  read_bytes(stream, action_tail, sizeof(action_tail));
  if (chr != NULL) {
    chr->act_attack.attack_item = (s8)action_tail[0];
    chr->act_attack.unk81 = action_tail[1];
    chr->act_attack.unk82 = action_tail[2];
    chr->act_attack.unk83 = action_tail[3];
  }
}

bool practice_states_save_chr_action(StateStream *stream,
                                     const ChrRecord *chr) {
  if (!is_supported_chr_action(chr)) {
    practiceLogWarn("Unsupported viewer CHR action %d", chr->actiontype);
    return FALSE;
  }

  save_supported_action(stream, chr);
  return TRUE;
}

void practice_states_load_chr_action(StateStream *stream, ChrRecord *chr) {
  load_supported_action(stream, chr);
}

typedef struct SavedModelAnimation {
  s32 anim_offset;
  s32 anim2_offset;
  s8 gunhand;
  s8 unk25;
  s8 animlooping;
  s8 unk27;
  f32 unk28;
  f32 unk2c;
  s16 framea;
  s16 frameb;
  s32 unk34;
  s32 unk38;
  f32 endframe;
  f32 speed;
  f32 newspeed;
  f32 oldspeed;
  f32 timespeed;
  f32 elapsespeed;
  f32 unk58;
  f32 unk5c;
  s16 frame2a;
  s16 frame2b;
  s32 unk64;
  s32 unk68;
  f32 unk6c;
  f32 speed2;
  s32 unk74;
  s32 unk78;
  f32 unk7c;
  s32 unk80;
  f32 unk84;
  f32 unk88;
  s32 unk8c;
  f32 animloopframe;
  f32 animloopmerge;
  s32 unk9c;
  s32 unka0;
  f32 playspeed;
  f32 animrate;
  f32 unkac;
  f32 unkb0;
  f32 unkb4;
  f32 unkb8;
  s32 unkbc;
  u8 anim_flip_callback;
  u8 has_root_data;
} SavedModelAnimation;

static void write_animation_zero_rle(StateStream *stream, const u8 *src,
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

static void read_animation_zero_rle(StateStream *stream, u8 *dst, u32 size) {
  u32 offset = 0;

  while (offset < size) {
    u8 control = read_u8(stream);
    u32 count = (control & 0x7f) + 1;

    if (count > size - offset) {
      practiceLogError("Model animation RLE exceeds destination size");
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

static bool model_rw_scalar_value(ModelNode *node,
                                  union ModelRwData **datas,
                                  s32 rwdata_count, u32 **value) {
  union ModelRwData *rwdata;
  u16 rwdata_index;

  if (datas == NULL || node == NULL || node->Data == NULL) {
    practiceLogError("Model scalar has incomplete model/node data");
    assert(FALSE);
    return FALSE;
  }

  switch (node->Opcode & 0xff) {
  case MODELNODE_OPCODE_OP07:
    rwdata_index = node->Data->Op07.RwDataIndex;
    break;
  case MODELNODE_OPCODE_LOD:
    rwdata_index = node->Data->LOD.RwDataIndex;
    break;
  case MODELNODE_OPCODE_SWITCH:
    rwdata_index = node->Data->Switch.RwDataIndex;
    break;
  case MODELNODE_OPCODE_BSP:
    rwdata_index = node->Data->BSP.RwDataIndex;
    break;
  case MODELNODE_OPCODE_OP11:
    rwdata_index = node->Data->Op11.RwDataIndex;
    break;
  case MODELNODE_OPCODE_GUNFIRE:
    rwdata_index = node->Data->Gunfire.RwDataIndex;
    break;
  default:
    return FALSE;
  }

  if (rwdata_index >= rwdata_count) {
    practiceLogError(
        "Model scalar opcode=%d RW index %d exceeds %d instance records",
        node->Opcode & 0xff, rwdata_index, rwdata_count);
    assert(FALSE);
    return FALSE;
  }

  rwdata = (union ModelRwData *)&datas[rwdata_index];

  switch (node->Opcode & 0xff) {
  case MODELNODE_OPCODE_OP07:
    *value = (u32 *)&rwdata->Op07.index;
    return TRUE;
  case MODELNODE_OPCODE_LOD:
    *value = (u32 *)&rwdata->LOD.visible;
    return TRUE;
  case MODELNODE_OPCODE_SWITCH:
    *value = (u32 *)&rwdata->Switch.visible;
    return TRUE;
  case MODELNODE_OPCODE_BSP:
    *value = (u32 *)&rwdata->BSP.visible;
    return TRUE;
  case MODELNODE_OPCODE_OP11:
    *value = (u32 *)&rwdata->Op11.unk00;
    return TRUE;
  case MODELNODE_OPCODE_GUNFIRE:
    *value = (u32 *)&rwdata->Gunfire.visible;
    return TRUE;
  default:
    return FALSE;
  }
}

static ModelNode *model_rw_scalar_child(ModelNode *node) {
  switch (node->Opcode & 0xff) {
  case MODELNODE_OPCODE_LOD:
    return node->Data->LOD.Affects;
  case MODELNODE_OPCODE_SWITCH:
    return node->Data->Switch.Controls;
  case MODELNODE_OPCODE_HEAD:
    return NULL;
  default:
    return node->Child;
  }
}

static bool restore_model_parent_links(ModelNode *node, ModelNode *parent,
                                       union ModelRwData **datas,
                                       s32 rwdata_count, s32 depth,
                                       s32 *nodes_seen) {
  ModelNode *previous = NULL;

  if (depth >= 64) {
    practiceLogError("Model parent-link restoration exceeded depth limit");
    assert(FALSE);
    return FALSE;
  }

  while (node != NULL) {
    ModelNode *child;
    union ModelRwData **child_datas = datas;
    s32 child_rwdata_count = rwdata_count;
    u8 opcode;

    if (++(*nodes_seen) > MAX_MODEL_TRAVERSAL_NODES || node->Data == NULL) {
      practiceLogError("Model parent-link restoration found invalid graph");
      assert(FALSE);
      return FALSE;
    }

    opcode = node->Opcode & 0xff;
    node->Parent = parent;
    node->Prev = previous;
    child = model_rw_scalar_child(node);

    if (opcode == MODELNODE_OPCODE_LOD) {
      u16 index = node->Data->LOD.RwDataIndex;

      if (datas == NULL || index >= rwdata_count) {
        return FALSE;
      }
      node->Child = ((union ModelRwData *)&datas[index])->LOD.visible
                        ? child
                        : NULL;
    } else if (opcode == MODELNODE_OPCODE_SWITCH) {
      u16 index = node->Data->Switch.RwDataIndex;

      if (datas == NULL || index >= rwdata_count) {
        return FALSE;
      }
      node->Child = ((union ModelRwData *)&datas[index])->Switch.visible
                        ? child
                        : NULL;
    } else if (opcode == MODELNODE_OPCODE_BSP) {
      u16 index = node->Data->BSP.RwDataIndex;

      if (datas == NULL || index >= rwdata_count) {
        return FALSE;
      }
      modelApplyReorderRelationsByArg(
          node, ((union ModelRwData *)&datas[index])->BSP.visible);
      child = node->Child;
    }

    if (opcode == MODELNODE_OPCODE_HEAD) {
      u16 index = node->Data->HeadPlaceholder.RwDataIndex;

      child = NULL;
      if (datas != NULL && index + 1 < rwdata_count) {
        ModelRwData_HeadPlaceholderRecord *head_data =
            (ModelRwData_HeadPlaceholderRecord *)&datas[index];

        if (head_data->ModelFileHeader != NULL && head_data->RwDatas != NULL) {
          u32 data_start = (u32)datas;
          u32 child_start = (u32)head_data->RwDatas;
          u32 child_offset;

          if (child_start >= data_start &&
              (child_start - data_start) % sizeof(*datas) == 0) {
            child_offset = (child_start - data_start) / sizeof(*datas);
          } else {
            child_offset = rwdata_count;
          }
          if (child_offset < (u32)rwdata_count) {
            child = head_data->ModelFileHeader->RootNode;
            child_datas = head_data->RwDatas;
            child_rwdata_count = rwdata_count - child_offset;
            node->Child = child;
          }
        }
      }
    }

    if (child != NULL &&
        !restore_model_parent_links(child, node, child_datas,
                                    child_rwdata_count, depth + 1,
                                    nodes_seen)) {
      return FALSE;
    }
    previous = node;
    node = node->Next;
  }

  return TRUE;
}

bool practice_states_restore_model_parent_links(Model *model) {
  s32 nodes_seen = 0;

  if (model == NULL || model->obj == NULL || model->obj->RootNode == NULL ||
      model->datas == NULL || model->Type <= 0) {
    practiceLogError("Cannot restore parent links for incomplete model");
    assert(FALSE);
    return FALSE;
  }

  return restore_model_parent_links(model->obj->RootNode, NULL, model->datas,
                                    model->Type, 0, &nodes_seen);
}

static bool restore_model_definition_links(ModelNode *node,
                                           ModelNode *parent, s32 depth,
                                           s32 *nodes_seen) {
  ModelNode *previous = NULL;

  if (depth >= 64) {
    return FALSE;
  }
  while (node != NULL) {
    ModelNode *child;
    u8 opcode;

    if (++(*nodes_seen) > MAX_MODEL_TRAVERSAL_NODES || node->Data == NULL) {
      return FALSE;
    }
    opcode = node->Opcode & 0xff;
    node->Parent = parent;
    node->Prev = previous;

    switch (opcode) {
    case MODELNODE_OPCODE_LOD:
      child = node->Data->LOD.Affects;
      node->Child = child;
      break;
    case MODELNODE_OPCODE_SWITCH:
      child = node->Data->Switch.Controls;
      node->Child = child;
      break;
    case MODELNODE_OPCODE_BSP:
      modelApplyReorderRelationsByArg(node, FALSE);
      child = node->Child;
      break;
    case MODELNODE_OPCODE_HEAD:
      child = NULL;
      node->Child = NULL;
      break;
    default:
      child = node->Child;
      break;
    }

    if (child != NULL &&
        !restore_model_definition_links(child, node, depth + 1,
                                        nodes_seen)) {
      return FALSE;
    }
    previous = node;
    node = node->Next;
  }
  return TRUE;
}

static bool restore_model_file_definition(ModelFileHeader *header) {
  s32 nodes_seen = 0;

  return header != NULL && header->RootNode != NULL &&
         restore_model_definition_links(header->RootNode, NULL, 0,
                                        &nodes_seen);
}

static s32 chr_model_entry_count(void) {
  s32 count = 0;

  while (count < 255 && c_item_entries[count].header != NULL) {
    count++;
  }
  if (count >= 255) {
    practiceLogError("Character model table is too large");
    assert(FALSE);
    return 0;
  }
  return count;
}

static s32 find_chr_model_parent_entry(ModelNode *parent, s32 count) {
  s32 i;

  if (parent == NULL) {
    return -1;
  }
  for (i = 0; i < count; i++) {
    ModelFileHeader *header = c_item_entries[i].header;

    if (header->RootNode != NULL && header->Switches != NULL &&
        header->numSwitches > 4 && header->Switches[4] != NULL &&
        (header->Switches[4]->Opcode & 0xff) == MODELNODE_OPCODE_HEAD &&
        header->Switches[4] == parent) {
      return i;
    }
  }
  return -1;
}

static u16 *model_definition_rw_index(ModelNode *node) {
  switch (node->Opcode & 0xff) {
  case MODELNODE_OPCODE_HEADER:
    return &node->Data->Header.RwDataIndex;
  case MODELNODE_OPCODE_OP07:
    return &node->Data->Op07.RwDataIndex;
  case MODELNODE_OPCODE_LOD:
    return &node->Data->LOD.RwDataIndex;
  case MODELNODE_OPCODE_SWITCH:
    return &node->Data->Switch.RwDataIndex;
  case MODELNODE_OPCODE_BSP:
    return &node->Data->BSP.RwDataIndex;
  case MODELNODE_OPCODE_OP11:
    return &node->Data->Op11.RwDataIndex;
  case MODELNODE_OPCODE_GUNFIRE:
    return &node->Data->Gunfire.RwDataIndex;
  case MODELNODE_OPCODE_HEAD:
    return &node->Data->HeadPlaceholder.RwDataIndex;
  case MODELNODE_OPCODE_DLCOLLISION:
    return &node->Data->DisplayListCollisions.RwDataIndex;
  default:
    return NULL;
  }
}

/* Visit a definition's own nodes in a stable order. Rendering can reorder BSP
 * child chains and can attach another definition below a HEAD node, so neither
 * the live Child pointers nor the live sibling order are sufficient here. */
static bool visit_model_definition_rw_indices(
    ModelNode *node, ModelNode *stop, StateStream *stream, bool save,
    u16 *index_count, s32 depth, s32 *nodes_seen) {
  if (depth >= 64) {
    practiceLogError("Model RW-index traversal exceeded depth limit");
    assert(FALSE);
    return FALSE;
  }

  while (node != NULL && node != stop) {
    ModelNode *child = node->Child;
    u16 *rw_index;
    u8 opcode;

    if (++(*nodes_seen) > MAX_MODEL_TRAVERSAL_NODES || node->Data == NULL) {
      practiceLogError("Model RW-index traversal found an invalid graph");
      assert(FALSE);
      return FALSE;
    }

    opcode = node->Opcode & 0xff;
    rw_index = model_definition_rw_index(node);
    if (rw_index != NULL) {
      if (stream != NULL) {
        if (save) {
          write_u16(stream, *rw_index);
        } else {
          *rw_index = read_u16(stream);
        }
      }
      (*index_count)++;
    }

    if (opcode == MODELNODE_OPCODE_BSP) {
      ModelNode *left = node->Data->BSP.leftChild;
      ModelNode *right = node->Data->BSP.rightChild;

      if (!visit_model_definition_rw_indices(
              right, left, stream, save, index_count, depth + 1,
              nodes_seen) ||
          !visit_model_definition_rw_indices(
              left, right, stream, save, index_count, depth + 1,
              nodes_seen)) {
        return FALSE;
      }
      child = NULL;
    } else if (opcode == MODELNODE_OPCODE_LOD) {
      child = node->Data->LOD.Affects;
    } else if (opcode == MODELNODE_OPCODE_SWITCH) {
      child = node->Data->Switch.Controls;
    } else if (opcode == MODELNODE_OPCODE_HEAD) {
      child = NULL;
    }

    if (child != NULL &&
        !visit_model_definition_rw_indices(
            child, NULL, stream, save, index_count, depth + 1, nodes_seen)) {
      return FALSE;
    }
    node = node->Next;
  }
  return TRUE;
}

static bool count_model_definition_rw_indices(ModelFileHeader *header,
                                              u16 *index_count) {
  s32 nodes_seen = 0;

  *index_count = 0;
  return header != NULL && header->RootNode != NULL &&
         visit_model_definition_rw_indices(
             header->RootNode, NULL, NULL, FALSE, index_count, 0,
             &nodes_seen);
}

static bool stream_model_definition_rw_indices(ModelFileHeader *header,
                                               StateStream *stream,
                                               bool save,
                                               u16 expected_count) {
  s32 nodes_seen = 0;
  u16 actual_count = 0;

  if (!count_model_definition_rw_indices(header, &actual_count) ||
      actual_count != expected_count) {
    practiceLogError("Character model RW-index count changed (%d != %d)",
                     actual_count, expected_count);
    assert(FALSE);
    return FALSE;
  }

  actual_count = 0;
  return visit_model_definition_rw_indices(
      header->RootNode, NULL, stream, save, &actual_count, 0, &nodes_seen);
}

/* modelAttachPart writes the attached head into shared body definitions and
 * writes the body placeholder back into shared head definitions. Recreating
 * the same CHRs in another allocation order can therefore leave a different
 * global graph even when every Model instance is correct. Preserve the final
 * owner links and record counts that ordinary gameplay had at save time. */
void practice_states_save_chr_model_definitions(StateStream *stream) {
  s32 count = chr_model_entry_count();
  s32 loaded_count = 0;
  s32 i;

  for (i = 0; i < count; i++) {
    if (c_item_entries[i].header->RootNode != NULL) {
      loaded_count++;
    }
  }
  write_u8(stream, loaded_count);
  for (i = 0; i < count; i++) {
    ModelFileHeader *header = c_item_entries[i].header;
    ModelNode *head_node;
    s32 parent_entry;
    s32 child_entry = -1;
    u16 rw_index_count;

    if (header->RootNode == NULL) {
      continue;
    }
    parent_entry =
        find_chr_model_parent_entry(header->RootNode->Parent, count);
    if (header->RootNode->Parent != NULL && parent_entry < 0) {
      practiceLogError("Character model %d has an unknown definition parent",
                       i);
      assert(FALSE);
    }
    head_node = header->Switches != NULL && header->numSwitches > 4
                    ? header->Switches[4]
                    : NULL;
    if (head_node != NULL && head_node->Data != NULL &&
        (head_node->Opcode & 0xff) == MODELNODE_OPCODE_HEAD) {
      if (head_node->Child != NULL) {
        s32 j;

        child_entry = -1;
        for (j = 0; j < count; j++) {
          if (c_item_entries[j].header->RootNode == head_node->Child) {
            child_entry = j;
            break;
          }
        }
        if (child_entry < 0) {
          practiceLogError("Character model %d has an unknown attached head",
                           i);
          assert(FALSE);
        }
      }
    }
    write_u8(stream, i);
    write_u16(stream, header->numRecords);
    write_u8(stream, parent_entry >= 0 ? parent_entry : 0xff);
    write_u8(stream, child_entry >= 0 ? child_entry : 0xff);
    if (!count_model_definition_rw_indices(header, &rw_index_count)) {
      rw_index_count = 0;
    }
    write_u16(stream, rw_index_count);
    stream_model_definition_rw_indices(header, stream, TRUE,
                                       rw_index_count);
  }
}

bool practice_states_canonicalize_chr_model_definitions(void) {
  s32 count = chr_model_entry_count();
  s32 i;

  if (count == 0) {
    return FALSE;
  }
  /* Character reconstruction mutates the shared node graph and its RW-data
   * indices once per allocated model. Rebuild every loaded definition once,
   * after all props exist, so the final shared indices are canonical before
   * any CHR display-list RW data is repaired. */
  for (i = 0; i < count; i++) {
    ModelFileHeader *header = c_item_entries[i].header;

    if (header->RootNode != NULL) {
      if (!restore_model_file_definition(header)) {
        practiceLogError("Could not rebuild character model definition %d", i);
        assert(FALSE);
        return FALSE;
      }
      modelCalculateRwDataLen(header);
    }
  }
  return TRUE;
}

bool practice_states_load_chr_model_definitions(StateStream *stream) {
  s32 count = chr_model_entry_count();
  u8 saved_count = read_u8(stream);
  s32 i;

  if (!practice_states_canonicalize_chr_model_definitions()) {
    return FALSE;
  }
  for (i = 0; i < saved_count; i++) {
    u8 entry = read_u8(stream);
    u16 num_records = read_u16(stream);
    u8 parent_entry = read_u8(stream);
    u8 child_entry = read_u8(stream);
    u16 rw_index_count = read_u16(stream);
    ModelFileHeader *header;
    ModelNode *node;
    ModelNode *parent = NULL;

    if (entry >= count || (parent_entry != 0xff && parent_entry >= count) ||
        (child_entry != 0xff && child_entry >= count)) {
      practiceLogError("Saved character model definition index is invalid");
      assert(FALSE);
      return FALSE;
    }
    header = c_item_entries[entry].header;
    if (header->RootNode == NULL) {
      practiceLogError("Saved character model %d is not loaded", entry);
      assert(FALSE);
      return FALSE;
    }
    if (parent_entry != 0xff) {
      ModelFileHeader *parent_header = c_item_entries[parent_entry].header;

      if (parent_header->RootNode == NULL || parent_header->Switches == NULL ||
          parent_header->numSwitches <= 4 ||
          parent_header->Switches[4] == NULL) {
        practiceLogError("Saved character model parent %d is invalid",
                         parent_entry);
        assert(FALSE);
        return FALSE;
      }
      parent = parent_header->Switches[4];
    }
    header->numRecords = num_records;
    for (node = header->RootNode; node != NULL; node = node->Next) {
      node->Parent = parent;
    }
    if (header->Switches != NULL && header->numSwitches > 4 &&
        header->Switches[4] != NULL && header->Switches[4]->Data != NULL &&
        (header->Switches[4]->Opcode & 0xff) == MODELNODE_OPCODE_HEAD) {
      header->Switches[4]->Child =
          child_entry != 0xff ? c_item_entries[child_entry].header->RootNode
                              : NULL;
    } else if (child_entry != 0xff) {
      practiceLogError("Saved character model child %d has no placeholder",
                       entry);
      assert(FALSE);
      return FALSE;
    }
    if (!stream_model_definition_rw_indices(header, stream, FALSE,
                                            rw_index_count)) {
      return FALSE;
    }
  }
  return TRUE;
}

bool practice_states_rebuild_chr_model_allocation(ChrRecord *chr) {
  Model *model;
  ModelFileHeader *body_header;

  if (chr == NULL || chr->model == NULL || chr->bodynum < 0) {
    return FALSE;
  }
  model = chr->model;
  body_header = c_item_entries[chr->bodynum].header;
  if (model->obj != body_header || model->datas == NULL || model->Type <= 0 ||
      !restore_model_file_definition(body_header)) {
    return FALSE;
  }

  modelCalculateRwDataLen(body_header);
  if (body_header->numRecords > model->Type) {
    return FALSE;
  }
  modelInitRwData(model, body_header->RootNode);

  if (chr->headnum >= 0 && !c_item_entries[chr->bodynum].hasHead) {
    ModelFileHeader *head_header = c_item_entries[chr->headnum].header;
    ModelNode *head_node;

    if (!restore_model_file_definition(head_header)) {
      return FALSE;
    }
    modelCalculateRwDataLen(head_header);
    if (body_header->numRecords + head_header->numRecords > model->Type ||
        body_header->Switches == NULL || body_header->numSwitches <= 4 ||
        body_header->Switches[4] == NULL) {
      return FALSE;
    }
    head_node = body_header->Switches[4];
    if ((head_node->Opcode & 0xff) != MODELNODE_OPCODE_HEAD) {
      return FALSE;
    }
    modelAttachHead(model, head_node, head_header);
  }

  return practice_states_restore_model_parent_links(model);
}

bool practice_states_restore_chr_model_allocation(
    ChrRecord *chr, const ChrAllocationState *allocation) {
  ModelFileHeader *body_header;
  Model *model;
  s32 allocated_type;

  if (chr == NULL || allocation == NULL || allocation->bodynum < 0 ||
      chr->bodynum != allocation->bodynum ||
      chr->headnum != allocation->headnum || chr->model == NULL) {
    return FALSE;
  }

  model = chr->model;
  if (model->chr != chr ||
      model->obj != c_item_entries[allocation->bodynum].header ||
      model->datas == NULL || model->Type <= 0) {
    return FALSE;
  }
  body_header = model->obj;
  allocated_type = model->Type;
  /* ModelFileHeader::numRecords is shared definition state. Attaching a head
   * for another CHR can change it to that other instance's combined size, so
   * it is not a valid bound for this model. The saved Type and explicit head
   * offsets below describe this instance's actual allocation. */
  if (allocation->model_type == 0 || allocation->model_type > allocated_type) {
    return FALSE;
  }

  /* Each model definition is global and modelAttachHead rewrites its RW-data
   * indices for the most recently attached instance. Rebuild the body alone,
   * and the selected head alone below, before interpreting this model's saved
   * data. Their canonical indices are relative to the corresponding instance
   * data blocks and do not inherit another guard's head size. */
  if (!restore_model_file_definition(body_header)) {
    return FALSE;
  }
  modelCalculateRwDataLen(body_header);
  if (body_header->numRecords > allocation->model_type) {
    return FALSE;
  }

  if (allocation->headnum >= 0 &&
      !c_item_entries[allocation->bodynum].hasHead) {
    ModelFileHeader *head_header = c_item_entries[allocation->headnum].header;
    ModelNode *head_node;
    ModelRwData_HeadPlaceholderRecord *head_data;

    if (!restore_model_file_definition(head_header)) {
      return FALSE;
    }
    modelCalculateRwDataLen(head_header);

    if (allocation->head_record_index == 0xffff ||
        allocation->head_data_offset == 0xffff ||
        allocation->head_record_index + 1 >= allocation->model_type ||
        allocation->head_data_offset >= allocation->model_type ||
        model->obj->Switches == NULL || model->obj->numSwitches <= 4 ||
        model->obj->Switches[4] == NULL ||
        model->obj->Switches[4]->Data == NULL) {
      return FALSE;
    }

    head_node = model->obj->Switches[4];
    if ((head_node->Opcode & 0xff) != MODELNODE_OPCODE_HEAD) {
      return FALSE;
    }
    head_node->Data->HeadPlaceholder.RwDataIndex =
        allocation->head_record_index;
    head_data = (ModelRwData_HeadPlaceholderRecord *)&model
                    ->datas[allocation->head_record_index];
    /* The shared head definition's freshly calculated numRecords can be
     * larger than this instance's saved head span. Its exact saved RW indices
     * are restored after all CHRs have been rebuilt, so the saved model Type
     * and head offset are the authoritative per-instance bounds here. */
    head_data->ModelFileHeader = head_header;
    head_data->RwDatas = &model->datas[allocation->head_data_offset];
  }

  /* Pool slots can be larger than the allocation which existed when the state
   * was saved. Model traversal treats Type as the authoritative end of the
   * instance graph, so retain that exact logical bound after reconstruction. */
  model->Type = allocation->model_type;
  /* Shared definition RW indices are still canonical reconstruction values
   * here and can exceed this instance's saved head span. Their links and exact
   * indices are restored globally after every CHR has been reconstructed; do
   * not traverse them as though they were private to this instance. */
  return TRUE;
}

static s32 count_model_rw_scalars(ModelNode *node, s32 depth) {
  s32 count = 0;

  if (node == NULL || depth >= 64) {
    return 0;
  }
  while (node != NULL) {
    switch (node->Opcode & 0xff) {
    case MODELNODE_OPCODE_OP07:
    case MODELNODE_OPCODE_LOD:
    case MODELNODE_OPCODE_SWITCH:
    case MODELNODE_OPCODE_BSP:
    case MODELNODE_OPCODE_OP11:
    case MODELNODE_OPCODE_GUNFIRE:
      count++;
      break;
    }
    {
      ModelNode *child = model_rw_scalar_child(node);
      if (child != NULL) {
        count += count_model_rw_scalars(child, depth + 1);
      }
    }
    node = node->Next;
  }
  return count;
}

static void save_model_rw_scalars(StateStream *stream, ModelNode *node,
                                  union ModelRwData **datas,
                                  s32 rwdata_count, s32 depth) {
  if (node == NULL || depth >= 64) {
    return;
  }
  while (node != NULL) {
    u32 *value;

    if (model_rw_scalar_value(node, datas, rwdata_count, &value)) {
      write_u32(stream, *value);
    }
    {
      ModelNode *child = model_rw_scalar_child(node);
      if (child != NULL) {
        save_model_rw_scalars(stream, child, datas, rwdata_count, depth + 1);
      }
    }
    node = node->Next;
  }
}

static void load_model_rw_scalars(StateStream *stream, ModelNode *node,
                                  union ModelRwData **datas,
                                  s32 rwdata_count, s32 depth, s32 saved_count,
                                  s32 *loaded_count) {
  if (node == NULL || depth >= 64 || *loaded_count >= saved_count) {
    return;
  }
  while (node != NULL && *loaded_count < saved_count) {
    u32 *value;

    if (model_rw_scalar_value(node, datas, rwdata_count, &value)) {
      *value = read_u32(stream);
      (*loaded_count)++;
    }
    {
      ModelNode *child = model_rw_scalar_child(node);
      if (child != NULL) {
        load_model_rw_scalars(stream, child, datas, rwdata_count, depth + 1,
                              saved_count, loaded_count);
      }
    }
    node = node->Next;
  }
}

void practice_states_save_model_animation(StateStream *stream,
                                          const Model *model) {
  SavedModelAnimation saved;
  ModelRwData_HeaderRecord *root_data;

  bzero(&saved, sizeof(saved));
  saved.anim_offset = get_animation_offset(model->anim);
  saved.anim2_offset = get_animation_offset(model->anim2);
  saved.gunhand = model->gunhand;
  saved.unk25 = model->unk25;
  saved.animlooping = model->animlooping;
  saved.unk27 = model->unk27;
  saved.unk28 = model->unk28;
  saved.unk2c = model->unk2c;
  saved.framea = model->framea;
  saved.frameb = model->frameb;
  saved.unk34 = model->unk34;
  saved.unk38 = model->unk38;
  saved.endframe = model->endframe;
  saved.speed = model->speed;
  saved.newspeed = model->newspeed;
  saved.oldspeed = model->oldspeed;
  saved.timespeed = model->timespeed;
  saved.elapsespeed = model->elapsespeed;
  saved.unk58 = model->unk58;
  saved.unk5c = model->unk5c;
  saved.frame2a = model->frame2a;
  saved.frame2b = model->frame2b;
  saved.unk64 = model->unk64;
  saved.unk68 = model->unk68;
  saved.unk6c = model->unk6c;
  saved.speed2 = model->speed2;
  saved.unk74 = model->unk74;
  saved.unk78 = model->unk78;
  saved.unk7c = model->unk7c;
  saved.unk80 = model->unk80;
  saved.unk84 = model->unk84;
  saved.unk88 = model->unk88;
  saved.unk8c = model->unk8c;
  saved.animloopframe = model->animloopframe;
  saved.animloopmerge = model->animloopmerge;
  saved.unk9c = model->unk9c;
  saved.unka0 = model->unka0;
  saved.playspeed = model->playspeed;
  saved.animrate = model->animrate;
  saved.unkac = model->unkac;
  saved.unkb0 = model->unkb0;
  saved.unkb4 = model->unkb4;
  saved.unkb8 = model->unkb8;
  saved.unkbc = model->unkbc;
  saved.has_root_data =
      model->obj != NULL && model->obj->RootNode != NULL &&
      (model->obj->RootNode->Opcode & 0xff) == MODELNODE_OPCODE_HEADER;
  if (model->animflipfunc == (s32)bheadFlipAnimation) {
    saved.anim_flip_callback = 1;
  } else if (model->animflipfunc != 0) {
    practiceLogError("Unsupported model animation flip callback %08x",
                     model->animflipfunc);
  }

  write_animation_zero_rle(stream, (u8 *)&saved, sizeof(saved));
  if (saved.has_root_data) {
    root_data = (ModelRwData_HeaderRecord *)modelGetNodeRwData(
        (Model *)model, model->obj->RootNode);
    write_animation_zero_rle(stream, (u8 *)root_data, sizeof(*root_data));
  }
  {
    s32 rwdata_count = model->Type > 0
                           ? model->Type
                           : model->obj != NULL ? model->obj->numRecords : 0;
    s32 scalar_count = model->obj != NULL && model->datas != NULL &&
                               rwdata_count > 0
                           ? count_model_rw_scalars(model->obj->RootNode, 0)
                           : 0;
    write_u16(stream, scalar_count);
    if (scalar_count > 0) {
      save_model_rw_scalars(stream, model->obj->RootNode, model->datas,
                            rwdata_count, 0);
    }
  }
}

static void practice_states_load_model_animation_internal(
    StateStream *stream, Model *model, u32 *saved_root_data_offset) {
  SavedModelAnimation saved;
  ModelRwData_HeaderRecord root_data;

  read_animation_zero_rle(stream, (u8 *)&saved, sizeof(saved));
  if (saved.has_root_data) {
    if (saved_root_data_offset != NULL) {
      *saved_root_data_offset =
          stream->base_address + stream->total_processed;
    }
    read_animation_zero_rle(stream, (u8 *)&root_data, sizeof(root_data));
  }

  {
    s32 scalar_count = read_u16(stream);
    s32 loaded_count = 0;
    s32 rwdata_count = model != NULL && model->Type > 0
                           ? model->Type
                           : model != NULL && model->obj != NULL
                                 ? model->obj->numRecords
                                 : 0;

    if (model != NULL && model->obj != NULL && model->datas != NULL &&
        rwdata_count > 0) {
      load_model_rw_scalars(stream, model->obj->RootNode, model->datas,
                            rwdata_count, 0, scalar_count, &loaded_count);
    }
    while (loaded_count < scalar_count) {
      read_u32(stream);
      loaded_count++;
    }
  }

  if (model == NULL) {
    return;
  }

  model->anim = get_animation_by_offset(saved.anim_offset);
  model->anim2 = get_animation_by_offset(saved.anim2_offset);
  model->gunhand = saved.gunhand;
  model->unk25 = saved.unk25;
  model->animlooping = saved.animlooping;
  model->unk27 = saved.unk27;
  model->unk28 = saved.unk28;
  model->unk2c = saved.unk2c;
  model->framea = saved.framea;
  model->frameb = saved.frameb;
  /* These are decoded-frame cache slots. subcalcmatrices regenerates them
   * from anim/framea/frameb before consuming them. A value captured from a
   * previous model instance is not persistent animation state. */
  model->unk34 = 0;
  model->unk38 = 0;
  model->endframe = saved.endframe;
  model->speed = saved.speed;
  model->newspeed = saved.newspeed;
  model->oldspeed = saved.oldspeed;
  model->timespeed = saved.timespeed;
  model->elapsespeed = saved.elapsespeed;
  model->unk58 = saved.unk58;
  model->unk5c = saved.unk5c;
  model->frame2a = saved.frame2a;
  model->frame2b = saved.frame2b;
  model->unk64 = 0;
  model->unk68 = 0;
  model->unk6c = saved.unk6c;
  model->speed2 = saved.speed2;
  model->unk74 = saved.unk74;
  model->unk78 = saved.unk78;
  model->unk7c = saved.unk7c;
  model->unk80 = saved.unk80;
  model->unk84 = saved.unk84;
  model->unk88 = saved.unk88;
  model->unk8c = saved.unk8c;
  model->animloopframe = saved.animloopframe;
  model->animloopmerge = saved.animloopmerge;
  model->animflipfunc =
      saved.anim_flip_callback == 1 ? (s32)bheadFlipAnimation : 0;
  model->unk9c = saved.unk9c;
  model->unka0 = saved.unka0;
  model->playspeed = saved.playspeed;
  model->animrate = saved.animrate;
  model->unkac = saved.unkac;
  model->unkb0 = saved.unkb0;
  model->unkb4 = saved.unkb4;
  model->unkb8 = saved.unkb8;
  model->unkbc = saved.unkbc;

  if (saved.has_root_data && model->obj != NULL &&
      model->obj->RootNode != NULL &&
      (model->obj->RootNode->Opcode & 0xff) == MODELNODE_OPCODE_HEADER) {
    ModelRwData_HeaderRecord *dst =
        (ModelRwData_HeaderRecord *)modelGetNodeRwData(model,
                                                       model->obj->RootNode);
    *dst = root_data;
  }
}

void practice_states_load_model_animation(StateStream *stream, Model *model) {
  practice_states_load_model_animation_internal(stream, model, NULL);
}

bool practice_states_reload_model_root_data(StateStream *stream, Model *model,
                                            u32 saved_root_data_offset) {
  ModelRwData_HeaderRecord root_data;
  u32 resume_offset;

  if (stream == NULL || model == NULL || model->obj == NULL ||
      model->datas == NULL || model->obj->RootNode == NULL ||
      (model->obj->RootNode->Opcode & 0xff) != MODELNODE_OPCODE_HEADER ||
      saved_root_data_offset == 0) {
    return FALSE;
  }

  resume_offset = stream->base_address + stream->total_processed;
  stream_seek(stream, saved_root_data_offset);
  read_animation_zero_rle(stream, (u8 *)&root_data, sizeof(root_data));
  stream_seek(stream, resume_offset);
  *(ModelRwData_HeaderRecord *)modelGetNodeRwData(
      model, model->obj->RootNode) = root_data;
  return TRUE;
}

typedef struct ModelBloodNode {
  ModelNode *node;
  union ModelRwData **datas;
  u16 component_index;
  u16 rwdata_index;
} ModelBloodNode;

static bool has_model_blood_node(const ModelBloodNode *nodes, s32 node_count,
                                 ModelNode *node, union ModelRwData **datas) {
  s32 i;

  for (i = 0; i < node_count; i++) {
    if (nodes[i].node == node && nodes[i].datas == datas) {
      return TRUE;
    }
  }

  return FALSE;
}

static void
collect_model_blood_nodes(ModelNode *node, union ModelRwData **datas,
                          u16 component_index, ModelBloodNode *blood_nodes,
                          s32 *blood_node_count, s32 *nodes_seen, s32 depth) {
  if (depth >= MAX_MODEL_TRAVERSAL_DEPTH) {
    return;
  }

  while (node != NULL && *nodes_seen < MAX_MODEL_TRAVERSAL_NODES) {
    union ModelRwData **child_datas = datas;
    u16 child_component_index = component_index;
    ModelNode *child = node->Child;

    (*nodes_seen)++;

    if ((node->Opcode & 0xff) == MODELNODE_OPCODE_DLCOLLISION &&
        *blood_node_count < MAX_MODEL_BLOOD_NODES &&
        !has_model_blood_node(blood_nodes, *blood_node_count, node, datas)) {
      blood_nodes[*blood_node_count].node = node;
      blood_nodes[*blood_node_count].datas = datas;
      blood_nodes[*blood_node_count].component_index = component_index;
      blood_nodes[*blood_node_count].rwdata_index =
          node->Data->DisplayListCollisions.RwDataIndex;
      (*blood_node_count)++;
    }

    if ((node->Opcode & 0xff) == MODELNODE_OPCODE_LOD) {
      child = node->Data->LOD.Affects;
    } else if ((node->Opcode & 0xff) == MODELNODE_OPCODE_SWITCH) {
      child = node->Data->Switch.Controls;
    } else if ((node->Opcode & 0xff) == MODELNODE_OPCODE_BSP) {
      ModelNode *left_child = node->Data->BSP.leftChild;
      ModelNode *right_child = node->Data->BSP.rightChild;

      if (left_child != NULL) {
        collect_model_blood_nodes(left_child, datas, component_index,
                                  blood_nodes, blood_node_count, nodes_seen,
                                  depth + 1);
      }
      child = right_child;
    } else if ((node->Opcode & 0xff) == MODELNODE_OPCODE_HEAD &&
               datas != NULL) {
      struct ModelRoData_HeadPlaceholderRecord *rodata =
          &node->Data->HeadPlaceholder;
      struct ModelRwData_HeadPlaceholderRecord *rwdata =
          (struct ModelRwData_HeadPlaceholderRecord
               *)&datas[rodata->RwDataIndex];

      child = NULL;
      if (rwdata->RwDatas != NULL) {
        child_datas = (union ModelRwData **)rwdata->RwDatas;
        child_component_index = rodata->RwDataIndex + 1;
        if (rwdata->ModelFileHeader != NULL) {
          child = rwdata->ModelFileHeader->RootNode;
        }
      }
    }

    if (child != NULL) {
      collect_model_blood_nodes(child, child_datas, child_component_index,
                                blood_nodes, blood_node_count, nodes_seen,
                                depth + 1);
    }

    node = node->Next;
  }
}

static s32 get_model_blood_nodes(Model *model, ModelBloodNode *blood_nodes) {
  s32 blood_node_count = 0;
  s32 nodes_seen = 0;

  if (model != NULL && model->obj != NULL && model->obj->RootNode != NULL &&
      model->datas != NULL) {
    collect_model_blood_nodes(model->obj->RootNode, model->datas, 0,
                              blood_nodes, &blood_node_count, &nodes_seen, 0);
  }

  return blood_node_count;
}

static ModelBloodNode *find_model_blood_node(ModelBloodNode *blood_nodes,
                                             s32 blood_node_count,
                                             u16 component_index,
                                             u16 rwdata_index) {
  s32 i;

  for (i = 0; i < blood_node_count; i++) {
    if (blood_nodes[i].component_index == component_index &&
        blood_nodes[i].rwdata_index == rwdata_index) {
      return &blood_nodes[i];
    }
  }

  return NULL;
}

static bool model_node_has_blood_patch(const ModelBloodNode *blood_node) {
  struct ModelRoData_DisplayList_CollisionRecord *rodata;
  struct ModelRwData_DisplayList_CollisionRecord *rwdata;
  ModelNode *node = blood_node->node;

  if (node == NULL || blood_node->datas == NULL ||
      (node->Opcode & 0xff) != MODELNODE_OPCODE_DLCOLLISION) {
    return FALSE;
  }

  rodata = (struct ModelRoData_DisplayList_CollisionRecord *)node->Data;
  rwdata = (struct ModelRwData_DisplayList_CollisionRecord *)&blood_node
               ->datas[rodata->RwDataIndex];

  return rodata != NULL && rwdata != NULL && rodata->Vertices != NULL &&
         dword_CODE_bss_8007A0E0 != NULL && dword_CODE_bss_8007A0D0 > 0 &&
         rwdata->Vertices >= dword_CODE_bss_8007A0E0 &&
         rwdata->Vertices < &dword_CODE_bss_8007A0E0[dword_CODE_bss_8007A0D0];
}

static void save_model_blood_patches(StateStream *stream, Model *model) {
  ModelBloodNode blood_nodes[MAX_MODEL_BLOOD_NODES];
  u16 patch_count = 0;
  s32 blood_node_count;
  s32 i;

  blood_node_count = get_model_blood_nodes(model, blood_nodes);

  for (i = 0; i < blood_node_count; i++) {
    if (model_node_has_blood_patch(&blood_nodes[i])) {
      patch_count++;
    }
  }

  write_u16(stream, patch_count);

  for (i = 0; i < blood_node_count; i++) {
    if (model_node_has_blood_patch(&blood_nodes[i])) {
      ModelNode *node = blood_nodes[i].node;
      struct ModelRoData_DisplayList_CollisionRecord *rodata =
          (struct ModelRoData_DisplayList_CollisionRecord *)node->Data;
      struct ModelRwData_DisplayList_CollisionRecord *rwdata =
          (struct ModelRwData_DisplayList_CollisionRecord *)&blood_nodes[i]
              .datas[rodata->RwDataIndex];

      write_u16(stream, blood_nodes[i].component_index);
      write_u16(stream, blood_nodes[i].rwdata_index);
      write_u16(stream, (u16)rodata->numVertices);
      {
        s32 vertex;
        for (vertex = 0; vertex < rodata->numVertices; vertex++) {
          write_u8(stream, rwdata->Vertices[vertex].a);
        }
      }
    }
  }
}

static void clear_model_blood_patches(ModelBloodNode *blood_nodes,
                                      s32 blood_node_count) {
  s32 i;

  for (i = 0; i < blood_node_count; i++) {
    if (model_node_has_blood_patch(&blood_nodes[i])) {
      ModelNode *node = blood_nodes[i].node;
      struct ModelRoData_DisplayList_CollisionRecord *rodata =
          (struct ModelRoData_DisplayList_CollisionRecord *)node->Data;
      struct ModelRwData_DisplayList_CollisionRecord *rwdata =
          (struct ModelRwData_DisplayList_CollisionRecord *)&blood_nodes[i]
              .datas[rodata->RwDataIndex];

      sub_GAME_7F09C044(rwdata->Vertices);
      rwdata->Vertices = rodata->Vertices;
    }
  }
}

void clear_chr_model_blood_patches(ChrRecord *chr) {
  ModelBloodNode blood_nodes[MAX_MODEL_BLOOD_NODES];
  s32 blood_node_count;

  if (chr == NULL || chr->model == NULL) {
    return;
  }

  blood_node_count = get_model_blood_nodes(chr->model, blood_nodes);
  clear_model_blood_patches(blood_nodes, blood_node_count);
}

void reset_chr_blood_vertex_pool(void) {
  s32 i;

  if (dword_CODE_bss_8007A0E8 == NULL || dword_CODE_bss_8007A0E0 == NULL ||
      dword_CODE_bss_8007A0D4 <= 0 || dword_CODE_bss_8007A0D0 <= 0) {
    return;
  }

  word_CODE_bss_8007A0F0 = (s16)dword_CODE_bss_8007A0D0;
  dword_CODE_bss_8007A0E8[0].vertices = dword_CODE_bss_8007A0E0;
  dword_CODE_bss_8007A0E8[0].count = (s16)dword_CODE_bss_8007A0D0;
  dword_CODE_bss_8007A0E8[0].ref_count = 0;
  dword_CODE_bss_8007A0E8[0].next = -1;
  dword_CODE_bss_8007A0E8[0].previous = -1;

  for (i = 1; i < dword_CODE_bss_8007A0D4; i++) {
    dword_CODE_bss_8007A0E8[i].ref_count = -1;
  }
}

static void load_model_blood_patches(StateStream *stream, Model *model) {
  ModelBloodNode blood_nodes[MAX_MODEL_BLOOD_NODES];
  s32 blood_node_count = get_model_blood_nodes(model, blood_nodes);
  u16 patch_count = read_u16(stream);
  u16 patch;

  /* The props loader releases all live clones and resets the shared pool
   * before loading any CHR records. Clearing an individual recreated model
   * here can free a stale vertex pointer into that freshly reset pool. */
  for (patch = 0; patch < patch_count; patch++) {
    u16 component_index = read_u16(stream);
    u16 rwdata_index = read_u16(stream);
    u16 vertex_count = read_u16(stream);
    ModelBloodNode *blood_node = find_model_blood_node(
        blood_nodes, blood_node_count, component_index, rwdata_index);
    ModelNode *node = blood_node != NULL ? blood_node->node : NULL;
    struct ModelRoData_DisplayList_CollisionRecord *rodata = NULL;
    struct ModelRwData_DisplayList_CollisionRecord *rwdata = NULL;
    Vertex *vertices = NULL;

    if (node != NULL && (node->Opcode & 0xff) == MODELNODE_OPCODE_DLCOLLISION) {
      rodata = (struct ModelRoData_DisplayList_CollisionRecord *)node->Data;
      rwdata = (struct ModelRwData_DisplayList_CollisionRecord *)&blood_node
                   ->datas[rodata->RwDataIndex];
    }

    if (rodata != NULL && rwdata != NULL && rodata->Vertices != NULL &&
        rodata->numVertices == vertex_count) {
      vertices = sub_GAME_7F09BE4C(vertex_count, 0xcccc, NULL, 0);
    }

    if (vertices != NULL) {
      s32 vertex;
      memcpy(vertices, rodata->Vertices, vertex_count * sizeof(Vertex));
      for (vertex = 0; vertex < vertex_count; vertex++) {
        vertices[vertex].a = read_u8(stream);
      }
      rwdata->Vertices = vertices;
    } else {
      stream_seek(stream, stream->base_address + stream->total_processed +
                              vertex_count);
      practiceLogWarn(
          "Could not restore blood patch %d (component=%d, rwdata=%d, "
          "vertices=%d)",
          patch, component_index, rwdata_index, vertex_count);
    }
  }
}

static void clear_chr_transient_sounds(ChrRecord *chr) {
  ALSoundState **sounds = (ALSoundState **)&chr->weapons_held[2];
  s32 i;

  /* Assembly in sub_GAME_7F02BFE4 indexes four ALSoundState pointers from
   * offset 0x168 as two slots per hand. The current ChrRecord declaration
   * misnames the first two words as weapons_held[2] and fireslot_word. */
  for (i = 0; i < 4; i++) {
    if (sounds[i] != NULL && sndGetPlayingState(sounds[i]) != 0) {
      sndDeactivate(sounds[i]);
    }
    sounds[i] = NULL;
  }
}

static void clear_chr_transient_joint_list(ChrRecord *chr) {
  if (chr->field_20 != NULL) {
    sub_GAME_7F06B248(chr->field_20);
    chr->field_20 = NULL;
  }
}

void save_chr_record(StateStream *stream, const ChrRecord *chr) {
  StateStream *storage_stream = stream;
  ChrCommonStream common_stream;
  s32 ailist_reference;
  u16 head_record_index = 0xffff;
  u16 head_data_offset = 0xffff;

  bool supported_action = is_supported_chr_action(chr);
  bool has_model_transform =
      chr->model != NULL && chr->model->obj != NULL &&
      chr->model->obj->RootNode != NULL &&
      (chr->model->obj->RootNode->Opcode & 0xff) == MODELNODE_OPCODE_HEADER;
  f32 model_heading = has_model_transform
                          ? normalize_chr_heading(getsubroty(chr->model))
                          : 0.0f;

  if (chr->model != NULL && chr->model->obj != NULL &&
      chr->model->datas != NULL && chr->model->Type > 0 && chr->headnum >= 0 &&
      chr->model->obj == c_item_entries[chr->bodynum].header &&
      !c_item_entries[chr->bodynum].hasHead &&
      chr->model->obj->Switches != NULL &&
      chr->model->obj->numSwitches > 4 &&
      chr->model->obj->Switches[4] != NULL &&
      chr->model->obj->Switches[4]->Data != NULL) {
    ModelNode *head_node = chr->model->obj->Switches[4];
    ModelRwData_HeadPlaceholderRecord *head_data;
    u32 data_start = (u32)chr->model->datas;
    u32 data_end = data_start + chr->model->Type * sizeof(*chr->model->datas);
    u32 attached_data;

    head_record_index = head_node->Data->HeadPlaceholder.RwDataIndex;
    if (head_record_index + 1 < chr->model->Type) {
      head_data = (ModelRwData_HeadPlaceholderRecord *)&chr->model
                      ->datas[head_record_index];
      attached_data = (u32)head_data->RwDatas;
      if (head_data->ModelFileHeader == c_item_entries[chr->headnum].header &&
          attached_data >= data_start && attached_data < data_end &&
          (attached_data - data_start) % sizeof(*chr->model->datas) == 0) {
        head_data_offset =
            (attached_data - data_start) / sizeof(*chr->model->datas);
      } else {
        head_record_index = 0xffff;
      }
    } else {
      head_record_index = 0xffff;
    }
  }

  // Allocation metadata is consumed before the destination ChrRecord exists so
  // a missing CHR can be recreated in its saved slot before the payload loads.
  write_u16(stream, (u16)(chr - g_ChrSlots));
  write_u8(stream, (u8)chr->headnum);
  write_u8(stream, (u8)chr->bodynum);
  write_f32(stream, model_heading);
  write_u16(stream, chr->model != NULL && chr->model->Type > 0
                        ? (u16)chr->model->Type
                        : 0);
  write_u16(stream, head_record_index);
  write_u16(stream, head_data_offset);

  chr_common_stream_init(&common_stream, 0);
  stream = &common_stream.base;

  write_u8(stream, (u8)chr->accuracyrating);
  write_u8(stream, (u8)chr->speedrating);
  write_u8(stream, (u8)chr->arghrating);
  write_u8(stream, chr->grenadeprob);
  write_f32(stream, chr->visionrange);
  write_f32(stream, chr->hearingscale);
  write_u8(stream, chr->morale);
  write_u8(stream, chr->alertness);
  write_f32(stream, chr->chrwidth);
  write_f32(stream, chr->chrheight);
  write_u8(stream, (u8)chr->flinchcnt);
  write_u16(stream, chr->hidden & CHR_FLINCH_HIDDEN_MASK);
  write_u32(stream, chr->chrflags & CHR_BEHAVIOR_FLAGS_MASK);

  write_u8(stream, chr->fadealpha);
  write_f32(stream, chr->damage);
  write_f32(stream, chr->maxdamage);
  write_u32(stream, chr->chrflags & CHR_DAMAGE_FLAGS_MASK);

  write_u8(stream, (u8)chr->numarghs);
  write_u8(stream, (u8)chr->numclosearghs);
  write_u8(stream, chr->random);
  write_u16(stream, (u16)chr->padpreset1);
  write_u16(stream, (u16)chr->chrpreset1);
  write_u16(stream, (u16)chr->chrseeshot);
  write_u16(stream, (u16)chr->chrseedie);
  write_u16(stream, chr->hidden & CHR_LIFECYCLE_HIDDEN_MASK);

  write_u32(stream, chr->lastseetarget60);
  write_bytes(stream, &chr->lastknowntargetpos, sizeof(coord3d));
  write_u32(stream, get_tile_offset((StandTile *)chr->targetTile));
  write_u32(stream, chr->seen_bond_time);
  write_u32(stream, chr->lastheartarget60);

  write_u16(stream, (u16)chr->chrnum);
  write_u8(stream, chr->flags2);
  write_u32(stream, chr->timer60);
  write_u8(stream, (chr->hidden & CHRHIDDEN_TIMER_ACTIVE) != 0);
  write_f32(stream, chr->shotbondsum);
  write_u8(stream, chr->firecount[0]);
  write_u8(stream, chr->firecount[1]);
  write_u8(stream, (u8)chr->aimendcount);
  write_bytes(stream, &chr->collision_bounds, sizeof(chr->collision_bounds));
  write_f32(stream, chr->aimuplshoulder);
  write_f32(stream, chr->aimuprshoulder);
  write_f32(stream, chr->aimupback);
  write_f32(stream, chr->aimsideback);
  write_f32(stream, chr->aimendlshoulder);
  write_f32(stream, chr->aimendrshoulder);
  write_f32(stream, chr->aimendback);
  write_f32(stream, chr->aimendsideback);
  /* Offset 0x16c is a transient sound pointer, not persistent firing state. */
  write_u32(stream, 0);
  write_u32(stream, chr->field_178[0]);
  write_u32(stream, chr->field_178[1]);
  write_bytes(stream, chr->unk180, sizeof(chr->unk180));
  write_u16(stream, chr->hidden & CHR_COMBAT_HIDDEN_MASK);
  write_bytes(stream, &chr->shadecol, sizeof(rgba_u8));
  write_bytes(stream, &chr->nextcol, sizeof(rgba_u8));

  ailist_reference = get_ai_list_reference(chr->ailist);
  write_u32(stream, ailist_reference);
  write_u16(stream, chr->aioffset);
  write_u16(stream, (u16)chr->aireturnlist);
  write_u8(stream, (u8)chr->sleep);

  write_u8(stream, (u8)chr->invalidmove);
  write_f32(stream, chr->sumground);
  write_f32(stream, chr->manground);
  write_f32(stream, chr->ground);
  write_bytes(stream, &chr->fallspeed, sizeof(coord3d));
  write_bytes(stream, &chr->prevpos, sizeof(coord3d));
  write_u32(stream, chr->lastwalk60);
  write_u32(stream, chr->lastmoveok60);

  write_u8(stream, has_model_transform);
  if (has_model_transform) {
    coord3d model_offset;

    getsuboffset(chr->model, &model_offset);
    write_bytes(stream, &model_offset, sizeof(coord3d));
    write_f32(stream, model_heading);
  }

  stream = storage_stream;
  write_chr_common_rle(stream, common_stream.bytes,
                       common_stream.base.total_processed);

  write_u8(stream, supported_action);
  if (supported_action) {
    save_supported_action(stream, chr);
    write_u8(stream, chr->model != NULL);
    if (chr->model != NULL) {
      practice_states_save_model_animation(stream, chr->model);
    }
  }

  save_model_blood_patches(stream, chr->model);

  write_u16(stream, get_prop_index(chr->weapons_held[0]));
  write_u16(stream, get_prop_index(chr->weapons_held[1]));
  /* The third legacy slot has no readers in GoldenEye and can retain a stale
   * pointer while equipment is being rebuilt. Keep the on-disk slot for
   * format compatibility, but normalize this derived/unused state. */
  write_u16(stream, (u16)-1);
  write_u16(stream, get_prop_index(chr->handle_positiondata_hat));

  {
    s32 hand;
    for (hand = 0; hand < 2; hand++) {
      PropRecord *prop = chr->weapons_held[hand];
      WeaponObjRecord *weapon = prop != NULL && prop->obj != NULL &&
                                        prop->obj->type == PROPDEF_COLLECTABLE
                                    ? prop->weapon
                                    : NULL;
      write_u16(stream, weapon != NULL ? (u16)weapon->obj : (u16)-1);
      write_u8(stream, weapon != NULL ? (u8)weapon->weaponnum : 0);
      write_u32(stream, weapon != NULL ? weapon->flags : 0);
      // Muzzle flash (GUNFIRE node) visibility is latched in the weapon model's
      // RwData when the CHR fires and only cleared on stop-firing, so it must
      // be saved alongside the held weapon rather than re-derived on load.
      write_u8(stream, (u8)(prop != NULL ? weaponIsGunfireVisible(prop) : 0));
    }
  }

  if (chr->handle_positiondata_hat != NULL &&
      chr->handle_positiondata_hat->obj != NULL &&
      chr->handle_positiondata_hat->obj->type == PROPDEF_HAT) {
    write_u16(stream, (u16)chr->handle_positiondata_hat->obj->obj);
    write_u32(stream, chr->handle_positiondata_hat->obj->flags);
  } else {
    write_u16(stream, (u16)-1);
    write_u32(stream, 0);
  }
}

void load_chr_allocation_state(StateStream *stream,
                               ChrAllocationState *allocation) {
  allocation->slot_index = (s16)read_u16(stream);
  allocation->headnum = (s8)read_u8(stream);
  allocation->bodynum = (s8)read_u8(stream);
  allocation->heading = normalize_chr_heading(read_f32(stream));
  allocation->model_type = read_u16(stream);
  allocation->head_record_index = read_u16(stream);
  allocation->head_data_offset = read_u16(stream);
}

void load_chr_record(StateStream *stream, ChrRecord *chr,
                     ChrAttachmentIndices *attachments,
                     u32 *saved_root_data_offset) {
  StateStream *storage_stream = stream;
  ChrCommonStream common_stream;
  s32 ailist_reference;
  bool has_model_transform;
  bool loaded_model_animation = FALSE;
  coord3d model_offset;
  f32 model_heading;
  s16 weapon_indices[3];
  s16 hat_index;
  s32 hand;

  if (saved_root_data_offset != NULL) {
    *saved_root_data_offset = 0;
  }

  clear_chr_transient_joint_list(chr);

  read_chr_common_rle(stream, &common_stream);
  stream = &common_stream.base;

  chr->accuracyrating = (s8)read_u8(stream);
  chr->speedrating = (s8)read_u8(stream);
  chr->arghrating = (s8)read_u8(stream);
  chr->grenadeprob = read_u8(stream);
  chr->visionrange = read_f32(stream);
  chr->hearingscale = read_f32(stream);
  chr->morale = read_u8(stream);
  chr->alertness = read_u8(stream);
  chr->chrwidth = read_f32(stream);
  chr->chrheight = read_f32(stream);
  chr->flinchcnt = (s8)read_u8(stream);
  chr->hidden = (chr->hidden & ~CHR_FLINCH_HIDDEN_MASK) |
                ((u16)read_u16(stream) & CHR_FLINCH_HIDDEN_MASK);
  chr->chrflags = (chr->chrflags & ~CHR_BEHAVIOR_FLAGS_MASK) |
                  (read_u32(stream) & CHR_BEHAVIOR_FLAGS_MASK);

  chr->fadealpha = read_u8(stream);
  chr->damage = read_f32(stream);
  chr->maxdamage = read_f32(stream);
  chr->chrflags = (chr->chrflags & ~CHR_DAMAGE_FLAGS_MASK) |
                  (read_u32(stream) & CHR_DAMAGE_FLAGS_MASK);

  chr->numarghs = (s8)read_u8(stream);
  chr->numclosearghs = (s8)read_u8(stream);
  chr->random = read_u8(stream);
  chr->padpreset1 = (s16)read_u16(stream);
  chr->chrpreset1 = (s16)read_u16(stream);
  chr->chrseeshot = (s16)read_u16(stream);
  chr->chrseedie = (s16)read_u16(stream);
  chr->hidden = (chr->hidden & ~CHR_LIFECYCLE_HIDDEN_MASK) |
                ((u16)read_u16(stream) & CHR_LIFECYCLE_HIDDEN_MASK);

  chr->lastseetarget60 = read_u32(stream);
  read_bytes(stream, &chr->lastknowntargetpos, sizeof(coord3d));
  chr->targetTile = get_tile_by_offset((s32)read_u32(stream));
  chr->seen_bond_time = read_u32(stream);
  chr->lastheartarget60 = read_u32(stream);

  chr->chrnum = (s16)read_u16(stream);
  chr->flags2 = read_u8(stream);
  chr->timer60 = read_u32(stream);
  if (read_u8(stream)) {
    chr->hidden |= CHRHIDDEN_TIMER_ACTIVE;
  } else {
    chr->hidden &= ~CHRHIDDEN_TIMER_ACTIVE;
  }
  chr->shotbondsum = read_f32(stream);
  chr->firecount[0] = read_u8(stream);
  chr->firecount[1] = read_u8(stream);
  chr->aimendcount = (s8)read_u8(stream);
  read_bytes(stream, &chr->collision_bounds, sizeof(chr->collision_bounds));
  chr->aimuplshoulder = read_f32(stream);
  chr->aimuprshoulder = read_f32(stream);
  chr->aimupback = read_f32(stream);
  chr->aimsideback = read_f32(stream);
  chr->aimendlshoulder = read_f32(stream);
  chr->aimendrshoulder = read_f32(stream);
  chr->aimendback = read_f32(stream);
  chr->aimendsideback = read_f32(stream);
  clear_chr_transient_sounds(chr);
  read_u32(stream);
  chr->fireslot_word = 0;
  chr->field_178[0] = read_u32(stream);
  chr->field_178[1] = read_u32(stream);
  read_bytes(stream, chr->unk180, sizeof(chr->unk180));
  chr->hidden = (chr->hidden & ~CHR_COMBAT_HIDDEN_MASK) |
                ((u16)read_u16(stream) & CHR_COMBAT_HIDDEN_MASK);
  read_bytes(stream, &chr->shadecol, sizeof(rgba_u8));
  read_bytes(stream, &chr->nextcol, sizeof(rgba_u8));

  ailist_reference = read_u32(stream);
  chr->ailist = get_ai_list_by_reference(ailist_reference);
  chr->aioffset = read_u16(stream);
  chr->aireturnlist = (s16)read_u16(stream);
  chr->sleep = (s8)read_u8(stream);

  if (chr->ailist == NULL) {
    chr->aioffset = 0;
    chr->aireturnlist = -1;
  }

  chr->invalidmove = (s8)read_u8(stream);
  chr->sumground = read_f32(stream);
  chr->manground = read_f32(stream);
  chr->ground = read_f32(stream);
  read_bytes(stream, &chr->fallspeed, sizeof(coord3d));
  read_bytes(stream, &chr->prevpos, sizeof(coord3d));
  chr->lastwalk60 = read_u32(stream);
  chr->lastmoveok60 = read_u32(stream);
  has_model_transform = read_u8(stream);
  if (has_model_transform) {
    read_bytes(stream, &model_offset, sizeof(coord3d));
    model_heading = normalize_chr_heading(read_f32(stream));
  }

  if (stream->total_processed != common_stream.size) {
    practiceLogError("Saved CHR common state has %d unread bytes",
                     common_stream.size - stream->total_processed);
    assert(FALSE);
  }
  stream = storage_stream;

  if (read_u8(stream)) {
    load_supported_action(stream, chr);
    if (read_u8(stream)) {
      practice_states_load_model_animation_internal(
          stream, chr != NULL ? chr->model : NULL, saved_root_data_offset);
      loaded_model_animation = TRUE;
    }
  }

  if (has_model_transform && chr->model != NULL && chr->model->obj != NULL &&
      chr->model->obj->RootNode != NULL) {
    ModelNode *root = chr->model->obj->RootNode;

    if (!loaded_model_animation &&
        (root->Opcode & 0xff) == MODELNODE_OPCODE_HEADER) {
      ModelRwData_HeaderRecord *root_data =
          (ModelRwData_HeaderRecord *)modelGetNodeRwData(chr->model, root);

      bzero(root_data, sizeof(*root_data));
    }
    setsuboffset(chr->model, &model_offset);
    setsubroty(chr->model, model_heading);
  }

  load_model_blood_patches(stream, chr->model);

  weapon_indices[0] = (s16)read_u16(stream);
  weapon_indices[1] = (s16)read_u16(stream);
  weapon_indices[2] = (s16)read_u16(stream);
  hat_index = (s16)read_u16(stream);

  if (attachments != NULL) {
    attachments->weapons_held[0] = weapon_indices[0];
    attachments->weapons_held[1] = weapon_indices[1];
    attachments->weapons_held[2] = weapon_indices[2];
    attachments->hat = hat_index;
  }

  for (hand = 0; hand < 2; hand++) {
    s16 model = (s16)read_u16(stream);
    s8 weaponnum = (s8)read_u8(stream);
    u32 flags = read_u32(stream);
    s8 gunfire_visible = (s8)read_u8(stream);

    if (attachments != NULL) {
      attachments->weapon_model[hand] = model;
      attachments->weaponnum[hand] = weaponnum;
      attachments->weapon_flags[hand] = flags;
      attachments->gunfire_visible[hand] = gunfire_visible;
    }
  }

  {
    s16 model = (s16)read_u16(stream);
    u32 flags = read_u32(stream);

    if (attachments != NULL) {
      attachments->hat_model = model;
      attachments->hat_flags = flags;
    }
  }
}

void load_chr_prop_spatial_state(PropRecord *prop, const coord3d *pos,
                                 s32 stan_offset, const u8 rooms[4]) {
  ChrRecord *chr = prop->chr;

  prop->pos = *pos;
  prop->stan = get_tile_by_offset(stan_offset);
  prop->rooms[0] = rooms[0];
  prop->rooms[1] = rooms[1];
  prop->rooms[2] = rooms[2];
  prop->rooms[3] = rooms[3];

  /* collision_bounds is derived from the prop position, but the CHR payload
   * loads before the prop's saved spatial state. Rebuild it only after the
   * saved position is installed so collision tests do not use stale bounds. */
  if (chr != NULL) {
    chr->collision_bounds.f[0] = prop->pos.x + chr->chrwidth;
    chr->collision_bounds.f[1] = prop->pos.z;
    chr->collision_bounds.f[2] = prop->pos.x;
    chr->collision_bounds.f[3] = prop->pos.z + chr->chrwidth;
    chr->collision_bounds.f[4] = prop->pos.x - chr->chrwidth;
    chr->collision_bounds.f[5] = prop->pos.z;
    chr->collision_bounds.f[6] = prop->pos.x;
    chr->collision_bounds.f[7] = prop->pos.z - chr->chrwidth;
  }
}
