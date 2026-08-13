#include "practice_bunker1.h"
#include "boss.h"
#include "practice_config.h"
#include <bondconstants.h>

#define BUNKER1_COMPUTER_ROOM_KEYCARD_GUARD_ID 0
#define BUNKER1_COMPUTER_ROOM_KEYCARD_GUARD_GRENADE_PROBABILITY 12

u8 practice_bunker1_guard_grenade_probability(s32 stage_id, s32 chrnum,
                                              u8 probability) {
  if (practice.bunker1_keycard_guard_grenade && stage_id == LEVELID_BUNKER1 &&
      chrnum == BUNKER1_COMPUTER_ROOM_KEYCARD_GUARD_ID) {
    return 0xff;
  }

  return probability;
}

void practice_bunker1_guard_pulled_grenade(ChrRecord *guard) {
  if (practice.bunker1_keycard_guard_grenade &&
      bossGetStageNum() == LEVELID_BUNKER1 && guard != NULL &&
      guard->chrnum == BUNKER1_COMPUTER_ROOM_KEYCARD_GUARD_ID) {
    guard->grenadeprob =
        BUNKER1_COMPUTER_ROOM_KEYCARD_GUARD_GRENADE_PROBABILITY;
  }
}
