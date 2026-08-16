#ifndef PRACTICE_TESTS_H
#define PRACTICE_TESTS_H

#include <ultra64.h>

#define PRACTICE_TEST_REPLAY_RUNWAY_SAVE_STATES 29
#define PRACTICE_TEST_STATE_RUNWAY_INTRO_DEATH_LOAD 30

#ifdef PRACTICE_TEST_ROM
extern s32 g_practice_test_case;

void practice_tests_set_restart_save_state_mode(s32 enabled);
s32 practice_tests_uses_config_sram_save_state(void);
void practice_tests_set_case(s32 test_case, s32 test_param);
s32 practice_tests_boot_level(s32 test_case);
s32 practice_tests_should_disable_intro(s32 test_case);
void practice_tests_tick();
void practice_tests_frame();
void practice_tests_before_replay_frame_check(void);
void practice_tests_before_hotkeys(s32 pending_gfx_tasks);
void practice_tests_before_player_model_reconcile(void);
#else
#define g_practice_test_case 0
#define practice_tests_set_restart_save_state_mode(enabled) ((void)0)
#define practice_tests_uses_config_sram_save_state() FALSE
#define practice_tests_set_case(test_case, test_param) ((void)0)
#define practice_tests_boot_level(test_case) (-1)
#define practice_tests_should_disable_intro(test_case) 0
#define practice_tests_tick() ((void)0)
#define practice_tests_frame() ((void)0)
#define practice_tests_before_replay_frame_check() ((void)0)
#define practice_tests_before_hotkeys(pending_gfx_tasks) ((void)0)
#define practice_tests_before_player_model_reconcile() ((void)0)
#endif

#endif /* PRACTICE_TESTS_H */
