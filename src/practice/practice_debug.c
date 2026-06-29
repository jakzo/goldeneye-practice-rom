#include "practice_debug.h"
#include "bondview.h"
#include "emu_log.h"
#include "practice_states.h"
#include "practice_ui.h"
#include <bondgame.h>
#include <memp.h>
#include <tlb_manage.h>
#include <ultra64.h>
#include "joy.h"
#include "front.h"
#include "objective_status.h"
#include "lvl.h"
#include "random.h"

/* g_mempPools[] is defined in memp.c; we need an extern here as it's not
 * in memp.h. */
extern MemoryPool g_mempPools[MEMPOOL_COUNT];

void practice_log_memory_usage() {
  s32 stage_start, stage_pos, stage_end, stage_size, stage_used, stage_free;
  s32 perm_start, perm_pos, perm_end, perm_size, perm_used, perm_free;
  s32 total_start, total_end, total_size;
  u8(*tlb_block)[TLB_BLOCK_SIZE];
  s32 pool_end, bss_addr, code_data_size, above_pool, free_rdram;
  s32 stage_free_kb, perm_free_kb, total_free_kb;

  /* ---- Pool boundaries ---------------------------------------------- */
  total_start = g_mempPools[MEMPOOL_TOTAL].start;
  total_end = g_mempPools[MEMPOOL_TOTAL].end;
  total_size = total_end - total_start;

  stage_start = g_mempPools[MEMPOOL_STAGE].start;
  stage_pos = g_mempPools[MEMPOOL_STAGE].pos;
  stage_end = g_mempPools[MEMPOOL_STAGE].end;

  perm_start = g_mempPools[MEMPOOL_PERMANENT].start;
  perm_pos = g_mempPools[MEMPOOL_PERMANENT].pos;
  perm_end = g_mempPools[MEMPOOL_PERMANENT].end;

  /* Handle uninitialized pools (pos == 0 means bank hasn't been set up) */
  if (stage_pos == 0) {
    stage_used = 0;
    stage_free = stage_end - stage_start;
  } else {
    stage_used = stage_pos - stage_start;
    stage_free = stage_end - stage_pos;
  }

  if (perm_pos == 0) {
    perm_used = 0;
    perm_free = perm_end - perm_start;
  } else {
    perm_used = perm_pos - perm_start;
    perm_free = perm_end - perm_pos;
  }

  stage_size = stage_end - stage_start;
  perm_size = perm_end - perm_start;

  /* ---- Full 4 MB RDRAM view ---------------------------------------- */
  bss_addr = (s32)(u32)&_bssSegmentEnd;
  tlb_block = tlbmanageGetTlbAllocatedBlock();
  pool_end = (s32)(u32)tlb_block;

  /* Everything between 0x80000000 and pool start: .text, .data, .bss */
  code_data_size = pool_end - bss_addr; /* actually the pool area */
  /* Wait -- the pool area IS bssSegmentEnd → tlbAllocatedBlock.
   * Code/data/BSS lives below bssSegmentEnd. */
  code_data_size = total_start - 0x80000000; /* rough code+data+BSS */

  /* Above the pool (TLB block + stacks + unused) */
  above_pool = 0x80400000 - pool_end;
  if (above_pool < 0)
    above_pool = 0; /* in case we somehow exceed 4MB */

  /* ---- Free RDRAM (unused pool space)  ------------------------------ */
  total_free_kb = (stage_free + perm_free) / 1024;

  /* ---- Log everything ----------------------------------------------- */
  practiceLogInfo("--- Memory ---");

  practiceLogInfo("RDRAM: 4MB total");
  practiceLogInfo("Code+data+BSS: ~%d KB", code_data_size / 1024);
  practiceLogInfo("Pool area:    %d KB  (0x%X - 0x%X)", total_size / 1024,
                  total_start, pool_end);
  practiceLogInfo("Stack/other:  ~%d KB", above_pool / 1024);

  practiceLogInfo("STAGE pool:   %d/%d KB used, %d KB free", stage_used / 1024,
                  stage_size / 1024, stage_free / 1024);
  practiceLogInfo("PERM  pool:   %d/%d KB used, %d KB free", perm_used / 1024,
                  perm_size / 1024, perm_free / 1024);

  practiceLogInfo("Pool free:    %d KB / %d KB total pool", total_free_kb,
                  total_size / 1024);
}

static s32 g_save_test_timer = -1;

void practice_debug_tick() {
#if TEST_CASE == STATE_DOOR
  if (g_save_test_timer == -1 && g_CameraMode == CAMERAMODE_FP) {
    g_save_test_timer = 0;
    emu_log_write("STATE_DOOR test started\n");
  }
  if (g_save_test_timer >= 0) {
    PropRecord *door = NULL;
    PropRecord *p = NULL;
    u32 door_num = 4; // roller door in front of bond at start
    for (p = ptr_obj_pos_list_first_entry; p != NULL; p = p->next) {
      if (p->type == PROP_TYPE_DOOR && door_num-- <= 0) {
        door = p;
        break;
      }
    }
    g_save_test_timer++;
    if (g_save_test_timer == 1) {
      emu_log_write("SAVE_TEST_TIMER_ACTIVE\n");
    }
    if (g_save_test_timer == 30) {
      if (door == NULL) {
        emu_log_write("DOOR_NOT_FOUND\n");
      } else {
        emu_log_write("DOOR_TEST_BEFORE\n");
        // bond_interact_object();
        propdoorInteract(door);
        propExecuteTickOperation(door, FALSE);
        emu_log_write("DOOR_TEST_AFTER\n");
      }
    }
    if (g_save_test_timer == 60) {
      emu_log_write("SAVE_TEST_TRIGGER_SAVE\n");
      save_game_state();
      emu_log_write("SAVE_TEST_SAVE_DONE\n");
    }
    if (g_save_test_timer == 90) {
      emu_log_write("SAVE_TEST_TRIGGER_LOAD\n");
      load_game_state();
      emu_log_write("SAVE_TEST_LOAD_DONE\n");
    }
  }
#elif TEST_CASE == STATE_PICKUP
  if (g_save_test_timer == -1 && g_CameraMode == CAMERAMODE_FP) {
    g_save_test_timer = 0;
    emu_log_write("STATE_PICKUP test started\n");
  }
  if (g_save_test_timer >= 0) {
    g_save_test_timer++;
    if (g_save_test_timer == 1) {
      emu_log_write("SAVE_TEST_TIMER_ACTIVE\n");
    }
    if (g_save_test_timer == 30) {
      emu_log_write("SAVE_TEST_TRIGGER_SAVE\n");
      save_game_state();
      emu_log_write("SAVE_TEST_SAVE_DONE\n");
    }
    if (g_save_test_timer == 60) {
      emu_log_write("SAVE_TEST_START_HOLD_C_LEFT\n");
      g_SimulatedButtons |= L_CBUTTONS;
    }
    if (g_save_test_timer == 90) {
      emu_log_write("SAVE_TEST_STOP_HOLD_C_LEFT\n");
      g_SimulatedButtons &= ~L_CBUTTONS;
    }
    if (g_save_test_timer == 120) {
      emu_log_write("SAVE_TEST_TRIGGER_LOAD\n");
      load_game_state();
      emu_log_write("SAVE_TEST_LOAD_DONE\n");
    }
  }
#endif
}

extern ALBank *g_musicSfxBufferPtr;
struct ALBankAlt_s;
struct ALSoundState_s;
extern struct ALSoundState_s *sndPlaySfx(struct ALBankAlt_s *soundBank, s16 soundIndex, struct ALSoundState_s *pendingState);

static int get_next_playable_level(int current) {
  int i = current + 1;
  while (mission_folder_setup_entries[i].folder_text_preset != 0) {
    if (mission_folder_setup_entries[i].type == MISSION_PART) {
      return i;
    }
    i++;
  }
  // Wrap around to start of array
  i = 0;
  while (i < current) {
    if (mission_folder_setup_entries[i].type == MISSION_PART) {
      return i;
    }
    i++;
  }
  return current;
}

static int get_prev_playable_level(int current) {
  int i = current - 1;
  while (i >= 0) {
    if (mission_folder_setup_entries[i].type == MISSION_PART) {
      return i;
    }
    i--;
  }
  // Wrap around to end of array
  // Find the last playable index first
  int last_playable = current;
  i = 0;
  while (mission_folder_setup_entries[i].folder_text_preset != 0) {
    if (mission_folder_setup_entries[i].type == MISSION_PART) {
      last_playable = i;
    }
    i++;
  }
  return last_playable;
}

/* ------------------------------------------------------------------ *
 * Mission report (end-of-level) preview
 *
 * Mirrors the briefing menu's L/R cycling but for the report screen
 * (constructor_menu0C_missionfailed in front.c). C-right / C-left from
 * the briefing menu open the report screen; from there they cycle the
 * reported status, then the difficulty, then the level.
 *
 * The report screen normally shows real end-of-level state, so the
 * report tick is a no-op unless report_preview_active is set, which
 * only happens when we launch the preview from the briefing menu.
 * ------------------------------------------------------------------ */
typedef enum REPORT_STATE {
  REPORT_COMPLETED,
  REPORT_FAILED,
  REPORT_ABORTED,
  REPORT_KIA,
  REPORT_STATE_COUNT
} REPORT_STATE;

static bool report_preview_active = FALSE;
static s32 report_state = REPORT_COMPLETED;

/* Force the globals that constructor_menu0C_missionfailed reads so the
 * report shows the selected status. objective_ptrs[] is nulled so that
 * get_status_of_objective() falls back to objectiveStatuses[] (the `id`
 * field is at offset 0, so a NULL pointer takes the safe path) rather
 * than walking stale per-stage objective data from the menu. */
static void report_force_status(void) {
  s32 i;
  OBJECTIVESTATUS objstatus;

  g_isBondKIA = (report_state == REPORT_KIA);
  mission_failed_or_aborted = (report_state == REPORT_ABORTED);

  objstatus = (report_state == REPORT_COMPLETED) ? OBJECTIVESTATUS_COMPLETE
                                                  : OBJECTIVESTATUS_FAILED;
  for (i = 0; i < OBJECTIVES_MAX; i++) {
    objective_ptrs[i] = NULL;
    objectiveStatuses[i] = objstatus;
  }
}

/* (Re)load the briefing data for the currently selected level/difficulty
 * so the report screen prints the right stage name and objectives. */
static void report_load_stage(void) {
  selected_stage = mission_folder_setup_entries[briefingpage].stage_id;
  lvlSetSelectedDifficulty(selected_difficulty);
  load_briefing_text_for_stage();
}

static void report_advance_difficulty_fwd(void) {
  if (selected_difficulty == DIFFICULTY_AGENT) {
    selected_difficulty = DIFFICULTY_SECRET;
  } else if (selected_difficulty == DIFFICULTY_SECRET) {
    selected_difficulty = DIFFICULTY_00;
  } else {
    selected_difficulty = DIFFICULTY_AGENT;
    briefingpage = get_next_playable_level(briefingpage);
  }
}

static void report_advance_difficulty_bwd(void) {
  if (selected_difficulty == DIFFICULTY_00) {
    selected_difficulty = DIFFICULTY_SECRET;
  } else if (selected_difficulty == DIFFICULTY_SECRET) {
    selected_difficulty = DIFFICULTY_AGENT;
  } else {
    selected_difficulty = DIFFICULTY_00;
    briefingpage = get_prev_playable_level(briefingpage);
  }
}

static void report_cycle_next(void) {
  report_state++;
  if (report_state >= REPORT_STATE_COUNT) {
    report_state = REPORT_COMPLETED;
    report_advance_difficulty_fwd();
  }
}

static void report_cycle_prev(void) {
  report_state--;
  if (report_state < 0) {
    report_state = REPORT_STATE_COUNT - 1;
    report_advance_difficulty_bwd();
  }
}

/* Launch the report preview from the briefing menu at a given status. */
static void enter_report_preview(s32 initial_state) {
  report_preview_active = TRUE;
  report_state = initial_state;
  report_load_stage();
  report_force_status();
  sndPlaySfx((struct ALBankAlt_s *)g_musicSfxBufferPtr, PAPER_TURN_SFX, NULL);
  frontChangeMenu(MENU_MISSION_FAILED, TRUE);
}

/* ------------------------------------------------------------------ *
 * Mission complete (stats) preview
 *
 * Mirrors the report preview but for the statistics screen
 * (constructor_menu0D_missioncomplete in front.c). C-up / C-down from
 * the briefing menu open the stats screen; from there they cycle the
 * difficulty then the level, generating fresh random-but-valid values
 * for each level/difficulty.
 *
 * The constructor reads its values from getters and save/array lookups
 * that aren't meaningful in the menu, so practice_stats_preview_apply()
 * overrides them in-place from the values randomised here. It is a
 * no-op (real values pass through) unless stats_preview_active is set,
 * which only happens when we launch the preview from the briefing menu.
 * ------------------------------------------------------------------ */
static bool stats_preview_active = FALSE;

/* Randomised values for the current level/difficulty screen. Times are
 * stored in the units the constructor expects: missiontimer in 1/60s
 * frames, best/target times in whole seconds. A best/target time of
 * "hidden" matches the screen's own missing-value handling (best < 0,
 * target <= 0). */
static s32 sp_missiontimer;
static s32 sp_killcount;
static s32 sp_shotsfired;
static s32 sp_headshots;
static s32 sp_bodyshots;
static s32 sp_limbshots;
static s32 sp_reg_gun;
static s32 sp_reg_hat;
static s32 sp_reg_object;
static s32 sp_besttime;
static s32 sp_targettime;
static s32 sp_favweapon;
static bool sp_favweapon_dual;

/* array_favweapon is real (save/game derived) state, so snapshot it on
 * entry and restore it when the preview ends. */
static s32 sp_saved_favweapon[2];

static s32 stats_rand_range(s32 lo, s32 hi) {
  return lo + (s32)(randomGetNext() % (u32)(hi - lo + 1));
}

static void stats_preview_randomize(void) {
  sp_headshots = stats_rand_range(0, 25);
  sp_bodyshots = stats_rand_range(3, 70);
  sp_limbshots = stats_rand_range(0, 40);
  sp_reg_gun = stats_rand_range(0, 12);
  sp_reg_hat = stats_rand_range(0, 8);
  sp_reg_object = stats_rand_range(0, 15);

  {
    s32 allhits = sp_headshots + sp_bodyshots + sp_limbshots + sp_reg_gun + sp_reg_hat;
    s32 hitshots = allhits + sp_reg_object;
    /* Keep accuracy <= 100% by only ever adding misses on top of hits. */
    sp_shotsfired = hitshots + stats_rand_range(0, 90);
    if (sp_shotsfired <= 0) {
      sp_shotsfired = 1;
    }
    sp_killcount = stats_rand_range(0, sp_headshots + sp_bodyshots + sp_limbshots);
  }

  /* Completion time: 30s .. 12min, stored as frames. */
  sp_missiontimer = stats_rand_range(30, 720) * 60;

  /* Target/best are optional - randomly present, and valid (< 0x3FF s). */
  if (randomGetNext() & 1) {
    sp_targettime = stats_rand_range(60, 900);
  } else {
    sp_targettime = 0; /* hidden */
  }
  if (randomGetNext() & 1) {
    sp_besttime = stats_rand_range(45, 900);
  } else {
    sp_besttime = -1; /* hidden */
  }

  /* Favourite weapon: a random handgun/rifle id, occasionally dual. */
  sp_favweapon = stats_rand_range(ITEM_WPPK, ITEM_RUGER);
  sp_favweapon_dual = ((randomGetNext() % 4) == 0);
}

/* Reload the stage name / briefing data for the current selection. */
static void stats_load_stage(void) {
  selected_stage = mission_folder_setup_entries[briefingpage].stage_id;
  lvlSetSelectedDifficulty(selected_difficulty);
  load_briefing_text_for_stage();
}

static void enter_stats_preview(void) {
  stats_preview_active = TRUE;
  sp_saved_favweapon[0] = array_favweapon[0][0];
  sp_saved_favweapon[1] = array_favweapon[0][1];
  stats_preview_randomize();
  stats_load_stage();
  sndPlaySfx((struct ALBankAlt_s *)g_musicSfxBufferPtr, PAPER_TURN_SFX, NULL);
  frontChangeMenu(MENU_MISSION_COMPLETE, TRUE);
}

void practice_briefing_menu_tick(void) {
  bool updated = FALSE;

  if (joyGetButtonsPressedThisFrame(PLAYER_1, R_CBUTTONS)) {
    enter_report_preview(REPORT_COMPLETED);
    return;
  } else if (joyGetButtonsPressedThisFrame(PLAYER_1, L_CBUTTONS)) {
    enter_report_preview(REPORT_KIA);
    return;
  } else if (joyGetButtonsPressedThisFrame(PLAYER_1, U_CBUTTONS | D_CBUTTONS)) {
    enter_stats_preview();
    return;
  }

  if (joyGetButtonsPressedThisFrame(PLAYER_1, R_TRIG)) {
    if (selected_difficulty == DIFFICULTY_AGENT) {
      selected_difficulty = DIFFICULTY_SECRET;
    } else if (selected_difficulty == DIFFICULTY_SECRET) {
      selected_difficulty = DIFFICULTY_00;
    } else {
      selected_difficulty = DIFFICULTY_AGENT;
      briefingpage = get_next_playable_level(briefingpage);
    }
    updated = TRUE;
  } else if (joyGetButtonsPressedThisFrame(PLAYER_1, L_TRIG)) {
    if (selected_difficulty == DIFFICULTY_00) {
      selected_difficulty = DIFFICULTY_SECRET;
    } else if (selected_difficulty == DIFFICULTY_SECRET) {
      selected_difficulty = DIFFICULTY_AGENT;
    } else {
      selected_difficulty = DIFFICULTY_00;
      briefingpage = get_prev_playable_level(briefingpage);
    }
    updated = TRUE;
  }

  if (updated) {
    selected_stage = mission_folder_setup_entries[briefingpage].stage_id;
    current_menu_briefing_page = BRIEFING_TITLE; // Go back to primary objectives page
    load_briefing_text_for_stage();
    sndPlaySfx((struct ALBankAlt_s *)g_musicSfxBufferPtr, PAPER_TURN_SFX, NULL);
  }
}

void practice_mission_report_menu_tick(void) {
  bool updated = FALSE;

  // Only drive the report screen when we launched it as a preview; leave
  // genuine end-of-level reports untouched.
  if (!report_preview_active) {
    return;
  }

  // Any button that navigates away from the report screen ends the
  // preview and restores the menu's default status globals.
  if (joyGetButtonsPressedThisFrame(PLAYER_1,
                                    START_BUTTON | Z_TRIG | A_BUTTON | B_BUTTON)) {
    report_preview_active = FALSE;
    g_isBondKIA = FALSE;
    mission_failed_or_aborted = FALSE;
    return;
  }

  if (joyGetButtonsPressedThisFrame(PLAYER_1, R_CBUTTONS)) {
    report_cycle_next();
    updated = TRUE;
  } else if (joyGetButtonsPressedThisFrame(PLAYER_1, L_CBUTTONS)) {
    report_cycle_prev();
    updated = TRUE;
  }

  if (updated) {
    report_load_stage();
    sndPlaySfx((struct ALBankAlt_s *)g_musicSfxBufferPtr, PAPER_TURN_SFX, NULL);
  }

  // Re-assert the forced status every frame so the screen always reflects
  // the previewed state.
  report_force_status();
}

void practice_mission_stats_menu_tick(void) {
  bool updated = FALSE;

  // Only drive the stats screen when we launched it as a preview; leave
  // genuine end-of-level stats untouched.
  if (!stats_preview_active) {
    return;
  }

  // Any button that navigates away from the stats screen ends the preview
  // and restores the favourite-weapon state we overwrote.
  if (joyGetButtonsPressedThisFrame(PLAYER_1,
                                    START_BUTTON | Z_TRIG | A_BUTTON | B_BUTTON)) {
    stats_preview_active = FALSE;
    array_favweapon[0][0] = sp_saved_favweapon[0];
    array_favweapon[0][1] = sp_saved_favweapon[1];
    return;
  }

  // C-up / C-down cycle the difficulty then the level, one screen each,
  // re-randomising the displayed values.
  if (joyGetButtonsPressedThisFrame(PLAYER_1, U_CBUTTONS)) {
    report_advance_difficulty_fwd();
    updated = TRUE;
  } else if (joyGetButtonsPressedThisFrame(PLAYER_1, D_CBUTTONS)) {
    report_advance_difficulty_bwd();
    updated = TRUE;
  }

  if (updated) {
    stats_preview_randomize();
    stats_load_stage();
    sndPlaySfx((struct ALBankAlt_s *)g_musicSfxBufferPtr, PAPER_TURN_SFX, NULL);
  }
}

/* Called from constructor_menu0D_missioncomplete to substitute the
 * randomised preview values for the real (menu-meaningless) ones. The
 * derived totals are recomputed here so accuracy/percentages stay
 * consistent. No-op outside a launched preview. */
void practice_stats_preview_apply(s32 *missiontimer, s32 *missiontime,
                                  s32 *killcount, s32 *shotsfired,
                                  s32 *headshots, s32 *bodyshots, s32 *limbshots,
                                  s32 *reg4, s32 *reg5, s32 *reg6, s32 *hitshots,
                                  s32 *allhits, s32 *besttime, s32 *targettime) {
  s32 hits;

  if (!stats_preview_active) {
    return;
  }

  *missiontimer = sp_missiontimer;
  *missiontime = sp_missiontimer / 60;
  *killcount = sp_killcount;
  *shotsfired = sp_shotsfired;
  *headshots = sp_headshots;
  *bodyshots = sp_bodyshots;
  *limbshots = sp_limbshots;
  *reg4 = sp_reg_gun;
  *reg5 = sp_reg_hat;
  *reg6 = sp_reg_object;

  hits = sp_headshots + sp_bodyshots + sp_limbshots + sp_reg_gun + sp_reg_hat;
  *hitshots = hits + sp_reg_object;
  if (hits <= 0) {
    hits = 1; /* mirror constructor's divide-by-zero guard */
  }
  *allhits = hits;

  *besttime = sp_besttime;
  *targettime = sp_targettime;

  array_favweapon[0][0] = sp_favweapon;
  array_favweapon[0][1] = sp_favweapon_dual ? sp_favweapon : 0;
}

