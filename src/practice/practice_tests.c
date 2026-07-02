#include "practice_tests.h"
#include "bondview.h"
#include "chr.h"
#include "chrai.h"
#include "chrobjhandler.h"
#include "emu_log.h"
#include "joy.h"
#include "player.h"
#include "practice_timescale.h"
#include "state/practice_states.h"
#include <bondgame.h>
#include <ultra64.h>

extern s32 g_ClockTimer;
extern s32 chraiGetAIListID(AIRecord *AIList, bool *isGlobalAIList);
extern u64 g_randomSeed;
extern u64 g_chrObjRandomSeed;
extern u64 g_tlbRandomSeed;
extern s32 propdoorInteract(PropRecord *doorprop);
extern void propExecuteTickOperation(PropRecord *prop, INV_ITEM_TYPE op);

// --- start test cases ---
#define STATE_DOOR 1
#define STATE_GRENADE 2
#define STATE_BUNKER 3
#define STATE_DAM 4
#define FIRE_SLOWMO 5
#define RNG_LOAD 6
// --- end test cases ---

static s32 g_save_test_timer = -1;
static u32 case_delta = 0;
static s32 g_practice_test_case;

void practice_tests_set_case(s32 test_case) {
  g_practice_test_case = test_case;
  g_save_test_timer = -1;
  case_delta = 0;
}

s32 practice_tests_boot_level(s32 test_case) {
  switch (test_case) {
  case STATE_DOOR:
  case STATE_GRENADE:
  case FIRE_SLOWMO:
  case RNG_LOAD:
    return LEVELID_RUNWAY;
  case STATE_BUNKER:
    return LEVELID_BUNKER1;
  case STATE_DAM:
    return LEVELID_DAM;
  default:
    return LEVELID_NONE;
  }
}

static bool after_frames(u32 num_frames) {
  case_delta += num_frames;
  return g_save_test_timer == case_delta;
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
  } break;

  case STATE_GRENADE:
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

  case STATE_DAM:
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

  case FIRE_SLOWMO:
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

  default:
    break;
  }
#endif
}

#ifdef DEV
extern s32 speedgraphframes; // finalized per-frame delta

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
void practice_tests_frame(void) {
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
