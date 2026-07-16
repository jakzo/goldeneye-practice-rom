#include "practice_debug.h"
#include "bondview.h"
#include "front.h"
#include "joy.h"
#include <ultra64.h>

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
