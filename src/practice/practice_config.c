#include <bondconstants.h>
#include <ultra64.h>

#include "practice/practice_config.h"

#ifndef BOOT_LEVELID
#define BOOT_LEVELID LEVELID_TITLE
#endif

struct PracticeConfig practice = {
    TRUE,         // skip_logos_on_startup
    TRUE,         // left_trigger_hotkeys
    BOOT_LEVELID, // boot_level
    TRUE,         // disable_intro_cutscenes
    5.0f,         // log_message_duration
    TRUE,         // show_hundredths_on_timer
    TRUE,         // show_mission_timer
    FALSE,        // grenade_cam
    TRUE,         // splits_enabled
};
