#ifndef _TLB_MANAGE_H_
#define _TLB_MANAGE_H_
#include <ultra64.h>

/**
 * Block size, in bytes.
 */
#define TLB_BLOCK_SIZE 0x2000

void tlbmanageEstablishManagementTable(void);
void tlbmanageResetCurrentEntriesCount(void);
void tlbmanageTranslateLoadRomFromTlbAddress(u32 address);
u8 (*tlbmanageGetTlbAllocatedBlock(void))[TLB_BLOCK_SIZE];
#ifdef PROFILE_REPLAY
void tlbmanageGetProfileTotals(u32 *loads, u32 *total_cycles,
                               u32 *dma_cycles, u32 *icache_cycles);
#endif

#endif
