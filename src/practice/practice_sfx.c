#include "practice_sfx.h"
#include <bondconstants.h>
#include <music.h>
#include <snd.h>
#include <ultra64.h>

#define PRACTICE_SFX_SLOT (SFX_SLOT_COUNT - 1)

extern u16 sndGetSfxSlotNaturalVolume(u8 sfxIndex);
extern void sndSetSfxSlotVolume(u8 sfxIndex, u16 volume);

static u16 g_PrePauseSfxSlotVolumes[PRACTICE_SFX_SLOT];
static s32 g_AreSfxPaused;
static ALSound g_SaveStateSound;
static ALKeyMap g_SaveStateSoundKeyMap;

void practice_sfx_pause(void) {
  s32 i;

  if (g_AreSfxPaused) {
    return;
  }

  /*
   * Sound effects run from the audio clock rather than g_ClockTimer. Mute each
   * game SFX category while practice time is stopped, but leave the dedicated
   * practice slot and sound graph running. Continuous effects therefore remain
   * alive when time resumes, while save/load feedback can still be heard.
   */
  for (i = 0; i < PRACTICE_SFX_SLOT; i++) {
    g_PrePauseSfxSlotVolumes[i] = sndGetSfxSlotNaturalVolume(i);
  }

  for (i = 0; i < PRACTICE_SFX_SLOT; i++) {
    sndSetSfxSlotVolume(i, 0);
  }

  g_AreSfxPaused = TRUE;
}

void practice_sfx_resume(void) {
  s32 i;

  if (!g_AreSfxPaused) {
    return;
  }

  for (i = 0; i < PRACTICE_SFX_SLOT; i++) {
    sndSetSfxSlotVolume(i, g_PrePauseSfxSlotVolumes[i]);
  }

  g_AreSfxPaused = FALSE;
}

void practice_sfx_play_save_state_sound(void) {
  struct ALBankAlt_s *sound_bank = (struct ALBankAlt_s *)g_musicSfxBufferPtr;
  ALSound **sound_slot =
      &sound_bank->instArray[0]->soundArray[CAMERA_BEEP1_SFX];
  ALSound *original_sound = *sound_slot;

  if (g_SaveStateSound.keyMap == NULL) {
    g_SaveStateSound = *original_sound;
    g_SaveStateSoundKeyMap = *original_sound->keyMap;
    g_SaveStateSoundKeyMap.keyMin =
        (g_SaveStateSoundKeyMap.keyMin & 0xc0) | PRACTICE_SFX_SLOT;
    g_SaveStateSound.keyMap = &g_SaveStateSoundKeyMap;
  }

  /*
   * sndPlaySfx stores the sound pointer in its newly allocated state before it
   * returns, so the bank entry only needs to point at the clone during this
   * call. Other uses of CAMERA_BEEP1_SFX retain the original sound and slot.
   */
  *sound_slot = &g_SaveStateSound;
  sndPlaySfx(sound_bank, CAMERA_BEEP1_SFX, NULL);
  *sound_slot = original_sound;
}
