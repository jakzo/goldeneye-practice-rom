#ifndef PRACTICE_DEBUG_H
#define PRACTICE_DEBUG_H

#include <ultra64.h>

#ifdef PROFILE_PRACTICE
void practice_log_memory_usage();
void practice_memory_profile_reset(void);
void practice_memory_profile_sample(Gfx *gdl);
#else
#define practice_log_memory_usage() ((void)0)
#define practice_memory_profile_reset() ((void)0)
#define practice_memory_profile_sample(gdl) ((void)0)
#endif

void practice_briefing_menu_tick(void);

#endif /* PRACTICE_DEBUG_H */
