#include "practice_debug.h"
#include "bondview.h"
#include "emu_log.h"
#include "practice_ui.h"
#include <bondgame.h>
#include <memp.h>
#include <tlb_manage.h>
#include <ultra64.h>

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

void practice_debug_got_door(PropRecord *door) {
  return;
  if (door != NULL) {
    DoorRecord *drec = door->door;
    PropDefHeaderRecord *pdhr = (PropDefHeaderRecord *)drec;
    ObjectRecord *obj = (ObjectRecord *)drec;
    s32 mi;

    emu_log("=== DOOR SAVE DATA ===");

    /* --- SavedPropRecord fields (from PropRecord) --- */
    emu_log("  [SavedPropRecord]");
    emu_log("type:        0x%02X", door->type);
    emu_log("flags:       0x%02X", door->flags);
    emu_log("timetoregen: 0x%04X", (u16)door->timetoregen);
    emu_log("pos.x:       0x%08X (f32)", *(u32 *)&door->pos.x);
    emu_log("pos.y:       0x%08X (f32)", *(u32 *)&door->pos.y);
    emu_log("pos.z:       0x%08X (f32)", *(u32 *)&door->pos.z);
    emu_log("stan:        0x%08X", (u32)door->stan);
    emu_log("zDepth:      0x%08X (f32)", *(u32 *)&door->zDepth);
    emu_log("parent:      0x%08X", (u32)door->parent);
    emu_log("child:       0x%08X", (u32)door->child);
    emu_log("prev:        0x%08X", (u32)door->prev);
    emu_log("next:        0x%08X", (u32)door->next);
    emu_log("rooms[0..3]: 0x%02X %02X %02X %02X", door->rooms[0],
            door->rooms[1], door->rooms[2], door->rooms[3]);
    emu_log("unk30:       0x%08X", door->unk30);

    /* --- SavedObjectRecord fields (inherited via PropDefHeaderRecord) --- */
    emu_log("  [SavedObjectRecord - inherited]");
    emu_log("extrascale:  0x%04X", pdhr->extrascale);
    emu_log("state:       0x%02X", pdhr->state);
    emu_log("type:        0x%02X", pdhr->type);
    emu_log("obj:         0x%04X", (u16)obj->obj);
    emu_log("pad:         0x%04X", (u16)obj->pad);
    emu_log("flags:       0x%08X", obj->flags);
    emu_log("flags2:      0x%08X", obj->flags2);

    for (mi = 0; mi < 16; mi++) {
      emu_log("mtx[%2d]:     0x%08X (f32)", mi,
              *(u32 *)&obj->mtx.m[mi / 4][mi % 4]);
    }

    emu_log("runtime_pos.x: 0x%08X (f32)", *(u32 *)&obj->runtime_pos.x);
    emu_log("runtime_pos.y: 0x%08X (f32)", *(u32 *)&obj->runtime_pos.y);
    emu_log("runtime_pos.z: 0x%08X (f32)", *(u32 *)&obj->runtime_pos.z);
    emu_log("runtime_bitflags: 0x%08X", obj->runtime_bitflags);
    emu_log("maxdamage:   0x%08X (f32)", *(u32 *)&obj->maxdamage);
    emu_log("damage:      0x%08X (f32)", *(u32 *)&obj->damage);
    emu_log("shadecol:    0x%08X", *(u32 *)&obj->shadecol);
    emu_log("nextcol:     0x%08X", *(u32 *)&obj->nextcol);

    /* --- SavedDoorRecord fields (door-specific) --- */
    emu_log("  [SavedDoorRecord - door-specific]");
    emu_log("linkedDoorOffset: 0x%08X", drec->linkedDoorOffset);
    emu_log("maxFrac:      0x%08X (f32)", *(u32 *)&drec->maxFrac);
    emu_log("perimFrac:    0x%08X (f32)", *(u32 *)&drec->perimFrac);
    emu_log("accel:        0x%08X (f32)", *(u32 *)&drec->accel);
    emu_log("decel:        0x%08X (f32)", *(u32 *)&drec->decel);
    emu_log("maxSpeed:     0x%08X (f32)", *(u32 *)&drec->maxSpeed);
    emu_log("doorFlags:    0x%04X", drec->doorFlags);
    emu_log("doorType:     0x%04X", drec->doorType);
    emu_log("keyflags:     0x%08X", drec->keyflags);
    emu_log("autoCloseFrames: 0x%08X", drec->autoCloseFrames);
    emu_log("doorOpenSound:   0x%08X", drec->doorOpenSound);
    emu_log("frac:         0x%08X (f32)", *(u32 *)&drec->frac);
    emu_log("unkac:        0x%08X (f32)", *(u32 *)&drec->unkac);
    emu_log("unkb0:        0x%08X (f32)", *(u32 *)&drec->unkb0);
    emu_log("openPosition: 0x%08X (f32)", *(u32 *)&drec->openPosition);
    emu_log("speed:        0x%08X (f32)", *(u32 *)&drec->speed);
    emu_log("openstate:    0x%02X", (u8)drec->openstate);
    emu_log("unkbd:        0x%02X", drec->unkbd);
    emu_log("calculatedopacity: 0x%04X", (u16)drec->calculatedopacity);
    emu_log("TintDist:     0x%08X", drec->TintDist);
    emu_log("CullDist:     0x%04X", (u16)drec->CullDist);
    emu_log("soundType:    0x%02X", (u8)drec->soundType);
    emu_log("fadeTime60:   0x%02X", (u8)drec->fadeTime60);
    emu_log("linkedDoor:   0x%08X", (u32)drec->linkedDoor);
    emu_log("unkcc:        0x%08X", (u32)drec->unkcc);
    emu_log("bbox.ModelNumber: 0x%08X", drec->bbox.ModelNumber);
    emu_log("bbox.Bounds.xmin: 0x%08X (f32)", *(u32 *)&drec->bbox.Bounds.xmin);
    emu_log("bbox.Bounds.xmax: 0x%08X (f32)", *(u32 *)&drec->bbox.Bounds.xmax);
    emu_log("bbox.Bounds.ymin: 0x%08X (f32)", *(u32 *)&drec->bbox.Bounds.ymin);
    emu_log("bbox.Bounds.ymax: 0x%08X (f32)", *(u32 *)&drec->bbox.Bounds.ymax);
    emu_log("bbox.Bounds.zmin: 0x%08X (f32)", *(u32 *)&drec->bbox.Bounds.zmin);
    emu_log("bbox.Bounds.zmax: 0x%08X (f32)", *(u32 *)&drec->bbox.Bounds.zmax);
    emu_log("openedTime:   0x%08X", drec->openedTime);
    emu_log("portalNumber: 0x%08X", drec->portalNumber);
    emu_log("openSoundState: 0x%08X", (u32)drec->openSoundState);
    emu_log("closeSoundState: 0x%08X", (u32)drec->closeSoundState);
    emu_log("lastcalc60i: 0x%08X", drec->lastcalc60i);
    emu_log("=== END DOOR SAVE DATA ===");
  }
}

#ifdef NON_FINAL
void practice_debug_tick() {
  extern void save_game_state(void);
  extern void load_game_state(void);
  if (g_save_test_timer == -1 && g_CameraMode == CAMERAMODE_FP) {
    g_save_test_timer = 0;
    emu_log_write("LEVEL_LOADED and SAVE_TEST_STARTED\n");
  }
  if (g_save_test_timer >= 0) {
    PropRecord *door = NULL;
    if (bossGetStageNum() == LEVELID_RUNWAY) {
      PropRecord *p = NULL;
      u32 door_num = 4; // roller door in front of bond at start
      for (p = ptr_obj_pos_list_first_entry; p != NULL; p = p->next) {
        if (p->type == PROP_TYPE_DOOR && door_num-- <= 0) {
          door = p;
          break;
        }
      }
    }
    g_save_test_timer++;
    if (g_save_test_timer == 1) {
      emu_log_write("SAVE_TEST_TIMER_ACTIVE\n");
    }
    if (bossGetStageNum() == LEVELID_RUNWAY && g_save_test_timer == 30) {
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
      // TODO: Break here and read the state of door number 4
      practice_debug_got_door(door);
      emu_log_write("SAVE_TEST_TRIGGER_SAVE\n");
      save_game_state();
      emu_log_write("SAVE_TEST_SAVE_DONE\n");
    }
    if (g_save_test_timer == 90) {
      emu_log_write("SAVE_TEST_TRIGGER_LOAD\n");
      load_game_state();
      emu_log_write("SAVE_TEST_LOAD_DONE\n");
      // TODO: Then break here and read the state of door number 4 and compare
      // it to before. It should be unchanged, otherwise it has been corrupted
      practice_debug_got_door(door);
    }
  }
}
#endif
