#include "practice_profile.h"
#include "emu_log.h"
#include "practice_replay.h"
#include "tlb_manage.h"

#ifdef PROFILE_PRACTICE

/*
 * Diagnostic-only CPU profiling for the deterministic Runway replay. Keeping
 * one start count and one elapsed count per area makes the profiler small and
 * avoids retaining a frame history in scarce N64 memory.
 */
static s32 g_ProfileEnabled;
static s32 g_ProfileFrameActive;
static s32 g_ProfileHeaderWritten;
static u32 g_ProfileFrame;
static s32 g_ProfileDeltaFrames;
static u32 g_ProfileStart[PRACTICE_PROFILE_AREA_COUNT];
static u32 g_ProfileCycles[PRACTICE_PROFILE_AREA_COUNT];
static u32 g_ProfileTlbStartLoads;
static u32 g_ProfileTlbStartTotalCycles;
static u32 g_ProfileTlbStartDmaCycles;
static u32 g_ProfileTlbStartIcacheCycles;

void practice_profile_set_enabled(s32 enabled) {
  s32 i;

  g_ProfileEnabled = enabled;
  g_ProfileFrameActive = FALSE;
  g_ProfileHeaderWritten = FALSE;
  g_ProfileFrame = 0;
  g_ProfileDeltaFrames = 0;
  for (i = 0; i < PRACTICE_PROFILE_AREA_COUNT; i++) {
    g_ProfileStart[i] = 0;
    g_ProfileCycles[i] = 0;
  }
}

void practice_profile_frame_start(s32 delta_frames) {
  s32 i;

  g_ProfileFrameActive = g_ProfileEnabled && g_ReplayIsPlaying;
  if (!g_ProfileFrameActive)
    return;

  g_ProfileDeltaFrames = delta_frames;
  for (i = 0; i < PRACTICE_PROFILE_AREA_COUNT; i++)
    g_ProfileCycles[i] = 0;
  tlbmanageGetProfileTotals(&g_ProfileTlbStartLoads,
                            &g_ProfileTlbStartTotalCycles,
                            &g_ProfileTlbStartDmaCycles,
                            &g_ProfileTlbStartIcacheCycles);
}

void practice_profile_begin(PracticeProfileArea area) {
  if (g_ProfileFrameActive)
    g_ProfileStart[area] = osGetCount();
}

void practice_profile_end(PracticeProfileArea area) {
  if (g_ProfileFrameActive)
    g_ProfileCycles[area] += osGetCount() - g_ProfileStart[area];
}

void practice_profile_frame_end(void) {
  u32 tlbLoads;
  u32 tlbTotalCycles;
  u32 tlbDmaCycles;
  u32 tlbIcacheCycles;

  if (!g_ProfileFrameActive)
    return;

  tlbmanageGetProfileTotals(&tlbLoads, &tlbTotalCycles, &tlbDmaCycles,
                            &tlbIcacheCycles);

  if (!g_ProfileHeaderWritten) {
    emu_log("PROFILE_CSV,frame,delta_frames,tick_cycles,physics_cycles,"
            "render_cycles,practice_ui_cycles,physics_setup,physics_core,"
            "physics_practice,render_setup,render_visibility,render_bg,"
            "render_actors,render_hud,render_practice,tlb_loads,tlb_cycles,"
            "tlb_dma_cycles,tlb_icache_cycles");
    g_ProfileHeaderWritten = TRUE;
  }

  emu_log("PROFILE_CSV,%u,%d,%u,%u,%u,%u,%u,%u,%u,%u,%u,%u,%u,%u,%u,"
          "%u,%u,%u,%u",
          g_ProfileFrame, g_ProfileDeltaFrames,
          g_ProfileCycles[PRACTICE_PROFILE_TICK],
          g_ProfileCycles[PRACTICE_PROFILE_PHYSICS],
          g_ProfileCycles[PRACTICE_PROFILE_RENDER],
          g_ProfileCycles[PRACTICE_PROFILE_UI],
          g_ProfileCycles[PRACTICE_PROFILE_PHYSICS_SETUP],
          g_ProfileCycles[PRACTICE_PROFILE_PHYSICS_CORE],
          g_ProfileCycles[PRACTICE_PROFILE_PHYSICS_PRACTICE],
          g_ProfileCycles[PRACTICE_PROFILE_RENDER_SETUP],
          g_ProfileCycles[PRACTICE_PROFILE_RENDER_VISIBILITY],
          g_ProfileCycles[PRACTICE_PROFILE_RENDER_BG],
          g_ProfileCycles[PRACTICE_PROFILE_RENDER_ACTORS],
          g_ProfileCycles[PRACTICE_PROFILE_RENDER_HUD],
          g_ProfileCycles[PRACTICE_PROFILE_RENDER_PRACTICE],
          tlbLoads - g_ProfileTlbStartLoads,
          tlbTotalCycles - g_ProfileTlbStartTotalCycles,
          tlbDmaCycles - g_ProfileTlbStartDmaCycles,
          tlbIcacheCycles - g_ProfileTlbStartIcacheCycles);
  g_ProfileFrame++;
  g_ProfileFrameActive = FALSE;
}

#endif
