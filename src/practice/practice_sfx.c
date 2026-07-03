#include "practice_sfx.h"
#include <snd.h>
#include <ultra64.h>

extern u16 sndGetSfxSlotNaturalVolume(u8 sfxIndex);
extern void sndSetSfxSlotVolume(u8 sfxIndex, u16 volume);

static u16 g_PrePauseSfxSlotVolumes[SFX_SLOT_COUNT];
static s32 g_AreSfxPaused;

void practice_sfx_pause(void) {
  s32 i;

  if (g_AreSfxPaused) {
    return;
  }

  /*
   * Sound effects run from the audio clock rather than g_ClockTimer. Mute every
   * SFX category while practice time is stopped, but leave the sound graph
   * running so continuous effects are still alive when time resumes.
   */
  for (i = 0; i < SFX_SLOT_COUNT; i++) {
    g_PrePauseSfxSlotVolumes[i] = sndGetSfxSlotNaturalVolume(i);
  }

  for (i = 0; i < SFX_SLOT_COUNT; i++) {
    sndSetSfxSlotVolume(i, 0);
  }

  g_AreSfxPaused = TRUE;
}

void practice_sfx_resume(void) {
  s32 i;

  if (!g_AreSfxPaused) {
    return;
  }

  for (i = 0; i < SFX_SLOT_COUNT; i++) {
    sndSetSfxSlotVolume(i, g_PrePauseSfxSlotVolumes[i]);
  }

  g_AreSfxPaused = FALSE;
}
