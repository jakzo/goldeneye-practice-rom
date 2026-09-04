#include "sc64.h"

#ifdef ENABLE_USB

#include "init.h"
#include "usb.h"
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

extern u32 sc64_reboot_start[];
extern u32 sc64_reboot_end[];
extern void sc64_reboot_jump(u32 skip_rdram_reset, u32 boot_device,
                             u32 boot_tv_type, u32 reset_type, u32 cic_seed,
                             u32 os_version);

static s32 g_Sc64Halted;

static s32 sc64_is_present(void)
{
    return usb_getcart() == CART_SC64;
}

static void sc64_ack(u32 value)
{
    IO_WRITE(SC64_REG_AUX, value);
}

static void sc64_halt(void)
{
    if (!g_Sc64Halted) {
        /* Leave the RMON thread alive so it can receive the reboot request. */
        osStopThread(&mainThread);
        osStopThread(&shedThread);
        g_Sc64Halted = TRUE;
    }

    sc64_ack(SC64_AUX_HALT);
}

static void sc64_reboot(void)
{
    volatile u32 *reboot_dst = (volatile u32 *)PHYS_TO_K1(SP_IMEM_START);
    volatile u32 *ipl3_dst = (volatile u32 *)PHYS_TO_K1(SP_DMEM_START);
    u32 *reboot_src = sc64_reboot_start;
    u32 reboot_words = sc64_reboot_end - sc64_reboot_start;
    u32 pi_config;
    u32 tv_type;
    u32 version;
    u32 i;

    sc64_ack(SC64_AUX_REBOOT);

    while (!(IO_READ(SP_STATUS_REG) & SP_STATUS_HALT)) {
    }
    IO_WRITE(SP_STATUS_REG,
             SP_CLR_SIG7 | SP_CLR_SIG6 | SP_CLR_SIG5 | SP_CLR_SIG4 |
                 SP_CLR_SIG3 | SP_CLR_SIG2 | SP_CLR_SIG1 | SP_CLR_SIG0 |
                 SP_CLR_INTR_BREAK | SP_CLR_SSTEP | SP_CLR_INTR |
                 SP_CLR_BROKE | SP_SET_HALT);
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
    sc64_reboot_jump(FALSE, 0, tv_type, 0, CIC_6102_SEED, version);

    for (;;) {
    }
}

void sc64_poll(void)
{
    u32 value;

    if (!sc64_is_present() ||
        !(IO_READ(SC64_REG_SR_CMD) & SC64_SR_AUX_IRQ_PENDING)) {
        return;
    }

    value = IO_READ(SC64_REG_AUX);
    IO_WRITE(SC64_REG_IRQ, SC64_IRQ_AUX_CLEAR);

    switch (value) {
    case SC64_AUX_PING:
        sc64_ack(value);
        break;
    case SC64_AUX_HALT:
        sc64_halt();
        break;
    case SC64_AUX_REBOOT:
        if (g_Sc64Halted) {
            sc64_reboot();
        }
        break;
    }
}

#endif
