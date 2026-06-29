#ifndef PRACTICE_DEBUG_H
#define PRACTICE_DEBUG_H

#include <ultra64.h>

#define STATE_DOOR 1
#define STATE_PICKUP 2

/**
 * Log memory usage stats for all memory pools via practiceLogInfo.
 * Reports MEMPOOL_STAGE, MEMPOOL_PERMANENT, total pool area, and overall
 * 4MB RDRAM breakdown.
 */
void practice_log_memory_usage();

void practice_debug_tick();
void practice_briefing_menu_tick(void);
void practice_mission_report_menu_tick(void);
void practice_mission_stats_menu_tick(void);
void practice_stats_preview_apply(s32 *missiontimer, s32 *missiontime,
                                  s32 *killcount, s32 *shotsfired,
                                  s32 *headshots, s32 *bodyshots, s32 *limbshots,
                                  s32 *reg4, s32 *reg5, s32 *reg6, s32 *hitshots,
                                  s32 *allhits, s32 *besttime, s32 *targettime);

#endif /* PRACTICE_DEBUG_H */
