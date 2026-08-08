#ifndef PRACTICE_TESTS_H
#define PRACTICE_TESTS_H

#include <ultra64.h>

#define PRACTICE_TEST_REPLAY_RUNWAY_SAVE_STATES 29

extern s32 g_practice_test_case;

void practice_tests_set_case(s32 test_case, s32 test_param);
s32 practice_tests_boot_level(s32 test_case);
s32 practice_tests_should_disable_intro(s32 test_case);
void practice_tests_tick();
void practice_tests_frame();
void practice_tests_before_replay_frame_check(void);
void practice_tests_before_hotkeys(s32 pending_gfx_tasks);

#endif /* PRACTICE_TESTS_H */
