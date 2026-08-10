#include <ultra64.h>
#include <PR/os_internal.h>
#include "../../../libdragon/src/libcart/cart.h"

#define PI_STATUS_REG_ADDR 0xa4600010
#define PI_STATUS_BUSY 0x03
#define PI_STATUS_CLEAR_INTERRUPT 0x02

extern void __osPiGetAccess(void);
extern void __osPiRelAccess(void);

/* These wrappers and libcart are linked into resident 0x700... code. Taking
 * the PI lock from pageable 0x7f... code can deadlock its own TLB page load. */
int practice_cart_init(int type) {
  int result;
  cart_type = type;
  __osPiGetAccess();
  result = cart_init();
  __osPiRelAccess();
  return result;
}

int practice_cart_card_init(void) {
  int result;
  __osPiGetAccess();
  result = cart_card_init();
  __osPiRelAccess();
  return result;
}

int practice_cart_card_rd_dram(void *dram, unsigned long lba,
                               unsigned long count) {
  int result;
  __osPiGetAccess();
  result = cart_card_rd_dram(dram, lba, count);
  __osPiRelAccess();
  return result;
}

int practice_cart_card_wr_dram(const void *dram, unsigned long lba,
                               unsigned long count) {
  int result;
  __osPiGetAccess();
  result = cart_card_wr_dram(dram, lba, count);
  __osPiRelAccess();
  return result;
}

void *practice_fatfs_memset(void *destination, int value, unsigned int size) {
  u8 *bytes = destination;
  unsigned int i;
  for (i = 0; i < size; i++) {
    bytes[i] = value;
  }
  return destination;
}

void data_cache_hit_writeback_invalidate(volatile void *address,
                                         unsigned long size) {
  osWritebackDCache((void *)address, size);
  osInvalDCache((void *)address, size);
}

void data_cache_hit_writeback(volatile void *address, unsigned long size) {
  osWritebackDCache((void *)address, size);
}

static void practice_wait_for_pi_idle(void) {
  while (*(volatile u32 *)PI_STATUS_REG_ADDR & PI_STATUS_BUSY) {
  }
}

void dma_read_raw_async(void *ram_address, unsigned long pi_address,
                        unsigned long size) {
  u32 *destination = ram_address;
  volatile const u32 *source =
      (volatile const u32 *)(pi_address | 0xa0000000);
  unsigned long word_count = size / sizeof(u32);

  /* libcart only requests word-aligned, word-sized transfers. Keep these
   * transfers synchronous and interrupt-free: osPiRawStartDma would deliver
   * its completion interrupt to the game's PI manager, whose queue belongs to
   * queued ROM transfers rather than libcart's polling DMA interface. */
  practice_wait_for_pi_idle();
  while (word_count-- != 0) {
    *destination++ = *source++;
  }
}

void dma_write_raw_async(const void *ram_address, unsigned long pi_address,
                         unsigned long size) {
  u32 interrupt_mask;

  /* SC64's transfer buffer does not preserve CPU PIO stores. Use the PI DMA
   * write path, but consume its completion locally. Otherwise the pending PI
   * interrupt is delivered to the game's PI manager and is mistaken for the
   * completion of a queued ROM transfer. */
  practice_wait_for_pi_idle();
  interrupt_mask = __osDisableInt();
  if (osPiRawStartDma(OS_WRITE, pi_address, (void *)ram_address, size) == 0) {
    practice_wait_for_pi_idle();
    *(volatile u32 *)PI_STATUS_REG_ADDR = PI_STATUS_CLEAR_INTERRUPT;
  }
  __osRestoreInt(interrupt_mask);
}

void dma_wait(void) {
  practice_wait_for_pi_idle();
}

u32 io_read(u32 pi_address) {
  dma_wait();
  return *(volatile u32 *)(pi_address | 0xa0000000);
}

void io_write(u32 pi_address, u32 data) {
  dma_wait();
  *(volatile u32 *)(pi_address | 0xa0000000) = data;
}
