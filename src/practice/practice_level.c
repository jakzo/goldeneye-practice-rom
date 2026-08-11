#include "practice_level.h"
#include <bondconstants.h>

const char *practice_level_short_name(s32 level_id) {
  switch (level_id) {
  case LEVELID_TITLE:
  case LEVELID_NONE:
    return "menu";
  case LEVELID_DAM:
    return "dam";
  case LEVELID_FACILITY:
    return "facility";
  case LEVELID_RUNWAY:
    return "runway";
  case LEVELID_SURFACE:
    return "surface1";
  case LEVELID_BUNKER1:
    return "bunker1";
  case LEVELID_SILO:
    return "silo";
  case LEVELID_FRIGATE:
    return "frigate";
  case LEVELID_SURFACE2:
    return "surface2";
  case LEVELID_BUNKER2:
    return "bunker2";
  case LEVELID_STATUE:
    return "statue";
  case LEVELID_ARCHIVES:
    return "archives";
  case LEVELID_STREETS:
    return "streets";
  case LEVELID_DEPOT:
    return "depot";
  case LEVELID_TRAIN:
    return "train";
  case LEVELID_JUNGLE:
    return "jungle";
  case LEVELID_CONTROL:
    return "control";
  case LEVELID_CAVERNS:
    return "caverns";
  case LEVELID_CRADLE:
    return "cradle";
  case LEVELID_AZTEC:
    return "aztec";
  case LEVELID_EGYPT:
    return "egypt";
  case LEVELID_TEMPLE:
    return "temple";
  case LEVELID_COMPLEX:
    return "complex";
  case LEVELID_LIBRARY:
    return "library";
  case LEVELID_BASEMENT:
    return "basement";
  case LEVELID_STACK:
    return "stack";
  case LEVELID_CAVES:
    return "caves";
  case LEVELID_CUBA:
    return "cuba";
  case LEVELID_TEST:
    return "test";
  default:
    return "unknown";
  }
}

const char *practice_difficulty_short_name(s32 difficulty) {
  switch (difficulty) {
  case DIFFICULTY_AGENT:
    return "A";
  case DIFFICULTY_SECRET:
    return "SA";
  case DIFFICULTY_00:
    return "00";
  case DIFFICULTY_007:
    return "007";
  case DIFFICULTY_MULTI:
    return "MP";
  default:
    return "UNK";
  }
}
