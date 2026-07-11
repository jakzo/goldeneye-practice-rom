#ifndef PRACTICE_TESTS_H
#define PRACTICE_TESTS_H

#include <ultra64.h>

void practice_tests_set_case(s32 test_case);
s32 practice_tests_boot_level(s32 test_case);
s32 practice_tests_should_disable_intro(s32 test_case);
void practice_tests_tick();
void practice_tests_frame();

#endif /* PRACTICE_TESTS_H */
