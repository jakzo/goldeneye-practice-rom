#include <ultra64.h>

#define PI_STATUS_REG_ADDR 0xa4600010
#define PI_STATUS_BUSY 0x03

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

void dma_read_raw_async(void *ram_address, unsigned long pi_address,
                        unsigned long size) {
  osPiRawStartDma(OS_READ, pi_address, ram_address, size);
}

void dma_write_raw_async(const void *ram_address, unsigned long pi_address,
                         unsigned long size) {
  osPiRawStartDma(OS_WRITE, pi_address, (void *)ram_address, size);
}

void dma_wait(void) {
  while (*(volatile u32 *)PI_STATUS_REG_ADDR & PI_STATUS_BUSY) {
  }
}

u32 io_read(u32 pi_address) {
  dma_wait();
  return *(volatile u32 *)(pi_address | 0xa0000000);
}

void io_write(u32 pi_address, u32 data) {
  dma_wait();
  *(volatile u32 *)(pi_address | 0xa0000000) = data;
}
