#ifndef PRACTICE_MEMORY_H
#define PRACTICE_MEMORY_H

/**
 * Log memory usage stats for all memory pools via practiceLogInfo.
 * Reports MEMPOOL_STAGE, MEMPOOL_PERMANENT, total pool area, and overall
 * 4MB RDRAM breakdown.
 */
void practice_log_memory_usage(void);

#endif /* PRACTICE_MEMORY_H */
