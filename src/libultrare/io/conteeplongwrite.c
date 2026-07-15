#include <ultra64.h>

extern u64 osClockRate;
extern u8 D_80365D20;
extern u8 _osContNumControllers;
extern OSTimer __osEepromTimer;
extern OSMesgQueue __osEepromTimerQ;
extern OSMesg __osEepromTimerMsg[4];

#ifdef __GNUC__
extern void __ull_divremi(u64 *div, u64 *rem, u64 value, u16 divisor);
#endif
// exactly the same as osEepromLongRead except for osEepromWrite call

s32 osEepromLongWrite(OSMesgQueue *mq, u8 address, u8 *buffer, int nbytes) {
    s32 result = 0;
    if (address > 0x40) {
        return -1;
    }

    while (nbytes > 0) {
        result = osEepromWrite(mq, address, buffer);
        if (result != 0) {
            return result;
        }

        nbytes -= 8;
        address += 1;
        buffer += 8;
#ifdef __GNUC__
        {
            u64 delay;
            u64 remainder;
            __ull_divremi(&delay, &remainder, 3 * osClockRate, 250);
            osSetTimer(&__osEepromTimer, delay, 0, &__osEepromTimerQ, __osEepromTimerMsg);
        }
#else
        osSetTimer(&__osEepromTimer, 12000 * osClockRate / 1000000, 0, &__osEepromTimerQ, __osEepromTimerMsg);
#endif
        osRecvMesg(&__osEepromTimerQ, NULL, OS_MESG_BLOCK);
    }

    return result;
}
