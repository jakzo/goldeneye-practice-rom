#include <ultra64.h>


OSMesg piCmdBuf[0x8];
OSMesgQueue piCmdQ;

void piCreateManager(void) {
#ifdef __GNUC__
    osCreatePiManager(0x96, &piCmdQ, piCmdBuf, 8);
#else
    osCreatePiManager(0x96, &piCmdQ, &piCmdBuf, 8);
#endif
}
