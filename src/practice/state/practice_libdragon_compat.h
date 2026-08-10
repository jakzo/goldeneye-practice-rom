#ifndef PRACTICE_LIBDRAGON_COMPAT_H
#define PRACTICE_LIBDRAGON_COMPAT_H

#include <string.h>

void *practice_fatfs_memset(void *destination, int value, unsigned int size);

/* GoldenEye exports memset as an alias of its two-argument bzero routine.
 * Include the system declarations first, then redirect libdragon calls so the
 * declaration cannot retain memset's assembler symbol. */
#define memset practice_fatfs_memset

int practice_cart_init(int type);
int practice_cart_card_init(void);
int practice_cart_card_rd_dram(void *dram, unsigned long lba,
                               unsigned long count);
int practice_cart_card_wr_dram(const void *dram, unsigned long lba,
                               unsigned long count);
#endif /* PRACTICE_LIBDRAGON_COMPAT_H */
