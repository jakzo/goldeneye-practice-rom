#include "practice_tests.h"
#include "bg.h"
#include "bondview.h"
#include "chr.h"
#include "chrai.h"
#include "chrobjhandler.h"
#include "emu_log.h"
#include "gun.h"
#include "joy.h"
#include "objecthandler.h"
#include "player.h"
#include "practice_timescale.h"
#include "state/practice_states.h"
#include "state/practice_states_utils.h"
#include <bondgame.h>
#include <string.h>
#include <ultra64.h>

extern s32 g_ClockTimer;
extern s32 chraiGetAIListID(AIRecord *AIList, bool *isGlobalAIList);
extern u64 g_randomSeed;
extern u64 g_chrObjRandomSeed;
extern u64 g_tlbRandomSeed;
extern s32 propdoorInteract(PropRecord *doorprop);
extern void propExecuteTickOperation(PropRecord *prop, INV_ITEM_TYPE op);
extern PropRecord *hatCreateForChr(ChrRecord *chr, s32 modelnum, u32 flags);
extern ModelNode *sub_GAME_7F04B478(ObjectRecord *obj);
extern bool sub_GAME_7F04B590(ModelFileHeader *header, ModelNode *node);
extern void objDeform(ObjectRecord *obj, s32 destroyed_level);

// --- start test cases ---
#define STATE_DOOR 1
#define STATE_GRENADE 2
#define STATE_BUNKER 3
#define STATE_DAM 4
#define FIRE_SLOWMO 5
#define RNG_LOAD 6
#define STATE_ARCHIVES_KEY 7
#define STATE_TRAIN_HATCH 8
#define STATE_CAVERNS_ATTACHMENTS 9
#define STATE_CORRUPT_FREELIST 10
#define TEST_MOVE_SPEED 11
#define STATE_TINTED_GLASS_PORTAL 12
#define STATE_DESTROYED_PROP 13
// --- end test cases ---

// Left out of test cases since it cannot assert
#define CRASH 14

#define MAX_TEST_TINTED_GLASS 32

static s32 g_save_test_timer = -1;
static u32 case_delta = 0;
static s32 g_practice_test_case;
static s32 g_TestMoveCaseIndex = -1;
static f32 g_TestMoveStartX;
static f32 g_TestMoveStartZ;
static s32 g_mission_timer_start;

void practice_tests_set_case(s32 test_case) {
  g_practice_test_case = test_case;
  g_save_test_timer = -1;
  case_delta = 0;
  g_TestMoveCaseIndex = -1;
  g_TestMoveStartX = 0.0f;
  g_TestMoveStartZ = 0.0f;
}

s32 practice_tests_boot_level(s32 test_case) {
  switch (test_case) {
  case STATE_DOOR:
  case STATE_GRENADE:
  case FIRE_SLOWMO:
  case RNG_LOAD:
  case STATE_CORRUPT_FREELIST:
  case STATE_DESTROYED_PROP:
  case CRASH:
    return LEVELID_RUNWAY;
  case STATE_BUNKER:
    return LEVELID_BUNKER1;
  case STATE_DAM:
    return LEVELID_DAM;
  case STATE_ARCHIVES_KEY:
    return LEVELID_ARCHIVES;
  case STATE_TRAIN_HATCH:
    return LEVELID_TRAIN;
  case STATE_CAVERNS_ATTACHMENTS:
    return LEVELID_CAVERNS;
  case STATE_TINTED_GLASS_PORTAL:
    return LEVELID_CONTROL;
  case TEST_MOVE_SPEED:
    return LEVELID_TEST;
  default:
    return LEVELID_NONE;
  }
}

#define MAX_SAVED_TEST_CHILDREN 8

typedef struct TestChildLink {
  s16 index;
  s16 prev;
  s16 next;
} TestChildLink;

static s32 count_chr_child_links(ChrRecord *chr) {
  PropRecord *child;
  PropRecord *newer = NULL;
  s32 count = 0;

  if (chr == NULL || chr->prop == NULL) {
    return -1;
  }

  for (child = chr->prop->child; child != NULL; child = child->prev) {
    if (count++ >= POS_DATA_ENTRY_LEN || child->parent != chr->prop ||
        child->next != newer) {
      return -1;
    }
    newer = child;
  }

  return count;
}

static bool after_frames(u32 num_frames) {
  case_delta += num_frames;
  return g_save_test_timer == case_delta;
}

#if defined(VERSION_EU)
#define TEST_MOVE_SPEED_MAX_TIME60 150
#else
#define TEST_MOVE_SPEED_MAX_TIME60 180
#endif

typedef struct TestMoveCase {
  s32 lag_chance_numerator;
  s32 lag_chance_denominator;
  s32 base_delta_frames;
  s32 expected_ticks;
} TestMoveCase;
static struct TestMoveCase g_TestMoveCases[] = {
    {0, 10, 1},

    // TODO: This is not deterministic for some reason
    // {0, 10, 1, 751},
    // {0, 10, 2, 764},

    // {0, 10, 1},  {1, 10, 1},  {2, 10, 1},  {4, 10, 1},  {6, 10, 1},
    // {8, 10, 1},  {0, 10, 2},  {5, 10, 2},  {0, 10, 3},  {5, 10, 3},
    // {0, 10, 4},  {0, 10, 5},  {0, 10, 6},  {0, 10, 7},  {0, 10, 8},
    // {0, 10, 9},  {0, 10, 10}, {0, 10, 11}, {0, 10, 12}, {0, 10, 13},
    // {0, 10, 14}, {0, 10, 15},
};
static TestMoveCase get_test_move_case(s32 index) {
  if (index < 0 || index >= ARRAYCOUNT(g_TestMoveCases)) {
    TestMoveCase empty = {0, 0, 0, 0};
    return empty;
  }
  return g_TestMoveCases[index];

  // Endless random test cases
  // u32 random = ((u32)index * 1664525u + 1013904223u) >> 16;
  // TestMoveCase test_case = {random % 10, 10, (random / 10) % 30 + 1, 0};
  // return test_case;
}

void practice_tests_tick() {
#ifdef DEV
  if (g_practice_test_case == 0) {
    return;
  }

  if (g_save_test_timer == -1 && g_CameraMode == CAMERAMODE_FP) {
    g_save_test_timer = 0;
    emu_log("TEST_STARTED");
  }

  if (g_save_test_timer < 0)
    return;

  g_save_test_timer++;
  case_delta = 0;

  switch (g_practice_test_case) {
  case STATE_DESTROYED_PROP: {
    static s16 saved_prop_index;
    static u32 saved_vertex_hash;
    PropRecord *prop;
    ObjectRecord *obj;
    ModelNode *node;
    struct ModelRoData_DisplayList_CollisionRecord *rodata;
    struct ModelRwData_DisplayList_CollisionRecord *rwdata;
    u32 vertex_hash;
    s32 i;

    if (after_frames(30)) {
      saved_prop_index = -1;

      for (prop = ptr_obj_pos_list_first_entry; prop != NULL;
           prop = prop->next) {
        if (prop->type != PROP_TYPE_OBJ || prop->obj == NULL ||
            prop->obj->prop != prop || prop->obj->model == NULL ||
            prop->obj->model->obj == NULL ||
            (prop->obj->flags2 & PROPFLAG2_00002000)) {
          continue;
        }

        node = sub_GAME_7F04B478(prop->obj);
        if (node != NULL &&
            (node->Opcode & 0xff) == MODELNODE_OPCODE_DLCOLLISION &&
            sub_GAME_7F04B590(prop->obj->model->obj, node)) {
          break;
        }
      }

      if (prop == NULL) {
        emu_log("DEFORMABLE_PROP_NOT_FOUND");
        emu_log("TEST_FAILED");
        break;
      }

      obj = prop->obj;
      obj->state |= PROPSTATE_DESTROYED;
      obj->maxdamage = 0.0f;
      objDeform(obj, 1);

      node = sub_GAME_7F04B478(obj);
      rodata = (struct ModelRoData_DisplayList_CollisionRecord *)node->Data;
      rwdata =
          (struct ModelRwData_DisplayList_CollisionRecord *)modelGetNodeRwData(
              obj->model, node);
      if (rwdata == NULL || rwdata->Vertices == NULL ||
          rwdata->Vertices == rodata->Vertices) {
        emu_log("PROP_NOT_DEFORMED");
        emu_log("TEST_FAILED");
        break;
      }

      vertex_hash = 2166136261u;
      for (i = 0; i < rodata->numVertices * sizeof(Vertex); i++) {
        vertex_hash = (vertex_hash ^ ((u8 *)rwdata->Vertices)[i]) * 16777619u;
      }
      saved_vertex_hash = vertex_hash;
      saved_prop_index = get_prop_index(prop);
      emu_log("TRIGGER_SAVE");
      save_game_state();
      emu_log("SAVE_DONE");
    } else if (after_frames(2)) {
      prop = get_prop_by_index(saved_prop_index);
      obj = prop != NULL ? prop->obj : NULL;
      if (obj != NULL) {
        obj->maxdamage = 4.0f;
        objDeform(obj, 2);
      }
    } else if (after_frames(2)) {
      emu_log("TRIGGER_LOAD");
      load_game_state();
      emu_log("LOAD_DONE");
    } else if (after_frames(2)) {
      prop = get_prop_by_index(saved_prop_index);
      obj = prop != NULL ? prop->obj : NULL;
      node = obj != NULL ? sub_GAME_7F04B478(obj) : NULL;

      if (obj == NULL || obj->prop != prop || node == NULL ||
          !(obj->state & PROPSTATE_DESTROYED)) {
        emu_log("DESTROYED_PROP_NOT_RESTORED");
        emu_log("TEST_FAILED");
        break;
      }

      rodata = (struct ModelRoData_DisplayList_CollisionRecord *)node->Data;
      rwdata =
          (struct ModelRwData_DisplayList_CollisionRecord *)modelGetNodeRwData(
              obj->model, node);
      vertex_hash = 2166136261u;
      for (i = 0; i < rodata->numVertices * sizeof(Vertex); i++) {
        vertex_hash = (vertex_hash ^ ((u8 *)rwdata->Vertices)[i]) * 16777619u;
      }

      if (rwdata->Vertices == rodata->Vertices ||
          vertex_hash != saved_vertex_hash) {
        emu_log("DEFORMATION_NOT_RESTORED hash=%x saved=%x", vertex_hash,
                saved_vertex_hash);
        emu_log("TEST_FAILED");
      } else {
        emu_log("TEST_COMPLETE");
      }
    }
    break;
  }

  case STATE_TINTED_GLASS_PORTAL: {
    static s32 portal = -1;
    static s32 saved_closed;
    static s16 saved_indices[MAX_TEST_TINTED_GLASS];
    static u8 saved_rooms[MAX_TEST_TINTED_GLASS][4];
    static s32 saved_count;

    if (after_frames(30)) {
      PropRecord *prop;

      portal = -1;
      saved_count = 0;
      for (prop = ptr_obj_pos_list_first_entry; prop != NULL;
           prop = prop->next) {
        if ((prop->type == PROP_TYPE_OBJ || prop->type == PROP_TYPE_WEAPON) &&
            prop->obj != NULL && prop->obj->type == PROPDEF_TINTED_GLASS) {
          TintedGlassRecord *glass = (TintedGlassRecord *)prop->obj;

          if (saved_count >= MAX_TEST_TINTED_GLASS) {
            emu_log("TOO_MANY_TINTED_GLASS_PROPS");
            emu_log("TEST_FAILED");
            break;
          }
          saved_indices[saved_count] = get_prop_index(prop);
          saved_rooms[saved_count][0] = prop->rooms[0];
          saved_rooms[saved_count][1] = prop->rooms[1];
          saved_rooms[saved_count][2] = prop->rooms[2];
          saved_rooms[saved_count][3] = prop->rooms[3];
          saved_count++;

          if (glass->portalnum >= 0) {
            portal = glass->portalnum;
          }
        }
      }

      if (portal < 0 || saved_count == 0) {
        emu_log("TINTED_GLASS_PORTAL_NOT_FOUND");
        emu_log("TEST_FAILED");
      } else {
        saved_closed = bgGetDataPortalsControlBytes1Bit1(portal) != 0;
        emu_log("TRIGGER_SAVE portal=%d closed=%d", portal, saved_closed);
        save_game_state();
        emu_log("SAVE_DONE");
      }
    } else if (after_frames(2)) {
      if (portal >= 0) {
        bgToggleDataPortalsContrlBytes1Bit1(portal, saved_closed);
        if ((bgGetDataPortalsControlBytes1Bit1(portal) != 0) == saved_closed) {
          emu_log("PORTAL_STATE_NOT_CHANGED");
          emu_log("TEST_FAILED");
        }
      }
    } else if (after_frames(2)) {
      if (portal >= 0) {
        s32 i;
        bool rooms_match = TRUE;

        emu_log("TRIGGER_LOAD");
        load_game_state();
        emu_log("LOAD_DONE");
        if ((bgGetDataPortalsControlBytes1Bit1(portal) != 0) != saved_closed) {
          emu_log("TINTED_GLASS_PORTAL_NOT_RESTORED");
          emu_log("TEST_FAILED");
        }

        for (i = 0; i < saved_count; i++) {
          PropRecord *glass = get_prop_by_index(saved_indices[i]);
          if (glass == NULL || memcmp(glass->rooms, saved_rooms[i],
                                      sizeof(saved_rooms[i])) != 0) {
            emu_log("TINTED_GLASS_ROOMS_CHANGED prop=%d", saved_indices[i]);
            rooms_match = FALSE;
          }
        }

        if (!rooms_match) {
          emu_log("TEST_FAILED");
        } else if ((bgGetDataPortalsControlBytes1Bit1(portal) != 0) ==
                   saved_closed) {
          emu_log("TEST_COMPLETE");
        }
      }
    }
    break;
  }

  case TEST_MOVE_SPEED: {
#define DISTANCE 10000.0f
    f32 x = g_CurrentPlayer->prop->pos.x;
    f32 z = g_CurrentPlayer->prop->pos.z;
    f32 distance_x = x - g_TestMoveStartX;
    f32 distance_z = z - g_TestMoveStartZ;
    f32 exceeded_distance =
        sqrtf(distance_x * distance_x + distance_z * distance_z) - DISTANCE;
    TestMoveCase test_case =
        get_test_move_case(g_TestMoveCaseIndex < 0 ? 0 : g_TestMoveCaseIndex);

    if (test_case.base_delta_frames == 0) {
      g_SimulatedButtons = 0;
      break;
    }

    // g_SimulatedButtons |= U_CBUTTONS;
    g_SimulatedButtons |= U_CBUTTONS | R_CBUTTONS;

    if (g_TestMoveCaseIndex >= 0) {
      g_ForcedDeltaFrames =
          g_save_test_timer % test_case.lag_chance_denominator <
                  test_case.lag_chance_numerator
              ? test_case.base_delta_frames + 1
              : test_case.base_delta_frames;
    }

    if (g_TestMoveCaseIndex >= 0 && exceeded_distance >= 0) {
      s32 ticks = mission_timer - g_mission_timer_start;
      emu_log("DISTANCE_REACHED x=%.1f z=%.1f in %.2fs (%d ticks + %.1f extra "
              "distance)",
              distance_x, distance_z, (f32)ticks / 60.0f, ticks,
              exceeded_distance);
    }

    if (g_TestMoveCaseIndex == -1) {
      // Wait for max move speed
      if (g_save_test_timer == 250) {
        g_TestMoveStartX = x;
        g_TestMoveStartZ = z;
        g_mission_timer_start = mission_timer;
        save_game_state();
      }
      if (g_save_test_timer < 280) {
        break;
      }
    }

    if (g_TestMoveCaseIndex == -1 || exceeded_distance >= 0) {
      TestMoveCase next_test_case = get_test_move_case(++g_TestMoveCaseIndex);
      s32 ticks = mission_timer - g_mission_timer_start;
      if (exceeded_distance >= 0 && test_case.expected_ticks != 0 &&
          test_case.expected_ticks != ticks) {
        emu_log("TEST_FAILED expected=%d ticks, actual=%d",
                test_case.expected_ticks, ticks);
      }
      if (next_test_case.base_delta_frames == 0) {
        emu_log("TEST_COMPLETE");
      } else {
        g_ForcedDeltaFrames = next_test_case.lag_chance_denominator > 0 &&
                                      next_test_case.lag_chance_numerator > 0
                                  ? next_test_case.base_delta_frames + 1
                                  : next_test_case.base_delta_frames;
        emu_log(
            "TEST_MOVE_SPEED_CASE %d (deltaFrames = %d%% @ %d or %d%% @ %d)",
            g_TestMoveCaseIndex,
            (next_test_case.lag_chance_denominator -
             next_test_case.lag_chance_numerator) *
                100 / next_test_case.lag_chance_denominator,
            next_test_case.base_delta_frames,
            next_test_case.lag_chance_numerator * 100 /
                next_test_case.lag_chance_denominator,
            next_test_case.base_delta_frames + 1);
      }

      // Teleport back to start position
      load_game_state();

      // Reset timer
      g_save_test_timer = 0;
    }
    break;
  }

  case STATE_DOOR: {
    PropRecord *door = NULL;
    PropRecord *p = NULL;
    u32 door_num = 4; // roller door in front of bond at start of runway
    for (p = ptr_obj_pos_list_first_entry; p != NULL; p = p->next) {
      if (p->type == PROP_TYPE_DOOR && door_num-- <= 0) {
        door = p;
        break;
      }
    }

    if (after_frames(30)) {
      if (door == NULL) {
        emu_log("DOOR_NOT_FOUND");
      } else {
        emu_log("DOOR_TEST_BEFORE");
        // bond_interact_object();
        propdoorInteract(door);
        propExecuteTickOperation(door, FALSE);
        emu_log("DOOR_TEST_AFTER");
      }
    } else if (after_frames(30)) {
      emu_log("TRIGGER_SAVE");
      save_game_state();
      emu_log("SAVE_DONE");
    } else if (after_frames(30)) {
      emu_log("TRIGGER_LOAD");
      load_game_state();
      emu_log("LOAD_DONE");
    } else if (after_frames(2)) {
      emu_log("TEST_COMPLETE");
    }
    break;
  }

  case STATE_GRENADE: {
    if (after_frames(20)) {
      emu_log("TRIGGER_SAVE");
      save_game_state();
      emu_log("SAVE_DONE");
    } else if (after_frames(20)) {
      emu_log("START_MOVING_LEFT");
      g_SimulatedButtons |= L_CBUTTONS;
    } else if (after_frames(20)) {
      emu_log("GRENADES_PICKED_UP");
      emu_log("STOP_MOVING_LEFT");
      g_SimulatedButtons &= ~L_CBUTTONS;
    } else if (after_frames(20)) {
      emu_log("TRIGGER_LOAD");
      load_game_state();
      emu_log("LOAD_DONE");
    } else if (after_frames(20)) {
      emu_log("START_MOVING_LEFT");
      g_SimulatedButtons |= L_CBUTTONS;
    } else if (after_frames(20)) {
      emu_log("GRENADES_PICKED_UP");
      emu_log("STOP_MOVING_LEFT");
      g_SimulatedButtons &= ~L_CBUTTONS;
    } else if (after_frames(20)) {
      emu_log("SWITCHING_WEAPONS_TO_GRENADES");
      g_SimulatedButtons |= A_BUTTON;
    } else if (after_frames(2)) {
      g_SimulatedButtons &= ~A_BUTTON;
    } else if (after_frames(30)) {
      emu_log("THROWING_GRENADE");
      g_SimulatedButtons |= Z_TRIG;
    } else if (after_frames(2)) {
      g_SimulatedButtons &= ~Z_TRIG;
    } else if (after_frames(15)) {
      emu_log("TRIGGER_SAVE");
      save_game_state();
      emu_log("SAVE_DONE");
    } else if (after_frames(300)) {
      emu_log("TRIGGER_LOAD");
      load_game_state();
      emu_log("LOAD_DONE");
    } else if (after_frames(2)) {
      emu_log("TRIGGER_SECOND_SAVE");
      save_game_state();
      emu_log("SECOND_SAVE_DONE");
    } else if (after_frames(2)) {
      emu_log("TEST_COMPLETE");
    }
    break;
  }

  case STATE_BUNKER: {
    static s32 previous_chr_count = -1;
    static bool chars_must_not_spawn = FALSE;
    s32 i;
    s32 chr_count = 0;

    for (i = 0; i < g_NumChrSlots; i++) {
      if (g_ChrSlots[i].model != NULL) {
        chr_count++;
      }
    }

    if (chr_count != previous_chr_count) {
      emu_log("CHR_COUNT_CHANGED frame=%d old=%d new=%d", g_save_test_timer,
              previous_chr_count, chr_count);
      for (i = 0; i < g_NumChrSlots; i++) {
        ChrRecord *chr = &g_ChrSlots[i];
        if (chr->model != NULL) {
          bool is_global = FALSE;
          s32 ailist_id = chr->ailist != NULL
                              ? chraiGetAIListID(chr->ailist, &is_global)
                              : -1;
          emu_log("CHR slot=%d num=%d body=%d ai=%04x off=%d flags=%08x "
                  "hidden=%04x hear=%d action=%d",
                  i, chr->chrnum, chr->bodynum, ailist_id, chr->aioffset,
                  chr->chrflags, chr->hidden, chr->lastheartarget60,
                  chr->actiontype);
        }
      }
      if (chars_must_not_spawn && chr_count > previous_chr_count) {
        emu_log("TEST_FAILED");
        chars_must_not_spawn = FALSE;
      }
      previous_chr_count = chr_count;
    }

    if (after_frames(30)) {
      emu_log("TRIGGER_SAVE");
      save_game_state();
      emu_log("SAVE_DONE");
    } else if (after_frames(30)) {
      emu_log("TRIGGER_ALARM");
      alarmActivate();
      objectiveregisters1 |= 0x100;
    } else if (after_frames(30)) {
      emu_log("PRE_LOAD alarm=%d flags=%08x", alarm_timer, objectiveregisters1);
      emu_log("TRIGGER_LOAD");
      load_game_state();
      emu_log("LOAD_DONE");
      emu_log("POST_LOAD alarm=%d flags=%08x", alarm_timer,
              objectiveregisters1);
      chars_must_not_spawn = TRUE;
    } else if (after_frames(1200) && chars_must_not_spawn) {
      emu_log("TEST_COMPLETE");
    }
    break;
  }

  case STATE_DAM: {
    if (after_frames(30)) {
      emu_log("TRIGGER_SAVE");
      save_game_state();
      emu_log("SAVE_DONE");
    } else if (after_frames(30)) {
      emu_log("TRIGGER_LOAD");
      load_game_state();
      emu_log("LOAD_DONE");
    } else if (after_frames(2)) {
      emu_log("TEST_COMPLETE");
    }
    break;
  }

  case STATE_CORRUPT_FREELIST: {
    if (after_frames(30)) {
      emu_log("TRIGGER_SAVE");
      save_game_state();
      emu_log("SAVE_DONE");
    } else if (after_frames(2)) {
      if (ptr_obj_pos_list_final_entry != NULL) {
        ptr_obj_pos_list_final_entry->prev = (PropRecord *)0x801e1830;
      }
      emu_log("CORRUPT_FREELIST");
      emu_log("TRIGGER_LOAD");
      load_game_state();
      emu_log("LOAD_DONE");
    } else if (after_frames(2)) {
      emu_log("TEST_COMPLETE");
    }
    break;
  }

  case STATE_ARCHIVES_KEY: {
    static PropRecord *key;
    s32 i;

    if (after_frames(30)) {
      key = NULL;
      for (i = 0; i < g_NumChrSlots && key == NULL; i++) {
        ChrRecord *chr = &g_ChrSlots[i];
        PropRecord *child;

        if (chr->prop == NULL) {
          continue;
        }
        for (child = chr->prop->child; child != NULL; child = child->prev) {
          if (child->obj != NULL && child->obj->type == PROPDEF_KEY) {
            key = child;
            break;
          }
        }
      }

      if (key == NULL) {
        emu_log("KEY_NOT_FOUND");
        emu_log("TEST_FAILED");
      } else {
        emu_log("TRIGGER_SAVE");
        save_game_state();
        emu_log("SAVE_DONE");
      }
    } else if (after_frames(2)) {
      if (key != NULL) {
        objDetach(key);
        chrpropActivate(key);
        chrpropEnable(key);
        chrpropRegisterRooms(key);
      }
    } else if (after_frames(2)) {
      emu_log("TRIGGER_LOAD");
      load_game_state();
      emu_log("LOAD_DONE");
    } else if (after_frames(2)) {
      if (key != NULL && key->parent != NULL &&
          key->parent->type == PROP_TYPE_CHR) {
        emu_log("KEY_RESTORED_TO_CHR");
        emu_log("TEST_COMPLETE");
      } else {
        emu_log("KEY_NOT_RESTORED");
        emu_log("TEST_FAILED");
      }
    }
    break;
  }

  case STATE_TRAIN_HATCH: {
    static s32 saved_hatch_part_count;
    static ChrRecord *test_chr;
    static WeaponObjRecord *post_save_weapon;
    static PropRecord *alias_chr_prop;
    static PropRecord *owner_chr_prop;
    s32 hatch_part_count = 0;
    s32 i;

    for (i = 0; i < POS_DATA_ENTRY_LEN; i++) {
      PropRecord *prop = &pos_data_entry[i];

      if ((prop->type == PROP_TYPE_OBJ || prop->type == PROP_TYPE_WEAPON) &&
          prop->obj != NULL && prop->obj->prop == prop &&
          prop->obj->type == PROPDEF_PROP && prop->obj->obj == PROP_HATCHBOLT) {
        hatch_part_count++;
      }
    }

    if (after_frames(30)) {
      PropRecord *prop;

      test_chr = NULL;
      post_save_weapon = NULL;
      alias_chr_prop = NULL;
      owner_chr_prop = NULL;
      for (prop = ptr_obj_pos_list_first_entry; prop != NULL;
           prop = prop->next) {
        if (prop->type == PROP_TYPE_CHR && prop->chr != NULL &&
            prop->chr->weapons_held[GUNLEFT] == NULL) {
          test_chr = prop->chr;
          break;
        }
      }

      for (prop = ptr_obj_pos_list_first_entry;
           prop != NULL && alias_chr_prop == NULL; prop = prop->next) {
        PropRecord *other;

        if (prop->type != PROP_TYPE_CHR || prop->chr == NULL ||
            prop->chr == test_chr || prop->chr->prop != prop) {
          continue;
        }
        for (other = prop->next; other != NULL; other = other->next) {
          if (other->type == PROP_TYPE_CHR && other->chr != NULL &&
              other->chr != test_chr && other->chr->prop == other &&
              other->chr->bodynum == prop->chr->bodynum &&
              other->chr->headnum == prop->chr->headnum) {
            if (prop < other) {
              alias_chr_prop = prop;
              owner_chr_prop = other;
            } else {
              alias_chr_prop = other;
              owner_chr_prop = prop;
            }
            break;
          }
        }
      }

      saved_hatch_part_count = hatch_part_count;
      emu_log("HATCH_PARTS_BEFORE=%d", saved_hatch_part_count);
      if (saved_hatch_part_count == 0 || test_chr == NULL ||
          alias_chr_prop == NULL) {
        emu_log("TEST_FAILED");
      } else {
        emu_log("TRIGGER_SAVE");
        save_game_state();
        emu_log("SAVE_DONE");
      }
    } else if (after_frames(2)) {
      PropRecord *post_save_prop = chrGiveWeapon(
          test_chr, PROP_CHRKALASH, ITEM_AK47, PROPFLAG_WEAPON_LEFTHANDED);

      if (post_save_prop == NULL) {
        emu_log("POST_SAVE_WEAPON_NOT_CREATED");
        emu_log("TEST_FAILED");
      } else {
        post_save_weapon = post_save_prop->weapon;
      }

      // Simulate a freed prop slot retaining a stale pointer to a same-model
      // character that is actually owned by a later slot.
      alias_chr_prop->chr = owner_chr_prop->chr;

      for (i = 0; i < POS_DATA_ENTRY_LEN; i++) {
        PropRecord *prop = &pos_data_entry[i];

        if ((prop->type == PROP_TYPE_OBJ || prop->type == PROP_TYPE_WEAPON) &&
            prop->obj != NULL && prop->obj->prop == prop &&
            prop->obj->type == PROPDEF_PROP &&
            prop->obj->obj == PROP_HATCHBOLT) {
          objFreePermanently(prop->obj, TRUE);
        }
      }
      emu_log("HATCH_PARTS_DESTROYED");
    } else if (after_frames(2)) {
      emu_log("TRIGGER_LOAD");
      load_game_state();
      emu_log("LOAD_DONE");
    } else if (after_frames(2)) {
      emu_log("HATCH_PARTS_AFTER=%d", hatch_part_count);
      if (hatch_part_count != saved_hatch_part_count) {
        emu_log("TEST_FAILED");
      }
      if (test_chr->weapons_held[GUNLEFT] != NULL || post_save_weapon == NULL ||
          post_save_weapon->prop != NULL) {
        emu_log("POST_SAVE_WEAPON_NOT_REMOVED");
        emu_log("TEST_FAILED");
      }
      if (alias_chr_prop->chr == NULL ||
          alias_chr_prop->chr->prop != alias_chr_prop ||
          owner_chr_prop->chr == NULL ||
          owner_chr_prop->chr->prop != owner_chr_prop ||
          alias_chr_prop->chr == owner_chr_prop->chr) {
        emu_log("CROSS_LINKED_CHR_NOT_REPAIRED");
        emu_log("TEST_FAILED");
      }
    } else if (after_frames(30)) {
      emu_log("TEST_COMPLETE");
    }
    break;
  }

  case STATE_CAVERNS_ATTACHMENTS: {
    static s16 saved_chr_prop_index;
    static s16 saved_child_head_index;
    static s16 saved_hat_index;
    static s32 saved_child_count;
    static TestChildLink saved_children[MAX_SAVED_TEST_CHILDREN];
    static ChrRecord *test_chr;
    s32 i;

    if (after_frames(60)) {
      PropRecord *prop;
      PropRecord *child;

      test_chr = NULL;
      saved_chr_prop_index = -1;
      saved_child_head_index = -1;
      saved_hat_index = -1;
      saved_child_count = 0;

      for (prop = ptr_obj_pos_list_first_entry; prop != NULL;
           prop = prop->next) {
        if (prop->type == PROP_TYPE_CHR && prop->chr != NULL &&
            prop->chr->prop == prop && prop->chr->model != NULL) {
          test_chr = prop->chr;
          break;
        }
      }

      if (test_chr == NULL) {
        emu_log("CHR_NOT_FOUND");
        emu_log("TEST_FAILED");
        break;
      }

      chrGiveWeapon(test_chr, PROP_CHRKALASH, ITEM_AK47, 0);
      chrGiveWeapon(test_chr, PROP_CHRUZI, ITEM_UZI,
                    PROPFLAG_WEAPON_LEFTHANDED);
      chrGiveWeapon(test_chr, PROP_CHRWPPK, ITEM_WPPK, PROPFLAG_CONCEAL_GUN);
      hatCreateForChr(test_chr, PROP_HATBERET, 0);
      if (test_chr->handle_positiondata_hat != NULL &&
          test_chr->handle_positiondata_hat->obj != NULL) {
        test_chr->handle_positiondata_hat->obj->flags2 = 0x5A1700DE;
      }

      if (count_chr_child_links(test_chr) < 3 ||
          test_chr->handle_positiondata_hat == NULL) {
        emu_log("NOT_ENOUGH_CHILDREN");
        emu_log("TEST_FAILED");
        break;
      }

      {
        PropRecord *right = test_chr->weapons_held[GUNRIGHT];
        PropRecord *left = test_chr->weapons_held[GUNLEFT];
        PropRecord *concealed = NULL;

        for (child = test_chr->prop->child; child != NULL;
             child = child->prev) {
          if (child != right && child != left &&
              child != test_chr->handle_positiondata_hat) {
            concealed = child;
            break;
          }
        }

        if (right != NULL && left != NULL && concealed != NULL) {
          test_chr->prop->child = right;
          right->prev = concealed;
          right->next = NULL;
          concealed->prev = left;
          concealed->next = right;
          left->prev = NULL;
          left->next = concealed;
        }
      }

      saved_chr_prop_index = get_prop_index(test_chr->prop);
      saved_child_head_index = get_prop_index(test_chr->prop->child);
      saved_hat_index = get_prop_index(test_chr->handle_positiondata_hat);
      for (child = test_chr->prop->child;
           child != NULL && saved_child_count < MAX_SAVED_TEST_CHILDREN;
           child = child->prev) {
        saved_children[saved_child_count].index = get_prop_index(child);
        saved_children[saved_child_count].prev = get_prop_index(child->prev);
        saved_children[saved_child_count].next = get_prop_index(child->next);
        saved_child_count++;
      }

      emu_log("ATTACHMENTS_BEFORE=%d", saved_child_count);
      emu_log("TRIGGER_SAVE");
      save_game_state();
      emu_log("SAVE_DONE");
    } else if (after_frames(2)) {
      PropRecord *child;
      PropRecord *hat = get_prop_by_index(saved_hat_index);

      if (test_chr != NULL && test_chr->prop != NULL) {
        chrGiveWeapon(test_chr, PROP_CHRMP5K, ITEM_MP5K, PROPFLAG_CONCEAL_GUN);
        chrDropItems(test_chr);

        if (hat != NULL && hat->obj != NULL) {
          ObjectRecord *hat_obj = hat->obj;
          objFreePermanently(hat->obj, TRUE);
          hat_obj->flags2 = 0;
        }

        child = test_chr->prop->child;
        if (child != NULL && child->prev != NULL) {
          test_chr->prop->child = child->prev;
        }
      }
    } else if (after_frames(2)) {
      emu_log("TRIGGER_LOAD");
      load_game_state();
      emu_log("LOAD_DONE");
    } else if (after_frames(2)) {
      PropRecord *chr_prop = get_prop_by_index(saved_chr_prop_index);
      bool ok = TRUE;

      if (chr_prop == NULL || chr_prop->type != PROP_TYPE_CHR ||
          chr_prop->chr == NULL || chr_prop->chr->prop != chr_prop) {
        emu_log("CHR_NOT_RESTORED");
        ok = FALSE;
      } else if (count_chr_child_links(chr_prop->chr) != saved_child_count) {
        emu_log("CHILD_CHAIN_INVALID count=%d saved=%d",
                count_chr_child_links(chr_prop->chr), saved_child_count);
        ok = FALSE;
      } else if (get_prop_index(chr_prop->child) != saved_child_head_index) {
        emu_log("CHILD_HEAD_CHANGED got=%d saved=%d",
                get_prop_index(chr_prop->child), saved_child_head_index);
        ok = FALSE;
      } else if (chr_prop->chr->handle_positiondata_hat == NULL ||
                 get_prop_index(chr_prop->chr->handle_positiondata_hat) !=
                     saved_hat_index ||
                 chr_prop->chr->handle_positiondata_hat->obj == NULL ||
                 chr_prop->chr->handle_positiondata_hat->obj->type !=
                     PROPDEF_HAT ||
                 chr_prop->chr->handle_positiondata_hat->obj->flags2 !=
                     0x5A1700DE) {
        emu_log("HAT_NOT_RESTORED");
        ok = FALSE;
      } else {
        for (i = 0; i < saved_child_count; i++) {
          PropRecord *child = get_prop_by_index(saved_children[i].index);

          if (child == NULL || child->parent != chr_prop ||
              get_prop_index(child->prev) != saved_children[i].prev ||
              get_prop_index(child->next) != saved_children[i].next) {
            emu_log("CHILD_LINK_CHANGED child=%d", saved_children[i].index);
            ok = FALSE;
            break;
          }
        }
      }

      if (!ok) {
        emu_log("TEST_FAILED");
      }
    } else if (after_frames(30)) {
      emu_log("TEST_COMPLETE");
    }
    break;
  }

  case FIRE_SLOWMO: {
    // The first-person gun must fire only when simulation time advances. A shot
    // (ammo decrement) on a frame where no time passed (prev_clock == 0) means
    // the gun fired while frozen/slowed -- the bug.
    {
      static s32 prev_ammo = -1;
      static s32 prev_clock = 1;
      s32 ammo = g_CurrentPlayer->hands[GUNRIGHT].weapon_ammo_in_magazine;
      if (prev_ammo >= 0 && ammo < prev_ammo) {
        emu_log("SHOT clock=%d ammo=%d->%d", prev_clock, prev_ammo, ammo);
        if (prev_clock == 0) {
          emu_log("BUG_FIRE_WHILE_FROZEN");
        }
      }
      prev_ammo = ammo;
      prev_clock = g_ClockTimer;
    }

    if (after_frames(60)) {
      // Hold fire at normal speed first so the gun is raised and shooting.
      emu_log("PRESS_Z");
      g_SimulatedButtons |= Z_TRIG;
    } else if (after_frames(30)) {
      emu_log("FREEZE");
      set_time_scale(0.0f);
    } else if (after_frames(120)) {
      emu_log("SLOWMO_0.1");
      set_time_scale(0.1f);
    } else if (after_frames(240)) {
      emu_log("UNFREEZE_NORMAL");
      set_time_scale(1.0f);
    } else if (after_frames(60)) {
      emu_log("RELEASE_Z");
      g_SimulatedButtons &= ~Z_TRIG;
    } else if (after_frames(20)) {
      emu_log("TEST_COMPLETE");
    }
    break;
  }

  case CRASH: {
    if (after_frames(30)) {
      volatile u32 *crash_ptr = (volatile u32 *)0;

      emu_log("TRIGGER_CRASH");
      *crash_ptr = 0x43524153;
      emu_log("Shouldn't have reached here...");
      emu_log("TEST_FAILED");
    }
    break;
  }

  default:
    break;
  }
#endif
}

#ifdef DEV
extern s32 speedgraphframes;

// Determinism check for loading a save state. Save once, then repeatedly load
// the state and log LOG_FRAMES consecutive frames of (deltaFrames, RNG seeds).
// Each iteration replays from the identical saved state, so if load + gameplay
// is deterministic every iteration must print the exact same per-frame
// sequence. Divergence in a given column pinpoints what isn't deterministic
// (e.g. deltaFrames jitter from wall-clock timing, or g_tlbRandomSeed which the
// save state does not restore).
//
// Runs from updateFrameCounters (via practice_tests_frame) once the frame delta
// and RNG seeds are finalized for the frame, before gameplay consumes any RNG.
void practice_tests_frame() {
  enum { PH_WAIT_FP, PH_SETTLE, PH_RUN, PH_DONE };
  static s32 phase = PH_WAIT_FP;
  static s32 iteration = 0;
  static s32 frame = 0;
  static s32 settle = 0;
  const s32 SETTLE_FRAMES = 30;
  const s32 LOG_FRAMES = 30;
  const s32 NUM_ITERATIONS = 10;

  if (g_practice_test_case != RNG_LOAD) {
    return;
  }

  switch (phase) {
  case PH_WAIT_FP:
    if (g_CameraMode == CAMERAMODE_FP) {
      emu_log("TEST_STARTED");
      phase = PH_SETTLE;
      settle = 0;
    }
    break;

  case PH_SETTLE:
    // Let the level settle before capturing the reference state.
    if (++settle >= SETTLE_FRAMES) {
      emu_log("TRIGGER_SAVE");
      save_game_state();
      emu_log("SAVE_DONE");
      load_game_state();
      iteration = 0;
      frame = 0;
      phase = PH_RUN;
    }
    break;

  case PH_RUN:
    emu_log("ITER %d f=%d delta=%d rng=%08x%08x chrobj=%08x%08x tlb=%08x%08x",
            iteration, frame, speedgraphframes, (u32)(g_randomSeed >> 32),
            (u32)g_randomSeed, (u32)(g_chrObjRandomSeed >> 32),
            (u32)g_chrObjRandomSeed, (u32)(g_tlbRandomSeed >> 32),
            (u32)g_tlbRandomSeed);
    frame++;
    if (frame >= LOG_FRAMES) {
      iteration++;
      if (iteration >= NUM_ITERATIONS) {
        emu_log("TEST_COMPLETE");
        phase = PH_DONE;
      } else {
        load_game_state();
        frame = 0;
      }
    }
    break;

  default:
    break;
  }
}
#else
void practice_tests_frame() {}
#endif
