#ifndef PRACTICE_DEBUG_H
#define PRACTICE_DEBUG_H

/**
 * Log memory usage stats for all memory pools via practiceLogInfo.
 * Reports MEMPOOL_STAGE, MEMPOOL_PERMANENT, total pool area, and overall
 * 4MB RDRAM breakdown.
 */
void practice_log_memory_usage();

#ifdef NON_FINAL
void practice_debug_tick();
#else
#define practice_debug_tick() ((void)0)
#endif

#endif /* PRACTICE_DEBUG_H */
