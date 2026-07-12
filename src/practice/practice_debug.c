#include "practice_debug.h"
#include "bondview.h"
#include "front.h"
#include "joy.h"
#include "practice_ui.h"
#include "game/dyn.h"
#include "game/image.h"
#include <mema.h>
#include <bondgame.h>
#include <memp.h>
#include <tlb_manage.h>
#include <ultra64.h>

/* g_mempPools[] is defined in memp.c; we need an extern here as it's not
 * in memp.h. */
extern MemoryPool g_mempPools[MEMPOOL_COUNT];

#ifdef PROFILE_PRACTICE
extern u8 *g_GfxBuffers[3];
extern u8 *g_VtxBuffers[3];
extern u8 *g_GfxMemPos;
extern u8 g_GfxActiveBufferIndex;
extern s32 g_MemaHeapSize;

typedef struct PracticeMemoryProfile {
  u32 stage_peak_used;
  u32 perm_peak_used;
  u32 mema_min_free;
  u32 mema_min_largest_free;
  u32 texture_min_free;
  u32 gfx_peak_used;
  u32 vtx_peak_used;
  s32 sampled;
} PracticeMemoryProfile;

static PracticeMemoryProfile g_PracticeMemoryProfile;

void practice_memory_profile_reset(void) {
  g_PracticeMemoryProfile.stage_peak_used = 0;
  g_PracticeMemoryProfile.perm_peak_used = 0;
  g_PracticeMemoryProfile.mema_min_free = 0xffffffff;
  g_PracticeMemoryProfile.mema_min_largest_free = 0xffffffff;
  g_PracticeMemoryProfile.texture_min_free = 0xffffffff;
  g_PracticeMemoryProfile.gfx_peak_used = 0;
  g_PracticeMemoryProfile.vtx_peak_used = 0;
  g_PracticeMemoryProfile.sampled = FALSE;
}

void practice_memory_profile_sample(Gfx *gdl) {
  struct texpool *texture_pool = (struct texpool *)&ptr_texture_alloc_start;
  u32 mema_free;
  u32 mema_largest;
  u32 used;
  s32 texture_free;

  if (g_mempPools[MEMPOOL_STAGE].pos != 0) {
    used = g_mempPools[MEMPOOL_STAGE].pos - g_mempPools[MEMPOOL_STAGE].start;
    if (used > g_PracticeMemoryProfile.stage_peak_used)
      g_PracticeMemoryProfile.stage_peak_used = used;
  }

  if (g_mempPools[MEMPOOL_PERMANENT].pos != 0) {
    used = g_mempPools[MEMPOOL_PERMANENT].pos -
           g_mempPools[MEMPOOL_PERMANENT].start;
    if (used > g_PracticeMemoryProfile.perm_peak_used)
      g_PracticeMemoryProfile.perm_peak_used = used;
  }

  if (g_MemaHeapSize > 0) {
    memaGetFreeStats(&mema_free, &mema_largest);
    if (mema_free < g_PracticeMemoryProfile.mema_min_free)
      g_PracticeMemoryProfile.mema_min_free = mema_free;
    if (mema_largest < g_PracticeMemoryProfile.mema_min_largest_free)
      g_PracticeMemoryProfile.mema_min_largest_free = mema_largest;
  }

  if (texture_pool->start != NULL) {
    texture_free = texFreeBytesInBuffer(texture_pool);
    if (texture_free >= 0 &&
        (u32)texture_free < g_PracticeMemoryProfile.texture_min_free)
      g_PracticeMemoryProfile.texture_min_free = texture_free;
  }

  if (gdl != NULL && g_GfxBuffers[g_GfxActiveBufferIndex] != NULL) {
    used = (u8 *)gdl - g_GfxBuffers[g_GfxActiveBufferIndex];
    if (used > g_PracticeMemoryProfile.gfx_peak_used)
      g_PracticeMemoryProfile.gfx_peak_used = used;
  }

  if (g_GfxMemPos != NULL && g_VtxBuffers[g_GfxActiveBufferIndex] != NULL) {
    used = g_GfxMemPos - g_VtxBuffers[g_GfxActiveBufferIndex];
    if (used > g_PracticeMemoryProfile.vtx_peak_used)
      g_PracticeMemoryProfile.vtx_peak_used = used;
  }

  g_PracticeMemoryProfile.sampled = TRUE;
}

void practice_log_memory_usage() {
  s32 stage_start, stage_pos, stage_end, stage_size, stage_used, stage_free;
  s32 perm_start, perm_pos, perm_end, perm_size, perm_used, perm_free;
  s32 total_start, total_end, total_size;
  u8(*tlb_block)[TLB_BLOCK_SIZE];
  s32 pool_end, bss_addr, code_data_size, above_pool, free_rdram;
  s32 total_free_kb;
  struct texpool *texture_pool = (struct texpool *)&ptr_texture_alloc_start;
  u32 texture_size = (u8 *)texture_pool->end - texture_pool->start;
  u32 gfx_size = g_GfxBuffers[1] - g_GfxBuffers[0];
  u32 vtx_size = g_VtxBuffers[1] - g_VtxBuffers[0];

  practice_memory_profile_sample(NULL);

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

  practiceLogInfo("--- Memory high-water ---");
  practiceLogInfo("STAGE peak: %d/%d KB, %d KB headroom",
                  g_PracticeMemoryProfile.stage_peak_used / 1024,
                  stage_size / 1024,
                  (stage_size - g_PracticeMemoryProfile.stage_peak_used) / 1024);
  practiceLogInfo("PERM peak:  %d/%d KB, %d KB headroom",
                  g_PracticeMemoryProfile.perm_peak_used / 1024,
                  perm_size / 1024,
                  (perm_size - g_PracticeMemoryProfile.perm_peak_used) / 1024);
  practiceLogInfo("MEMA:       %d KB reserved, min free %d KB, largest %d KB",
                  g_MemaHeapSize / 1024,
                  g_PracticeMemoryProfile.mema_min_free / 1024,
                  g_PracticeMemoryProfile.mema_min_largest_free / 1024);
  practiceLogInfo("TEXTURES:   peak %d/%d KB, %d KB headroom",
                  (texture_size - g_PracticeMemoryProfile.texture_min_free) / 1024,
                  texture_size / 1024,
                  g_PracticeMemoryProfile.texture_min_free / 1024);
  practiceLogInfo("GFX:        peak %d/%d KB, %d KB headroom",
                  g_PracticeMemoryProfile.gfx_peak_used / 1024,
                  gfx_size / 1024,
                  (gfx_size - g_PracticeMemoryProfile.gfx_peak_used) / 1024);
  practiceLogInfo("VTX:        peak %d/%d KB, %d KB headroom",
                  g_PracticeMemoryProfile.vtx_peak_used / 1024,
                  vtx_size / 1024,
                  (vtx_size - g_PracticeMemoryProfile.vtx_peak_used) / 1024);
}
#endif

extern ALBank *g_musicSfxBufferPtr;
struct ALBankAlt_s;
struct ALSoundState_s;
extern struct ALSoundState_s *sndPlaySfx(struct ALBankAlt_s *soundBank,
                                         s16 soundIndex,
                                         struct ALSoundState_s *pendingState);

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

void practice_briefing_menu_tick(void) {
  bool updated = FALSE;

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
    current_menu_briefing_page =
        BRIEFING_TITLE; // Go back to primary objectives page
    load_briefing_text_for_stage();
    sndPlaySfx((struct ALBankAlt_s *)g_musicSfxBufferPtr, PAPER_TURN_SFX, NULL);
  }
}
