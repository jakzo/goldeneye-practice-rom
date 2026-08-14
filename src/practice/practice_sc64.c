#include "practice_sc64.h"

#ifdef PRACTICE_TEST_ROM

#include "../init.h"
#include "../usb.h"
#include <PR/R4300.h>
#include <PR/rcp.h>
#include <ultra64.h>

#define SC64_REGS_BASE 0x1FFF0000
#define SC64_REG_SR_CMD (SC64_REGS_BASE + 0x00)
#define SC64_REG_IRQ (SC64_REGS_BASE + 0x14)
#define SC64_REG_AUX (SC64_REGS_BASE + 0x18)

#define SC64_SR_AUX_IRQ_PENDING (1 << 23)
#define SC64_IRQ_AUX_CLEAR (1 << 28)

#define SC64_AUX_PING 0xFF000000
#define SC64_AUX_HALT 0xFF000001
#define SC64_AUX_REBOOT 0xFF000002

#define CART_ROM_BASE 0x10000000
#define IPL3_WORD_START 16
#define IPL3_WORD_END 1024
#define CIC_6102_SEED 0x3F

extern u32 practice_sc64_reboot_start[];
extern u32 practice_sc64_reboot_end[];
extern void practice_sc64_reboot_entry(void);

static s32 s_sc64_halted;

s32 practice_sc64_is_present(void) { return usb_getcart() == CART_SC64; }

static void practice_sc64_ack(u32 value) { IO_WRITE(SC64_REG_AUX, value); }

static void practice_sc64_halt(void) {
  if (!s_sc64_halted) {
    /*
     * The deployer overwrites cartridge ROM after this acknowledgement.
     * Stop the threads which can schedule new graphics/audio work or load
     * overlays, while leaving this RMON thread alive to receive REBOOT.
     */
    osStopThread(&mainThread);
    osStopThread(&shedThread);
    s_sc64_halted = TRUE;
  }
  practice_sc64_ack(SC64_AUX_HALT);
}

static void practice_sc64_reboot(void) {
  volatile u32 *reboot_dst = (volatile u32 *)PHYS_TO_K1(SP_IMEM_START);
  volatile u32 *ipl3_dst = (volatile u32 *)PHYS_TO_K1(SP_DMEM_START);
  u32 *reboot_src = practice_sc64_reboot_start;
  u32 reboot_words = practice_sc64_reboot_end - practice_sc64_reboot_start;
  u32 pi_config;
  u32 tv_type;
  u32 version;
  u32 i;
  register u32 skip_rdram_reset asm("$4");
  register u32 boot_device asm("$19");
  register u32 boot_tv_type asm("$20");
  register u32 reset_type asm("$21");
  register u32 cic_seed asm("$22");
  register u32 os_version asm("$23");

  practice_sc64_ack(SC64_AUX_REBOOT);

  while (!(IO_READ(SP_STATUS_REG) & SP_STATUS_HALT)) {
  }
  IO_WRITE(SP_STATUS_REG,
           SP_CLR_SIG7 | SP_CLR_SIG6 | SP_CLR_SIG5 | SP_CLR_SIG4 | SP_CLR_SIG3 |
               SP_CLR_SIG2 | SP_CLR_SIG1 | SP_CLR_SIG0 | SP_CLR_INTR_BREAK |
               SP_CLR_SSTEP | SP_CLR_INTR | SP_CLR_BROKE | SP_SET_HALT);
  IO_WRITE(SP_SEMAPHORE_REG, 0);
  IO_WRITE(SP_PC_REG, 0);
  while (IO_READ(SP_DMA_BUSY_REG)) {
  }

  IO_WRITE(PI_STATUS_REG, PI_STATUS_CLR_INTR | PI_STATUS_RESET);
  while ((IO_READ(VI_CURRENT_REG) & ~1) != 0) {
  }
  IO_WRITE(VI_V_INTR_REG, 0x3FF);
  IO_WRITE(VI_H_START_REG, 0);
  IO_WRITE(VI_CURRENT_REG, 0);
  IO_WRITE(AI_DRAM_ADDR_REG, 0);
  IO_WRITE(AI_LEN_REG, 0);

  for (i = 0; i < reboot_words; i++) {
    reboot_dst[i] = reboot_src[i];
  }

  IO_WRITE(PI_BSD_DOM1_LAT_REG, 0xFF);
  IO_WRITE(PI_BSD_DOM1_PWD_REG, 0xFF);
  IO_WRITE(PI_BSD_DOM1_PGS_REG, 0x0F);
  IO_WRITE(PI_BSD_DOM1_RLS_REG, 0x03);
  pi_config = IO_READ(CART_ROM_BASE);
  IO_WRITE(PI_BSD_DOM1_LAT_REG, pi_config & 0xFF);
  IO_WRITE(PI_BSD_DOM1_PWD_REG, (pi_config >> 8) & 0xFF);
  IO_WRITE(PI_BSD_DOM1_PGS_REG, (pi_config >> 16) & 0x0F);
  IO_WRITE(PI_BSD_DOM1_RLS_REG, (pi_config >> 20) & 0x03);

  if (IO_READ(DPC_STATUS_REG) & DPC_STATUS_XBUS_DMEM_DMA) {
    while (IO_READ(DPC_STATUS_REG) & DPC_STATUS_PIPE_BUSY) {
    }
  }

  for (i = IPL3_WORD_START; i < IPL3_WORD_END; i++) {
    ipl3_dst[i] = IO_READ(CART_ROM_BASE + i * sizeof(u32));
  }

  tv_type = osTvType;
  version = tv_type == OS_TV_PAL ? 6 : tv_type == OS_TV_NTSC ? 1 : 4;
  skip_rdram_reset = FALSE;
  boot_device = 0;
  boot_tv_type = tv_type;
  reset_type = 0;
  cic_seed = CIC_6102_SEED;
  os_version = version;

  __asm__ volatile("li $11, %6\n"
                   "mtc0 $11, $12\n"
                   "ctc1 $0, $31\n"
                   "la $11, practice_sc64_reboot_entry\n"
                   "jr $11\n"
                   "nop\n"
                   :
                   : "r"(skip_rdram_reset), "r"(boot_device), "r"(boot_tv_type),
                     "r"(reset_type), "r"(cic_seed), "r"(os_version),
                     "i"(SR_CU1 | SR_CU0 | SR_FR)
                   : "$11");

  for (;;) {
  }
}

void practice_sc64_poll(void) {
  u32 value;

  if (!practice_sc64_is_present() ||
      !(IO_READ(SC64_REG_SR_CMD) & SC64_SR_AUX_IRQ_PENDING)) {
    return;
  }

  value = IO_READ(SC64_REG_AUX);
  IO_WRITE(SC64_REG_IRQ, SC64_IRQ_AUX_CLEAR);

  switch (value) {
  case SC64_AUX_PING:
    practice_sc64_ack(value);
    break;
  case SC64_AUX_HALT:
    practice_sc64_halt();
    break;
  case SC64_AUX_REBOOT:
    if (s_sc64_halted) {
      practice_sc64_reboot();
    }
    break;
  }
}

#endif /* PRACTICE_TEST_ROM */
