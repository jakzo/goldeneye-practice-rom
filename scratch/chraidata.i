# 0 "<stdin>"
# 0 "<built-in>"
# 0 "<command-line>"
# 1 "<stdin>"
# 14 "<stdin>"
# 1 "include/ultra64.h" 1



# 1 "include/math.h" 1
# 93 "include/math.h"
float sinf(float);
double sin(double);
float cosf(float);
double cos(double);

float sqrtf(float);
# 5 "include/ultra64.h" 2





# 1 "include/PR/ultratypes.h" 1
# 31 "include/PR/ultratypes.h"
typedef unsigned char u8;
typedef unsigned short u16;
typedef unsigned int u32;
typedef unsigned long long u64;

typedef signed char s8;
typedef short s16;
typedef int s32;
typedef long long s64;

typedef volatile unsigned char vu8;
typedef volatile unsigned short vu16;
typedef volatile unsigned int vu32;
typedef volatile unsigned long long vu64;

typedef volatile signed char vs8;
typedef volatile short vs16;
typedef volatile int vs32;
typedef volatile long long vs64;

typedef float f32;
typedef double f64;
# 79 "include/PR/ultratypes.h"
# 1 "include/stddef.h" 1





typedef unsigned long size_t;
# 80 "include/PR/ultratypes.h" 2
# 1 "/opt/n64/lib/gcc/mips64-elf/14.2.0/include/stdint.h" 1 3 4
# 9 "/opt/n64/lib/gcc/mips64-elf/14.2.0/include/stdint.h" 3 4
# 1 "/opt/n64/mips64-elf/include/stdint.h" 1 3 4
# 12 "/opt/n64/mips64-elf/include/stdint.h" 3 4
# 1 "/opt/n64/mips64-elf/include/machine/_default_types.h" 1 3 4







# 1 "/opt/n64/mips64-elf/include/sys/features.h" 1 3 4
# 28 "/opt/n64/mips64-elf/include/sys/features.h" 3 4
# 1 "/opt/n64/mips64-elf/include/_newlib_version.h" 1 3 4
# 29 "/opt/n64/mips64-elf/include/sys/features.h" 2 3 4
# 9 "/opt/n64/mips64-elf/include/machine/_default_types.h" 2 3 4
# 41 "/opt/n64/mips64-elf/include/machine/_default_types.h" 3 4

# 41 "/opt/n64/mips64-elf/include/machine/_default_types.h" 3 4
typedef signed char __int8_t;

typedef unsigned char __uint8_t;
# 55 "/opt/n64/mips64-elf/include/machine/_default_types.h" 3 4
typedef short int __int16_t;

typedef short unsigned int __uint16_t;
# 77 "/opt/n64/mips64-elf/include/machine/_default_types.h" 3 4
typedef long int __int32_t;

typedef long unsigned int __uint32_t;
# 103 "/opt/n64/mips64-elf/include/machine/_default_types.h" 3 4
typedef long long int __int64_t;

typedef long long unsigned int __uint64_t;
# 134 "/opt/n64/mips64-elf/include/machine/_default_types.h" 3 4
typedef signed char __int_least8_t;

typedef unsigned char __uint_least8_t;
# 160 "/opt/n64/mips64-elf/include/machine/_default_types.h" 3 4
typedef short int __int_least16_t;

typedef short unsigned int __uint_least16_t;
# 182 "/opt/n64/mips64-elf/include/machine/_default_types.h" 3 4
typedef long int __int_least32_t;

typedef long unsigned int __uint_least32_t;
# 200 "/opt/n64/mips64-elf/include/machine/_default_types.h" 3 4
typedef long long int __int_least64_t;

typedef long long unsigned int __uint_least64_t;
# 214 "/opt/n64/mips64-elf/include/machine/_default_types.h" 3 4
typedef long long int __intmax_t;







typedef long long unsigned int __uintmax_t;







typedef int __intptr_t;

typedef unsigned int __uintptr_t;
# 13 "/opt/n64/mips64-elf/include/stdint.h" 2 3 4
# 1 "/opt/n64/mips64-elf/include/sys/_intsup.h" 1 3 4
# 35 "/opt/n64/mips64-elf/include/sys/_intsup.h" 3 4
       
       
       
       
       
       
       
       
# 190 "/opt/n64/mips64-elf/include/sys/_intsup.h" 3 4
       
       
       
       
       
       
       
       
# 14 "/opt/n64/mips64-elf/include/stdint.h" 2 3 4
# 1 "/opt/n64/mips64-elf/include/sys/_stdint.h" 1 3 4
# 20 "/opt/n64/mips64-elf/include/sys/_stdint.h" 3 4
typedef __int8_t int8_t ;



typedef __uint8_t uint8_t ;







typedef __int16_t int16_t ;



typedef __uint16_t uint16_t ;







typedef __int32_t int32_t ;



typedef __uint32_t uint32_t ;







typedef __int64_t int64_t ;



typedef __uint64_t uint64_t ;






typedef __intmax_t intmax_t;




typedef __uintmax_t uintmax_t;




typedef __intptr_t intptr_t;




typedef __uintptr_t uintptr_t;
# 15 "/opt/n64/mips64-elf/include/stdint.h" 2 3 4






typedef __int_least8_t int_least8_t;
typedef __uint_least8_t uint_least8_t;




typedef __int_least16_t int_least16_t;
typedef __uint_least16_t uint_least16_t;




typedef __int_least32_t int_least32_t;
typedef __uint_least32_t uint_least32_t;




typedef __int_least64_t int_least64_t;
typedef __uint_least64_t uint_least64_t;
# 51 "/opt/n64/mips64-elf/include/stdint.h" 3 4
  typedef int int_fast8_t;
  typedef unsigned int uint_fast8_t;
# 61 "/opt/n64/mips64-elf/include/stdint.h" 3 4
  typedef int int_fast16_t;
  typedef unsigned int uint_fast16_t;
# 71 "/opt/n64/mips64-elf/include/stdint.h" 3 4
  typedef int int_fast32_t;
  typedef unsigned int uint_fast32_t;
# 81 "/opt/n64/mips64-elf/include/stdint.h" 3 4
  typedef long long int int_fast64_t;
  typedef long long unsigned int uint_fast64_t;
# 10 "/opt/n64/lib/gcc/mips64-elf/14.2.0/include/stdint.h" 2 3 4
# 81 "include/PR/ultratypes.h" 2
    
# 81 "include/PR/ultratypes.h"
   typedef ptrdiff_t ssize_t;
# 11 "include/ultra64.h" 2
# 1 "include/PR/os.h" 1
# 47 "include/PR/os.h"
typedef s32 OSPri;
typedef s32 OSId;
typedef union { struct { f32 f_odd; f32 f_even; } f; f64 d; } __OSfp;

typedef struct {
 u64 at, v0, v1, a0, a1, a2, a3;
 u64 t0, t1, t2, t3, t4, t5, t6, t7;
 u64 s0, s1, s2, s3, s4, s5, s6, s7;
 u64 t8, t9, gp, sp, s8, ra;
 u64 lo, hi;
 u32 sr, pc, cause, badvaddr, rcp;
 u32 fpcsr;
 __OSfp fp0, fp2, fp4, fp6, fp8, fp10, fp12, fp14;
 __OSfp fp16, fp18, fp20, fp22, fp24, fp26, fp28, fp30;
} __OSThreadContext;

typedef struct OSThread_s {
 struct OSThread_s *next;
 OSPri priority;
 struct OSThread_s **queue;
 struct OSThread_s *tlnext;
 u16 state;
 u16 flags;
 OSId id;
 int fp;
 __OSThreadContext context;
} OSThread;

typedef u32 OSEvent;
typedef u32 OSIntMask;
typedef u32 OSPageMask;
typedef u32 OSHWIntr;




typedef void * OSMesg;




typedef struct OSMesgQueue_s {
 OSThread *mtqueue;

 OSThread *fullqueue;

 s32 validCount;
 s32 first;
 s32 msgCount;
 OSMesg *msg;
} OSMesgQueue;
# 108 "include/PR/os.h"
typedef struct {
 u32 errStatus;
        void *dramAddr;
 void *C2Addr;
 u32 sectorSize;
 u32 C1ErrNum;
 u32 C1ErrSector[4];
} __OSBlockInfo;

typedef struct {
 u32 cmdType;
 u16 transferMode;
 u16 blockNum;
 s32 sectorNum;
 u32 devAddr;
 u32 bmCtlShadow;
 u32 seqCtlShadow;
 __OSBlockInfo block[2];
} __OSTranxInfo;


typedef struct OSPiHandle_s {
        struct OSPiHandle_s *next;
        u8 type;
        u8 latency;
        u8 pageSize;
        u8 relDuration;
        u8 pulse;
     u8 domain;
        u32 baseAddress;
        u32 speed;

        __OSTranxInfo transferInfo;
} OSPiHandle;

typedef struct {
        u8 type;
        u32 address;
} OSPiInfo;




typedef struct {
        u16 type;
        u8 pri;
        u8 status;
 OSMesgQueue *retQueue;

} OSIoMesgHdr;

typedef struct {
 OSIoMesgHdr hdr;
 void * dramAddr;
 u32 devAddr;
 u32 size;
 OSPiHandle *piHandle;
} OSIoMesg;




typedef struct {
        s32 active;
 OSThread *thread;
        OSMesgQueue *cmdQueue;
        OSMesgQueue *evtQueue;
        OSMesgQueue *acsQueue;

        s32 (*dma)(s32, u32, void *, u32);
        s32 (*edma)(OSPiHandle *, s32, u32, void *, u32);
} OSDevMgr;





typedef struct {
    u32 ctrl;
    u32 width;
    u32 burst;
    u32 vSync;
    u32 hSync;
    u32 leap;
    u32 hStart;
    u32 xScale;
    u32 vCurrent;
} OSViCommonRegs;





typedef struct {
    u32 origin;
    u32 yScale;
    u32 vStart;
    u32 vBurst;
    u32 vIntr;
} OSViFieldRegs;





typedef struct {
    u8 type;
    OSViCommonRegs comRegs;
    OSViFieldRegs fldRegs[2];
} OSViMode;




typedef u64 OSTime;




typedef struct OSTimer_s {
 struct OSTimer_s *next;
 struct OSTimer_s *prev;
 OSTime interval;
 OSTime value;

 OSMesgQueue *mq;
 OSMesg msg;
} OSTimer;





typedef struct {
 u16 type;
 u8 status;
 u8 errno;
}OSContStatus;

typedef struct {
 u16 button;
 s8 stick_x;
 s8 stick_y;
 u8 errno;
} OSContPad;

typedef struct {
 void *address;
 u8 databuffer[32];
        u8 addressCrc;
 u8 dataCrc;
 u8 errno;
} OSContRamIo;







typedef struct {
 int status;
 OSMesgQueue *queue;
 int channel;
 u8 id[32];
 u8 label[32];
 int version;
 int dir_size;
 int inode_table;
 int minode_table;
 int dir_table;
 int inode_start_page;
 u8 banks;
 u8 activebank;
} OSPfs;


typedef struct {
 u32 file_size;
   u32 game_code;
   u16 company_code;
   char ext_name[4];
   char game_name[16];
} OSPfsState;




typedef struct {
 u16 *histo_base;
 u32 histo_size;
 u32 *text_start;
 u32 *text_end;
} OSProf;
# 702 "include/PR/os.h"
extern u64 osClockRate;
# 725 "include/PR/os.h"
extern OSViMode osViModeTable[];

extern OSViMode osViModeNtscLpn1;
extern OSViMode osViModeNtscLpf1;
extern OSViMode osViModeNtscLan1;
extern OSViMode osViModeNtscLaf1;
extern OSViMode osViModeNtscLpn2;
extern OSViMode osViModeNtscLpf2;
extern OSViMode osViModeNtscLan2;
extern OSViMode osViModeNtscLaf2;
extern OSViMode osViModeNtscHpn1;
extern OSViMode osViModeNtscHpf1;
extern OSViMode osViModeNtscHan1;
extern OSViMode osViModeNtscHaf1;
extern OSViMode osViModeNtscHpn2;
extern OSViMode osViModeNtscHpf2;

extern OSViMode osViModePalLpn1;
extern OSViMode osViModePalLpf1;
extern OSViMode osViModePalLan1;
extern OSViMode osViModePalLaf1;
extern OSViMode osViModePalLpn2;
extern OSViMode osViModePalLpf2;
extern OSViMode osViModePalLan2;
extern OSViMode osViModePalLaf2;
extern OSViMode osViModePalHpn1;
extern OSViMode osViModePalHpf1;
extern OSViMode osViModePalHan1;
extern OSViMode osViModePalHaf1;
extern OSViMode osViModePalHpn2;
extern OSViMode osViModePalHpf2;

extern OSViMode osViModeMpalLpn1;
extern OSViMode osViModeMpalLpf1;
extern OSViMode osViModeMpalLan1;
extern OSViMode osViModeMpalLaf1;
extern OSViMode osViModeMpalLpn2;
extern OSViMode osViModeMpalLpf2;
extern OSViMode osViModeMpalLan2;
extern OSViMode osViModeMpalLaf2;
extern OSViMode osViModeMpalHpn1;
extern OSViMode osViModeMpalHpf1;
extern OSViMode osViModeMpalHan1;
extern OSViMode osViModeMpalHaf1;
extern OSViMode osViModeMpalHpn2;
extern OSViMode osViModeMpalHpf2;

extern s32 osRomType;
extern u32 osRomBase;
extern u32 osTvType;
extern u32 osResetType;
extern s32 osCicId;
extern s32 osVersion;
extern u32 osMemSize;
extern u8 osAppNMIBuffer[];

extern OSIntMask __OSGlobalIntMask;
extern OSPiHandle *__osPiTable;
extern OSPiHandle *__osDiskHandle;
# 795 "include/PR/os.h"
extern void osCreateThread(OSThread *, OSId, void (*)(void *),
           void *, void *, OSPri);
extern void osDestroyThread(OSThread *);
extern void osYieldThread(void);
extern void osStartThread(OSThread *);
extern void osStopThread(OSThread *);
extern OSId osGetThreadId(OSThread *);
extern void osSetThreadPri(OSThread *, OSPri);
extern OSPri osGetThreadPri(OSThread *);



extern void osCreateMesgQueue(OSMesgQueue *, OSMesg *, s32);
extern s32 osSendMesg(OSMesgQueue *, OSMesg, s32);
extern s32 osJamMesg(OSMesgQueue *, OSMesg, s32);
extern s32 osRecvMesg(OSMesgQueue *, OSMesg *, s32);



extern void osSetEventMesg(OSEvent, OSMesgQueue *, OSMesg);



extern OSIntMask osGetIntMask(void);
extern OSIntMask osSetIntMask(OSIntMask);



extern void osInitRdb(u8 *sendBuf, u32 sendSize);



extern void osInvalDCache(void *, s32);
extern void osInvalICache(void *, s32);
extern void osWritebackDCache(void *, s32);
extern void osWritebackDCacheAll(void);






extern void osMapTLB(s32, OSPageMask, void *, u32, u32, s32);
extern void osMapTLBRdb(void);
extern void osUnmapTLB(s32);
extern void osUnmapTLBAll(void);
extern void osSetTLBASID(s32);



extern u32 osVirtualToPhysical(void *);
extern void * osPhysicalToVirtual(u32);
# 857 "include/PR/os.h"
extern u32 osAiGetStatus(void);
extern u32 osAiGetLength(void);
extern s32 osAiSetFrequency(u32);
extern s32 osAiSetNextBuffer(void *, u32);


extern u32 osDpGetStatus(void);
extern void osDpSetStatus(u32);
extern void osDpGetCounters(u32 *);
extern s32 osDpSetNextBuffer(void *, u64);


extern u32 osPiGetStatus(void);
extern s32 osPiGetDeviceType(void);
extern s32 osPiRawWriteIo(u32, u32);
extern s32 osPiRawReadIo(u32, u32 *);
extern s32 osPiRawStartDma(s32, u32, void *, u32);
extern s32 osPiWriteIo(u32, u32);
extern s32 osPiReadIo(u32, u32 *);
extern s32 osPiStartDma(OSIoMesg *, s32, s32, u32, void *, u32,
         OSMesgQueue *);
extern void osCreatePiManager(OSPri, OSMesgQueue *, OSMesg *, s32);


extern u32 osViGetStatus(void);
extern u32 osViGetCurrentMode(void);
extern u32 osViGetCurrentLine(void);
extern u32 osViGetCurrentField(void);
extern void *osViGetCurrentFramebuffer(void);
extern void *osViGetNextFramebuffer(void);
extern void osViSetXScale(f32);
extern void osViSetYScale(f32);
extern void osViSetSpecialFeatures(u32);
extern void osViSetMode(OSViMode *);
extern void osViSetEvent(OSMesgQueue *, OSMesg, u32);
extern void osViSwapBuffer(void *);
extern void osViBlack(u8);
extern void osViFade(u8, u16);
extern void osViRepeatLine(u8);
extern void osCreateViManager(OSPri);



extern OSTime osGetTime(void);
extern void osSetTime(OSTime);
extern int osSetTimer(OSTimer *, OSTime, OSTime,
       OSMesgQueue *, OSMesg);
extern int osStopTimer(OSTimer *);



extern s32 osContInit(OSMesgQueue *, u8 *, OSContStatus *);
extern s32 osContReset(OSMesgQueue *, OSContStatus *);
extern s32 osContStartQuery(OSMesgQueue *);
extern s32 osContStartReadData(OSMesgQueue *);

extern s32 osContSetCh(u8);

extern void osContGetQuery(OSContStatus *);
extern void osContGetReadData(OSContPad *);



extern s32 osPfsInitPak(OSMesgQueue *, OSPfs *, int);
extern s32 osPfsRepairId(OSPfs *);
extern s32 osPfsInit(OSMesgQueue *, OSPfs *, int);
extern s32 osPfsReFormat(OSPfs *, OSMesgQueue *, int);
extern s32 osPfsChecker(OSPfs *);
extern s32 osPfsAllocateFile(OSPfs *, u16, u32, u8 *, u8 *, int, s32 *);
extern s32 osPfsFindFile(OSPfs *, u16, u32, u8 *, u8 *, s32 *);
extern s32 osPfsDeleteFile(OSPfs *, u16, u32, u8 *, u8 *);
extern s32 osPfsReadWriteFile(OSPfs *, s32, u8, int, int, u8 *);
extern s32 osPfsFileState(OSPfs *, s32, OSPfsState *);
extern s32 osPfsGetLabel(OSPfs *, u8 *, int *);
extern s32 osPfsSetLabel(OSPfs *, u8 *);
extern s32 osPfsIsPlug(OSMesgQueue *, u8 *);
extern s32 osPfsFreeBlocks(OSPfs *, s32 *);
extern s32 osPfsNumFiles(OSPfs *, s32 *, s32 *);



extern s32 osEepromProbe(OSMesgQueue *);
extern s32 osEepromRead(OSMesgQueue *, u8, u8 *);
extern s32 osEepromWrite(OSMesgQueue *, u8, u8 *);
extern s32 osEepromLongRead(OSMesgQueue *, u8, u8 *, int);
extern s32 osEepromLongWrite(OSMesgQueue *, u8, u8 *, int);



extern s32 osMotorInit(OSMesgQueue *, OSPfs *, int);
extern s32 osMotorStop(OSPfs *);
extern s32 osMotorStart(OSPfs *);



extern OSPiHandle *osCartRomInit(void);
extern OSPiHandle *osLeoDiskInit(void);
extern OSPiHandle *osDriveRomInit(void);

extern s32 osEPiDeviceType(OSPiHandle *, OSPiInfo *);
extern s32 osEPiRawWriteIo(OSPiHandle *, u32 , u32);
extern s32 osEPiRawReadIo(OSPiHandle *, u32 , u32 *);
extern s32 osEPiRawStartDma(OSPiHandle *, s32 , u32 , void *, u32 );
extern s32 osEPiWriteIo(OSPiHandle *, u32 , u32 );
extern s32 osEPiReadIo(OSPiHandle *, u32 , u32 *);
extern s32 osEPiStartDma(OSPiHandle *, OSIoMesg *, s32);
extern s32 osEPiLinkHandle(OSPiHandle *);



extern void osProfileInit(OSProf *, u32 profcnt);
extern void osProfileStart(u32);
extern void osProfileFlush(void);
extern void osProfileStop(void);



extern s32 osTestHost(void);
extern void osReadHost(void *, u32);
extern void osWriteHost(void *, u32);
extern void osAckRamromRead(void);
extern void osAckRamromWrite(void);




extern void bcopy(const void *, void *, int);
extern int bcmp(const void *, const void *, int);
extern void bzero(void *, int);



extern void osInitialize(void);
extern u32 osGetCount(void);
extern void osExit(void);
extern u32 osGetMemSize(void);



extern int sprintf(char *s, const char *fmt, ...);
extern void osSyncPrintf(const char *fmt, ...);
extern void osAsyncPrintf(const char *fmt, ...);
extern int osSyncGetChars(char *buf);
extern int osAsyncGetChars(char *buf);
# 12 "include/ultra64.h" 2

# 1 "include/PR/gu.h" 1
# 24 "include/PR/gu.h"
# 1 "include/PR/mbi.h" 1



# 1 "include/platform_info.h" 1
# 5 "include/PR/mbi.h" 2
# 65 "include/PR/mbi.h"
# 1 "include/PR/gbi.h" 1
# 1112 "include/PR/gbi.h"
typedef struct {

 short ob[3];



 unsigned short flag;
 short tc[2];
 unsigned char cn[4];
} Vtx_t;




typedef struct {

 short ob[3];



 unsigned short flag;
 short tc[2];
 signed char n[3];
 unsigned char a;
} Vtx_tn;

typedef union {
    Vtx_t v;
    Vtx_tn n;
    long long int force_structure_alignment;
} Vtx;





typedef struct {
  void *SourceImagePointer;
  void *TlutPointer;
  short Stride;
  short SubImageWidth;
  short SubImageHeight;
  char SourceImageType;
  char SourceImageBitSize;
  short SourceImageOffsetS;
  short SourceImageOffsetT;



  char dummy[4];

} uSprite_t;

typedef union {
  uSprite_t s;


  long long int force_structure_allignment[3];
} uSprite;




typedef struct {
 unsigned char flag;
 unsigned char v[3];
} Tri;







typedef s32 Mtx_t[4][4];

typedef union {
    Mtx_t m;
    long long int force_structure_alignment;
} Mtx;
# 1232 "include/PR/gbi.h"
typedef struct {
 short vscale[4];
 short vtrans[4];

} Vp_t;

typedef union {
    Vp_t vp;
    long long int force_structure_alignment;
} Vp;
# 1398 "include/PR/gbi.h"
typedef struct {
  unsigned char col[3];
  char pad1;
  unsigned char colc[3];
  char pad2;
  signed char dir[3];
  char pad3;
} Light_t;

typedef struct {
  unsigned char col[3];
  char pad1;
  unsigned char colc[3];
  char pad2;
} Ambient_t;

typedef struct {
  int x1,y1,x2,y2;
} Hilite_t;

typedef union {
    Light_t l;
    long long int force_structure_alignment[2];
} Light;

typedef union {
    Ambient_t l;
    long long int force_structure_alignment[1];
} Ambient;

typedef struct {
    Ambient a;
    Light l[7];
} Lightsn;

typedef struct {
    Ambient a;
    Light l[1];
} Lights0;

typedef struct {
    Ambient a;
    Light l[1];
} Lights1;

typedef struct {
    Ambient a;
    Light l[2];
} Lights2;

typedef struct {
    Ambient a;
    Light l[3];
} Lights3;

typedef struct {
    Ambient a;
    Light l[4];
} Lights4;

typedef struct {
    Ambient a;
    Light l[5];
} Lights5;

typedef struct {
    Ambient a;
    Light l[6];
} Lights6;

typedef struct {
    Ambient a;
    Light l[7];
} Lights7;

typedef struct {
    Light l[2];
} LookAt;

typedef union {
    Hilite_t h;
    long int force_structure_alignment[4];
} Hilite;
# 1542 "include/PR/gbi.h"
typedef struct {
 int cmd:8;
 unsigned int par:8;
 unsigned int len:16;
 uintptr_t addr;
} Gdma;




typedef struct {
  int cmd:8;
  int pad:24;
  Tri tri;
} Gtri;

typedef struct {
  int cmd:8;
  int pad1:24;
  int pad2:24;
  unsigned char param:8;
} Gpopmtx;
# 1574 "include/PR/gbi.h"
typedef struct {
  int cmd:8;
  int pad0:8;
  int mw_index:8;
  int number:8;
  int pad1:8;
  int base:24;
} Gsegment;

typedef struct {
  int cmd:8;
  int pad0:8;
  int sft:8;
  int len:8;
  unsigned int data:32;
} GsetothermodeL;

typedef struct {
  int cmd:8;
  int pad0:8;
  int sft:8;
  int len:8;
  unsigned int data:32;
} GsetothermodeH;

typedef struct {
  unsigned char cmd;
  unsigned char lodscale;
  unsigned char tile;
  unsigned char on;
  unsigned short s;
  unsigned short t;
} Gtexture;

typedef struct {
  int cmd:8;
  int pad:24;
  Tri line;
} Gline3D;

typedef struct {
  int cmd:8;
  int pad1:24;
  short int pad2;
  short int scale;
} Gperspnorm;





typedef struct {
                int cmd:8;
                unsigned int fmt:3;
                unsigned int siz:2;
                unsigned int pad:7;
                unsigned int wd:12;
                uintptr_t dram;
} Gsetimg;

typedef struct {
  int cmd:8;
  unsigned int muxs0:24;
  unsigned int muxs1:32;
} Gsetcombine;

typedef struct {
  int cmd:8;
  unsigned char pad;
  unsigned char prim_min_level;
  unsigned char prim_level;
  unsigned long color;
} Gsetcolor;

typedef struct {
  int cmd:8;
  int x0:10;
  int x0frac:2;
  int y0:10;
  int y0frac:2;
  unsigned int pad:8;
  int x1:10;
  int x1frac:2;
  int y1:10;
  int y1frac:2;
} Gfillrect;

typedef struct {
  int cmd:8;
  unsigned int fmt:3;
  unsigned int siz:2;
  unsigned int pad0:1;
  unsigned int line:9;
  unsigned int tmem:9;
  unsigned int pad1:5;
  unsigned int tile:3;
  unsigned int palette:4;
  unsigned int ct:1;
  unsigned int mt:1;
  unsigned int maskt:4;
  unsigned int shiftt:4;
  unsigned int cs:1;
  unsigned int ms:1;
  unsigned int masks:4;
  unsigned int shifts:4;
} Gsettile;

typedef struct {
  int cmd:8;
  unsigned int sl:12;
  unsigned int tl:12;
  int pad:5;
  unsigned int tile:3;
  unsigned int sh:12;
  unsigned int th:12;
} Gloadtile;

typedef Gloadtile Gloadblock;

typedef Gloadtile Gsettilesize;

typedef Gloadtile Gloadtlut;

typedef struct {
  unsigned int cmd:8;
  unsigned int xl:12;
  unsigned int yl:12;
  unsigned int pad1:5;
  unsigned int tile:3;
  unsigned int xh:12;
  unsigned int yh:12;
  unsigned int s:16;
  unsigned int t:16;
  unsigned int dsdx:16;
  unsigned int dtdy:16;
} Gtexrect;




typedef struct {
    unsigned long w0;
    unsigned long w1;
    unsigned long w2;
    unsigned long w3;
} TexRect;
# 1728 "include/PR/gbi.h"
typedef struct {
 uintptr_t w0;
 uintptr_t w1;
} Gwords;
# 1741 "include/PR/gbi.h"
typedef union {
 Gwords words;

 Gdma dma;
 Gtri tri;
 Gline3D line;
 Gpopmtx popmtx;
 Gsegment segment;
 GsetothermodeH setothermodeH;
 GsetothermodeL setothermodeL;
 Gtexture texture;
 Gperspnorm perspnorm;
 Gsetimg setimg;
 Gsetcombine setcombine;
 Gsetcolor setcolor;
 Gfillrect fillrect;
 Gsettile settile;
 Gloadtile loadtile;
 Gsettilesize settilesize;
 Gloadtlut loadtlut;

 long long int force_structure_alignment;
} Gfx;
# 66 "include/PR/mbi.h" 2







# 1 "include/PR/abi.h" 1
# 78 "include/PR/abi.h"
typedef struct {
   unsigned int cmd:8;
 unsigned int flags:8;
 unsigned int gain:16;
 unsigned int addr;
} Aadpcm;

typedef struct {
   unsigned int cmd:8;
 unsigned int flags:8;
 unsigned int gain:16;
 unsigned int addr;
} Apolef;

typedef struct {
   unsigned int cmd:8;
 unsigned int flags:8;
 unsigned int pad1:16;
 unsigned int addr;
} Aenvelope;

typedef struct {
   unsigned int cmd:8;
 unsigned int pad1:8;
 unsigned int dmem:16;
 unsigned int pad2:16;
 unsigned int count:16;
} Aclearbuff;

typedef struct {
   unsigned int cmd:8;
 unsigned int pad1:8;
 unsigned int pad2:16;
 unsigned int inL:16;
        unsigned int inR:16;
} Ainterleave;

typedef struct {
   unsigned int cmd:8;
 unsigned int pad1:24;
 unsigned int addr;
} Aloadbuff;

typedef struct {
   unsigned int cmd:8;
 unsigned int flags:8;
 unsigned int pad1:16;
 unsigned int addr;
} Aenvmixer;

typedef struct {
   unsigned int cmd:8;
 unsigned int flags:8;
 unsigned int gain:16;
 unsigned int dmemi:16;
 unsigned int dmemo:16;
} Amixer;

typedef struct {
   unsigned int cmd:8;
 unsigned int flags:8;
 unsigned int dmem2:16;
 unsigned int addr;
} Apan;

typedef struct {
   unsigned int cmd:8;
 unsigned int flags:8;
 unsigned int pitch:16;
 unsigned int addr;
} Aresample;

typedef struct {
   unsigned int cmd:8;
 unsigned int flags:8;
 unsigned int pad1:16;
 unsigned int addr;
} Areverb;

typedef struct {
   unsigned int cmd:8;
 unsigned int pad1:24;
 unsigned int addr;
} Asavebuff;

typedef struct {
   unsigned int cmd:8;
 unsigned int pad1:24;
 unsigned int pad2:2;
 unsigned int number:4;
 unsigned int base:24;
} Asegment;

typedef struct {
   unsigned int cmd:8;
 unsigned int flags:8;
 unsigned int dmemin:16;
 unsigned int dmemout:16;
 unsigned int count:16;
} Asetbuff;

typedef struct {
   unsigned int cmd:8;
 unsigned int flags:8;
 unsigned int vol:16;
 unsigned int voltgt:16;
 unsigned int volrate:16;
} Asetvol;

typedef struct {
    unsigned int cmd:8;
    unsigned int pad1:8;
    unsigned int dmemin:16;
    unsigned int dmemout:16;
    unsigned int count:16;
} Admemmove;

typedef struct {
    unsigned int cmd:8;
    unsigned int pad1:8;
    unsigned int count:16;
    unsigned int addr;
} Aloadadpcm;

typedef struct {
    unsigned int cmd:8;
    unsigned int pad1:8;
    unsigned int pad2:16;
    unsigned int addr;
} Asetloop;





typedef struct {
 unsigned int w0;
 unsigned int w1;
} Awords;

typedef union {
 Awords words;
 Aadpcm adpcm;
        Apolef polef;
 Aclearbuff clearbuff;
 Aenvelope envelope;
        Ainterleave interleave;
 Aloadbuff loadbuff;
        Aenvmixer envmixer;
 Aresample resample;
 Areverb reverb;
 Asavebuff savebuff;
 Asegment segment;
 Asetbuff setbuff;
 Asetvol setvol;
        Admemmove dmemmove;
        Aloadadpcm loadadpcm;
        Amixer mixer;
        Asetloop setloop;
        long long int force_union_align;
} Acmd;






typedef short ADPCM_STATE[16];




typedef short POLEF_STATE[4];




typedef short RESAMPLE_STATE[16];
# 266 "include/PR/abi.h"
typedef short ENVMIX_STATE[40];
# 74 "include/PR/mbi.h" 2
# 25 "include/PR/gu.h" 2

# 1 "include/PR/sptask.h" 1
# 132 "include/PR/sptask.h"
typedef struct
{
             u32 type;
             u32 flags;

             u64 *ucode_boot;
             u32 ucode_boot_size;

             u64 *ucode;
             u32 ucode_size;

             u64 *ucode_data;
             u32 ucode_data_size;

             u64 *dram_stack;
             u32 dram_stack_size;

             u64 *output_buff;
             u64 *output_buff_size;

             u64 *data_ptr;
             u32 data_size;

             u64 *yield_data_ptr;
             u32 yield_data_size;
} OSTask_t;

typedef union {
    OSTask_t t;
    long long int force_structure_alignment;
} OSTask;

typedef u32 OSYieldResult;







void osSpTaskLoad(OSTask *task);
void osSpTaskStartGo(OSTask *task);
void osSpTaskYield(void);
OSYieldResult osSpTaskYielded(OSTask *task);
# 27 "include/PR/gu.h" 2
# 54 "include/PR/gu.h"
typedef struct {
 unsigned char *base;
 int fmt, siz;
 int xsize, ysize;
 int lsize;

 int addr;
 int w, h;
 int s, t;
} Image;

typedef struct {
 float col[3];
 float pos[3];
 float a1, a2;
} PositionalLight;






extern int guLoadTextureBlockMipMap(Gfx **glist, unsigned char *tbuf, Image *im,
  unsigned char startTile, unsigned char pal, unsigned char cms,
  unsigned char cmt, unsigned char masks, unsigned char maskt,
  unsigned char shifts, unsigned char shiftt, unsigned char cfs,
  unsigned char cft);

extern int guGetDPLoadTextureTileSz (int ult, int lrt);
extern void guDPLoadTextureTile (Gfx *glistp, void *timg,
   int texl_fmt, int texl_size,
   int img_width, int img_height,
   int uls, int ult, int lrs, int lrt,
   int palette,
   int cms, int cmt,
   int masks, int maskt,
   int shifts, int shiftt);
# 100 "include/PR/gu.h"
extern void guMtxIdent(Mtx *m);
extern void guMtxIdentF(float mf[4][4]);
extern void guOrtho(Mtx *m, float l, float r, float b, float t,
      float n, float f, float scale);
extern void guOrthoF(float mf[4][4], float l, float r, float b, float t,
       float n, float f, float scale);
extern void guFrustum(Mtx *m, float l, float r, float b, float t,
        float n, float f, float scale);
extern void guFrustumF(float mf[4][4], float l, float r, float b, float t,
         float n, float f, float scale);
extern void guPerspective(Mtx *m, u16 *perspNorm, float fovy,
     float aspect, float near, float far, float scale);
extern void guPerspectiveF(float mf[4][4], u16 *perspNorm, float fovy,
      float aspect, float near, float far, float scale);
extern void guLookAt(Mtx *m,
   float xEye, float yEye, float zEye,
   float xAt, float yAt, float zAt,
   float xUp, float yUp, float zUp);
extern void guLookAtF(float mf[4][4], float xEye, float yEye, float zEye,
        float xAt, float yAt, float zAt,
        float xUp, float yUp, float zUp);
extern void guLookAtReflect(Mtx *m, LookAt *l,
   float xEye, float yEye, float zEye,
   float xAt, float yAt, float zAt,
   float xUp, float yUp, float zUp);
extern void guLookAtReflectF(float mf[4][4], LookAt *l,
        float xEye, float yEye, float zEye,
        float xAt, float yAt, float zAt,
        float xUp, float yUp, float zUp);
extern void guLookAtHilite(Mtx *m, LookAt *l, Hilite *h,
                float xEye, float yEye, float zEye,
                float xAt, float yAt, float zAt,
                float xUp, float yUp, float zUp,
                float xl1, float yl1, float zl1,
                float xl2, float yl2, float zl2,
  int twidth, int theight);
extern void guLookAtHiliteF(float mf[4][4], LookAt *l, Hilite *h,
  float xEye, float yEye, float zEye,
  float xAt, float yAt, float zAt,
  float xUp, float yUp, float zUp,
  float xl1, float yl1, float zl1,
  float xl2, float yl2, float zl2,
  int twidth, int theight);
extern void guLookAtStereo(Mtx *m,
   float xEye, float yEye, float zEye,
   float xAt, float yAt, float zAt,
   float xUp, float yUp, float zUp,
   float eyedist);
extern void guLookAtStereoF(float mf[4][4],
         float xEye, float yEye, float zEye,
         float xAt, float yAt, float zAt,
         float xUp, float yUp, float zUp,
   float eyedist);
extern void guRotate(Mtx *m, float a, float x, float y, float z);
extern void guRotateF(float mf[4][4], float a, float x, float y, float z);
extern void guRotateRPY(Mtx *m, float r, float p, float y);
extern void guRotateRPYF(float mf[4][4], float r, float p, float h);
extern void guAlign(Mtx *m, float a, float x, float y, float z);
extern void guAlignF(float mf[4][4], float a, float x, float y, float z);
extern void guScale(Mtx *m, float x, float y, float z);
extern void guScaleF(float mf[4][4], float x, float y, float z);
extern void guTranslate(Mtx *m, float x, float y, float z);
extern void guTranslateF(float mf[4][4], float x, float y, float z);
extern void guPosition(Mtx *m, float r, float p, float h, float s,
         float x, float y, float z);
extern void guPositionF(float mf[4][4], float r, float p, float h, float s,
   float x, float y, float z);
extern void guMtxF2L(float mf[4][4], Mtx *m);
extern void guMtxL2F(float mf[4][4], Mtx *m);
extern void guMtxCatF(float m[4][4], float n[4][4], float r[4][4]);
extern void guMtxCatL(Mtx *m, Mtx *n, Mtx *res);
extern void guMtxXFMF(float mf[4][4], float x, float y, float z,
        float *ox, float *oy, float *oz);
extern void guMtxXFML(Mtx *m, float x, float y, float z,
        float *ox, float *oy, float *oz);


extern void guNormalize(float *x, float *y, float *z);


void guPosLight(PositionalLight *pl, Light *l,
                float xOb, float yOb, float zOb);
void guPosLightHilite(PositionalLight *pl1, PositionalLight *pl2,
                Light *l1, Light *l2,
                LookAt *l, Hilite *h,
                float xEye, float yEye, float zEye,
                float xOb, float yOb, float zOb,
                float xUp, float yUp, float zUp,
                int twidth, int theight);
extern int guRandom(void);




extern float sinf(float angle);
extern float cosf(float angle);
extern signed short sins (unsigned short angle);
extern signed short coss (unsigned short angle);
extern float sqrtf(float value);
# 211 "include/PR/gu.h"
extern void guParseRdpDL(u64 *rdp_dl, u64 nbytes, u8 flags);
extern void guParseString(char *StringPointer, u64 nbytes);
# 224 "include/PR/gu.h"
extern void
guBlinkRdpDL(u64 *rdp_dl_in, u64 nbytes_in,
             u64 *rdp_dl_out, u64 *nbytes_out,
             u32 x, u32 y, u32 radius,
             u8 red, u8 green, u8 blue,
             u8 flags);
# 242 "include/PR/gu.h"
extern void guParseGbiDL(u64 *gbi_dl, u32 nbytes, u8 flags);
extern void guDumpGbiDL(OSTask *tp,u8 flags);
# 252 "include/PR/gu.h"
typedef struct {
    int dataSize;
    int dlType;
    int flags;
    u32 paddr;
} guDLPrintCB;

void guSprite2DInit(uSprite *SpritePointer,
      void *SourceImagePointer,
      void *TlutPointer,
      int Stride,
      int SubImageWidth,
      int SubImageHeight,
      int SourceImageType,
      int SourceImageBitSize,
      int SourceImageOffsetS,
      int SourceImageOffsetT);
# 14 "include/ultra64.h" 2

# 1 "include/PR/libaudio.h" 1
# 70 "include/PR/libaudio.h"
typedef s32 ALMicroTime;
typedef u8 ALPan;
# 113 "include/PR/libaudio.h"
typedef struct ALLink_s {
    struct ALLink_s *next;
    struct ALLink_s *prev;
} ALLink;

void alUnlink(ALLink *element);
void alLink(ALLink *element, ALLink *after);

typedef s32 (*ALDMAproc)(s32 addr, s32 len, void *state);
typedef ALDMAproc (*ALDMANew)(void *state);

void alCopy(void *src, void *dest, s32 len);

typedef struct {
    u8 *base;
    u8 *cur;
    s32 len;
    s32 count;
} ALHeap;





void alHeapInit(ALHeap *hp, u8 *base, s32 len);
void *alHeapDBAlloc(u8 *file, s32 line, ALHeap *hp, s32 num, s32 size);
s32 alHeapCheck(ALHeap *hp);
# 158 "include/PR/libaudio.h"
typedef u8 ALFxId;
typedef void *ALFxRef;
# 168 "include/PR/libaudio.h"
enum {AL_ADPCM_WAVE = 0,
         AL_RAW16_WAVE};

typedef struct {
    s32 order;
    s32 npredictors;
    s16 book[1];
} ALADPCMBook;

typedef struct {
    u32 start;
    u32 end;
    u32 count;
    ADPCM_STATE state;
} ALADPCMloop;

typedef struct {
    u32 start;
    u32 end;
    u32 count;
} ALRawLoop;

typedef struct {
    ALMicroTime attackTime;
    ALMicroTime decayTime;
    ALMicroTime releaseTime;
    u8 attackVolume;
    u8 decayVolume;
} ALEnvelope;

typedef struct {
    u8 velocityMin;
    u8 velocityMax;
    u8 keyMin;
    u8 keyMax;
    u8 keyBase;
    s8 detune;
} ALKeyMap;

typedef struct {
    ALADPCMloop *loop;
    ALADPCMBook *book;
} ALADPCMWaveInfo;

typedef struct {
    ALRawLoop *loop;
} ALRAWWaveInfo;

typedef struct ALWaveTable_s {
    u8 *base;
    s32 len;
    u8 type;
    u8 flags;
    union {
        ALADPCMWaveInfo adpcmWave;
        ALRAWWaveInfo rawWave;
    } waveInfo;
} ALWaveTable;

typedef struct ALSound_s {
    ALEnvelope *envelope;
    ALKeyMap *keyMap;
    ALWaveTable *wavetable;
    ALPan samplePan;
    u8 sampleVolume;
    u8 flags;
} ALSound;

typedef struct {
    u8 volume;
    ALPan pan;
    u8 priority;
    u8 flags;
    u8 tremType;
    u8 tremRate;
    u8 tremDepth;
    u8 tremDelay;
    u8 vibType;
    u8 vibRate;
    u8 vibDepth;
    u8 vibDelay;
    s16 bendRange;
    s16 soundCount;
    ALSound *soundArray[1];
} ALInstrument;

typedef struct ALBank_s {
    s16 instCount;
    u8 flags;
    u8 pad;
    s32 sampleRate;
    ALInstrument *percussion;
    ALInstrument *instArray[1];
} ALBank;

typedef struct {
    s16 revision;
    s16 bankCount;
    ALBank *bankArray[1];
} ALBankFile;

void alBnkfNew(ALBankFile *f, u8 *table);






typedef struct {
    u8 *offset;
    s32 len;
} ALSeqData;

typedef struct {
    s16 revision;
    s16 seqCount;
    ALSeqData seqArray[1];
} ALSeqFile;

void alSeqFileNew(ALSeqFile *f, u8 *base);




typedef ALMicroTime (*ALVoiceHandler)(void *);

typedef struct {
    s32 maxVVoices;
    s32 maxPVoices;
    s32 maxUpdates;
    s32 maxFXbusses;
    void *dmaproc;
    ALHeap *heap;
    s32 outputRate;
    ALFxId fxType;
    s32 *params;
} ALSynConfig;

typedef struct ALPlayer_s {
    struct ALPlayer_s *next;
    void *clientData;
    ALVoiceHandler handler;
    ALMicroTime callTime;
    s32 samplesLeft;
} ALPlayer;

typedef struct ALVoice_s {
    ALLink node;
    struct PVoice_s *pvoice;
    ALWaveTable *table;
    void *clientPrivate;
    s16 state;
    s16 priority;
    s16 fxBus;
    s16 unityPitch;
} ALVoice;

typedef struct ALVoiceConfig_s {
    s16 priority;
    s16 fxBus;
    u8 unityPitch;
} ALVoiceConfig;

typedef struct {
    ALPlayer *head;
    ALLink pFreeList;
    ALLink pAllocList;
    ALLink pLameList;
    s32 paramSamples;
    s32 curSamples;
    ALDMANew dma;
    ALHeap *heap;

    struct ALParam_s *paramList;

    struct ALMainBus_s *mainBus;
    struct ALAuxBus_s *auxBus;
    struct ALFilter_s *outputFilter;

    s32 numPVoices;
    s32 maxAuxBusses;
    s32 outputRate;
    s32 maxOutSamples;

} ALSynth;

void alSynNew(ALSynth *s, ALSynConfig *config);
void alSynDelete(ALSynth *s);

void alSynAddPlayer(ALSynth *s, ALPlayer *client);
void alSynRemovePlayer(ALSynth *s, ALPlayer *client);

s32 alSynAllocVoice(ALSynth *s, ALVoice *v, ALVoiceConfig *vc);
void alSynFreeVoice(ALSynth *s, ALVoice *voice);

void alSynStartVoice(ALSynth *s, ALVoice *voice, ALWaveTable *w);
void alSynStartVoiceParams(ALSynth *s, ALVoice *voice, ALWaveTable *w,
                              f32 pitch, s16 vol, ALPan pan, u8 fxmix,
                              ALMicroTime t);
void alSynStopVoice(ALSynth *s, ALVoice *voice);

void alSynSetVol(ALSynth *s, ALVoice *v, s16 vol, ALMicroTime delta);
void alSynSetPitch(ALSynth *s, ALVoice *voice, f32 ratio);
void alSynSetPan(ALSynth *s, ALVoice *voice, ALPan pan);
void alSynSetFXMix(ALSynth *s, ALVoice *voice, u8 fxmix);
void alSynSetPriority(ALSynth *s, ALVoice *voice, s16 priority);
s16 alSynGetPriority(ALSynth *s, ALVoice *voice);

ALFxRef *alSynAllocFX(ALSynth *s, s16 bus, ALSynConfig *c, ALHeap *hp);
ALFxRef alSynGetFXRef(ALSynth *s, s16 bus, s16 index);
void alSynFreeFX(ALSynth *s, ALFxRef *fx);
void alSynSetFXParam(ALSynth *s, ALFxRef fx, s16 paramID, void *param);




typedef struct {
    ALSynth drvr;
} ALGlobals;

extern ALGlobals *alGlobals;

void alInit(ALGlobals *glob, ALSynConfig *c);
void alClose(ALGlobals *glob);

Acmd *alAudioFrame(Acmd *cmdList, s32 *cmdLen, s16 *outBuf, s32 outLen);
# 413 "include/PR/libaudio.h"
enum ALMsg {
    AL_SEQ_REF_EVT,
    AL_SEQ_MIDI_EVT,
    AL_SEQP_MIDI_EVT,
    AL_TEMPO_EVT,
    AL_SEQ_END_EVT,
    AL_NOTE_END_EVT,
    AL_SEQP_ENV_EVT,
    AL_SEQP_META_EVT,
    AL_SEQP_PROG_EVT,
    AL_SEQP_API_EVT,
    AL_SEQP_VOL_EVT,
    AL_SEQP_LOOP_EVT,
    AL_SEQP_PRIORITY_EVT,
    AL_SEQP_SEQ_EVT,
    AL_SEQP_BANK_EVT,
    AL_SEQP_PLAY_EVT,
    AL_SEQP_STOP_EVT,
    AL_SEQP_STOPPING_EVT,
    AL_TRACK_END,
    AL_CSP_LOOPSTART,
    AL_CSP_LOOPEND,
    AL_CSP_NOTEOFF_EVT,
    AL_TREM_OSC_EVT,
    AL_VIB_OSC_EVT
};






enum AL_MIDIstatus {

    AL_MIDI_ChannelMask = 0x0F,
    AL_MIDI_StatusMask = 0xF0,


    AL_MIDI_ChannelVoice = 0x80,
    AL_MIDI_NoteOff = 0x80,
    AL_MIDI_NoteOn = 0x90,
    AL_MIDI_PolyKeyPressure = 0xA0,
    AL_MIDI_ControlChange = 0xB0,
    AL_MIDI_ChannelModeSelect = 0xB0,
    AL_MIDI_ProgramChange = 0xC0,
    AL_MIDI_ChannelPressure = 0xD0,
    AL_MIDI_PitchBendChange = 0xE0,


    AL_MIDI_SysEx = 0xF0,


    AL_MIDI_SystemCommon = 0xF1,
    AL_MIDI_TimeCodeQuarterFrame = 0xF1,
    AL_MIDI_SongPositionPointer = 0xF2,
    AL_MIDI_SongSelect = 0xF3,
    AL_MIDI_Undefined1 = 0xF4,
    AL_MIDI_Undefined2 = 0xF5,
    AL_MIDI_TuneRequest = 0xF6,
    AL_MIDI_EOX = 0xF7,


    AL_MIDI_SystemRealTime = 0xF8,
    AL_MIDI_TimingClock = 0xF8,
    AL_MIDI_Undefined3 = 0xF9,
    AL_MIDI_Start = 0xFA,
    AL_MIDI_Continue = 0xFB,
    AL_MIDI_Stop = 0xFC,
    AL_MIDI_Undefined4 = 0xFD,
    AL_MIDI_ActiveSensing = 0xFE,
    AL_MIDI_SystemReset = 0xFF,
    AL_MIDI_Meta = 0xFF
};

enum AL_MIDIctrl {
    AL_MIDI_VOLUME_CTRL = 0x07,
    AL_MIDI_PAN_CTRL = 0x0A,
    AL_MIDI_PRIORITY_CTRL = 0x10,
    AL_MIDI_FX_CTRL_0 = 0x14,
    AL_MIDI_FX_CTRL_1 = 0x15,
    AL_MIDI_FX_CTRL_2 = 0x16,
    AL_MIDI_FX_CTRL_3 = 0x17,
    AL_MIDI_FX_CTRL_4 = 0x18,
    AL_MIDI_FX_CTRL_5 = 0x19,
    AL_MIDI_FX_CTRL_6 = 0x1A,
    AL_MIDI_FX_CTRL_7 = 0x1B,
    AL_MIDI_FX_CTRL_8 = 0x1C,
    AL_MIDI_FX_CTRL_9 = 0x1D,
    AL_MIDI_SUSTAIN_CTRL = 0x40,
    AL_MIDI_FX1_CTRL = 0x5B,
    AL_MIDI_FX3_CTRL = 0x5D
};

enum AL_MIDImeta {
    AL_MIDI_META_TEMPO = 0x51,
    AL_MIDI_META_EOT = 0x2f
};
# 520 "include/PR/libaudio.h"
typedef struct {
    u8 *curPtr;
    s32 lastTicks;
    s32 curTicks;
    s16 lastStatus;
} ALSeqMarker;

typedef struct {
    s32 ticks;
    u8 status;
    u8 byte1;
    u8 byte2;
    u32 duration;
} ALMIDIEvent;

typedef struct {
    s32 ticks;
    u8 status;
    u8 type;
    u8 len;
    u8 byte1;
    u8 byte2;
    u8 byte3;
} ALTempoEvent;

typedef struct {
    s32 ticks;
    u8 status;
    u8 type;
    u8 len;
} ALEndEvent;

typedef struct {
    struct ALVoice_s *voice;
} ALNoteEvent;

typedef struct {
    struct ALVoice_s *voice;
    ALMicroTime delta;
    u8 vol;
} ALVolumeEvent;

typedef struct {
    s16 vol;
} ALSeqpVolEvent;

typedef struct {
    ALSeqMarker *start;
    ALSeqMarker *end;
    s32 count;
} ALSeqpLoopEvent;

typedef struct {
    u8 chan;
    u8 priority;
} ALSeqpPriorityEvent;

typedef struct {
    void *seq;
} ALSeqpSeqEvent;

typedef struct {
    ALBank *bank;
} ALSeqpBankEvent;

typedef struct {
    struct ALVoiceState_s *vs;
    void *oscState;
    u8 chan;
} ALOscEvent;

typedef struct {
    s16 type;
    union {
        ALMIDIEvent midi;
        ALTempoEvent tempo;
        ALEndEvent end;
        ALNoteEvent note;
        ALVolumeEvent vol;
        ALSeqpLoopEvent loop;
        ALSeqpVolEvent spvol;
 ALSeqpPriorityEvent sppriority;
 ALSeqpSeqEvent spseq;
 ALSeqpBankEvent spbank;
        ALOscEvent osc;
    } msg;
} ALEvent;

typedef struct {
    ALLink node;
    ALMicroTime delta;
    ALEvent evt;
} ALEventListItem;

typedef struct {
    ALLink freeList;
    ALLink allocList;
    s32 eventCount;
} ALEventQueue;

void alEvtqNew(ALEventQueue *evtq, ALEventListItem *items,
                          s32 itemCount);
ALMicroTime alEvtqNextEvent(ALEventQueue *evtq, ALEvent *evt);
void alEvtqPostEvent(ALEventQueue *evtq, ALEvent *evt,
                                ALMicroTime delta);
void alEvtqFlush(ALEventQueue *evtq);
void alEvtqFlushType(ALEventQueue *evtq, s16 type);
# 636 "include/PR/libaudio.h"
typedef struct ALVoiceState_s {
    struct ALVoiceState_s *next;
    ALVoice voice;
    ALSound *sound;
    ALMicroTime envEndTime;
    f32 pitch;
    f32 vibrato;
    u8 envGain;
    u8 channel;
    u8 key;
    u8 velocity;
    u8 envPhase;
    u8 phase;
    u8 tremelo;
    u8 flags;

} ALVoiceState;

typedef struct {
    ALInstrument *instrument;
    s16 bendRange;
    ALFxId fxId;
    ALPan pan;
    u8 priority;
    u8 vol;
    u8 fxmix;
    u8 sustain;
    f32 pitchBend;
} ALChanState;

typedef struct ALSeq_s {
    u8 *base;
    u8 *trackStart;
    u8 *curPtr;
    s32 lastTicks;
    s32 len;
    f32 qnpt;
    s16 division;
    s16 lastStatus;
} ALSeq;

typedef struct {
    u32 trackOffset[16];
    u32 division;
} ALCMidiHdr;

typedef struct ALCSeq_s {
    ALCMidiHdr *base;
    u32 validTracks;
    f32 qnpt;
    u32 lastTicks;
    u32 lastDeltaTicks;
    u32 deltaFlag;
    u8 *curLoc[16];


    u8 *curBUPtr[16];
    u8 curBULen[16];
    u8 lastStatus[16];
    u32 evtDeltaTicks[16];
} ALCSeq;

typedef struct {
    u32 validTracks;
    s32 lastTicks;
    u32 lastDeltaTicks;
    u8 *curLoc[16];
    u8 *curBUPtr[16];
    u8 curBULen[16];
    u8 lastStatus[16];
    u32 evtDeltaTicks[16];
} ALCSeqMarker;





typedef struct {
    s32 maxVoices;
    s32 maxEvents;
    u8 maxChannels;
    u8 debugFlags;
    ALHeap *heap;
    void *initOsc;
    void *updateOsc;
    void *stopOsc;
} ALSeqpConfig;

typedef ALMicroTime (*ALOscInit)(void **oscState,f32 *initVal, u8 oscType,
                                   u8 oscRate, u8 oscDepth, u8 oscDelay);
typedef ALMicroTime (*ALOscUpdate)(void *oscState, f32 *updateVal);
typedef void (*ALOscStop)(void *oscState);

typedef struct {
    ALPlayer node;
    ALSynth *drvr;
    ALSeq *target;
    ALMicroTime curTime;
    ALBank *bank;
    s32 uspt;
    s32 nextDelta;
    s32 state;
    u16 chanMask;
    s16 vol;
    u8 maxChannels;
    u8 debugFlags;
    ALEvent nextEvent;
    ALEventQueue evtq;
    ALMicroTime frameTime;
    ALChanState *chanState;
    ALVoiceState *vAllocHead;
    ALVoiceState *vAllocTail;
    ALVoiceState *vFreeList;
    ALOscInit initOsc;
    ALOscUpdate updateOsc;
    ALOscStop stopOsc;
    ALSeqMarker *loopStart;
    ALSeqMarker *loopEnd;
    s32 loopCount;
} ALSeqPlayer;

typedef struct {
    ALPlayer node;
    ALSynth *drvr;
    ALCSeq *target;
    ALMicroTime curTime;
    ALBank *bank;
    s32 uspt;
    s32 nextDelta;
    s32 state;
    u16 chanMask;
    s16 vol;
    u8 maxChannels;
    u8 debugFlags;
    ALEvent nextEvent;
    ALEventQueue evtq;
    ALMicroTime frameTime;
    ALChanState *chanState;
    ALVoiceState *vAllocHead;
    ALVoiceState *vAllocTail;
    ALVoiceState *vFreeList;
    ALOscInit initOsc;
    ALOscUpdate updateOsc;
    ALOscStop stopOsc;
} ALCSPlayer;




void alSeqNew(ALSeq *seq, u8 *ptr, s32 len);
void alSeqNextEvent(ALSeq *seq, ALEvent *event);
s32 alSeqGetTicks(ALSeq *seq);
f32 alSeqTicksToSec(ALSeq *seq, s32 ticks, u32 tempo);
u32 alSeqSecToTicks(ALSeq *seq, f32 sec, u32 tempo);
void alSeqNewMarker(ALSeq *seq, ALSeqMarker *m, u32 ticks);
void alSeqSetLoc(ALSeq *seq, ALSeqMarker *marker);
void alSeqGetLoc(ALSeq *seq, ALSeqMarker *marker);



void alCSeqNew(ALCSeq *seq, u8 *ptr);
void alCSeqNextEvent(ALCSeq *seq,ALEvent *evt);
s32 alCSeqGetTicks(ALCSeq *seq);
f32 alCSeqTicksToSec(ALCSeq *seq, s32 ticks, u32 tempo);
u32 alCSeqSecToTicks(ALCSeq *seq, f32 sec, u32 tempo);
void alCSeqNewMarker(ALCSeq *seq, ALCSeqMarker *m, u32 ticks);
void alCSeqSetLoc(ALCSeq *seq, ALCSeqMarker *marker);
void alCSeqGetLoc(ALCSeq *seq, ALCSeqMarker *marker);




f32 alCents2Ratio(s32 cents);

void alSeqpNew(ALSeqPlayer *seqp, ALSeqpConfig *config);
void alSeqpDelete(ALSeqPlayer *seqp);
void alSeqpSetSeq(ALSeqPlayer *seqp, ALSeq *seq);
ALSeq *alSeqpGetSeq(ALSeqPlayer *seqp);
void alSeqpPlay(ALSeqPlayer *seqp);
void alSeqpStop(ALSeqPlayer *seqp);
s32 alSeqpGetState(ALSeqPlayer *seqp);
void alSeqpSetBank(ALSeqPlayer *seqp, ALBank *b);
void alSeqpSetTempo(ALSeqPlayer *seqp, s32 tempo);
s32 alSeqpGetTempo(ALSeqPlayer *seqp);
s16 alSeqpGetVol(ALSeqPlayer *seqp);
void alSeqpSetVol(ALSeqPlayer *seqp, s16 vol);
void alSeqpLoop(ALSeqPlayer *seqp, ALSeqMarker *start, ALSeqMarker *end, s32 count);

void alSeqpSetChlProgram(ALSeqPlayer *seqp, u8 chan, u8 prog);
s32 alSeqpGetChlProgram(ALSeqPlayer *seqp, u8 chan);
void alSeqpSetChlFXMix(ALSeqPlayer *seqp, u8 chan, u8 fxmix);
u8 alSeqpGetChlFXMix(ALSeqPlayer *seqp, u8 chan);
void alSeqpSetChlVol(ALSeqPlayer *seqp, u8 chan, u8 vol);
u8 alSeqpGetChlVol(ALSeqPlayer *seqp, u8 chan);
void alSeqpSetChlPan(ALSeqPlayer *seqp, u8 chan, ALPan pan);
ALPan alSeqpGetChlPan(ALSeqPlayer *seqp, u8 chan);
void alSeqpSetChlPriority(ALSeqPlayer *seqp, u8 chan, u8 priority);
u8 alSeqpGetChlPriority(ALSeqPlayer *seqp, u8 chan);
void alSeqpSendMidi(ALSeqPlayer *seqp, s32 ticks, u8 status, u8 byte1, u8 byte2);
# 852 "include/PR/libaudio.h"
void alCSPNew(ALCSPlayer *seqp, ALSeqpConfig *config);
void alCSPDelete(ALCSPlayer *seqp);
void alCSPSetSeq(ALCSPlayer *seqp, ALCSeq *seq);
ALCSeq *alCSPGetSeq(ALCSPlayer *seqp);
void alCSPPlay(ALCSPlayer *seqp);
void alCSPStop(ALCSPlayer *seqp);
s32 alCSPGetState(ALCSPlayer *seqp);
void alCSPSetBank(ALCSPlayer *seqp, ALBank *b);
void alCSPSetTempo(ALCSPlayer *seqp, s32 tempo);
s32 alCSPGetTempo(ALCSPlayer *seqp);
s16 alCSPGetVol(ALCSPlayer *seqp);
void alCSPSetVol(ALCSPlayer *seqp, s16 vol);

void alCSPSetChlProgram(ALCSPlayer *seqp, u8 chan, u8 prog);
s32 alCSPGetChlProgram(ALCSPlayer *seqp, u8 chan);
void alCSPSetChlFXMix(ALCSPlayer *seqp, u8 chan, u8 fxmix);
u8 alCSPGetChlFXMix(ALCSPlayer *seqp, u8 chan);
void alCSPSetChlPan(ALCSPlayer *seqp, u8 chan, ALPan pan);
ALPan alCSPGetChlPan(ALCSPlayer *seqp, u8 chan);
void alCSPSetChlVol(ALCSPlayer *seqp, u8 chan, u8 vol);
u8 alCSPGetChlVol(ALCSPlayer *seqp, u8 chan);
void alCSPSetChlPriority(ALCSPlayer *seqp, u8 chan, u8 priority);
u8 alCSPGetChlPriority(ALCSPlayer *seqp, u8 chan);
void alCSPSendMidi(ALCSPlayer *seqp, s32 ticks, u8 status,
                       u8 byte1, u8 byte2);
# 895 "include/PR/libaudio.h"
typedef struct {
    s32 maxSounds;
    s32 maxEvents;
    ALHeap *heap;
} ALSndpConfig;

typedef struct {
    ALPlayer node;
    ALEventQueue evtq;
    ALEvent nextEvent;
    ALSynth *drvr;
    s32 target;
    void *sndState;
    s32 maxSounds;
    ALMicroTime frameTime;
    ALMicroTime nextDelta;
    ALMicroTime curTime;
} ALSndPlayer;

typedef s16 ALSndId;

void alSndpNew(ALSndPlayer *sndp, ALSndpConfig *c);
void alSndpDelete(ALSndPlayer *sndp);

ALSndId alSndpAllocate(ALSndPlayer *sndp, ALSound *sound);
void alSndpDeallocate(ALSndPlayer *sndp, ALSndId id);

void alSndpSetSound(ALSndPlayer *sndp, ALSndId id);
ALSndId alSndpGetSound(ALSndPlayer *sndp);

void alSndpPlay(ALSndPlayer *sndp);
void alSndpPlayAt(ALSndPlayer *sndp, ALMicroTime delta);
void alSndpStop(ALSndPlayer *sndp);

void alSndpSetVol(ALSndPlayer *sndp, s16 vol);
void alSndpSetPitch(ALSndPlayer *sndp, f32 pitch);
void alSndpSetPan(ALSndPlayer *sndp, ALPan pan);
void alSndpSetPriority(ALSndPlayer *sndp, ALSndId id, u8 priority);

void alSndpSetFXMix(ALSndPlayer *sndp, u8 mix);
s32 alSndpGetState(ALSndPlayer *sndp);


void alParseAbiCL(Acmd *cmdList, u32 nbytes);
# 16 "include/ultra64.h" 2
# 1 "include/PR/libultra.h" 1
# 12 "include/PR/libultra.h"
extern u32 osTvType;
extern u32 osRomBase;
extern u32 osResetType;
extern u32 osMemSize;
extern u8 osAppNmiBuffer[64];
# 17 "include/ultra64.h" 2

# 1 "include/PR/ucode.h" 1
# 9 "include/PR/ucode.h"
extern u64 rspF3DBootStart[], rspF3DBootEnd[];


extern u64 rspF3DStart[], rspF3DEnd[];


extern u64 rspF3DDataStart[], rspF3DDataEnd[];


extern u64 rspAspMainStart[], rspAspMainEnd[];


extern u64 rspAspMainDataStart[], rspAspMainDataEnd[];
# 19 "include/ultra64.h" 2
# 15 "<stdin>" 2
# 1 "src/bondgame.h" 1


# 1 "include/ultra64.h" 1
# 4 "src/bondgame.h" 2

# 1 "src/bondconstants.h" 1
# 16 "src/bondconstants.h"
# 1 "include/ultra64.h" 1
# 17 "src/bondconstants.h" 2
# 1 "include/CPPLib.h" 1
# 18 "src/bondconstants.h" 2

#pragma region Tools
# 167 "src/bondconstants.h"
#pragma endregion

#pragma region Bitflags







typedef enum ATTACKTYPE
{
    ATTACKTYPE_NONE,
    ATTACKTYPE_BOND = 1 << 0,
    ATTACKTYPE_FORWARD = 1 << 1,
    ATTACKTYPE_CHR = 1 << 2,
    ATTACKTYPE_PAD = 1 << 3,
    ATTACKTYPE_DIRECTION = 1 << 4,
    ATTACKTYPE_AIMONLY = 1 << 5,
    ATTACKTYPE_DONTTURN = 1 << 6,
    ATTACKTYPE_TARGET = 1 << 7
}ATTACKTYPE;
# 203 "src/bondconstants.h"
typedef enum CHRHIDDEN
{
    CHRHIDDEN_NONE,
    CHRHIDDEN_DROP_HELD_ITEMS = 0x0001,
    CHRHIDDEN_ALERT_GUARD_RELATED = 0x0002,
    CHRHIDDEN_FIRE_WEAPON_LEFT = 0x0004,
    CHRHIDDEN_FIRE_WEAPON_RIGHT = 0x0008,
# 218 "src/bondconstants.h"
    CHRHIDDEN_OFFSCREEN_PATROL = 0x0010,
    CHRHIDDEN_REMOVE = 0x0020,
    CHRHIDDEN_TIMER_ACTIVE = 0x0040,
    CHRHIDDEN_FIRE_TRACER = 0x0080,
    CHRHIDDEN_MOVING = 0x0100,
    CHRHIDDEN_BACKGROUND_AI = 0x0200,
    CHRHIDDEN_0400 = 0x0400,
    CHRHIDDEN_FREEZE = 0x0800,
    CHRHIDDEN_RAND_FLINCH_1 = 0x1000,
    CHRHIDDEN_RAND_FLINCH_2 = 0x2000,
    CHRHIDDEN_RAND_FLINCH_4 = 0x4000,
    CHRHIDDEN_RAND_FLINCH_8 = 0x8000
}CHRHIDDEN;



typedef enum CHRFLAG
{
    CHRFLAG_NONE,
    CHRFLAG_INIT = 0x00000001 ,
    CHRFLAG_CLONE = 0x00000002 ,
    CHRFLAG_NEAR_MISS = 0x00000004 ,
    CHRFLAG_HAS_BEEN_ON_SCREEN = 0x00000008 ,
    CHRFLAG_INVINCIBLE = 0x00000010 ,
    CHRSTART_FORCENOBLOOD = 0x00000020,
    CHRFLAG_CAN_SHOOT_CHRS = 0x00000040 ,
    CHRFLAG_00000080 = 0x00000080 ,
    CHRFLAG_WAS_DAMAGED = 0x00000100 ,
    CHRFLAG_00000200 = 0x00000200 ,
    CHRFLAG_HIDDEN = 0x00000400 ,
    CHRFLAG_NO_AUTOAIM = 0x00000800 ,
    CHRFLAG_LOCK_Y_POS = 0x00001000 ,
    CHRFLAG_NO_SHADOW = 0x00002000 ,
    CHRFLAG_IGNORE_ANIM_TRANSLATION = 0x00004000 ,
    CHRFLAG_IMPACT_ALWAYS = 0x00008000 ,
    CHRFLAG_00010000 = 0x00010000 ,
    CHRFLAG_00020000 = 0x00020000 ,
    CHRFLAG_00040000 = 0x00040000 ,
    CHRFLAG_INCREASE_RUNNING_SPEED = 0x00080000 ,
    CHRFLAG_COUNT_DEATH_AS_CIVILIAN = 0x00100000 ,
    CHRFLAG_WAS_HIT = 0x00200000 ,
    CHRFLAG_00400000 = 0x00400000 ,
    CHRFLAG_CULL_USING_HITBOX = 0x00800000 ,
    CHRFLAG_01000000 = 0x01000000 ,
    CHRFLAG_02000000 = 0x02000000 ,
    CHRFLAG_04000000 = 0x04000000 ,
    CHRFLAG_08000000 = 0x08000000 ,
    CHRFLAG_10000000 = 0x10000000 ,
    CHRFLAG_20000000 = 0x20000000 ,
    CHRFLAG_40000000 = 0x40000000 ,
    CHRFLAG_80000000 = 0x80000000
}CHRFLAG;


typedef enum CDTYPE
{
    CDTYPE_OBJS = 0x0001,
    CDTYPE_DOORS = 0x0002,
    CDTYPE_PLAYERS = 0x0004,
    CDTYPE_CHRS = 0x0008,
    CDTYPE_PATHBLOCKER = 0x0010,
    CDTYPE_BG = 0x0020,
    CDTYPE_OBJSIMMUNETOGUNFIRE = 0x0080,
    CDTYPE_AIOPAQUE = 0x0100,
    CDTYPE_OBJSIMMUNETOEXPLOSIONS = 0x0200,
    CDTYPE_OBJSNOTSAFEORHELI = 0x0400,
    CDTYPE_DOORSWITHOUTFLAG = 0x0800,
    CDTYPE_CLOSEDDOORS = 0x1000,
    CDTYPE_OPENDOORS = 0x2000,
    CDTYPE_AJARDOORS = 0x4000,
    CDTYPE_DOORSLOCKEDTOAI = 0x8000,
    CDTYPE_ALL = 0x003f
} CDTYPE;

typedef enum GUARD_SETUP_FLAG
{
    GUARD_SETUP_FLAG_NONE = 0,
    GUARD_SETUP_FLAG_SUNGLASSES = 0x0001,
    GUARD_SETUP_FLAG_SUNGLASSES_50 = 0x0002,
    GUARD_SETUP_FLAG_CHR_CLONE = 0x0004,
    GUARD_SETUP_FLAG_CHR_INVINCIBLE = 0x0008,
    GUARD_SETUP_FLAG_0010 = 0x0010,
    GUARD_SETUP_FLAG_0020 = 0x0020,
    GUARD_SETUP_FLAG_0040 = 0x0040,
    GUARD_SETUP_FLAG_0080 = 0x0080,
    GUARD_SETUP_FLAG_0100 = 0x0100,
    GUARD_SETUP_FLAG_0200 = 0x0200,
    GUARD_SETUP_FLAG_0400 = 0x0400,
    GUARD_SETUP_FLAG_0800 = 0x0800,
    GUARD_SETUP_FLAG_1000 = 0x1000,
    GUARD_SETUP_FLAG_2000 = 0x2000,
    GUARD_SETUP_FLAG_4000 = 0x4000,
    GUARD_SETUP_FLAG_8000 = 0x8000
} GUARD_SETUP_FLAG;


typedef enum FLAGS2 { FLAGS2_NONE, FLAGS2_DONT_POINT_AT_BOND = 1 << 0, FLAGS2_02 = 1 << 0 +1, FLAGS2_04 = 1 << 0 +1 +1 } FLAGS2;






typedef enum PS_FLAGS2 { PS_FLAGS2_NONE, PS_FLAGS2_NO_DISTANCEQ = 1 << 0 } PS_FLAGS2;




typedef enum PROPFLAG
{
    PROPFLAG_RENDERPOSTBG = 0x00000001,
    PROPFLAG_ONSCREEN = 0x00000002,
    PROPFLAG_ENABLED = 0x00000004,
    PROPFLAG_00000008 = 0x00000008,
    PROPFLAG_00000010 = 0x00000010,
    PROPFLAG_00000020 = 0x00000020,
    PROPFLAG_00000040 = 0x00000040,
    PROPFLAG_00000080 = 0x00000080,
    PROPFLAG_00000100 = 0x00000100,
    PROPFLAG_00000200 = 0x00000200,
    PROPFLAG_ILLUMINATED = 0x00000400,
    PROPFLAG_00000800 = 0x00000800,
    PROPFLAG_00001000 = 0x00001000,
    PROPFLAG_AIUNDROPPABLE = 0x00002000,
    PROPFLAG_ASSIGNEDTOCHR = 0x00004000,
    PROPFLAG_INSIDEANOTHEROBJ = 0x00008000,
    PROPFLAG_FORCEMORTAL = 0x00010000,
    PROPFLAG_INVINCIBLE = 0x00020000,
    PROPFLAG_00040000 = 0x00040000,
    PROPFLAG_00080000 = 0x00080000,
    PROPFLAG_UNCOLLECTABLE = 0x00100000,
    PROPFLAG_00200000 = 0x00200000,
    PROPFLAG_00400000 = 0x00400000,
    PROPFLAG_00800000 = 0x00800000,
    PROPFLAG_01000000 = 0x01000000,
    PROPFLAG_CANNOT_ACTIVATE = 0x02000000,
    PROPFLAG_04000000 = 0x04000000,
    PROPFLAG_DOOR_TWOWAY = 0x08000000,
    PROPFLAG_WEAPON_LEFTHANDED = 0x10000000,
    PROPFLAG_GLASS_HASPORTAL = 0x10000000,
    PROPFLAG_CULL_BEHIND_DOOR = 0x10000000,
    PROPFLAG_FIXED_MONITOR = 0x10000000,
    PROPFLAG_CCTV_DISABLED = 0x10000000,
    PROPFLAG_IS_DRONE_GUN = 0x10000000,
    PROPFLAG_DOOR_OPENTOFRONT = 0x20000000,
    PROPFLAG_SPECIAL_FUNC = 0x20000000,
    PROPFLAG_CONCEAL_GUN = 0x20000000,
    PROPFLAG_MONITOR_RENDERPOSTBG = 0x40000000,
    PROPFLAG_NO_PORTAL_CLOSE = 0x40000000,
    PROPFLAG_NO_AMMO = 0x40000000,
    PROPFLAG_80000000 = 0x80000000,
    PROPFLAG_IS_DOUBLE = 0x80000000
}PROPFLAG;

typedef enum PROPFLAG2
{
    PROPFLAG2_00000001 = 0x00000001,
    PROPFLAG2_00000002 = 0x00000002,
    PROPFLAG2_00000004 = 0x00000004,
    PROPFLAG2_00000008 = 0x00000008,
    PROPFLAG2_00000010 = 0x00000010,
    PROPFLAG2_00000020 = 0x00000020,
    PROPFLAG2_00000040 = 0x00000040,
    PROPFLAG2_00000080 = 0x00000080,
    PROPFLAG2_NOFALL = 0x00000100,
    PROPFLAG2_FALLWITHOUTROTATION = 0x00000200,
    PROPFLAG2_LINKEDTOSAFE = 0x00000400,
    PROPFLAG2_INTERACTCHECKLOS = 0x00000800,
    PROPFLAG2_00001000 = 0x00001000,
    PROPFLAG2_00002000 = 0x00002000,
    PROPFLAG2_00004000 = 0x00004000,
    PROPFLAG2_SHOOTTHROUGH = 0x00008000,
    PROPFLAG2_00010000 = 0x00010000,
    PROPFLAG2_00020000 = 0x00020000,
    PROPFLAG2_00040000 = 0x00040000,
    PROPFLAG2_00080000 = 0x00080000,
    PROPFLAG2_00100000 = 0x00100000,
    PROPFLAG2_00200000 = 0x00200000,
    PROPFLAG2_00400000 = 0x00400000,
    PROPFLAG2_00800000 = 0x00800000,
    PROPFLAG2_01000000 = 0x01000000,
    PROPFLAG2_THROWTHROUGH = 0x02000000,
    PROPFLAG2_04000000 = 0x04000000,
    PROPFLAG2_08000000 = 0x08000000,
    PROPFLAG2_10000000 = 0x10000000,
    PROPFLAG2_20000000 = 0x20000000,
    PROPFLAG2_40000000 = 0x40000000,
    PROPFLAG2_DOOR_ALTCOORDSYSTEM = 0x80000000
} PROPFLAG2;

typedef enum DOORFLAG
{
    DOORFLAG_0001 = 0x00000001,
    DOORFLAG_WINDOWED = 0x00000002,
    DOORFLAG_0004 = 0x00000004,
    DOORFLAG_FLIP = 0x00000008,
    DOORFLAG_AUTOMATIC = 0x00000010,
    DOORFLAG_0020 = 0x00000020,
    DOORFLAG_ROTATEDPAD = 0x00000040,
    DOORFLAG_080 = 0x00000080,
    DOORFLAG_100 = 0x00000100,
    DOORFLAG_LONGRANGE = 0x00000200,
    DOORFLAG_DAMAGEONCONTACT = 0x00000400,
    DOORFLAG_UNBLOCKABLEOPEN = 0x00000800,
    DOORFLAG_4000 = 0x00004000,
    DOORFLAG_CANNOT_ACTIVATE = 0x02000000,
    DOORFLAG_KEEPOPEN = 0x80000000

} DOORFLAG;

typedef enum DOORMODE
{
    DOORMODE_IDLE,
    DOORMODE_OPENING,
    DOORMODE_CLOSING,

    DOORMODE_WAITING
} DOORMODE;

typedef enum DOORSTATE
{
    DOORSTATE_STATIONARY,
    DOORSTATE_OPENING,
    DOORSTATE_CLOSING,
    DOORSTATE_WAITING
} DOORSTATE;
# 464 "src/bondconstants.h"
typedef enum DOOR_OPEN_SOUND
{
    DOOR_OPEN_SOUND_NONE = 0,
    DOOR_OPEN_SOUND_01,
    DOOR_OPEN_SOUND_02,
    DOOR_OPEN_SOUND_METAL,
    DOOR_OPEN_SOUND_04,
    DOOR_OPEN_SOUND_WOOD,
    DOOR_OPEN_SOUND_06,
    DOOR_OPEN_SOUND_WOOD_2,
    DOOR_OPEN_SOUND_WOOD_3,
    DOOR_OPEN_SOUND_09,
    DOOR_OPEN_SOUND_METAL_2,
    DOOR_OPEN_SOUND_11,
    DOOR_OPEN_SOUND_METAL_3,
    DOOR_OPEN_SOUND_13,
    DOOR_OPEN_SOUND_HYDROLIC,
    DOOR_OPEN_SOUND_STONE,
    DOOR_OPEN_SOUND_16,
    DOOR_OPEN_SOUND_METAL_4,
    DOOR_OPEN_SOUND_18
} DOOR_OPEN_SOUND;

typedef enum DROPTYPE
{
    DROPTYPE_DEFAULT = 1,
    DROPTYPE_SURRENDER = 2,
    DROPTYPE_THROWGRENADE = 3,
    DROPTYPE_HAT = 4
} DROPTYPE;

typedef enum PROJECTILEFLAG
{
    PROJECTILEFLAG_AIRBORNE = 0x00000001,
    PROJECTILEFLAG_00000002 = 0x00000002,
    PROJECTILEFLAG_STICKY = 0x00000004,
    PROJECTILEFLAG_POWERED = 0x00000010,
    PROJECTILEFLAG_00000020 = 0x00000020,
    PROJECTILEFLAG_LAUNCHING = 0x00000080,
    PROJECTILEFLAG_00000100 = 0x00000100,
    PROJECTILEFLAG_FALLING = 0x00000400,
    PROJECTILEFLAG_SLIDING = 0x00000800,
    PROJECTILEFLAG_00001000 = 0x00001000,
    PROJECTILEFLAG_00002000 = 0x00002000,
    PROJECTILEFLAG_NOTIMELIMIT = 0x00004000,
    PROJECTILEFLAG_INROOM = 0x00008000,
    PROJECTILEFLAG_00010000 = 0x00010000,
    PROJECTILEFLAG_LIGHTWEIGHT = 0x40000000,
    PROJECTILEFLAG_FREE = 0x80000000
} PROJECTILE_FLAG;





typedef enum DOOR_LOCK { DOOR_LOCK_NONE, DOOR_LOCK_0 = 1 << 0, DOOR_LOCK_1 = 1 << 0 +1, DOOR_LOCK_2 = 1 << 0 +1 +1, DOOR_LOCK_3 = 1 << 0 +1 +1 +1, DOOR_LOCK_4 = 1 << 0 +1 +1 +1 +1, DOOR_LOCK_5 = 1 << 0 +1 +1 +1 +1 +1, DOOR_LOCK_6 = 1 << 0 +1 +1 +1 +1 +1 +1, DOOR_LOCK_7 = 1 << 0 +1 +1 +1 +1 +1 +1 +1 } DOOR_LOCK;
# 531 "src/bondconstants.h"
typedef enum PROPSTATE {
    PROPSTATE_NONE = 0x00,
    PROPSTATE_DAMAGED = 0x01,
    PROPSTATE_2 = 0x02,
    PROPSTATE_RESPAWN = 0x04,
    PROPSTATE_EXT_COLISION_BLOCK = 0x08,
    PROPSTATE_10 = 0x10,
    PROPSTATE_20 = 0x20,
    PROPSTATE_ACTIVATED = 0x40,
    PROPSTATE_DESTROYED = 0x80
} PROPSTATE;



typedef enum PLAYERFLAG { PLAYERFLAG_NONE, PLAYERFLAG_LOCKCONTROLS = 1 << 0, PLAYERFLAG_NOCONTROL = 1 << 0 +1, PLAYERFLAG_NOTIMER = 1 << 0 +1 +1 } PLAYERFLAG;





typedef enum RUNTIMEBITFLAG { RUNTIMEBITFLAG_NONE, RUNTIMEBITFLAG_00000001 = 1 << 0, RUNTIMEBITFLAG_00000002 = 1 << 0 +1, RUNTIMEBITFLAG_REMOVE = 1 << 0 +1 +1, RUNTIMEBITFLAG_ISRETICK = 1 << 0 +1 +1 +1, RUNTIMEBITFLAG_TAGGED = 1 << 0 +1 +1 +1 +1, RUNTIMEBITFLAG_THROWING_KNIFE_RELATED = 1 << 0 +1 +1 +1 +1 +1, RUNTIMEBITFLAG_EMBEDDED = 1 << 0 +1 +1 +1 +1 +1 +1, RUNTIMEBITFLAG_DEPOSIT = 1 << 0 +1 +1 +1 +1 +1 +1 +1, RUNTIMEBITFLAG_00000100 = 1 << 0 +1 +1 +1 +1 +1 +1 +1 +1, RUNTIMEBITFLAG_BEENOPENED = 1 << 0 +1 +1 +1 +1 +1 +1 +1 +1 +1, RUNTIMEBITFLAG_DESTROYED = 1 << 0 +1 +1 +1 +1 +1 +1 +1 +1 +1 +1, RUNTIMEBITFLAG_00000800 = 1 << 0 +1 +1 +1 +1 +1 +1 +1 +1 +1 +1 +1, RUNTIMEBITFLAG_00001000 = 1 << 0 +1 +1 +1 +1 +1 +1 +1 +1 +1 +1 +1 +1, RUNTIMEBITFLAG_PADLOCKEDDOOR = 1 << 0 +1 +1 +1 +1 +1 +1 +1 +1 +1 +1 +1 +1 +1, RUNTIMEBITFLAG_ACTIVATED = 1 << 0 +1 +1 +1 +1 +1 +1 +1 +1 +1 +1 +1 +1 +1 +1, RUNTIMEBITFLAG_00008000 = 1 << 0 +1 +1 +1 +1 +1 +1 +1 +1 +1 +1 +1 +1 +1 +1 +1, RUNTIMEBITFLAG_00010000 = 1 << 0 +1 +1 +1 +1 +1 +1 +1 +1 +1 +1 +1 +1 +1 +1 +1 +1, RUNTIMEBITFLAG_00020000 = 1 << 0 +1 +1 +1 +1 +1 +1 +1 +1 +1 +1 +1 +1 +1 +1 +1 +1 +1, RUNTIMEBITFLAG_00040000 = 1 << 0 +1 +1 +1 +1 +1 +1 +1 +1 +1 +1 +1 +1 +1 +1 +1 +1 +1 +1, RUNTIMEBITFLAG_HASOWNER = 1 << 0 +1 +1 +1 +1 +1 +1 +1 +1 +1 +1 +1 +1 +1 +1 +1 +1 +1 +1 +1, RUNTIMEBITFLAG_00100000 = 1 << 0 +1 +1 +1 +1 +1 +1 +1 +1 +1 +1 +1 +1 +1 +1 +1 +1 +1 +1 +1 +1, RUNTIMEBITFLAG_00200000 = 1 << 0 +1 +1 +1 +1 +1 +1 +1 +1 +1 +1 +1 +1 +1 +1 +1 +1 +1 +1 +1 +1 +1, RUNTIMEBITFLAG_00400000 = 1 << 0 +1 +1 +1 +1 +1 +1 +1 +1 +1 +1 +1 +1 +1 +1 +1 +1 +1 +1 +1 +1 +1 +1, RUNTIMEBITFLAG_00800000 = 1 << 0 +1 +1 +1 +1 +1 +1 +1 +1 +1 +1 +1 +1 +1 +1 +1 +1 +1 +1 +1 +1 +1 +1 +1, RUNTIMEBITFLAG_01000000 = 1 << 0 +1 +1 +1 +1 +1 +1 +1 +1 +1 +1 +1 +1 +1 +1 +1 +1 +1 +1 +1 +1 +1 +1 +1 +1, RUNTIMEBITFLAG_02000000 = 1 << 0 +1 +1 +1 +1 +1 +1 +1 +1 +1 +1 +1 +1 +1 +1 +1 +1 +1 +1 +1 +1 +1 +1 +1 +1 +1, RUNTIMEBITFLAG_04000000 = 1 << 0 +1 +1 +1 +1 +1 +1 +1 +1 +1 +1 +1 +1 +1 +1 +1 +1 +1 +1 +1 +1 +1 +1 +1 +1 +1 +1, RUNTIMEBITFLAG_08000000 = 1 << 0 +1 +1 +1 +1 +1 +1 +1 +1 +1 +1 +1 +1 +1 +1 +1 +1 +1 +1 +1 +1 +1 +1 +1 +1 +1 +1 +1, RUNTIMEBITFLAG_10000000 = 1 << 0 +1 +1 +1 +1 +1 +1 +1 +1 +1 +1 +1 +1 +1 +1 +1 +1 +1 +1 +1 +1 +1 +1 +1 +1 +1 +1 +1 +1, RUNTIMEBITFLAG_20000000 = 1 << 0 +1 +1 +1 +1 +1 +1 +1 +1 +1 +1 +1 +1 +1 +1 +1 +1 +1 +1 +1 +1 +1 +1 +1 +1 +1 +1 +1 +1 +1, RUNTIMEBITFLAG_40000000 = 1 << 0 +1 +1 +1 +1 +1 +1 +1 +1 +1 +1 +1 +1 +1 +1 +1 +1 +1 +1 +1 +1 +1 +1 +1 +1 +1 +1 +1 +1 +1 +1, RUNTIMEBITFLAG_80000000 = 1 << 0 +1 +1 +1 +1 +1 +1 +1 +1 +1 +1 +1 +1 +1 +1 +1 +1 +1 +1 +1 +1 +1 +1 +1 +1 +1 +1 +1 +1 +1 +1 +1 } RUNTIMEBITFLAG;
# 590 "src/bondconstants.h"
typedef enum WEAPONSTATBITFLAG { WEAPONSTATBITFLAG_NONE, WEAPONSTATBITFLAG_00000001 = 1 << 0, WEAPONSTATBITFLAG_SINGLE_USE_RELOAD = 1 << 0 +1, WEAPONSTATBITFLAG_BURST_FIRE = 1 << 0 +1 +1, WEAPONSTATBITFLAG_HAS_AUTO_AIM = 1 << 0 +1 +1 +1, WEAPONSTATBITFLAG_CLICKY = 1 << 0 +1 +1 +1 +1, WEAPONSTATBITFLAG_00000020 = 1 << 0 +1 +1 +1 +1 +1, WEAPONSTATBITFLAG_00000040 = 1 << 0 +1 +1 +1 +1 +1 +1, WEAPONSTATBITFLAG_00000080 = 1 << 0 +1 +1 +1 +1 +1 +1 +1, WEAPONSTATBITFLAG_ONLY_1_HANDED = 1 << 0 +1 +1 +1 +1 +1 +1 +1 +1, WEAPONSTATBITFLAG_HOLD_AS_GUN = 1 << 0 +1 +1 +1 +1 +1 +1 +1 +1 +1, WEAPONSTATBITFLAG_MIRROR_DUAL = 1 << 0 +1 +1 +1 +1 +1 +1 +1 +1 +1 +1, WEAPONSTATBITFLAG_SHOW_FIRST_PERSON = 1 << 0 +1 +1 +1 +1 +1 +1 +1 +1 +1 +1 +1, WEAPONSTATBITFLAG_FIRST_SHOT_ACCURACY = 1 << 0 +1 +1 +1 +1 +1 +1 +1 +1 +1 +1 +1 +1, WEAPONSTATBITFLAG_HIDE_FIRST_PERSON_HAND = 1 << 0 +1 +1 +1 +1 +1 +1 +1 +1 +1 +1 +1 +1 +1, WEAPONSTATBITFLAG_HIDE_FIRST_PERSON_MENU = 1 << 0 +1 +1 +1 +1 +1 +1 +1 +1 +1 +1 +1 +1 +1 +1, WEAPONSTATBITFLAG_DISABLE_CROUCH = 1 << 0 +1 +1 +1 +1 +1 +1 +1 +1 +1 +1 +1 +1 +1 +1 +1, WEAPONSTATBITFLAG_PLAYER_STAT_HIT = 1 << 0 +1 +1 +1 +1 +1 +1 +1 +1 +1 +1 +1 +1 +1 +1 +1 +1, WEAPONSTATBITFLAG_USE_HOLD_TIME = 1 << 0 +1 +1 +1 +1 +1 +1 +1 +1 +1 +1 +1 +1 +1 +1 +1 +1 +1, WEAPONSTATBITFLAG_HAS_AMMO = 1 << 0 +1 +1 +1 +1 +1 +1 +1 +1 +1 +1 +1 +1 +1 +1 +1 +1 +1 +1, WEAPONSTATBITFLAG_HIDE_AMMO_DISPLAY = 1 << 0 +1 +1 +1 +1 +1 +1 +1 +1 +1 +1 +1 +1 +1 +1 +1 +1 +1 +1 +1, WEAPONSTATBITFLAG_CAN_DUAL_WIELD = 1 << 0 +1 +1 +1 +1 +1 +1 +1 +1 +1 +1 +1 +1 +1 +1 +1 +1 +1 +1 +1 +1, WEAPONSTATBITFLAG_AMMO_CLIP_LIMIT = 1 << 0 +1 +1 +1 +1 +1 +1 +1 +1 +1 +1 +1 +1 +1 +1 +1 +1 +1 +1 +1 +1 +1, WEAPONSTATBITFLAG_NO_CLIP_RELOADS = 1 << 0 +1 +1 +1 +1 +1 +1 +1 +1 +1 +1 +1 +1 +1 +1 +1 +1 +1 +1 +1 +1 +1 +1, WEAPONSTATBITFLAG_00800000 = 1 << 0 +1 +1 +1 +1 +1 +1 +1 +1 +1 +1 +1 +1 +1 +1 +1 +1 +1 +1 +1 +1 +1 +1 +1, WEAPONSTATBITFLAG_01000000 = 1 << 0 +1 +1 +1 +1 +1 +1 +1 +1 +1 +1 +1 +1 +1 +1 +1 +1 +1 +1 +1 +1 +1 +1 +1 +1, WEAPONSTATBITFLAG_02000000 = 1 << 0 +1 +1 +1 +1 +1 +1 +1 +1 +1 +1 +1 +1 +1 +1 +1 +1 +1 +1 +1 +1 +1 +1 +1 +1 +1, WEAPONSTATBITFLAG_04000000 = 1 << 0 +1 +1 +1 +1 +1 +1 +1 +1 +1 +1 +1 +1 +1 +1 +1 +1 +1 +1 +1 +1 +1 +1 +1 +1 +1 +1, WEAPONSTATBITFLAG_08000000 = 1 << 0 +1 +1 +1 +1 +1 +1 +1 +1 +1 +1 +1 +1 +1 +1 +1 +1 +1 +1 +1 +1 +1 +1 +1 +1 +1 +1 +1, WEAPONSTATBITFLAG_10000000 = 1 << 0 +1 +1 +1 +1 +1 +1 +1 +1 +1 +1 +1 +1 +1 +1 +1 +1 +1 +1 +1 +1 +1 +1 +1 +1 +1 +1 +1 +1, WEAPONSTATBITFLAG_20000000 = 1 << 0 +1 +1 +1 +1 +1 +1 +1 +1 +1 +1 +1 +1 +1 +1 +1 +1 +1 +1 +1 +1 +1 +1 +1 +1 +1 +1 +1 +1 +1, WEAPONSTATBITFLAG_40000000 = 1 << 0 +1 +1 +1 +1 +1 +1 +1 +1 +1 +1 +1 +1 +1 +1 +1 +1 +1 +1 +1 +1 +1 +1 +1 +1 +1 +1 +1 +1 +1 +1, WEAPONSTATBITFLAG_80000000 = 1 << 0 +1 +1 +1 +1 +1 +1 +1 +1 +1 +1 +1 +1 +1 +1 +1 +1 +1 +1 +1 +1 +1 +1 +1 +1 +1 +1 +1 +1 +1 +1 +1 } WEAPONSTATBITFLAG;
# 676 "src/bondconstants.h"
#pragma endregion

#pragma region Constants
# 712 "src/bondconstants.h"
typedef enum AI_CMD
{
# 730 "src/bondconstants.h"
# 1 "src/aicommands.def" 1
# 164 "src/aicommands.def"
AI_GotoNext,







# 204 "src/aicommands.def"
AI_GotoFirst,
# 222 "src/aicommands.def"

# 237 "src/aicommands.def"

# 269 "src/aicommands.def"
AI_Label,
# 287 "src/aicommands.def"

# 309 "src/aicommands.def"

# 343 "src/aicommands.def"
AI_Yield,
# 390 "src/aicommands.def"
AI_EndList,
# 419 "src/aicommands.def"
AI_SetChrAiList,
# 457 "src/aicommands.def"

# 472 "src/aicommands.def"

# 487 "src/aicommands.def"

# 502 "src/aicommands.def"

# 516 "src/aicommands.def"

# 532 "src/aicommands.def"

# 567 "src/aicommands.def"
AI_SetReturnAiList,
# 616 "src/aicommands.def"
AI_Return,
# 660 "src/aicommands.def"
AI_Stop,
# 692 "src/aicommands.def"
AI_Kneel,
# 733 "src/aicommands.def"
AI_PlayAnimation,
# 773 "src/aicommands.def"

# 805 "src/aicommands.def"
AI_IFPlayingAnimation,
# 848 "src/aicommands.def"
AI_PointAtBond,
# 880 "src/aicommands.def"
AI_LookSurprised,
# 915 "src/aicommands.def"
AI_TRYSidestepping,
# 957 "src/aicommands.def"
AI_TRYSideHopping,
# 999 "src/aicommands.def"
AI_TRYSideRunning,
# 1041 "src/aicommands.def"
AI_TRYFiringWalk,
# 1083 "src/aicommands.def"
AI_TRYFiringRun,
# 1125 "src/aicommands.def"
AI_TRYFiringRoll,
# 1170 "src/aicommands.def"
AI_TRYFireOrAimAtTarget,
# 1197 "src/aicommands.def"













# 1242 "src/aicommands.def"
AI_TRYFireOrAimAtTargetKneel,
# 1269 "src/aicommands.def"













# 1315 "src/aicommands.def"
AI_TRYFireOrAimAtTargetUpdate,
# 1342 "src/aicommands.def"













# 1390 "src/aicommands.def"
AI_TRYFacingTarget,
# 1408 "src/aicommands.def"





# 1445 "src/aicommands.def"
AI_HitChrWithItem,
# 1460 "src/aicommands.def"

# 1493 "src/aicommands.def"
AI_ChrHitChr,
# 1526 "src/aicommands.def"





# 1565 "src/aicommands.def"
AI_TRYThrowingGrenade,
# 1610 "src/aicommands.def"
AI_TRYDroppingItem,
# 1652 "src/aicommands.def"
AI_RunToPad,
# 1695 "src/aicommands.def"
AI_RunToPadPreset,
# 1729 "src/aicommands.def"
AI_WalkToPad,
# 1765 "src/aicommands.def"
AI_SprintToPad,
# 1802 "src/aicommands.def"
AI_StartPatrol,
# 1838 "src/aicommands.def"
AI_Surrender,
# 1874 "src/aicommands.def"
AI_RemoveMe,
# 1909 "src/aicommands.def"
AI_ChrRemoveInstant,
# 1931 "src/aicommands.def"

# 1966 "src/aicommands.def"
AI_TRYTriggeringAlarmAtPad,
# 2006 "src/aicommands.def"
AI_AlarmOn,
# 2036 "src/aicommands.def"
AI_AlarmOff,
# 2068 "src/aicommands.def"
AI_TRYRunFromBond,
# 2109 "src/aicommands.def"
AI_TRYRunToBond,
# 2152 "src/aicommands.def"
AI_TRYWalkToBond,
# 2195 "src/aicommands.def"
AI_TRYSprintToBond,
# 2236 "src/aicommands.def"
AI_TRYFindCover,
# 2280 "src/aicommands.def"
AI_TRYRunToChr,
# 2296 "src/aicommands.def"

# 2329 "src/aicommands.def"
AI_TRYWalkToChr,
# 2345 "src/aicommands.def"

# 2378 "src/aicommands.def"
AI_TRYSprintToChr,
# 2394 "src/aicommands.def"

# 2427 "src/aicommands.def"
AI_IFImOnPatrolOrStopped,
# 2469 "src/aicommands.def"
AI_IFChrDyingOrDead,
# 2486 "src/aicommands.def"

# 2519 "src/aicommands.def"
AI_IFChrDoesNotExist,
# 2536 "src/aicommands.def"






# 2581 "src/aicommands.def"
AI_IFISeeBond,
# 2622 "src/aicommands.def"
AI_SetNewRandom,
# 2657 "src/aicommands.def"
AI_IFRandomLessThan,
# 2673 "src/aicommands.def"

# 2706 "src/aicommands.def"
AI_IFRandomGreaterThan,
# 2722 "src/aicommands.def"

# 2757 "src/aicommands.def"
AI_IFICanHearAlarm,
# 2798 "src/aicommands.def"
AI_IFAlarmIsOn,
# 2840 "src/aicommands.def"
AI_IFGasIsLeaking,
# 2884 "src/aicommands.def"
AI_IFIHeardBond,
# 2928 "src/aicommands.def"
AI_IFISeeSomeoneShot,
# 2971 "src/aicommands.def"
AI_IFISeeSomeoneDie,
# 3017 "src/aicommands.def"
AI_IFICouldSeeBond,
# 3060 "src/aicommands.def"
AI_IFICouldSeeBondsStan,
# 3108 "src/aicommands.def"
AI_IFIWasShotRecently,
# 3152 "src/aicommands.def"
AI_IFIHeardBondRecently,
# 3194 "src/aicommands.def"
AI_IFImInRoomWithChr,
# 3238 "src/aicommands.def"
AI_IFIveNotBeenSeen,
# 3280 "src/aicommands.def"
AI_IFImOnScreen,
# 3324 "src/aicommands.def"
AI_IFMyRoomIsOnScreen,
# 3367 "src/aicommands.def"
AI_IFRoomWithPadIsOnScreen,
# 3410 "src/aicommands.def"
AI_IFImTargetedByBond,
# 3454 "src/aicommands.def"
AI_IFBondMissedMe,
# 3499 "src/aicommands.def"
AI_IFMyAngleToBondLessThan,
# 3530 "src/aicommands.def"

# 3547 "src/aicommands.def"

# 3579 "src/aicommands.def"
AI_IFMyAngleToBondGreaterThan,
# 3610 "src/aicommands.def"

# 3627 "src/aicommands.def"

# 3657 "src/aicommands.def"
AI_IFMyAngleFromBondLessThan,
# 3688 "src/aicommands.def"

# 3705 "src/aicommands.def"

# 3735 "src/aicommands.def"
AI_IFMyAngleFromBondGreaterThan,
# 3766 "src/aicommands.def"

# 3783 "src/aicommands.def"

# 3813 "src/aicommands.def"
AI_IFMyDistanceToBondLessThanDecimeter,
# 3838 "src/aicommands.def"

# 3869 "src/aicommands.def"
AI_IFMyDistanceToBondGreaterThanDecimeter,
# 3894 "src/aicommands.def"

# 3926 "src/aicommands.def"
AI_IFChrDistanceToPadLessThanDecimeter,
# 3945 "src/aicommands.def"

# 3960 "src/aicommands.def"





# 3996 "src/aicommands.def"
AI_IFChrDistanceToPadGreaterThanDecimeter,
# 4015 "src/aicommands.def"

# 4029 "src/aicommands.def"





# 4065 "src/aicommands.def"
AI_IFMyDistanceToChrLessThanDecimeter,
# 4091 "src/aicommands.def"

# 4123 "src/aicommands.def"
AI_IFMyDistanceToChrGreaterThanDecimeter,
# 4149 "src/aicommands.def"

# 4182 "src/aicommands.def"
AI_TRYSettingMyPresetToChrWithinDistanceDecimeter,
# 4209 "src/aicommands.def"

# 4240 "src/aicommands.def"
AI_IFBondDistanceToPadLessThanDecimeter,
# 4266 "src/aicommands.def"

# 4297 "src/aicommands.def"
AI_IFBondDistanceToPadGreaterThanDecimeter,
# 4323 "src/aicommands.def"

# 4354 "src/aicommands.def"
AI_IFChrInRoomWithPad,
# 4378 "src/aicommands.def"

# 4408 "src/aicommands.def"
AI_IFBondInRoomWithPad,
# 4450 "src/aicommands.def"
AI_IFBondCollectedObject,
# 4495 "src/aicommands.def"
AI_IFKeyDropped,
# 4540 "src/aicommands.def"
AI_IFItemIsAttachedToObject,
# 4606 "src/aicommands.def"
AI_IFBondHasItemEquipped,
# 4647 "src/aicommands.def"
AI_IFObjectExists,
# 4689 "src/aicommands.def"
AI_IFObjectNotDestroyed,
# 4734 "src/aicommands.def"
AI_IFObjectWasActivated,
# 4783 "src/aicommands.def"
AI_IFBondUsedGadgetOnObject,
# 4826 "src/aicommands.def"
AI_ActivateObject,
# 4873 "src/aicommands.def"
AI_DestroyObject,
# 4916 "src/aicommands.def"
AI_DropObject,
# 4959 "src/aicommands.def"
AI_ChrDropAllConcealedItems,
# 4973 "src/aicommands.def"

# 5006 "src/aicommands.def"
AI_ChrDropAllHeldItems,
# 5029 "src/aicommands.def"

# 5059 "src/aicommands.def"
AI_BondCollectObject,
# 5102 "src/aicommands.def"
AI_ChrEquipObject,
# 5155 "src/aicommands.def"
AI_MoveObject,
# 5207 "src/aicommands.def"
AI_DoorOpen,
# 5242 "src/aicommands.def"
AI_DoorClose,
# 5281 "src/aicommands.def"
AI_IFDoorStateEqual,
# 5330 "src/aicommands.def"





















































# 5414 "src/aicommands.def"
AI_IFDoorHasBeenOpenedBefore,
# 5455 "src/aicommands.def"
AI_DoorSetLock,
# 5495 "src/aicommands.def"
AI_DoorUnsetLock,
# 5536 "src/aicommands.def"
AI_IFDoorLockEqual,
# 5590 "src/aicommands.def"
AI_IFObjectiveNumComplete,
# 5641 "src/aicommands.def"
AI_TRYUnknown6e,
# 5687 "src/aicommands.def"
AI_TRYUnknown6f,
# 5730 "src/aicommands.def"
AI_IFGameDifficultyLessThan,
# 5773 "src/aicommands.def"
AI_IFGameDifficultyGreaterThan,
# 5813 "src/aicommands.def"
AI_IFMissionTimeLessThan,
# 5854 "src/aicommands.def"
AI_IFMissionTimeGreaterThan,
# 5895 "src/aicommands.def"
AI_IFSystemPowerTimeLessThan,
# 5936 "src/aicommands.def"
AI_IFSystemPowerTimeGreaterThan,
# 5977 "src/aicommands.def"
AI_IFLevelIdLessThan,
# 6017 "src/aicommands.def"
AI_IFLevelIdGreaterThan,
# 6058 "src/aicommands.def"
AI_IFMyNumArghsLessThan,
# 6099 "src/aicommands.def"
AI_IFMyNumArghsGreaterThan,
# 6139 "src/aicommands.def"
AI_IFMyNumCloseArghsLessThan,
# 6179 "src/aicommands.def"
AI_IFMyNumCloseArghsGreaterThan,
# 6223 "src/aicommands.def"
AI_IFChrHealthLessThan,
# 6241 "src/aicommands.def"









# 6283 "src/aicommands.def"
AI_IFChrHealthGreaterThan,
# 6301 "src/aicommands.def"









# 6343 "src/aicommands.def"
AI_IFChrWasDamagedSinceLastCheck,
# 6361 "src/aicommands.def"

# 6393 "src/aicommands.def"
AI_IFBondHealthLessThan,
# 6435 "src/aicommands.def"
AI_IFBondHealthGreaterThan,
# 6475 "src/aicommands.def"
AI_SetMyMorale,
# 6509 "src/aicommands.def"
AI_AddToMyMorale,
# 6550 "src/aicommands.def"
AI_SubtractFromMyMorale,
# 6590 "src/aicommands.def"
AI_IFMyMoraleLessThan,
# 6629 "src/aicommands.def"
AI_IFMyMoraleLessThanRandom,
# 6668 "src/aicommands.def"
AI_SetMyAlertness,
# 6702 "src/aicommands.def"
AI_AddToMyAlertness,
# 6743 "src/aicommands.def"
AI_SubtractFromMyAlertness,
# 6783 "src/aicommands.def"
AI_IFMyAlertnessLessThan,
# 6822 "src/aicommands.def"
AI_IFMyAlertnessLessThanRandom,
# 6864 "src/aicommands.def"
AI_SetMyHearingScale,
# 6875 "src/aicommands.def"

# 6908 "src/aicommands.def"
AI_SetMyVisionRange,
# 6944 "src/aicommands.def"
AI_SetMyGrenadeProbability,
# 6977 "src/aicommands.def"
AI_SetMyChrNum,
# 7015 "src/aicommands.def"
AI_SetMyHealthTotal,
# 7026 "src/aicommands.def"

# 7064 "src/aicommands.def"
AI_SetMyArmour,
# 7075 "src/aicommands.def"

# 7088 "src/aicommands.def"

# 7125 "src/aicommands.def"
AI_SetMySpeedRating,
# 7168 "src/aicommands.def"
AI_SetMyArghRating,
# 7207 "src/aicommands.def"
AI_SetMyAccuracyRating,
# 7248 "src/aicommands.def"
AI_SetMyFlags2,
# 7258 "src/aicommands.def"

# 7293 "src/aicommands.def"
AI_UnsetMyFlags2,
# 7303 "src/aicommands.def"

# 7338 "src/aicommands.def"
AI_IFMyFlags2Has,
# 7354 "src/aicommands.def"

# 7389 "src/aicommands.def"
AI_SetChrBitfield,
# 7427 "src/aicommands.def"
AI_UnsetChrBitfield,
# 7466 "src/aicommands.def"
AI_IFChrBitfieldHas,
# 7512 "src/aicommands.def"
AI_SetObjectiveBitfield,
# 7553 "src/aicommands.def"
AI_UnsetObjectiveBitfield,
# 7587 "src/aicommands.def"
AI_IFObjectiveBitfieldHas,
# 7631 "src/aicommands.def"
AI_SetMychrflags,
# 7642 "src/aicommands.def"

# 7675 "src/aicommands.def"
AI_UnsetMychrflags,
# 7686 "src/aicommands.def"

# 7721 "src/aicommands.def"
AI_IFMychrflagsHas,
# 7738 "src/aicommands.def"

# 7770 "src/aicommands.def"
AI_SetChrchrflags,
# 7810 "src/aicommands.def"
AI_UnsetChrchrflags,
# 7851 "src/aicommands.def"
AI_IFChrchrflagsHas,
# 7892 "src/aicommands.def"
AI_SetObjectFlags,
# 7930 "src/aicommands.def"
AI_UnsetObjectFlags,
# 7969 "src/aicommands.def"
AI_IFObjectFlagsHas,
# 8010 "src/aicommands.def"
AI_SetObjectFlags2,
# 8048 "src/aicommands.def"
AI_UnsetObjectFlags2,
# 8087 "src/aicommands.def"
AI_IFObjectFlags2Has,
# 8126 "src/aicommands.def"
AI_SetMyChrPreset,
# 8136 "src/aicommands.def"

# 8164 "src/aicommands.def"
AI_SetChrChrPreset,
# 8195 "src/aicommands.def"
AI_SetMyPadPreset,
# 8213 "src/aicommands.def"

# 8241 "src/aicommands.def"
AI_SetChrPadPreset,
# 8276 "src/aicommands.def"
AI_PRINT,
# 8308 "src/aicommands.def"
AI_MyTimerStart,
# 8317 "src/aicommands.def"

# 8348 "src/aicommands.def"
AI_MyTimerReset,
# 8357 "src/aicommands.def"

# 8388 "src/aicommands.def"
AI_MyTimerPause,
# 8397 "src/aicommands.def"

# 8428 "src/aicommands.def"
AI_MyTimerResume,
# 8437 "src/aicommands.def"

# 8468 "src/aicommands.def"
AI_IFMyTimerIsNotRunning,
# 8484 "src/aicommands.def"

# 8516 "src/aicommands.def"
AI_IFMyTimerLessThanTicks,
# 8533 "src/aicommands.def"

# 8546 "src/aicommands.def"





# 8583 "src/aicommands.def"
AI_IFMyTimerGreaterThanTicks,
# 8600 "src/aicommands.def"

# 8613 "src/aicommands.def"





# 8644 "src/aicommands.def"
AI_HudCountdownShow,
# 8676 "src/aicommands.def"
AI_HudCountdownHide,
# 8708 "src/aicommands.def"
AI_HudCountdownSet,
# 8740 "src/aicommands.def"
AI_HudCountdownStop,
# 8770 "src/aicommands.def"
AI_HudCountdownStart,
# 8803 "src/aicommands.def"
AI_IFHudCountdownIsNotRunning,
# 8844 "src/aicommands.def"
AI_IFHudCountdownLessThan,
# 8886 "src/aicommands.def"
AI_IFHudCountdownGreaterThan,
# 8932 "src/aicommands.def"
AI_TRYSpawningChrAtPad,
# 8981 "src/aicommands.def"
AI_TRYSpawningChrNextToChr,
# 9030 "src/aicommands.def"
AI_TRYGiveMeItem,
# 9131 "src/aicommands.def"
AI_TRYGiveMeHat,
# 9180 "src/aicommands.def"
AI_TRYCloningChr,
# 9273 "src/aicommands.def"

# 9304 "src/aicommands.def"
AI_TextPrintBottom,
# 9339 "src/aicommands.def"
AI_TextPrintTop,
# 9392 "src/aicommands.def"
AI_SfxPlay,
# 9403 "src/aicommands.def"

# 9435 "src/aicommands.def"
AI_SfxEmitFromObject,
# 9486 "src/aicommands.def"
AI_SfxEmitFromPad,
# 9547 "src/aicommands.def"
AI_SfxSetChannelVolume,
# 9600 "src/aicommands.def"
AI_SfxFadeChannelVolume,
# 9648 "src/aicommands.def"
AI_SfxStopChannel,
# 9684 "src/aicommands.def"
AI_IFSfxChannelVolumeLessThan,
# 9734 "src/aicommands.def"
AI_VehicleStartPath,
# 9774 "src/aicommands.def"
AI_VehicleSpeed,
# 9790 "src/aicommands.def"









# 9830 "src/aicommands.def"
AI_AircraftRotorSpeed,
# 9846 "src/aicommands.def"

# 9880 "src/aicommands.def"
AI_IFCameraIsInIntro,
# 9920 "src/aicommands.def"
AI_IFCameraIsInBondSwirl,
# 9960 "src/aicommands.def"
AI_TvChangeScreenBank,
# 10010 "src/aicommands.def"
AI_IFBondInTank,
# 10052 "src/aicommands.def"
AI_EndLevel,
# 10085 "src/aicommands.def"









# 10124 "src/aicommands.def"
AI_CameraReturnToBond,
# 10162 "src/aicommands.def"
AI_CameraLookAtBondFromPad,
# 10211 "src/aicommands.def"
AI_CameraSwitch,
# 10261 "src/aicommands.def"
AI_IFBondYPosLessThan,
# 10309 "src/aicommands.def"
AI_BondDisableControl,
# 10357 "src/aicommands.def"
AI_BondEnableControl,
# 10394 "src/aicommands.def"
AI_TRYTeleportingChrToPad,
# 10457 "src/aicommands.def"

# 10487 "src/aicommands.def"
AI_ScreenFadeToBlack,
# 10523 "src/aicommands.def"
AI_ScreenFadeFromBlack,
# 10560 "src/aicommands.def"
AI_IFScreenFadeCompleted,
# 10600 "src/aicommands.def"
AI_HideAllChrs,
# 10637 "src/aicommands.def"
AI_ShowAllChrs,
# 10679 "src/aicommands.def"
AI_DoorOpenInstant,
# 10722 "src/aicommands.def"
AI_ChrRemoveItemInHand,
# 10760 "src/aicommands.def"
AI_IfNumberOfActivePlayersLessThan,
# 10789 "src/aicommands.def"

# 10820 "src/aicommands.def"
AI_IFBondItemTotalAmmoLessThan,
# 10859 "src/aicommands.def"
AI_BondEquipItem,
# 10893 "src/aicommands.def"
AI_BondEquipItemCinema,
# 10930 "src/aicommands.def"
AI_BondSetLockedVelocity,
# 10965 "src/aicommands.def"
AI_IFObjectInRoomWithPad,
# 11015 "src/aicommands.def"
AI_IFImFiringAndLockedForward,
# 11056 "src/aicommands.def"
AI_IFImFiring,
# 11096 "src/aicommands.def"
AI_SwitchSky,
# 11130 "src/aicommands.def"
AI_TriggerFadeAndExitLevelOnButtonPress,
# 11165 "src/aicommands.def"
AI_IFBondIsDead,
# 11206 "src/aicommands.def"
AI_BondDisableDamageAndPickups,
# 11236 "src/aicommands.def"
AI_BondHideWeapons,
# 11282 "src/aicommands.def"
AI_CameraOrbitPad,
# 11335 "src/aicommands.def"
AI_CreditsRoll,
# 11367 "src/aicommands.def"
AI_IFCreditsHasCompleted,
# 11409 "src/aicommands.def"
AI_IFObjectiveAllCompleted,
# 11454 "src/aicommands.def"
AI_IFFolderActorIsEqual,
# 11496 "src/aicommands.def"
AI_IFBondDamageAndPickupsDisabled,
# 11540 "src/aicommands.def"
AI_MusicPlaySlot,
# 11578 "src/aicommands.def"
AI_MusicStopSlot,
# 11611 "src/aicommands.def"
AI_TriggerExplosionsAroundBond,
# 11646 "src/aicommands.def"
AI_IFKilledCiviliansGreaterThan,
# 11688 "src/aicommands.def"
AI_IFChrWasShotSinceLastCheck,
# 11729 "src/aicommands.def"
AI_BondKilledInAction,
# 11759 "src/aicommands.def"
AI_RaiseArms,
# 11795 "src/aicommands.def"
AI_GasLeakAndFadeFog,
# 11817 "src/aicommands.def"

# 11846 "src/aicommands.def"
AI_ObjectRocketLaunch,
# 11872 "src/aicommands.def"

# 731 "src/bondconstants.h" 2


    AI_CMD_COUNT
} AI_CMD;
# 747 "src/bondconstants.h"
typedef enum ANIMATIONS
{
    ANIM_idle,
    ANIM_fire_standing,
    ANIM_fire_standing_fast,
    ANIM_fire_hip,
    ANIM_fire_shoulder_left,
    ANIM_fire_turn_right1,
    ANIM_fire_turn_right2,
    ANIM_fire_kneel_right_leg,
    ANIM_fire_kneel_left_leg,
    ANIM_fire_kneel_left,
    ANIM_fire_kneel_right,
    ANIM_fire_roll_left,
    ANIM_fire_roll_right1,
    ANIM_fire_roll_left_fast,
    ANIM_hit_left_shoulder,
    ANIM_hit_right_shoulder,
    ANIM_hit_left_arm,
    ANIM_hit_right_arm,
    ANIM_hit_left_hand,
    ANIM_hit_right_hand,
    ANIM_hit_left_leg,
    ANIM_hit_right_leg,
    ANIM_death_genitalia,
    ANIM_hit_neck,
    ANIM_death_neck,
    ANIM_death_stagger_back_to_wall,
    ANIM_death_forward_face_down,
    ANIM_death_forward_spin_face_up,
    ANIM_death_backward_fall_face_up1,
    ANIM_death_backward_spin_face_down_right,
    ANIM_death_backward_spin_face_up_right,
    ANIM_death_backward_spin_face_down_left,
    ANIM_death_backward_spin_face_up_left,
    ANIM_death_forward_face_down_hard,
    ANIM_death_forward_face_down_soft,
    ANIM_death_fetal_position_right,
    ANIM_death_fetal_position_left,
    ANIM_death_backward_fall_face_up2,
    ANIM_side_step_left,
    ANIM_fire_roll_right2,
    ANIM_walking,
    ANIM_sprinting,
    ANIM_running,
    ANIM_bond_eye_walk,
    ANIM_bond_eye_fire,
    ANIM_bond_watch,
    ANIM_surrendering_armed,
    ANIM_surrendering_armed_drop_weapon,
    ANIM_fire_walking,
    ANIM_fire_running,
    ANIM_null50,
    ANIM_null51,
    ANIM_fire_jump_to_side_left,
    ANIM_fire_jump_to_side_right,
    ANIM_hit_butt_long,
    ANIM_hit_butt_short,
    ANIM_death_head,
    ANIM_death_left_leg,
    ANIM_slide_right,
    ANIM_slide_left,
    ANIM_jump_backwards,
    ANIM_extending_left_hand,
    ANIM_fire_throw_grenade,
    ANIM_spotting_bond,
    ANIM_look_around,
    ANIM_fire_standing_one_handed_weapon,
    ANIM_fire_standing_draw_one_handed_weapon_fast,
    ANIM_fire_standing_draw_one_handed_weapon_slow,
    ANIM_fire_hip_one_handed_weapon_fast,
    ANIM_fire_hip_one_handed_weapon_slow,
    ANIM_fire_hip_forward_one_handed_weapon,
    ANIM_fire_standing_right_one_handed_weapon,
    ANIM_fire_step_right_one_handed_weapon,
    ANIM_fire_standing_left_one_handed_weapon_slow,
    ANIM_fire_standing_left_one_handed_weapon_fast,
    ANIM_fire_kneel_forward_one_handed_weapon_slow,
    ANIM_fire_kneel_forward_one_handed_weapon_fast,
    ANIM_fire_kneel_right_one_handed_weapon_slow,
    ANIM_fire_kneel_right_one_handed_weapon_fast,
    ANIM_fire_kneel_left_one_handed_weapon_slow,
    ANIM_fire_kneel_left_one_handed_weapon_fast,
    ANIM_fire_kneel_left_one_handed_weapon,
    ANIM_aim_walking_one_handed_weapon,
    ANIM_aim_walking_left_one_handed_weapon,
    ANIM_aim_walking_right_one_handed_weapon,
    ANIM_aim_running_one_handed_weapon,
    ANIM_aim_running_right_one_handed_weapon,
    ANIM_aim_running_left_one_handed_weapon,
    ANIM_aim_sprinting_one_handed_weapon,
    ANIM_running_one_handed_weapon,
    ANIM_sprinting_one_handed_weapon,
    ANIM_null91,
    ANIM_null92,
    ANIM_null93,
    ANIM_null94,
    ANIM_null95,
    ANIM_null96,
    ANIM_draw_one_handed_weapon_and_look_around,
    ANIM_draw_one_handed_weapon_and_stand_up,
    ANIM_aim_one_handed_weapon_left_right,
    ANIM_cock_one_handed_weapon_and_turn_around,
    ANIM_holster_one_handed_weapon_and_cross_arms,
    ANIM_cock_one_handed_weapon_turn_around_and_stand_up,
    ANIM_draw_one_handed_weapon_and_turn_around,
    ANIM_step_foward_and_hold_one_handed_weapon,
    ANIM_holster_one_handed_weapon_and_adjust_suit,
    ANIM_idle_unarmed,
    ANIM_walking_unarmed,
    ANIM_fire_walking_dual_wield,
    ANIM_fire_walking_dual_wield_hands_crossed,
    ANIM_fire_running_dual_wield,
    ANIM_fire_running_dual_wield_hands_crossed,
    ANIM_fire_sprinting_dual_wield,
    ANIM_fire_sprinting_dual_wield_hands_crossed,
    ANIM_walking_female,
    ANIM_running_female,
    ANIM_fire_kneel_dual_wield,
    ANIM_fire_kneel_dual_wield_left,
    ANIM_fire_kneel_dual_wield_right,
    ANIM_fire_kneel_dual_wield_hands_crossed,
    ANIM_fire_kneel_dual_wield_hands_crossed_left,
    ANIM_fire_kneel_dual_wield_hands_crossed_right,
    ANIM_fire_standing_dual_wield,
    ANIM_fire_standing_dual_wield_left,
    ANIM_fire_standing_dual_wield_right,
    ANIM_fire_standing_dual_wield_hands_crossed_left,
    ANIM_fire_standing_dual_wield_hands_crossed_right,
    ANIM_fire_standing_aiming_down_sights,
    ANIM_fire_kneel_aiming_down_sights,
    ANIM_hit_taser,
    ANIM_death_explosion_forward,
    ANIM_death_explosion_left1,
    ANIM_death_explosion_back_left,
    ANIM_death_explosion_back1,
    ANIM_death_explosion_right,
    ANIM_death_explosion_forward_right1,
    ANIM_death_explosion_back2,
    ANIM_death_explosion_forward_roll,
    ANIM_death_explosion_forward_face_down,
    ANIM_death_explosion_left2,
    ANIM_death_explosion_forward_right2,
    ANIM_death_explosion_forward_right2_alt,
    ANIM_death_explosion_forward_right3,
    ANIM_null143,
    ANIM_null144,
    ANIM_null145,
    ANIM_null146,
    ANIM_running_hands_up,
    ANIM_sprinting_hands_up,
    ANIM_aim_and_blow_one_handed_weapon,
    ANIM_aim_one_handed_weapon_left,
    ANIM_aim_one_handed_weapon_right,
    ANIM_conversation,
    ANIM_drop_weapon_and_show_fight_stance,
    ANIM_yawning,
    ANIM_swatting_flies,
    ANIM_scratching_leg,
    ANIM_scratching_butt,
    ANIM_adjusting_crotch,
    ANIM_sneeze,
    ANIM_conversation_cleaned,
    ANIM_conversation_listener,
    ANIM_startled_and_looking_around,
    ANIM_laughing_in_disbelief,
    ANIM_surrendering_unarmed,
    ANIM_coughing_standing,
    ANIM_coughing_kneel1,
    ANIM_coughing_kneel2,
    ANIM_standing_up,
    ANIM_null169,
    ANIM_dancing,
    ANIM_dancing_one_handed_weapon,
    ANIM_keyboard_right_hand1,
    ANIM_keyboard_right_hand2,
    ANIM_keyboard_left_hand,
    ANIM_keyboard_right_hand_tapping,
    ANIM_bond_eye_fire_alt,
    ANIM_dam_jump,
    ANIM_surface_vent_jump,
    ANIM_cradle_jump,
    ANIM_cradle_fall,
    ANIM_credits_bond_kissing,
    ANIM_credits_natalya_kissing,
    ANIM_MAX
} ANIMATIONS;

typedef enum AIRCRAFT_ANIMATION
{
    AIRCRAFT_ANIMATION_helicopter_cradle,
    AIRCRAFT_ANIMATION_plane_runway,
    AIRCRAFT_ANIMATION_helicopter_takeoff,
    AIRCRAFT_ANIMATION_MAX
} AIRCRAFT_ANIMATION;





typedef enum CHEAT_IDS
{

    CHEAT_UNUSED,
    CHEAT_EXTRA_MP_CHARS,
    CHEAT_INVINCIBILITY,
    CHEAT_ALLGUNS,
    CHEAT_MAXAMMO,
    CHEAT_DEBUG_UNK5,
    CHEAT_DEACTIVATE_INVINCIBILITY,
    CHEAT_LINEMODE,
    CHEAT_2X_HEALTH,
    CHEAT_2X_ARMOR,
    CHEAT_INVISIBILITY,
    CHEAT_INFINITE_AMMO,
    CHEAT_DK_MODE,
    CHEAT_EXTRA_WEAPONS,
    CHEAT_TINY_BOND,
    CHEAT_PAINTBALL,
    CHEAT_10X_HEALTH,
    CHEAT_MAGNUM,
    CHEAT_LASER,
    CHEAT_GOLDEN_GUN,
    CHEAT_SILVER_PP7,
    CHEAT_GOLD_PP7,
    CHEAT_BONDPHASE,
    CHEAT_NO_RADAR_MP,
    CHEAT_TURBO_MODE,
    CHEAT_DEBUG_POS,
    CHEAT_FAST_ANIMATION,
    CHEAT_SLOW_ANIMATION,
    CHEAT_ENEMY_ROCKETS,
    CHEAT_2X_ROCKET_LAUNCHER,
    CHEAT_2X_GRENADE_LAUNCHER,
    CHEAT_2X_RCP90,
    CHEAT_2X_THROWING_KNIFE,
    CHEAT_2X_HUNTING_KNIFE,
    CHEAT_2X_LASER,

    CHEAT_UNLOCK_PAINTBALL,
    CHEAT_UNLOCK_INVINCIBLE,
    CHEAT_UNLOCK_DKMODE,
    CHEAT_UNLOCK_2XGL,
    CHEAT_UNLOCK_2XRL,
    CHEAT_UNLOCK_TURBO,
    CHEAT_UNLOCK_NORADAR,
    CHEAT_UNLOCK_TINY,
    CHEAT_UNLOCK_2XTKNIFE,
    CHEAT_UNLOCK_FAST,
    CHEAT_UNLOCK_INVIS,
    CHEAT_UNLOCK_ENEMYRL,
    CHEAT_UNLOCK_SLOW,
    CHEAT_UNLOCK_SILVERPPK,
    CHEAT_UNLOCK_2XHKNIFE,
    CHEAT_UNLOCK_INFAMMO,
    CHEAT_UNLOCK_2XFNP0,
    CHEAT_UNLOCK_GOLDPPK,
    CHEAT_UNLOCK_2XLASER,
    CHEAT_UNLOCK_ALLGUNS,

    CHEAT_UNLOCK_DAM,
    CHEAT_UNLOCK_FACILITY,
    CHEAT_UNLOCK_RUNWAY,
    CHEAT_UNLOCK_SURFACE,
    CHEAT_UNLOCK_BUNKER,
    CHEAT_UNLOCK_SILO,
    CHEAT_UNLOCK_FRIGATE,
    CHEAT_UNLOCK_SURFACE2,
    CHEAT_UNLOCK_BUNKER2,
    CHEAT_UNLOCK_STATUE,
    CHEAT_UNLOCK_ARCHIVES,
    CHEAT_UNLOCK_STREETS,
    CHEAT_UNLOCK_DEPOT,
    CHEAT_UNLOCK_TRAIN,
    CHEAT_UNLOCK_JUNGLE,
    CHEAT_UNLOCK_CONTROL,
    CHEAT_UNLOCK_CAVERNS,
    CHEAT_UNLOCK_CRADLE,
    CHEAT_UNLOCK_AZTEK,
    CHEAT_UNLOCK_EGYPT,

    CHEAT_INVALID,

    CHEAT_MAX = CHEAT_INVALID+5
} CHEAT_ID;



typedef enum CONTROLLER_CONFIG
{

    CONTROLLER_CONFIG_HONEY,


    CONTROLLER_CONFIG_SOLITARE,


    CONTROLLER_CONFIG_KISSY,


    CONTROLLER_CONFIG_GOODNIGHT,


    CONTROLLER_CONFIG_PLENTY,


    CONTROLLER_CONFIG_GALORE,


    CONTROLLER_CONFIG_DOMINO,


    CONTROLLER_CONFIG_GOODHEAD,

    CONTROLLER_CONFIG_CINEMA
} CONTROLLER_CONFIG;

enum WALLETBOND_SWITCHNAMES
{
    SW_TABS,
    SW_PAPER,
    SW_EYESONLY,
    SW_OHMSS,
    SW_CONFIDENTIAL,
    SW_CONFIDENTIAL2,
    SW_CLASSIFIED,
    SW_PHOTOBOND,
    SW_BROSNAN,
    SW_CONNERY,
    SW_DALTON,
    SW_MOORE,
    SW_PHOTOBRIEF,
    SW_COVER,
    SW_PHOTOCOVER,
    SW_BROSNANCOVER,
    SW_CONNERYCOVER,
    SW_DALTONCOVER,
    SW_MOORECOVER,
    SW_SLIDES,
    SW_PICS,
    GFXHIT0_PICS,
    SW_BRIEF1,
    SW_BRIEF2,
    SW_BRIEF3,
    SW_BRIEF4,
    SW_BRIEF5,
    SW_BRIEF6,
    SW_BRIEF7,
    SW_BRIEF8,
    SW_BRIEF9,
    SW_BRIEF10,
    SW_BRIEF11,
    SW_BRIEF12,
    SW_BRIEF13,
    SW_BRIEF14,
    SW_BRIEF15,
    SW_BRIEF16,
    SW_BRIEF17,
    SW_BRIEF18,
    SW_BRIEF19,
    SW_BRIEF20,
    SW_BLANK
} ;

typedef enum GAMELENGTH
{
    LEN_UNLIMITED,
    LEN_5MIN,
    LEN_10MIN,
    LEN_20MIN,
    LEN_5PT,
    LEN_10PT,
    LEN_20PT,
    LEN_LAST
} GAMELENGTH;

typedef enum CUFF_TYPES
{
    CUFF_BLUE,
    CUFF_BROSNAN,
    CUFF_JUNGLE,
    CUFF_BOILER,
    CUFF_SNOW,
    CUFF_CONNERY,
    CUFF_DALTON,
    CUFF_MOORE,
    CUFF_FOLDER
} CUFF_TYPES;

typedef enum CREDITS_ALIGNMENT
{
    CREDITS_ALIGN_PREVIOUS = -1,
    CREDITS_ALIGN_RIGHT,
    CREDITS_ALIGN_LEFT,
    CREDITS_ALIGN_CENTER
} CREDITS_ALIGNMENT;

typedef enum CROUCH
{
    CROUCH_SQUAT,
    CROUCH_HALF,
    CROUCH_STAND
}CROUCH;

typedef enum DEBUG_OPTIONS
{
    DEB_MOVE_VIEW,
    DEB_STAN_VIEW,
    DEB_BOND_VIEW,
    DEB_LEVEL,
    DEB_REGION,
    DEB_SCALE,
    DEB_PLAYTITLE,
    DEB_BONDDIE,
    DEB_SELANIM,
    DEB_GUNPOS,
    DEB_FLASHCOLOR,
    DEB_HITCOLOR,
    DEB_MUSIC,
    DEB_SFX,
    DEB_INVINCIBLE,
    DEB_VISIBILE,
    DEB_COLLISIONS,
    DEB_ALLGUNS,
    DEB_MAXAMMO,
    DEB_DISPSPEED,
    DEB_BG,
    DEB_PROPS,
    DEB_STANHIT,
    DEB_STANREGION,
    DEB_STANPROBLEMS,
    DEB_PRTMANPOS,
    DEB_PORTCLOSE,
    DEB_PORT_INF,
    DEB_PORT_APPROX,
    DEB_PRROOMLOADS,
    DEB_SHOWMEMUSE,
    DEB_SHOWMEMBARS,
    DEB_GRABRGB,
    DEB_GRABJPG,
    DEB_TASKGRAB,
    DEB_RNDWALK,
    DEB_RECRAMROM,
    DEB_RECORD1,
    DEB_RECORD2,
    DEB_RECORD3,
    DEB_REPLAYRAMROM,
    DEB_SAVERAMROM,
    DEB_LOADRAMROM,
    DEB_AUTOAIMY,
    DEB_AUTOAIMX,
    DEB_007,
    DEB_AGENT,
    DEB_ALL,
    DEB_FAST,
    DEB_OBJECTIVES,
    DEB_MARGTOP,
    DEB_MARGBOT,
    DEB_MARGLEFT,
    DEB_MARGRIGHT,
    DEB_MARGRESET,
    DEB_SCREENSIZE,
    DEB_SCREENPOS,
    DEB_SHOWPATROLS,
    DEB_INTRO,
    DEB_INTRO_EDIT,
    DEB_INTRO_POS,
    DEB_WORLD_POS,
    DEB_GUNKEYPOS,
    DEB_VISCVG,
    DEB_CHRNUM,
    DEB_ROOMBLOCKS,
    DEB_PROFILE,
    DEB_OBJLOAD,
    DEB_WEAPONLOAD,
    DEB_JOY2SKYEDIT,
    DEB_JOY2HITSEDIT,
    DEB_JOY2DETAILEDIT,
    DEB_EXPLOSIONINFO,
    DEB_MAGICFOG,
    DEB_GUNWATCHPOS,
    DEB_TESTINGMANPOS,
    DEB_FOG
} DEBUG_MODES;


typedef enum DIFFICULTY
{
    DIFFICULTY_MULTI = -1,
    DIFFICULTY_AGENT,
    DIFFICULTY_SECRET,
    DIFFICULTY_00,
    DIFFICULTY_007,
    DIFFICULTY_MAX
} DIFFICULTY;

typedef enum ENVIRONMENTDATA_IDS
{
    ENVIRONMENTDATA_PLAYERS_1,
    ENVIRONMENTDATA_ALT = 100,
    ENVIRONMENTDATA_PLAYERS_2 = 200,
    ENVIRONMENTDATA_PLAYERS_3 = 300,
    ENVIRONMENTDATA_PLAYERS_4 = 400,
    ENVIRONMENTDATA_CINEMA = 900

} ENVIRONMENTDATA_IDS;



typedef enum E_EXPLOSIONTYPE
{
    EXPLOSION_BREAK_OBJECT,
    EXPLOSION_BREAK_OBJECT2,
    EXPLOSION_MEDIUM,
    EXPLOSION_MEDIUM2,
    EXPLOSION_LARGE,
    EXPLOSION_LARGE2,
    EXPLOSION_SMALL,
    EXPLOSION_SMALL2,
    EXPLOSION_MEDUIM_SMALLFIRE,
    EXPLOSION_MULTIFIRE,
    EXPLOSION_MEDIUM3,
    EXPLOSION_MEDIUM4,
    EXPLOSION_EXTINGUISHER,
    EXPLOSION_LARGE4,
    EXPLOSION_MUSHROOM,
    EXPLOSION_BITS,
    E_EXPLOSIONTYPE_COUNT
} E_EXPLOSIONTYPE;





typedef enum EXPLOSION_DEF
{
    EXPLOSION_DEF_00,
    EXPLOSION_DEF_01,
    EXPLOSION_DEF_02,
    EXPLOSION_DEF_03,
    EXPLOSION_DEF_04,
    EXPLOSION_DEF_05,
    EXPLOSION_DEF_06,
    EXPLOSION_DEF_07,
    EXPLOSION_DEF_08,
    EXPLOSION_DEF_09,
    EXPLOSION_DEF_10,
    EXPLOSION_DEF_11,
    EXPLOSION_DEF_DRONE,

    EXPLOSION_DEF_STANDARD,
    EXPLOSION_DEF_14,
    EXPLOSION_DEF_15,
    EXPLOSION_DEF_16,
    EXPLOSION_DEF_MASSIVE,
    EXPLOSION_DEF_PLAYER,
    EXPLOSION_DEF_FACILITY_REMOTE,
    EXPLOSION_DEF_20,
    EXPLOSION_DEF_21
} EXPLOSION_DEF;

typedef enum GAMEMODE
{
    GAMEMODE_INTRO = -1,
    GAMEMODE_SOLO,
    GAMEMODE_MULTI,
    GAMEMODE_CHEATS
} GAMEMODE;

typedef enum GUNHAND
{
    GUNRIGHT,
    GUNLEFT,
    GUNHANDS
} GUNHAND;



typedef enum HIT_TYPE
{
    HIT_DEFAULT,
    HIT_STONE,
    HIT_WOOD,
    HIT_METAL,
    HIT_GLASS,
    HIT_WATER,
    HIT_SNOW,
    HIT_DIRT,
    HIT_MUD,
    HIT_TILE,
    HIT_METALOBJ,
    HIT_CHR,
    HIT_GLASS_XLU,
    HIT_TYPE_COUNT
}HIT_TYPE;
# 1360 "src/bondconstants.h"
typedef enum IMAGEIDS
{
# 1 "./assets/images.def" 1
IMAGE_COPYICON,
IMAGE_X,
IMAGE_DELICON,
IMAGE_SELECTFILE,
IMAGE_CHECK,
IMAGE_TRAIN715_5264,
IMAGE_TRAIN715,
IMAGE_VERTICALSTRIPES_BW,
IMAGE_VERTICALSTRIPES_yELLOW,
IMAGE_STOMEMAN_L,
IMAGE_STOMEMAN_R,
IMAGE_11,
IMAGE_7SEG_DASH,
IMAGE_7SEG_0,
IMAGE_7SEG_3,
IMAGE_7SEG_5,
IMAGE_7SEG_8,
IMAGE_17,
IMAGE_VERTICALSTRIPES_RED,
IMAGE_VERTICALSTRIPES_RED2,
IMAGE_GRAVELGREY,
IMAGE_GRAVELDETAIL,
IMAGE_TARDETAIL,
IMAGE_TAZER_TEXT,
IMAGE_BOY_TEXT,
IMAGE_25,
IMAGE_BARRELSPECULAR,
IMAGE_BLACKGUNFRAME_L,
IMAGE_BLACKGUNFRAME_R,
IMAGE_BUZZERSWITCH,
IMAGE_ALARM,
IMAGE_BELL,
IMAGE_BELL2,
IMAGE_AMMOCRATE1,
IMAGE_AMMOCRATE2,
IMAGE_AMMOCRATEBANDED1,
IMAGE_AMMOCRATEBANDED2,
IMAGE_CRATEROPE,
IMAGE_12GCART,
IMAGE_AMMOTEXT765,
IMAGE_AMMOTEXTGL,
IMAGE_AMMOTEXTGRENADES,
IMAGE_AMMOTEXT9MM,
IMAGE_43,
IMAGE_44,
IMAGE_45,
IMAGE_46,
IMAGE_47,
IMAGE_48,
IMAGE_49,
IMAGE_50,
IMAGE_51,
IMAGE_52,
IMAGE_53,
IMAGE_WHEELSIDE,
IMAGE_WHEELSIDE_SM,
IMAGE_56,
IMAGE_57,
IMAGE_58,
IMAGE_59,
IMAGE_60,
IMAGE_61,
IMAGE_62,
IMAGE_63,
IMAGE_64,
IMAGE_65,
IMAGE_66,
IMAGE_67,
IMAGE_68,
IMAGE_69,
IMAGE_70,
IMAGE_71,
IMAGE_72,
IMAGE_73,
IMAGE_74,
IMAGE_75,
IMAGE_76,
IMAGE_GAZ_SIDE,
IMAGE_78,
IMAGE_79,
IMAGE_GAZ_DOOR,
IMAGE_81,
IMAGE_82,
IMAGE_83,
IMAGE_84,
IMAGE_DARK_CONCRETE_WALL,
IMAGE_DARK_CONCRETE_WALL_LIT,
IMAGE_HOLE_IN_SNOW_DECAL,
IMAGE_AZTEC_88,
IMAGE_AZTEC_89,
IMAGE_AZTEC_90,
IMAGE_AZTEC_91,
IMAGE_AZTEC_92,
IMAGE_AZTEC_93,
IMAGE_AZTEC_94,
IMAGE_AZTEC_95,
IMAGE_AZTEC_96,
IMAGE_AZTEC_97,
IMAGE_AZTEC_98,
IMAGE_AZTEC_99,
IMAGE_AZTEC_100,
IMAGE_AZTEC_101,
IMAGE_AZTEC_102,
IMAGE_AZTEC_103,
IMAGE_AZTEC_104,
IMAGE_AZTEC_105,
IMAGE_AZTEC_106,
IMAGE_AZTEC_107,
IMAGE_AZTEC_108,
IMAGE_AZTEC_109,
IMAGE_AZTEC_110,
IMAGE_AZTEC_111,
IMAGE_AZTEC_112,
IMAGE_AZTEC_113,
IMAGE_AZTEC_114,
IMAGE_AZTEC_115,
IMAGE_AZTEC_116,
IMAGE_AZTEC_117,
IMAGE_AZTEC_118,
IMAGE_AZTEC_119,
IMAGE_AZTEC_120,
IMAGE_AZTEC_121,
IMAGE_AZTEC_122,
IMAGE_AZTEC_123,
IMAGE_AZTEC_124,
IMAGE_AZTEC_125,
IMAGE_AZTEC_126,
IMAGE_AZTEC_127,
IMAGE_AZTEC_128,
IMAGE_AZTEC_129,
IMAGE_AZTEC_130,
IMAGE_AZTEC_131,
IMAGE_AZTEC_132,
IMAGE_AZTEC_133,
IMAGE_AZTEC_134,
IMAGE_AZTEC_135,
IMAGE_AZTEC_136,
IMAGE_AZTEC_137,
IMAGE_AZTEC_138,
IMAGE_AZTEC_139,
IMAGE_AZTEC_140,
IMAGE_AZTEC_141,
IMAGE_AZTEC_142,
IMAGE_AZTEC_143,
IMAGE_AZTEC_144,
IMAGE_AZTEC_145,
IMAGE_AZTEC_146,
IMAGE_CROSSMEMBER,
IMAGE_RAILING_BLUE,
IMAGE_RAILING_3BAR,
IMAGE_RAILING_2BAR,
IMAGE_RAILING_BLUE_LIGHT,
IMAGE_RAILING_3BAR_LIGHT,
IMAGE_RAILNG_VERTICAL,
IMAGE_RAILING_3BAR_MID,
IMAGE_RAILING_GLASS,
IMAGE_RAILING_ARECEBO,
IMAGE_157,
IMAGE_158,
IMAGE_159,
IMAGE_160,
IMAGE_161,
IMAGE_162,
IMAGE_163,
IMAGE_164,
IMAGE_165,
IMAGE_166,
IMAGE_167,
IMAGE_168,
IMAGE_169,
IMAGE_170,
IMAGE_171,
IMAGE_172,
IMAGE_173,
IMAGE_174,
IMAGE_175,
IMAGE_SAW,
IMAGE_177,
IMAGE_178,
IMAGE_179,
IMAGE_180,
IMAGE_181,
IMAGE_BLOTTER,
IMAGE_183,
IMAGE_184,
IMAGE_BOOK_SPINE,
IMAGE_186,
IMAGE_187,
IMAGE_188,
IMAGE_BUNKERPLAN_TL,
IMAGE_BUNKERPLAN_TR,
IMAGE_BUNKERPLAN_BL,
IMAGE_BUNKERPLAN_BR,
IMAGE_193,
IMAGE_194,
IMAGE_BRICK,
IMAGE_196,
IMAGE_197,
IMAGE_198,
IMAGE_199,
IMAGE_RUSTYDRUM_END,
IMAGE_WALL_LAMP,
IMAGE_202,
IMAGE_203_LIGHT,
IMAGE_204,
IMAGE_205_LIGHT,
IMAGE_IMPACTLOTS,
IMAGE_207,
IMAGE_208,
IMAGE_209,
IMAGE_210,
IMAGE_211,
IMAGE_212,
IMAGE_213,
IMAGE_214,
IMAGE_215,
IMAGE_216,
IMAGE_217,
IMAGE_218,
IMAGE_219,
IMAGE_220,
IMAGE_221,
IMAGE_222,
IMAGE_223,
IMAGE_224,
IMAGE_225,
IMAGE_226,
IMAGE_227,
IMAGE_228,
IMAGE_229,
IMAGE_230,
IMAGE_231,
IMAGE_232,
IMAGE_233,
IMAGE_234,
IMAGE_235,
IMAGE_236,
IMAGE_237,
IMAGE_238,
IMAGE_239,
IMAGE_240,
IMAGE_241,
IMAGE_242,
IMAGE_243,
IMAGE_244,
IMAGE_245,
IMAGE_246,
IMAGE_247,
IMAGE_248,
IMAGE_249,
IMAGE_250,
IMAGE_251,
IMAGE_252_LIGHT,
IMAGE_253,
IMAGE_PANEL_LAMP,
IMAGE_255_LIGHT,
IMAGE_256_LIGHT,
IMAGE_257,
IMAGE_258,
IMAGE_259,
IMAGE_260,
IMAGE_261,
IMAGE_262,
IMAGE_263,
IMAGE_264,
IMAGE_265,
IMAGE_266,
IMAGE_267,
IMAGE_268,
IMAGE_269,
IMAGE_270,
IMAGE_271,
IMAGE_272,
IMAGE_273,
IMAGE_274,
IMAGE_275,
IMAGE_276,
IMAGE_277,
IMAGE_278,
IMAGE_279,
IMAGE_280,
IMAGE_281,
IMAGE_282,
IMAGE_283,
IMAGE_284,
IMAGE_285,
IMAGE_286,
IMAGE_287,
IMAGE_288,
IMAGE_289,
IMAGE_290,
IMAGE_291,
IMAGE_292,
IMAGE_293,
IMAGE_294,
IMAGE_295,
IMAGE_296,
IMAGE_297,
IMAGE_298,
IMAGE_299,
IMAGE_300,
IMAGE_301,
IMAGE_302,
IMAGE_303,
IMAGE_304,
IMAGE_305,
IMAGE_306,
IMAGE_307,
IMAGE_308,
IMAGE_309,
IMAGE_310,
IMAGE_311,
IMAGE_312,
IMAGE_313,
IMAGE_314,
IMAGE_315,
IMAGE_316,
IMAGE_317,
IMAGE_318,
IMAGE_319,
IMAGE_320,
IMAGE_321,
IMAGE_322,
IMAGE_323,
IMAGE_324,
IMAGE_325,
IMAGE_326,
IMAGE_327,
IMAGE_328,
IMAGE_329,
IMAGE_330,
IMAGE_331,
IMAGE_332,
IMAGE_333,
IMAGE_334,
IMAGE_335,
IMAGE_336,
IMAGE_337,
IMAGE_338,
IMAGE_339,
IMAGE_340,
IMAGE_341,
IMAGE_342,
IMAGE_343,
IMAGE_344,
IMAGE_345,
IMAGE_346,
IMAGE_347,
IMAGE_348,
IMAGE_349,
IMAGE_350,
IMAGE_351,
IMAGE_352,
IMAGE_353,
IMAGE_354,
IMAGE_355,
IMAGE_356,
IMAGE_357,
IMAGE_358,
IMAGE_359,
IMAGE_360,
IMAGE_361,
IMAGE_362,
IMAGE_363,
IMAGE_364,
IMAGE_365,
IMAGE_366,
IMAGE_367,
IMAGE_368,
IMAGE_369,
IMAGE_370,
IMAGE_371,
IMAGE_372,
IMAGE_373,
IMAGE_374,
IMAGE_375,
IMAGE_376,
IMAGE_377,
IMAGE_378,
IMAGE_379,
IMAGE_380,
IMAGE_381,
IMAGE_382,
IMAGE_383,
IMAGE_384,
IMAGE_385,
IMAGE_386,
IMAGE_387,
IMAGE_388,
IMAGE_389,
IMAGE_390,
IMAGE_391,
IMAGE_392,
IMAGE_393,
IMAGE_394,
IMAGE_395,
IMAGE_396,
IMAGE_397,
IMAGE_398,
IMAGE_399,
IMAGE_400,
IMAGE_401,
IMAGE_402,
IMAGE_403,
IMAGE_404,
IMAGE_405,
IMAGE_406,
IMAGE_407,
IMAGE_408,
IMAGE_409,
IMAGE_410,
IMAGE_411,
IMAGE_412,
IMAGE_413,
IMAGE_414,
IMAGE_415,
IMAGE_416,
IMAGE_417,
IMAGE_418,
IMAGE_419,
IMAGE_420,
IMAGE_421,
IMAGE_422,
IMAGE_423,
IMAGE_424,
IMAGE_425,
IMAGE_426,
IMAGE_427,
IMAGE_HANGING_LAMP,
IMAGE_429,
IMAGE_430,
IMAGE_431,
IMAGE_432,
IMAGE_433,
IMAGE_434,
IMAGE_435,
IMAGE_436,
IMAGE_437,
IMAGE_438,
IMAGE_439,
IMAGE_440,
IMAGE_441,
IMAGE_442,
IMAGE_443,
IMAGE_444,
IMAGE_445,
IMAGE_446,
IMAGE_447,
IMAGE_448,
IMAGE_449,
IMAGE_450,
IMAGE_451,
IMAGE_452,
IMAGE_453,
IMAGE_454,
IMAGE_455,
IMAGE_456,
IMAGE_457,
IMAGE_458,
IMAGE_459,
IMAGE_460,
IMAGE_461,
IMAGE_462,
IMAGE_463,
IMAGE_464,
IMAGE_465,
IMAGE_466,
IMAGE_467,
IMAGE_468,
IMAGE_469,
IMAGE_470,
IMAGE_471,
IMAGE_472,
IMAGE_473,
IMAGE_474,
IMAGE_475,
IMAGE_476,
IMAGE_477,
IMAGE_478,
IMAGE_479,
IMAGE_480,
IMAGE_481,
IMAGE_482,
IMAGE_483,
IMAGE_484,
IMAGE_485,
IMAGE_486,
IMAGE_487,
IMAGE_488,
IMAGE_489,
IMAGE_490,
IMAGE_491,
IMAGE_492,
IMAGE_493,
IMAGE_494,
IMAGE_495,
IMAGE_496,
IMAGE_497,
IMAGE_498,
IMAGE_499,
IMAGE_500,
IMAGE_501,
IMAGE_502,
IMAGE_503,
IMAGE_504,
IMAGE_505,
IMAGE_506,
IMAGE_507,
IMAGE_508,
IMAGE_509,
IMAGE_510,
IMAGE_511,
IMAGE_512,
IMAGE_513,
IMAGE_514,
IMAGE_515,
IMAGE_516,
IMAGE_517,
IMAGE_518,
IMAGE_519,
IMAGE_520,
IMAGE_521,
IMAGE_522,
IMAGE_523,
IMAGE_524,
IMAGE_525,
IMAGE_526,
IMAGE_527,
IMAGE_528,
IMAGE_529,
IMAGE_530,
IMAGE_531,
IMAGE_532,
IMAGE_533,
IMAGE_534,
IMAGE_535,
IMAGE_536,
IMAGE_537,
IMAGE_538,
IMAGE_539,
IMAGE_540,
IMAGE_541,
IMAGE_542,
IMAGE_543,
IMAGE_544,
IMAGE_545,
IMAGE_546,
IMAGE_547,
IMAGE_548,
IMAGE_549,
IMAGE_550,
IMAGE_551,
IMAGE_552,
IMAGE_553,
IMAGE_554,
IMAGE_555,
IMAGE_556,
IMAGE_557,
IMAGE_558,
IMAGE_559,
IMAGE_560,
IMAGE_561,
IMAGE_562,
IMAGE_563,
IMAGE_564,
IMAGE_565,
IMAGE_566,
IMAGE_567,
IMAGE_568,
IMAGE_569,
IMAGE_570,
IMAGE_571,
IMAGE_572,
IMAGE_573,
IMAGE_574,
IMAGE_575,
IMAGE_576,
IMAGE_577,
IMAGE_578,
IMAGE_579,
IMAGE_580,
IMAGE_MONITOR_EARTHTEXT,
IMAGE_MONITOR_SHUTTLE1,
IMAGE_MONITOR_SHUTTLE2,
IMAGE_MONITOR_EARTHFULL1,
IMAGE_585,
IMAGE_586,
IMAGE_587,
IMAGE_588,
IMAGE_589,
IMAGE_590,
IMAGE_591,
IMAGE_592,
IMAGE_593,
IMAGE_594,
IMAGE_595,
IMAGE_596,
IMAGE_597,
IMAGE_598,
IMAGE_599,
IMAGE_600,
IMAGE_601,
IMAGE_602,
IMAGE_603,
IMAGE_604,
IMAGE_605,
IMAGE_606,
IMAGE_607,
IMAGE_608,
IMAGE_609,
IMAGE_610,
IMAGE_611,
IMAGE_612,
IMAGE_613,
IMAGE_614,
IMAGE_615,
IMAGE_616,
IMAGE_617,
IMAGE_618,
IMAGE_619,
IMAGE_620,
IMAGE_621,
IMAGE_622,
IMAGE_623,
IMAGE_624,
IMAGE_625,
IMAGE_626,
IMAGE_627,
IMAGE_628,
IMAGE_629,
IMAGE_630,
IMAGE_631,
IMAGE_632,
IMAGE_633,
IMAGE_634,
IMAGE_635,
IMAGE_636,
IMAGE_637,
IMAGE_638,
IMAGE_639,
IMAGE_640,
IMAGE_641,
IMAGE_642,
IMAGE_643,
IMAGE_644,
IMAGE_645,
IMAGE_646,
IMAGE_647,
IMAGE_648,
IMAGE_649,
IMAGE_650,
IMAGE_651,
IMAGE_652,
IMAGE_653,
IMAGE_GLASS3,
IMAGE_655,
IMAGE_656,
IMAGE_657,
IMAGE_GLASS7,
IMAGE_659,
IMAGE_660,
IMAGE_661,
IMAGE_662,
IMAGE_663,
IMAGE_664,
IMAGE_665,
IMAGE_666,
IMAGE_667,
IMAGE_668,
IMAGE_669,
IMAGE_670,
IMAGE_671,
IMAGE_672,
IMAGE_673,
IMAGE_674,
IMAGE_675,
IMAGE_676,
IMAGE_677,
IMAGE_678,
IMAGE_679,
IMAGE_680,
IMAGE_681,
IMAGE_682,
IMAGE_683,
IMAGE_684,
IMAGE_685,
IMAGE_686,
IMAGE_687,
IMAGE_688,
IMAGE_689,
IMAGE_690,
IMAGE_691,
IMAGE_692,
IMAGE_693,
IMAGE_694,
IMAGE_695,
IMAGE_696,
IMAGE_697,
IMAGE_698,
IMAGE_699,
IMAGE_700,
IMAGE_701,
IMAGE_702,
IMAGE_703,
IMAGE_704,
IMAGE_705,
IMAGE_706,
IMAGE_707,
IMAGE_708,
IMAGE_709,
IMAGE_710,
IMAGE_711,
IMAGE_712,
IMAGE_713,
IMAGE_714,
IMAGE_715,
IMAGE_716,
IMAGE_717,
IMAGE_718,
IMAGE_719,
IMAGE_720,
IMAGE_721,
IMAGE_722,
IMAGE_723,
IMAGE_724,
IMAGE_725,
IMAGE_726,
IMAGE_727,
IMAGE_728,
IMAGE_729,
IMAGE_730,
IMAGE_731,
IMAGE_732,
IMAGE_733,
IMAGE_734,
IMAGE_735,
IMAGE_736,
IMAGE_737,
IMAGE_738,
IMAGE_739,
IMAGE_740,
IMAGE_741,
IMAGE_742,
IMAGE_743,
IMAGE_744,
IMAGE_745,
IMAGE_746,
IMAGE_747,
IMAGE_748,
IMAGE_749,
IMAGE_750,
IMAGE_751,
IMAGE_752,
IMAGE_753,
IMAGE_754,
IMAGE_755,
IMAGE_756,
IMAGE_757,
IMAGE_758,
IMAGE_759,
IMAGE_760,
IMAGE_761,
IMAGE_762,
IMAGE_763,
IMAGE_764,
IMAGE_765,
IMAGE_766,
IMAGE_767,
IMAGE_768,
IMAGE_769,
IMAGE_770,
IMAGE_771,
IMAGE_772,
IMAGE_773,
IMAGE_774,
IMAGE_775,
IMAGE_776,
IMAGE_777,
IMAGE_778,
IMAGE_779,
IMAGE_780,
IMAGE_781,
IMAGE_782,
IMAGE_783,
IMAGE_784,
IMAGE_785,
IMAGE_786,
IMAGE_787,
IMAGE_788,
IMAGE_789,
IMAGE_790,
IMAGE_791,
IMAGE_792,
IMAGE_793,
IMAGE_794,
IMAGE_795,
IMAGE_796,
IMAGE_797,
IMAGE_798,
IMAGE_799,
IMAGE_800,
IMAGE_801,
IMAGE_802,
IMAGE_803,
IMAGE_804,
IMAGE_805,
IMAGE_806,
IMAGE_807,
IMAGE_808,
IMAGE_809,
IMAGE_810,
IMAGE_811,
IMAGE_812,
IMAGE_813,
IMAGE_814,
IMAGE_815,
IMAGE_816,
IMAGE_817,
IMAGE_818,
IMAGE_819,
IMAGE_820,
IMAGE_821,
IMAGE_822,
IMAGE_823,
IMAGE_824,
IMAGE_825,
IMAGE_826,
IMAGE_827,
IMAGE_828,
IMAGE_829,
IMAGE_830,
IMAGE_831,
IMAGE_832,
IMAGE_833,
IMAGE_834,
IMAGE_835,
IMAGE_836,
IMAGE_MONITOR_KEYBOARDKEY,
IMAGE_838,
IMAGE_839,
IMAGE_840,
IMAGE_841,
IMAGE_842,
IMAGE_843,
IMAGE_844,
IMAGE_845,
IMAGE_846,
IMAGE_847,
IMAGE_848,
IMAGE_849,
IMAGE_850,
IMAGE_851,
IMAGE_852,
IMAGE_853,
IMAGE_854,
IMAGE_855,
IMAGE_856,
IMAGE_857,
IMAGE_858,
IMAGE_859,
IMAGE_860,
IMAGE_861,
IMAGE_862,
IMAGE_863,
IMAGE_864,
IMAGE_865,
IMAGE_866,
IMAGE_867,
IMAGE_868,
IMAGE_869,
IMAGE_870,
IMAGE_871,
IMAGE_872,
IMAGE_873,
IMAGE_874,
IMAGE_875,
IMAGE_876,
IMAGE_877,
IMAGE_878,
IMAGE_879,
IMAGE_880,
IMAGE_881,
IMAGE_882,
IMAGE_883,
IMAGE_884,
IMAGE_885,
IMAGE_886,
IMAGE_887,
IMAGE_888,
IMAGE_889,
IMAGE_890,
IMAGE_891,
IMAGE_892,
IMAGE_893,
IMAGE_894,
IMAGE_895,
IMAGE_896,
IMAGE_897,
IMAGE_898,
IMAGE_899,
IMAGE_900,
IMAGE_901,
IMAGE_902,
IMAGE_903,
IMAGE_904,
IMAGE_905,
IMAGE_906,
IMAGE_907,
IMAGE_908,
IMAGE_909,
IMAGE_910,
IMAGE_911,
IMAGE_912,
IMAGE_913,
IMAGE_914,
IMAGE_915,
IMAGE_916,
IMAGE_917,
IMAGE_918,
IMAGE_919,
IMAGE_920,
IMAGE_921,
IMAGE_922,
IMAGE_923,
IMAGE_924,
IMAGE_925,
IMAGE_926,
IMAGE_927,
IMAGE_928,
IMAGE_929,
IMAGE_930,
IMAGE_931,
IMAGE_932,
IMAGE_933,
IMAGE_934,
IMAGE_935,
IMAGE_936,
IMAGE_937,
IMAGE_938,
IMAGE_939,
IMAGE_940,
IMAGE_941,
IMAGE_942,
IMAGE_943,
IMAGE_944,
IMAGE_945,
IMAGE_946,
IMAGE_947,
IMAGE_948,
IMAGE_949,
IMAGE_950,
IMAGE_951,
IMAGE_952,
IMAGE_953,
IMAGE_954,
IMAGE_955,
IMAGE_956,
IMAGE_957,
IMAGE_958,
IMAGE_959,
IMAGE_960,
IMAGE_961,
IMAGE_962,
IMAGE_963,
IMAGE_964,
IMAGE_965,
IMAGE_966,
IMAGE_967,
IMAGE_968,
IMAGE_969,
IMAGE_970,
IMAGE_971,
IMAGE_972,
IMAGE_973,
IMAGE_974,
IMAGE_975,
IMAGE_976,
IMAGE_977,
IMAGE_978,
IMAGE_979,
IMAGE_980,
IMAGE_981,
IMAGE_NEON_LAMP,
IMAGE_983,
IMAGE_984,
IMAGE_985,
IMAGE_986,
IMAGE_987,
IMAGE_988,
IMAGE_989,
IMAGE_990,
IMAGE_991,
IMAGE_992,
IMAGE_993,
IMAGE_994,
IMAGE_995,
IMAGE_996,
IMAGE_997,
IMAGE_998,
IMAGE_999,
IMAGE_1000,
IMAGE_1001,
IMAGE_1002,
IMAGE_1003,
IMAGE_1004,
IMAGE_1005,
IMAGE_1006,
IMAGE_1007,
IMAGE_1008,
IMAGE_1009,
IMAGE_1010,
IMAGE_1011,
IMAGE_1012,
IMAGE_1013,
IMAGE_PAPERCLIP,
IMAGE_STATIC_NOISE,
IMAGE_1016,
IMAGE_1017,
IMAGE_1018,
IMAGE_1019,
IMAGE_1020,
IMAGE_1021,
IMAGE_1022,
IMAGE_1023,
IMAGE_1024,
IMAGE_1025,
IMAGE_1026,
IMAGE_1027,
IMAGE_1028,
IMAGE_1029,
IMAGE_1030,
IMAGE_1031,
IMAGE_1032,
IMAGE_1033,
IMAGE_1034,
IMAGE_1035,
IMAGE_1036,
IMAGE_1037,
IMAGE_1038,
IMAGE_1039,
IMAGE_1040,
IMAGE_1041,
IMAGE_1042,
IMAGE_1043,
IMAGE_1044,
IMAGE_1045,
IMAGE_1046,
IMAGE_1047,
IMAGE_1048,
IMAGE_1049,
IMAGE_1050,
IMAGE_1051,
IMAGE_1052,
IMAGE_1053,
IMAGE_1054,
IMAGE_1055,
IMAGE_1056,
IMAGE_1057,
IMAGE_1058,
IMAGE_1059,
IMAGE_1060,
IMAGE_1061,
IMAGE_1062,
IMAGE_1063,
IMAGE_1064,
IMAGE_1065,
IMAGE_1066,
IMAGE_1067,
IMAGE_1068,
IMAGE_1069,
IMAGE_1070,
IMAGE_1071,
IMAGE_1072,
IMAGE_1073,
IMAGE_1074,
IMAGE_1075,
IMAGE_1076,
IMAGE_1077,
IMAGE_1078,
IMAGE_1079,
IMAGE_1080,
IMAGE_1081,
IMAGE_1082,
IMAGE_1083,
IMAGE_1084,
IMAGE_1085,
IMAGE_1086,
IMAGE_1087,
IMAGE_1088,
IMAGE_1089,
IMAGE_1090,
IMAGE_1091,
IMAGE_1092,
IMAGE_1093,
IMAGE_1094,
IMAGE_1095,
IMAGE_1096,
IMAGE_1097,
IMAGE_1098,
IMAGE_1099,
IMAGE_1100,
IMAGE_1101,
IMAGE_1102,
IMAGE_1103,
IMAGE_1104,
IMAGE_1105,
IMAGE_1106,
IMAGE_1107,
IMAGE_1108,
IMAGE_1109,
IMAGE_1110,
IMAGE_1111,
IMAGE_1112,
IMAGE_1113,
IMAGE_1114,
IMAGE_1115,
IMAGE_1116,
IMAGE_1117,
IMAGE_1118,
IMAGE_1119,
IMAGE_1120,
IMAGE_1121,
IMAGE_1122,
IMAGE_1123,
IMAGE_1124,
IMAGE_1125,
IMAGE_1126,
IMAGE_1127,
IMAGE_1128,
IMAGE_1129,
IMAGE_1130,
IMAGE_1131,
IMAGE_1132,
IMAGE_1133,
IMAGE_1134,
IMAGE_1135,
IMAGE_1136,
IMAGE_1137,
IMAGE_1138,
IMAGE_1139,
IMAGE_1140,
IMAGE_1141,
IMAGE_1142,
IMAGE_1143,
IMAGE_1144,
IMAGE_1145,
IMAGE_1146,
IMAGE_1147,
IMAGE_1148,
IMAGE_1149,
IMAGE_1150,
IMAGE_1151,
IMAGE_1152,
IMAGE_1153,
IMAGE_1154,
IMAGE_1155,
IMAGE_1156,
IMAGE_1157,
IMAGE_1158,
IMAGE_1159,
IMAGE_1160,
IMAGE_1161,
IMAGE_1162,
IMAGE_1163,
IMAGE_1164,
IMAGE_1165,
IMAGE_1166,
IMAGE_1167,
IMAGE_1168,
IMAGE_1169,
IMAGE_1170,
IMAGE_1171,
IMAGE_1172,
IMAGE_1173,
IMAGE_1174,
IMAGE_1175,
IMAGE_1176,
IMAGE_1177,
IMAGE_1178,
IMAGE_1179,
IMAGE_1180,
IMAGE_1181,
IMAGE_1182,
IMAGE_1183,
IMAGE_1184,
IMAGE_MONITOR_DESKTOPBARS,
IMAGE_MONITOR_DESKTOP,
IMAGE_MONITOR_DESKTOPSTAGGERED,
IMAGE_1188,
IMAGE_1189,
IMAGE_1190,
IMAGE_1191,
IMAGE_1192,
IMAGE_1193,
IMAGE_1194,
IMAGE_1195,
IMAGE_1196,
IMAGE_1197,
IMAGE_1198,
IMAGE_1199,
IMAGE_1200,
IMAGE_1201,
IMAGE_1202,
IMAGE_1203,
IMAGE_1204,
IMAGE_1205,
IMAGE_1206,
IMAGE_1207,
IMAGE_1208,
IMAGE_1209,
IMAGE_1210,
IMAGE_1211,
IMAGE_1212,
IMAGE_1213,
IMAGE_1214,
IMAGE_1215,
IMAGE_1216,
IMAGE_1217,
IMAGE_1218,
IMAGE_1219,
IMAGE_1220,
IMAGE_1221,
IMAGE_1222,
IMAGE_1223,
IMAGE_1224,
IMAGE_1225,
IMAGE_1226,
IMAGE_1227,
IMAGE_1228,
IMAGE_1229,
IMAGE_1230,
IMAGE_1231,
IMAGE_1232,
IMAGE_1233,
IMAGE_1234,
IMAGE_1235,
IMAGE_1236,
IMAGE_1237,
IMAGE_1238,
IMAGE_1239,
IMAGE_1240,
IMAGE_1241,
IMAGE_1242,
IMAGE_1243,
IMAGE_1244,
IMAGE_1245,
IMAGE_1246,
IMAGE_1247,
IMAGE_1248,
IMAGE_1249,
IMAGE_1250,
IMAGE_1251,
IMAGE_1252,
IMAGE_1253,
IMAGE_1254,
IMAGE_1255,
IMAGE_1256,
IMAGE_1257,
IMAGE_1258,
IMAGE_1259,
IMAGE_1260,
IMAGE_1261,
IMAGE_1262,
IMAGE_1263,
IMAGE_1264,
IMAGE_1265,
IMAGE_1266,
IMAGE_1267,
IMAGE_1268,
IMAGE_1269,
IMAGE_1270,
IMAGE_1271,
IMAGE_1272,
IMAGE_1273,
IMAGE_1274,
IMAGE_1275,
IMAGE_1276,
IMAGE_GBGRADIENT,
IMAGE_1278,
IMAGE_1279,
IMAGE_1280,
IMAGE_1281,
IMAGE_1282,
IMAGE_1283,
IMAGE_1284,
IMAGE_1285,
IMAGE_1286,
IMAGE_1287,
IMAGE_1288,
IMAGE_1289,
IMAGE_1290,
IMAGE_1291,
IMAGE_1292,
IMAGE_1293,
IMAGE_1294,
IMAGE_1295,
IMAGE_1296,
IMAGE_1297,
IMAGE_1298,
IMAGE_1299,
IMAGE_1300,
IMAGE_1301,
IMAGE_1302,
IMAGE_1303,
IMAGE_1304,
IMAGE_1305,
IMAGE_1306,
IMAGE_1307,
IMAGE_1308,
IMAGE_1309,
IMAGE_1310,
IMAGE_1311,
IMAGE_1312,
IMAGE_1313,
IMAGE_1314,
IMAGE_1315,
IMAGE_1316,
IMAGE_1317,
IMAGE_1318,
IMAGE_1319,
IMAGE_1320,
IMAGE_1321,
IMAGE_1322,
IMAGE_1323,
IMAGE_1324,
IMAGE_1325,
IMAGE_1326,
IMAGE_1327,
IMAGE_1328,
IMAGE_1329,
IMAGE_1330,
IMAGE_1331,
IMAGE_1332,
IMAGE_1333,
IMAGE_1334,
IMAGE_1335,
IMAGE_1336,
IMAGE_1337,
IMAGE_1338,
IMAGE_1339,
IMAGE_1340,
IMAGE_1341,
IMAGE_1342,
IMAGE_1343,
IMAGE_1344,
IMAGE_1345,
IMAGE_1346,
IMAGE_1347,
IMAGE_1348,
IMAGE_1349,
IMAGE_1350,
IMAGE_1351,
IMAGE_1352,
IMAGE_1353,
IMAGE_1354,
IMAGE_1355,
IMAGE_1356,
IMAGE_1357,
IMAGE_1358,
IMAGE_1359,
IMAGE_1360,
IMAGE_1361,
IMAGE_1362,
IMAGE_1363,
IMAGE_1364,
IMAGE_1365,
IMAGE_1366,
IMAGE_1367,
IMAGE_1368,
IMAGE_1369,
IMAGE_1370,
IMAGE_1371,
IMAGE_1372,
IMAGE_1373,
IMAGE_1374,
IMAGE_1375,
IMAGE_1376,
IMAGE_1377,
IMAGE_1378,
IMAGE_1379,
IMAGE_1380,
IMAGE_1381,
IMAGE_1382,
IMAGE_LINEAR_LAMP,
IMAGE_1384,
IMAGE_1385,
IMAGE_1386,
IMAGE_1387,
IMAGE_1388,
IMAGE_1389,
IMAGE_1390,
IMAGE_1391,
IMAGE_1392,
IMAGE_1393,
IMAGE_1394,
IMAGE_1395,
IMAGE_1396,
IMAGE_1397,
IMAGE_1398,
IMAGE_1399,
IMAGE_1400,
IMAGE_1401,
IMAGE_1402,
IMAGE_1403,
IMAGE_1404,
IMAGE_1405,
IMAGE_1406,
IMAGE_1407,
IMAGE_1408,
IMAGE_1409,
IMAGE_1410,
IMAGE_1411,
IMAGE_1412,
IMAGE_1413,
IMAGE_1414,
IMAGE_1415,
IMAGE_1416,
IMAGE_1417,
IMAGE_1418,
IMAGE_1419,
IMAGE_1420,
IMAGE_1421,
IMAGE_1422,
IMAGE_1423,
IMAGE_1424,
IMAGE_1425,
IMAGE_1426,
IMAGE_1427,
IMAGE_1428,
IMAGE_1429,
IMAGE_1430,
IMAGE_1431,
IMAGE_1432,
IMAGE_1433,
IMAGE_1434,
IMAGE_1435,
IMAGE_1436,
IMAGE_1437,
IMAGE_1438,
IMAGE_1439,
IMAGE_1440,
IMAGE_1441,
IMAGE_1442,
IMAGE_1443,
IMAGE_1444,
IMAGE_1445,
IMAGE_1446,
IMAGE_1447,
IMAGE_1448,
IMAGE_1449,
IMAGE_1450,
IMAGE_1451,
IMAGE_1452,
IMAGE_1453,
IMAGE_1454,
IMAGE_1455,
IMAGE_1456,
IMAGE_1457,
IMAGE_1458,
IMAGE_1459,
IMAGE_1460,
IMAGE_1461,
IMAGE_1462,
IMAGE_1463,
IMAGE_1464,
IMAGE_1465,
IMAGE_1466,
IMAGE_1467,
IMAGE_1468,
IMAGE_1469,
IMAGE_1470,
IMAGE_1471,
IMAGE_1472,
IMAGE_1473,
IMAGE_1474,
IMAGE_IMPACTREDBRICK1,
IMAGE_IMPACTREDBRICK3,
IMAGE_1477,
IMAGE_IMPACTBROWNBRICK1,
IMAGE_IMPACTBROWNBRICK2,
IMAGE_1480,
IMAGE_1481,
IMAGE_1482,
IMAGE_1483,
IMAGE_1484,
IMAGE_1485,
IMAGE_1486,
IMAGE_1487,
IMAGE_1488,
IMAGE_1489,
IMAGE_1490,
IMAGE_1491,
IMAGE_1492,
IMAGE_1493,
IMAGE_1494,
IMAGE_1495,
IMAGE_1496,
IMAGE_1497,
IMAGE_1498,
IMAGE_1499,
IMAGE_1500,
IMAGE_1501,
IMAGE_1502,
IMAGE_1503,
IMAGE_1504,
IMAGE_1505,
IMAGE_1506,
IMAGE_1507,
IMAGE_WATER_GRAYSCALE,
IMAGE_WATER_BLUE,
IMAGE_1510,
IMAGE_1511,
IMAGE_1512,
IMAGE_1513,
IMAGE_1514,
IMAGE_1515,
IMAGE_1516,
IMAGE_1517,
IMAGE_1518,
IMAGE_1519,
IMAGE_1520,
IMAGE_1521,
IMAGE_1522,
IMAGE_1523,
IMAGE_1524,
IMAGE_1525,
IMAGE_1526,
IMAGE_1527,
IMAGE_1528,
IMAGE_1529,
IMAGE_1530,
IMAGE_1531,
IMAGE_1532,
IMAGE_1533,
IMAGE_1534,
IMAGE_1535,
IMAGE_1536,
IMAGE_1537,
IMAGE_1538,
IMAGE_1539,
IMAGE_1540,
IMAGE_1541,
IMAGE_1542,
IMAGE_1543,
IMAGE_1544,
IMAGE_1545,
IMAGE_1546,
IMAGE_1547,
IMAGE_1548,
IMAGE_1549,
IMAGE_1550,
IMAGE_1551,
IMAGE_1552,
IMAGE_1553,
IMAGE_1554,
IMAGE_1555,
IMAGE_1556,
IMAGE_1557,
IMAGE_1558,
IMAGE_1559,
IMAGE_1560,
IMAGE_1561,
IMAGE_1562,
IMAGE_1563,
IMAGE_1564,
IMAGE_1565,
IMAGE_1566,
IMAGE_1567,
IMAGE_1568,
IMAGE_1569,
IMAGE_1570,
IMAGE_1571,
IMAGE_1572,
IMAGE_1573,
IMAGE_1574,
IMAGE_1575,
IMAGE_1576,
IMAGE_1577,
IMAGE_1578,
IMAGE_1579,
IMAGE_1580,
IMAGE_1581,
IMAGE_1582,
IMAGE_1583,
IMAGE_1584,
IMAGE_1585,
IMAGE_1586,
IMAGE_1587,
IMAGE_1588,
IMAGE_1589,
IMAGE_1590,
IMAGE_1591,
IMAGE_1592,
IMAGE_1593,
IMAGE_1594,
IMAGE_1595,
IMAGE_1596,
IMAGE_1597,
IMAGE_1598,
IMAGE_1599,
IMAGE_1600,
IMAGE_1601,
IMAGE_1602,
IMAGE_1603,
IMAGE_1604,
IMAGE_1605,
IMAGE_1606,
IMAGE_1607,
IMAGE_1608,
IMAGE_1609,
IMAGE_1610,
IMAGE_1611,
IMAGE_1612,
IMAGE_1613,
IMAGE_1614,
IMAGE_1615,
IMAGE_1616,
IMAGE_1617,
IMAGE_1618,
IMAGE_1619,
IMAGE_1620,
IMAGE_1621,
IMAGE_1622,
IMAGE_1623,
IMAGE_1624,
IMAGE_1625,
IMAGE_1626,
IMAGE_1627,
IMAGE_1628,
IMAGE_1629,
IMAGE_1630,
IMAGE_1631,
IMAGE_1632,
IMAGE_1633,
IMAGE_1634,
IMAGE_1635,
IMAGE_1636,
IMAGE_1637,
IMAGE_1638,
IMAGE_1639,
IMAGE_1640,
IMAGE_1641,
IMAGE_1642,
IMAGE_1643,
IMAGE_1644,
IMAGE_1645,
IMAGE_1646,
IMAGE_1647,
IMAGE_1648,
IMAGE_1649,
IMAGE_1650,
IMAGE_1651,
IMAGE_1652,
IMAGE_1653,
IMAGE_1654,
IMAGE_1655,
IMAGE_1656,
IMAGE_1657,
IMAGE_1658,
IMAGE_1659,
IMAGE_1660,
IMAGE_1661,
IMAGE_1662,
IMAGE_1663,
IMAGE_1664,
IMAGE_1665,
IMAGE_1666,
IMAGE_1667,
IMAGE_1668,
IMAGE_1669,
IMAGE_1670,
IMAGE_1671,
IMAGE_1672,
IMAGE_1673,
IMAGE_1674,
IMAGE_1675,
IMAGE_1676,
IMAGE_1677,
IMAGE_1678,
IMAGE_1679,
IMAGE_1680,
IMAGE_1681,
IMAGE_1682,
IMAGE_1683,
IMAGE_1684,
IMAGE_1685,
IMAGE_1686,
IMAGE_1687,
IMAGE_1688,
IMAGE_1689,
IMAGE_1690,
IMAGE_1691,
IMAGE_1692,
IMAGE_1693,
IMAGE_1694,
IMAGE_1695,
IMAGE_1696,
IMAGE_1697,
IMAGE_1698,
IMAGE_1699,
IMAGE_1700,
IMAGE_1701,
IMAGE_1702,
IMAGE_1703,
IMAGE_1704,
IMAGE_1705,
IMAGE_1706,
IMAGE_1707,
IMAGE_1708,
IMAGE_1709,
IMAGE_1710,
IMAGE_1711,
IMAGE_1712,
IMAGE_1713,
IMAGE_1714,
IMAGE_1715,
IMAGE_1716,
IMAGE_1717,
IMAGE_1718,
IMAGE_1719,
IMAGE_1720,
IMAGE_1721,
IMAGE_1722,
IMAGE_1723,
IMAGE_1724,
IMAGE_1725,
IMAGE_1726,
IMAGE_1727,
IMAGE_1728,
IMAGE_1729,
IMAGE_1730,
IMAGE_1731,
IMAGE_1732,
IMAGE_1733,
IMAGE_1734,
IMAGE_1735,
IMAGE_1736,
IMAGE_1737,
IMAGE_1738,
IMAGE_1739,
IMAGE_1740,
IMAGE_1741,
IMAGE_1742,
IMAGE_1743,
IMAGE_1744,
IMAGE_1745,
IMAGE_1746,
IMAGE_1747,
IMAGE_1748,
IMAGE_1749,
IMAGE_1750,
IMAGE_1751,
IMAGE_1752,
IMAGE_1753,
IMAGE_1754,
IMAGE_1755,
IMAGE_1756,
IMAGE_1757,
IMAGE_1758,
IMAGE_1759,
IMAGE_1760,
IMAGE_1761,
IMAGE_1762,
IMAGE_1763,
IMAGE_1764,
IMAGE_1765,
IMAGE_1766,
IMAGE_1767,
IMAGE_1768,
IMAGE_1769,
IMAGE_1770,
IMAGE_1771,
IMAGE_1772,
IMAGE_1773,
IMAGE_1774,
IMAGE_1775,
IMAGE_1776,
IMAGE_1777,
IMAGE_1778,
IMAGE_1779,
IMAGE_1780,
IMAGE_1781,
IMAGE_1782,
IMAGE_1783,
IMAGE_1784,
IMAGE_1785,
IMAGE_1786,
IMAGE_1787,
IMAGE_1788,
IMAGE_1789,
IMAGE_1790,
IMAGE_1791,
IMAGE_1792,
IMAGE_1793,
IMAGE_1794,
IMAGE_1795,
IMAGE_1796,
IMAGE_1797,
IMAGE_1798,
IMAGE_1799,
IMAGE_1800,
IMAGE_1801,
IMAGE_1802,
IMAGE_1803,
IMAGE_1804,
IMAGE_1805,
IMAGE_1806,
IMAGE_1807,
IMAGE_1808,
IMAGE_1809,
IMAGE_1810,
IMAGE_1811,
IMAGE_1812,
IMAGE_1813,
IMAGE_1814,
IMAGE_1815,
IMAGE_1816,
IMAGE_1817,
IMAGE_1818,
IMAGE_1819,
IMAGE_1820,
IMAGE_1821,
IMAGE_1822,
IMAGE_1823,
IMAGE_1824,
IMAGE_1825,
IMAGE_1826,
IMAGE_1827,
IMAGE_1828,
IMAGE_1829,
IMAGE_1830,
IMAGE_1831,
IMAGE_1832,
IMAGE_1833,
IMAGE_1834,
IMAGE_1835,
IMAGE_1836,
IMAGE_1837,
IMAGE_1838,
IMAGE_1839,
IMAGE_1840,
IMAGE_1841,
IMAGE_1842,
IMAGE_1843,
IMAGE_1844,
IMAGE_1845,
IMAGE_1846,
IMAGE_1847,
IMAGE_1848,
IMAGE_1849,
IMAGE_1850,
IMAGE_1851,
IMAGE_1852,
IMAGE_1853,
IMAGE_1854,
IMAGE_1855,
IMAGE_1856,
IMAGE_1857,
IMAGE_1858,
IMAGE_1859,
IMAGE_1860,
IMAGE_1861,
IMAGE_1862,
IMAGE_1863,
IMAGE_1864,
IMAGE_1865,
IMAGE_1866,
IMAGE_1867,
IMAGE_1868,
IMAGE_1869,
IMAGE_1870,
IMAGE_1871,
IMAGE_1872,
IMAGE_1873,
IMAGE_1874,
IMAGE_1875,
IMAGE_1876,
IMAGE_1877,
IMAGE_1878,
IMAGE_1879,
IMAGE_1880,
IMAGE_1881,
IMAGE_1882,
IMAGE_1883,
IMAGE_1884,
IMAGE_1885,
IMAGE_1886,
IMAGE_1887,
IMAGE_1888,
IMAGE_1889,
IMAGE_1890,
IMAGE_1891,
IMAGE_1892,
IMAGE_1893,
IMAGE_1894,
IMAGE_1895,
IMAGE_1896,
IMAGE_1897,
IMAGE_1898,
IMAGE_1899,
IMAGE_1900,
IMAGE_1901,
IMAGE_1902,
IMAGE_1903,
IMAGE_1904,
IMAGE_1905,
IMAGE_1906,
IMAGE_1907,
IMAGE_1908,
IMAGE_1909,
IMAGE_1910,
IMAGE_1911,
IMAGE_1912,
IMAGE_1913,
IMAGE_1914,
IMAGE_1915,
IMAGE_1916,
IMAGE_1917,
IMAGE_1918,
IMAGE_1919,
IMAGE_1920,
IMAGE_1921,
IMAGE_1922,
IMAGE_1923,
IMAGE_1924,
IMAGE_1925,
IMAGE_1926,
IMAGE_1927,
IMAGE_1928,
IMAGE_1929,
IMAGE_1930,
IMAGE_1931,
IMAGE_1932,
IMAGE_1933,
IMAGE_1934,
IMAGE_1935,
IMAGE_1936,
IMAGE_1937,
IMAGE_1938,
IMAGE_1939,
IMAGE_1940,
IMAGE_1941,
IMAGE_1942,
IMAGE_1943,
IMAGE_1944,
IMAGE_1945,
IMAGE_1946,
IMAGE_1947,
IMAGE_1948,
IMAGE_1949,
IMAGE_1950,
IMAGE_1951,
IMAGE_1952,
IMAGE_1953,
IMAGE_1954,
IMAGE_1955,
IMAGE_1956,
IMAGE_1957,
IMAGE_1958,
IMAGE_1959,
IMAGE_1960,
IMAGE_1961,
IMAGE_1962,
IMAGE_1963,
IMAGE_1964,
IMAGE_1965,
IMAGE_1966,
IMAGE_1967,
IMAGE_1968,
IMAGE_1969,
IMAGE_1970,
IMAGE_1971,
IMAGE_1972,
IMAGE_1973,
IMAGE_1974,
IMAGE_1975,
IMAGE_1976,
IMAGE_1977,
IMAGE_1978,
IMAGE_1979,
IMAGE_1980,
IMAGE_1981,
IMAGE_1982,
IMAGE_1983,
IMAGE_1984,
IMAGE_1985,
IMAGE_1986,
IMAGE_1987,
IMAGE_1988,
IMAGE_1989,
IMAGE_1990,
IMAGE_1991,
IMAGE_1992,
IMAGE_1993,
IMAGE_1994,
IMAGE_1995,
IMAGE_1996,
IMAGE_1997,
IMAGE_1998,
IMAGE_1999,
IMAGE_2000,
IMAGE_2001,
IMAGE_2002,
IMAGE_2003,
IMAGE_2004,
IMAGE_2005,
IMAGE_2006,
IMAGE_2007,
IMAGE_2008,
IMAGE_2009,
IMAGE_2010,
IMAGE_2011,
IMAGE_2012,
IMAGE_2013,
IMAGE_2014,
IMAGE_2015,
IMAGE_2016,
IMAGE_2017,
IMAGE_2018,
IMAGE_2019,
IMAGE_2020,
IMAGE_2021,
IMAGE_2022,
IMAGE_2023,
IMAGE_2024,
IMAGE_2025,
IMAGE_2026,
IMAGE_2027,
IMAGE_2028,
IMAGE_2029,
IMAGE_2030,
IMAGE_2031,
IMAGE_2032,
IMAGE_2033,
IMAGE_2034,
IMAGE_2035,
IMAGE_2036,
IMAGE_2037,
IMAGE_2038,
IMAGE_2039,
IMAGE_2040,
IMAGE_2041,
IMAGE_2042,
IMAGE_2043,
IMAGE_2044,
IMAGE_2045,
IMAGE_2046,
IMAGE_2047,
IMAGE_2048,
IMAGE_2049,
IMAGE_2050,
IMAGE_2051,
IMAGE_2052,
IMAGE_2053,
IMAGE_2054,
IMAGE_2055,
IMAGE_2056,
IMAGE_2057,
IMAGE_2058,
IMAGE_2059,
IMAGE_2060,
IMAGE_2061,
IMAGE_2062,
IMAGE_2063,
IMAGE_2064,
IMAGE_2065,
IMAGE_2066,
IMAGE_2067,
IMAGE_2068,
IMAGE_2069,
IMAGE_2070,
IMAGE_2071,
IMAGE_2072,
IMAGE_2073,
IMAGE_2074,
IMAGE_2075,
IMAGE_2076,
IMAGE_2077,
IMAGE_2078,
IMAGE_2079,
IMAGE_2080,
IMAGE_2081,
IMAGE_2082,
IMAGE_2083,
IMAGE_SMOKE_0,
IMAGE_FIRE_0,
IMAGE_SMOKE_1,
IMAGE_FIRE_1,
IMAGE_SMOKE_2,
IMAGE_FIRE_2,
IMAGE_SMOKE_3,
IMAGE_FIRE_3,
IMAGE_SMOKE_4,
IMAGE_FIRE_4,
IMAGE_SMOKE_5,
IMAGE_FIRE_5,
IMAGE_SMOKE_6,
IMAGE_FIRE_6,
IMAGE_SMOKE_7,
IMAGE_FIRE_7,
IMAGE_SMOKE_8,
IMAGE_FIRE_8,
IMAGE_SMOKE_9,
IMAGE_FIRE_9,
IMAGE_SMOKE_10,
IMAGE_FIRE_10,
IMAGE_SMOKE_11,
IMAGE_FIRE_11,
IMAGE_SMOKE_12,
IMAGE_FIRE_12,
IMAGE_SMOKE_13,
IMAGE_FIRE_13,
IMAGE_SMOKE_14,
IMAGE_FIRE_14,
IMAGE_2114,
IMAGE_2115,
IMAGE_2116,
IMAGE_2117,
IMAGE_2118,
IMAGE_2119,
IMAGE_2120,
IMAGE_2121,
IMAGE_2122,
IMAGE_2123,
IMAGE_2124,
IMAGE_2125,
IMAGE_2126,
IMAGE_2127,
IMAGE_FLAREORANGELINE,
IMAGE_2129,
IMAGE_2130,
IMAGE_2131,
IMAGE_2132,
IMAGE_2133,
IMAGE_FLAREBLUEROUND,
IMAGE_FLAREBLUELINE,
IMAGE_FLAREWHITEROUND,
IMAGE_2137,
IMAGE_2138,
IMAGE_2139,
IMAGE_2140,
IMAGE_2141,
IMAGE_2142,
IMAGE_2143,
IMAGE_2144,
IMAGE_2145,
IMAGE_2146,
IMAGE_2147,
IMAGE_2148,
IMAGE_2149,
IMAGE_2150,
IMAGE_2151,
IMAGE_2152,
IMAGE_2153,
IMAGE_2154,
IMAGE_2155,
IMAGE_2156,
IMAGE_2157,
IMAGE_2158,
IMAGE_2159,
IMAGE_2160,
IMAGE_ROCKETAMMO,
IMAGE_PLAINMINEAMMO,
IMAGE_GRENADEAMMO,
IMAGE_MAGAMMO,
IMAGE_GLAMMO,
IMAGE_KNIFEAMMO,
IMAGE_SHOTAMMO,
IMAGE_IMPACT1,
IMAGE_IMPACT2,
IMAGE_IMPACT3,
IMAGE_IMPACT4,
IMAGE_IMPACTMULTI,
IMAGE_IMPACTREDBRICK2,
IMAGE_IMPACTBRICK2,
IMAGE_IMPACTBRICK3,
IMAGE_SMOKE1,
IMAGE_SMOKE2,
IMAGE_SMOKE3,
IMAGE_SMOKE4,
IMAGE_SMOKE5,
IMAGE_SMOKE6,
IMAGE_SMOKEBALLS1,
IMAGE_SMOKEBALLS2,
IMAGE_SMOKEBALLS3,
IMAGE_SMOKEBALLS4,
IMAGE_SMOKEBALLS5,
IMAGE_MONITOR_BOND,
IMAGE_MONITOR_LOCATION,
IMAGE_MONITOR_BEGINARMING,
IMAGE_MONITOR_TARGET,
IMAGE_MONITOR_SEVERNAYA,
IMAGE_MONITOR_BREAKTARGET,
IMAGE_MONITOR_AIMER,
IMAGE_MONITOR_EARTH,
IMAGE_MONITOR_DESKTOPBANG,
IMAGE_MONITOR_HEATMAP,
IMAGE_MONITOR_3DMATH,
IMAGE_MONITOR_2DMATH,
IMAGE_MONITOR_SATELLITE,
IMAGE_MONITOR_CUBE1,
IMAGE_MONITOR_EARTHFULL2,
IMAGE_MONITOR_BLUESTARS,
IMAGE_MONITOR_GALAXY1,
IMAGE_MONITOR_GALAXY2,
IMAGE_MONITOR_TARGETEARTH,
IMAGE_MONITOR_GALAXY3,
IMAGE_MONITOR_SKATEBOARD1,
IMAGE_MONITOR_SKATEBOARD2,
IMAGE_MONITOR_SKATEBOARD3,
IMAGE_MONITOR_TALK1,
IMAGE_MONITOR_TALK2,
IMAGE_MONITOR_TALK3,
IMAGE_MONITOR_TALK4,
IMAGE_MONITOR_WORLDMAP,
IMAGE_MONITOR_CUBE2,
IMAGE_MONITOR_CUBE3,
IMAGE_MONITOR_CUBE4,
IMAGE_MONITOR_SKATEBOARD4,
IMAGE_MONITOR_FIST1,
IMAGE_MONITOR_FIST2,
IMAGE_MONITOR_FIST3,
IMAGE_MONITOR_FIST4,
IMAGE_MONITOR_SINE,
IMAGE_MONITOR_TEXT,
IMAGE_MONITOR_BARS,
IMAGE_MONITOR_SQUARES,
IMAGE_MONITOR_STATIC,
IMAGE_CLOUDS_GRAYSCALE,
IMAGE_FLARENOVA,
IMAGE_WHITEBOX,
IMAGE_9MMAMMO,
IMAGE_RIFLEAMMO,
IMAGE_GGAMMO,
IMAGE_MINEAMMO,
IMAGE_PROXAMMO,
IMAGE_CROSSHAIR1,
IMAGE_CROSSHAIR2,
IMAGE_TIMEAMMO,
IMAGE_2239,
IMAGE_2240,
IMAGE_2241,
IMAGE_2242,
IMAGE_2243,
IMAGE_2244,
IMAGE_2245,
IMAGE_2246,
IMAGE_2247,
IMAGE_2248,
IMAGE_2249,
IMAGE_2250,
IMAGE_2251,
IMAGE_2252,
IMAGE_2253,
IMAGE_2254,
IMAGE_2255,
IMAGE_2256,
IMAGE_2257,
IMAGE_2258,
IMAGE_2259,
IMAGE_2260,
IMAGE_2261,
IMAGE_2262,
IMAGE_MONITOR_TRIANGLE,
IMAGE_2264,
IMAGE_2265,
IMAGE_2266,
IMAGE_2267,
IMAGE_2268,
IMAGE_2269,
IMAGE_2270,
IMAGE_2271,
IMAGE_2272,
IMAGE_2273,
IMAGE_2274,
IMAGE_2275,
IMAGE_2276,
IMAGE_2277,
IMAGE_2278,
IMAGE_2279,
IMAGE_2280,
IMAGE_2281,
IMAGE_2282,
IMAGE_2283,
IMAGE_2284,
IMAGE_2285,
IMAGE_2286,
IMAGE_2287,
IMAGE_2288,
IMAGE_2289,
IMAGE_2290,
IMAGE_2291,
IMAGE_2292,
IMAGE_2293,
IMAGE_2294,
IMAGE_2295,
IMAGE_2296,
IMAGE_2297,
IMAGE_2298,
IMAGE_2299,
IMAGE_2300,
IMAGE_2301,
IMAGE_2302,
IMAGE_2303,
IMAGE_2304,
IMAGE_2305,
IMAGE_2306,
IMAGE_2307,
IMAGE_2308,
IMAGE_2309,
IMAGE_2310,
IMAGE_2311,
IMAGE_2312,
IMAGE_2313,
IMAGE_2314,
IMAGE_2315,
IMAGE_2316,
IMAGE_2317,
IMAGE_2318,
IMAGE_2319,
IMAGE_2320,
IMAGE_2321,
IMAGE_2322,
IMAGE_2323,
IMAGE_2324,
IMAGE_2325,
IMAGE_2326,
IMAGE_2327,
IMAGE_2328,
IMAGE_2329,
IMAGE_2330,
IMAGE_2331,
IMAGE_2332,
IMAGE_2333,
IMAGE_2334,
IMAGE_2335,
IMAGE_2336,
IMAGE_2337,
IMAGE_2338,
IMAGE_2339,
IMAGE_2340,
IMAGE_2341,
IMAGE_2342,
IMAGE_2343,
IMAGE_2344,
IMAGE_2345,
IMAGE_2346,
IMAGE_2347,
IMAGE_2348,
IMAGE_2349,
IMAGE_2350,
IMAGE_2351,
IMAGE_2352,
IMAGE_2353,
IMAGE_2354,
IMAGE_2355,
IMAGE_2356,
IMAGE_2357,
IMAGE_2358,
IMAGE_2359,
IMAGE_2360,
IMAGE_2361,
IMAGE_2362,
IMAGE_2363,
IMAGE_2364,
IMAGE_2365,
IMAGE_2366,
IMAGE_2367,
IMAGE_2368,
IMAGE_2369,
IMAGE_2370,
IMAGE_2371,
IMAGE_2372,
IMAGE_2373,
IMAGE_2374,
IMAGE_2375,
IMAGE_2376,
IMAGE_2377,
IMAGE_2378,
IMAGE_2379,
IMAGE_2380,
IMAGE_2381,
IMAGE_2382,
IMAGE_2383,
IMAGE_2384,
IMAGE_2385,
IMAGE_2386,
IMAGE_2387,
IMAGE_2388,
IMAGE_2389,
IMAGE_2390,
IMAGE_2391,
IMAGE_2392,
IMAGE_2393,
IMAGE_2394,
IMAGE_2395,
IMAGE_2396,
IMAGE_2397,
IMAGE_2398,
IMAGE_2399,
IMAGE_2400,
IMAGE_2401,
IMAGE_2402,
IMAGE_2403,
IMAGE_2404,
IMAGE_2405,
IMAGE_2406,
IMAGE_2407,
IMAGE_2408,
IMAGE_2409,
IMAGE_2410,
IMAGE_2411,
IMAGE_2412,
IMAGE_2413,
IMAGE_2414,
IMAGE_2415,
IMAGE_2416,
IMAGE_2417,
IMAGE_2418,
IMAGE_2419,
IMAGE_2420,
IMAGE_2421,
IMAGE_2422,
IMAGE_2423,
IMAGE_2424,
IMAGE_2425,
IMAGE_2426,
IMAGE_2427,
IMAGE_2428,
IMAGE_2429,
IMAGE_2430,
IMAGE_2431,
IMAGE_2432,
IMAGE_2433,
IMAGE_2434,
IMAGE_2435,
IMAGE_2436,
IMAGE_2437,
IMAGE_2438,
IMAGE_2439,
IMAGE_2440,
IMAGE_2441,
IMAGE_2442,
IMAGE_2443,
IMAGE_2444,
IMAGE_2445,
IMAGE_2446,
IMAGE_2447,
IMAGE_2448,
IMAGE_2449,
IMAGE_2450,
IMAGE_2451,
IMAGE_2452,
IMAGE_2453,
IMAGE_2454,
IMAGE_2455,
IMAGE_2456,
IMAGE_2457,
IMAGE_2458,
IMAGE_2459,
IMAGE_2460,
IMAGE_2461,
IMAGE_2462,
IMAGE_2463,
IMAGE_TANKAMMO,
IMAGE_2465,
IMAGE_2466,
IMAGE_2467,
IMAGE_2468,
IMAGE_2469,
IMAGE_2470,
IMAGE_2471,
IMAGE_2472,
IMAGE_2473,
IMAGE_2474,
IMAGE_2475,
IMAGE_2476,
IMAGE_2477,
IMAGE_2478,
IMAGE_2479,
IMAGE_2480,
IMAGE_2481,
IMAGE_2482,
IMAGE_2483,
IMAGE_2484,
IMAGE_2485,
IMAGE_2486,
IMAGE_2487,
IMAGE_2488,
IMAGE_2489,
IMAGE_2490,
IMAGE_2491,
IMAGE_2492,
IMAGE_2493,
IMAGE_2494,
IMAGE_2495,
IMAGE_2496,
IMAGE_2497,
IMAGE_2498,
IMAGE_2499,
IMAGE_2500,
IMAGE_2501,
IMAGE_2502,
IMAGE_2503,
IMAGE_2504,
IMAGE_2505,
IMAGE_2506,
IMAGE_2507,
IMAGE_2508,
IMAGE_2509,
IMAGE_2510,
IMAGE_2511,
IMAGE_2512,
IMAGE_2513,
IMAGE_2514,
IMAGE_2515,
IMAGE_2516,
IMAGE_2517,
IMAGE_2518,
IMAGE_2519,
IMAGE_2520,
IMAGE_2521,
IMAGE_2522,
IMAGE_2523,
IMAGE_2524,
IMAGE_2525,
IMAGE_2526,
IMAGE_2527,
IMAGE_2528,
IMAGE_2529,
IMAGE_2530,
IMAGE_2531,
IMAGE_2532,
IMAGE_2533,
IMAGE_2534,
IMAGE_2535,
IMAGE_2536,
IMAGE_2537,
IMAGE_ARCHIVES_U,
IMAGE_ARCHIVES_L,
IMAGE_CONTROL_U,
IMAGE_CONTROL_L,
IMAGE_FACILITY_U,
IMAGE_FACILITY_L,
IMAGE_AZTEC_U,
IMAGE_AZTEC_L,
IMAGE_EGYPT_U,
IMAGE_EGYPT_L,
IMAGE_CAVERNS_U,
IMAGE_CAVERNS_L,
IMAGE_CRADLE_U,
IMAGE_CRADLE_L,
IMAGE_DAM_U,
IMAGE_DAM_L,
IMAGE_DEPOT_U,
IMAGE_DEPOT_L,
IMAGE_FRIGATE_U,
IMAGE_FRIGATE_L,
IMAGE_JUNGLE_U,
IMAGE_JUNGLE_L,
IMAGE_STREETS_U,
IMAGE_STREETS_L,
IMAGE_RUNWAY_U,
IMAGE_RUNWAY_L,
IMAGE_BUNKERI_U,
IMAGE_BUNKERI_L,
IMAGE_BUNKERII_U,
IMAGE_BUNKERII_L,
IMAGE_SURFACEI_U,
IMAGE_SURFACEI_L,
IMAGE_SURFACEII_U,
IMAGE_SURFACEII_L,
IMAGE_SILO_U,
IMAGE_SILO_L,
IMAGE_STATUE_U,
IMAGE_STATUE_L,
IMAGE_TRAIN_U,
IMAGE_TRAIN_L,
IMAGE_MP_ARCHIVES,
IMAGE_MP_CONTROL,
IMAGE_MP_FACILITY,
IMAGE_MP_AZTEC,
IMAGE_MP_CAVERNS,
IMAGE_MP_CRADLE,
IMAGE_MP_EGYPT,
IMAGE_MP_DAM,
IMAGE_MP_DEPOT,
IMAGE_MP_FRIGATE,
IMAGE_MP_JUNGLE,
IMAGE_MP_STREETS,
IMAGE_MP_RUNWAY,
IMAGE_MP_BUNKER,
IMAGE_MP_BUNKER2,
IMAGE_MP_SURFACE,
IMAGE_MP_SURFACE2,
IMAGE_MP_SILO,
IMAGE_MP_STATUE,
IMAGE_MP_TRAIN,
IMAGE_MI6_UL,
IMAGE_MI6_UR,
IMAGE_MI6_LL,
IMAGE_MI6_LR,
IMAGE_BROSNAN_UL,
IMAGE_BROSNAN_UR,
IMAGE_BROSNAN_LL,
IMAGE_BROSNAN_LR,
IMAGE_CONNERY_UL,
IMAGE_CONNERY_UR,
IMAGE_CONNERY_LL,
IMAGE_CONNERY_LR,
IMAGE_MOORE_UL,
IMAGE_MOORE_UR,
IMAGE_MOORE_LL,
IMAGE_MOORE_LR,
IMAGE_DALTON_UL,
IMAGE_DALTON_UR,
IMAGE_DALTON_LL,
IMAGE_DALTON_LR,
IMAGE_CLASSIFIED_L,
IMAGE_CLASSIFIED_R,
IMAGE_CONFIDENTIAL_L,
IMAGE_CONFIDENTIAL_R,
IMAGE_EYESONLY_L,
IMAGE_EYESONLY_R,
IMAGE_FORYOUR_L,
IMAGE_FORYOUR_R,
IMAGE_FOLDERTEX,
IMAGE_OHMSS_L,
IMAGE_OHMSS_R,
IMAGE_PAPERTEX,
IMAGE_MI6,
IMAGE_DOT,
IMAGE_BORIS_UL,
IMAGE_BORIS_UR,
IMAGE_BORIS_LL,
IMAGE_BORIS_LR,
IMAGE_OURUMOV_UL,
IMAGE_OURUMOV_UR,
IMAGE_OURUMOV_LL,
IMAGE_OURUMOV_LR,
IMAGE_TREVELYAN_UL,
IMAGE_TREVELYAN_UR,
IMAGE_TREVELYAN_LL,
IMAGE_TREVELYAN_LR,
IMAGE_VALENTIN_UL,
IMAGE_VALENTIN_UR,
IMAGE_VALENTIN_LL,
IMAGE_VALENTIN_LR,
IMAGE_XENIA_UL,
IMAGE_XENIA_UR,
IMAGE_XENIA_LL,
IMAGE_XENIA_LR,
IMAGE_NATALYA_UL,
IMAGE_NATALYA_UR,
IMAGE_NATALYA_LL,
IMAGE_NATALYA_LR,
IMAGE_BARON_UL,
IMAGE_BARON_UR,
IMAGE_BARON_LL,
IMAGE_BARON_LR,
IMAGE_JAWS_UL,
IMAGE_JAWS_UR,
IMAGE_JAWS_LL,
IMAGE_JAWS_LR,
IMAGE_MAYDAY_UL,
IMAGE_MAYDAY_UR,
IMAGE_MAYDAY_LL,
IMAGE_MAYDAY_LR,
IMAGE_ODDJOB_UL,
IMAGE_ODDJOB_UR,
IMAGE_ODDJOB_LL,
IMAGE_ODDJOB_LR,
IMAGE_2672,
IMAGE_2673,
IMAGE_2674,
IMAGE_2675,
IMAGE_2676,
IMAGE_2677,
IMAGE_2678,
IMAGE_2679,
IMAGE_2680,
IMAGE_2681,
IMAGE_RANDOM_UL,
IMAGE_RANDOM_UR,
IMAGE_RANDOM_LL,
IMAGE_RANDOM_LR,
IMAGE_MP_TEMPLE,
IMAGE_MP_BASEMENT,
IMAGE_MP_COMPLEX,
IMAGE_MP_CAVES,
IMAGE_2690,
IMAGE_MISHKIN_LL,
IMAGE_MISHKIN_LR,
IMAGE_MISHKIN_UR,
IMAGE_MISHKIN_UL,
IMAGE_MP_RANDOM,
IMAGE_2696,
IMAGE_2697,
# 1363 "src/bondconstants.h" 2
    IMAGEIDS_COUNT
} IMAGEIDS;



typedef enum IMGAGESIZES
{
# 1 "./assets/images.def" 1
IMAGE_COPYICON_SIZE = 0x754,
IMAGE_X_SIZE = 0x16A,
IMAGE_DELICON_SIZE = 0x974,
IMAGE_SELECTFILE_SIZE = 0x38B,
IMAGE_CHECK_SIZE = 0x153,
IMAGE_TRAIN715_5264_SIZE = 0x25E,
IMAGE_TRAIN715_SIZE = 0x3BE,
IMAGE_VERTICALSTRIPES_BW_SIZE = 0x1A,
IMAGE_VERTICALSTRIPES_yELLOW_SIZE = 0x1A,
IMAGE_STOMEMAN_L_SIZE = 0x37F,
IMAGE_STOMEMAN_R_SIZE = 0x391,
IMAGE_11_SIZE = 0xE0,
IMAGE_7SEG_DASH_SIZE = 0x3A4,
IMAGE_7SEG_0_SIZE = 0x4A3,
IMAGE_7SEG_3_SIZE = 0x477,
IMAGE_7SEG_5_SIZE = 0x471,
IMAGE_7SEG_8_SIZE = 0x4AC,
IMAGE_17_SIZE = 0x3DA,
IMAGE_VERTICALSTRIPES_RED_SIZE = 0x1A,
IMAGE_VERTICALSTRIPES_RED2_SIZE = 0x1A,
IMAGE_GRAVELGREY_SIZE = 0x619,
IMAGE_GRAVELDETAIL_SIZE = 0x135,
IMAGE_TARDETAIL_SIZE = 0x5F5,
IMAGE_TAZER_TEXT_SIZE = 0x35F,
IMAGE_BOY_TEXT_SIZE = 0x350,
IMAGE_25_SIZE = 0x60C,
IMAGE_BARRELSPECULAR_SIZE = 0xC5,
IMAGE_BLACKGUNFRAME_L_SIZE = 0xA6,
IMAGE_BLACKGUNFRAME_R_SIZE = 0xB5,
IMAGE_BUZZERSWITCH_SIZE = 0x3DD,
IMAGE_ALARM_SIZE = 0x3FC,
IMAGE_BELL_SIZE = 0x308,
IMAGE_BELL2_SIZE = 0x306,
IMAGE_AMMOCRATE1_SIZE = 0x64A,
IMAGE_AMMOCRATE2_SIZE = 0x5CC,
IMAGE_AMMOCRATEBANDED1_SIZE = 0x6A0,
IMAGE_AMMOCRATEBANDED2_SIZE = 0x6C6,
IMAGE_CRATEROPE_SIZE = 0x3EB,
IMAGE_12GCART_SIZE = 0x113,
IMAGE_AMMOTEXT765_SIZE = 0x227,
IMAGE_AMMOTEXTGL_SIZE = 0x219,
IMAGE_AMMOTEXTGRENADES_SIZE = 0x22B,
IMAGE_AMMOTEXT9MM_SIZE = 0x222,
IMAGE_43_SIZE = 0x53D,
IMAGE_44_SIZE = 0x55C,
IMAGE_45_SIZE = 0x3EF,
IMAGE_46_SIZE = 0x19A,
IMAGE_47_SIZE = 0x454,
IMAGE_48_SIZE = 0x6BE,
IMAGE_49_SIZE = 0x27C,
IMAGE_50_SIZE = 0x5E6,
IMAGE_51_SIZE = 0x338,
IMAGE_52_SIZE = 0x4D0,
IMAGE_53_SIZE = 0x5A0,
IMAGE_WHEELSIDE_SIZE = 0x492,
IMAGE_WHEELSIDE_SM_SIZE = 0x318,
IMAGE_56_SIZE = 0x10E,
IMAGE_57_SIZE = 0xF1,
IMAGE_58_SIZE = 0x119,
IMAGE_59_SIZE = 0x2A6,
IMAGE_60_SIZE = 0x1A0,
IMAGE_61_SIZE = 0x22,
IMAGE_62_SIZE = 0x114,
IMAGE_63_SIZE = 0x9C,
IMAGE_64_SIZE = 0x3A5,
IMAGE_65_SIZE = 0x378,
IMAGE_66_SIZE = 0x344,
IMAGE_67_SIZE = 0xF7,
IMAGE_68_SIZE = 0x39B,
IMAGE_69_SIZE = 0x1AD,
IMAGE_70_SIZE = 0x1E4,
IMAGE_71_SIZE = 0x2D5,
IMAGE_72_SIZE = 0xC6,
IMAGE_73_SIZE = 0xC7,
IMAGE_74_SIZE = 0x2B3,
IMAGE_75_SIZE = 0x1A2,
IMAGE_76_SIZE = 0x190,
IMAGE_GAZ_SIDE_SIZE = 0x5E1,
IMAGE_78_SIZE = 0x2AF,
IMAGE_79_SIZE = 0xDE,
IMAGE_GAZ_DOOR_SIZE = 0x1F4,
IMAGE_81_SIZE = 0x5E4,
IMAGE_82_SIZE = 0x1B9,
IMAGE_83_SIZE = 0x5F3,
IMAGE_84_SIZE = 0x227,
IMAGE_DARK_CONCRETE_WALL_SIZE = 0x2D7,
IMAGE_DARK_CONCRETE_WALL_LIT_SIZE = 0x3AE,
IMAGE_HOLE_IN_SNOW_DECAL_SIZE = 0x2F9,
IMAGE_AZTEC_88_SIZE = 0xC2A,
IMAGE_AZTEC_89_SIZE = 0xBFD,
IMAGE_AZTEC_90_SIZE = 0xC31,
IMAGE_AZTEC_91_SIZE = 0xB18,
IMAGE_AZTEC_92_SIZE = 0xBC6,
IMAGE_AZTEC_93_SIZE = 0xC68,
IMAGE_AZTEC_94_SIZE = 0xCA5,
IMAGE_AZTEC_95_SIZE = 0xBD4,
IMAGE_AZTEC_96_SIZE = 0xB7D,
IMAGE_AZTEC_97_SIZE = 0xBD8,
IMAGE_AZTEC_98_SIZE = 0xC41,
IMAGE_AZTEC_99_SIZE = 0xB9F,
IMAGE_AZTEC_100_SIZE = 0xBE6,
IMAGE_AZTEC_101_SIZE = 0xCEE,
IMAGE_AZTEC_102_SIZE = 0xD06,
IMAGE_AZTEC_103_SIZE = 0xBA1,
IMAGE_AZTEC_104_SIZE = 0xB27,
IMAGE_AZTEC_105_SIZE = 0xD18,
IMAGE_AZTEC_106_SIZE = 0xCCA,
IMAGE_AZTEC_107_SIZE = 0xA5A,
IMAGE_AZTEC_108_SIZE = 0x876,
IMAGE_AZTEC_109_SIZE = 0x8AA,
IMAGE_AZTEC_110_SIZE = 0x8CF,
IMAGE_AZTEC_111_SIZE = 0x794,
IMAGE_AZTEC_112_SIZE = 0xA7A,
IMAGE_AZTEC_113_SIZE = 0xC92,
IMAGE_AZTEC_114_SIZE = 0xB8A,
IMAGE_AZTEC_115_SIZE = 0xA53,
IMAGE_AZTEC_116_SIZE = 0xBF0,
IMAGE_AZTEC_117_SIZE = 0xD0C,
IMAGE_AZTEC_118_SIZE = 0xCCC,
IMAGE_AZTEC_119_SIZE = 0x9F8,
IMAGE_AZTEC_120_SIZE = 0xCC1,
IMAGE_AZTEC_121_SIZE = 0xD3B,
IMAGE_AZTEC_122_SIZE = 0xD6D,
IMAGE_AZTEC_123_SIZE = 0xBF0,
IMAGE_AZTEC_124_SIZE = 0x980,
IMAGE_AZTEC_125_SIZE = 0xB91,
IMAGE_AZTEC_126_SIZE = 0xCA1,
IMAGE_AZTEC_127_SIZE = 0xA82,
IMAGE_AZTEC_128_SIZE = 0xC7A,
IMAGE_AZTEC_129_SIZE = 0xD4B,
IMAGE_AZTEC_130_SIZE = 0xD85,
IMAGE_AZTEC_131_SIZE = 0xC76,
IMAGE_AZTEC_132_SIZE = 0xBA9,
IMAGE_AZTEC_133_SIZE = 0xD5A,
IMAGE_AZTEC_134_SIZE = 0xD50,
IMAGE_AZTEC_135_SIZE = 0xC35,
IMAGE_AZTEC_136_SIZE = 0xA8D,
IMAGE_AZTEC_137_SIZE = 0xD26,
IMAGE_AZTEC_138_SIZE = 0xB25,
IMAGE_AZTEC_139_SIZE = 0x899,
IMAGE_AZTEC_140_SIZE = 0x477,
IMAGE_AZTEC_141_SIZE = 0x485,
IMAGE_AZTEC_142_SIZE = 0x456,
IMAGE_AZTEC_143_SIZE = 0x840,
IMAGE_AZTEC_144_SIZE = 0x84A,
IMAGE_AZTEC_145_SIZE = 0x86B,
IMAGE_AZTEC_146_SIZE = 0x7C2,
IMAGE_CROSSMEMBER_SIZE = 0x212,
IMAGE_RAILING_BLUE_SIZE = 0x24B,
IMAGE_RAILING_3BAR_SIZE = 0x149,
IMAGE_RAILING_2BAR_SIZE = 0x125,
IMAGE_RAILING_BLUE_LIGHT_SIZE = 0x310,
IMAGE_RAILING_3BAR_LIGHT_SIZE = 0x137,
IMAGE_RAILNG_VERTICAL_SIZE = 0x127,
IMAGE_RAILING_3BAR_MID_SIZE = 0x13B,
IMAGE_RAILING_GLASS_SIZE = 0x2C1,
IMAGE_RAILING_ARECEBO_SIZE = 0x3BC,
IMAGE_157_SIZE = 0x58E,
IMAGE_158_SIZE = 0x146,
IMAGE_159_SIZE = 0x13B,
IMAGE_160_SIZE = 0x233,
IMAGE_161_SIZE = 0x2E1,
IMAGE_162_SIZE = 0x31C,
IMAGE_163_SIZE = 0x30B,
IMAGE_164_SIZE = 0x33F,
IMAGE_165_SIZE = 0x2F5,
IMAGE_166_SIZE = 0x33E,
IMAGE_167_SIZE = 0x30E,
IMAGE_168_SIZE = 0x353,
IMAGE_169_SIZE = 0x477,
IMAGE_170_SIZE = 0x2F9,
IMAGE_171_SIZE = 0x25B,
IMAGE_172_SIZE = 0xBA,
IMAGE_173_SIZE = 0x2A6,
IMAGE_174_SIZE = 0x3AA,
IMAGE_175_SIZE = 0x23C,
IMAGE_SAW_SIZE = 0x7DE,
IMAGE_177_SIZE = 0x271,
IMAGE_178_SIZE = 0x2F3,
IMAGE_179_SIZE = 0x314,
IMAGE_180_SIZE = 0x128,
IMAGE_181_SIZE = 0x1F9,
IMAGE_BLOTTER_SIZE = 0x2B3,
IMAGE_183_SIZE = 0x207,
IMAGE_184_SIZE = 0x25B,
IMAGE_BOOK_SPINE_SIZE = 0x549,
IMAGE_186_SIZE = 0x44B,
IMAGE_187_SIZE = 0x37F,
IMAGE_188_SIZE = 0x620,
IMAGE_BUNKERPLAN_TL_SIZE = 0x453,
IMAGE_BUNKERPLAN_TR_SIZE = 0x56C,
IMAGE_BUNKERPLAN_BL_SIZE = 0x597,
IMAGE_BUNKERPLAN_BR_SIZE = 0x516,
IMAGE_193_SIZE = 0x288,
IMAGE_194_SIZE = 0x2C8,
IMAGE_BRICK_SIZE = 0x597,
IMAGE_196_SIZE = 0x47D,
IMAGE_197_SIZE = 0x29F,
IMAGE_198_SIZE = 0x2A3,
IMAGE_199_SIZE = 0x14A,
IMAGE_RUSTYDRUM_END_SIZE = 0x4BA,
IMAGE_WALL_LAMP_SIZE = 0x3FA,
IMAGE_202_SIZE = 0x52F,
IMAGE_203_LIGHT_SIZE = 0x460,
IMAGE_204_SIZE = 0x561,
IMAGE_205_LIGHT_SIZE = 0x4F7,
IMAGE_IMPACTLOTS_SIZE = 0x409,
IMAGE_207_SIZE = 0x643,
IMAGE_208_SIZE = 0xB2B,
IMAGE_209_SIZE = 0xC3E,
IMAGE_210_SIZE = 0x578,
IMAGE_211_SIZE = 0xCF,
IMAGE_212_SIZE = 0x290,
IMAGE_213_SIZE = 0x4D6,
IMAGE_214_SIZE = 0xE0,
IMAGE_215_SIZE = 0x26E,
IMAGE_216_SIZE = 0x14E,
IMAGE_217_SIZE = 0x1E0,
IMAGE_218_SIZE = 0x119,
IMAGE_219_SIZE = 0x3D8,
IMAGE_220_SIZE = 0x409,
IMAGE_221_SIZE = 0x3F2,
IMAGE_222_SIZE = 0x409,
IMAGE_223_SIZE = 0x442,
IMAGE_224_SIZE = 0x43D,
IMAGE_225_SIZE = 0x3F6,
IMAGE_226_SIZE = 0x2B3,
IMAGE_227_SIZE = 0x37F,
IMAGE_228_SIZE = 0x269,
IMAGE_229_SIZE = 0x229,
IMAGE_230_SIZE = 0x383,
IMAGE_231_SIZE = 0x5DA,
IMAGE_232_SIZE = 0x341,
IMAGE_233_SIZE = 0x12E,
IMAGE_234_SIZE = 0x554,
IMAGE_235_SIZE = 0x39D,
IMAGE_236_SIZE = 0x496,
IMAGE_237_SIZE = 0x4E9,
IMAGE_238_SIZE = 0x3C7,
IMAGE_239_SIZE = 0x3DF,
IMAGE_240_SIZE = 0x400,
IMAGE_241_SIZE = 0x245,
IMAGE_242_SIZE = 0x1F6,
IMAGE_243_SIZE = 0x15F,
IMAGE_244_SIZE = 0x478,
IMAGE_245_SIZE = 0x20D,
IMAGE_246_SIZE = 0x2E2,
IMAGE_247_SIZE = 0xC7,
IMAGE_248_SIZE = 0xB4,
IMAGE_249_SIZE = 0x98,
IMAGE_250_SIZE = 0xC7,
IMAGE_251_SIZE = 0x329,
IMAGE_252_LIGHT_SIZE = 0x2A1,
IMAGE_253_SIZE = 0x2CD,
IMAGE_PANEL_LAMP_SIZE = 0x408,
IMAGE_255_LIGHT_SIZE = 0x3EB,
IMAGE_256_LIGHT_SIZE = 0x1CF,
IMAGE_257_SIZE = 0x33D,
IMAGE_258_SIZE = 0x287,
IMAGE_259_SIZE = 0x326,
IMAGE_260_SIZE = 0x380,
IMAGE_261_SIZE = 0x2B1,
IMAGE_262_SIZE = 0x2C4,
IMAGE_263_SIZE = 0x22E,
IMAGE_264_SIZE = 0x584,
IMAGE_265_SIZE = 0x1A8,
IMAGE_266_SIZE = 0x4F,
IMAGE_267_SIZE = 0x558,
IMAGE_268_SIZE = 0x2D7,
IMAGE_269_SIZE = 0x582,
IMAGE_270_SIZE = 0x2B6,
IMAGE_271_SIZE = 0x516,
IMAGE_272_SIZE = 0x4E0,
IMAGE_273_SIZE = 0x799,
IMAGE_274_SIZE = 0x780,
IMAGE_275_SIZE = 0x80C,
IMAGE_276_SIZE = 0x7A7,
IMAGE_277_SIZE = 0x784,
IMAGE_278_SIZE = 0x6B3,
IMAGE_279_SIZE = 0x29A,
IMAGE_280_SIZE = 0x389,
IMAGE_281_SIZE = 0x2EE,
IMAGE_282_SIZE = 0x31F,
IMAGE_283_SIZE = 0x322,
IMAGE_284_SIZE = 0x578,
IMAGE_285_SIZE = 0x31E,
IMAGE_286_SIZE = 0x428,
IMAGE_287_SIZE = 0x4C1,
IMAGE_288_SIZE = 0x479,
IMAGE_289_SIZE = 0x4F9,
IMAGE_290_SIZE = 0x48B,
IMAGE_291_SIZE = 0x3DF,
IMAGE_292_SIZE = 0x546,
IMAGE_293_SIZE = 0x530,
IMAGE_294_SIZE = 0x52B,
IMAGE_295_SIZE = 0x3EF,
IMAGE_296_SIZE = 0x440,
IMAGE_297_SIZE = 0x3A2,
IMAGE_298_SIZE = 0x30F,
IMAGE_299_SIZE = 0x303,
IMAGE_300_SIZE = 0x303,
IMAGE_301_SIZE = 0x374,
IMAGE_302_SIZE = 0x535,
IMAGE_303_SIZE = 0x27F,
IMAGE_304_SIZE = 0xC8,
IMAGE_305_SIZE = 0x23D,
IMAGE_306_SIZE = 0x2AF,
IMAGE_307_SIZE = 0x560,
IMAGE_308_SIZE = 0x385,
IMAGE_309_SIZE = 0x3B5,
IMAGE_310_SIZE = 0x547,
IMAGE_311_SIZE = 0x32F,
IMAGE_312_SIZE = 0x388,
IMAGE_313_SIZE = 0x27C,
IMAGE_314_SIZE = 0x27C,
IMAGE_315_SIZE = 0x245,
IMAGE_316_SIZE = 0x245,
IMAGE_317_SIZE = 0x246,
IMAGE_318_SIZE = 0x2A6,
IMAGE_319_SIZE = 0x141,
IMAGE_320_SIZE = 0x218,
IMAGE_321_SIZE = 0x45B,
IMAGE_322_SIZE = 0x5EB,
IMAGE_323_SIZE = 0x61B,
IMAGE_324_SIZE = 0x62D,
IMAGE_325_SIZE = 0x61F,
IMAGE_326_SIZE = 0x641,
IMAGE_327_SIZE = 0x653,
IMAGE_328_SIZE = 0x5F5,
IMAGE_329_SIZE = 0x344,
IMAGE_330_SIZE = 0x386,
IMAGE_331_SIZE = 0x399,
IMAGE_332_SIZE = 0x360,
IMAGE_333_SIZE = 0x161,
IMAGE_334_SIZE = 0x11A,
IMAGE_335_SIZE = 0x133,
IMAGE_336_SIZE = 0x12A,
IMAGE_337_SIZE = 0x348,
IMAGE_338_SIZE = 0x527,
IMAGE_339_SIZE = 0x39A,
IMAGE_340_SIZE = 0x3EE,
IMAGE_341_SIZE = 0x3D8,
IMAGE_342_SIZE = 0x45B,
IMAGE_343_SIZE = 0xE8,
IMAGE_344_SIZE = 0x122,
IMAGE_345_SIZE = 0x40D,
IMAGE_346_SIZE = 0x3E1,
IMAGE_347_SIZE = 0xC1,
IMAGE_348_SIZE = 0x10F,
IMAGE_349_SIZE = 0x302,
IMAGE_350_SIZE = 0x215,
IMAGE_351_SIZE = 0xD5,
IMAGE_352_SIZE = 0x332,
IMAGE_353_SIZE = 0x1C8,
IMAGE_354_SIZE = 0xA8,
IMAGE_355_SIZE = 0x38A,
IMAGE_356_SIZE = 0x2C0,
IMAGE_357_SIZE = 0x44A,
IMAGE_358_SIZE = 0x3DB,
IMAGE_359_SIZE = 0x35D,
IMAGE_360_SIZE = 0x34D,
IMAGE_361_SIZE = 0x536,
IMAGE_362_SIZE = 0x3C0,
IMAGE_363_SIZE = 0x4C8,
IMAGE_364_SIZE = 0x3CD,
IMAGE_365_SIZE = 0x214,
IMAGE_366_SIZE = 0x2F4,
IMAGE_367_SIZE = 0x3B6,
IMAGE_368_SIZE = 0x3DE,
IMAGE_369_SIZE = 0x26E,
IMAGE_370_SIZE = 0x26E,
IMAGE_371_SIZE = 0x55F,
IMAGE_372_SIZE = 0x58A,
IMAGE_373_SIZE = 0x337,
IMAGE_374_SIZE = 0x3FF,
IMAGE_375_SIZE = 0x3B5,
IMAGE_376_SIZE = 0x3B5,
IMAGE_377_SIZE = 0x332,
IMAGE_378_SIZE = 0x3FC,
IMAGE_379_SIZE = 0x3C4,
IMAGE_380_SIZE = 0x446,
IMAGE_381_SIZE = 0x3FA,
IMAGE_382_SIZE = 0x261,
IMAGE_383_SIZE = 0x535,
IMAGE_384_SIZE = 0x56D,
IMAGE_385_SIZE = 0x44F,
IMAGE_386_SIZE = 0x97D,
IMAGE_387_SIZE = 0x902,
IMAGE_388_SIZE = 0x92D,
IMAGE_389_SIZE = 0x914,
IMAGE_390_SIZE = 0x981,
IMAGE_391_SIZE = 0x8FE,
IMAGE_392_SIZE = 0x8F5,
IMAGE_393_SIZE = 0x878,
IMAGE_394_SIZE = 0x221,
IMAGE_395_SIZE = 0x470,
IMAGE_396_SIZE = 0x526,
IMAGE_397_SIZE = 0x4E6,
IMAGE_398_SIZE = 0x425,
IMAGE_399_SIZE = 0x35B,
IMAGE_400_SIZE = 0x435,
IMAGE_401_SIZE = 0x439,
IMAGE_402_SIZE = 0x33C,
IMAGE_403_SIZE = 0x30C,
IMAGE_404_SIZE = 0x2FF,
IMAGE_405_SIZE = 0x2B3,
IMAGE_406_SIZE = 0x529,
IMAGE_407_SIZE = 0x34D,
IMAGE_408_SIZE = 0x20D,
IMAGE_409_SIZE = 0x39D,
IMAGE_410_SIZE = 0x2F1,
IMAGE_411_SIZE = 0x1B3,
IMAGE_412_SIZE = 0x20E,
IMAGE_413_SIZE = 0x16B,
IMAGE_414_SIZE = 0x207,
IMAGE_415_SIZE = 0x18F,
IMAGE_416_SIZE = 0x1CF,
IMAGE_417_SIZE = 0x1BF,
IMAGE_418_SIZE = 0x6D6,
IMAGE_419_SIZE = 0x3DC,
IMAGE_420_SIZE = 0x6E0,
IMAGE_421_SIZE = 0x223,
IMAGE_422_SIZE = 0x1E3,
IMAGE_423_SIZE = 0x158,
IMAGE_424_SIZE = 0x181,
IMAGE_425_SIZE = 0x453,
IMAGE_426_SIZE = 0x55E,
IMAGE_427_SIZE = 0x5BB,
IMAGE_HANGING_LAMP_SIZE = 0x525,
IMAGE_429_SIZE = 0x72D,
IMAGE_430_SIZE = 0x4BE,
IMAGE_431_SIZE = 0x2F3,
IMAGE_432_SIZE = 0x43C,
IMAGE_433_SIZE = 0x3E6,
IMAGE_434_SIZE = 0x62B,
IMAGE_435_SIZE = 0x31D,
IMAGE_436_SIZE = 0x300,
IMAGE_437_SIZE = 0x64F,
IMAGE_438_SIZE = 0x5F7,
IMAGE_439_SIZE = 0x665,
IMAGE_440_SIZE = 0x3C8,
IMAGE_441_SIZE = 0x3CB,
IMAGE_442_SIZE = 0x366,
IMAGE_443_SIZE = 0x6B6,
IMAGE_444_SIZE = 0x722,
IMAGE_445_SIZE = 0x8A7,
IMAGE_446_SIZE = 0x8CC,
IMAGE_447_SIZE = 0x3F0,
IMAGE_448_SIZE = 0x33B,
IMAGE_449_SIZE = 0x244,
IMAGE_450_SIZE = 0x200,
IMAGE_451_SIZE = 0x1F7,
IMAGE_452_SIZE = 0x473,
IMAGE_453_SIZE = 0x3C4,
IMAGE_454_SIZE = 0x3DB,
IMAGE_455_SIZE = 0x1E0,
IMAGE_456_SIZE = 0x1ED,
IMAGE_457_SIZE = 0x52D,
IMAGE_458_SIZE = 0x47F,
IMAGE_459_SIZE = 0x250,
IMAGE_460_SIZE = 0x286,
IMAGE_461_SIZE = 0xB4,
IMAGE_462_SIZE = 0xA3,
IMAGE_463_SIZE = 0x1E6,
IMAGE_464_SIZE = 0x1AE,
IMAGE_465_SIZE = 0x285,
IMAGE_466_SIZE = 0x223,
IMAGE_467_SIZE = 0x1FA,
IMAGE_468_SIZE = 0x7A2,
IMAGE_469_SIZE = 0x79E,
IMAGE_470_SIZE = 0x140,
IMAGE_471_SIZE = 0x276,
IMAGE_472_SIZE = 0x290,
IMAGE_473_SIZE = 0x264,
IMAGE_474_SIZE = 0x484,
IMAGE_475_SIZE = 0x3C6,
IMAGE_476_SIZE = 0x343,
IMAGE_477_SIZE = 0x33B,
IMAGE_478_SIZE = 0x329,
IMAGE_479_SIZE = 0x3CD,
IMAGE_480_SIZE = 0x141,
IMAGE_481_SIZE = 0x520,
IMAGE_482_SIZE = 0x473,
IMAGE_483_SIZE = 0x4C0,
IMAGE_484_SIZE = 0x381,
IMAGE_485_SIZE = 0x3B0,
IMAGE_486_SIZE = 0x3F2,
IMAGE_487_SIZE = 0x45E,
IMAGE_488_SIZE = 0x490,
IMAGE_489_SIZE = 0x3A9,
IMAGE_490_SIZE = 0x39B,
IMAGE_491_SIZE = 0x362,
IMAGE_492_SIZE = 0x2A8,
IMAGE_493_SIZE = 0x5B9,
IMAGE_494_SIZE = 0xBB,
IMAGE_495_SIZE = 0xBA,
IMAGE_496_SIZE = 0xB7,
IMAGE_497_SIZE = 0x109,
IMAGE_498_SIZE = 0xE1,
IMAGE_499_SIZE = 0x49E,
IMAGE_500_SIZE = 0x473,
IMAGE_501_SIZE = 0x35C,
IMAGE_502_SIZE = 0x337,
IMAGE_503_SIZE = 0x27E,
IMAGE_504_SIZE = 0x248,
IMAGE_505_SIZE = 0x23B,
IMAGE_506_SIZE = 0x152,
IMAGE_507_SIZE = 0x2B3,
IMAGE_508_SIZE = 0x188,
IMAGE_509_SIZE = 0x277,
IMAGE_510_SIZE = 0x205,
IMAGE_511_SIZE = 0x1A4,
IMAGE_512_SIZE = 0x9E,
IMAGE_513_SIZE = 0x32C,
IMAGE_514_SIZE = 0x4AF,
IMAGE_515_SIZE = 0x10A,
IMAGE_516_SIZE = 0x138,
IMAGE_517_SIZE = 0x13F,
IMAGE_518_SIZE = 0x4D4,
IMAGE_519_SIZE = 0x147,
IMAGE_520_SIZE = 0x287,
IMAGE_521_SIZE = 0x9D,
IMAGE_522_SIZE = 0x2ED,
IMAGE_523_SIZE = 0x25F,
IMAGE_524_SIZE = 0x10A,
IMAGE_525_SIZE = 0x2E0,
IMAGE_526_SIZE = 0x373,
IMAGE_527_SIZE = 0x1AA,
IMAGE_528_SIZE = 0x2ED,
IMAGE_529_SIZE = 0x14B,
IMAGE_530_SIZE = 0x484,
IMAGE_531_SIZE = 0x2EA,
IMAGE_532_SIZE = 0x45,
IMAGE_533_SIZE = 0x1E4,
IMAGE_534_SIZE = 0x3C4,
IMAGE_535_SIZE = 0x430,
IMAGE_536_SIZE = 0x325,
IMAGE_537_SIZE = 0x296,
IMAGE_538_SIZE = 0x497,
IMAGE_539_SIZE = 0x33C,
IMAGE_540_SIZE = 0x2AF,
IMAGE_541_SIZE = 0xF9,
IMAGE_542_SIZE = 0x416,
IMAGE_543_SIZE = 0x343,
IMAGE_544_SIZE = 0x5EC,
IMAGE_545_SIZE = 0x764,
IMAGE_546_SIZE = 0x76E,
IMAGE_547_SIZE = 0x72B,
IMAGE_548_SIZE = 0x42C,
IMAGE_549_SIZE = 0x3C8,
IMAGE_550_SIZE = 0x40B,
IMAGE_551_SIZE = 0x6E7,
IMAGE_552_SIZE = 0x68F,
IMAGE_553_SIZE = 0x602,
IMAGE_554_SIZE = 0x602,
IMAGE_555_SIZE = 0x5F5,
IMAGE_556_SIZE = 0x6C6,
IMAGE_557_SIZE = 0x5F9,
IMAGE_558_SIZE = 0x6AB,
IMAGE_559_SIZE = 0x768,
IMAGE_560_SIZE = 0x66C,
IMAGE_561_SIZE = 0x62D,
IMAGE_562_SIZE = 0x636,
IMAGE_563_SIZE = 0x601,
IMAGE_564_SIZE = 0x6A8,
IMAGE_565_SIZE = 0x69F,
IMAGE_566_SIZE = 0x646,
IMAGE_567_SIZE = 0x17A,
IMAGE_568_SIZE = 0x7ED,
IMAGE_569_SIZE = 0x736,
IMAGE_570_SIZE = 0x76F,
IMAGE_571_SIZE = 0x802,
IMAGE_572_SIZE = 0x7EC,
IMAGE_573_SIZE = 0x770,
IMAGE_574_SIZE = 0x701,
IMAGE_575_SIZE = 0x7EE,
IMAGE_576_SIZE = 0x363,
IMAGE_577_SIZE = 0x4BE,
IMAGE_578_SIZE = 0x491,
IMAGE_579_SIZE = 0x1F4,
IMAGE_580_SIZE = 0xC9,
IMAGE_MONITOR_EARTHTEXT_SIZE = 0x5FE,
IMAGE_MONITOR_SHUTTLE1_SIZE = 0x5F7,
IMAGE_MONITOR_SHUTTLE2_SIZE = 0x60C,
IMAGE_MONITOR_EARTHFULL1_SIZE = 0x605,
IMAGE_585_SIZE = 0x40F,
IMAGE_586_SIZE = 0x400,
IMAGE_587_SIZE = 0x462,
IMAGE_588_SIZE = 0x51C,
IMAGE_589_SIZE = 0x4DD,
IMAGE_590_SIZE = 0x531,
IMAGE_591_SIZE = 0x4AC,
IMAGE_592_SIZE = 0x46A,
IMAGE_593_SIZE = 0x593,
IMAGE_594_SIZE = 0x37B,
IMAGE_595_SIZE = 0x4BA,
IMAGE_596_SIZE = 0x58E,
IMAGE_597_SIZE = 0x1B1,
IMAGE_598_SIZE = 0x561,
IMAGE_599_SIZE = 0x653,
IMAGE_600_SIZE = 0x780,
IMAGE_601_SIZE = 0x651,
IMAGE_602_SIZE = 0x1E1,
IMAGE_603_SIZE = 0x57A,
IMAGE_604_SIZE = 0x525,
IMAGE_605_SIZE = 0x5B5,
IMAGE_606_SIZE = 0x166,
IMAGE_607_SIZE = 0x145,
IMAGE_608_SIZE = 0x69E,
IMAGE_609_SIZE = 0x839,
IMAGE_610_SIZE = 0x57C,
IMAGE_611_SIZE = 0x7DA,
IMAGE_612_SIZE = 0x2BE,
IMAGE_613_SIZE = 0x268,
IMAGE_614_SIZE = 0x29B,
IMAGE_615_SIZE = 0x286,
IMAGE_616_SIZE = 0x26E,
IMAGE_617_SIZE = 0x251,
IMAGE_618_SIZE = 0x39E,
IMAGE_619_SIZE = 0x114,
IMAGE_620_SIZE = 0x263,
IMAGE_621_SIZE = 0x598,
IMAGE_622_SIZE = 0x468,
IMAGE_623_SIZE = 0x47A,
IMAGE_624_SIZE = 0x125,
IMAGE_625_SIZE = 0x1BE,
IMAGE_626_SIZE = 0x10B,
IMAGE_627_SIZE = 0x148,
IMAGE_628_SIZE = 0x140,
IMAGE_629_SIZE = 0x148,
IMAGE_630_SIZE = 0x5B2,
IMAGE_631_SIZE = 0x3E8,
IMAGE_632_SIZE = 0x26D,
IMAGE_633_SIZE = 0x3C1,
IMAGE_634_SIZE = 0x3CD,
IMAGE_635_SIZE = 0x55,
IMAGE_636_SIZE = 0x264,
IMAGE_637_SIZE = 0x366,
IMAGE_638_SIZE = 0x1BF,
IMAGE_639_SIZE = 0x738,
IMAGE_640_SIZE = 0x32C,
IMAGE_641_SIZE = 0x553,
IMAGE_642_SIZE = 0x38C,
IMAGE_643_SIZE = 0x12D,
IMAGE_644_SIZE = 0x2D6,
IMAGE_645_SIZE = 0x1BD,
IMAGE_646_SIZE = 0x4A3,
IMAGE_647_SIZE = 0x199,
IMAGE_648_SIZE = 0x2BD,
IMAGE_649_SIZE = 0x264,
IMAGE_650_SIZE = 0x1FE,
IMAGE_651_SIZE = 0x4EC,
IMAGE_652_SIZE = 0x26C,
IMAGE_653_SIZE = 0x24B,
IMAGE_GLASS3_SIZE = 0x345,
IMAGE_655_SIZE = 0x379,
IMAGE_656_SIZE = 0xA4,
IMAGE_657_SIZE = 0xAB,
IMAGE_GLASS7_SIZE = 0x55,
IMAGE_659_SIZE = 0x401,
IMAGE_660_SIZE = 0x342,
IMAGE_661_SIZE = 0x4D9,
IMAGE_662_SIZE = 0x4B5,
IMAGE_663_SIZE = 0xC4,
IMAGE_664_SIZE = 0xE2,
IMAGE_665_SIZE = 0x5E7,
IMAGE_666_SIZE = 0x3F6,
IMAGE_667_SIZE = 0x3E5,
IMAGE_668_SIZE = 0x3BD,
IMAGE_669_SIZE = 0x43C,
IMAGE_670_SIZE = 0x349,
IMAGE_671_SIZE = 0x439,
IMAGE_672_SIZE = 0x379,
IMAGE_673_SIZE = 0x362,
IMAGE_674_SIZE = 0x36B,
IMAGE_675_SIZE = 0x318,
IMAGE_676_SIZE = 0x2A3,
IMAGE_677_SIZE = 0x81,
IMAGE_678_SIZE = 0x27A,
IMAGE_679_SIZE = 0x4EA,
IMAGE_680_SIZE = 0x5A3,
IMAGE_681_SIZE = 0x580,
IMAGE_682_SIZE = 0x4FE,
IMAGE_683_SIZE = 0x13A,
IMAGE_684_SIZE = 0x1DE,
IMAGE_685_SIZE = 0x2E6,
IMAGE_686_SIZE = 0x2A2,
IMAGE_687_SIZE = 0x313,
IMAGE_688_SIZE = 0x33F,
IMAGE_689_SIZE = 0x21F,
IMAGE_690_SIZE = 0x116,
IMAGE_691_SIZE = 0xA2,
IMAGE_692_SIZE = 0x14D,
IMAGE_693_SIZE = 0x44D,
IMAGE_694_SIZE = 0x251,
IMAGE_695_SIZE = 0x1FC,
IMAGE_696_SIZE = 0x211,
IMAGE_697_SIZE = 0x1C4,
IMAGE_698_SIZE = 0x184,
IMAGE_699_SIZE = 0x21F,
IMAGE_700_SIZE = 0x157,
IMAGE_701_SIZE = 0x33C,
IMAGE_702_SIZE = 0x8A,
IMAGE_703_SIZE = 0x20D,
IMAGE_704_SIZE = 0x208,
IMAGE_705_SIZE = 0x374,
IMAGE_706_SIZE = 0x296,
IMAGE_707_SIZE = 0x1F2,
IMAGE_708_SIZE = 0x15F,
IMAGE_709_SIZE = 0x4BE,
IMAGE_710_SIZE = 0x315,
IMAGE_711_SIZE = 0x4DA,
IMAGE_712_SIZE = 0x24D,
IMAGE_713_SIZE = 0x60C,
IMAGE_714_SIZE = 0xC0,
IMAGE_715_SIZE = 0x16E,
IMAGE_716_SIZE = 0x352,
IMAGE_717_SIZE = 0xFD,
IMAGE_718_SIZE = 0x1F3,
IMAGE_719_SIZE = 0x251,
IMAGE_720_SIZE = 0x21B,
IMAGE_721_SIZE = 0xB1,
IMAGE_722_SIZE = 0x228,
IMAGE_723_SIZE = 0x218,
IMAGE_724_SIZE = 0x353,
IMAGE_725_SIZE = 0x426,
IMAGE_726_SIZE = 0x1C5,
IMAGE_727_SIZE = 0x399,
IMAGE_728_SIZE = 0x3F1,
IMAGE_729_SIZE = 0x1C9,
IMAGE_730_SIZE = 0x198,
IMAGE_731_SIZE = 0x1C7,
IMAGE_732_SIZE = 0x537,
IMAGE_733_SIZE = 0x51B,
IMAGE_734_SIZE = 0x4A3,
IMAGE_735_SIZE = 0x5EE,
IMAGE_736_SIZE = 0x5D8,
IMAGE_737_SIZE = 0x585,
IMAGE_738_SIZE = 0x542,
IMAGE_739_SIZE = 0x582,
IMAGE_740_SIZE = 0x449,
IMAGE_741_SIZE = 0x49B,
IMAGE_742_SIZE = 0x56D,
IMAGE_743_SIZE = 0x5BE,
IMAGE_744_SIZE = 0x5DB,
IMAGE_745_SIZE = 0x404,
IMAGE_746_SIZE = 0x4FE,
IMAGE_747_SIZE = 0x58C,
IMAGE_748_SIZE = 0x563,
IMAGE_749_SIZE = 0x5B2,
IMAGE_750_SIZE = 0x57B,
IMAGE_751_SIZE = 0x48B,
IMAGE_752_SIZE = 0x4AE,
IMAGE_753_SIZE = 0x514,
IMAGE_754_SIZE = 0x59C,
IMAGE_755_SIZE = 0x568,
IMAGE_756_SIZE = 0x5BA,
IMAGE_757_SIZE = 0x591,
IMAGE_758_SIZE = 0x208,
IMAGE_759_SIZE = 0x1FE,
IMAGE_760_SIZE = 0x505,
IMAGE_761_SIZE = 0x39F,
IMAGE_762_SIZE = 0xB2,
IMAGE_763_SIZE = 0xA7,
IMAGE_764_SIZE = 0x67,
IMAGE_765_SIZE = 0x55F,
IMAGE_766_SIZE = 0x1FE,
IMAGE_767_SIZE = 0x321,
IMAGE_768_SIZE = 0x131,
IMAGE_769_SIZE = 0x153,
IMAGE_770_SIZE = 0x482,
IMAGE_771_SIZE = 0x60C,
IMAGE_772_SIZE = 0x259,
IMAGE_773_SIZE = 0x383,
IMAGE_774_SIZE = 0x78,
IMAGE_775_SIZE = 0x4CC,
IMAGE_776_SIZE = 0x543,
IMAGE_777_SIZE = 0x5BC,
IMAGE_778_SIZE = 0x371,
IMAGE_779_SIZE = 0x5C2,
IMAGE_780_SIZE = 0x4E2,
IMAGE_781_SIZE = 0x645,
IMAGE_782_SIZE = 0x53B,
IMAGE_783_SIZE = 0x448,
IMAGE_784_SIZE = 0x1F8,
IMAGE_785_SIZE = 0x128,
IMAGE_786_SIZE = 0x16C,
IMAGE_787_SIZE = 0x3C7,
IMAGE_788_SIZE = 0x23D,
IMAGE_789_SIZE = 0x62C,
IMAGE_790_SIZE = 0x602,
IMAGE_791_SIZE = 0x5DF,
IMAGE_792_SIZE = 0x668,
IMAGE_793_SIZE = 0x678,
IMAGE_794_SIZE = 0x5ED,
IMAGE_795_SIZE = 0x640,
IMAGE_796_SIZE = 0x270,
IMAGE_797_SIZE = 0x24C,
IMAGE_798_SIZE = 0x264,
IMAGE_799_SIZE = 0xE1,
IMAGE_800_SIZE = 0x98,
IMAGE_801_SIZE = 0x1C3,
IMAGE_802_SIZE = 0x29F,
IMAGE_803_SIZE = 0x19F,
IMAGE_804_SIZE = 0x1D0,
IMAGE_805_SIZE = 0x30F,
IMAGE_806_SIZE = 0x3DB,
IMAGE_807_SIZE = 0x145,
IMAGE_808_SIZE = 0x299,
IMAGE_809_SIZE = 0x420,
IMAGE_810_SIZE = 0x258,
IMAGE_811_SIZE = 0x29E,
IMAGE_812_SIZE = 0x335,
IMAGE_813_SIZE = 0x1C9,
IMAGE_814_SIZE = 0x479,
IMAGE_815_SIZE = 0x1DF,
IMAGE_816_SIZE = 0x2D9,
IMAGE_817_SIZE = 0x436,
IMAGE_818_SIZE = 0x263,
IMAGE_819_SIZE = 0x603,
IMAGE_820_SIZE = 0x56F,
IMAGE_821_SIZE = 0x44,
IMAGE_822_SIZE = 0x251,
IMAGE_823_SIZE = 0x184,
IMAGE_824_SIZE = 0x2AB,
IMAGE_825_SIZE = 0x376,
IMAGE_826_SIZE = 0x455,
IMAGE_827_SIZE = 0x536,
IMAGE_828_SIZE = 0x4E8,
IMAGE_829_SIZE = 0x559,
IMAGE_830_SIZE = 0x579,
IMAGE_831_SIZE = 0x69E,
IMAGE_832_SIZE = 0x75A,
IMAGE_833_SIZE = 0x6AB,
IMAGE_834_SIZE = 0x3BD,
IMAGE_835_SIZE = 0x316,
IMAGE_836_SIZE = 0x4D4,
IMAGE_MONITOR_KEYBOARDKEY_SIZE = 0x24E,
IMAGE_838_SIZE = 0x775,
IMAGE_839_SIZE = 0x7AA,
IMAGE_840_SIZE = 0x68B,
IMAGE_841_SIZE = 0x68D,
IMAGE_842_SIZE = 0x57A,
IMAGE_843_SIZE = 0x571,
IMAGE_844_SIZE = 0x2FA,
IMAGE_845_SIZE = 0x46E,
IMAGE_846_SIZE = 0x38E,
IMAGE_847_SIZE = 0x4A3,
IMAGE_848_SIZE = 0xB7,
IMAGE_849_SIZE = 0x5FC,
IMAGE_850_SIZE = 0x30F,
IMAGE_851_SIZE = 0x3C5,
IMAGE_852_SIZE = 0x1CC,
IMAGE_853_SIZE = 0x1F9,
IMAGE_854_SIZE = 0x185,
IMAGE_855_SIZE = 0x2DE,
IMAGE_856_SIZE = 0x28F,
IMAGE_857_SIZE = 0x452,
IMAGE_858_SIZE = 0x31B,
IMAGE_859_SIZE = 0x6FC,
IMAGE_860_SIZE = 0x70A,
IMAGE_861_SIZE = 0x211,
IMAGE_862_SIZE = 0x2D8,
IMAGE_863_SIZE = 0x1BF,
IMAGE_864_SIZE = 0x22D,
IMAGE_865_SIZE = 0x193,
IMAGE_866_SIZE = 0x4E8,
IMAGE_867_SIZE = 0x126,
IMAGE_868_SIZE = 0xFF,
IMAGE_869_SIZE = 0x31C,
IMAGE_870_SIZE = 0x17B,
IMAGE_871_SIZE = 0x160,
IMAGE_872_SIZE = 0x43,
IMAGE_873_SIZE = 0x190,
IMAGE_874_SIZE = 0x2F,
IMAGE_875_SIZE = 0x14E,
IMAGE_876_SIZE = 0x13A,
IMAGE_877_SIZE = 0x2F,
IMAGE_878_SIZE = 0x39,
IMAGE_879_SIZE = 0x51,
IMAGE_880_SIZE = 0x29,
IMAGE_881_SIZE = 0x4B,
IMAGE_882_SIZE = 0x2B,
IMAGE_883_SIZE = 0xDD,
IMAGE_884_SIZE = 0x700,
IMAGE_885_SIZE = 0x248,
IMAGE_886_SIZE = 0x14F,
IMAGE_887_SIZE = 0x16C,
IMAGE_888_SIZE = 0x3F8,
IMAGE_889_SIZE = 0x2B0,
IMAGE_890_SIZE = 0x1BF,
IMAGE_891_SIZE = 0x3B0,
IMAGE_892_SIZE = 0x3E9,
IMAGE_893_SIZE = 0x3D0,
IMAGE_894_SIZE = 0x372,
IMAGE_895_SIZE = 0x5D2,
IMAGE_896_SIZE = 0x711,
IMAGE_897_SIZE = 0x672,
IMAGE_898_SIZE = 0x4CA,
IMAGE_899_SIZE = 0x3EA,
IMAGE_900_SIZE = 0x227,
IMAGE_901_SIZE = 0x3BE,
IMAGE_902_SIZE = 0x354,
IMAGE_903_SIZE = 0x356,
IMAGE_904_SIZE = 0x3B5,
IMAGE_905_SIZE = 0x41D,
IMAGE_906_SIZE = 0x2B9,
IMAGE_907_SIZE = 0x1AD,
IMAGE_908_SIZE = 0x513,
IMAGE_909_SIZE = 0x4C4,
IMAGE_910_SIZE = 0x46F,
IMAGE_911_SIZE = 0x4DD,
IMAGE_912_SIZE = 0x65B,
IMAGE_913_SIZE = 0x397,
IMAGE_914_SIZE = 0x2EF,
IMAGE_915_SIZE = 0x316,
IMAGE_916_SIZE = 0x316,
IMAGE_917_SIZE = 0x98,
IMAGE_918_SIZE = 0x1A8,
IMAGE_919_SIZE = 0x276,
IMAGE_920_SIZE = 0x799,
IMAGE_921_SIZE = 0x29C,
IMAGE_922_SIZE = 0x390,
IMAGE_923_SIZE = 0x3BA,
IMAGE_924_SIZE = 0x8A6,
IMAGE_925_SIZE = 0x33E,
IMAGE_926_SIZE = 0x60C,
IMAGE_927_SIZE = 0x650,
IMAGE_928_SIZE = 0x237,
IMAGE_929_SIZE = 0x35C,
IMAGE_930_SIZE = 0x313,
IMAGE_931_SIZE = 0x1DE,
IMAGE_932_SIZE = 0x1BD,
IMAGE_933_SIZE = 0x269,
IMAGE_934_SIZE = 0x150,
IMAGE_935_SIZE = 0x436,
IMAGE_936_SIZE = 0x166,
IMAGE_937_SIZE = 0x18D,
IMAGE_938_SIZE = 0x201,
IMAGE_939_SIZE = 0x420,
IMAGE_940_SIZE = 0x2D6,
IMAGE_941_SIZE = 0x2DA,
IMAGE_942_SIZE = 0x32F,
IMAGE_943_SIZE = 0x2B9,
IMAGE_944_SIZE = 0x29D,
IMAGE_945_SIZE = 0x1FA,
IMAGE_946_SIZE = 0x27F,
IMAGE_947_SIZE = 0xF1,
IMAGE_948_SIZE = 0x5E7,
IMAGE_949_SIZE = 0x671,
IMAGE_950_SIZE = 0x52B,
IMAGE_951_SIZE = 0x878,
IMAGE_952_SIZE = 0x799,
IMAGE_953_SIZE = 0x5E4,
IMAGE_954_SIZE = 0x608,
IMAGE_955_SIZE = 0x568,
IMAGE_956_SIZE = 0x5C1,
IMAGE_957_SIZE = 0x5E5,
IMAGE_958_SIZE = 0x774,
IMAGE_959_SIZE = 0x287,
IMAGE_960_SIZE = 0x287,
IMAGE_961_SIZE = 0x760,
IMAGE_962_SIZE = 0x84A,
IMAGE_963_SIZE = 0x86D,
IMAGE_964_SIZE = 0x869,
IMAGE_965_SIZE = 0x83E,
IMAGE_966_SIZE = 0x831,
IMAGE_967_SIZE = 0x84E,
IMAGE_968_SIZE = 0x820,
IMAGE_969_SIZE = 0x285,
IMAGE_970_SIZE = 0x2F1,
IMAGE_971_SIZE = 0x4CB,
IMAGE_972_SIZE = 0x87F,
IMAGE_973_SIZE = 0x8AD,
IMAGE_974_SIZE = 0x87F,
IMAGE_975_SIZE = 0x87F,
IMAGE_976_SIZE = 0x8A2,
IMAGE_977_SIZE = 0x87F,
IMAGE_978_SIZE = 0x91A,
IMAGE_979_SIZE = 0x259,
IMAGE_980_SIZE = 0x676,
IMAGE_981_SIZE = 0x676,
IMAGE_NEON_LAMP_SIZE = 0x4A,
IMAGE_983_SIZE = 0x370,
IMAGE_984_SIZE = 0x307,
IMAGE_985_SIZE = 0x311,
IMAGE_986_SIZE = 0x51A,
IMAGE_987_SIZE = 0x19A,
IMAGE_988_SIZE = 0x11,
IMAGE_989_SIZE = 0x129,
IMAGE_990_SIZE = 0x142,
IMAGE_991_SIZE = 0x191,
IMAGE_992_SIZE = 0x180,
IMAGE_993_SIZE = 0x13C,
IMAGE_994_SIZE = 0x162,
IMAGE_995_SIZE = 0x126,
IMAGE_996_SIZE = 0x175,
IMAGE_997_SIZE = 0x35E,
IMAGE_998_SIZE = 0x232,
IMAGE_999_SIZE = 0x50A,
IMAGE_1000_SIZE = 0x40C,
IMAGE_1001_SIZE = 0x3C9,
IMAGE_1002_SIZE = 0x4D0,
IMAGE_1003_SIZE = 0x4ED,
IMAGE_1004_SIZE = 0x267,
IMAGE_1005_SIZE = 0x33A,
IMAGE_1006_SIZE = 0xF0,
IMAGE_1007_SIZE = 0x491,
IMAGE_1008_SIZE = 0x28D,
IMAGE_1009_SIZE = 0x1B2,
IMAGE_1010_SIZE = 0x612,
IMAGE_1011_SIZE = 0x71E,
IMAGE_1012_SIZE = 0x199,
IMAGE_1013_SIZE = 0x6AD,
IMAGE_PAPERCLIP_SIZE = 0x1F3,
IMAGE_STATIC_NOISE_SIZE = 0x389,
IMAGE_1016_SIZE = 0x367,
IMAGE_1017_SIZE = 0x2FF,
IMAGE_1018_SIZE = 0x379,
IMAGE_1019_SIZE = 0x48E,
IMAGE_1020_SIZE = 0x51F,
IMAGE_1021_SIZE = 0x545,
IMAGE_1022_SIZE = 0x30A,
IMAGE_1023_SIZE = 0x3B3,
IMAGE_1024_SIZE = 0x476,
IMAGE_1025_SIZE = 0x22A,
IMAGE_1026_SIZE = 0x96,
IMAGE_1027_SIZE = 0x331,
IMAGE_1028_SIZE = 0x44F,
IMAGE_1029_SIZE = 0x331,
IMAGE_1030_SIZE = 0x343,
IMAGE_1031_SIZE = 0xAD,
IMAGE_1032_SIZE = 0x5D2,
IMAGE_1033_SIZE = 0x488,
IMAGE_1034_SIZE = 0x477,
IMAGE_1035_SIZE = 0x4D3,
IMAGE_1036_SIZE = 0x61A,
IMAGE_1037_SIZE = 0x48F,
IMAGE_1038_SIZE = 0x4FB,
IMAGE_1039_SIZE = 0x53B,
IMAGE_1040_SIZE = 0x6A0,
IMAGE_1041_SIZE = 0x212,
IMAGE_1042_SIZE = 0x2E7,
IMAGE_1043_SIZE = 0x2A5,
IMAGE_1044_SIZE = 0x244,
IMAGE_1045_SIZE = 0x130,
IMAGE_1046_SIZE = 0x15C,
IMAGE_1047_SIZE = 0x35C,
IMAGE_1048_SIZE = 0x676,
IMAGE_1049_SIZE = 0x2CD,
IMAGE_1050_SIZE = 0x76E,
IMAGE_1051_SIZE = 0x723,
IMAGE_1052_SIZE = 0x7B2,
IMAGE_1053_SIZE = 0x613,
IMAGE_1054_SIZE = 0x228,
IMAGE_1055_SIZE = 0x5D2,
IMAGE_1056_SIZE = 0x207,
IMAGE_1057_SIZE = 0x59E,
IMAGE_1058_SIZE = 0x198,
IMAGE_1059_SIZE = 0x54C,
IMAGE_1060_SIZE = 0x50E,
IMAGE_1061_SIZE = 0x1E3,
IMAGE_1062_SIZE = 0x524,
IMAGE_1063_SIZE = 0x3D6,
IMAGE_1064_SIZE = 0x292,
IMAGE_1065_SIZE = 0x16D,
IMAGE_1066_SIZE = 0x3C5,
IMAGE_1067_SIZE = 0x5BB,
IMAGE_1068_SIZE = 0x53A,
IMAGE_1069_SIZE = 0x5A3,
IMAGE_1070_SIZE = 0x71A,
IMAGE_1071_SIZE = 0x40A,
IMAGE_1072_SIZE = 0x6E1,
IMAGE_1073_SIZE = 0x723,
IMAGE_1074_SIZE = 0x47E,
IMAGE_1075_SIZE = 0x280,
IMAGE_1076_SIZE = 0x383,
IMAGE_1077_SIZE = 0x2E4,
IMAGE_1078_SIZE = 0x140,
IMAGE_1079_SIZE = 0xE6,
IMAGE_1080_SIZE = 0x5EA,
IMAGE_1081_SIZE = 0x4C8,
IMAGE_1082_SIZE = 0x50F,
IMAGE_1083_SIZE = 0xE4,
IMAGE_1084_SIZE = 0x48,
IMAGE_1085_SIZE = 0x5AC,
IMAGE_1086_SIZE = 0x21B,
IMAGE_1087_SIZE = 0x35F,
IMAGE_1088_SIZE = 0x206,
IMAGE_1089_SIZE = 0x16B,
IMAGE_1090_SIZE = 0x46B,
IMAGE_1091_SIZE = 0x53F,
IMAGE_1092_SIZE = 0x3EA,
IMAGE_1093_SIZE = 0x3EB,
IMAGE_1094_SIZE = 0x51B,
IMAGE_1095_SIZE = 0x48A,
IMAGE_1096_SIZE = 0x4C8,
IMAGE_1097_SIZE = 0x456,
IMAGE_1098_SIZE = 0x386,
IMAGE_1099_SIZE = 0x4F0,
IMAGE_1100_SIZE = 0x556,
IMAGE_1101_SIZE = 0x533,
IMAGE_1102_SIZE = 0x3EA,
IMAGE_1103_SIZE = 0x4E8,
IMAGE_1104_SIZE = 0x54E,
IMAGE_1105_SIZE = 0x518,
IMAGE_1106_SIZE = 0x49F,
IMAGE_1107_SIZE = 0x54A,
IMAGE_1108_SIZE = 0x537,
IMAGE_1109_SIZE = 0x535,
IMAGE_1110_SIZE = 0x3E6,
IMAGE_1111_SIZE = 0x4B0,
IMAGE_1112_SIZE = 0x52D,
IMAGE_1113_SIZE = 0x535,
IMAGE_1114_SIZE = 0x46C,
IMAGE_1115_SIZE = 0x4DA,
IMAGE_1116_SIZE = 0x405,
IMAGE_1117_SIZE = 0x501,
IMAGE_1118_SIZE = 0x585,
IMAGE_1119_SIZE = 0x2BA,
IMAGE_1120_SIZE = 0x2A0,
IMAGE_1121_SIZE = 0x31C,
IMAGE_1122_SIZE = 0x19C,
IMAGE_1123_SIZE = 0xD8,
IMAGE_1124_SIZE = 0x728,
IMAGE_1125_SIZE = 0x4AD,
IMAGE_1126_SIZE = 0x325,
IMAGE_1127_SIZE = 0x4EC,
IMAGE_1128_SIZE = 0x801,
IMAGE_1129_SIZE = 0x7E3,
IMAGE_1130_SIZE = 0x657,
IMAGE_1131_SIZE = 0x432,
IMAGE_1132_SIZE = 0x9AA,
IMAGE_1133_SIZE = 0x37F,
IMAGE_1134_SIZE = 0x4DE,
IMAGE_1135_SIZE = 0x5AB,
IMAGE_1136_SIZE = 0x60C,
IMAGE_1137_SIZE = 0x29A,
IMAGE_1138_SIZE = 0x13E,
IMAGE_1139_SIZE = 0x1E9,
IMAGE_1140_SIZE = 0x3BB,
IMAGE_1141_SIZE = 0x43E,
IMAGE_1142_SIZE = 0x425,
IMAGE_1143_SIZE = 0x2B0,
IMAGE_1144_SIZE = 0xD5,
IMAGE_1145_SIZE = 0x14F,
IMAGE_1146_SIZE = 0x129,
IMAGE_1147_SIZE = 0x350,
IMAGE_1148_SIZE = 0x33C,
IMAGE_1149_SIZE = 0x354,
IMAGE_1150_SIZE = 0x191,
IMAGE_1151_SIZE = 0x4CA,
IMAGE_1152_SIZE = 0x323,
IMAGE_1153_SIZE = 0x387,
IMAGE_1154_SIZE = 0x79C,
IMAGE_1155_SIZE = 0x706,
IMAGE_1156_SIZE = 0x2D0,
IMAGE_1157_SIZE = 0x2FA,
IMAGE_1158_SIZE = 0x498,
IMAGE_1159_SIZE = 0x49F,
IMAGE_1160_SIZE = 0x307,
IMAGE_1161_SIZE = 0x549,
IMAGE_1162_SIZE = 0x4E5,
IMAGE_1163_SIZE = 0x3EE,
IMAGE_1164_SIZE = 0x407,
IMAGE_1165_SIZE = 0x5E3,
IMAGE_1166_SIZE = 0x541,
IMAGE_1167_SIZE = 0x533,
IMAGE_1168_SIZE = 0x439,
IMAGE_1169_SIZE = 0x5F0,
IMAGE_1170_SIZE = 0x5B1,
IMAGE_1171_SIZE = 0x374,
IMAGE_1172_SIZE = 0x25,
IMAGE_1173_SIZE = 0x2A4,
IMAGE_1174_SIZE = 0x240,
IMAGE_1175_SIZE = 0x20F,
IMAGE_1176_SIZE = 0x369,
IMAGE_1177_SIZE = 0x227,
IMAGE_1178_SIZE = 0x121,
IMAGE_1179_SIZE = 0x269,
IMAGE_1180_SIZE = 0x1D9,
IMAGE_1181_SIZE = 0x24A,
IMAGE_1182_SIZE = 0x21B,
IMAGE_1183_SIZE = 0x476,
IMAGE_1184_SIZE = 0x6B1,
IMAGE_MONITOR_DESKTOPBARS_SIZE = 0x566,
IMAGE_MONITOR_DESKTOP_SIZE = 0x5A8,
IMAGE_MONITOR_DESKTOPSTAGGERED_SIZE = 0x53F,
IMAGE_1188_SIZE = 0x5E,
IMAGE_1189_SIZE = 0x2EA,
IMAGE_1190_SIZE = 0x3BE,
IMAGE_1191_SIZE = 0x155,
IMAGE_1192_SIZE = 0xC6,
IMAGE_1193_SIZE = 0x342,
IMAGE_1194_SIZE = 0x3D6,
IMAGE_1195_SIZE = 0x329,
IMAGE_1196_SIZE = 0x7CE,
IMAGE_1197_SIZE = 0x5D4,
IMAGE_1198_SIZE = 0x621,
IMAGE_1199_SIZE = 0x7F3,
IMAGE_1200_SIZE = 0x76E,
IMAGE_1201_SIZE = 0x7AC,
IMAGE_1202_SIZE = 0x1E3,
IMAGE_1203_SIZE = 0x28C,
IMAGE_1204_SIZE = 0x2D2,
IMAGE_1205_SIZE = 0x3B8,
IMAGE_1206_SIZE = 0x544,
IMAGE_1207_SIZE = 0x4CA,
IMAGE_1208_SIZE = 0x526,
IMAGE_1209_SIZE = 0x575,
IMAGE_1210_SIZE = 0x4BB,
IMAGE_1211_SIZE = 0x575,
IMAGE_1212_SIZE = 0x5D2,
IMAGE_1213_SIZE = 0x59F,
IMAGE_1214_SIZE = 0x13D,
IMAGE_1215_SIZE = 0x6E4,
IMAGE_1216_SIZE = 0x67D,
IMAGE_1217_SIZE = 0x2E9,
IMAGE_1218_SIZE = 0x373,
IMAGE_1219_SIZE = 0x162,
IMAGE_1220_SIZE = 0x111,
IMAGE_1221_SIZE = 0xC3,
IMAGE_1222_SIZE = 0x21E,
IMAGE_1223_SIZE = 0x43A,
IMAGE_1224_SIZE = 0x54D,
IMAGE_1225_SIZE = 0x28D,
IMAGE_1226_SIZE = 0x3C3,
IMAGE_1227_SIZE = 0x3C1,
IMAGE_1228_SIZE = 0x3AC,
IMAGE_1229_SIZE = 0x59E,
IMAGE_1230_SIZE = 0x349,
IMAGE_1231_SIZE = 0x3D8,
IMAGE_1232_SIZE = 0x453,
IMAGE_1233_SIZE = 0x41F,
IMAGE_1234_SIZE = 0x6AF,
IMAGE_1235_SIZE = 0x39D,
IMAGE_1236_SIZE = 0x300,
IMAGE_1237_SIZE = 0x3E2,
IMAGE_1238_SIZE = 0x38B,
IMAGE_1239_SIZE = 0x32A,
IMAGE_1240_SIZE = 0x40E,
IMAGE_1241_SIZE = 0x333,
IMAGE_1242_SIZE = 0x2C5,
IMAGE_1243_SIZE = 0x265,
IMAGE_1244_SIZE = 0x36C,
IMAGE_1245_SIZE = 0x53E,
IMAGE_1246_SIZE = 0x4B0,
IMAGE_1247_SIZE = 0x198,
IMAGE_1248_SIZE = 0x4E9,
IMAGE_1249_SIZE = 0x29C,
IMAGE_1250_SIZE = 0x2E3,
IMAGE_1251_SIZE = 0x2D9,
IMAGE_1252_SIZE = 0x276,
IMAGE_1253_SIZE = 0x2E3,
IMAGE_1254_SIZE = 0x3AB,
IMAGE_1255_SIZE = 0x249,
IMAGE_1256_SIZE = 0xDA,
IMAGE_1257_SIZE = 0x377,
IMAGE_1258_SIZE = 0x547,
IMAGE_1259_SIZE = 0x3E7,
IMAGE_1260_SIZE = 0x303,
IMAGE_1261_SIZE = 0x1F2,
IMAGE_1262_SIZE = 0x128,
IMAGE_1263_SIZE = 0x1B2,
IMAGE_1264_SIZE = 0x104,
IMAGE_1265_SIZE = 0x4A4,
IMAGE_1266_SIZE = 0x33E,
IMAGE_1267_SIZE = 0x54C,
IMAGE_1268_SIZE = 0x461,
IMAGE_1269_SIZE = 0x2F9,
IMAGE_1270_SIZE = 0x54D,
IMAGE_1271_SIZE = 0x1E1,
IMAGE_1272_SIZE = 0x227,
IMAGE_1273_SIZE = 0x1F5,
IMAGE_1274_SIZE = 0x33E,
IMAGE_1275_SIZE = 0x2D6,
IMAGE_1276_SIZE = 0x1A1,
IMAGE_GBGRADIENT_SIZE = 0xCB,
IMAGE_1278_SIZE = 0x4F0,
IMAGE_1279_SIZE = 0x193,
IMAGE_1280_SIZE = 0x370,
IMAGE_1281_SIZE = 0x3AE,
IMAGE_1282_SIZE = 0x418,
IMAGE_1283_SIZE = 0x1B7,
IMAGE_1284_SIZE = 0x298,
IMAGE_1285_SIZE = 0x814,
IMAGE_1286_SIZE = 0x80A,
IMAGE_1287_SIZE = 0x814,
IMAGE_1288_SIZE = 0x80A,
IMAGE_1289_SIZE = 0x60C,
IMAGE_1290_SIZE = 0x43D,
IMAGE_1291_SIZE = 0x601,
IMAGE_1292_SIZE = 0x60C,
IMAGE_1293_SIZE = 0x59C,
IMAGE_1294_SIZE = 0x5EF,
IMAGE_1295_SIZE = 0x5EE,
IMAGE_1296_SIZE = 0x60C,
IMAGE_1297_SIZE = 0x60C,
IMAGE_1298_SIZE = 0x4A1,
IMAGE_1299_SIZE = 0x4EE,
IMAGE_1300_SIZE = 0x5E0,
IMAGE_1301_SIZE = 0x60C,
IMAGE_1302_SIZE = 0x60C,
IMAGE_1303_SIZE = 0x60C,
IMAGE_1304_SIZE = 0x814,
IMAGE_1305_SIZE = 0x80A,
IMAGE_1306_SIZE = 0x80A,
IMAGE_1307_SIZE = 0x80A,
IMAGE_1308_SIZE = 0x5F9,
IMAGE_1309_SIZE = 0x60C,
IMAGE_1310_SIZE = 0x7E6,
IMAGE_1311_SIZE = 0x7EC,
IMAGE_1312_SIZE = 0x7FB,
IMAGE_1313_SIZE = 0x80A,
IMAGE_1314_SIZE = 0x80C,
IMAGE_1315_SIZE = 0x80A,
IMAGE_1316_SIZE = 0x80A,
IMAGE_1317_SIZE = 0x805,
IMAGE_1318_SIZE = 0x5E9,
IMAGE_1319_SIZE = 0x5D5,
IMAGE_1320_SIZE = 0x5E9,
IMAGE_1321_SIZE = 0x452,
IMAGE_1322_SIZE = 0x60C,
IMAGE_1323_SIZE = 0x60C,
IMAGE_1324_SIZE = 0x814,
IMAGE_1325_SIZE = 0x814,
IMAGE_1326_SIZE = 0x80A,
IMAGE_1327_SIZE = 0x808,
IMAGE_1328_SIZE = 0x80A,
IMAGE_1329_SIZE = 0x80A,
IMAGE_1330_SIZE = 0x80A,
IMAGE_1331_SIZE = 0x80A,
IMAGE_1332_SIZE = 0x49A,
IMAGE_1333_SIZE = 0x5EB,
IMAGE_1334_SIZE = 0x537,
IMAGE_1335_SIZE = 0x609,
IMAGE_1336_SIZE = 0x5ED,
IMAGE_1337_SIZE = 0x506,
IMAGE_1338_SIZE = 0x383,
IMAGE_1339_SIZE = 0x4F5,
IMAGE_1340_SIZE = 0x814,
IMAGE_1341_SIZE = 0x814,
IMAGE_1342_SIZE = 0x80A,
IMAGE_1343_SIZE = 0x80A,
IMAGE_1344_SIZE = 0x80A,
IMAGE_1345_SIZE = 0x80A,
IMAGE_1346_SIZE = 0x80A,
IMAGE_1347_SIZE = 0x80A,
IMAGE_1348_SIZE = 0x814,
IMAGE_1349_SIZE = 0x814,
IMAGE_1350_SIZE = 0x80A,
IMAGE_1351_SIZE = 0x80A,
IMAGE_1352_SIZE = 0x80A,
IMAGE_1353_SIZE = 0x80A,
IMAGE_1354_SIZE = 0x80A,
IMAGE_1355_SIZE = 0x80A,
IMAGE_1356_SIZE = 0x315,
IMAGE_1357_SIZE = 0x283,
IMAGE_1358_SIZE = 0x286,
IMAGE_1359_SIZE = 0x486,
IMAGE_1360_SIZE = 0x5F6,
IMAGE_1361_SIZE = 0x60C,
IMAGE_1362_SIZE = 0x609,
IMAGE_1363_SIZE = 0x4C7,
IMAGE_1364_SIZE = 0x33E,
IMAGE_1365_SIZE = 0x427,
IMAGE_1366_SIZE = 0x1DA,
IMAGE_1367_SIZE = 0x613,
IMAGE_1368_SIZE = 0x4C2,
IMAGE_1369_SIZE = 0x4E3,
IMAGE_1370_SIZE = 0x453,
IMAGE_1371_SIZE = 0x47B,
IMAGE_1372_SIZE = 0x27C,
IMAGE_1373_SIZE = 0x327,
IMAGE_1374_SIZE = 0x2E1,
IMAGE_1375_SIZE = 0x337,
IMAGE_1376_SIZE = 0x24E,
IMAGE_1377_SIZE = 0x1F0,
IMAGE_1378_SIZE = 0x333,
IMAGE_1379_SIZE = 0xD2,
IMAGE_1380_SIZE = 0x4BB,
IMAGE_1381_SIZE = 0x28,
IMAGE_1382_SIZE = 0x26,
IMAGE_LINEAR_LAMP_SIZE = 0x33A,
IMAGE_1384_SIZE = 0x279,
IMAGE_1385_SIZE = 0x602,
IMAGE_1386_SIZE = 0x313,
IMAGE_1387_SIZE = 0x313,
IMAGE_1388_SIZE = 0x545,
IMAGE_1389_SIZE = 0x3FF,
IMAGE_1390_SIZE = 0x462,
IMAGE_1391_SIZE = 0x3FD,
IMAGE_1392_SIZE = 0x18F,
IMAGE_1393_SIZE = 0x24E,
IMAGE_1394_SIZE = 0x3AC,
IMAGE_1395_SIZE = 0x58F,
IMAGE_1396_SIZE = 0x55E,
IMAGE_1397_SIZE = 0x417,
IMAGE_1398_SIZE = 0x4C6,
IMAGE_1399_SIZE = 0x585,
IMAGE_1400_SIZE = 0x466,
IMAGE_1401_SIZE = 0x94F,
IMAGE_1402_SIZE = 0x8F0,
IMAGE_1403_SIZE = 0x901,
IMAGE_1404_SIZE = 0x92D,
IMAGE_1405_SIZE = 0x940,
IMAGE_1406_SIZE = 0x909,
IMAGE_1407_SIZE = 0x940,
IMAGE_1408_SIZE = 0x8F1,
IMAGE_1409_SIZE = 0x1C4,
IMAGE_1410_SIZE = 0x173,
IMAGE_1411_SIZE = 0x155,
IMAGE_1412_SIZE = 0x311,
IMAGE_1413_SIZE = 0x292,
IMAGE_1414_SIZE = 0x22B,
IMAGE_1415_SIZE = 0x1FD,
IMAGE_1416_SIZE = 0x1CD,
IMAGE_1417_SIZE = 0x203,
IMAGE_1418_SIZE = 0x282,
IMAGE_1419_SIZE = 0x24E,
IMAGE_1420_SIZE = 0x1DF,
IMAGE_1421_SIZE = 0x214,
IMAGE_1422_SIZE = 0x417,
IMAGE_1423_SIZE = 0x1F4,
IMAGE_1424_SIZE = 0x5E1,
IMAGE_1425_SIZE = 0x5D,
IMAGE_1426_SIZE = 0x556,
IMAGE_1427_SIZE = 0x161,
IMAGE_1428_SIZE = 0x159,
IMAGE_1429_SIZE = 0x4BF,
IMAGE_1430_SIZE = 0x283,
IMAGE_1431_SIZE = 0x32E,
IMAGE_1432_SIZE = 0x134,
IMAGE_1433_SIZE = 0x24E,
IMAGE_1434_SIZE = 0x116,
IMAGE_1435_SIZE = 0xF0,
IMAGE_1436_SIZE = 0x112,
IMAGE_1437_SIZE = 0x3D3,
IMAGE_1438_SIZE = 0x3D3,
IMAGE_1439_SIZE = 0x3E7,
IMAGE_1440_SIZE = 0x44A,
IMAGE_1441_SIZE = 0x11B,
IMAGE_1442_SIZE = 0x12C,
IMAGE_1443_SIZE = 0x175,
IMAGE_1444_SIZE = 0x27E,
IMAGE_1445_SIZE = 0x1C8,
IMAGE_1446_SIZE = 0x2E1,
IMAGE_1447_SIZE = 0x1FF,
IMAGE_1448_SIZE = 0x2F8,
IMAGE_1449_SIZE = 0x29B,
IMAGE_1450_SIZE = 0x278,
IMAGE_1451_SIZE = 0x29F,
IMAGE_1452_SIZE = 0x251,
IMAGE_1453_SIZE = 0x28E,
IMAGE_1454_SIZE = 0x26E,
IMAGE_1455_SIZE = 0x2D2,
IMAGE_1456_SIZE = 0x391,
IMAGE_1457_SIZE = 0x432,
IMAGE_1458_SIZE = 0x13B,
IMAGE_1459_SIZE = 0x27A,
IMAGE_1460_SIZE = 0x26F,
IMAGE_1461_SIZE = 0x505,
IMAGE_1462_SIZE = 0x4DE,
IMAGE_1463_SIZE = 0x4B2,
IMAGE_1464_SIZE = 0x33B,
IMAGE_1465_SIZE = 0x2D7,
IMAGE_1466_SIZE = 0x1B6,
IMAGE_1467_SIZE = 0x1F2,
IMAGE_1468_SIZE = 0x6A3,
IMAGE_1469_SIZE = 0x6A3,
IMAGE_1470_SIZE = 0x27F,
IMAGE_1471_SIZE = 0x25B,
IMAGE_1472_SIZE = 0x1A4,
IMAGE_1473_SIZE = 0x582,
IMAGE_1474_SIZE = 0x21D,
IMAGE_IMPACTREDBRICK1_SIZE = 0x51D,
IMAGE_IMPACTREDBRICK3_SIZE = 0x4F8,
IMAGE_1477_SIZE = 0x487,
IMAGE_IMPACTBROWNBRICK1_SIZE = 0x368,
IMAGE_IMPACTBROWNBRICK2_SIZE = 0x37C,
IMAGE_1480_SIZE = 0x78A,
IMAGE_1481_SIZE = 0x49,
IMAGE_1482_SIZE = 0x4B,
IMAGE_1483_SIZE = 0x33,
IMAGE_1484_SIZE = 0x3BE,
IMAGE_1485_SIZE = 0xBE,
IMAGE_1486_SIZE = 0x1A8,
IMAGE_1487_SIZE = 0x6CE,
IMAGE_1488_SIZE = 0x674,
IMAGE_1489_SIZE = 0x706,
IMAGE_1490_SIZE = 0x6C4,
IMAGE_1491_SIZE = 0x6DE,
IMAGE_1492_SIZE = 0x700,
IMAGE_1493_SIZE = 0x6BC,
IMAGE_1494_SIZE = 0x6E5,
IMAGE_1495_SIZE = 0x66B,
IMAGE_1496_SIZE = 0x73B,
IMAGE_1497_SIZE = 0x3E6,
IMAGE_1498_SIZE = 0x404,
IMAGE_1499_SIZE = 0x39D,
IMAGE_1500_SIZE = 0x2EA,
IMAGE_1501_SIZE = 0x574,
IMAGE_1502_SIZE = 0x10E,
IMAGE_1503_SIZE = 0x210,
IMAGE_1504_SIZE = 0x15F,
IMAGE_1505_SIZE = 0x311,
IMAGE_1506_SIZE = 0x3AF,
IMAGE_1507_SIZE = 0x4F7,
IMAGE_WATER_GRAYSCALE_SIZE = 0x28A,
IMAGE_WATER_BLUE_SIZE = 0x234,
IMAGE_1510_SIZE = 0x292,
IMAGE_1511_SIZE = 0x285,
IMAGE_1512_SIZE = 0x549,
IMAGE_1513_SIZE = 0x1F6,
IMAGE_1514_SIZE = 0xC,
IMAGE_1515_SIZE = 0x168,
IMAGE_1516_SIZE = 0x190,
IMAGE_1517_SIZE = 0x250,
IMAGE_1518_SIZE = 0x57A,
IMAGE_1519_SIZE = 0x42C,
IMAGE_1520_SIZE = 0x4FF,
IMAGE_1521_SIZE = 0x323,
IMAGE_1522_SIZE = 0x1DF,
IMAGE_1523_SIZE = 0x503,
IMAGE_1524_SIZE = 0x1A5,
IMAGE_1525_SIZE = 0x2EC,
IMAGE_1526_SIZE = 0x485,
IMAGE_1527_SIZE = 0x728,
IMAGE_1528_SIZE = 0x223,
IMAGE_1529_SIZE = 0x3CB,
IMAGE_1530_SIZE = 0x15A,
IMAGE_1531_SIZE = 0xE7,
IMAGE_1532_SIZE = 0x176,
IMAGE_1533_SIZE = 0x1B4,
IMAGE_1534_SIZE = 0x2C3,
IMAGE_1535_SIZE = 0x2A8,
IMAGE_1536_SIZE = 0x23A,
IMAGE_1537_SIZE = 0x246,
IMAGE_1538_SIZE = 0x240,
IMAGE_1539_SIZE = 0x27C,
IMAGE_1540_SIZE = 0x2EA,
IMAGE_1541_SIZE = 0x4B1,
IMAGE_1542_SIZE = 0x474,
IMAGE_1543_SIZE = 0x485,
IMAGE_1544_SIZE = 0x7D6,
IMAGE_1545_SIZE = 0x881,
IMAGE_1546_SIZE = 0x42A,
IMAGE_1547_SIZE = 0x5BC,
IMAGE_1548_SIZE = 0x72C,
IMAGE_1549_SIZE = 0x891,
IMAGE_1550_SIZE = 0x75C,
IMAGE_1551_SIZE = 0x8DC,
IMAGE_1552_SIZE = 0x94A,
IMAGE_1553_SIZE = 0x924,
IMAGE_1554_SIZE = 0x970,
IMAGE_1555_SIZE = 0x24E,
IMAGE_1556_SIZE = 0x9A4,
IMAGE_1557_SIZE = 0x337,
IMAGE_1558_SIZE = 0x608,
IMAGE_1559_SIZE = 0x60C,
IMAGE_1560_SIZE = 0x9E7,
IMAGE_1561_SIZE = 0x9DC,
IMAGE_1562_SIZE = 0x9C2,
IMAGE_1563_SIZE = 0x1F8,
IMAGE_1564_SIZE = 0x251,
IMAGE_1565_SIZE = 0x232,
IMAGE_1566_SIZE = 0x1FB,
IMAGE_1567_SIZE = 0x5C0,
IMAGE_1568_SIZE = 0x407,
IMAGE_1569_SIZE = 0x9E9,
IMAGE_1570_SIZE = 0x8A6,
IMAGE_1571_SIZE = 0x968,
IMAGE_1572_SIZE = 0x5C5,
IMAGE_1573_SIZE = 0x41A,
IMAGE_1574_SIZE = 0x386,
IMAGE_1575_SIZE = 0x3DE,
IMAGE_1576_SIZE = 0x272,
IMAGE_1577_SIZE = 0x395,
IMAGE_1578_SIZE = 0x350,
IMAGE_1579_SIZE = 0x2EC,
IMAGE_1580_SIZE = 0x324,
IMAGE_1581_SIZE = 0x2C6,
IMAGE_1582_SIZE = 0x71E,
IMAGE_1583_SIZE = 0x70F,
IMAGE_1584_SIZE = 0x359,
IMAGE_1585_SIZE = 0x3C9,
IMAGE_1586_SIZE = 0x3A0,
IMAGE_1587_SIZE = 0x36B,
IMAGE_1588_SIZE = 0xB7,
IMAGE_1589_SIZE = 0x4FB,
IMAGE_1590_SIZE = 0x53F,
IMAGE_1591_SIZE = 0x4D1,
IMAGE_1592_SIZE = 0x70D,
IMAGE_1593_SIZE = 0x2DC,
IMAGE_1594_SIZE = 0x3A6,
IMAGE_1595_SIZE = 0x19B,
IMAGE_1596_SIZE = 0x3CC,
IMAGE_1597_SIZE = 0x28,
IMAGE_1598_SIZE = 0x35,
IMAGE_1599_SIZE = 0x237,
IMAGE_1600_SIZE = 0x3A5,
IMAGE_1601_SIZE = 0x38E,
IMAGE_1602_SIZE = 0x2A5,
IMAGE_1603_SIZE = 0x28F,
IMAGE_1604_SIZE = 0xC,
IMAGE_1605_SIZE = 0x1DB,
IMAGE_1606_SIZE = 0x75B,
IMAGE_1607_SIZE = 0x278,
IMAGE_1608_SIZE = 0xC,
IMAGE_1609_SIZE = 0x3D7,
IMAGE_1610_SIZE = 0x155,
IMAGE_1611_SIZE = 0x3FB,
IMAGE_1612_SIZE = 0x3AD,
IMAGE_1613_SIZE = 0x59C,
IMAGE_1614_SIZE = 0x131,
IMAGE_1615_SIZE = 0x3B9,
IMAGE_1616_SIZE = 0x4F4,
IMAGE_1617_SIZE = 0x14F,
IMAGE_1618_SIZE = 0x3B5,
IMAGE_1619_SIZE = 0x496,
IMAGE_1620_SIZE = 0x16E,
IMAGE_1621_SIZE = 0x3CB,
IMAGE_1622_SIZE = 0x4E7,
IMAGE_1623_SIZE = 0x1B2,
IMAGE_1624_SIZE = 0x860,
IMAGE_1625_SIZE = 0x87A,
IMAGE_1626_SIZE = 0x6FC,
IMAGE_1627_SIZE = 0x14F,
IMAGE_1628_SIZE = 0x2FA,
IMAGE_1629_SIZE = 0x2FF,
IMAGE_1630_SIZE = 0x134,
IMAGE_1631_SIZE = 0x2D4,
IMAGE_1632_SIZE = 0x4FE,
IMAGE_1633_SIZE = 0x534,
IMAGE_1634_SIZE = 0x383,
IMAGE_1635_SIZE = 0x36A,
IMAGE_1636_SIZE = 0x2F1,
IMAGE_1637_SIZE = 0x36C,
IMAGE_1638_SIZE = 0x13A,
IMAGE_1639_SIZE = 0x37,
IMAGE_1640_SIZE = 0x399,
IMAGE_1641_SIZE = 0x2E9,
IMAGE_1642_SIZE = 0x41F,
IMAGE_1643_SIZE = 0x32C,
IMAGE_1644_SIZE = 0x5BE,
IMAGE_1645_SIZE = 0x2CD,
IMAGE_1646_SIZE = 0x30D,
IMAGE_1647_SIZE = 0x5CE,
IMAGE_1648_SIZE = 0x728,
IMAGE_1649_SIZE = 0x47C,
IMAGE_1650_SIZE = 0x5F2,
IMAGE_1651_SIZE = 0x40,
IMAGE_1652_SIZE = 0x6BC,
IMAGE_1653_SIZE = 0x9AA,
IMAGE_1654_SIZE = 0x7FA,
IMAGE_1655_SIZE = 0x3FE,
IMAGE_1656_SIZE = 0x4CE,
IMAGE_1657_SIZE = 0x2BC,
IMAGE_1658_SIZE = 0x5D0,
IMAGE_1659_SIZE = 0x56D,
IMAGE_1660_SIZE = 0x791,
IMAGE_1661_SIZE = 0x951,
IMAGE_1662_SIZE = 0x78D,
IMAGE_1663_SIZE = 0x24E,
IMAGE_1664_SIZE = 0x30D,
IMAGE_1665_SIZE = 0x936,
IMAGE_1666_SIZE = 0x4D1,
IMAGE_1667_SIZE = 0x54E,
IMAGE_1668_SIZE = 0x176,
IMAGE_1669_SIZE = 0x5A1,
IMAGE_1670_SIZE = 0x586,
IMAGE_1671_SIZE = 0xC,
IMAGE_1672_SIZE = 0x462,
IMAGE_1673_SIZE = 0x5F7,
IMAGE_1674_SIZE = 0x734,
IMAGE_1675_SIZE = 0x73A,
IMAGE_1676_SIZE = 0x6C7,
IMAGE_1677_SIZE = 0x799,
IMAGE_1678_SIZE = 0x465,
IMAGE_1679_SIZE = 0x65E,
IMAGE_1680_SIZE = 0x49B,
IMAGE_1681_SIZE = 0x680,
IMAGE_1682_SIZE = 0x98C,
IMAGE_1683_SIZE = 0x920,
IMAGE_1684_SIZE = 0x91D,
IMAGE_1685_SIZE = 0x9BE,
IMAGE_1686_SIZE = 0x66F,
IMAGE_1687_SIZE = 0x611,
IMAGE_1688_SIZE = 0x5B3,
IMAGE_1689_SIZE = 0x147,
IMAGE_1690_SIZE = 0x69A,
IMAGE_1691_SIZE = 0x535,
IMAGE_1692_SIZE = 0x40A,
IMAGE_1693_SIZE = 0x31C,
IMAGE_1694_SIZE = 0x2D0,
IMAGE_1695_SIZE = 0x2BF,
IMAGE_1696_SIZE = 0x998,
IMAGE_1697_SIZE = 0x1C1,
IMAGE_1698_SIZE = 0x2B4,
IMAGE_1699_SIZE = 0x31C,
IMAGE_1700_SIZE = 0x388,
IMAGE_1701_SIZE = 0x3EE,
IMAGE_1702_SIZE = 0x8DB,
IMAGE_1703_SIZE = 0x859,
IMAGE_1704_SIZE = 0x36A,
IMAGE_1705_SIZE = 0x957,
IMAGE_1706_SIZE = 0x99F,
IMAGE_1707_SIZE = 0x9D0,
IMAGE_1708_SIZE = 0x696,
IMAGE_1709_SIZE = 0x1BE,
IMAGE_1710_SIZE = 0x2A1,
IMAGE_1711_SIZE = 0x558,
IMAGE_1712_SIZE = 0x1BB,
IMAGE_1713_SIZE = 0x2B7,
IMAGE_1714_SIZE = 0x283,
IMAGE_1715_SIZE = 0x250,
IMAGE_1716_SIZE = 0x2C1,
IMAGE_1717_SIZE = 0x4E1,
IMAGE_1718_SIZE = 0x36C,
IMAGE_1719_SIZE = 0x307,
IMAGE_1720_SIZE = 0x2EA,
IMAGE_1721_SIZE = 0x2C1,
IMAGE_1722_SIZE = 0x943,
IMAGE_1723_SIZE = 0x8C3,
IMAGE_1724_SIZE = 0x907,
IMAGE_1725_SIZE = 0xC,
IMAGE_1726_SIZE = 0x9D2,
IMAGE_1727_SIZE = 0x9B3,
IMAGE_1728_SIZE = 0x990,
IMAGE_1729_SIZE = 0x3FE,
IMAGE_1730_SIZE = 0x40A,
IMAGE_1731_SIZE = 0x447,
IMAGE_1732_SIZE = 0x418,
IMAGE_1733_SIZE = 0x833,
IMAGE_1734_SIZE = 0x7E9,
IMAGE_1735_SIZE = 0x341,
IMAGE_1736_SIZE = 0x171,
IMAGE_1737_SIZE = 0x607,
IMAGE_1738_SIZE = 0x58D,
IMAGE_1739_SIZE = 0x3B7,
IMAGE_1740_SIZE = 0x400,
IMAGE_1741_SIZE = 0x45A,
IMAGE_1742_SIZE = 0x712,
IMAGE_1743_SIZE = 0x226,
IMAGE_1744_SIZE = 0x415,
IMAGE_1745_SIZE = 0x535,
IMAGE_1746_SIZE = 0x774,
IMAGE_1747_SIZE = 0x7A8,
IMAGE_1748_SIZE = 0x76D,
IMAGE_1749_SIZE = 0x27C,
IMAGE_1750_SIZE = 0x2A9,
IMAGE_1751_SIZE = 0x10B,
IMAGE_1752_SIZE = 0x342,
IMAGE_1753_SIZE = 0x1E5,
IMAGE_1754_SIZE = 0x1FB,
IMAGE_1755_SIZE = 0x5C8,
IMAGE_1756_SIZE = 0x39B,
IMAGE_1757_SIZE = 0x416,
IMAGE_1758_SIZE = 0x452,
IMAGE_1759_SIZE = 0x428,
IMAGE_1760_SIZE = 0x29B,
IMAGE_1761_SIZE = 0x1D2,
IMAGE_1762_SIZE = 0x65A,
IMAGE_1763_SIZE = 0x964,
IMAGE_1764_SIZE = 0x8BC,
IMAGE_1765_SIZE = 0x8E8,
IMAGE_1766_SIZE = 0x9EA,
IMAGE_1767_SIZE = 0x9AB,
IMAGE_1768_SIZE = 0x9B5,
IMAGE_1769_SIZE = 0x38C,
IMAGE_1770_SIZE = 0x559,
IMAGE_1771_SIZE = 0x667,
IMAGE_1772_SIZE = 0x57A,
IMAGE_1773_SIZE = 0x480,
IMAGE_1774_SIZE = 0x1A0,
IMAGE_1775_SIZE = 0xF6,
IMAGE_1776_SIZE = 0x317,
IMAGE_1777_SIZE = 0x37E,
IMAGE_1778_SIZE = 0x61A,
IMAGE_1779_SIZE = 0x4B5,
IMAGE_1780_SIZE = 0x139,
IMAGE_1781_SIZE = 0x142,
IMAGE_1782_SIZE = 0x191,
IMAGE_1783_SIZE = 0x301,
IMAGE_1784_SIZE = 0x1C2,
IMAGE_1785_SIZE = 0x367,
IMAGE_1786_SIZE = 0x3DD,
IMAGE_1787_SIZE = 0x386,
IMAGE_1788_SIZE = 0x33E,
IMAGE_1789_SIZE = 0x30A,
IMAGE_1790_SIZE = 0x2E8,
IMAGE_1791_SIZE = 0x340,
IMAGE_1792_SIZE = 0x368,
IMAGE_1793_SIZE = 0x3D5,
IMAGE_1794_SIZE = 0x3DB,
IMAGE_1795_SIZE = 0x4D7,
IMAGE_1796_SIZE = 0x498,
IMAGE_1797_SIZE = 0x49D,
IMAGE_1798_SIZE = 0x2EA,
IMAGE_1799_SIZE = 0x374,
IMAGE_1800_SIZE = 0x68C,
IMAGE_1801_SIZE = 0x72E,
IMAGE_1802_SIZE = 0x8E2,
IMAGE_1803_SIZE = 0x820,
IMAGE_1804_SIZE = 0x8A1,
IMAGE_1805_SIZE = 0x83A,
IMAGE_1806_SIZE = 0x1F6,
IMAGE_1807_SIZE = 0x1D4,
IMAGE_1808_SIZE = 0x2F4,
IMAGE_1809_SIZE = 0x13E,
IMAGE_1810_SIZE = 0x173,
IMAGE_1811_SIZE = 0x678,
IMAGE_1812_SIZE = 0x299,
IMAGE_1813_SIZE = 0x5A3,
IMAGE_1814_SIZE = 0xA0C,
IMAGE_1815_SIZE = 0x9E3,
IMAGE_1816_SIZE = 0x9E9,
IMAGE_1817_SIZE = 0x940,
IMAGE_1818_SIZE = 0x9B2,
IMAGE_1819_SIZE = 0x966,
IMAGE_1820_SIZE = 0x9E1,
IMAGE_1821_SIZE = 0x929,
IMAGE_1822_SIZE = 0x948,
IMAGE_1823_SIZE = 0x9E8,
IMAGE_1824_SIZE = 0x9CD,
IMAGE_1825_SIZE = 0x9A4,
IMAGE_1826_SIZE = 0x961,
IMAGE_1827_SIZE = 0x965,
IMAGE_1828_SIZE = 0x909,
IMAGE_1829_SIZE = 0x393,
IMAGE_1830_SIZE = 0x157,
IMAGE_1831_SIZE = 0x28B,
IMAGE_1832_SIZE = 0x312,
IMAGE_1833_SIZE = 0x9D9,
IMAGE_1834_SIZE = 0x905,
IMAGE_1835_SIZE = 0x928,
IMAGE_1836_SIZE = 0x277,
IMAGE_1837_SIZE = 0x27C,
IMAGE_1838_SIZE = 0x116,
IMAGE_1839_SIZE = 0x1C7,
IMAGE_1840_SIZE = 0x104,
IMAGE_1841_SIZE = 0x288,
IMAGE_1842_SIZE = 0x61D,
IMAGE_1843_SIZE = 0x185,
IMAGE_1844_SIZE = 0x859,
IMAGE_1845_SIZE = 0x89F,
IMAGE_1846_SIZE = 0x7E8,
IMAGE_1847_SIZE = 0x9A6,
IMAGE_1848_SIZE = 0x8AD,
IMAGE_1849_SIZE = 0x8BF,
IMAGE_1850_SIZE = 0x929,
IMAGE_1851_SIZE = 0x959,
IMAGE_1852_SIZE = 0x93B,
IMAGE_1853_SIZE = 0x8F0,
IMAGE_1854_SIZE = 0x8B8,
IMAGE_1855_SIZE = 0x933,
IMAGE_1856_SIZE = 0x4DB,
IMAGE_1857_SIZE = 0x941,
IMAGE_1858_SIZE = 0x6C3,
IMAGE_1859_SIZE = 0x844,
IMAGE_1860_SIZE = 0x32A,
IMAGE_1861_SIZE = 0x60A,
IMAGE_1862_SIZE = 0x3A6,
IMAGE_1863_SIZE = 0x110,
IMAGE_1864_SIZE = 0x2B6,
IMAGE_1865_SIZE = 0x725,
IMAGE_1866_SIZE = 0x4FE,
IMAGE_1867_SIZE = 0xC,
IMAGE_1868_SIZE = 0x8CF,
IMAGE_1869_SIZE = 0x94A,
IMAGE_1870_SIZE = 0x9A8,
IMAGE_1871_SIZE = 0x78C,
IMAGE_1872_SIZE = 0x9DE,
IMAGE_1873_SIZE = 0x114,
IMAGE_1874_SIZE = 0x57F,
IMAGE_1875_SIZE = 0x1C2,
IMAGE_1876_SIZE = 0x7D4,
IMAGE_1877_SIZE = 0x7E5,
IMAGE_1878_SIZE = 0x9E3,
IMAGE_1879_SIZE = 0x7BD,
IMAGE_1880_SIZE = 0x4A8,
IMAGE_1881_SIZE = 0xBA,
IMAGE_1882_SIZE = 0x5D1,
IMAGE_1883_SIZE = 0x96C,
IMAGE_1884_SIZE = 0x87A,
IMAGE_1885_SIZE = 0x800,
IMAGE_1886_SIZE = 0x470,
IMAGE_1887_SIZE = 0x211,
IMAGE_1888_SIZE = 0x20A,
IMAGE_1889_SIZE = 0x32A,
IMAGE_1890_SIZE = 0x3E7,
IMAGE_1891_SIZE = 0x263,
IMAGE_1892_SIZE = 0xA0C,
IMAGE_1893_SIZE = 0x60C,
IMAGE_1894_SIZE = 0x5F0,
IMAGE_1895_SIZE = 0x66A,
IMAGE_1896_SIZE = 0x322,
IMAGE_1897_SIZE = 0x37F,
IMAGE_1898_SIZE = 0x37F,
IMAGE_1899_SIZE = 0x314,
IMAGE_1900_SIZE = 0x333,
IMAGE_1901_SIZE = 0x19C,
IMAGE_1902_SIZE = 0x232,
IMAGE_1903_SIZE = 0x151,
IMAGE_1904_SIZE = 0x176,
IMAGE_1905_SIZE = 0x7A9,
IMAGE_1906_SIZE = 0x79A,
IMAGE_1907_SIZE = 0x932,
IMAGE_1908_SIZE = 0x8CD,
IMAGE_1909_SIZE = 0x8CE,
IMAGE_1910_SIZE = 0x104,
IMAGE_1911_SIZE = 0x2F3,
IMAGE_1912_SIZE = 0x5B4,
IMAGE_1913_SIZE = 0x33E,
IMAGE_1914_SIZE = 0x50E,
IMAGE_1915_SIZE = 0x5FA,
IMAGE_1916_SIZE = 0x464,
IMAGE_1917_SIZE = 0xC,
IMAGE_1918_SIZE = 0x577,
IMAGE_1919_SIZE = 0x583,
IMAGE_1920_SIZE = 0x8A4,
IMAGE_1921_SIZE = 0x352,
IMAGE_1922_SIZE = 0x2D7,
IMAGE_1923_SIZE = 0x35B,
IMAGE_1924_SIZE = 0x3C3,
IMAGE_1925_SIZE = 0x477,
IMAGE_1926_SIZE = 0x47F,
IMAGE_1927_SIZE = 0x291,
IMAGE_1928_SIZE = 0x8F5,
IMAGE_1929_SIZE = 0x908,
IMAGE_1930_SIZE = 0x9D3,
IMAGE_1931_SIZE = 0x8C9,
IMAGE_1932_SIZE = 0x2A0,
IMAGE_1933_SIZE = 0x838,
IMAGE_1934_SIZE = 0x7F5,
IMAGE_1935_SIZE = 0x315,
IMAGE_1936_SIZE = 0x516,
IMAGE_1937_SIZE = 0x46B,
IMAGE_1938_SIZE = 0x305,
IMAGE_1939_SIZE = 0x4DE,
IMAGE_1940_SIZE = 0x46B,
IMAGE_1941_SIZE = 0x320,
IMAGE_1942_SIZE = 0x695,
IMAGE_1943_SIZE = 0x96E,
IMAGE_1944_SIZE = 0x99D,
IMAGE_1945_SIZE = 0x996,
IMAGE_1946_SIZE = 0x38E,
IMAGE_1947_SIZE = 0x248,
IMAGE_1948_SIZE = 0x2CC,
IMAGE_1949_SIZE = 0x79,
IMAGE_1950_SIZE = 0x61C,
IMAGE_1951_SIZE = 0x48E,
IMAGE_1952_SIZE = 0x55A,
IMAGE_1953_SIZE = 0x523,
IMAGE_1954_SIZE = 0x652,
IMAGE_1955_SIZE = 0x543,
IMAGE_1956_SIZE = 0x4D7,
IMAGE_1957_SIZE = 0x563,
IMAGE_1958_SIZE = 0x3C6,
IMAGE_1959_SIZE = 0x446,
IMAGE_1960_SIZE = 0x26A,
IMAGE_1961_SIZE = 0x561,
IMAGE_1962_SIZE = 0x259,
IMAGE_1963_SIZE = 0x757,
IMAGE_1964_SIZE = 0x712,
IMAGE_1965_SIZE = 0x42B,
IMAGE_1966_SIZE = 0x172,
IMAGE_1967_SIZE = 0x35C,
IMAGE_1968_SIZE = 0x3F9,
IMAGE_1969_SIZE = 0x594,
IMAGE_1970_SIZE = 0x98A,
IMAGE_1971_SIZE = 0x90B,
IMAGE_1972_SIZE = 0x9C5,
IMAGE_1973_SIZE = 0x8E3,
IMAGE_1974_SIZE = 0x949,
IMAGE_1975_SIZE = 0x90F,
IMAGE_1976_SIZE = 0xA0C,
IMAGE_1977_SIZE = 0x960,
IMAGE_1978_SIZE = 0x95C,
IMAGE_1979_SIZE = 0x1B3,
IMAGE_1980_SIZE = 0x425,
IMAGE_1981_SIZE = 0x76C,
IMAGE_1982_SIZE = 0x775,
IMAGE_1983_SIZE = 0x1AF,
IMAGE_1984_SIZE = 0x176,
IMAGE_1985_SIZE = 0x2C0,
IMAGE_1986_SIZE = 0x394,
IMAGE_1987_SIZE = 0x306,
IMAGE_1988_SIZE = 0x55D,
IMAGE_1989_SIZE = 0x53E,
IMAGE_1990_SIZE = 0x179,
IMAGE_1991_SIZE = 0x17D,
IMAGE_1992_SIZE = 0x1B7,
IMAGE_1993_SIZE = 0x1CC,
IMAGE_1994_SIZE = 0x181,
IMAGE_1995_SIZE = 0xF0,
IMAGE_1996_SIZE = 0x220,
IMAGE_1997_SIZE = 0x998,
IMAGE_1998_SIZE = 0x9B7,
IMAGE_1999_SIZE = 0x96D,
IMAGE_2000_SIZE = 0x8BC,
IMAGE_2001_SIZE = 0x8BA,
IMAGE_2002_SIZE = 0x884,
IMAGE_2003_SIZE = 0x924,
IMAGE_2004_SIZE = 0x812,
IMAGE_2005_SIZE = 0x276,
IMAGE_2006_SIZE = 0x19C,
IMAGE_2007_SIZE = 0xFE,
IMAGE_2008_SIZE = 0x2B1,
IMAGE_2009_SIZE = 0x269,
IMAGE_2010_SIZE = 0x768,
IMAGE_2011_SIZE = 0xB9,
IMAGE_2012_SIZE = 0xE4,
IMAGE_2013_SIZE = 0x141,
IMAGE_2014_SIZE = 0x30F,
IMAGE_2015_SIZE = 0x140,
IMAGE_2016_SIZE = 0x398,
IMAGE_2017_SIZE = 0x183,
IMAGE_2018_SIZE = 0x12F,
IMAGE_2019_SIZE = 0x158,
IMAGE_2020_SIZE = 0x1AE,
IMAGE_2021_SIZE = 0x1D5,
IMAGE_2022_SIZE = 0xC4,
IMAGE_2023_SIZE = 0x398,
IMAGE_2024_SIZE = 0x2BF,
IMAGE_2025_SIZE = 0x761,
IMAGE_2026_SIZE = 0x11E,
IMAGE_2027_SIZE = 0x3BB,
IMAGE_2028_SIZE = 0x654,
IMAGE_2029_SIZE = 0x5B7,
IMAGE_2030_SIZE = 0x66F,
IMAGE_2031_SIZE = 0x6C3,
IMAGE_2032_SIZE = 0x400,
IMAGE_2033_SIZE = 0x308,
IMAGE_2034_SIZE = 0x382,
IMAGE_2035_SIZE = 0x5A4,
IMAGE_2036_SIZE = 0x2B1,
IMAGE_2037_SIZE = 0x441,
IMAGE_2038_SIZE = 0x1F4,
IMAGE_2039_SIZE = 0x1A7,
IMAGE_2040_SIZE = 0x900,
IMAGE_2041_SIZE = 0x7F4,
IMAGE_2042_SIZE = 0x125,
IMAGE_2043_SIZE = 0x19A,
IMAGE_2044_SIZE = 0x55A,
IMAGE_2045_SIZE = 0x230,
IMAGE_2046_SIZE = 0x30E,
IMAGE_2047_SIZE = 0x944,
IMAGE_2048_SIZE = 0x281,
IMAGE_2049_SIZE = 0x255,
IMAGE_2050_SIZE = 0x451,
IMAGE_2051_SIZE = 0x3BE,
IMAGE_2052_SIZE = 0x1BB,
IMAGE_2053_SIZE = 0x6CA,
IMAGE_2054_SIZE = 0x7AB,
IMAGE_2055_SIZE = 0x3C7,
IMAGE_2056_SIZE = 0x972,
IMAGE_2057_SIZE = 0xC,
IMAGE_2058_SIZE = 0x196,
IMAGE_2059_SIZE = 0x27B,
IMAGE_2060_SIZE = 0x1AA,
IMAGE_2061_SIZE = 0x981,
IMAGE_2062_SIZE = 0x96B,
IMAGE_2063_SIZE = 0x903,
IMAGE_2064_SIZE = 0x245,
IMAGE_2065_SIZE = 0x12F,
IMAGE_2066_SIZE = 0x48B,
IMAGE_2067_SIZE = 0x2FA,
IMAGE_2068_SIZE = 0x426,
IMAGE_2069_SIZE = 0x60B,
IMAGE_2070_SIZE = 0x4BB,
IMAGE_2071_SIZE = 0x38A,
IMAGE_2072_SIZE = 0x45F,
IMAGE_2073_SIZE = 0x492,
IMAGE_2074_SIZE = 0x30F,
IMAGE_2075_SIZE = 0x409,
IMAGE_2076_SIZE = 0x396,
IMAGE_2077_SIZE = 0x9AE,
IMAGE_2078_SIZE = 0x7A8,
IMAGE_2079_SIZE = 0x8B2,
IMAGE_2080_SIZE = 0x2DE,
IMAGE_2081_SIZE = 0x3A8,
IMAGE_2082_SIZE = 0x6B5,
IMAGE_2083_SIZE = 0x62A,
IMAGE_SMOKE_0_SIZE = 7,
IMAGE_FIRE_0_SIZE = 8,
IMAGE_SMOKE_1_SIZE = 0x4DD,
IMAGE_FIRE_1_SIZE = 0x123,
IMAGE_SMOKE_2_SIZE = 0x3AF,
IMAGE_FIRE_2_SIZE = 0xD1,
IMAGE_SMOKE_3_SIZE = 0x485,
IMAGE_FIRE_3_SIZE = 0xFE,
IMAGE_SMOKE_4_SIZE = 0x425,
IMAGE_FIRE_4_SIZE = 0xDA,
IMAGE_SMOKE_5_SIZE = 0x313,
IMAGE_FIRE_5_SIZE = 0x78,
IMAGE_SMOKE_6_SIZE = 0xAA,
IMAGE_FIRE_6_SIZE = 0x32,
IMAGE_SMOKE_7_SIZE = 0x3CB,
IMAGE_FIRE_7_SIZE = 0x100,
IMAGE_SMOKE_8_SIZE = 0x4AC,
IMAGE_FIRE_8_SIZE = 0x112,
IMAGE_SMOKE_9_SIZE = 0x555,
IMAGE_FIRE_9_SIZE = 0x120,
IMAGE_SMOKE_10_SIZE = 0x5B3,
IMAGE_FIRE_10_SIZE = 0x132,
IMAGE_SMOKE_11_SIZE = 0x5ED,
IMAGE_FIRE_11_SIZE = 0x142,
IMAGE_SMOKE_12_SIZE = 0x5AB,
IMAGE_FIRE_12_SIZE = 0x137,
IMAGE_SMOKE_13_SIZE = 0x580,
IMAGE_FIRE_13_SIZE = 0x135,
IMAGE_SMOKE_14_SIZE = 0x526,
IMAGE_FIRE_14_SIZE = 0x12D,
IMAGE_2114_SIZE = 0x8F9,
IMAGE_2115_SIZE = 0x8C6,
IMAGE_2116_SIZE = 0x82E,
IMAGE_2117_SIZE = 0x8C5,
IMAGE_2118_SIZE = 0x19C,
IMAGE_2119_SIZE = 0x8FB,
IMAGE_2120_SIZE = 0x761,
IMAGE_2121_SIZE = 0x6CB,
IMAGE_2122_SIZE = 0x6F2,
IMAGE_2123_SIZE = 0x6D3,
IMAGE_2124_SIZE = 0x78B,
IMAGE_2125_SIZE = 0x73C,
IMAGE_2126_SIZE = 0x792,
IMAGE_2127_SIZE = 0x775,
IMAGE_FLAREORANGELINE_SIZE = 0x47D,
IMAGE_2129_SIZE = 0x624,
IMAGE_2130_SIZE = 0x60C,
IMAGE_2131_SIZE = 0x60C,
IMAGE_2132_SIZE = 0x60C,
IMAGE_2133_SIZE = 0x60A,
IMAGE_FLAREBLUEROUND_SIZE = 0xBC7,
IMAGE_FLAREBLUELINE_SIZE = 0x514,
IMAGE_FLAREWHITEROUND_SIZE = 0x487,
IMAGE_2137_SIZE = 0x9FD,
IMAGE_2138_SIZE = 0x95F,
IMAGE_2139_SIZE = 0x7C5,
IMAGE_2140_SIZE = 0x8A7,
IMAGE_2141_SIZE = 0x759,
IMAGE_2142_SIZE = 0x690,
IMAGE_2143_SIZE = 0x664,
IMAGE_2144_SIZE = 0x6B3,
IMAGE_2145_SIZE = 0x80E,
IMAGE_2146_SIZE = 0x79F,
IMAGE_2147_SIZE = 0x775,
IMAGE_2148_SIZE = 0x77F,
IMAGE_2149_SIZE = 0x6B9,
IMAGE_2150_SIZE = 0x646,
IMAGE_2151_SIZE = 0x649,
IMAGE_2152_SIZE = 0x654,
IMAGE_2153_SIZE = 0x580,
IMAGE_2154_SIZE = 0x50B,
IMAGE_2155_SIZE = 0x505,
IMAGE_2156_SIZE = 0x52B,
IMAGE_2157_SIZE = 0x832,
IMAGE_2158_SIZE = 0x7E7,
IMAGE_2159_SIZE = 0x7D3,
IMAGE_2160_SIZE = 0x787,
IMAGE_ROCKETAMMO_SIZE = 0x21A,
IMAGE_PLAINMINEAMMO_SIZE = 0x2E8,
IMAGE_GRENADEAMMO_SIZE = 0x38E,
IMAGE_MAGAMMO_SIZE = 0x95,
IMAGE_GLAMMO_SIZE = 0x283,
IMAGE_KNIFEAMMO_SIZE = 0x201,
IMAGE_SHOTAMMO_SIZE = 0x8E,
IMAGE_IMPACT1_SIZE = 0x3F9,
IMAGE_IMPACT2_SIZE = 0x34F,
IMAGE_IMPACT3_SIZE = 0x658,
IMAGE_IMPACT4_SIZE = 0x474,
IMAGE_IMPACTMULTI_SIZE = 0x21C,
IMAGE_IMPACTREDBRICK2_SIZE = 0x529,
IMAGE_IMPACTBRICK2_SIZE = 0x450,
IMAGE_IMPACTBRICK3_SIZE = 0x457,
IMAGE_SMOKE1_SIZE = 0x1E7,
IMAGE_SMOKE2_SIZE = 0x4E1,
IMAGE_SMOKE3_SIZE = 0x19E,
IMAGE_SMOKE4_SIZE = 0x284,
IMAGE_SMOKE5_SIZE = 0x39B,
IMAGE_SMOKE6_SIZE = 0x365,
IMAGE_SMOKEBALLS1_SIZE = 0x565,
IMAGE_SMOKEBALLS2_SIZE = 0x601,
IMAGE_SMOKEBALLS3_SIZE = 0x64C,
IMAGE_SMOKEBALLS4_SIZE = 0x63A,
IMAGE_SMOKEBALLS5_SIZE = 0x645,
IMAGE_MONITOR_BOND_SIZE = 0x38D,
IMAGE_MONITOR_LOCATION_SIZE = 0x399,
IMAGE_MONITOR_BEGINARMING_SIZE = 0x45C,
IMAGE_MONITOR_TARGET_SIZE = 0x34E,
IMAGE_MONITOR_SEVERNAYA_SIZE = 0x40F,
IMAGE_MONITOR_BREAKTARGET_SIZE = 0x42B,
IMAGE_MONITOR_AIMER_SIZE = 0x122,
IMAGE_MONITOR_EARTH_SIZE = 0x5E5,
IMAGE_MONITOR_DESKTOPBANG_SIZE = 0x5D3,
IMAGE_MONITOR_HEATMAP_SIZE = 0x59D,
IMAGE_MONITOR_3DMATH_SIZE = 0x56B,
IMAGE_MONITOR_2DMATH_SIZE = 0x587,
IMAGE_MONITOR_SATELLITE_SIZE = 0x58A,
IMAGE_MONITOR_CUBE1_SIZE = 0x17A,
IMAGE_MONITOR_EARTHFULL2_SIZE = 0x5F7,
IMAGE_MONITOR_BLUESTARS_SIZE = 0x5F6,
IMAGE_MONITOR_GALAXY1_SIZE = 0x58D,
IMAGE_MONITOR_GALAXY2_SIZE = 0x5D6,
IMAGE_MONITOR_TARGETEARTH_SIZE = 0x5EA,
IMAGE_MONITOR_GALAXY3_SIZE = 0x526,
IMAGE_MONITOR_SKATEBOARD1_SIZE = 0x1EB,
IMAGE_MONITOR_SKATEBOARD2_SIZE = 0x1CB,
IMAGE_MONITOR_SKATEBOARD3_SIZE = 0x1EC,
IMAGE_MONITOR_TALK1_SIZE = 0x43C,
IMAGE_MONITOR_TALK2_SIZE = 0x442,
IMAGE_MONITOR_TALK3_SIZE = 0x43B,
IMAGE_MONITOR_TALK4_SIZE = 0x43B,
IMAGE_MONITOR_WORLDMAP_SIZE = 0x6B6,
IMAGE_MONITOR_CUBE2_SIZE = 0x1B0,
IMAGE_MONITOR_CUBE3_SIZE = 0x1AF,
IMAGE_MONITOR_CUBE4_SIZE = 0x1B1,
IMAGE_MONITOR_SKATEBOARD4_SIZE = 0x1C2,
IMAGE_MONITOR_FIST1_SIZE = 0x31B,
IMAGE_MONITOR_FIST2_SIZE = 0x334,
IMAGE_MONITOR_FIST3_SIZE = 0x330,
IMAGE_MONITOR_FIST4_SIZE = 0x345,
IMAGE_MONITOR_SINE_SIZE = 0x1C7,
IMAGE_MONITOR_TEXT_SIZE = 0x88,
IMAGE_MONITOR_BARS_SIZE = 0x32,
IMAGE_MONITOR_SQUARES_SIZE = 0x54,
IMAGE_MONITOR_STATIC_SIZE = 0x649,
IMAGE_CLOUDS_GRAYSCALE_SIZE = 0x6B1,
IMAGE_FLARENOVA_SIZE = 0x6B5,
IMAGE_WHITEBOX_SIZE = 0x468,
IMAGE_9MMAMMO_SIZE = 0xBA,
IMAGE_RIFLEAMMO_SIZE = 0x18B,
IMAGE_GGAMMO_SIZE = 0x8F,
IMAGE_MINEAMMO_SIZE = 0x2DF,
IMAGE_PROXAMMO_SIZE = 0x2E1,
IMAGE_CROSSHAIR1_SIZE = 0x403,
IMAGE_CROSSHAIR2_SIZE = 0x1AE,
IMAGE_TIMEAMMO_SIZE = 0x2E0,
IMAGE_2239_SIZE = 0x9E1,
IMAGE_2240_SIZE = 0x9F3,
IMAGE_2241_SIZE = 0x5B5,
IMAGE_2242_SIZE = 0x4C4,
IMAGE_2243_SIZE = 0x458,
IMAGE_2244_SIZE = 0x77D,
IMAGE_2245_SIZE = 0xBD2,
IMAGE_2246_SIZE = 0x9CF,
IMAGE_2247_SIZE = 0x683,
IMAGE_2248_SIZE = 0x370,
IMAGE_2249_SIZE = 0xC8B,
IMAGE_2250_SIZE = 0x5DB,
IMAGE_2251_SIZE = 0x5E4,
IMAGE_2252_SIZE = 0x594,
IMAGE_2253_SIZE = 0x59B,
IMAGE_2254_SIZE = 0x412,
IMAGE_2255_SIZE = 0x1DE,
IMAGE_2256_SIZE = 0x2DB,
IMAGE_2257_SIZE = 0x32B,
IMAGE_2258_SIZE = 0x35C,
IMAGE_2259_SIZE = 0x2B3,
IMAGE_2260_SIZE = 0x83A,
IMAGE_2261_SIZE = 0x59E,
IMAGE_2262_SIZE = 0x6EA,
IMAGE_MONITOR_TRIANGLE_SIZE = 0x1AA,
IMAGE_2264_SIZE = 0x350,
IMAGE_2265_SIZE = 0x49B,
IMAGE_2266_SIZE = 0x700,
IMAGE_2267_SIZE = 0x7CF,
IMAGE_2268_SIZE = 0x7CB,
IMAGE_2269_SIZE = 0x6D5,
IMAGE_2270_SIZE = 0x4F4,
IMAGE_2271_SIZE = 0x4AD,
IMAGE_2272_SIZE = 0x210,
IMAGE_2273_SIZE = 0x342,
IMAGE_2274_SIZE = 0xA7,
IMAGE_2275_SIZE = 0x1C0,
IMAGE_2276_SIZE = 0x353,
IMAGE_2277_SIZE = 0x40C,
IMAGE_2278_SIZE = 0x3BA,
IMAGE_2279_SIZE = 0x400,
IMAGE_2280_SIZE = 0xE3E,
IMAGE_2281_SIZE = 0x422,
IMAGE_2282_SIZE = 0x400,
IMAGE_2283_SIZE = 0x295,
IMAGE_2284_SIZE = 0x5D5,
IMAGE_2285_SIZE = 0x223,
IMAGE_2286_SIZE = 0x258,
IMAGE_2287_SIZE = 0x242,
IMAGE_2288_SIZE = 0x1BA,
IMAGE_2289_SIZE = 0x1D3,
IMAGE_2290_SIZE = 0xFC,
IMAGE_2291_SIZE = 0x1EE,
IMAGE_2292_SIZE = 0x1BA,
IMAGE_2293_SIZE = 0x1EB,
IMAGE_2294_SIZE = 0x23E,
IMAGE_2295_SIZE = 0x563,
IMAGE_2296_SIZE = 0x1BB,
IMAGE_2297_SIZE = 0x18D,
IMAGE_2298_SIZE = 0x280,
IMAGE_2299_SIZE = 0x9D,
IMAGE_2300_SIZE = 0x314,
IMAGE_2301_SIZE = 0x3C7,
IMAGE_2302_SIZE = 0x561,
IMAGE_2303_SIZE = 0x3E3,
IMAGE_2304_SIZE = 0x4DF,
IMAGE_2305_SIZE = 0x42C,
IMAGE_2306_SIZE = 0x604,
IMAGE_2307_SIZE = 0xF1,
IMAGE_2308_SIZE = 0x337,
IMAGE_2309_SIZE = 0x44A,
IMAGE_2310_SIZE = 0x4FF,
IMAGE_2311_SIZE = 0x197,
IMAGE_2312_SIZE = 0x2FF,
IMAGE_2313_SIZE = 0x2AC,
IMAGE_2314_SIZE = 0x306,
IMAGE_2315_SIZE = 0x3E4,
IMAGE_2316_SIZE = 0x289,
IMAGE_2317_SIZE = 0x29A,
IMAGE_2318_SIZE = 0x4AF,
IMAGE_2319_SIZE = 0x43F,
IMAGE_2320_SIZE = 0x3F0,
IMAGE_2321_SIZE = 0x3F4,
IMAGE_2322_SIZE = 0x41F,
IMAGE_2323_SIZE = 0x3C6,
IMAGE_2324_SIZE = 0x41D,
IMAGE_2325_SIZE = 0x2D0,
IMAGE_2326_SIZE = 0x293,
IMAGE_2327_SIZE = 0x3E6,
IMAGE_2328_SIZE = 0x20E,
IMAGE_2329_SIZE = 0x377,
IMAGE_2330_SIZE = 0x39B,
IMAGE_2331_SIZE = 0x51C,
IMAGE_2332_SIZE = 0x2B2,
IMAGE_2333_SIZE = 0x504,
IMAGE_2334_SIZE = 0x5AD,
IMAGE_2335_SIZE = 0x2E3,
IMAGE_2336_SIZE = 0x3FF,
IMAGE_2337_SIZE = 0x1A7,
IMAGE_2338_SIZE = 0x2C2,
IMAGE_2339_SIZE = 0xED,
IMAGE_2340_SIZE = 0x2D2,
IMAGE_2341_SIZE = 0x504,
IMAGE_2342_SIZE = 0x43D,
IMAGE_2343_SIZE = 0x419,
IMAGE_2344_SIZE = 0x2B5,
IMAGE_2345_SIZE = 0x60C,
IMAGE_2346_SIZE = 0x29D,
IMAGE_2347_SIZE = 0x4EE,
IMAGE_2348_SIZE = 0x4F1,
IMAGE_2349_SIZE = 0x236,
IMAGE_2350_SIZE = 0x354,
IMAGE_2351_SIZE = 0x28E,
IMAGE_2352_SIZE = 0x3FA,
IMAGE_2353_SIZE = 0x4D2,
IMAGE_2354_SIZE = 0x584,
IMAGE_2355_SIZE = 0x3FE,
IMAGE_2356_SIZE = 0x4AB,
IMAGE_2357_SIZE = 0x474,
IMAGE_2358_SIZE = 0x470,
IMAGE_2359_SIZE = 0x440,
IMAGE_2360_SIZE = 0x4CC,
IMAGE_2361_SIZE = 0x4CF,
IMAGE_2362_SIZE = 0x4A7,
IMAGE_2363_SIZE = 0x18A,
IMAGE_2364_SIZE = 0x1A6,
IMAGE_2365_SIZE = 0x14A,
IMAGE_2366_SIZE = 0x383,
IMAGE_2367_SIZE = 0x1FC,
IMAGE_2368_SIZE = 0x185,
IMAGE_2369_SIZE = 0x3D1,
IMAGE_2370_SIZE = 0x4B4,
IMAGE_2371_SIZE = 0x575,
IMAGE_2372_SIZE = 0x40D,
IMAGE_2373_SIZE = 0x7F3,
IMAGE_2374_SIZE = 0x62D,
IMAGE_2375_SIZE = 0x30C,
IMAGE_2376_SIZE = 0x416,
IMAGE_2377_SIZE = 0x444,
IMAGE_2378_SIZE = 0x1AA,
IMAGE_2379_SIZE = 0x221,
IMAGE_2380_SIZE = 0x648,
IMAGE_2381_SIZE = 0x420,
IMAGE_2382_SIZE = 0x3FD,
IMAGE_2383_SIZE = 0x403,
IMAGE_2384_SIZE = 0x408,
IMAGE_2385_SIZE = 0x455,
IMAGE_2386_SIZE = 0x3FC,
IMAGE_2387_SIZE = 0x408,
IMAGE_2388_SIZE = 0x313,
IMAGE_2389_SIZE = 0x32D,
IMAGE_2390_SIZE = 0x27B,
IMAGE_2391_SIZE = 0x2B5,
IMAGE_2392_SIZE = 0x51A,
IMAGE_2393_SIZE = 0x3E4,
IMAGE_2394_SIZE = 0x293,
IMAGE_2395_SIZE = 0x38C,
IMAGE_2396_SIZE = 0x493,
IMAGE_2397_SIZE = 0x316,
IMAGE_2398_SIZE = 0x2DA,
IMAGE_2399_SIZE = 0x2B6,
IMAGE_2400_SIZE = 0x304,
IMAGE_2401_SIZE = 0x28E,
IMAGE_2402_SIZE = 0x37F,
IMAGE_2403_SIZE = 0x60C,
IMAGE_2404_SIZE = 0x60C,
IMAGE_2405_SIZE = 0x3BE,
IMAGE_2406_SIZE = 0x60C,
IMAGE_2407_SIZE = 0x254,
IMAGE_2408_SIZE = 0x4C9,
IMAGE_2409_SIZE = 0x60C,
IMAGE_2410_SIZE = 0x63F,
IMAGE_2411_SIZE = 0x5F1,
IMAGE_2412_SIZE = 0x6FF,
IMAGE_2413_SIZE = 0x63A,
IMAGE_2414_SIZE = 0x2EA,
IMAGE_2415_SIZE = 0x41F,
IMAGE_2416_SIZE = 0x33D,
IMAGE_2417_SIZE = 0x3A5,
IMAGE_2418_SIZE = 0x32E,
IMAGE_2419_SIZE = 0x3FA,
IMAGE_2420_SIZE = 0x361,
IMAGE_2421_SIZE = 0x71A,
IMAGE_2422_SIZE = 0x794,
IMAGE_2423_SIZE = 0x6A2,
IMAGE_2424_SIZE = 0x221,
IMAGE_2425_SIZE = 0x3F8,
IMAGE_2426_SIZE = 0x31C,
IMAGE_2427_SIZE = 0x1F1,
IMAGE_2428_SIZE = 0xC2,
IMAGE_2429_SIZE = 0x20A,
IMAGE_2430_SIZE = 0x5C9,
IMAGE_2431_SIZE = 0xAB,
IMAGE_2432_SIZE = 0x136,
IMAGE_2433_SIZE = 0x356,
IMAGE_2434_SIZE = 0x311,
IMAGE_2435_SIZE = 0x5CD,
IMAGE_2436_SIZE = 0x1E1,
IMAGE_2437_SIZE = 0x353,
IMAGE_2438_SIZE = 0x307,
IMAGE_2439_SIZE = 0x17A,
IMAGE_2440_SIZE = 0x5B9,
IMAGE_2441_SIZE = 0x33C,
IMAGE_2442_SIZE = 0x374,
IMAGE_2443_SIZE = 0x3F5,
IMAGE_2444_SIZE = 0x46C,
IMAGE_2445_SIZE = 0x302,
IMAGE_2446_SIZE = 0x4A6,
IMAGE_2447_SIZE = 0x3B1,
IMAGE_2448_SIZE = 0x2A9,
IMAGE_2449_SIZE = 0x223,
IMAGE_2450_SIZE = 0xA5,
IMAGE_2451_SIZE = 0x492,
IMAGE_2452_SIZE = 0xC4,
IMAGE_2453_SIZE = 0x1BC,
IMAGE_2454_SIZE = 0x439,
IMAGE_2455_SIZE = 0x2FC,
IMAGE_2456_SIZE = 0x308,
IMAGE_2457_SIZE = 0x55,
IMAGE_2458_SIZE = 0x20F,
IMAGE_2459_SIZE = 0x10C,
IMAGE_2460_SIZE = 0x171,
IMAGE_2461_SIZE = 0x3AE,
IMAGE_2462_SIZE = 0x714,
IMAGE_2463_SIZE = 0xB6,
IMAGE_TANKAMMO_SIZE = 0xE1,
IMAGE_2465_SIZE = 0x213,
IMAGE_2466_SIZE = 0x270,
IMAGE_2467_SIZE = 0x567,
IMAGE_2468_SIZE = 0x4F3,
IMAGE_2469_SIZE = 0x15A,
IMAGE_2470_SIZE = 0x1A2,
IMAGE_2471_SIZE = 0x252,
IMAGE_2472_SIZE = 0x288,
IMAGE_2473_SIZE = 0x35B,
IMAGE_2474_SIZE = 0x514,
IMAGE_2475_SIZE = 0x236,
IMAGE_2476_SIZE = 0x838,
IMAGE_2477_SIZE = 0x3AC,
IMAGE_2478_SIZE = 0x358,
IMAGE_2479_SIZE = 0x2F4,
IMAGE_2480_SIZE = 0x1BE,
IMAGE_2481_SIZE = 0x172,
IMAGE_2482_SIZE = 0x34D,
IMAGE_2483_SIZE = 0x55A,
IMAGE_2484_SIZE = 0x593,
IMAGE_2485_SIZE = 0x2E2,
IMAGE_2486_SIZE = 0x24F,
IMAGE_2487_SIZE = 0x2DB,
IMAGE_2488_SIZE = 0x4EF,
IMAGE_2489_SIZE = 0x4D0,
IMAGE_2490_SIZE = 0x35D,
IMAGE_2491_SIZE = 0x391,
IMAGE_2492_SIZE = 0xF,
IMAGE_2493_SIZE = 0x552,
IMAGE_2494_SIZE = 0x1D8,
IMAGE_2495_SIZE = 0x50D,
IMAGE_2496_SIZE = 0x5A8,
IMAGE_2497_SIZE = 0x4D7,
IMAGE_2498_SIZE = 0x38A,
IMAGE_2499_SIZE = 0x2E3,
IMAGE_2500_SIZE = 0x46A,
IMAGE_2501_SIZE = 0x42F,
IMAGE_2502_SIZE = 0x391,
IMAGE_2503_SIZE = 0x918,
IMAGE_2504_SIZE = 0x8AC,
IMAGE_2505_SIZE = 0x924,
IMAGE_2506_SIZE = 0x98B,
IMAGE_2507_SIZE = 0x9BB,
IMAGE_2508_SIZE = 0x99B,
IMAGE_2509_SIZE = 0x5BD,
IMAGE_2510_SIZE = 0x406,
IMAGE_2511_SIZE = 0x429,
IMAGE_2512_SIZE = 0x469,
IMAGE_2513_SIZE = 0x457,
IMAGE_2514_SIZE = 0x37C,
IMAGE_2515_SIZE = 0x535,
IMAGE_2516_SIZE = 0x769,
IMAGE_2517_SIZE = 0x7B0,
IMAGE_2518_SIZE = 0x797,
IMAGE_2519_SIZE = 0x6F4,
IMAGE_2520_SIZE = 0x5DF,
IMAGE_2521_SIZE = 0x747,
IMAGE_2522_SIZE = 0x82F,
IMAGE_2523_SIZE = 0x98A,
IMAGE_2524_SIZE = 0x91A,
IMAGE_2525_SIZE = 0x6F5,
IMAGE_2526_SIZE = 0x3D4,
IMAGE_2527_SIZE = 0x452,
IMAGE_2528_SIZE = 0x4E7,
IMAGE_2529_SIZE = 0x807,
IMAGE_2530_SIZE = 0x7A9,
IMAGE_2531_SIZE = 0x69F,
IMAGE_2532_SIZE = 0x8CD,
IMAGE_2533_SIZE = 0x6FA,
IMAGE_2534_SIZE = 0x306,
IMAGE_2535_SIZE = 0x44B,
IMAGE_2536_SIZE = 0x412,
IMAGE_2537_SIZE = 0x7EB,
IMAGE_ARCHIVES_U_SIZE = 0x925,
IMAGE_ARCHIVES_L_SIZE = 0x959,
IMAGE_CONTROL_U_SIZE = 0x9A1,
IMAGE_CONTROL_L_SIZE = 0x950,
IMAGE_FACILITY_U_SIZE = 0x77E,
IMAGE_FACILITY_L_SIZE = 0xA2E,
IMAGE_AZTEC_U_SIZE = 0x8CB,
IMAGE_AZTEC_L_SIZE = 0x91E,
IMAGE_EGYPT_U_SIZE = 0x90F,
IMAGE_EGYPT_L_SIZE = 0x8E8,
IMAGE_CAVERNS_U_SIZE = 0x7A9,
IMAGE_CAVERNS_L_SIZE = 0x9E0,
IMAGE_CRADLE_U_SIZE = 0x8BB,
IMAGE_CRADLE_L_SIZE = 0x913,
IMAGE_DAM_U_SIZE = 0x6B5,
IMAGE_DAM_L_SIZE = 0x83D,
IMAGE_DEPOT_U_SIZE = 0x85D,
IMAGE_DEPOT_L_SIZE = 0x840,
IMAGE_FRIGATE_U_SIZE = 0x62A,
IMAGE_FRIGATE_L_SIZE = 0x677,
IMAGE_JUNGLE_U_SIZE = 0x407,
IMAGE_JUNGLE_L_SIZE = 0x8B7,
IMAGE_STREETS_U_SIZE = 0x8ED,
IMAGE_STREETS_L_SIZE = 0x934,
IMAGE_RUNWAY_U_SIZE = 0x730,
IMAGE_RUNWAY_L_SIZE = 0x701,
IMAGE_BUNKERI_U_SIZE = 0x785,
IMAGE_BUNKERI_L_SIZE = 0x862,
IMAGE_BUNKERII_U_SIZE = 0x799,
IMAGE_BUNKERII_L_SIZE = 0x86E,
IMAGE_SURFACEI_U_SIZE = 0x71B,
IMAGE_SURFACEI_L_SIZE = 0x5F3,
IMAGE_SURFACEII_U_SIZE = 0x733,
IMAGE_SURFACEII_L_SIZE = 0x758,
IMAGE_SILO_U_SIZE = 0xA27,
IMAGE_SILO_L_SIZE = 0xAA4,
IMAGE_STATUE_U_SIZE = 0x761,
IMAGE_STATUE_L_SIZE = 0x6F4,
IMAGE_TRAIN_U_SIZE = 0x632,
IMAGE_TRAIN_L_SIZE = 0x68B,
IMAGE_MP_ARCHIVES_SIZE = 0x97D,
IMAGE_MP_CONTROL_SIZE = 0xA2E,
IMAGE_MP_FACILITY_SIZE = 0x998,
IMAGE_MP_AZTEC_SIZE = 0x9AE,
IMAGE_MP_CAVERNS_SIZE = 0x9A5,
IMAGE_MP_CRADLE_SIZE = 0x8AD,
IMAGE_MP_EGYPT_SIZE = 0x998,
IMAGE_MP_DAM_SIZE = 0x81C,
IMAGE_MP_DEPOT_SIZE = 0x8EE,
IMAGE_MP_FRIGATE_SIZE = 0x7DD,
IMAGE_MP_JUNGLE_SIZE = 0x884,
IMAGE_MP_STREETS_SIZE = 0x979,
IMAGE_MP_RUNWAY_SIZE = 0x864,
IMAGE_MP_BUNKER_SIZE = 0x91F,
IMAGE_MP_BUNKER2_SIZE = 0x8B6,
IMAGE_MP_SURFACE_SIZE = 0x758,
IMAGE_MP_SURFACE2_SIZE = 0x833,
IMAGE_MP_SILO_SIZE = 0xA6E,
IMAGE_MP_STATUE_SIZE = 0x837,
IMAGE_MP_TRAIN_SIZE = 0x8E6,
IMAGE_MI6_UL_SIZE = 0x529,
IMAGE_MI6_UR_SIZE = 0x51E,
IMAGE_MI6_LL_SIZE = 0x584,
IMAGE_MI6_LR_SIZE = 0x5A6,
IMAGE_BROSNAN_UL_SIZE = 0x57F,
IMAGE_BROSNAN_UR_SIZE = 0x5FD,
IMAGE_BROSNAN_LL_SIZE = 0x4CA,
IMAGE_BROSNAN_LR_SIZE = 0x60B,
IMAGE_CONNERY_UL_SIZE = 0x566,
IMAGE_CONNERY_UR_SIZE = 0x5AC,
IMAGE_CONNERY_LL_SIZE = 0x485,
IMAGE_CONNERY_LR_SIZE = 0x6E4,
IMAGE_MOORE_UL_SIZE = 0x50A,
IMAGE_MOORE_UR_SIZE = 0x568,
IMAGE_MOORE_LL_SIZE = 0x616,
IMAGE_MOORE_LR_SIZE = 0x59F,
IMAGE_DALTON_UL_SIZE = 0x594,
IMAGE_DALTON_UR_SIZE = 0x4BE,
IMAGE_DALTON_LL_SIZE = 0x54C,
IMAGE_DALTON_LR_SIZE = 0x513,
IMAGE_CLASSIFIED_L_SIZE = 0x73D,
IMAGE_CLASSIFIED_R_SIZE = 0x6E2,
IMAGE_CONFIDENTIAL_L_SIZE = 0x6F4,
IMAGE_CONFIDENTIAL_R_SIZE = 0x6DE,
IMAGE_EYESONLY_L_SIZE = 0x66F,
IMAGE_EYESONLY_R_SIZE = 0x6C4,
IMAGE_FORYOUR_L_SIZE = 0x59B,
IMAGE_FORYOUR_R_SIZE = 0x75A,
IMAGE_FOLDERTEX_SIZE = 0x3A3,
IMAGE_OHMSS_L_SIZE = 0x67A,
IMAGE_OHMSS_R_SIZE = 0x6F2,
IMAGE_PAPERTEX_SIZE = 0x28A,
IMAGE_MI6_SIZE = 0x542,
IMAGE_DOT_SIZE = 0x35,
IMAGE_BORIS_UL_SIZE = 0x4B8,
IMAGE_BORIS_UR_SIZE = 0x523,
IMAGE_BORIS_LL_SIZE = 0x527,
IMAGE_BORIS_LR_SIZE = 0x5FA,
IMAGE_OURUMOV_UL_SIZE = 0x614,
IMAGE_OURUMOV_UR_SIZE = 0x509,
IMAGE_OURUMOV_LL_SIZE = 0x598,
IMAGE_OURUMOV_LR_SIZE = 0x5BB,
IMAGE_TREVELYAN_UL_SIZE = 0x4E7,
IMAGE_TREVELYAN_UR_SIZE = 0x4ED,
IMAGE_TREVELYAN_LL_SIZE = 0x413,
IMAGE_TREVELYAN_LR_SIZE = 0x432,
IMAGE_VALENTIN_UL_SIZE = 0x4DA,
IMAGE_VALENTIN_UR_SIZE = 0x514,
IMAGE_VALENTIN_LL_SIZE = 0x4AB,
IMAGE_VALENTIN_LR_SIZE = 0x4CF,
IMAGE_XENIA_UL_SIZE = 0x552,
IMAGE_XENIA_UR_SIZE = 0x4BC,
IMAGE_XENIA_LL_SIZE = 0x536,
IMAGE_XENIA_LR_SIZE = 0x60F,
IMAGE_NATALYA_UL_SIZE = 0x543,
IMAGE_NATALYA_UR_SIZE = 0x54E,
IMAGE_NATALYA_LL_SIZE = 0x4BD,
IMAGE_NATALYA_LR_SIZE = 0x4C3,
IMAGE_BARON_UL_SIZE = 0x4D3,
IMAGE_BARON_UR_SIZE = 0x489,
IMAGE_BARON_LL_SIZE = 0x697,
IMAGE_BARON_LR_SIZE = 0x642,
IMAGE_JAWS_UL_SIZE = 0x567,
IMAGE_JAWS_UR_SIZE = 0x51C,
IMAGE_JAWS_LL_SIZE = 0x53D,
IMAGE_JAWS_LR_SIZE = 0x56D,
IMAGE_MAYDAY_UL_SIZE = 0x55E,
IMAGE_MAYDAY_UR_SIZE = 0x45B,
IMAGE_MAYDAY_LL_SIZE = 0x5A5,
IMAGE_MAYDAY_LR_SIZE = 0x5AB,
IMAGE_ODDJOB_UL_SIZE = 0x554,
IMAGE_ODDJOB_UR_SIZE = 0x516,
IMAGE_ODDJOB_LL_SIZE = 0x57F,
IMAGE_ODDJOB_LR_SIZE = 0x4BC,
IMAGE_2672_SIZE = 0xA0,
IMAGE_2673_SIZE = 0xF8,
IMAGE_2674_SIZE = 0x397,
IMAGE_2675_SIZE = 0xBC,
IMAGE_2676_SIZE = 0x435,
IMAGE_2677_SIZE = 0xA0C,
IMAGE_2678_SIZE = 0x735,
IMAGE_2679_SIZE = 0x2D6,
IMAGE_2680_SIZE = 0x8DE,
IMAGE_2681_SIZE = 0x277,
IMAGE_RANDOM_UL_SIZE = 0x3B3,
IMAGE_RANDOM_UR_SIZE = 0x3B4,
IMAGE_RANDOM_LL_SIZE = 0x2C1,
IMAGE_RANDOM_LR_SIZE = 0x2C4,
IMAGE_MP_TEMPLE_SIZE = 0x976,
IMAGE_MP_BASEMENT_SIZE = 0x9C8,
IMAGE_MP_COMPLEX_SIZE = 0xA39,
IMAGE_MP_CAVES_SIZE = 0x86F,
IMAGE_2690_SIZE = 0x380,
IMAGE_MISHKIN_LL_SIZE = 0x544,
IMAGE_MISHKIN_LR_SIZE = 0x513,
IMAGE_MISHKIN_UR_SIZE = 0x502,
IMAGE_MISHKIN_UL_SIZE = 0x4E4,
IMAGE_MP_RANDOM_SIZE = 0x1A3,
IMAGE_2696_SIZE = 0x201,
IMAGE_2697_SIZE = 0x53D,
# 1371 "src/bondconstants.h" 2
    IMGAGESIZES_COUNT
} IMGAGESIZES;



typedef enum LEVELID
{

    LEVELID_NONE = -1,
    LEVELID_DEFAULT,

    LEVELID_BUNKER1 = 9,

    LEVELID_SILO = 20,

    LEVELID_STATUE = 22,
    LEVELID_CONTROL,
    LEVELID_ARCHIVES,
    LEVELID_TRAIN,
    LEVELID_FRIGATE,
    LEVELID_BUNKER2,
    LEVELID_AZTEC,
    LEVELID_STREETS,
    LEVELID_DEPOT,
    LEVELID_COMPLEX,
    LEVELID_EGYPT,
    LEVELID_DAM,
    LEVELID_FACILITY,
    LEVELID_RUNWAY,
    LEVELID_SURFACE,
    LEVELID_JUNGLE,
    LEVELID_TEMPLE,
    LEVELID_CAVERNS,
    LEVELID_CITADEL,
    LEVELID_CRADLE,
    LEVELID_SHO,
    LEVELID_SURFACE2,
    LEVELID_ELD,
    LEVELID_BASEMENT,
    LEVELID_STACK,
    LEVELID_LUE,
    LEVELID_LIBRARY,
    LEVELID_RIT,
    LEVELID_CAVES,
    LEVELID_EAR,
    LEVELID_LEE,
    LEVELID_LIP,
    LEVELID_CUBA,
    LEVELID_WAX,
    LEVELID_PAM,
    LEVELID_MAX,
    LEVELID_TITLE = 90,
    LEVELID_BUNKER2_MP = LEVELID_BUNKER2 + ENVIRONMENTDATA_PLAYERS_4,
    LEVELID_ARCHIVES_MP = LEVELID_ARCHIVES + ENVIRONMENTDATA_PLAYERS_4,
    LEVELID_CAVERNS_MP = LEVELID_CAVERNS + ENVIRONMENTDATA_PLAYERS_4,
    LEVELID_FACILITY_MP = LEVELID_FACILITY + ENVIRONMENTDATA_PLAYERS_4,
    LEVELID_EGYPT_MP = LEVELID_EGYPT + ENVIRONMENTDATA_PLAYERS_4
} LEVELID;

typedef enum LEVEL_SOLO_SEQUENCE
{
    SP_LEVEL_DAM,
    SP_LEVEL_FACILITY,
    SP_LEVEL_RUNWAY,
    SP_LEVEL_SURFACE1,
    SP_LEVEL_BUNKER1,
    SP_LEVEL_SILO,
    SP_LEVEL_FRIGATE,
    SP_LEVEL_SURFACE2,
    SP_LEVEL_BUNKER2,
    SP_LEVEL_STATUE,
    SP_LEVEL_ARCHIVES,
    SP_LEVEL_STREETS,
    SP_LEVEL_DEPOT,
    SP_LEVEL_TRAIN,
    SP_LEVEL_JUNGLE,
    SP_LEVEL_CONTROL,
    SP_LEVEL_CAVERNS,
    SP_LEVEL_CRADLE,
    SP_LEVEL_AZTEC,
    SP_LEVEL_EGYPT,
    SP_LEVEL_MAX
} LEVEL_SOLO_SEQUENCE;

typedef enum LEVEL_INDEX
{
    LEVEL_INDEX_SEVBUNKER,
    LEVEL_INDEX_SILO,
    LEVEL_INDEX_STATUE,
    LEVEL_INDEX_CONTROL,
    LEVEL_INDEX_ARCH,
    LEVEL_INDEX_TRA,
    LEVEL_INDEX_DEST,
    LEVEL_INDEX_SEVB,
    LEVEL_INDEX_AZT,
    LEVEL_INDEX_PETE,
    LEVEL_INDEX_DEPO,
    LEVEL_INDEX_REF,
    LEVEL_INDEX_CRYP,
    LEVEL_INDEX_DAM,
    LEVEL_INDEX_ARK,
    LEVEL_INDEX_RUN,
    LEVEL_INDEX_SEVX,
    LEVEL_INDEX_JUN,
    LEVEL_INDEX_DISH,
    LEVEL_INDEX_CAVE,
    LEVEL_INDEX_CAT,
    LEVEL_INDEX_CRAD,
    LEVEL_INDEX_SHO,
    LEVEL_INDEX_SEVXB,
    LEVEL_INDEX_ELD,
    LEVEL_INDEX_IMP,
    LEVEL_INDEX_ASH,
    LEVEL_INDEX_LUE,
    LEVEL_INDEX_AME,
    LEVEL_INDEX_RIT,
    LEVEL_INDEX_OAT,
    LEVEL_INDEX_EAR,
    LEVEL_INDEX_LEE,
    LEVEL_INDEX_LIP,
    LEVEL_INDEX_LEN,
    LEVEL_INDEX_WAX,
    LEVEL_INDEX_PAM,
    LEVEL_INDEX_X,
    LEVEL_INDEX_MAX = LEVEL_INDEX_X
} LEVEL_INDEX;

typedef enum TEXT_ALIGNMENT
{
    LEFT_ALIGN,
    CENTER_ALIGN,
    RIGHT_ALIGN,
    TOP_ALIGN = LEFT_ALIGN,
    BOTTOM_ALIGN = RIGHT_ALIGN
} TEXT_ALIGNMENT;



typedef enum MENU
{
    MENU_INVALID = -1,
    MENU_LEGAL_SCREEN,
    MENU_NINTENDO_LOGO,
    MENU_RAREWARE_LOGO,
    MENU_EYE_INTRO,
    MENU_GOLDENEYE_LOGO,
    MENU_FILE_SELECT,
    MENU_MODE_SELECT,
    MENU_MISSION_SELECT,
    MENU_DIFFICULTY,
    MENU_007_OPTIONS,
    MENU_BRIEFING,
    MENU_RUN_STAGE,
    MENU_MISSION_FAILED,
    MENU_MISSION_COMPLETE,
    MENU_MP_OPTIONS,
    MENU_MP_CHAR_SELECT,
    MENU_MP_HANDICAP,
    MENU_MP_CONTROL_STYLE,
    MENU_MP_STAGE_SELECT,
    MENU_MP_SCENARIO_SELECT,
    MENU_MP_TEAMS,
    MENU_CHEAT,
    MENU_NO_CONTROLLERS,
    MENU_SWITCH_SCREENS,
    MENU_DISPLAY_CAST,
    MENU_SPECTRUM_EMU,
    MENU_MAX
} MENU;

typedef enum mission_setup_type
{
    MISSION_PART,
    MISSION_HEADER
} mission_setup_type;

typedef enum MISSION_BRIEFING
{
    BRIEF_OVERVIEW,
    BRIEF_M,
    BRIEF_Q,
    BRIEF_MONEYPENNY
} MISSION_BRIEFING;

typedef enum MODELNODE_OPCODE
{
    MODELNODE_OPCODE_NULL,
    MODELNODE_OPCODE_HEADER,
    MODELNODE_OPCODE_GROUP,
    MODELNODE_OPCODE_OP03,
    MODELNODE_OPCODE_DL,
    MODELNODE_OPCODE_OP05,
    MODELNODE_OPCODE_OP06,
    MODELNODE_OPCODE_OP07,
    MODELNODE_OPCODE_LOD,
    MODELNODE_OPCODE_BSP,
    MODELNODE_OPCODE_BBOX,
    MODELNODE_OPCODE_OP11,
    MODELNODE_OPCODE_GUNFIRE,
    MODELNODE_OPCODE_SHADOW,
    MODELNODE_OPCODE_OP14,
    MODELNODE_OPCODE_INTERLINK,
    MODELNODE_OPCODE_OP16,
    MODELNODE_OPCODE_OP17,
    MODELNODE_OPCODE_SWITCH,
    MODELNODE_OPCODE_OP19,
    MODELNODE_OPCODE_OP20,
    MODELNODE_OPCODE_GROUPSIMPLE,
    MODELNODE_OPCODE_DLPRIMARY,
    MODELNODE_OPCODE_HEAD,
    MODELNODE_OPCODE_DLCOLLISION,
    MODELNODE_OPCODE_MAX
} MODELNODE_OPCODE;

typedef enum MODELNODE_CHILD
{
    MODELNODE_CHILD_NULL,
    MODELNODE_CHILD_VTX,
    MODELNODE_CHILD_IMAGE,
    MODELNODE_CHILD_TRI,
    MODELNODE_CHILD_MAX
} MODELNODE_CHILD;

typedef enum BOND
{
    BOND_BROSNAN,
    BOND_CONNERY,
    BOND_DALTON,
    BOND_MOORE
} BOND;

typedef enum MP_STAGE_SELECTED
{
    MP_STAGE_RANDOM,
    MP_STAGE_TEMPLE,
    MP_STAGE_COMPLEX,
    MP_STAGE_CAVES,
    MP_STAGE_LIBRARY,
    MP_STAGE_BASEMENT,
    MP_STAGE_STACK,
    MP_STAGE_FACILITY,
    MP_STAGE_BUNKER,
    MP_STAGE_ARCHIVES,
    MP_STAGE_CAVERNS,
    MP_STAGE_EGYPT,
    MP_STAGE_SELECTED_MAX
} MP_STAGE_SELECTED;

typedef enum MPMENU
{
    MENU_GOWOC,
    MENU_LOSSES,
    MENU_KILLS,
    MENU_SCORES,
    MENU_PAUSE,
    MENU_EXIT,
    MENU_6,
    MENU_7
} MPMENU;

typedef enum MPSCENARIOS
{
    SCENARIO_NORMAL,
    SCENARIO_YOLT,
    SCENARIO_TLD,
    SCENARIO_MWTGG,
    SCENARIO_LTK,
    SCENARIO_2v2,
    SCENARIO_3v1,
    SCENARIO_2v1,
    MPSCENARIOS_MAX
} MPSCENARIOS;

typedef enum MUSIC_TRACKS
{
    M_NONE,
    M_SHORT_SOLO_DEATH,
    M_INTRO,
    M_TRAIN,
    M_DEPOT,
    M_MPTHEME,
    M_CITADEL,
    M_FACILITY,
    M_CONTROL,
    M_DAM,
    M_FRIGATE,
    M_ARCHIVES,
    M_SILO,
    M_MPTHEME2,
    M_STREETS,
    M_BUNKER1,
    M_BUNKER2,
    M_STATUE,
    M_ELEVATOR_CONTROL,
    M_CRADLE,
    M_UNK,
    M_ELEVATOR_WC,
    M_EGYPTIAN,
    M_FOLDERS,
    M_WATCH,
    M_AZTEC,
    M_WATERCAVERNS,
    M_DEATHSOLO,
    M_SURFACE2,
    M_TRAINX,
    M_UNK2,
    M_FACILITYX,
    M_DEPOTX,
    M_CONTROLX,
    M_WATERCAVERNSX,
    M_DAMX,
    M_FRIGATEX,
    M_ARCHIVESX,
    M_SILOX,
    M_EGYPTIANX,
    M_STREETSX,
    M_BUNKER1X,
    M_BUNKER2X,
    M_JUNGLEX,
    M_INTROSWOOSH,
    M_STATUEX,
    M_AZTECX,
    M_EGYPTX,
    M_CRADLEX,
    M_CUBA,
    M_RUNWAY,
    M_RUNWAYPLANE,
    M_MPTHEME3,
    M_WIND,
    M_GUITARGLISS,
    M_JUNGLE,
    M_RUNWAYX,
    M_SURFACE1,
    M_MPDEATH,
    M_SURFACE2X,
    M_SURFACE2END,
    M_STATUEPART,
    M_END_SOMETHING
} MUSIC_TRACKS;
# 1772 "src/bondconstants.h"
typedef enum OPTIONS
{
    PLAYER_OPTION_LOOK,
    PLAYER_OPTION_AUTOAIM,
    PLAYER_OPTION_AIM,
    PLAYER_OPTION_SIGHT,
    PLAYER_OPTION_LOOKAHEAD,
    PLAYER_OPTION_AMMODISPLAY,
    PLAYER_OPTION_SCREEN,
    PLAYER_OPTION_RATIO
} OPTIONS;

typedef enum PLAYER_ID
{
    PLAYER_1,
    PLAYER_2,
    PLAYER_3,
    PLAYER_4
} PLAYER_ID;
# 1818 "src/bondconstants.h"
typedef enum RGBA_ENUM
{
    RED,
    GREEN,
    BLUE,
    ALPHA
} RGBA_ENUM;


typedef enum SFX_ID
{
    NOTHING_SFX,
    ROCKET_LAUNCH_SFX,
    GLASS_SHATTERING_SFX,
    KNIFE_HIT_WALL_SFX,
    GRENADE_THROW_SFX,
    GRENADE_THROW_QUIET_SFX,
    GRENADE_THROW_FAINT_SFX,
    TRAIN_SLIDE_DOOR_SLIDE_SFX,
    TRAIN_RAILS_SFX,
    TRAIN_RAILS2_SFX,
    WATCH_INTERFERENCE_SFX,
    GUN_TANK2BIG_1_SFX,
    GUN_TANK2BIGBIG_1_SFX,
    GET_HIT_GIRL1_SFX,
    GET_HIT_GIRL2_SFX,
    GET_HIT_GIRL3_SFX,
    BEEP_SFX,
    BEEP_QUIET_SFX,
    OPTION_CLICK2_SFX,
    RICO_12_GBU_A_SFX,
    RICO_12_GBU_B_SFX,
    RICO_12_GBU_C_SFX,
    RICO_12_GBU_D_SFX,
    RICO_6_TAJ_A_SFX,
    RICO_6_TAJ_B_SFX,
    RICO_6_TAJ_C_SFX,
    RICO_6_TAJ_D_SFX,
    RICO_8_AFDM_A_SFX,
    RICO_8_AFDM_B_SFX,
    RICO_8_AFDM_C_SFX,
    RICO_8_AFDM_D_SFX,
    RICO_4_A_SFX,
    RICO_4_B_SFX,
    RICO_4_C_SFX,
    RICO_4_D_SFX,
    RICO_5_A_SFX,
    RICO_5_B_SFX,
    RICO_5_C_SFX,
    RICO_5_D_SFX,
    RICO_6_HBBA_A_SFX,
    RICO_6_HBBA_B_SFX,
    RICO_6_HBBA_C_SFX,
    RICO_6_HBBA_D_SFX,
    OPTION_CHOOSE_SFX,
    UNKNOWN1_SFX,
    DROP_GUN_SFX,
    GUN_SILPPK_A_SFX,
    PUNCH1_SFX,
    PUNCH2_SFX,
    PUNCH3_SFX,
    GUN_RIFLECOCK_SFX,
    TRAIN_CLUTTER3_SFX,
    TRAIN_CLUTTER3B_SFX,
    TRAIN_CLUTTER3C_SFX,
    EVIL_LAUGH_SFX,
    EVIL_LAUGH_QUIET_SFX,
    EVIL_LAUGH_FAINT_SFX,
    EVIL_LAUGH_HUSH_SFX,
    HELI_RUN_SFX,
    HELI_FLY_SFX,
    ENGINE_ROOM_SFX,
    TRAIN_STOP_STILL_SFX,
    TANK_SFX,
    TRAIN_STOP_SFX,
    TRAIN_GO_SFX,
    TRUCK_RUN_SFX,
    TRUCK_START_SFX,
    TRUCK_ENGINE_SFX,
    BOND_GET_HIT1_SFX,
    HIT_BULLET_FLESH_SFX,
    HIT_BULLET_GLASS_SFX,
    HIT_GLASS_SMASH_SFX,
    HIT_BULLET_METAL_A_SFX,
    HIT_BULLET_METAL_B_SFX,
    HIT_BULLET_SNOW_SFX,
    HIT_BULLET_WOOD_SFX,
    HIT_BULLET_WATER_SFX,
    PAPER_TURN_SFX,
    PAPER_TURN_2_SFX,
    COPY_FILE_SFX,
    RADIO_MESSAGE_SFX,
    ARMOUR_COLLECT_SFX,
    OBJ_REGEN_SFX,
    DOOR_DECODER_SFX,
    GIRL_GET_HIT1_SFX,
    CONSOLE_OFF_SFX,
    CONSOLE_OFF2_SFX,
    CONSOLE_ON2_SFX,
    CONSOLE_ON3_SFX,
    EMPTY_GUN_FIRE_SFX,
    SHELL_CASE_SFX,
    RICO_LASER1_SFX,
    RICO_LASER2_SFX,
    RICO_LASER3_SFX,
    RADIO_SFX,
    KNIFE_THROW1_SFX,
    KNIFE_THROW2_SFX,
    KNIFE_THROW3_SFX,
    COUGH_SFX,
    COUGH2_SFX,
    GUN_TASER_SFX,
    GUN_TASER_LOOP_SFX,
    GAS_HISS_SFX,
    UNKNOWN2_SFX,
    UNKNOWN3_SFX,
    PUNCHING_AIR_SFX,
    GUN_B1_MGUN3_3_SFX,
    GUN_B2_HEAVY_SFX,
    GUN_UNKNOWN_SFX,
    GUN_B4_BOLTACTION_SFX,
    GUN_B5_WINC44_SFX,
    GUN_RIFLE7BIG_1_SFX,
    GUN_B8_ANOTHER_SFX,
    GUN_B9_CANNON_SFX,
    GUN_GRENADE_LAUNCHER_SFX,
    GUN_UNKNOWN2_SFX,
    GUN_B12_FULLAMRIFLE_SFX,
    GUN_B13_M60AMMGUN_SFX,
    GUN_M60AMMGUN_3_SFX,
    GUN_UNKNOWN3_SFX,
    HIT_METAL_OBJECT1_SFX,
    GUN_B17_RIFLE_SFX,
    CART_SPENT_SFX,
    BODY_FALL_C1_SFX,
    BODY_FALL_C2_SFX,
    BODY_FALL_C3_SFX,
    BODY_FALL_D1_SFX,
    BODY_FALL_D2_SFX,
    BODY_FALL_D3_SFX,
    BODY_FALL_D4_SFX,
    BODY_FALL_E1_SFX,
    BODY_FALL_E2_SFX,
    BODY_FALL_E3_SFX,
    BODY_ROLLOVER_SFX,
    GET_HIT_MALE0_SFX,
    GET_HIT_MALE1_SFX,
    GET_HIT_MALE2_SFX,
    GET_HIT_MALE3_SFX,
    GET_HIT_MALE4_SFX,
    GET_HIT_MALE5_SFX,
    GET_HIT_MALE6_SFX,
    GET_HIT_MALE7_SFX,
    GET_HIT_MALE8_SFX,
    GET_HIT_MALE9_SFX,
    GET_HIT_MALE10_SFX,
    GET_HIT_MALE11_SFX,
    GET_HIT_MALE12_SFX,
    GET_HIT_MALE13_SFX,
    GET_HIT_MALE14_SFX,
    GET_HIT_MALE15_SFX,
    GET_HIT_MALE16_SFX,
    GET_HIT_MALE17_SFX,
    GET_HIT_MALE18_SFX,
    GET_HIT_MALE19_SFX,
    GET_HIT_MALE20_SFX,
    GET_HIT_MALE21_SFX,
    GET_HIT_MALE22_SFX,
    GET_HIT_MALE23_SFX,
    GET_HIT_MALE24_SFX,
    CAMERA_BEEP1_SFX,
    BING_SFX,
    ALARM1_SFX,
    ALARM2_SFX,
    ALARM3_SFX,
    RICO_EAR_WHISTLE1_SFX,
    RICO_EAR_WHISTLE2_SFX,
    RICO_EAR_WHISTLE3_SFX,
    RICO_EAR_WHISTLE4_SFX,
    RICO_EAR_WHISTLE5_SFX,
    EXPLOSION_2A_SFX,
    EXPLOSION_2B_SFX,
    EXPLOSION_3_SFX,
    EXPLOSION_4A_SFX,
    EXPLOSION_4B_SFX,
    EXPLOSION_5A_SFX,
    EXPLOSION_5B_SFX,
    EXPLOSION_5C_SFX,
    EXPLOSION_6_SFX,
    EXPLOSION_7_SFX,
    EXPLOSION_8_SFX,
    EXPLOSION_9_SFX,
    EXPLOSION_1B_SFX,
    EXPLOSION_1C_SFX,
    CRUSHED_YELL_SFX,
    KEY_SEQUENCE_SFX,
    HIT_BULLET_DIRT2_SFX,
    ALARM_SWITCH_SFX,
    DOOR_WOOD_CLOSE_SFX,
    DOOR_WOOD_OPEN_SFX,
    ATOMIC_BOMB_SFX,
    KEY_ANALYSER2_SFX,
    DOOR_WOOD_SLIDE_SFX,
    TRAIN_SLIDE_DOOR_CATCH_SFX,
    GAS_LEAK_SFX,
    DOOR_SHUTTER_OPEN_SFX,
    DOOR_SHUTTER_CLOSE_SFX,
    DOOR_METAL_OPEN_SFX,
    DOOR_METAL_CLOSE_SFX,
    CONSOLE_ON_SFX,
    DOOR_METAL_CLOSE2_SFX,
    DOOR_METAL_OPEN3_SFX,
    DOOR_METAL_CLOSE3_SFX,
    METAL_SLIDE_OPEN_SFX,
    METAL_SLIDE_CLOSE_SFX,
    METAL_SLIDE_LOOP_SFX,
    UNKNOWN4_SFX,
    UNKNOWN5_SFX,
    UNKNOWN6_SFX,
    HIT_BULLET_STONE1_SFX,
    HIT_BULLET_STONE2_SFX,
    DOOR_SMART_CATCH1_SFX,
    DOOR_SMART_SLIDE1_SFX,
    HIT_BULLET_TILE_SFX,
    TANK_CRUSH_MAN_SFX,
    HEAVY_SLIDE_OPEN_SFX,
    HEAVY_SLIDE_CLOSE_SFX,
    HEAVY_SINGLE_LOOP_SFX,
    HIT_BULLET_WOOD2_SFX,
    DOOR_HYDRAL_CLOSE_SFX,
    DOOR_HYDRAL_OPEN_SFX,
    HIT_BULLET_DIRT1_SFX,
    HIT_METAL_OBJECT2_SFX,
    DOOR_LOCK_SFX,
    TRAIN_CLUTTER2_SFX,
    HIT_BULLET_MUD3_SFX,
    DOOR_SLIDE_STONE_OPEN_SFX,
    DOOR_SLIDE_STONE_CLOSE_SFX,
    DATA_DOWNLOAD_SFX,
    LASER_GUN_SFX,
    KEYCARD_SFX,
    HIT_BULLET_MUD2_SFX,
    HIT_BULLET_MUD1_SFX,
    PICKUP_GUN_SFX,
    PICKUP_KNIFE_SFX,
    PICKUP_AMMO_SFX,
    PICKUP_MINE_SFX,
    WATCH_STATIC_SFX,
    WATCH_ON_SFX,
    WATCH_OFF_SFX,
    HIT_BULLET_METAL_A3_SFX,
    HIT_BULLET_METAL_A4_SFX,
    ATTACH_MINE_SFX,
    PICKUP_LASER_SFX,
    WATCH_DETONATE_MINE_SFX,
    CAMERA_CLICK_SFX,
    KEY_ANALYSER_SFX,
    MAGNETIC_HUM_SFX,
    BOMB_DEFUSE_SFX,
    BI_PLANE_SFX,
    TRAIN_CLUTTER_SFX,
    TRAIN_CLUTTERB_SFX,
    TRAIN_CLUTTERC_SFX,
    TRAIN_CLUTTERD_SFX,
    GUN_B9_CANNON_SHORT_SFX,
    UNKNOWN_QUIET_SFX,
    CAMERA_ZOOM_LOOP_SFX,
    CAMERA_ZOOM_STOP_SFX,
    SNEEZE_SFX,
    RARELOGO_SFX,
    RARELOGO_QUIET_SFX,
    RARELOGO_FAINT_SFX,
    BIG_CLANK_SFX
} SFX_ID;
# 2360 "src/bondconstants.h"
typedef enum SHOT_REGISTER
{
    SHOT_REGISTER_TOTAL,
    SHOT_REGISTER_HEAD,
    SHOT_REGISTER_BODY,
    SHOT_REGISTER_LIMB,
    SHOT_REGISTER_GUN,
    SHOT_REGISTER_HAT,
    SHOT_REGISTER_OBJECT
} SHOT_REGISTER;

typedef enum SPEED
{
    SPEED_WALK,
    SPEED_RUN,
    SPEED_SPRINT
} SPEED;

typedef enum STAGESTATUS
{
    STAGESTATUS_LOCKED,
    STAGESTATUS_UNLOCKED,
    STAGESTATUS_NOTCOMPLETED,
    STAGESTATUS_COMPLETED
} STAGESTATUS;

typedef enum TEXTBANK_LEVEL_INDEX
{
    LNULL,
    LAME,
    LARCH,
    LARK,
    LASH,
    LAZT,
    LCAT,
    LCAVE,
    LAREC,
    LCRAD,
    LCRYP,
    LDAM,
    LDEPO,
    LDEST,
    LDISH,
    LEAR,
    LELD,
    LIMP,
    LJUN,
    LLEE,
    LLEN,
    LLIP,
    LLUE,
    LOAT,
    LPAM,
    LPETE,
    LREF,
    LRIT,
    LRUN,
    LSEVB,
    LSEV,
    LSEVX,
    LSEVXB,
    LSHO,
    LSILO,
    LSTAT,
    LTRA,
    LWAX,
    LGUN,
    LTITLE,
    LMPMENU,
    LPROPOBJ,
    LMPWEAPONS,
    LOPTIONS,
    LMISC
} TEXTBANK_LEVEL_INDEX;
# 2444 "src/bondconstants.h"
typedef enum WATCH_ANIMATION_STATE_IDS {
    WATCH_ANIMATION_0x0 = 0,
    WATCH_ANIMATION_0x1,
    WATCH_ANIMATION_0x2,
    WATCH_ANIMATION_0x3,
    WATCH_ANIMATION_0x4,
    WATCH_ANIMATION_0x5,
    WATCH_ANIMATION_0x6,
    WATCH_ANIMATION_0x7,
    WATCH_ANIMATION_0x8,
    WATCH_ANIMATION_0x9,
    WATCH_ANIMATION_0xa,
    WATCH_ANIMATION_0xb,
    WATCH_ANIMATION_0xc,
    WATCH_ANIMATION_0xd
} WATCH_ANIMATION_STATE;

typedef enum WATCH_BRIEFING_PAGE
{
    BRIEFING_INVALID = -1,
    BRIEFING_TITLE,
    BRIEFING_OVERVIEW,
    BRIEFING_M,
    BRIEFING_Q,
    BRIEFING_MONEYPENNY
} WATCH_BRIEFING_PAGE;


typedef enum AWARD {

    AWARD_MOSTSUICIDAL = 0x00001,
    AWARD_WHONEEDSAMMO = 0x00002,
    AWARD_WHERESTHEARMOUR = 0x00004,
    AWARD_ACNEGATIVE10 = 0x00008,
    AWARD_MARKSMANSHIP = 0x00010,
    AWARD_MOSTPROFESSIONAL = 0x00020,
    AWARD_MOSTDEADLY = 0x00040,
    AWARD_MOSTHARMLESS = 0x00080,
    AWARD_MOSTCOWARDLY = 0x00100,
    AWARD_MOSTFRANTIC = 0x00200,
    AWARD_MOSTHONORABLE = 0x00400,
    AWARD_MOSTDISHONORABLE = 0x00800,
    AWARD_SHORTESTINNINGS = 0x01000,
    AWARD_LONGESTINNINGS = 0x02000,
    AWARD_DOUBLEKILL = 0x04000,
    AWARD_TRIPLEKILL = 0x08000,
    AWARD_QUADKILL = 0x10000
} AWARD;


typedef enum WAYMODE
{
    WAYMODE_0,
    WAYMODE_1,
    WAYMODE_2,
    WAYMODE_3,
    WAYMODE_4,
    WAYMODE_5,
    WAYMODE_MAGIC
} WAYMODE;

typedef enum SPSEGMENT
{
    SPSEGMENT_PHYSICAL = 0,
    SPSEGMENT_UNKNOWN = 1,
    SPSEGMENT_GETITLE = 2,
    SPSEGMENT_MODEL_MTX = 3,
    SPSEGMENT_MODEL_VTX = 4,
    SPSEGMENT_MODEL_COL1 = 5,
    SPSEGMENT_MODEL_COL2 = 6,
    SPSEGMENT_BG_COL = 13,
    SPSEGMENT_BG_VTX = 14,
    SPSEGMENT_BG_DL = 15
} SPSEGMENT;

typedef enum TVCMD
{
    TVCMD_STOPSCROLL = 0x00,
    TVCMD_SCROLLRELX = 0x01,
    TVCMD_SCROLLRELY = 0x02,
    TVCMD_SCROLLABSX = 0x03,
    TVCMD_SCROLLABSY = 0x04,
    TVCMD_SCALEABSX = 0x05,
    TVCMD_SCALEABSY = 0x06,
    TVCMD_SETTEXTURE = 0x07,
    TVCMD_PAUSE = 0x08,
    TVCMD_SETCMDLIST = 0x09,
    TVCMD_RANDSETCMDLIST = 0x0a,
    TVCMD_RESTART = 0x0b,
    TVCMD_YIELD = 0x0c,
    TVCMD_SETCOLOUR = 0x0d,
    TVCMD_ROTATEABS = 0x0e,
    TVCMD_ROTATEREL = 0x0f
} TVCMD;

enum CCRMLUT
{
    CCRMLUT_UNKNOWN,
    CCRMLUT_PRIMARY_ADDFOG,
    CCRMLUT_BILLBOARD,
    CCRMLUT_WATER,
    CCRMLUT_CLOUD,
    CCRMLUT_SECONDARY_ADDFOG,
    CCRMLUT_PRIMARY,
    CCRMLUT_SECONDARY,
    CCRMLUT_WALLETBOND,
    CCRMLUT_FIXFOGALPHA3
};

#pragma region Object Instance Stuff
    typedef enum BODIES
    {
        BODY_Jungle_Commando,
        BODY_St_Petersburg_Guard,
        BODY_Russian_Soldier,
        BODY_Russian_Infantry,
        BODY_Janus_Special_Forces,
        BODY_Brosnan_Tuxedo,





        BODY_Boris,
        BODY_Ourumov,
        BODY_Trevelyan_Janus,
        BODY_Trevelyan_006,
        BODY_Valentin_,
        BODY_Xenia,
        BODY_Baron_Samedi,
        BODY_Jaws,
        BODY_Mayday,
        BODY_Oddjob,
        BODY_Natalya_Skirt,
        BODY_Janus_Marine,
        BODY_Russian_Commandant,
        BODY_Siberian_Guard_1_Mishkin,
        BODY_Naval_Officer,
        BODY_Siberian_Special_Forces,
        BODY_Special_Operations_Uniform,
        BODY_Formal_Wear,
        BODY_Jungle_Fatigues,
        BODY_Parka,
        BODY_Unused_Female,
        BODY_Rosika,
        BODY_Scientist_2_Female,
        BODY_Civilian_1_Female,
        BODY_Unused_Male_1,
        BODY_Unused_Male_2,
        BODY_Civilian_4,
        BODY_Civilian_2,
        BODY_Civilian_3,
        BODY_Scientist_1_Male,
        BODY_Helicopter_Pilot,
        BODY_Siberian_Guard_2,
        BODY_Arctic_Commando,
        BODY_Moonraker_Elite_1_Male,
        BODY_Moonraker_Elite_2_Female,
        BODY_Left_Suit_Hand_Floating_Arm,
        HEAD_START,
        BODY_Male_Karl = HEAD_START,
        BODY_Male_Alan,
        BODY_Male_Pete,
        BODY_Male_Martin,
        BODY_Male_Mark,
        BODY_Male_Duncan,
        BODY_Male_Shaun,
        BODY_Male_Dwayne,
        BODY_Male_B,
        BODY_Male_Dave_Dr_Doak,
        BODY_Male_Grant,
        BODY_Male_Des,
        BODY_Male_Chris,
        BODY_Male_Lee,
        BODY_Male_Neil,
        BODY_Male_Jim,
        BODY_Male_Robin,
        BODY_Male_Steve_H,
        BODY_Male_Terrorist,
        BODY_Male_Biker,
        BODY_Male_Graeme,
        BODY_Male_Steve_Ellis,
        BODY_Male_Joel,
        BODY_Male_Scott,
        BODY_Male_Joe_Altered,
        BODY_Male_Ken,
        BODY_Male_Joe,
        BODY_Male_Mishkin,
        BODY_Female_Sally,
        BODY_Female_Marion_Rosika,
        BODY_Female_Mandy,
        BODY_Female_Vivien,
        BODY_Male_Pierce_Bond_1,
        BODY_Male_Pierce_Bond_2,
        BODY_Male_Pierce_Bond_3,
        BODY_Male_Pierce_Bond_Parka,
        BODY_Male_Pierce_Bond_Tuxedo,
        BODY_Natalya_Jungle_Fatigues,
        BODIES_MAX
    } BODIES;
    typedef enum GENDER
    {
        FEMALE,
        MALE
    } GENDER;

    typedef enum HEADS
    {
        HEAD_Male_Karl = HEAD_START,
        HEAD_Male_Alan,
        HEAD_Male_Pete,
        HEAD_Male_Martin,
        HEAD_Male_Mark,
        HEAD_Male_Duncan,
        HEAD_Male_Shaun,
        HEAD_Male_Dwayne,
        HEAD_Male_B,
        HEAD_Male_Dave_Dr_Doak,
        HEAD_Male_Grant,
        HEAD_Male_Des,
        HEAD_Male_Chris,
        HEAD_Male_Lee,
        HEAD_Male_Neil,
        HEAD_Male_Jim,
        HEAD_Male_Robin,
        HEAD_Male_Steve_H,
        HEAD_Male_Terrorist,
        HEAD_Male_Biker,
        HEAD_Male_Graeme,
        HEAD_Male_Steve_Ellis,
        HEAD_Male_Joel,
        HEAD_Male_Scott,
        HEAD_Male_Joe_Altered,
        HEAD_Male_Ken,
        HEAD_Male_Joe,
        HEAD_Male_Mishkin,
        HEAD_F_START,
        HEAD_Female_Sally = HEAD_F_START,
        HEAD_Female_Marion_Rosika,
        HEAD_Female_Mandy,
        HEAD_Female_Vivien,
        HEAD_BOND_START,
        HEAD_Male_Brosnan_Boiler = HEAD_BOND_START,
        HEAD_Male_Brosnan_Default,
        HEAD_Male_Brosnan_Jungle,
        HEAD_Male_Brosnan_Parka,
        HEAD_Male_Brosnan_Tuxedo,





        HEAD_Natalya_Jungle_Fatigues,
        HEAD_END,

        HEAD_COUNT = HEAD_END - HEAD_START,
        HEAD_MALE_COUNT = HEAD_F_START - HEAD_START,
        HEAD_FEMALE_COUNT = HEAD_BOND_START - HEAD_F_START
    } HEADS;


    typedef enum PROP
    {
        PROP_ALARM1,
        PROP_ALARM2,
        PROP_EXPLOSIONBIT,
        PROP_AMMO_CRATE1,
        PROP_AMMO_CRATE2,
        PROP_AMMO_CRATE3,
        PROP_AMMO_CRATE4,
        PROP_AMMO_CRATE5,
        PROP_BIN1,
        PROP_BLOTTER1,
        PROP_BOOK1,
        PROP_BOOKSHELF1,
        PROP_BRIDGE_CONSOLE1A,
        PROP_BRIDGE_CONSOLE1B,
        PROP_BRIDGE_CONSOLE2A,
        PROP_BRIDGE_CONSOLE2B,
        PROP_BRIDGE_CONSOLE3A,
        PROP_BRIDGE_CONSOLE3B,
        PROP_CARD_BOX1,
        PROP_CARD_BOX2,
        PROP_CARD_BOX3,
        PROP_CARD_BOX4_LG,
        PROP_CARD_BOX5_LG,
        PROP_CARD_BOX6_LG,
        PROP_CCTV,
        PROP_CONSOLE1,
        PROP_CONSOLE2,
        PROP_CONSOLE3,
        PROP_CONSOLE_SEVA,
        PROP_CONSOLE_SEVB,
        PROP_CONSOLE_SEVC,
        PROP_CONSOLE_SEVD,
        PROP_CONSOLE_SEV2A,
        PROP_CONSOLE_SEV2B,
        PROP_CONSOLE_SEV2C,
        PROP_CONSOLE_SEV2D,
        PROP_CONSOLE_SEV_GEA,
        PROP_CONSOLE_SEV_GEB,
        PROP_DESK1,
        PROP_DESK2,
        PROP_DESK_LAMP2,
        PROP_DISC_READER,
        PROP_DISK_DRIVE1,
        PROP_FILING_CABINET1,
        PROP_JERRY_CAN1,
        PROP_KEYBOARD1,
        PROP_KIT_UNITS1,
        PROP_LETTER_TRAY1,
        PROP_MAINFRAME1,
        PROP_MAINFRAME2,
        PROP_METAL_CHAIR1,
        PROP_METAL_CRATE1,
        PROP_METAL_CRATE2,
        PROP_METAL_CRATE3,
        PROP_METAL_CRATE4,
        PROP_MISSILE_RACK,
        PROP_MISSILE_RACK2,
        PROP_OIL_DRUM1,
        PROP_OIL_DRUM2,
        PROP_OIL_DRUM3,
        PROP_OIL_DRUM5,
        PROP_OIL_DRUM6,
        PROP_OIL_DRUM7,
        PROP_PADLOCK,
        PROP_PHONE1,
        PROP_RADIO_UNIT1,
        PROP_RADIO_UNIT2,
        PROP_RADIO_UNIT3,
        PROP_RADIO_UNIT4,
        PROP_SAT1_REFLECT,
        PROP_SATDISH,
        PROP_SATBOX,
        PROP_STOOL1,
        PROP_SWIVEL_CHAIR1,
        PROP_TORPEDO_RACK,
        PROP_TV1,
        PROP_TV_HOLDER,
        PROP_TVSCREEN,
        PROP_TV4SCREEN,
        PROP_WOOD_LG_CRATE1,
        PROP_WOOD_LG_CRATE2,
        PROP_WOOD_MD_CRATE3,
        PROP_WOOD_SM_CRATE4,
        PROP_WOOD_SM_CRATE5,
        PROP_WOOD_SM_CRATE6,
        PROP_WOODEN_TABLE1,
        PROP_SWIPE_CARD2,
        PROP_BORG_CRATE,
        PROP_BOXES4X4,
        PROP_BOXES3X4,
        PROP_BOXES2X4,
        PROP_SEC_PANEL,
        PROP_ICBM_NOSE,
        PROP_ICBM,
        PROP_TUNING_CONSOLE1,
        PROP_DESK_ARECIBO1,
        PROP_LOCKER3,
        PROP_LOCKER4,
        PROP_ROOFGUN,
        PROP_DEST_ENGINE,
        PROP_DEST_EXOCET,
        PROP_DEST_GUN,
        PROP_DEST_HARPOON,
        PROP_DEST_SEAWOLF,
        PROP_WINDOW,
        PROP_WINDOW_LIB_LG1,
        PROP_WINDOW_LIB_SM1,
        PROP_WINDOW_COR11,
        PROP_JUNGLE3_TREE,
        PROP_PALM,
        PROP_PALMTREE,
        PROP_PLANT2B,
        PROP_LABBENCH,
        PROP_GASBARREL,
        PROP_GASBARRELS,
        PROP_BODYARMOUR,
        PROP_BODYARMOURVEST,
        PROP_GASTANK,
        PROP_GLASSWARE1,
        PROP_HATCHBOLT,
        PROP_BRAKEUNIT,
        PROP_AK47MAG,
        PROP_M16MAG,
        PROP_MP5KMAG,
        PROP_SKORPIONMAG,
        PROP_SPECTREMAG,
        PROP_UZIMAG,
        PROP_SILENCER,
        PROP_CHREXTINGUISHER,
        PROP_BOXCARTRIDGES,
        PROP_FNP90MAG,
        PROP_GOLDENSHELLS,
        PROP_MAGNUMSHELLS,
        PROP_WPPKMAG,
        PROP_TT33MAG,
        PROP_SEV_DOOR,
        PROP_SEV_DOOR3,
        PROP_SEV_DOOR3_WIND,
        PROP_SEV_DOOR4_WIND,
        PROP_SEV_TRISLIDE,
        PROP_SEV_DOOR_V1,
        PROP_STEEL_DOOR1,
        PROP_STEEL_DOOR2,
        PROP_STEEL_DOOR3,
        PROP_SILO_LIFT_DOOR,
        PROP_STEEL_DOOR2B,
        PROP_DOOR_ROLLER1,
        PROP_DOOR_ROLLER2,
        PROP_DOOR_ROLLER3,
        PROP_DOOR_ROLLER4,
        PROP_DOOR_ST_AREC1,
        PROP_DOOR_ST_AREC2,
        PROP_DOOR_DEST1,
        PROP_DOOR_DEST2,
        PROP_GAS_PLANT_SW_DO1,
        PROP_GAS_PLANT_SW2_DO1,
        PROP_GAS_PLANT_SW3_DO1,
        PROP_GAS_PLANT_SW4_DO1,
        PROP_GAS_PLANT_MET1_DO1,
        PROP_GAS_PLANT_WC_CUB1,
        PROP_GASPLANT_CLEAR_DOOR,
        PROP_TRAIN_DOOR,
        PROP_TRAIN_DOOR2,
        PROP_TRAIN_DOOR3,
        PROP_DOOR_EYELID,
        PROP_DOOR_IRIS,
        PROP_SEVDOORWOOD,
        PROP_SEVDOORWIND,
        PROP_SEVDOORNOWIND,
        PROP_SEVDOORMETSLIDE,
        PROP_CRYPTDOOR1A,
        PROP_CRYPTDOOR1B,
        PROP_CRYPTDOOR2A,
        PROP_CRYPTDOOR2B,
        PROP_CRYPTDOOR3,
        PROP_CRYPTDOOR4,
        PROP_VERTDOOR,
        PROP_HATCHDOOR,
        PROP_DAMGATEDOOR,
        PROP_DAMTUNDOOR,
        PROP_DAMCHAINDOOR,
        PROP_SILOTOPDOOR,
        PROP_DOORPRISON1,
        PROP_DOORSTATGATE,
        PROP_CHRKALASH,
        PROP_CHRGRENADELAUNCH,
        PROP_CHRKNIFE,
        PROP_CHRLASER,
        PROP_CHRM16,
        PROP_CHRMP5K,
        PROP_CHRRUGER,
        PROP_CHRWPPK,
        PROP_CHRSHOTGUN,
        PROP_CHRSKORPION,
        PROP_CHRSPECTRE,
        PROP_CHRUZI,
        PROP_CHRGRENADE,
        PROP_CHRFNP90,
        PROP_CHRBRIEFCASE,
        PROP_CHRREMOTEMINE,
        PROP_CHRPROXIMITYMINE,
        PROP_CHRTIMEDMINE,
        PROP_CHRROCKET,
        PROP_CHRGRENADEROUND,
        PROP_CHRWPPKSIL,
        PROP_CHRTT33,
        PROP_CHRMP5KSIL,
        PROP_CHRAUTOSHOT,
        PROP_CHRGOLDEN,
        PROP_CHRTHROWKNIFE,
        PROP_CHRSNIPERRIFLE,
        PROP_CHRROCKETLAUNCH,
        PROP_HATFURRY,
        PROP_HATFURRYBROWN,
        PROP_HATFURRYBLACK,
        PROP_HATTBIRD,
        PROP_HATTBIRDBROWN,
        PROP_HATHELMET,
        PROP_HATHELMETGREY,
        PROP_HATMOON,
        PROP_HATBERET,
        PROP_HATBERETBLUE,
        PROP_HATBERETRED,
        PROP_HATPEAKED,
        PROP_CHRWRISTDART,
        PROP_CHREXPLOSIVEPEN,
        PROP_CHRBOMBCASE,
        PROP_CHRFLAREPISTOL,
        PROP_CHRPITONGUN,
        PROP_CHRFINGERGUN,
        PROP_CHRSILVERWPPK,
        PROP_CHRGOLDWPPK,
        PROP_CHRDYNAMITE,
        PROP_CHRBUNGEE,
        PROP_CHRDOORDECODER,
        PROP_CHRBOMBDEFUSER,
        PROP_CHRBUGDETECTOR,
        PROP_CHRSAFECRACKERCASE,
        PROP_CHRCAMERA,
        PROP_CHRLOCKEXPLODER,
        PROP_CHRDOOREXPLODER,
        PROP_CHRKEYANALYSERCASE,
        PROP_CHRWEAPONCASE,
        PROP_CHRKEYYALE,
        PROP_CHRKEYBOLT,
        PROP_CHRBUG,
        PROP_CHRMICROCAMERA,
        PROP_FLOPPY,
        PROP_CHRGOLDENEYEKEY,
        PROP_CHRPOLARIZEDGLASSES,
        PROP_CHRCREDITCARD,
        PROP_CHRDARKGLASSES,
        PROP_CHRGASKEYRING,
        PROP_CHRDATATHIEF,
        PROP_SAFE,
        PROP_BOMB,
        PROP_CHRPLANS,
        PROP_CHRSPYFILE,
        PROP_CHRBLUEPRINTS,
        PROP_CHRCIRCUITBOARD,
        PROP_CHRMAP,
        PROP_CHRSPOOLTAPE,
        PROP_CHRAUDIOTAPE,
        PROP_CHRMICROFILM,
        PROP_CHRMICROCODE,
        PROP_CHRLECTRE,
        PROP_CHRMONEY,
        PROP_CHRGOLDBAR,
        PROP_CHRHEROIN,
        PROP_CHRCLIPBOARD,
        PROP_CHRDOSSIERRED,
        PROP_CHRSTAFFLIST,
        PROP_CHRDATTAPE,
        PROP_CHRPLASTIQUE,
        PROP_CHRBLACKBOX,
        PROP_CHRVIDEOTAPE,
        PROP_NINTENDOLOGO,
        PROP_GOLDENEYELOGO,
        PROP_WALLETBOND,
        PROP_MILTRUCK,
        PROP_JEEP,
        PROP_ARTIC,
        PROP_HELICOPTER,
        PROP_TIGER,
        PROP_MILCOPTER,
        PROP_HIND,
        PROP_ARTICTRAILER,
        PROP_MOTORBIKE,
        PROP_TANK,
        PROP_APC,
        PROP_SPEEDBOAT,
        PROP_PLANE,
        PROP_GUN_RUNWAY1,
        PROP_SAFEDOOR,
        PROP_KEY_HOLDER,
        PROP_HATCHSEVX,
        PROP_SEVDISH,
        PROP_ARCHSECDOOR1,
        PROP_ARCHSECDOOR2,
        PROP_GROUNDGUN,
        PROP_TRAINEXTDOOR,
        PROP_CARBMW,
        PROP_CARESCORT,
        PROP_CARGOLF,
        PROP_CARWEIRD,
        PROP_CARZIL,
        PROP_SHUTTLE_DOOR_L,
        PROP_SHUTTLE_DOOR_R,
        PROP_DEPOT_GATE_ENTRY,
        PROP_DEPOT_DOOR_STEEL,
        PROP_GLASSWARE2,
        PROP_GLASSWARE3,
        PROP_GLASSWARE4,
        PROP_LANDMINE,
        PROP_PLANT1,
        PROP_PLANT11,
        PROP_PLANT2,
        PROP_PLANT3,
        PROP_JUNGLE5_TREE,
        PROP_LEGALPAGE,
        PROP_ST_PETE_ROOM_1I,
        PROP_ST_PETE_ROOM_2I,
        PROP_ST_PETE_ROOM_3T,
        PROP_ST_PETE_ROOM_5C,
        PROP_ST_PETE_ROOM_6C,
        PROP_DOOR_ROLLERTRAIN,
        PROP_DOOR_WIN,
        PROP_DOOR_AZTEC,
        PROP_SHUTTLE,
        PROP_DOOR_AZT_DESK,
        PROP_DOOR_AZT_DESK_TOP,
        PROP_DOOR_AZT_CHAIR,
        PROP_DOOR_MF,
        PROP_FLAG,
        PROP_BARRICADE,
        PROP_MODEMBOX,
        PROP_DOORPANEL,
        PROP_DOORCONSOLE,
        PROP_CHRTESTTUBE,
        PROP_BOLLARD,
        PROP_MAX
    } PROP;






typedef enum PROJECTILES
    {
        PROJECTILES_MAX = 0x2E,
        PROJECTILES_TYPE_KNIFE = PROP_CHRKNIFE,
        PROJECTILES_TYPE_GRENADE = PROP_CHRGRENADE,
        PROJECTILES_TYPE_REMOTE_MINE = PROP_CHRREMOTEMINE,
        PROJECTILES_TYPE_PROX_MINE = PROP_CHRPROXIMITYMINE,
        PROJECTILES_TYPE_TIMED_MINE = PROP_CHRTIMEDMINE,
        PROJECTILES_TYPE_ROCKET_ROUND = PROP_CHRROCKET,
        PROJECTILES_TYPE_ROCKET_ROUND2 = PROP_CHRROCKET,
        PROJECTILES_TYPE_GLAUNCH_ROUND = PROP_CHRGRENADEROUND,
        PROJECTILES_TYPE_BOMBCASE = PROP_CHRBOMBCASE,
        PROJECTILES_TYPE_BUG = PROP_CHRBUG,
        PROJECTILES_TYPE_MICROCAMERA = PROP_CHRMICROCAMERA,
        PROJECTILES_TYPE_GE_KEY = PROP_CHRGOLDENEYEKEY,
        PROJECTILES_TYPE_PLASTIQUE = PROP_CHRPLASTIQUE
    } PROJECTILES;


    typedef enum PROP_TYPE
    {
        PROP_TYPE_NUL,
        PROP_TYPE_OBJ,
        PROP_TYPE_DOOR,
        PROP_TYPE_CHR,
        PROP_TYPE_WEAPON,
        PROP_TYPE_PLAYER,
        PROP_TYPE_VIEWER,
        PROP_TYPE_EXPLOSION,
        PROP_TYPE_SMOKE,
        PROP_TYPE_MAX
    } PROP_TYPE;
# 3101 "src/bondconstants.h"
#pragma endregion

#pragma region PropDef stuff



    typedef enum ACT_STATUS
    {
        ACT_STATUS_NONE,
        ACT_STATUS_NORMAL,
        ACT_STATUS_COVERWAIT,
        ACT_STATUS_GRENADEWAIT,
        ACT_STATUS_WAITING,
        ACT_STATUS_COVERGOTO,
        ACT_STATUS_COVERBREAK,
        ACT_STATUS_COVERSEEN,
        ACT_STATUS_FLANKLEFT,
        ACT_STATUS_FLANKRIGHT,
        ACT_STATUS_DODGE,
        ACT_STATUS_GRENADE,
        ACT_STATUS_WAITSEEN,
        ACT_STATUS_WITHDRAW,
        ACT_STATUS_SHOOTING,
        ACT_STATUS_SYNCSHOOT,
        ACT_STATUS_WAITTIMEOUT,
        ACT_STATUS_COVERTIMEOUT,
        ACT_STATUS_TRACKING,
        ACT_STATUS_RETREAT,
        ACT_STATUS_SURRENDER,
        ACT_STATUS_TALKING,
        ACT_STATUS_LISTENING,
        ACT_STATUS_GOTOALARM,
        ACT_STATUS_BOTFRIENDFOLLOW,
        ACT_STATUS_BOTHIDE,
        ACT_STATUS_BOTPATH,
        ACT_STATUS_BOTINJURED,
        ACT_STATUS_BOTNORMAL,
        ACT_STATUS_BOTSHOOTING,
        ACT_STATUS_DRUGGED,
        ACT_STATUS_PANIC,
        ACT_STATUS_RUNFROMGRENADE,
        ACT_STATUS_UNARMEDATTACK,
        ACT_STATUS_MAX
    } ACT_STATUS;
# 3183 "src/bondconstants.h"
    typedef enum ACT_TYPE
    {
        ACT_INIT,
        ACT_STAND,
        ACT_KNEEL,
        ACT_ANIM,
        ACT_DIE,
        ACT_DEAD,
        ACT_ARGH,
        ACT_PREARGH,
        ACT_ATTACK,
        ACT_ATTACKWALK,
        ACT_ATTACKROLL,
        ACT_SIDESTEP,
        ACT_JUMPOUT,
        ACT_RUNPOS,
        ACT_PATROL,
        ACT_GOPOS,
        ACT_SURRENDER,
        ACT_LOOKATTARGET,
        ACT_SURPRISED,
        ACT_STARTALARM,
        ACT_THROWGRENADE,
        ACT_TURNDIR,
        ACT_TEST,
        ACT_BONDINTRO,
        ACT_BONDDIE,
        ACT_BONDMULTI,
# 3222 "src/bondconstants.h"
        ACT_NULL,
        ACT_TYPE_MAX
    } ACT_TYPE;
# 3258 "src/bondconstants.h"
    typedef enum AMMOTYPE
    {
        AMMO_NONE,
        AMMO_9MM,
        AMMO_9MM_2,
        AMMO_RIFLE,
        AMMO_SHOTGUN,
        AMMO_GRENADE,
        AMMO_ROCKETS,
        AMMO_REMOTEMINE,
        AMMO_PROXMINE,
        AMMO_TIMEDMINE,
        AMMO_KNIFE,
        AMMO_GRENADEROUND,
        AMMO_MAGNUM,
        AMMO_GGUN,
        AMMOTYPE_GLOBAL_MAX = AMMO_GGUN,

        AMMO_DARTS,
        AMMO_EXPLOSIVEPEN,
        AMMO_BOMBCASE,
        AMMO_FLARE,
        AMMO_PITON,
        AMMO_DYNAMITE,
        AMMO_BUG,
        AMMO_MICRO_CAMERA,
        AMMO_GEKEY,
        AMMO_PLASTIQUE,
        AMMO_WATCH_LASER,
        AMMO_WATCH_MAGNET,
        AMMO_UNK,
        AMMO_CAMERA,
        AMMO_TANK,
        AMMO_TOKEN,
        AMMOTYPE_MAX
    } AMMOTYPE;

    typedef enum HATTYPE
    {
        HATTYPE_OTHER = -1,
        HATTYPE_BERRET,
        HATTYPE_BIRD,
        HATTYPE_PEAKED,
        HATTYPE_HELMATE,
        HATTYPE_FURRY,
        HATTYPE_MOON
    } HATTYPE;
    typedef enum INV_ITEM_TYPE
    {
        INV_ITEM_NONE,
        INV_ITEM_WEAPON,
        INV_ITEM_PROP,
        INV_ITEM_DUAL,
        INV_ITEM_PICKUP
    } INV_ITEM_TYPE;

    typedef enum ITEM_IDS
    {
        ITEM_UNARMED,
        ITEM_FIST,
        ITEM_KNIFE,
        ITEM_THROWKNIFE,
        ITEM_WPPK,
        ITEM_WPPKSIL,
        ITEM_TT33,
        ITEM_SKORPION,
        ITEM_AK47,
        ITEM_UZI,
        ITEM_MP5K,
        ITEM_MP5KSIL,
        ITEM_SPECTRE,
        ITEM_M16,
        ITEM_FNP90,
        ITEM_SHOTGUN,
        ITEM_AUTOSHOT,
        ITEM_SNIPERRIFLE,
        ITEM_RUGER,
        ITEM_GOLDENGUN,
        ITEM_SILVERWPPK,
        ITEM_GOLDWPPK,
        ITEM_LASER,
        ITEM_WATCHLASER,
        ITEM_GRENADELAUNCH,
        ITEM_ROCKETLAUNCH,
        ITEM_GRENADE,
        ITEM_TIMEDMINE,
        ITEM_PROXIMITYMINE,
        ITEM_REMOTEMINE,
        ITEM_TRIGGER,
        ITEM_TASER,
        ITEM_TANKSHELLS,
        ITEM_BOMBCASE,
        ITEM_PLASTIQUE,
        ITEM_FLAREPISTOL,
        ITEM_PITONGUN,
        ITEM_BUNGEE,
        ITEM_DOORDECODER,
        ITEM_BOMBDEFUSER,
        ITEM_CAMERA,
        ITEM_LOCKEXPLODER,
        ITEM_DOOREXPLODER,
        ITEM_BRIEFCASE,
        ITEM_WEAPONCASE,
        ITEM_SAFECRACKERCASE,
        ITEM_KEYANALYSERCASE,
        ITEM_BUG,
        ITEM_MICROCAMERA,
        ITEM_BUGDETECTOR,
        ITEM_EXPLOSIVEFLOPPY,
        ITEM_POLARIZEDGLASSES,
        ITEM_DARKGLASSES,
        ITEM_CREDITCARD,
        ITEM_GASKEYRING,
        ITEM_DATATHIEF,
        ITEM_WATCHIDENTIFIER,
        ITEM_WATCHCOMMUNICATOR,
        ITEM_WATCHGEIGERCOUNTER,
        ITEM_WATCHMAGNETREPEL,
        ITEM_WATCHMAGNETATTRACT,
        ITEM_GOLDENEYEKEY,
        ITEM_BLACKBOX,
        ITEM_CIRCUITBOARD,
        ITEM_CLIPBOARD,
        ITEM_STAFFLIST,
        ITEM_DOSSIERRED,
        ITEM_PLANS,
        ITEM_SPYFILE,
        ITEM_BLUEPRINTS,
        ITEM_MAP,
        ITEM_AUDIOTAPE,
        ITEM_VIDEOTAPE,
        ITEM_DATTAPE,
        ITEM_SPOOLTAPE,
        ITEM_MICROFILM,
        ITEM_MICROCODE,
        ITEM_LECTRE,
        ITEM_MONEY,
        ITEM_GOLDBAR,
        ITEM_HEROIN,
        ITEM_KEYCARD,
        ITEM_KEYYALE,
        ITEM_KEYBOLT,
        ITEM_SUIT_LF_HAND,
        ITEM_JOYPAD,
        ITEM_NULL86,
        ITEM_NULL87,
        ITEM_TOKEN,
        ITEM_IDS_MAX
    } ITEM_IDS;

    typedef enum PROPDEF_TYPE
    {
        PROPDEF_NOTHING,
        PROPDEF_DOOR,
        PROPDEF_DOOR_SCALE,
        PROPDEF_PROP,
        PROPDEF_KEY,
        PROPDEF_ALARM,
        PROPDEF_CCTV,
        PROPDEF_MAGAZINE,
        PROPDEF_COLLECTABLE,
        PROPDEF_GUARD,
        PROPDEF_MONITOR,
        PROPDEF_MULTI_MONITOR,
        PROPDEF_RACK,
        PROPDEF_AUTOGUN,
        PROPDEF_LINK,
        PROPDEF_DEBRIS,
        PROPDEF_UNK16,
        PROPDEF_HAT,
        PROPDEF_GUARD_ATTRIBUTE,
        PROPDEF_SWITCH,
        PROPDEF_AMMO,
        PROPDEF_ARMOUR,
        PROPDEF_TAG,
        PROPDEF_OBJECTIVE_START,
        PROPDEF_OBJECTIVE_END,
        PROPDEF_OBJECTIVE_DESTROY_OBJECT,
        PROPDEF_OBJECTIVE_COMPLETE_CONDITION,
        PROPDEF_OBJECTIVE_FAIL_CONDITION,
        PROPDEF_OBJECTIVE_COLLECT_OBJECT,
        PROPDEF_OBJECTIVE_DEPOSIT_OBJECT,
        PROPDEF_OBJECTIVE_PHOTOGRAPH,
        PROPDEF_OBJECTIVE_NULL,
        PROPDEF_OBJECTIVE_ENTER_ROOM,
        PROPDEF_OBJECTIVE_DEPOSIT_OBJECT_IN_ROOM,
        PROPDEF_OBJECTIVE_COPY_ITEM,
        PROPDEF_WATCH_MENU_OBJECTIVE_TEXT,
        PROPDEF_GAS_RELEASING,
        PROPDEF_RENAME,
        PROPDEF_LOCK_DOOR,
        PROPDEF_VEHICHLE,
        PROPDEF_AIRCRAFT,
        PROPDEF_UNK41,
        PROPDEF_GLASS,
        PROPDEF_SAFE,
        PROPDEF_SAFE_ITEM,
        PROPDEF_TANK,
        PROPDEF_CAMERAPOS,
        PROPDEF_TINTED_GLASS,
        PROPDEF_END,
        PROPDEF_MAX
    } PROPDEF_TYPE;


#pragma endregion

#pragma region Setup






   typedef enum CAMERAMODE
    {
        CAMERAMODE_NONE,
        CAMERAMODE_INTRO,
        CAMERAMODE_FADESWIRL,
        CAMERAMODE_SWIRL,
        CAMERAMODE_FP,
        CAMERAMODE_DEATH_CAM_SP,
        CAMERAMODE_DEATH_CAM_MP,
        CAMERAMODE_POSEND,
        CAMERAMODE_FP_NOINPUT,
        CAMERAMODE_MP,
        CAMERAMODE_FADE_TO_TITLE,
        CAMERAMODE_COUNT
    } CAMERAMODE;

    typedef enum INTRO_TYPE
    {
        INTROTYPE_SPAWN,
        INTROTYPE_ITEM,
        INTROTYPE_AMMO,
        INTROTYPE_SWIRL,
        INTROTYPE_ANIM,
        INTROTYPE_CUFF,
        INTROTYPE_CAMERA,
        INTROTYPE_WATCH,
        INTROTYPE_CREDITS,
        INTROTYPE_END,
        INTROTYPE_MAX
    } INTRO_TYPE;
    typedef enum MISSION_STATE_IDS
    {
        MISSION_STATE_0,
        MISSION_STATE_1,
        MISSION_STATE_2,
        MISSION_STATE_3,
        MISSION_STATE_4,
        MISSION_STATE_5,
        MISSION_STATE_6
    } MISSION_STATE_ID;

    typedef enum OBJECTIVESTATUS
    {
        OBJECTIVESTATUS_INCOMPLETE,
        OBJECTIVESTATUS_COMPLETE,
        OBJECTIVESTATUS_FAILED
    } OBJECTIVESTATUS;

    typedef enum TANK_RUN_STATE
    {
        TANK_RUN_STATE_NOT_RUNNING = 0,
        TANK_RUN_STATE_STARTING,
        TANK_RUN_STATE_RUNNING
    } TANK_RUN_STATE;

#pragma endregion

#pragma region VI
    typedef enum COLORMODE
    {
        COLORMODE_32BIT,
        COLORMODE_16BIT
    } COLORMODE;

    typedef enum SCREEN_SIZE
    {
        SCREEN_SIZE_320x240,
        SCREEN_SIZE_440x330
    } SCREEN_SIZE;

    typedef enum SCREEN_SIZE_OPTION
    {
        SCREEN_SIZE_FULLSCREEN,
        SCREEN_SIZE_WIDESCREEN,
        SCREEN_SIZE_CINEMA
    } SCREEN_SIZE_OPTION;

    typedef enum SCREEN_RATIO_OPTION
    {
        SCREEN_RATIO_NORMAL,
        SCREEN_RATIO_16_9
    } SCREEN_RATIO_OPTION;


    typedef enum VIDEOMODE
    {
        MD_BLACK,
        MD_NORMAL,
        MD_MAXIMUM,


        VIDEOMODE_DISABLE_320x240 = MD_BLACK,
        VIDEOMODE_320x240 = MD_NORMAL,
        VIDEOMODE_640x480 = MD_MAXIMUM
    } VIDEOMODE;
#pragma endregion
# 3612 "src/bondconstants.h"
#pragma endregion



#pragma region Inline Macro Functions
# 3836 "src/bondconstants.h"
#pragma endregion
# 6 "src/bondgame.h" 2
# 1 "src/bondtypes.h" 1
# 27 "src/bondtypes.h"
# 1 "include/ultra64.h" 1
# 28 "src/bondtypes.h" 2

# 1 "src/snd.h" 1



# 1 "include/ultra64.h" 1
# 5 "src/snd.h" 2
# 17 "src/snd.h"
typedef enum ALSndpMsgType_e {
    AL_SNDP_PLAY_EVT = (1 << 0),
    AL_SNDP_STOP_EVT = (1 << 1),
    AL_SNDP_PAN_EVT = (1 << 2),
    AL_SNDP_VOL_EVT = (1 << 3),
    AL_SNDP_PITCH_EVT = (1 << 4),
    AL_SNDP_API_EVT = (1 << 5),
    AL_SNDP_DECAY_EVT = (1 << 6),
    AL_SNDP_END_EVT = (1 << 7),
    AL_SNDP_FX_EVT = (1 << 8),
    AL_SNDP_PLAY_SFX_EVT = (1 << 9),
    AL_SNDP_DEACTIVATE_EVT = (1 << 10),
    AL_SNDP_RELEASE_EVT = (1 << 11),
    AL_SNDP_UNKNOWN_12_EVT = (1 << 12),
    AL_SNDP_UNUSED_13_EVT = (1 << 13),
    AL_SNDP_UNUSED_14_EVT = (1 << 14),
    AL_SNDP_UNUSED_15_EVT = (1 << 15)
} ALSndpMsgType;


struct ALSoundState_s;
typedef struct ALSoundState_s {

    ALLink link;


    ALSound *sound;


    ALVoice voice;



    f32 pitch_28;



    f32 pitch_2c;



    struct ALSoundState_s *state;


    s16 vol;


    u8 priority;


    s8 unk37;


    s32 unk38;



    ALPan pan;



    u8 fxMix;
# 89 "src/snd.h"
    u8 unk3e;





    u8 playingState;

} ALSoundState;






typedef struct ALSeqpSfxConfig_s {







    s32 maybeSndStateCount;





    s32 maxEvents;






    u32 maybeMaxSounds;





    ALHeap *heap;
} ALSeqpSfxConfig;



struct ALInstrumentAlt_s
{
    s32 unk0;
    s32 unk4;
    s32 unk8;
    ALSound *soundArray[1];
};

struct ALBankAlt_s
{
    s16 instCount;
    u8 flags;
    u8 pad;
    s32 sampleRate;
    ALInstrument *percussion;
    struct ALInstrumentAlt_s *instArray[1];
};



void sndNewPlayerInit(ALSeqpSfxConfig *arg0);
u8 sndGetPlayingState(ALSoundState *state);
void sndDeactivate(ALSoundState *state);
void sndDeactivateAllSfxByFlag_1(void);
void sndCreatePostEvent(ALSoundState *state, s16 eventType, s32 arg2);
ALSoundState *sndPlaySfx(struct ALBankAlt_s *soundBank, s16 soundIndex, ALSoundState *pendingState);
u16 sndGetSfxSlotFirstNaturalVolume(void);
void sndApplyVolumeAllSfxSlot(u16 arg0);
void sndSetScalerApplyVolumeAllSfxSlot(f32 arg0);

extern s8 g_sndBootswitchSound;
# 30 "src/bondtypes.h" 2
# 49 "src/bondtypes.h"
typedef s32 bool;
typedef u32 romptr_t;
struct object_standard;
struct ChrRecord;
struct PropRecord;
struct ObjectRecord;
struct WeaponObjRecord;
struct WeaponObjRecordExtended;
union ModelRoData;





#pragma region AI

typedef struct AIRecord
{
    u8 cmd;
    u8 val[];
} AIRecord;
typedef struct AIRecord1
{
    u8 cmd;
    u8 val;
} AIRecord1;
typedef struct AIRecord1s
{
    u8 cmd;
    s8 val;
} AIRecord1s;

typedef struct AIListRecord
{
    AIRecord *ailist;
    int ID;
} AIListRecord;

#pragma endregion AI




#pragma region geometry






typedef union
{
    f32 m[4][4];
    s32 unused;
} Mtxf;

typedef union
{
    Mtxf pos;
    s32 view[4][4];
} RenderPosView;


#pragma region colour

    typedef struct rgba_u8
    {
        union
        {
            struct
            {
                u8 r;
                u8 g;
                u8 b;
                u8 a;
            };
            u8 rgba[4];
            u32 word;
        };
    } rgba_u8;

    typedef struct rgba_16
    {
        union
        {
            struct
            {
                u16 r : 5;
                u16 g : 5;
                u16 b : 5;
                u16 a : 1;
            };
            u16 AsInteger;
        };
    } rgba_16;

    typedef struct rgba_f32
    {
        union
        {
            struct
            {
                f32 r;
                f32 g;
                f32 b;
                f32 a;
            };
            f32 rgba[4];
        };
    } rgba_f32;
    typedef struct rgb_s32
    {
        union
        {
            struct
            {
                s32 r;
                s32 g;
                s32 b;
            };
            s32 rgb[3];
        };
    } rgb_s32;
    typedef struct rgba_s32
    {
        union
        {
            struct
            {
                s32 r;
                s32 g;
                s32 b;
                s32 a;
            };
            s32 rgba[4];
        };
    } rgba_s32;

#pragma endregion
#pragma region coordinates





    typedef struct coord2d
    {
        union
        {
            struct
            {
                f32 x;
                f32 y;
            };
            f32 f[2];
        };
    } coord2d;





    typedef coord2d vec2;





    typedef struct point2d
    {
        union
        {
            struct
            {
                s32 x;
                s32 y;
            };
            s32 p[2];
        };
    } point2d;

    typedef struct coord3d
    {
        union
        {
            struct
            {
                f32 x;
                f32 y;
                f32 z;
            };
            f32 f[3];
        };
    } coord3d;
# 254 "src/bondtypes.h"
    typedef coord3d vec3d;
# 269 "src/bondtypes.h"
    typedef struct coord16
    {
        union
        {
            struct
            {
                s16 x;
                s16 y;
                s16 z;
            };
            s16 AsArray[3];
        };
    } coord16;

#pragma endregion


    struct WatchVertex {
        struct coord16 coord1;
        struct coord16 coord2;
        struct rgba_u8 color;
    };
# 299 "src/bondtypes.h"
    struct WatchRectangle {
        struct WatchVertex vtx[4];
    };

#pragma region volumes and areas

    typedef struct bbox2d
    {
        union
        {
            struct
            {
                coord2d min;
                coord2d max;
            };
            f32 f[2][2];
        };
    } bbox2d;

    typedef struct bbox
    {
        float xmin;
        float xmax;
        float ymin;
        float ymax;
        float zmin;
        float zmax;
    }bbox;

    typedef struct view4s32
    {
        union
        {
            struct
            {
                s32 left;
                s32 top;
                s32 width;
                s32 height;
            };
            s32 v[4];
            f32 f[4];
        };
    }Vew4s32;

    typedef struct view4f
    {
        union
        {
            struct
            {
                f32 left;
                f32 top;
                f32 width;
                f32 height;
            };
            s32 v[4];
            f32 f[4];
        };
    }view4f;

    typedef struct rect4f
    {
        union
        {
            struct coord2d points[4];
            f32 f[8];
        };
    }rect4f;

    struct rectbbox
    {
        f32 left;
        f32 up;
        f32 right;
        f32 down;
    };
#pragma endregion
#pragma endregion

#pragma region Stand Tiles






    typedef struct StandTilePoint
    {
        struct coord16;
        u16 link;
    } StandTilePoint;



    typedef struct StandTileHeaderMid
    {
        u16 special : 4;
        u16 r : 4;
        u16 g : 4;
        u16 b : 4;
    } StandTileHeaderMid;

    typedef struct StandTileHeaderTail
    {
        s16 pointCount : 4;

        s16 headerC : 4;
        s16 headerD : 4;
        s16 headerE : 4;
    } StandTileHeaderTail;

    typedef struct StandTile
    {




        u32 id : 24;

        u8 room;

        union
        {
            StandTileHeaderMid headerMid;
            s16 half;
        } mid;




        union
        {
            StandTileHeaderTail hdrTail;
            s16 half;
        } tail;


        StandTilePoint points[];
    } StandTile;

    typedef struct StandFilePoint
    {
        u16 x;
        u16 y;
        u16 z;
        u16 link;
    } StandFilePoint;

    typedef struct StandFileTile
    {
        u32 id : 24;

        u8 room;
        union
        {
            StandTileHeaderMid headerMid;
            s16 half;
        } mid;




        union
        {
            StandTileHeaderTail hdrTail;
            s16 half;
        } tail;




    } StandFileTile;

    typedef struct StandFileHeader
    {
        void * unk1;
        StandTile *firstTile;
        u8 unk2[];
    } StandFileHeader;


    struct StandTileWalkCallbackRecord
    {
        s32 *roomBuf;
        s32 count;
        s32 bufMax;
        s32 lastRoom;
    };
    typedef void (*standTileWalkCallback_t)(struct StandTile *, struct StandTile *, struct StandTileWalkCallbackRecord *);


    struct StandTileLocusCallbackRecord
    {
        s32 unk00;
        s32 count;
        s32 bufMax;
        s32 nearEdgeCount;
    };

    typedef struct StandFileFooter
    {
        char strictstring[8];
        void *unk3;
        void *unk4;
        void *unk5;
        void *unk6;
    } StandFileFooter;

    typedef s32 (*standTileLocusCallback_A_t)(struct StandTile *, struct StandTileLocusCallbackRecord *);
    typedef s32 (*standTileLocusCallback_B_t)(StandTile *arg0, s32 arg1, f32 arg2, f32 arg3, s32 arg4, struct StandTileLocusCallbackRecord *arg5);
    typedef s32 (*standTileLocusCallback_C_t)(struct StandTile **, s32, struct StandTileLocusCallbackRecord *);

    typedef s32 (*tilePredicate_t)(struct StandTile *);



    typedef struct BetaStandFilePoint
    {
        struct coord3d;
        u32 link;
    } BetaStandFilePoint;

    typedef struct BetaStandTileHeaderTail
    {
        u8 pointCount;
        u8 headerC;
        u8 headerD;
        u8 headerE;
    } BetaStandTileHeaderTail;

    typedef struct BetaStandTile
    {
        const char *id;
        StandTileHeaderMid headerMid;
        u16 betaUnknown;
        BetaStandTileHeaderTail hdrTail;
        struct BetaStandFilePoint points[];
    } BetaStandTile;

#pragma endregion


#pragma region AnimationStuff

    typedef struct AnimTable2
    {
        int a;
        char b;
    } AnimTable2;

    typedef struct ModelAnimation
    {
        s32 address;
        u16 unk04;
        u16 unk06;
        u16 unk08;
        u16 unk0A;
        u16 unk0C;
        u16 unk0E;
        int unk10;
        int unk14;
        int unk18;
        int unk1c;
        int unk20;
        int unk24;
        int unk28;
        int unk2c;
        int unk30;
        int unk34;
        int unk38;
        int unk3c;


    } ModelAnimation;


    struct object_animation_controller
    {
        void *ptranimation;
        u16 offsettocurcmd;
        u16 waitcounter;
        u32 imagenum;
        f32 rotation;
        f32 curzoomx;
        f32 startzoomx;
        f32 zoomxtimer;
        f32 initialzoomx;
        f32 finalzoomx;
        f32 curzoomy;
        f32 startzoomy;
        f32 zoomytimer;
        f32 initialzoomy;
        f32 finalzoomy;
        f32 curhorizontalpos;
        f32 starthorscroll;
        f32 horscrolltimer;
        f32 horinitpos;
        f32 horfinalpos;
        f32 curverpos;
        f32 startverscroll;
        f32 verscrolltimer;
        f32 verinitpos;
        f32 verfinalpos;
        u8 curredcomponant;
        u8 initredcomponant;
        u8 finalredcomponant;
        u8 curgreencomponant;
        u8 initgreencomponant;
        u8 finalgreencomponant;
        u8 curbluecomponant;
        u8 initbluecomponant;
        u8 finalbluecomponant;
        u8 curalphacomponant;
        u8 initalphacomponant;
        u8 finalalphacomponant;
        f32 startcolorshift;
        f32 colorshifttimer;
    };
# 684 "src/bondtypes.h"
#pragma endregion AnimationStuff

#pragma region ModelTypes




    typedef struct ModelRenderData
    {
        Mtxf *unk_matrix;
        bool zbufferenabled;
        u32 flags;
        Gfx *gdl;

        Mtxf *mtxlist;
        u32 unk14;
        u32 unk18;
        u32 unk1c;

        u32 unk20;
        u32 unk24;
        u32 unk28;
        u32 unk2c;

        s32 PropType;
        rgba_u8 envcolour;
        rgba_u8 fogcolour;
        u32 cullmode;
    } ModelRenderData;





    typedef struct Vertex
    {
        coord16 coord;
        s16 index;
        union
        {
            struct
            {
                s16 s;
                s16 t;
            };
            struct Vertex *LinkedTo;
        };
        union
        {
            u8 r;
            u8 nx;
        };
        union
        {
            u8 g;
            u8 ny;
        };
        union
        {
            u8 b;
            u8 nz;
        };
        u8 a;
    } Vertex;

#pragma region OpenFlight Records
# 797 "src/bondtypes.h"
    typedef struct ModelFileTextures
    {
        u32 TextureID;
        u8 Width;
        u8 Height;
        u8 MipMapTiles;
        u8 Type;
        u8 RenderDepth;
        u8 sflags;
        u8 tflags;

    } ModelFileTextures;




    typedef struct ModelNode
    {

        u16 Opcode;
        union ModelRoData *Data;
        struct ModelNode *Parent;
        struct ModelNode *Next;
        struct ModelNode *Prev;
        struct ModelNode *Child;
    } ModelNode;


#pragma region Model Node OpCode Definitions






        typedef struct ModelRoData_Child
        {
            u8 NumEntries;
            u8 unk01;
            u16 unk02;
            u8 *unk04;
        } ModelRoData_Child;

        typedef struct ModelRoData_Child_Vtx
        {
            u8 Type;
            u8 unk01;
            u16 VtxIndex;
        } ModelRoData_Child_Vtx;

        typedef struct ModelRoData_Child_Tri
        {
            u8 Type;
            u8 VtxIndex1;
            u8 VtxIndex2;
            u8 VtxIndex3;
        } ModelRoData_Child_Tri;

        typedef struct ModelRoData_Child_Image
        {
            u8 Type;
            u8 ImageIndex;
        } ModelRoData_Child_Image;

#pragma endregion

#pragma region Model Node OpCode Definitions






        typedef struct ModelRoData_HeaderRecord
        {
            u32 ModelType;
            struct ModelRoData_GroupRecord *FirstGroup;

            union
            {
                struct
                {
                    u16 Group1;
                    u16 Group2;
                };
                f32 GroupsAsF32;
            };

            u16 RwDataIndex;
            u16 reserved;
        } ModelRoData_HeaderRecord;

        typedef struct ModelRwData_HeaderRecord
        {
            s8 unk00;
            s8 unk01;
            s8 unk02;
            f32 ground;
            coord3d pos;
            f32 unk14;
            f32 unk18;
            f32 unk1c;
            f32 unk20;
            coord3d unk24;
            f32 unk30;
            coord3d unk34;
            coord3d unk40;
            coord3d unk4c;
            f32 unk58;
            f32 unk5c;
        } ModelRwData_HeaderRecord;
# 924 "src/bondtypes.h"
        typedef struct ModelRoData_GroupRecord
        {
            coord3d Origin;
            u16 JointID;
            union {
                s16 MatrixIDs[3];
                struct {
                    s16 MatrixID0;
                    s16 MatrixID1;
                    s16 MatrixID2;
                };
            };
            struct ModelRoData_GroupRecord *ChildGroup;
            f32 BoundingVolumeRadius;
        } ModelRoData_GroupRecord;
# 949 "src/bondtypes.h"
        typedef struct ModelRoData_DisplayListRecord
        {
            Gfx *Primary;
            Gfx *Secondary;
            void *BaseAddr;
            Vertex *Vertices;
            u16 numVertices;
# 964 "src/bondtypes.h"
            s8 ModelType;
        } ModelRoData_DisplayListRecord;

        typedef struct ModelRwData_DisplayListRecord
        {
            s32 unk00;
        } ModelRwData_DisplayListRecord;





        typedef struct ModelRoData_Op05Record
        {

            s32 NumChildren;
            struct ModelRoData_Child *Children;
            struct Vertex *Vertices;
            struct sImageTableEntry *Images;
            u8 Data[400];

            u32 unk1A0;
            void* BaseAddr;
        } ModelRoData_Op05Record;





        typedef struct ModelRoData_Op06Record
        {
            u32 unk00;
            u32 unk04;
            u32 unk08;
            u32 unk0C;
            u32 unk10;
            void* BaseAddr;
        } ModelRoData_Op06Record;





        typedef struct ModelRoData_Op07Record
        {
            struct ModelNode* unk00;
            struct ModelNode* unk04;


            s32 NumChildren;
            struct ModelRoData_Child *Children;
            struct Vertex *Vertices;
            struct sImageTableEntry *Images;
            u8 Data[400];

            u16 unk1A8;
            u16 RwDataIndex;
            void* BaseAddr;
        } ModelRoData_Op07Record;

        typedef struct ModelRwData_Op07Record
        {
            s32 index;
        } ModelRwData_Op07Record;







        typedef struct ModelRoData_LODRecord
        {
            f32 MinDistance;
            f32 MaxDistance;
            ModelNode *Affects;
            u16 RwDataIndex;
            u16 reserved;
        } ModelRoData_LODRecord;

        typedef struct ModelRwData_LODRecord
        {
            bool visible;
        } ModelRwData_LODRecord;






        typedef struct ModelRoData_BSPRecord
        {
            coord3d Point;
            coord3d Vector;
            ModelNode *leftChild;
            ModelNode *rightChild;
            s16 reserved;
            u16 RwDataIndex;
        } ModelRoData_BSPRecord;

        typedef struct ModelRwData_BSPRecord
        {
            bool visible;
        } ModelRwData_BSPRecord;





        typedef struct ModelRoData_BoundingBoxRecord
        {
            u32 ModelNumber;
            struct bbox Bounds;
        } ModelRoData_BoundingBoxRecord;





        typedef struct ModelRoData_Op11Record
        {
            u32 unk0c[16];
            f32 BoundingVolumeRadius;
            u16 RwDataIndex;
            u16 unk46;
            void* BaseAddr;
        } ModelRoData_Op11Record;

        typedef struct ModelRwData_Op11Record
        {
            s16 unk00;
            s16 unk02;
        } ModelRwData_Op11Record;




        typedef struct ModelRoData_GunfireRecord
        {
            coord3d Offset;
            coord3d Size;
            void * Image;
            f32 Scale;
            u16 RwDataIndex;
            u16 reserved;
            u32 BaseAddr;
        } ModelRoData_GunfireRecord;

        typedef struct ModelRwData_GunfireRecord
        {
            s16 visible;
            u16 unk02;
        } ModelRwData_GunfireRecord;





        typedef struct ModelRoData_ShadowRecord
        {
            coord2d pos;
            coord2d size;
            void *image;
            ModelRoData_HeaderRecord *Header;
            f32 Scale;
            void *BaseAddr;
        } ModelRoData_ShadowRecord;





        typedef struct ModelRoData_Op14Record
        {
            coord3d pos;
            f32 Scale;

        } ModelRoData_Op14Record;
# 1154 "src/bondtypes.h"
        typedef struct ModelRoData_InterlinkageRecord
        {
            coord3d pos;
            u32 unknown1;
            u32 unknown2;
            u32 unknown3;
            f32 Scale;
        } ModelRoData_InterlinkageRecord;





        typedef struct ModelNode_Op16Record
        {
            coord3d pos;
            u32 unknown1;
            u32 unknown2;
            f32 Scale;
        } ModelNode_Op16Record;
# 1186 "src/bondtypes.h"
        typedef struct ModelRoData_SwitchRecord
        {
            ModelNode *Controls;
            u16 RwDataIndex;
            u16 reserved;
        } ModelRoData_SwitchRecord;

        typedef struct ModelRwData_SwitchRecord
        {
            bool visible;
        } ModelRwData_SwitchRecord;
# 1212 "src/bondtypes.h"
        typedef struct ModelRoData_GroupSimpleRecord
        {
            coord3d Origin;
            s16 Group1;
            u16 Group2;
            f32 BoundingVolumeRadius;
        } ModelRoData_GroupSimpleRecord;





        typedef struct ModelRoData_DisplayListPrimaryRecord
        {
            s32 numVertices;
            Vertex *Vertices;
            Gfx *Primary;
            void *BaseAddr;
        } ModelRoData_DisplayListPrimaryRecord;





        typedef struct ModelRoData_HeadPlaceholderRecord
        {
            u16 RwDataIndex;
        } ModelRoData_HeadPlaceholderRecord;

        typedef struct ModelRwData_HeadPlaceholderRecord
        {
            struct ModelFileHeader *ModelFileHeader;
            void *RwDatas;
        } ModelRwData_HeadPlaceholderRecord;





        typedef struct ModelRoData_DisplayList_CollisionRecord
        {
            Gfx *Primary;
            Gfx *Secondary;
            Vertex *Vertices;
            s16 numVertices;
            s16 numCollisionVertices;
            Vertex *CollisionVertices;
            s16 *PointUsage;
            s16 ModelType;
            u16 RwDataIndex;
            void *BaseAddr;

        } ModelRoData_DisplayList_CollisionRecord;

        typedef struct ModelRwData_DisplayList_CollisionRecord
        {
            Vertex *Vertices;
            Gfx *gdl;
        } ModelRwData_DisplayList_CollisionRecord;


        struct ModelRwData_Raw
        {
            s32 unk00;
        };

#pragma endregion Model Node OpCode Definitions




    union ModelRoData
    {
        struct ModelRoData_HeaderRecord Header;
        struct ModelRoData_GroupRecord Group;
        struct ModelRoData_DisplayListRecord DisplayList;
        struct ModelRoData_Op05Record Op05;
        struct ModelRoData_Op06Record Op06;
        struct ModelRoData_Op07Record Op07;
        struct ModelRoData_LODRecord LOD;
        struct ModelRoData_BSPRecord BSP;
        struct ModelRoData_BoundingBoxRecord BoundingBox;
        struct ModelRoData_Op11Record Op11;
        struct ModelRoData_GunfireRecord Gunfire;
        struct ModelRoData_ShadowRecord Shadow;
        struct ModelRoData_Op14Record Op14;
        struct ModelRoData_InterlinkageRecord Interlinkage;
        struct ModelNode_Op16Record Op16;
        struct ModelRoData_SwitchRecord Switch;
        struct ModelRoData_GroupSimpleRecord GroupSimple;
        struct ModelRoData_DisplayListPrimaryRecord DisplayListPrimary;
        struct ModelRoData_HeadPlaceholderRecord HeadPlaceholder;
        struct ModelRoData_DisplayList_CollisionRecord DisplayListCollisions;
    };




    union ModelRwData
    {
        struct ModelRwData_HeaderRecord Header;
        struct ModelRwData_DisplayListRecord DisplayList;
        struct ModelRwData_Op07Record Op07;
        struct ModelRwData_LODRecord LOD;
        struct ModelRwData_BSPRecord BSP;
        struct ModelRwData_Op11Record Op11;
        struct ModelRwData_GunfireRecord Gunfire;
        struct ModelRwData_SwitchRecord Switch;
        struct ModelRwData_HeadPlaceholderRecord HeadPlaceholder;
        struct ModelRwData_DisplayList_CollisionRecord DisplayListCollisions;
        struct ModelRwData_Raw Raw;
    };


#pragma endregion OpenFlight Records

#pragma region Data Structures
# 1337 "src/bondtypes.h"
        typedef struct ModelJoint
        {
            u16 NodeType;
            u16 mtxA;
            u16 mtxB;
        } ModelJoint;





        typedef struct ModelSkeleton
        {
            short numjoints;
            short pad1;
            ModelJoint *Joints;
            short SkeletonSize;
            short pad2;



        } ModelSkeleton;

        typedef struct ModelFileHeader
        {
            ModelNode *RootNode;
            ModelSkeleton *Skeleton;

            ModelNode **Switches;
            s16 numSwitches;
            s16 numMatrices;
            f32 BoundingVolumeRadius;
            s16 numRecords;
            s16 numtextures;
            ModelFileTextures *Textures;





            s32 isLoaded;

        } ModelFileHeader;

        typedef struct ItemModelFileRecord
        {
            ModelFileHeader *header;
            char *filename;
            float scale;



        } ItemModelFileRecord;

        typedef struct ChrModelFileRecord
        {
            ModelFileHeader *header;
            char *filename;
            float scale;
            float pov;
            u8 isMale;
            u8 hasHead;
            u8 pad1;
            u8 pad2;



        } ChrModelFileRecord;
# 1430 "src/bondtypes.h"
        struct modeldata_root
        {
            u16 unk00;
            u8 unk02;
            f32 ground;
            coord3d pos;
            f32 subroty;
            f32 unk18;
            u32 unk1c;
            f32 unk20;
            coord3d unk24;
            f32 unk30;
            coord3d unk34;
            coord3d unk40;
            coord3d unk4c;
            f32 unk58;
            f32 unk5c;
        };





        typedef struct Model
        {
            s16 unk00;
            s16 Type;


            struct ChrRecord *chr;
            ModelFileHeader *obj;




            RenderPosView *render_pos;
            union ModelRwData **datas;

            f32 scale;
            struct Model *attachedto;
            ModelNode *attachedto_objinst;


            ModelAnimation *anim;

            s8 gunhand;
            s8 unk25;
            s8 animlooping;
            s8 unk27;

            f32 unk28;
            f32 unk2c;





            s16 framea;





            s16 frameb;
            s32 unk34;
            s32 unk38;
            f32 endframe;

            f32 speed;
            f32 newspeed;
            f32 oldspeed;
            f32 timespeed;
            f32 elapsespeed;

            ModelAnimation *anim2;

            f32 unk58;
            f32 unk5c;





            s16 frame2a;





            s16 frame2b;

            s32 unk64;
            s32 unk68;
            f32 unk6c;

            f32 speed2;
            s32 unk74;
            s32 unk78;
            f32 unk7c;

            s32 unk80;




            f32 unk84;

            f32 unk88;
            s32 unk8c;

            f32 animloopframe;
            f32 animloopmerge;
            s32 animflipfunc;
            s32 unk9c;

            s32 unka0;
            f32 playspeed;
            f32 animrate;
            f32 unkac;

            f32 unkb0;
            f32 unkb4;
            f32 unkb8;
            s32 unkbc;
        } Model;
# 1566 "src/bondtypes.h"
#pragma endregion Data Structures

#pragma endregion ModelTypes


#pragma region Explosion Stuff

        typedef struct ExplosionDetailsRecord
        {
            s16 TypeID;
            s16 Seed[6];
        } ExplosionDetailsRecord;

        struct ExplosionDetailsRecordSeed
        {
            s16 seed[6];
        };

        typedef struct ExplosionDetailsRecordEuList
        {
            u8 typeids[344];
            struct ExplosionDetailsRecordSeed seeds[105];
        } ExplosionDetailsRecordEuList;
#pragma endregion Explosion Stuff

#pragma region PadAndPaths




    typedef struct PadRecord
    {
        coord3d pos;
        coord3d up;
        coord3d look;
        char *plink;
        StandTile *stan;
    } PadRecord;





    typedef struct BoundPadRecord
    {

        coord3d pos;
        coord3d up;
        coord3d look;
        char * plink;
        StandTile *stan;

        bbox bbox;
    } BoundPadRecord;
# 1643 "src/bondtypes.h"
    typedef struct waypoint
    {
        s32 padID;
        s32 *neighbours;
        s32 groupNum;
        s32 dist;
    } waypoint;






    typedef struct waygroup
    {
        s32 *neighbours;
        s32 *waypoints;
        s32 dist;
    } waygroup;





    typedef struct PathRecord
    {
        s32 *waypoints;
        u8 ID;
        u8 isLoop;
        u16 len;
    } PathRecord;



    struct waydata
    {




        s8 mode;


        s8 unk01;






        s8 unk02;






        s8 unk03;

        coord3d pos;


        coord3d pos2;

        coord3d pos3;


        s32 age;


        coord3d pos_copy;




        f32 segdistdone;


        f32 segdisttotal;
    };


#pragma endregion PadAndPaths

#pragma region ACT_TYPES

    struct act_init
    {
        int padding[30];
    };

    struct act_stand
    {
        s32 prestand;
        s32 face_entitytype;
        s32 face_entityid;
        s32 reaim;
        s32 turning;
        u32 checkfacingwall;
        s32 wallcount;
        f32 mergetime;
        s8 face_target;
    };

    struct act_kneel
    {
        int padding[30];
    };

    struct act_anim
    {
        u32 unk02c;
        u32 holdLastFrame;
        u32 playSfx;
        u32 idleOnEnd;
        u32 noTranslate;
        u8 unk040;
        u8 unk041;
        u16 unk042;
        u16 unk044;
        u16 unk046;
        s16 animnum;
        u8 flip;
        f32 startframe;
        f32 endframe;
        f32 unk054;
        f32 unk058;
    };

    struct act_die
    {
        s32 notifychrindex;
        f32 thudframe1;
        f32 thudframe2;
        f32 timeextra;
        f32 elapseextra;
        coord3d extraspeed;
        s16 drcarollimagedelay;
    };

    struct act_dead
    {
        bool allowfade;
        bool allowreap;
        s32 reaptimer;
        s32 fadetimer;
        s32 notifychrindex;
    };

    struct act_argh
    {
        s32 notifychrindex;
        s32 unk30;
    };

    struct act_preargh
    {
        coord3d pos;
        f32 unk038;
        s32 unk03c;
        s32 unk040;
        u32 unk044;
    };

    struct act_attack
    {
        struct weapon_firing_animation_table *animfloats;

        s8 unk30;
        s8 unk31;
        s8 unk32;
        s8 unk33;

        s8 unk34;
        s8 unk35;
        s8 unk36;
        s8 unk37;

        s8 unk38[2];
        s8 unk3a[2];

        s8 unk3c[2];
        s8 unk3e;
        s8 unk3f;

        u32 unk40;





        s32 unk44;




        s32 attack_time;





        u32 attacktype;
        u32 entityid;
        u32 unk54;

        s32 type_of_motion;
        u32 unk5C;

        u32 unk60;
        u32 unk64;
        u32 unk68;
        u32 unk6c;

        u32 unk70;
        u32 unk74;
        u32 unk78;
        u32 unk7c;

        s8 attack_item;
        u8 unk81;
        u8 unk82;
        u8 unk83;
    };

    struct act_attackwalk
    {
        u32 unk02c;
        s32 clock_timer30;
        s32 clock_timer34;
        u32 unk038;
        struct weapon_firing_animation_table *animfloats;
        s32 timer40;

        s32 unk044;

        s8 unk48[2];
        s8 unk4a[2];

        s8 unk4C[2];
        u8 flip;
        s8 unk4f;

        s32 unk50;
        f32 speed;

        u32 unk58;
        u32 unk5C;

        u32 unk60;
        u32 unk64;
        u32 unk68;
        u32 unk6c;

        u32 unk70;
        u32 unk74;
        u32 unk78;
        u32 unk7c;

        s8 attack_item;
        u8 unk81;
        u8 unk82;
        u8 unk83;
    };

    struct act_attackroll
    {
        struct weapon_firing_animation_table *animfloats;

        s8 unk30;
        s8 unk31;
        s8 unk32;
        s8 unk33;

        s8 unk34;
        s8 unk35;
        s8 unk36;
        s8 unk37;

        s8 unk38[2];
        s8 unk3a[2];

        s8 unk3c[2];
        s8 unk3e;
        s8 unk3f;

        u32 unk40;





        s32 unk44;




        s32 attack_time;

        s32 unk4c[2];
        s32 unk54[2];

        u32 unk5C;

        u32 unk60;
        u32 unk64;
        u32 unk68;
        u32 unk6c;

        u32 unk70;
        u32 unk74;
        u32 unk78;
        u32 unk7c;

        s8 attack_item;
        u8 unk81;
        u8 unk82;
        u8 unk83;
    };

    struct act_sidestep
    {
        bool side;
    };

    struct act_jumpout
    {
        bool side;
    };

    struct act_runpos
    {
        coord3d pos;




        f32 neardist;




        s32 eta60;




        f32 turnspeed;
    };

    struct patrol_path
    {
        s32 *data;
        u8 unk04;
        u8 flags;
        u16 len;
    };

    struct act_patrol
    {
        struct patrol_path *path;
        s32 nextstep;
        bool forward;
        struct waydata waydata;
        s32 lastvisible60;




        f32 speed;

        s32 unk80;
        s32 unk84;
        s32 unk88;
        s32 unk8c;
        s32 unk90;
        s32 unk94;
        s32 unk98;
        s32 unk9c;
        s32 unka0;
    };

    struct act_gopos
    {
        coord3d targetpos;
        StandTile *target;

        waypoint *target_path;



        waypoint *waypoints[6];







        u8 curindex;
        u8 unk59;


        u16 unk5a;


        struct waydata waydata;


        s32 unk9c;




        f32 speed;
# 2100 "src/bondtypes.h"
    };
    struct act_surrender
    {
        int padding[30];
    };

    struct act_lookattarget
    {
        int padding[30];
    };

    struct act_surprised
    {
        u32 type;
    };

    struct act_startalarm
    {
        int padding[30];
    };

    struct act_throwgrenade
    {
        u32 entitytype;
        u32 entityid;
        u32 hand;
        bool needsequip;
    };

    struct act_turndir
    {
        int padding[30];
    };

    struct act_test
    {
        int padding[30];
    };

    struct act_bondintro
    {
        int padding[30];
    };

    struct act_bonddie
    {
        int padding[30];
    };

    struct act_bondmulti
    {
        f32 *unk2c;

        s32 unk30;
        s32 unk34;
        s32 unk38;
        s32 unk3c;

        int padding[25];
    };

    struct act_null
    {
        int padding[30];
    };

    struct act_bytes
    {
        s8 padding[120];
    };

    struct act_ubytes
    {
        u8 padding[120];
    };
#pragma endregion ACT_TYPES

#pragma region Prop, Chr and Definition Records






    typedef struct PropRecord
    {
        u8 type;
        u8 flags;
        s16 timetoregen;
        union
        {
            struct ChrRecord *chr;
            struct ObjectRecord *obj;
            struct DoorRecord *door;
            struct WeaponObjRecord *weapon;
            struct Explosion *explosion;
            struct Smoke *smoke;
            struct Scorch *scorch;
            void *voidp;
        };
        coord3d pos;
        StandTile *stan;




        f32 zDepth;
        struct PropRecord *parent;
        struct PropRecord *child;
        struct PropRecord *prev;
        struct PropRecord *next;
        u8 rooms[4];
        s32 unk30;
# 2229 "src/bondtypes.h"
    } PropRecord;

#pragma region GlobalPropDef




    typedef struct ChrRecord_f180
    {

        s8 unk00;
        s8 item_id;
        char unk02;
        char unk03;

        coord3d pos;




        struct coord3d delta;
        f32 unk1c;

        f32 unk20;
        f32 unk24;
        f32 unk28;
    } ChrRecord_f180;


    typedef struct PropDefHeaderRecord
    {
        u16 extrascale;
# 2272 "src/bondtypes.h"
        u8 state;
        u8 type;
    } PropDefHeaderRecord;





#pragma endregion GlobalPropDef


    typedef struct ChrRecord
    {
        s16 chrnum;
        s8 accuracyrating;
        s8 speedrating;
        u8 firecount[2];
        s8 headnum;
        ACT_TYPE actiontype : 8;
        s8 sleep;
        s8 invalidmove;
        s8 numclosearghs;
        s8 numarghs;
        u8 fadealpha;
        s8 arghrating;
        s8 aimendcount;
        s8 bodynum;
        u8 grenadeprob;
        s8 flinchcnt;
        u16 hidden;
        CHRFLAG chrflags;
        PropRecord *prop;
        Model *model;

        void *field_20;
        f32 chrwidth;
        f32 chrheight;
        union
        {
            struct act_init act_init;
            struct act_stand act_stand;
            struct act_kneel act_kneel;
            struct act_anim act_anim;
            struct act_die act_die;
            struct act_dead act_dead;
            struct act_argh act_argh;
            struct act_preargh act_preargh;
            struct act_attack act_attack;
            struct act_attackwalk act_attackwalk;
            struct act_attackroll act_attackroll;
            struct act_sidestep act_sidestep;
            struct act_jumpout act_jumpout;
            struct act_runpos act_runpos;
            struct act_patrol act_patrol;
            struct act_gopos act_gopos;
            struct act_surrender act_surrender;
            struct act_lookattarget act_lookattarget;
            struct act_surprised act_surprised;
            struct act_startalarm act_startalarm;
            struct act_throwgrenade act_throwgrenade;
            struct act_turndir act_turndir;
            struct act_test act_test;
            struct act_bondintro act_bondintro;
            struct act_bonddie act_bonddie;
            struct act_bondmulti act_bondmulti;
            struct act_null act_null;
            struct act_bytes act_bytes;
            struct act_ubytes act_ubytes;
        };
        f32 sumground;
        f32 manground;
        f32 ground;
        coord3d fallspeed;
        coord3d prevpos;
        s32 lastwalk60;
        s32 lastmoveok60;
        f32 visionrange;
        s32 lastseetarget60;
        coord3d lastknowntargetpos;
        void *targetTile;


        union
        {
            s32 seen_bond_time;
            struct
            {
                s16 lastshooter;
                s16 timeshooter;
            };
        };
        f32 hearingscale;
        s32 lastheartarget60;

        rgba_u8 shadecol;
        rgba_u8 nextcol;
        f32 damage;
        f32 maxdamage;
        AIRecord *ailist;
        u16 aioffset;
        s16 aireturnlist;
        u8 morale;
        u8 alertness;
        u8 flags2;
        u8 random;
        s32 timer60;
        s16 padpreset1;
        s16 chrpreset1;
        s16 chrseeshot;
        s16 chrseedie;
# 2391 "src/bondtypes.h"
        rect4f collision_bounds;


        f32 shotbondsum;

        f32 aimuplshoulder;
        f32 aimuprshoulder;
        f32 aimupback;
        f32 aimsideback;

        f32 aimendlshoulder;
        f32 aimendrshoulder;
        f32 aimendback;
        f32 aimendsideback;







        PropRecord *weapons_held[3];
        union {
            s32 fireslot_word;
            s8 fireslot[2];
        };
        int *ptr_SEbuffer3;
        int *ptr_SEbuffer4;
        int field_178[2];







        ChrRecord_f180 unk180[2];

        PropRecord *handle_positiondata_hat;
    } ChrRecord;


#pragma region IndividualObjectTypes

    struct collision_data {
        s32 unk00;

        rect4f unk04;

        s32 unk24;
        s32 unk28;
        s32 unk2C;

        s32 unk30;
        s32 unk34;
        s32 unk38;
        s32 unk3C;

        s32 unk40;
        f32 unk44;


        f32 unk48;
    };

    typedef struct Projectile {
        u32 flags;
        coord3d speed;
        coord3d unk10;

        f32 unk1C;
        Mtxf mtx;

        f32 unk60;
        f32 unk64;

        f32 unk68[4];
        f32 unk78[4];

        struct PropRecord* ownerprop;
        f32 unk8C;

        u32 unk90;
        f32 unk94;
        struct ALSoundState* sound1;
        struct ALSoundState* sound2;

        u32 unkA0;
        u32 unkA4;
        u32 unkA8;
        u32 unkAC;

        f32 unkB0;
        f32 unkB4;
        DROPTYPE droptype;
        s32 refreshrate;

        f32 unkC0;
        f32 unkC4;
        f32 unkC8;

        u8 unkCC;
        u8 unkCD;
        u8 unkCE;
        u8 unkCF;

        u32 unkD0;
        f32 unkd4;
        u32 unkD8;
        u32 unkDC;

        u32 unkE0;
        struct ObjectRecord* obj;
        s32 unkE8;
    } Projectile;

    typedef struct Embedment {
                  s32 flags;
                  Mtxf matrix;
                  struct Projectile *projectile;
    } Embedment;




    typedef struct ObjectRecord
    {
        struct PropDefHeaderRecord;
        s16 obj;







        s16 pad;
# 2583 "src/bondtypes.h"
        u32 flags;
# 2622 "src/bondtypes.h"
        u32 flags2;
        PropRecord *prop;
        Model *model;
# 2644 "src/bondtypes.h"
        Mtxf mtx;
        coord3d runtime_pos;
        union
        {
# 2693 "src/bondtypes.h"
            u32 runtime_bitflags;
        };
        struct collision_data *ptr_allocated_collisiondata_block;

        union {
            struct Projectile *projectile;
            struct Embedment *embedment;
        };

        f32 maxdamage;
        f32 damage;
        rgba_u8 shadecol;
        rgba_u8 nextcol;
    } ObjectRecord;
# 2715 "src/bondtypes.h"
    typedef struct DoorRecord
    {
        struct ObjectRecord;
# 2758 "src/bondtypes.h"
        s32 linkedDoorOffset;





        f32 maxFrac;





        f32 perimFrac;





        f32 accel;





        f32 decel;





        f32 maxSpeed;
# 2803 "src/bondtypes.h"
        u16 doorFlags;





        u16 doorType;





        u32 keyflags;





        u32 autoCloseFrames;






        u32 doorOpenSound;





        f32 frac;

        f32 unkac;
        f32 unkb0;





        f32 openPosition;





        f32 speed;






        s8 openstate;

        u8 unkbd;


        s16 calculatedopacity;
        s32 TintDist;
        s16 CullDist;
        s8 soundType;
        s8 fadeTime60;





        struct DoorRecord *linkedDoor;

        Vertex* unkcc;

        struct ModelRoData_BoundingBoxRecord bbox;
# 2884 "src/bondtypes.h"
        u32 openedTime;





        s32 portalNumber;







        ALSoundState *openSoundState;

        ALSoundState *closeSoundState;
# 2911 "src/bondtypes.h"
        union {
            s32 lastcalc60i;
            f32 lastcalc60f;
        };
    } DoorRecord;
# 2928 "src/bondtypes.h"
    typedef struct GlobalDoorScaleRecord
    {
        struct PropDefHeaderRecord;
        s32 Scale;
    } GlobalDoorScaleRecord;
# 2942 "src/bondtypes.h"
    typedef struct KeyRecord
    {
        struct ObjectRecord;
        union
        {
            s8 keyID;
            u32 keyflags;
        };
    } KeyRecord;
# 2963 "src/bondtypes.h"
    struct beam {
                 s8 age;
                 s8 weaponnum;
                 struct coord3d from;
                 struct coord3d dir;
                 f32 maxdist;
                 f32 speed;
                 f32 mindist;
                 f32 dist;
    };

    typedef struct AutogunRecord
    {
        struct ObjectRecord;
        s32 padID;


        f32 rot_related;


        f32 unk88;


        f32 unk8C;


        f32 unk90;


        f32 unk94;
        f32 unk98;


        f32 unk9C;


        f32 unkA0;


        f32 speed;






        f32 aimdist;
        s32 unkAC;


        f32 unkB0;


        f32 unkB4;
        s32 unkB8;
        s32 unkBC;
        s32 unkC0;
        ALSoundState *unkC4;
        ALSoundState *unkC8;
        struct beam *beam;





        s32 is_active;


        f32 unkD4;
    } AutogunRecord;


    typedef struct CCTVRecord
    {
        struct ObjectRecord;
        s32 pad_cctv;
        Mtxf unk84;
        f32 unkC4;
        f32 unkC8;
        f32 unkCC;

        f32 unkD0;
        s32 unkD4;
        f32 unkD8;
        f32 unkDC;

        s32 timer;
        s32 convert_to_f32;
        f32 unkE8;
        s32 unkEC;

        s32 unkF0;
        s32 unkF4;
        s32 unkF8;
    } CCTVRecord;







    typedef struct AmmoCrateRecord
    {
        struct ObjectRecord;
        AMMOTYPE ammoType;
    } AmmoCrateRecord;







    typedef struct WeaponObjRecord
    {
        struct ObjectRecord;
        s8 weaponnum;
        s8 LinkedWeaponType;
        s16 timer;
# 3095 "src/bondtypes.h"
        struct WeaponObjRecord *dualweapon;
    } WeaponObjRecord;







    typedef struct GuardRecord
    {
        struct PropDefHeaderRecord;
        u16 chrnum;
        u16 PadID;
        u16 BodyID;
        u16 AIListID;
        u16 Preset;
        u16 chrpreset1;
        u16 health;
        u16 ReactionTime;





        u16 bitflags;
        s16 HeadID;
        ChrRecord *Data;
    } GuardRecord;







    typedef struct MonitorRecord
    {
        u32 *cmdlist;
        u16 offset;
        s16 pause60;
        struct sImageTableEntry *tconfig;
        f32 rot;
        f32 xscale;
        f32 xscalefrac;
        f32 xscaleinc;
        f32 xscaleold;
        f32 xscalenew;
        f32 yscale;
        f32 yscalefrac;
        f32 yscaleinc;
        f32 yscaleold;
        f32 yscalenew;
        f32 xmid;
        f32 xmidfrac;
        f32 xmidinc;
        f32 xmidold;
        f32 xmidnew;
        f32 ymid;
        f32 ymidfrac;
        f32 ymidinc;
        f32 ymidold;
        f32 ymidnew;
        u8 red;
        u8 redold;
        u8 rednew;
        u8 green;
        u8 greenold;
        u8 greennew;
        u8 blue;
        u8 blueold;
        u8 bluenew;
        u8 alpha;
        u8 alphaold;
        u8 alphanew;
        f32 colfrac;
        f32 colinc;
    } MonitorRecord;


    typedef struct MonitorObjRecord
    {
        struct ObjectRecord;
        MonitorRecord Monitor;
        s32 OwnerOffset;
        s32 OwnerPart;
        s32 ImageNum;
    } MonitorObjRecord;






    typedef struct MultiMonitorObjRecord
    {
        struct ObjectRecord;
        MonitorRecord Monitor[4];
        u8 ImageNums[4];
    } MultiMonitorObjRecord;
# 3213 "src/bondtypes.h"
    typedef struct LinkRecord
    {
        struct PropDefHeaderRecord;
        union
        {
            struct PropRecord *first;
            s32 Index1;
        };
        union
        {
            struct PropRecord *second;
            s32 Index2;
        };
        struct LinkRecord *next;
    } LinkRecord;
# 3236 "src/bondtypes.h"
    typedef struct HatRecord
    {
        struct ObjectRecord;
    } HatRecord;


    typedef struct GuardAttributeRecord
    {
        struct PropDefHeaderRecord;
        s32 chrnum;
        s16 unk8;
        s8 unkA;
        s8 GrenadeProb;
    } GuardAttributeRecord;






    typedef LinkRecord SwitchRecord;






    typedef struct MultiAmmoCrateRecord
    {
        struct ObjectRecord;
        u16 unk80;
        u16 quantities[AMMOTYPE_GLOBAL_MAX];
    } MultiAmmoCrateRecord;






    typedef struct BodyArmourRecord
    {
        struct ObjectRecord;
        f32 initialamount;
        f32 amount;
    } BodyArmourRecord;






    typedef struct TagObjectRecord
    {
        struct PropDefHeaderRecord;
        u16 ID;
        s16 OffsetToObj;
        struct TagObjectRecord *NextTag;
        ObjectRecord * TaggedObject;
    } TagObjectRecord;






    typedef struct WatchMenuObjectiveTextRecord
    {
        struct PropDefHeaderRecord;
        u16 unk4;
    } WatchMenuObjectiveTextRecord;






    typedef struct MissionObjectiveRecord
    {
        struct PropDefHeaderRecord;
        s32 ObjRefID;
        u32 TextID;
        DIFFICULTY MinDificulty;
        WatchMenuObjectiveTextRecord *nextentry;
    } MissionObjectiveRecord;
# 3330 "src/bondtypes.h"
    typedef struct DestroyObjectRecord
    {
        struct PropDefHeaderRecord;
        u16 unk4;
    } DestroyObjectRecord;






    typedef struct CompleteConditionRecord
    {
        struct PropDefHeaderRecord;
        u16 unk4;
    } CompleteConditionRecord;






    typedef struct FailConditionRecord
    {
        struct PropDefHeaderRecord;
        u16 unk4;
    } FailConditionRecord;






    typedef struct CollectObjectRecord
    {
        struct PropDefHeaderRecord;
        u16 unk4;
    } CollectObjectRecord;






    typedef struct DepositObjectRecord
    {
        struct PropDefHeaderRecord;
        u16 unk4;
    } DepositObjectRecord;






    typedef struct PhotographObjectRecord
    {
        struct PropDefHeaderRecord;
        u16 unk4;
        u16 unk6;
        u32 unk8;
        PropRecord *lastprop;
    } PhotographObjectRecord;






    typedef struct NULLObjectRecord
    {
        struct PropDefHeaderRecord;
    } NULLObjectRecord;






    typedef struct EnterRoomRecord
    {
        struct PropDefHeaderRecord;
        u16 unk4;
        u32 unk8;
        s32 lastroomentered;
    } EnterRoomRecord;






    typedef struct DepositObjectInRoomRecord
    {
        struct PropDefHeaderRecord;
        u16 unk4;
    } DepositObjectInRoomRecord;






    typedef struct CoopyObjectRecord
    {
        struct PropDefHeaderRecord;
        u16 unk4;
    } CoopyObjectRecord;






    typedef struct GasReleasingRecord
    {
        struct PropDefHeaderRecord;
        u16 unk4;
    } GasReleasingRecord;






    typedef struct RenameObjectRecord
    {
        struct PropDefHeaderRecord;
        s32 TagID;
        s32 unk08;
        s32 unk0c;
        s32 unk10;
        s32 unk14;
        s32 unk18;
        s32 unk1c;
        s32 unk20;
        ObjectRecord *renobj;
    } RenameObjectRecord;






    typedef struct LockDoorRecord
    {
        struct PropDefHeaderRecord;
        union
        {
            struct DoorRecord* door;
            s32 Index1;
        };
        union
        {
            struct ObjectRecord* lock;
            s32 Index2;
        };
        struct LockDoorRecord* next;
    } LockDoorRecord;
# 3497 "src/bondtypes.h"
    typedef struct VehichleRecord
    {
        struct ObjectRecord;
        AIRecord * ailist;
        u16 aioffset;
        s16 aireturnlist;
        f32 speed;
        f32 wheelxrot;
        f32 wheelyrot;
        f32 speedaim;
        f32 speedtime60;
        f32 turnrot60;
        f32 roty;
        PathRecord *path;
        s32 nextstep;
        struct ALSoundState *Sound;
    } VehichleRecord;
# 3524 "src/bondtypes.h"
    typedef struct AircraftRecord
    {
        struct ObjectRecord;
        AIRecord * ailist;
        u16 aioffset;
        s16 aireturnlist;
        f32 rotoryrot;
        f32 rotaryspeed;
        f32 rotaryspeedaim;
        f32 rotaryspeedtime;
        f32 speed;
        f32 speedaim;
        f32 speedtime60;
        f32 yrot;
        s32 nextstep;
        PathRecord *path;
        struct ALSoundState *Sound;
    } AircraftRecord;
# 3550 "src/bondtypes.h"
    typedef struct GlassRecord
    {
        struct ObjectRecord;
        coord3d normal;
    } GlassRecord;


    typedef struct SafeRecord
    {
        struct ObjectRecord;
        coord3d normal;
    } SafeRecord;


    typedef struct SafeObjectRecord
    {
        struct PropDefHeaderRecord;
        union {
            struct ObjectRecord *item;
            s32 Index1;
        };
        union {
            struct SafeRecord *safe;
            s32 Index2;
        };
        union {
            struct DoorRecord *door;
            s32 Index3;
        };
        struct SafeObjectRecord *next;
    } SafeObjectRecord;



    typedef struct TankRecord
    {
        struct ObjectRecord;
        struct collision_data *unk80;
        struct rect4f rect;







        s32 unkA4;
        s32 unkA8;
        s32 unkAC;
        s32 unkB0;
        s32 unkB4;
        s32 unkB8;
        s32 unkBC;
        s32 unkC0;
        s32 is_firing_tank;
        f32 turret_vertical_angle;
        f32 turret_orientation_angle;
        f32 unkD0;
        f32 stan_y;
        s32 unkD8;
        f32 tank_orientation_angle;
    } TankRecord;






    typedef struct CutsceneRecord
    {
        struct PropDefHeaderRecord;
        struct coord3d pos;
        f32 theta;
        f32 verta;
        s32 pad;
    } CutsceneRecord;






    typedef struct TintedGlassRecord
    {
        struct ObjectRecord;
        s32 TintDist;
        s32 CullDist;
        s32 calculatedopacity;
        s32 portalnum;
        f32 unk90;
    } TintedGlassRecord;






#pragma endregion IndividualObjectTypes
#pragma endregion

#pragma region Intro

union SetupFloatInt
{
    f32 fval;
    s32 ival;
};




struct SetupIntroEmpty
{
    s32 type;
};





struct SetupIntroSpawn
{
    s32 type;
    s32 index;
    s32 is_demo_playback;
};





struct SetupIntroItem
{
    s32 type;
    s32 item_right;
    s32 item_left;
    s32 is_demo_playback;
};





struct SetupIntroAmmo
{
    s32 type;
    s32 ammo_type;
    s32 ammo_amount;
    s32 is_demo_playback;
};





struct SetupIntroSwirl
{
    s32 type;
    s32 unk04;
    union SetupFloatInt unk08;
    union SetupFloatInt unk0C;
    union SetupFloatInt unk10;
    union SetupFloatInt unk14;
    union SetupFloatInt unk18;
    s32 unk1C;
};





struct SetupIntroAnim
{
    s32 type;
    s32 intro_anim;
};





struct SetupIntroCuff
{
    s32 type;
    s32 bondtype;
};





struct SetupIntroCamera
{
    s32 type;
    union SetupFloatInt unk04;
    union SetupFloatInt unk08;
    union SetupFloatInt unk0C;
    union SetupFloatInt unk10;
    union SetupFloatInt unk14;
    s32 unk18;
    union {
        u16 lang_index[2];
        char* lang_ptr;
    } lang1c;
    union {
        s32 lang_index;
        char *lang_ptr;
    } lang20;
    struct SetupIntroCamera *prev;
};





struct SetupIntroWatch
{
    s32 type;
    s32 hours;
    s32 minutes;
};




typedef struct CreditsEntry_s {
    u16 TextId1;
    u16 TextId2;
    s16 Position1;




    u16 Alignment1;

    s16 Position2;




    u16 Alignment2;

} CreditsEntry;





struct SetupIntroCredits
{
    s32 type;
    s32 unk04;
};


#pragma endregion Intro

#pragma region Objectives

    struct watchMenuObjectiveText
    {
        u32 id;
        enum WATCH_BRIEFING_PAGE menu;
        u16 reserved;
        u16 text;
        struct watchMenuObjectiveText *nextentry;
    };


    struct objective_entry
    {
        u32 id;
        enum WATCH_BRIEFING_PAGE menu;
        u16 reserved;
        u16 text;
        u16 unkC;
        u8 unkD;
        s8 difficulty;
    };
#pragma endregion Objectives

#pragma region gamefile
#pragma endregion gamefile

#pragma region Images
    typedef struct sImageTableEntry
    {
        u32 index;
        u8 width;
        u8 height;
        u8 level;
        u8 format;
        u8 depth;
        u8 flagsS;
        u8 flagsT;
        u8 pad;
    } sImageTableEntry;
#pragma endregion Images

#pragma region Player

    typedef struct SkyRelated38 {
        f32 unk00;
        f32 unk04;
        f32 unk08;
        f32 unk0c;
        f32 r;
        f32 g;
        f32 b;
        f32 a;
        f32 unk20;
        f32 unk24;
        f32 unk28;
        f32 unk2c;
        f32 unk30;
        f32 unk34;
    } SkyRelated38;

    typedef struct SkyRelated18 {
        f32 unk00;
        f32 unk04;
        f32 unk08;
        f32 unk0c;
        f32 unk10;
        u8 r;
        u8 g;
        u8 b;
        u8 a;
    } SkyRelated18;

#pragma endregion Player

#pragma region stagesetup.h
    struct pname {
        union {
            char *p;
            s32 offset;
        };
    };

    typedef struct stagesetup
    {
        waypoint *pathwaypoints;
        waygroup *waypointgroups;
        struct SetupIntroEmpty *intro;
        PropDefHeaderRecord *propDefs;
        PathRecord *patrolpaths;
        AIListRecord *ailists;
        PadRecord *pads;
        BoundPadRecord *boundpads;
        struct pname *padnames;
        struct pname *boundpadnames;
    } stagesetup;


#pragma endregion stagesetup.h

    typedef struct sfxRecord
    {
        ALSoundState *state;
        s32 Volume2;
        s32 sfxID;
        s32 Volume;
        coord3d *pos;
        ObjectRecord *Obj;
    } sfxRecord;
# 3934 "src/bondtypes.h"
typedef struct object_standard {
    s16 scale;
    u8 state;
    u8 type;
    u16 objectID;
    u16 presetID;
    u32 flags1;
    u32 flags2;
    void* ptrPOSData;
    void* ptrObjInstanceController;
    f32 runtime_MATRIX[4][4];
    f32 xPOS;
    f32 yPOS;
    f32 zPOS;
    int bitflags;
    int ptrCollisionblock;
    int field_6C;
    float field_70;
    short damage;
    short maxdamage;
    int field_78;
    int field_7C;
} object_standard;

typedef struct object_weapon {
    object_standard object;
    u8 gun_pickup;
    u8 linked_item;
    s16 timer;
    void* ptr_linkeditem;
} object_weapon;


struct MoveData {

    s32 analogWalk;
    s32 analogStrafe;
    s32 analogPitch;
    s32 analogTurn;


    s32 controlStickYRaw;
    s32 controlStickXRaw;
    s32 controlStickYSafe;
    s32 controlStickXSafe;


    s32 disableLookAhead;
    s32 invertPitch;
    s32 canAutoAim;
    s32 detonating;


    s32 rLeanRight;
    s32 rLeanLeft;
    s32 crouchUp;
    s32 crouchDown;


    f32 zoomInFovPersec;

    f32 zoomOutFovPersec;
    s32 zooming;
    s32 aiming;


    s32 weaponForwardOffset;
    s32 weaponBackOffset;
    f32 aimTurnRightSpeed;
    f32 aimTurnLeftSpeed;


    f32 speedVertaUp;
    f32 speedVertaDown;
    f32 tankTurnRightSpeed;
    f32 tankTurnLeftSpeed;


    s32 digitalStepRight;
    s32 digitalStepLeft;
    s32 digitalStepBack;
    s32 digitalStepForward;


    s32 canNaturalPitch;
    s32 canNaturalTurn;
    s32 canTurnTank;
    s32 canLookAhead;


    s32 btap;
    s32 triggerOn;
    s32 canManualAim;

    s32 canSwivelGun;
};


struct unkown_gun_struct
{
    union
    {
        u32 arr[2];
        struct
        {
            s32 unk00;
            s32 unk04;
        };
    };
};

struct setup_objective_text {
    s32 unk00;
    s32 unk04;
    s32 unk08;
    struct setup_objective_text *next;
};


struct criteria_roomentered {
    u32 unk00;
    u32 pad;
    u32 status;
    struct criteria_roomentered *next;
};



struct criteria_deposit {
    s32 unk00;
    s32 weaponnum;
    s32 padid;
    s32 flag;
    struct criteria_deposit *next;
};

struct criteria_picture {
    s32 unk00;
    s32 tag_id;
    s32 flag;
    struct criteria_picture *next;
};

struct PortalMetric {
    struct coord3d normal;
    f32 min;
    f32 max;
};

struct FolderSelect {
    s32 unk00;
    s32 unk04;
    s32 unk08;
};

struct damage_display_val {
    s16 unk00;
    s16 unk02;
    s16 unk04;
    s16 unk06;
    s16 unk08;
    s16 unk0A;
    s8 unk0C;
    s8 unk0D;
    s8 unk0E;
    s8 unk0F;
};

struct damage_display_parent {
    struct damage_display_val items[2];
};


struct s_display_list_something
{
    s16 unk0;
    s16 unk2;
    s16 unk4;
    s16 unk6;
    s16 unk8;
    s16 unkA;
    u8 unkC;
    u8 unkD;
    u8 unkE;
    u8 unkF;
};
# 7 "src/bondgame.h" 2

extern u32 *_bssSegmentEnd;

extern u32 *_codeSegmentStart;
extern u32 *_codeSegmentEnd;
extern u32 *_codeSegmentRomStart;
extern u32 *_codeSegmentRomEnd;

extern u32 *_csegmentSegmentStart;
extern u32 *_csegmentSegmentEnd;
extern u32 *_cdataSegmentRomStart;
extern u32 *_cdataSegmentRomEnd;

extern u32 *_inflateSegmentVaddrStart;
extern u32 *_inflateSegmentVaddrEnd;
extern u32 *_inflateSegmentRomStart;
extern u32 *_inflateSegmentRomEnd;

extern u32 *_gameSegmentVaddrStart;
extern u32 *_gameSegmentVaddrEnd;
extern u32 *_gameSegmentRomStart;
extern u32 *_gameSegmentRomEnd;

extern u32* _animation_dataSegmentRomStart;
extern u32* _animation_dataSegmentStart;
extern u32* _animation_dataSegmentEnd;

extern u32* _alt_startSegmentRomStart;
extern u32* _alt_startSegmentStart;

extern u8 sp_boot[0x10];
extern u8 sp_rmon[0x300];
extern u8 sp_idle[0x40];
extern u8 sp_shed[0x200];
extern u8 sp_main[0x8000];
extern u8 sp_audi[0x1000];
extern u8 sp_debug[0x6B0];
# 16 "<stdin>" 2
# 1 "src/bondaicommands.h" 1
# 229 "src/bondaicommands.h"
# 1 "include/ultra64.h" 1
# 230 "src/bondaicommands.h" 2
# 287 "src/bondaicommands.h"
typedef enum GAILISTID
{




    GAILIST_AIM_AT_BOND,






    GAILIST_DEAD_AI,
# 310 "src/bondaicommands.h"
    GAILIST_STANDARD_GUARD,







    GAILIST_PLAY_IDLE_ANIMATION,







    GAILIST_BASH_KEYBOARD,






    GAILIST_SIMPLE_GUARD_DEAF,







    GAILIST_ATTACK_BOND,






    GAILIST_SIMPLE_GUARD,







    GAILIST_RUN_TO_BOND,







    GAILIST_SIMPLE_GUARD_ALARM_RAISER,







    GAILIST_STARTLE_AND_RUN_TO_BOND,






    GAILIST_TRY_CLONE_SEND_OR_RUN_TO_BOND,





    GAILIST_STANDARD_CLONE,





    GAILIST_PERSISTENTLY_CHASE_AND_ATTACK_BOND,





    GAILIST_WAIT_ONE_SECOND,





    GAILIST_END_LEVEL,






    GAILIST_DRAW_TT33_AND_ATTCK_BOND,





    GAILIST_REMOVE_CHR
} GAILISTID;
# 448 "src/bondaicommands.h"
typedef enum HITTARGET
{
    HIT_NULL_PART,
    HIT_LEFT_FOOT,
    HIT_LEFT_LEG,
    HIT_LEFT_THIGH,
    HIT_RIGHT_FOOT,
    HIT_RIGHT_LEG,
    HIT_RIGHT_THIGH,
    HIT_PELVIS,
    HIT_HEAD,
    HIT_LEFT_HAND,
    HIT_LEFT_ARM,
    HIT_LEFT_SHOULDER,
    HIT_RIGHT_HAND,
    HIT_RIGHT_ARM,
    HIT_RIGHT_SHOULDER,
    HIT_CHEST,
    HIT_GUN = 0x64,
    HIT_HAT = 0x6E,
    HIT_GENERAL = 0xC8,
    HIT_GENERALHALF = 0xC9
} HITTARGET;
# 649 "src/bondaicommands.h"
# 1 "src/aicommands2.h" 1
# 1 "tools/cmdbuilder.c"
# 1 "include/CPPLib.h"
# 400 "include/CPPLib.h"
# 648 "include/CPPLib.h"
# 10 "tools/cmdbuilder.c"
# 1 "src/aicommands.def"
# 1 "include/CPPLib.h"
# 649 "include/CPPLib.h"
# 87 "src/aicommands.def"
# 169 "src/aicommands.def"
# 175 "src/aicommands.def"
# 209 "src/aicommands.def"
# 226 "src/aicommands.def"
# 241 "src/aicommands.def"
# 274 "src/aicommands.def"
# 315 "src/aicommands.def"
# 391 "src/aicommands.def"
# 460 "src/aicommands.def"
# 475 "src/aicommands.def"
# 490 "src/aicommands.def"
# 505 "src/aicommands.def"
# 651 "src/aicommands.def"
# 683 "src/aicommands.def"
# 764 "src/aicommands.def"
# 839 "src/aicommands.def"
# 871 "src/aicommands.def"
# 1186 "src/aicommands.def"
# 1190 "src/aicommands.def"
# 1194 "src/aicommands.def"
# 1198 "src/aicommands.def"
# 1258 "src/aicommands.def"
# 1262 "src/aicommands.def"
# 1266 "src/aicommands.def"
# 1270 "src/aicommands.def"
# 1331 "src/aicommands.def"
# 1335 "src/aicommands.def"
# 1339 "src/aicommands.def"
# 1343 "src/aicommands.def"
# 1397 "src/aicommands.def"
# 1401 "src/aicommands.def"
# 1449 "src/aicommands.def"
# 1515 "src/aicommands.def"
# 1519 "src/aicommands.def"
# 1686 "src/aicommands.def"
# 1829 "src/aicommands.def"
# 1865 "src/aicommands.def"
# 1920 "src/aicommands.def"
# 1997 "src/aicommands.def"
# 2027 "src/aicommands.def"
# 2285 "src/aicommands.def"
# 2334 "src/aicommands.def"
# 2383 "src/aicommands.def"
# 2475 "src/aicommands.def"
# 2525 "src/aicommands.def"
# 2530 "src/aicommands.def"
# 2613 "src/aicommands.def"
# 2662 "src/aicommands.def"
# 2711 "src/aicommands.def"
# 3519 "src/aicommands.def"
# 3536 "src/aicommands.def"
# 3599 "src/aicommands.def"
# 3616 "src/aicommands.def"
# 3677 "src/aicommands.def"
# 3694 "src/aicommands.def"
# 3755 "src/aicommands.def"
# 3772 "src/aicommands.def"
# 3827 "src/aicommands.def"
# 3883 "src/aicommands.def"
# 3934 "src/aicommands.def"
# 3949 "src/aicommands.def"
# 3953 "src/aicommands.def"
# 4004 "src/aicommands.def"
# 4018 "src/aicommands.def"
# 4022 "src/aicommands.def"
# 4080 "src/aicommands.def"
# 4138 "src/aicommands.def"
# 4198 "src/aicommands.def"
# 4255 "src/aicommands.def"
# 4312 "src/aicommands.def"
# 4367 "src/aicommands.def"
# 5306 "src/aicommands.def"
# 5310 "src/aicommands.def"
# 5314 "src/aicommands.def"
# 5318 "src/aicommands.def"
# 5322 "src/aicommands.def"
# 5326 "src/aicommands.def"
# 5330 "src/aicommands.def"
# 5334 "src/aicommands.def"
# 5338 "src/aicommands.def"
# 5342 "src/aicommands.def"
# 5346 "src/aicommands.def"
# 5350 "src/aicommands.def"
# 5354 "src/aicommands.def"
# 5358 "src/aicommands.def"
# 6217 "src/aicommands.def"
# 6221 "src/aicommands.def"
# 6225 "src/aicommands.def"
# 6277 "src/aicommands.def"
# 6281 "src/aicommands.def"
# 6285 "src/aicommands.def"
# 6337 "src/aicommands.def"
# 6454 "src/aicommands.def"
# 6647 "src/aicommands.def"
# 6851 "src/aicommands.def"
# 6887 "src/aicommands.def"
# 6923 "src/aicommands.def"
# 6956 "src/aicommands.def"
# 7002 "src/aicommands.def"
# 7051 "src/aicommands.def"
# 7067 "src/aicommands.def"
# 7227 "src/aicommands.def"
# 7234 "src/aicommands.def"
# 7272 "src/aicommands.def"
# 7279 "src/aicommands.def"
# 7330 "src/aicommands.def"
# 7368 "src/aicommands.def"
# 7406 "src/aicommands.def"
# 7618 "src/aicommands.def"
# 7662 "src/aicommands.def"
# 7714 "src/aicommands.def"
# 8105 "src/aicommands.def"
# 8112 "src/aicommands.def"
# 8143 "src/aicommands.def"
# 8189 "src/aicommands.def"
# 8221 "src/aicommands.def"
# 8286 "src/aicommands.def"
# 8293 "src/aicommands.def"
# 8326 "src/aicommands.def"
# 8333 "src/aicommands.def"
# 8366 "src/aicommands.def"
# 8373 "src/aicommands.def"
# 8406 "src/aicommands.def"
# 8413 "src/aicommands.def"
# 8460 "src/aicommands.def"
# 8509 "src/aicommands.def"
# 8522 "src/aicommands.def"
# 8526 "src/aicommands.def"
# 8576 "src/aicommands.def"
# 8589 "src/aicommands.def"
# 8593 "src/aicommands.def"
# 8622 "src/aicommands.def"
# 8654 "src/aicommands.def"
# 8718 "src/aicommands.def"
# 8748 "src/aicommands.def"
# 8921 "src/aicommands.def"
# 8969 "src/aicommands.def"
# 9072 "src/aicommands.def"
# 9233 "src/aicommands.def"
# 9249 "src/aicommands.def"
# 9372 "src/aicommands.def"
# 9379 "src/aicommands.def"
# 9627 "src/aicommands.def"
# 9766 "src/aicommands.def"
# 9770 "src/aicommands.def"
# 9774 "src/aicommands.def"
# 9822 "src/aicommands.def"
# 10102 "src/aicommands.def"
# 10426 "src/aicommands.def"
# 10433 "src/aicommands.def"
# 10765 "src/aicommands.def"
# 10911 "src/aicommands.def"
# 11074 "src/aicommands.def"
# 11184 "src/aicommands.def"
# 11313 "src/aicommands.def"
# 11519 "src/aicommands.def"
# 11557 "src/aicommands.def"
# 11589 "src/aicommands.def"
# 11707 "src/aicommands.def"
# 11737 "src/aicommands.def"
# 11790 "src/aicommands.def"
# 11846 "src/aicommands.def"
# 195 "tools/cmdbuilder.c"
# 650 "src/bondaicommands.h" 2
# 17 "<stdin>" 2
# 1 "src/game/chrobjdata.h" 1



# 1 "include/ultra64.h" 1
# 5 "src/game/chrobjdata.h" 2







struct headHat
{
    float xoffset;
    float yoffset;
    float zoffset;
    float xsize;
    float ysize;
    float zsize;
};

extern struct ItemModelFileRecord PitemZ_entries[];

extern struct ModelSkeleton skeleton_cctv;
extern struct ModelSkeleton skeleton_console_one_screen;
extern struct ModelSkeleton skeleton_console_four_screen;
extern struct ModelSkeleton skeleton_tv_holder;
extern struct ModelSkeleton skeleton_rotating_stuff;
extern struct ModelSkeleton skeleton_eyelid_door;
extern struct ModelSkeleton skeleton_iris_door;
extern struct ModelSkeleton skeleton_walletbond;
extern struct ModelSkeleton skeleton_car;
extern struct ModelSkeleton skeleton_flying;
extern struct ModelSkeleton skeleton_door;
extern struct ModelSkeleton skeleton_tank;
extern struct ModelSkeleton skeleton_hat;
extern struct ModelSkeleton skeleton_standard_object;
extern struct ModelSkeleton skeleton_prop_weapon;
extern struct ModelSkeleton skeleton_guard;
extern struct ModelNode player_gait_hdr;
extern struct ModelNode player_gait_pos_hdr_1;
extern struct ModelNode player_gait_pos_hdr_2;
extern struct ModelNode player_gait_pos_hdr_3;
extern struct ModelRoData_HeaderRecord player_gait_obj;
extern struct ModelRoData_GroupRecord player_gait_pos_1;
extern struct ModelRoData_GroupRecord player_gait_pos_2;
extern struct ModelRoData_GroupRecord player_gait_pos_3;

extern struct ModelFileHeader player_gait_object_header;

extern struct ModelFileHeader wppk_header;
extern struct ModelFileHeader wppksil_header;
extern struct ModelFileHeader tt33_header;
extern struct ModelFileHeader skorpion_header;
extern struct ModelFileHeader ak47_header;
extern struct ModelFileHeader uzi_header;
extern struct ModelFileHeader mp5k_header;
extern struct ModelFileHeader mp5ksil_header;
extern struct ModelFileHeader m16_header;
extern struct ModelFileHeader fnp90_header;
extern struct ModelFileHeader G8003C900_struct;
extern struct ModelFileHeader ruger_header;
extern struct ModelFileHeader goldengun_header;
extern struct ModelFileHeader shotgun_header;
extern struct ModelFileHeader autoshot_header;
extern struct ModelFileHeader grenadelaunch_header;
extern struct ModelFileHeader spectre_header;
extern struct ModelFileHeader laser_header;
extern struct ModelFileHeader rocketlaunch_header;
extern struct ModelFileHeader knife_header;
extern struct ModelFileHeader throwknife_header;
extern struct ModelFileHeader taser_header;
extern struct ModelFileHeader remotemine_header;
extern struct ModelFileHeader proximitymine_header;
extern struct ModelFileHeader timedmine_header;
extern struct ModelFileHeader trigger_header;
extern struct ModelFileHeader grenade_header;
extern struct ModelFileHeader fist_header;
extern struct ModelFileHeader sniperrifle_header;
extern struct ModelFileHeader cartridge_header;
extern struct ModelFileHeader cartrifle_header;
extern struct ModelFileHeader cartblue_header;
extern struct ModelFileHeader cartshell_header;
extern struct ModelFileHeader joypad_header;
extern struct ModelFileHeader G8003CC00_struct;
extern struct ModelFileHeader G8003CC20_struct;
extern struct ModelFileHeader bombcase_header;
extern struct ModelFileHeader flarepistol_header;
extern struct ModelFileHeader pitongun_header;
extern struct ModelFileHeader G8003CCA0_struct;
extern struct ModelFileHeader silverwppk_header;
extern struct ModelFileHeader goldwppk_header;
extern struct ModelFileHeader G8003CD00_struct;
extern struct ModelFileHeader bungee_header;
extern struct ModelFileHeader doordecoder_header;
extern struct ModelFileHeader bombdefuser_header;
extern struct ModelFileHeader bugdetector_header;
extern struct ModelFileHeader safecrackercase_header;
extern struct ModelFileHeader camera_header;
extern struct ModelFileHeader lockexploder_header;
extern struct ModelFileHeader doorexploder_header;
extern struct ModelFileHeader keyanalysercase_header;
extern struct ModelFileHeader weaponcase_header;
extern struct ModelFileHeader keycard_header;
extern struct ModelFileHeader keyyale_header;
extern struct ModelFileHeader keybolt_header;
extern struct ModelFileHeader bug_header;
extern struct ModelFileHeader microcamera_header;
extern struct ModelFileHeader explosivefloppy_header;
extern struct ModelFileHeader goldeneyekey_header;
extern struct ModelFileHeader polarizedglasses_header;
extern struct ModelFileHeader creditcard_header;
extern struct ModelFileHeader darkglasses_header;
extern struct ModelFileHeader watchidentifier_header;
extern struct ModelFileHeader watchcommunicator_header;
extern struct ModelFileHeader watchlaser_header;
extern struct ModelFileHeader watchgeigercounter_header;
extern struct ModelFileHeader watchmagnetrepel_header;
extern struct ModelFileHeader watchmagnetattract_header;
extern struct ModelFileHeader gaskeyring_header;
extern struct ModelFileHeader datathief_header;
extern struct ModelFileHeader briefcase_header;
extern struct ModelFileHeader blackbox_header;
extern struct ModelFileHeader plastique_header;
extern struct ModelFileHeader videotape_header;
extern struct ModelFileHeader clipboard_header;
extern struct ModelFileHeader stafflist_header;
extern struct ModelFileHeader dossierred_header;
extern struct ModelFileHeader audiotape_header;
extern struct ModelFileHeader dattape_header;
extern struct ModelFileHeader plans_header;
extern struct ModelFileHeader spyfile_header;
extern struct ModelFileHeader blueprints_header;
extern struct ModelFileHeader circuitboard_header;
extern struct ModelFileHeader map_header;
extern struct ModelFileHeader spooltape_header;
extern struct ModelFileHeader microfilm_header;
extern struct ModelFileHeader microcode_header;
extern struct ModelFileHeader lectre_header;
extern struct ModelFileHeader money_header;
extern struct ModelFileHeader goldbar_header;
extern struct ModelFileHeader heroin_header;
extern struct ModelSkeleton skeleton_g_weapon;

extern struct ModelFileHeader camguard_header;
extern struct ModelFileHeader greyguard_header;
extern struct ModelFileHeader oliveguard_header;
extern struct ModelFileHeader rusguard_header;
extern struct ModelFileHeader trevguard_header;
extern struct ModelFileHeader boris_header;
extern struct ModelFileHeader orumov_header;
extern struct ModelFileHeader trevelyan_header;
extern struct ModelFileHeader boilertrev_header;
extern struct ModelFileHeader valentin_header;
extern struct ModelFileHeader xenia_header;
extern struct ModelFileHeader baronsamedi_header;
extern struct ModelFileHeader jaws_header;
extern struct ModelFileHeader mayday_header;
extern struct ModelFileHeader oddjob_header;
extern struct ModelFileHeader natalya_header;
extern struct ModelFileHeader armourguard_header;
extern struct ModelFileHeader commguard_header;
extern struct ModelFileHeader greatguard_header;
extern struct ModelFileHeader navyguard_header;
extern struct ModelFileHeader snowguard_header;
extern struct ModelFileHeader bluewoman_header;
extern struct ModelFileHeader fattechwoman_header;
extern struct ModelFileHeader techwoman_header;
extern struct ModelFileHeader jeanwoman_header;
extern struct ModelFileHeader greyman_header;
extern struct ModelFileHeader blueman_header;
extern struct ModelFileHeader redman_header;
extern struct ModelFileHeader cardiman_header;
extern struct ModelFileHeader checkman_header;
extern struct ModelFileHeader techman_header;
extern struct ModelFileHeader ilot_header;
extern struct ModelFileHeader greatguard2_header;
extern struct ModelFileHeader bluecamguard_header;
extern struct ModelFileHeader moonguard_header;
extern struct ModelFileHeader moonfemale_header;
extern struct ModelFileHeader boilerbond_header;
extern struct ModelFileHeader suitbond_header;
extern struct ModelFileHeader timberbond_header;
extern struct ModelFileHeader snowbond_header;
extern struct ModelFileHeader djbond_header;
extern struct ModelFileHeader suit_lf_hand_header;
extern struct ModelFileHeader headkarl_header;
extern struct ModelFileHeader headalan_header;
extern struct ModelFileHeader headpete_header;
extern struct ModelFileHeader headmartin_header;
extern struct ModelFileHeader headmark_header;
extern struct ModelFileHeader headduncan_header;
extern struct ModelFileHeader headb_header;
extern struct ModelFileHeader headshaun_header;
extern struct ModelFileHeader headdwayne_header;
extern struct ModelFileHeader headdave_header;
extern struct ModelFileHeader headgrant_header;
extern struct ModelFileHeader headdes_header;
extern struct ModelFileHeader headchris_header;
extern struct ModelFileHeader headlee_header;
extern struct ModelFileHeader headneil_header;
extern struct ModelFileHeader headjim_header;
extern struct ModelFileHeader headrobin_header;
extern struct ModelFileHeader headsteveh_header;
extern struct ModelFileHeader headbalaclava_header;
extern struct ModelFileHeader headbike_header;
extern struct ModelFileHeader headgraham_header;
extern struct ModelFileHeader headstevee_header;
extern struct ModelFileHeader headjoel_header;
extern struct ModelFileHeader headscott_header;
extern struct ModelFileHeader headjoe_header;
extern struct ModelFileHeader headken_header;
extern struct ModelFileHeader headjoe2_header;
extern struct ModelFileHeader headmishkin_header;
extern struct ModelFileHeader headsally_header;
extern struct ModelFileHeader headmarion_header;
extern struct ModelFileHeader headmandy_header;
extern struct ModelFileHeader headvivien_header;
extern struct ModelFileHeader headbrosnanboiler_header;
extern struct ModelFileHeader headbrosnansuit_header;
extern struct ModelFileHeader headbrosnantimber_header;
extern struct ModelFileHeader headbrosnansnow_header;
extern struct ModelFileHeader headbrosnan_header;
extern struct ModelFileHeader spicebond_header;

extern struct ModelFileHeader alarm1_header;
extern struct ModelFileHeader alarm2_header;
extern struct ModelFileHeader explosionbit_header;
extern struct ModelFileHeader ammo_crate1_header;
extern struct ModelFileHeader ammo_crate2_header;
extern struct ModelFileHeader ammo_crate3_header;
extern struct ModelFileHeader ammo_crate4_header;
extern struct ModelFileHeader ammo_crate5_header;
extern struct ModelFileHeader bin1_header;
extern struct ModelFileHeader blotter1_header;
extern struct ModelFileHeader book1_header;
extern struct ModelFileHeader bookshelf1_header;
extern struct ModelFileHeader bridge_console1a_header;
extern struct ModelFileHeader bridge_console1b_header;
extern struct ModelFileHeader bridge_console2a_header;
extern struct ModelFileHeader bridge_console2b_header;
extern struct ModelFileHeader bridge_console3a_header;
extern struct ModelFileHeader bridge_console3b_header;
extern struct ModelFileHeader card_box1_header;
extern struct ModelFileHeader card_box2_header;
extern struct ModelFileHeader card_box3_header;
extern struct ModelFileHeader card_box4_lg_header;
extern struct ModelFileHeader card_box5_lg_header;
extern struct ModelFileHeader card_box6_lg_header;
extern struct ModelFileHeader cctv_header;
extern struct ModelFileHeader console1_header;
extern struct ModelFileHeader console2_header;
extern struct ModelFileHeader console3_header;
extern struct ModelFileHeader console_seva_header;
extern struct ModelFileHeader console_sevb_header;
extern struct ModelFileHeader console_sevc_header;
extern struct ModelFileHeader console_sevd_header;
extern struct ModelFileHeader console_sev2a_header;
extern struct ModelFileHeader console_sev2b_header;
extern struct ModelFileHeader console_sev2c_header;
extern struct ModelFileHeader console_sev2d_header;
extern struct ModelFileHeader console_sev_GEa_header;
extern struct ModelFileHeader console_sev_GEb_header;
extern struct ModelFileHeader desk1_header;
extern struct ModelFileHeader desk2_header;
extern struct ModelFileHeader desk_lamp2_header;
extern struct ModelFileHeader disc_reader_header;
extern struct ModelFileHeader disk_drive1_header;
extern struct ModelFileHeader filing_cabinet1_header;
extern struct ModelFileHeader jerry_can1_header;
extern struct ModelFileHeader keyboard1_header;
extern struct ModelFileHeader kit_units1_header;
extern struct ModelFileHeader letter_tray1_header;
extern struct ModelFileHeader mainframe1_header;
extern struct ModelFileHeader mainframe2_header;
extern struct ModelFileHeader metal_chair1_header;
extern struct ModelFileHeader metal_crate1_header;
extern struct ModelFileHeader metal_crate2_header;
extern struct ModelFileHeader metal_crate3_header;
extern struct ModelFileHeader metal_crate4_header;
extern struct ModelFileHeader missile_rack_header;
extern struct ModelFileHeader missile_rack2_header;
extern struct ModelFileHeader oil_drum1_header;
extern struct ModelFileHeader oil_drum2_header;
extern struct ModelFileHeader oil_drum3_header;
extern struct ModelFileHeader oil_drum5_header;
extern struct ModelFileHeader oil_drum6_header;
extern struct ModelFileHeader oil_drum7_header;
extern struct ModelFileHeader padlock_header;
extern struct ModelFileHeader phone1_header;
extern struct ModelFileHeader radio_unit1_header;
extern struct ModelFileHeader radio_unit2_header;
extern struct ModelFileHeader radio_unit3_header;
extern struct ModelFileHeader radio_unit4_header;
extern struct ModelFileHeader sat1_reflect_header;
extern struct ModelFileHeader satdish_header;
extern struct ModelFileHeader satbox_header;
extern struct ModelFileHeader stool1_header;
extern struct ModelFileHeader swivel_chair1_header;
extern struct ModelFileHeader torpedo_rack_header;
extern struct ModelFileHeader tv1_header;
extern struct ModelFileHeader tv_holder_header;
extern struct ModelFileHeader tvscreen_header;
extern struct ModelFileHeader tv4screen_header;
extern struct ModelFileHeader wood_lg_crate1_header;
extern struct ModelFileHeader wood_lg_crate2_header;
extern struct ModelFileHeader wood_md_crate3_header;
extern struct ModelFileHeader wood_sm_crate4_header;
extern struct ModelFileHeader wood_sm_crate5_header;
extern struct ModelFileHeader wood_sm_crate6_header;
extern struct ModelFileHeader wooden_table1_header;
extern struct ModelFileHeader swipe_card2_header;
extern struct ModelFileHeader borg_crate_header;
extern struct ModelFileHeader boxes4x4_header;
extern struct ModelFileHeader boxes3x4_header;
extern struct ModelFileHeader boxes2x4_header;
extern struct ModelFileHeader sec_panel_header;
extern struct ModelFileHeader ICBM_nose_header;
extern struct ModelFileHeader ICBM_header;
extern struct ModelFileHeader tuning_console1_header;
extern struct ModelFileHeader desk_arecibo1_header;
extern struct ModelFileHeader locker3_header;
extern struct ModelFileHeader locker4_header;
extern struct ModelFileHeader roofgun_header;
extern struct ModelFileHeader dest_engine_header;
extern struct ModelFileHeader dest_exocet_header;
extern struct ModelFileHeader dest_gun_header;
extern struct ModelFileHeader dest_harpoon_header;
extern struct ModelFileHeader dest_seawolf_header;
extern struct ModelFileHeader window_header;
extern struct ModelFileHeader window_lib_lg1_header;
extern struct ModelFileHeader window_lib_sm1_header;
extern struct ModelFileHeader window_cor11_header;
extern struct ModelFileHeader jungle3_tree_header;
extern struct ModelFileHeader palm_header;
extern struct ModelFileHeader palmtree_header;
extern struct ModelFileHeader plant2b_header;
extern struct ModelFileHeader labbench_header;
extern struct ModelFileHeader gasbarrel_header;
extern struct ModelFileHeader gasbarrels_header;
extern struct ModelFileHeader bodyarmour_header;
extern struct ModelFileHeader bodyarmourvest_header;
extern struct ModelFileHeader gastank_header;
extern struct ModelFileHeader glassware1_header;
extern struct ModelFileHeader hatchbolt_header;
extern struct ModelFileHeader brakeunit_header;
extern struct ModelFileHeader ak47mag_header;
extern struct ModelFileHeader m16mag_header;
extern struct ModelFileHeader mp5kmag_header;
extern struct ModelFileHeader skorpionmag_header;
extern struct ModelFileHeader spectremag_header;
extern struct ModelFileHeader uzimag_header;
extern struct ModelFileHeader silencer_header;
extern struct ModelFileHeader chrextinguisher_header;
extern struct ModelFileHeader boxcartridges_header;
extern struct ModelFileHeader fnp90mag_header;
extern struct ModelFileHeader goldenshells_header;
extern struct ModelFileHeader magnumshells_header;
extern struct ModelFileHeader wppkmag_header;
extern struct ModelFileHeader tt33mag_header;
extern struct ModelFileHeader sev_door_header;
extern struct ModelFileHeader sev_door3_header;
extern struct ModelFileHeader sev_door3_wind_header;
extern struct ModelFileHeader sev_door4_wind_header;
extern struct ModelFileHeader sev_trislide_header;
extern struct ModelFileHeader sev_door_v1_header;
extern struct ModelFileHeader steel_door1_header;
extern struct ModelFileHeader steel_door2_header;
extern struct ModelFileHeader steel_door3_header;
extern struct ModelFileHeader silo_lift_door_header;
extern struct ModelFileHeader steel_door2b_header;
extern struct ModelFileHeader door_roller1_header;
extern struct ModelFileHeader door_roller2_header;
extern struct ModelFileHeader door_roller3_header;
extern struct ModelFileHeader door_roller4_header;
extern struct ModelFileHeader door_st_arec1_header;
extern struct ModelFileHeader door_st_arec2_header;
extern struct ModelFileHeader door_dest1_header;
extern struct ModelFileHeader door_dest2_header;
extern struct ModelFileHeader gas_plant_sw_do1_header;
extern struct ModelFileHeader gas_plant_sw2_do1_header;
extern struct ModelFileHeader gas_plant_sw3_do1_header;
extern struct ModelFileHeader gas_plant_sw4_do1_header;
extern struct ModelFileHeader gas_plant_met1_do1_header;
extern struct ModelFileHeader gas_plant_wc_cub1_header;
extern struct ModelFileHeader gasplant_clear_door_header;
extern struct ModelFileHeader train_door_header;
extern struct ModelFileHeader train_door2_header;
extern struct ModelFileHeader train_door3_header;
extern struct ModelFileHeader door_eyelid_header;
extern struct ModelFileHeader door_iris_header;
extern struct ModelFileHeader sevdoorwood_header;
extern struct ModelFileHeader sevdoorwind_header;
extern struct ModelFileHeader sevdoornowind_header;
extern struct ModelFileHeader sevdoormetslide_header;
extern struct ModelFileHeader cryptdoor1a_header;
extern struct ModelFileHeader cryptdoor1b_header;
extern struct ModelFileHeader cryptdoor2a_header;
extern struct ModelFileHeader cryptdoor2b_header;
extern struct ModelFileHeader cryptdoor3_header;
extern struct ModelFileHeader cryptdoor4_header;
extern struct ModelFileHeader vertdoor_header;
extern struct ModelFileHeader hatchdoor_header;
extern struct ModelFileHeader damgatedoor_header;
extern struct ModelFileHeader damtundoor_header;
extern struct ModelFileHeader damchaindoor_header;
extern struct ModelFileHeader silotopdoor_header;
extern struct ModelFileHeader doorprison1_header;
extern struct ModelFileHeader doorstatgate_header;
extern struct ModelFileHeader chrkalash_header;
extern struct ModelFileHeader chrgrenadelaunch_header;
extern struct ModelFileHeader chrknife_header;
extern struct ModelFileHeader chrlaser_header;
extern struct ModelFileHeader chrm16_header;
extern struct ModelFileHeader chrmp5k_header;
extern struct ModelFileHeader chrruger_header;
extern struct ModelFileHeader chrwppk_header;
extern struct ModelFileHeader chrshotgun_header;
extern struct ModelFileHeader chrskorpion_header;
extern struct ModelFileHeader chrspectre_header;
extern struct ModelFileHeader chruzi_header;
extern struct ModelFileHeader chrgrenade_header;
extern struct ModelFileHeader chrfnp90_header;
extern struct ModelFileHeader chrbriefcase_header;
extern struct ModelFileHeader chrremotemine_header;
extern struct ModelFileHeader chrproximitymine_header;
extern struct ModelFileHeader chrtimedmine_header;
extern struct ModelFileHeader chrrocket_header;
extern struct ModelFileHeader chrgrenaderound_header;
extern struct ModelFileHeader chrwppksil_header;
extern struct ModelFileHeader chrtt33_header;
extern struct ModelFileHeader chrmp5ksil_header;
extern struct ModelFileHeader chrautoshot_header;
extern struct ModelFileHeader chrgolden_header;
extern struct ModelFileHeader chrthrowknife_header;
extern struct ModelFileHeader chrsniperrifle_header;
extern struct ModelFileHeader chrrocketlaunch_header;
extern struct ModelFileHeader hatfurry_header;
extern struct ModelFileHeader hatfurrybrown_header;
extern struct ModelFileHeader hatfurryblack_header;
extern struct ModelFileHeader hattbird_header;
extern struct ModelFileHeader hattbirdbrown_header;
extern struct ModelFileHeader hathelmet_header;
extern struct ModelFileHeader hathelmetgrey_header;
extern struct ModelFileHeader hatmoon_header;
extern struct ModelFileHeader hatberet_header;
extern struct ModelFileHeader hatberetblue_header;
extern struct ModelFileHeader hatberetred_header;
extern struct ModelFileHeader hatpeaked_header;
extern struct ModelFileHeader chrwristdart_header;
extern struct ModelFileHeader chrexplosivepen_header;
extern struct ModelFileHeader chrbombcase_header;
extern struct ModelFileHeader chrflarepistol_header;
extern struct ModelFileHeader chrpitongun_header;
extern struct ModelFileHeader chrfingergun_header;
extern struct ModelFileHeader chrsilverwppk_header;
extern struct ModelFileHeader chrgoldwppk_header;
extern struct ModelFileHeader chrdynamite_header;
extern struct ModelFileHeader chrbungee_header;
extern struct ModelFileHeader chrdoordecoder_header;
extern struct ModelFileHeader chrbombdefuser_header;
extern struct ModelFileHeader chrbugdetector_header;
extern struct ModelFileHeader chrsafecrackercase_header;
extern struct ModelFileHeader chrcamera_header;
extern struct ModelFileHeader chrlockexploder_header;
extern struct ModelFileHeader chrdoorexploder_header;
extern struct ModelFileHeader chrkeyanalysercase_header;
extern struct ModelFileHeader chrweaponcase_header;
extern struct ModelFileHeader chrkeyyale_header;
extern struct ModelFileHeader chrkeybolt_header;
extern struct ModelFileHeader chrbug_header;
extern struct ModelFileHeader chrmicrocamera_header;
extern struct ModelFileHeader floppy_header;
extern struct ModelFileHeader chrgoldeneyekey_header;
extern struct ModelFileHeader chrpolarizedglasses_header;
extern struct ModelFileHeader chrcreditcard_header;
extern struct ModelFileHeader chrdarkglasses_header;
extern struct ModelFileHeader chrgaskeyring_header;
extern struct ModelFileHeader chrdatathief_header;
extern struct ModelFileHeader safe_header;
extern struct ModelFileHeader bomb_header;
extern struct ModelFileHeader chrplans_header;
extern struct ModelFileHeader chrspyfile_header;
extern struct ModelFileHeader chrblueprints_header;
extern struct ModelFileHeader chrcircuitboard_header;
extern struct ModelFileHeader chrmap_header;
extern struct ModelFileHeader chrspooltape_header;
extern struct ModelFileHeader chraudiotape_header;
extern struct ModelFileHeader chrmicrofilm_header;
extern struct ModelFileHeader chrmicrocode_header;
extern struct ModelFileHeader chrlectre_header;
extern struct ModelFileHeader chrmoney_header;
extern struct ModelFileHeader chrgoldbar_header;
extern struct ModelFileHeader chrheroin_header;
extern struct ModelFileHeader chrclipboard_header;
extern struct ModelFileHeader chrdossierred_header;
extern struct ModelFileHeader chrstafflist_header;
extern struct ModelFileHeader chrdattape_header;
extern struct ModelFileHeader chrplastique_header;
extern struct ModelFileHeader chrblackbox_header;
extern struct ModelFileHeader chrvideotape_header;
extern struct ModelFileHeader nintendologo_header;
extern struct ModelFileHeader goldeneyelogo_header;
extern struct ModelFileHeader walletbond_header;
extern struct ModelFileHeader miltruck_header;
extern struct ModelFileHeader jeep_header;
extern struct ModelFileHeader artic_header;
extern struct ModelFileHeader helicopter_header;
extern struct ModelFileHeader tiger_header;
extern struct ModelFileHeader milcopter_header;
extern struct ModelFileHeader hind_header;
extern struct ModelFileHeader artictrailer_header;
extern struct ModelFileHeader motorbike_header;
extern struct ModelFileHeader tank_header;
extern struct ModelFileHeader apc_header;
extern struct ModelFileHeader speedboat_header;
extern struct ModelFileHeader plane_header;
extern struct ModelFileHeader gun_runway1_header;
extern struct ModelFileHeader safedoor_header;
extern struct ModelFileHeader key_holder_header;
extern struct ModelFileHeader hatchsevx_header;
extern struct ModelFileHeader sevdish_header;
extern struct ModelFileHeader archsecdoor1_header;
extern struct ModelFileHeader archsecdoor2_header;
extern struct ModelFileHeader groundgun_header;
extern struct ModelFileHeader trainextdoor_header;
extern struct ModelFileHeader carbmw_header;
extern struct ModelFileHeader carescort_header;
extern struct ModelFileHeader cargolf_header;
extern struct ModelFileHeader carweird_header;
extern struct ModelFileHeader carzil_header;
extern struct ModelFileHeader shuttle_door_l_header;
extern struct ModelFileHeader shuttle_door_r_header;
extern struct ModelFileHeader depot_gate_entry_header;
extern struct ModelFileHeader depot_door_steel_header;
extern struct ModelFileHeader glassware2_header;
extern struct ModelFileHeader glassware3_header;
extern struct ModelFileHeader glassware4_header;
extern struct ModelFileHeader landmine_header;
extern struct ModelFileHeader plant1_header;
extern struct ModelFileHeader plant11_header;
extern struct ModelFileHeader plant2_header;
extern struct ModelFileHeader plant3_header;
extern struct ModelFileHeader jungle5_tree_header;
extern struct ModelFileHeader legalpage_header;
extern struct ModelFileHeader st_pete_room_1i_header;
extern struct ModelFileHeader st_pete_room_2i_header;
extern struct ModelFileHeader st_pete_room_3t_header;
extern struct ModelFileHeader st_pete_room_5c_header;
extern struct ModelFileHeader st_pete_room_6c_header;
extern struct ModelFileHeader door_rollertrain_header;
extern struct ModelFileHeader door_win_header;
extern struct ModelFileHeader door_aztec_header;
extern struct ModelFileHeader shuttle_header;
extern struct ModelFileHeader door_azt_desk_header;
extern struct ModelFileHeader door_azt_desk_top_header;
extern struct ModelFileHeader door_azt_chair_header;
extern struct ModelFileHeader door_mf_header;
extern struct ModelFileHeader flag_header;
extern struct ModelFileHeader barricade_header;
extern struct ModelFileHeader modembox_header;
extern struct ModelFileHeader doorpanel_header;
extern struct ModelFileHeader doorconsole_header;
extern struct ModelFileHeader chrtesttube_header;
extern struct ModelFileHeader bollard_header;

extern struct ChrModelFileRecord c_item_entries[];
extern struct ModelSkeleton skeleton_guard;
extern struct ModelSkeleton skeleton_suit_lf_hand;
# 18 "<stdin>" 2

#pragma region Private Members


static enum lbl
{
    lblZero,
    lblLoop,
    lblDone,
    lblNext,
    lblWaiting,
    lblAlarmActivated,
    lblStoppedMoving,
    lblSeesBond,
    lblKneel = 11,
    lbl12,
    lblBuddyShot,
    lblCloneContinue,
    lblRunToObjective,
    lblNearMiss,
    lblDead,

    lblRunning = 27,
    lblResetAndRunAgain,
    lblAbleToRun,
    lblSitRep,
    lblCheckDistance = 36,
    lblRun = 40,
    lblMaybeThrowGrenade = 43,
    lblCheckDistance2 = 45,
    lbl46,
    lblInvincibleCheck
};
# 61 "<stdin>"
u8 m_AimAtBond[] = {
    AI_TRYFireOrAimAtTarget , ((0x0001 | 0x0020) & 0xFF00) >> 8, (0x0001 | 0x0020) & 0x00FF , ((0) & 0xFF00) >> 8, (0) & 0x00FF , lblLoop ,
    AI_Label , lblLoop , AI_Yield , AI_GotoFirst , lblLoop ,
    AI_EndList
};







u8 m_DeadAI[] = {

    AI_Label , lblDead , AI_Yield , AI_GotoFirst , lblDead ,
    AI_EndList
};
# 88 "<stdin>"
u8 m_StandardGuard[] = {







    AI_Label , lblLoop , AI_Yield ,
        AI_IFChrDyingOrDead , -3 , lblDead ,
        AI_IFImOnPatrolOrStopped , lblStoppedMoving ,

        AI_GotoFirst , lblLoop ,


        AI_Label , lblStoppedMoving ,

            AI_IFISeeBond , lblSeesBond ,
            AI_IFIWasShotRecently , lblBuddyShot ,
            AI_IFIHeardBondRecently , lblCloneContinue ,
            AI_IFBondMissedMe , lblNearMiss ,
            AI_IFISeeSomeoneShot , lblBuddyShot ,
            AI_IFISeeSomeoneDie , lblBuddyShot ,
            AI_IFPlayingAnimation , lblNext ,
            AI_SetNewRandom ,AI_IFRandomGreaterThan , 1 , lblNext ,

            IF_ELSE ( 0 ) ( ( ((('E','R','R','O','R',':',' ','C','a','n','n','o','t',' ','C','a','l','l',' ','A','I','_','L','I','S','T',' ','w','i','t','h','o','u','t',' ','d','e','f','i','n','i','n','g',' ','T','H','I','S',' ','f','o','r',' ','r','e','t','u','r','n'),1)) , ) ) ( ( AI_SetReturnAiList , ((GAILIST_STANDARD_GUARD) & 0xFF00) >> 8, (GAILIST_STANDARD_GUARD) & 0x00FF ,AI_SetChrAiList , -3 , (((!((GAILIST_PLAY_IDLE_ANIMATION) >= 4096) && ((GAILIST_PLAY_IDLE_ANIMATION == GAILIST_PLAY_IDLE_ANIMATION) | (GAILIST_PLAY_IDLE_ANIMATION == GAILIST_BASH_KEYBOARD) | (GAILIST_PLAY_IDLE_ANIMATION == GAILIST_ATTACK_BOND) | (GAILIST_PLAY_IDLE_ANIMATION == GAILIST_RUN_TO_BOND) | (GAILIST_PLAY_IDLE_ANIMATION == GAILIST_STARTLE_AND_RUN_TO_BOND) | (GAILIST_PLAY_IDLE_ANIMATION == GAILIST_WAIT_ONE_SECOND) IF(0) ( | SETUPSUBROUTINES(GAILIST_PLAY_IDLE_ANIMATION) )) ? GAILIST_PLAY_IDLE_ANIMATION : (('E','R','R','O','R',':',' ','A','I','_','L','I','S','T',' ','N','O','T',' ','A',' ','S','U','B','R','O','U','T','I','N','E',' ','O','R',' ','N','O','T',' ','F','O','R',' ','C','H','R'), 1))) & 0xFF00) >> 8, ((!((GAILIST_PLAY_IDLE_ANIMATION) >= 4096) && ((GAILIST_PLAY_IDLE_ANIMATION == GAILIST_PLAY_IDLE_ANIMATION) | (GAILIST_PLAY_IDLE_ANIMATION == GAILIST_BASH_KEYBOARD) | (GAILIST_PLAY_IDLE_ANIMATION == GAILIST_ATTACK_BOND) | (GAILIST_PLAY_IDLE_ANIMATION == GAILIST_RUN_TO_BOND) | (GAILIST_PLAY_IDLE_ANIMATION == GAILIST_STARTLE_AND_RUN_TO_BOND) | (GAILIST_PLAY_IDLE_ANIMATION == GAILIST_WAIT_ONE_SECOND) IF(0) ( | SETUPSUBROUTINES(GAILIST_PLAY_IDLE_ANIMATION) )) ? GAILIST_PLAY_IDLE_ANIMATION : (('E','R','R','O','R',':',' ','A','I','_','L','I','S','T',' ','N','O','T',' ','A',' ','S','U','B','R','O','U','T','I','N','E',' ','O','R',' ','N','O','T',' ','F','O','R',' ','C','H','R'), 1))) & 0x00FF , ) )

        AI_Label , lblNext ,

    AI_GotoFirst , lblLoop ,


    AI_Label , lblNearMiss ,
        AI_SetNewRandom ,AI_IFRandomGreaterThan , 127 , lblSeesBond ,
        IF_ELSE ( 0 ) ( ( ((('E','R','R','O','R',':',' ','C','a','n','n','o','t',' ','C','a','l','l',' ','A','I','_','L','I','S','T',' ','w','i','t','h','o','u','t',' ','d','e','f','i','n','i','n','g',' ','T','H','I','S',' ','f','o','r',' ','r','e','t','u','r','n'),1)) , ) ) ( ( AI_SetReturnAiList , ((GAILIST_STANDARD_GUARD) & 0xFF00) >> 8, (GAILIST_STANDARD_GUARD) & 0x00FF ,AI_SetChrAiList , -3 , (((!((GAILIST_STARTLE_AND_RUN_TO_BOND) >= 4096) && ((GAILIST_STARTLE_AND_RUN_TO_BOND == GAILIST_PLAY_IDLE_ANIMATION) | (GAILIST_STARTLE_AND_RUN_TO_BOND == GAILIST_BASH_KEYBOARD) | (GAILIST_STARTLE_AND_RUN_TO_BOND == GAILIST_ATTACK_BOND) | (GAILIST_STARTLE_AND_RUN_TO_BOND == GAILIST_RUN_TO_BOND) | (GAILIST_STARTLE_AND_RUN_TO_BOND == GAILIST_STARTLE_AND_RUN_TO_BOND) | (GAILIST_STARTLE_AND_RUN_TO_BOND == GAILIST_WAIT_ONE_SECOND) IF(0) ( | SETUPSUBROUTINES(GAILIST_STARTLE_AND_RUN_TO_BOND) )) ? GAILIST_STARTLE_AND_RUN_TO_BOND : (('E','R','R','O','R',':',' ','A','I','_','L','I','S','T',' ','N','O','T',' ','A',' ','S','U','B','R','O','U','T','I','N','E',' ','O','R',' ','N','O','T',' ','F','O','R',' ','C','H','R'), 1))) & 0xFF00) >> 8, ((!((GAILIST_STARTLE_AND_RUN_TO_BOND) >= 4096) && ((GAILIST_STARTLE_AND_RUN_TO_BOND == GAILIST_PLAY_IDLE_ANIMATION) | (GAILIST_STARTLE_AND_RUN_TO_BOND == GAILIST_BASH_KEYBOARD) | (GAILIST_STARTLE_AND_RUN_TO_BOND == GAILIST_ATTACK_BOND) | (GAILIST_STARTLE_AND_RUN_TO_BOND == GAILIST_RUN_TO_BOND) | (GAILIST_STARTLE_AND_RUN_TO_BOND == GAILIST_STARTLE_AND_RUN_TO_BOND) | (GAILIST_STARTLE_AND_RUN_TO_BOND == GAILIST_WAIT_ONE_SECOND) IF(0) ( | SETUPSUBROUTINES(GAILIST_STARTLE_AND_RUN_TO_BOND) )) ? GAILIST_STARTLE_AND_RUN_TO_BOND : (('E','R','R','O','R',':',' ','A','I','_','L','I','S','T',' ','N','O','T',' ','A',' ','S','U','B','R','O','U','T','I','N','E',' ','O','R',' ','N','O','T',' ','F','O','R',' ','C','H','R'), 1))) & 0x00FF , ) )

    AI_Label , lblSeesBond ,
        IF_ELSE ( 0 ) ( ( ((('E','R','R','O','R',':',' ','C','a','n','n','o','t',' ','C','a','l','l',' ','A','I','_','L','I','S','T',' ','w','i','t','h','o','u','t',' ','d','e','f','i','n','i','n','g',' ','T','H','I','S',' ','f','o','r',' ','r','e','t','u','r','n'),1)) , ) ) ( ( AI_SetReturnAiList , ((GAILIST_STANDARD_GUARD) & 0xFF00) >> 8, (GAILIST_STANDARD_GUARD) & 0x00FF ,AI_SetChrAiList , -3 , (((!((GAILIST_ATTACK_BOND) >= 4096) && ((GAILIST_ATTACK_BOND == GAILIST_PLAY_IDLE_ANIMATION) | (GAILIST_ATTACK_BOND == GAILIST_BASH_KEYBOARD) | (GAILIST_ATTACK_BOND == GAILIST_ATTACK_BOND) | (GAILIST_ATTACK_BOND == GAILIST_RUN_TO_BOND) | (GAILIST_ATTACK_BOND == GAILIST_STARTLE_AND_RUN_TO_BOND) | (GAILIST_ATTACK_BOND == GAILIST_WAIT_ONE_SECOND) IF(0) ( | SETUPSUBROUTINES(GAILIST_ATTACK_BOND) )) ? GAILIST_ATTACK_BOND : (('E','R','R','O','R',':',' ','A','I','_','L','I','S','T',' ','N','O','T',' ','A',' ','S','U','B','R','O','U','T','I','N','E',' ','O','R',' ','N','O','T',' ','F','O','R',' ','C','H','R'), 1))) & 0xFF00) >> 8, ((!((GAILIST_ATTACK_BOND) >= 4096) && ((GAILIST_ATTACK_BOND == GAILIST_PLAY_IDLE_ANIMATION) | (GAILIST_ATTACK_BOND == GAILIST_BASH_KEYBOARD) | (GAILIST_ATTACK_BOND == GAILIST_ATTACK_BOND) | (GAILIST_ATTACK_BOND == GAILIST_RUN_TO_BOND) | (GAILIST_ATTACK_BOND == GAILIST_STARTLE_AND_RUN_TO_BOND) | (GAILIST_ATTACK_BOND == GAILIST_WAIT_ONE_SECOND) IF(0) ( | SETUPSUBROUTINES(GAILIST_ATTACK_BOND) )) ? GAILIST_ATTACK_BOND : (('E','R','R','O','R',':',' ','A','I','_','L','I','S','T',' ','N','O','T',' ','A',' ','S','U','B','R','O','U','T','I','N','E',' ','O','R',' ','N','O','T',' ','F','O','R',' ','C','H','R'), 1))) & 0x00FF , ) )

    AI_Label , lblBuddyShot ,
        IF_ELSE ( 0 ) ( ( ((('E','R','R','O','R',':',' ','C','a','n','n','o','t',' ','C','a','l','l',' ','A','I','_','L','I','S','T',' ','w','i','t','h','o','u','t',' ','d','e','f','i','n','i','n','g',' ','T','H','I','S',' ','f','o','r',' ','r','e','t','u','r','n'),1)) , ) ) ( ( AI_SetReturnAiList , ((GAILIST_STANDARD_GUARD) & 0xFF00) >> 8, (GAILIST_STANDARD_GUARD) & 0x00FF ,AI_SetChrAiList , -3 , (((!((GAILIST_RUN_TO_BOND) >= 4096) && ((GAILIST_RUN_TO_BOND == GAILIST_PLAY_IDLE_ANIMATION) | (GAILIST_RUN_TO_BOND == GAILIST_BASH_KEYBOARD) | (GAILIST_RUN_TO_BOND == GAILIST_ATTACK_BOND) | (GAILIST_RUN_TO_BOND == GAILIST_RUN_TO_BOND) | (GAILIST_RUN_TO_BOND == GAILIST_STARTLE_AND_RUN_TO_BOND) | (GAILIST_RUN_TO_BOND == GAILIST_WAIT_ONE_SECOND) IF(0) ( | SETUPSUBROUTINES(GAILIST_RUN_TO_BOND) )) ? GAILIST_RUN_TO_BOND : (('E','R','R','O','R',':',' ','A','I','_','L','I','S','T',' ','N','O','T',' ','A',' ','S','U','B','R','O','U','T','I','N','E',' ','O','R',' ','N','O','T',' ','F','O','R',' ','C','H','R'), 1))) & 0xFF00) >> 8, ((!((GAILIST_RUN_TO_BOND) >= 4096) && ((GAILIST_RUN_TO_BOND == GAILIST_PLAY_IDLE_ANIMATION) | (GAILIST_RUN_TO_BOND == GAILIST_BASH_KEYBOARD) | (GAILIST_RUN_TO_BOND == GAILIST_ATTACK_BOND) | (GAILIST_RUN_TO_BOND == GAILIST_RUN_TO_BOND) | (GAILIST_RUN_TO_BOND == GAILIST_STARTLE_AND_RUN_TO_BOND) | (GAILIST_RUN_TO_BOND == GAILIST_WAIT_ONE_SECOND) IF(0) ( | SETUPSUBROUTINES(GAILIST_RUN_TO_BOND) )) ? GAILIST_RUN_TO_BOND : (('E','R','R','O','R',':',' ','A','I','_','L','I','S','T',' ','N','O','T',' ','A',' ','S','U','B','R','O','U','T','I','N','E',' ','O','R',' ','N','O','T',' ','F','O','R',' ','C','H','R'), 1))) & 0x00FF , ) )

    AI_Label , lblCloneContinue ,
        AI_SetReturnAiList , ((GAILIST_STANDARD_GUARD) & 0xFF00) >> 8, (GAILIST_STANDARD_GUARD) & 0x00FF ,
        AI_SetChrAiList , -3 , (((!((GAILIST_TRY_CLONE_SEND_OR_RUN_TO_BOND) >= 4096) ? GAILIST_TRY_CLONE_SEND_OR_RUN_TO_BOND : (('E','R','R','O','R',':',' ','A','I','_','L','I','S','T',' ','I','S',' ','A',' ','S','U','B','R','O','U','T','I','N','E',' ','O','R',' ','N','O','T',' ','F','O','R',' ','C','H','R'), 1))) & 0xFF00) >> 8, ((!((GAILIST_TRY_CLONE_SEND_OR_RUN_TO_BOND) >= 4096) ? GAILIST_TRY_CLONE_SEND_OR_RUN_TO_BOND : (('E','R','R','O','R',':',' ','A','I','_','L','I','S','T',' ','I','S',' ','A',' ','S','U','B','R','O','U','T','I','N','E',' ','O','R',' ','N','O','T',' ','F','O','R',' ','C','H','R'), 1))) & 0x00FF ,

    AI_Label , lblDead ,

        AI_SetChrAiList , -3 , (((!((GAILIST_DEAD_AI) >= 4096) ? GAILIST_DEAD_AI : (('E','R','R','O','R',':',' ','A','I','_','L','I','S','T',' ','I','S',' ','A',' ','S','U','B','R','O','U','T','I','N','E',' ','O','R',' ','N','O','T',' ','F','O','R',' ','C','H','R'), 1))) & 0xFF00) >> 8, ((!((GAILIST_DEAD_AI) >= 4096) ? GAILIST_DEAD_AI : (('E','R','R','O','R',':',' ','A','I','_','L','I','S','T',' ','I','S',' ','A',' ','S','U','B','R','O','U','T','I','N','E',' ','O','R',' ','N','O','T',' ','F','O','R',' ','C','H','R'), 1))) & 0x00FF ,
    AI_EndList



};
# 151 "<stdin>"
u8 m_IdleAnimations[] = {
# 183 "<stdin>"
    AI_SetNewRandom 0}; Error: Switch Limited to 15 elements + 1 default u8 (183) & 0x00FF[] = {((0) & 0xFF00) >> 8 ((0) & 0x00FF,lblNext) 0x10(lblDone)AI_Label , lblNext , AI_PlayAnimation (((ANIM_scratching_leg) & 0xFF00) >> 8,lblNext) ((183) & 0xFF00) >> 8(lblDone)AI_Label , lblNext , AI_GotoNext , ANIM_scratching_leg , & 0x00FF(lblDone)AI_Label , lblNext , (123) & 0x00FF (0x10 | 0x08,lblNext) 100(lblDone)AI_Label , lblNext , ((0) & 0xFF00) >> 8 ((0) & 0x00FF,lblNext) 0x10(lblDone)AI_Label , lblNext , AI_PlayAnimation (((ANIM_scratching_butt) & 0xFF00) >> 8,lblNext) ((123) & 0xFF00) >> 8(lblDone)AI_Label , lblNext , AI_GotoNext , ANIM_scratching_butt , & 0x00FF(lblDone)AI_Label , lblNext , (56) & 0x00FF (0x10 | 0x08,lblNext) 150(lblDone)AI_Label , lblNext , ((0) & 0xFF00) >> 8 ((0) & 0x00FF,lblNext) 0x10(lblDone)AI_Label , lblNext , AI_PlayAnimation (((ANIM_adjusting_crotch) & 0xFF00) >> 8,lblNext) ((56) & 0xFF00) >> 8(lblDone)AI_Label , lblNext , AI_GotoNext , ANIM_adjusting_crotch , & 0x00FF(lblDone)AI_Label , lblNext , (137) & 0x00FF (0x10 | 0x08,lblNext) 200(lblDone)AI_Label , lblNext , ((0) & 0xFF00) >> 8 ((0) & 0x00FF,lblNext) 0x10(lblDone)AI_Label , lblNext , AI_PlayAnimation (((ANIM_sneeze) & 0xFF00) >> 8,lblNext) ((137) & 0xFF00) >> 8(lblDone)AI_Label , lblNext , (IFRandomGreaterThan,lblNext) (ANIM_sneeze) & 0x00FF(lblDone)
# 204 "<stdin>"
    AI_Return ,
    AI_EndList
};
# 215 "<stdin>"
u8 m_BashKeyboard[] = {
# 236 "<stdin>"
    AI_SetNewRandom 0}; Error: Switch Limited to 15 elements + 1 default u8 (69) & 0x00FF[] = {((0) & 0xFF00) >> 8 ((0) & 0x00FF,lblNext) 0x10(lblDone)AI_Label , lblNext , AI_PlayAnimation (((ANIM_keyboard_right_hand1) & 0xFF00) >> 8,lblNext) ((69) & 0xFF00) >> 8(lblDone)AI_Label , lblNext , AI_GotoNext , ANIM_keyboard_right_hand1 , & 0x00FF(lblDone)AI_Label , lblNext , (74) & 0x00FF (0x00,lblNext) (lblDone)AI_Label , lblNext , ((0) & 0xFF00) >> 8 ((0) & 0x00FF,lblNext) 0x10(lblDone)AI_Label , lblNext , AI_PlayAnimation (((ANIM_keyboard_right_hand2) & 0xFF00) >> 8,lblNext) ((74) & 0xFF00) >> 8(lblDone)AI_Label , lblNext , AI_GotoNext , ANIM_keyboard_right_hand2 , & 0x00FF(lblDone)AI_Label , lblNext , (79) & 0x00FF (0x00,lblNext) 60(lblDone)AI_Label , lblNext , ((0) & 0xFF00) >> 8 ((0) & 0x00FF,lblNext) 0x10(lblDone)AI_Label , lblNext , AI_PlayAnimation (((ANIM_keyboard_left_hand) & 0xFF00) >> 8,lblNext) ((79) & 0xFF00) >> 8(lblDone)AI_Label , lblNext , AI_GotoNext , ANIM_keyboard_left_hand , & 0x00FF(lblDone)AI_Label , lblNext , (89) & 0x00FF (0x00,lblNext) 120(lblDone)AI_Label , lblNext , ((0) & 0xFF00) >> 8 ((0) & 0x00FF,lblNext) 0x10(lblDone)AI_Label , lblNext , AI_PlayAnimation (((ANIM_keyboard_right_hand_tapping) & 0xFF00) >> 8,lblNext) ((89) & 0xFF00) >> 8(lblDone)AI_Label , lblNext , (IFRandomGreaterThan,lblNext) (ANIM_keyboard_right_hand_tapping) & 0x00FF(lblDone)
# 251 "<stdin>"
    AI_Return ,
    AI_EndList};







u8 m_SimpleGuardDeaf[] = {
    AI_Label , lblLoop , AI_Yield ,
        AI_IFImOnPatrolOrStopped , lblStoppedMoving ,

        AI_GotoFirst , lblLoop ,

        AI_Label , lblStoppedMoving ,
            AI_IFISeeBond , lblSeesBond ,
            AI_IFIWasShotRecently , lblBuddyShot ,
            AI_IFBondMissedMe , lblNearMiss ,
            AI_IFISeeSomeoneShot , lblBuddyShot ,
            AI_IFISeeSomeoneDie , lblBuddyShot ,
    AI_GotoFirst , lblLoop ,


    AI_Label , lblNearMiss ,
        AI_SetNewRandom ,AI_IFRandomGreaterThan , 127 , lblSeesBond ,
        AI_SetReturnAiList , ((GAILIST_STANDARD_GUARD) & 0xFF00) >> 8, (GAILIST_STANDARD_GUARD) & 0x00FF ,AI_SetChrAiList , -3 , (((!((GAILIST_STARTLE_AND_RUN_TO_BOND) >= 4096) ? GAILIST_STARTLE_AND_RUN_TO_BOND : (('E','R','R','O','R',':',' ','A','I','_','L','I','S','T',' ','I','S',' ','A',' ','S','U','B','R','O','U','T','I','N','E',' ','O','R',' ','N','O','T',' ','F','O','R',' ','C','H','R'), 1))) & 0xFF00) >> 8, ((!((GAILIST_STARTLE_AND_RUN_TO_BOND) >= 4096) ? GAILIST_STARTLE_AND_RUN_TO_BOND : (('E','R','R','O','R',':',' ','A','I','_','L','I','S','T',' ','I','S',' ','A',' ','S','U','B','R','O','U','T','I','N','E',' ','O','R',' ','N','O','T',' ','F','O','R',' ','C','H','R'), 1))) & 0x00FF ,

    AI_Label , lblSeesBond ,
        AI_SetReturnAiList , ((GAILIST_STANDARD_GUARD) & 0xFF00) >> 8, (GAILIST_STANDARD_GUARD) & 0x00FF ,AI_SetChrAiList , -3 , (((!((GAILIST_ATTACK_BOND) >= 4096) ? GAILIST_ATTACK_BOND : (('E','R','R','O','R',':',' ','A','I','_','L','I','S','T',' ','I','S',' ','A',' ','S','U','B','R','O','U','T','I','N','E',' ','O','R',' ','N','O','T',' ','F','O','R',' ','C','H','R'), 1))) & 0xFF00) >> 8, ((!((GAILIST_ATTACK_BOND) >= 4096) ? GAILIST_ATTACK_BOND : (('E','R','R','O','R',':',' ','A','I','_','L','I','S','T',' ','I','S',' ','A',' ','S','U','B','R','O','U','T','I','N','E',' ','O','R',' ','N','O','T',' ','F','O','R',' ','C','H','R'), 1))) & 0x00FF ,

    AI_Label , lblBuddyShot ,
        AI_SetReturnAiList , ((GAILIST_STANDARD_GUARD) & 0xFF00) >> 8, (GAILIST_STANDARD_GUARD) & 0x00FF ,AI_SetChrAiList , -3 , (((!((GAILIST_RUN_TO_BOND) >= 4096) ? GAILIST_RUN_TO_BOND : (('E','R','R','O','R',':',' ','A','I','_','L','I','S','T',' ','I','S',' ','A',' ','S','U','B','R','O','U','T','I','N','E',' ','O','R',' ','N','O','T',' ','F','O','R',' ','C','H','R'), 1))) & 0xFF00) >> 8, ((!((GAILIST_RUN_TO_BOND) >= 4096) ? GAILIST_RUN_TO_BOND : (('E','R','R','O','R',':',' ','A','I','_','L','I','S','T',' ','I','S',' ','A',' ','S','U','B','R','O','U','T','I','N','E',' ','O','R',' ','N','O','T',' ','F','O','R',' ','C','H','R'), 1))) & 0x00FF ,
    AI_EndList
};
# 294 "<stdin>"
u8 m_AttackBond[] = {
# 327 "<stdin>"
    AI_IFMyFlags2Has , FLAGS2_DONT_POINT_AT_BOND , lblNext ,
    AI_SetNewRandom ,AI_IFRandomGreaterThan , 32 , lblNext ,
    AI_PointAtBond ,
    AI_SetMyFlags2 , FLAGS2_DONT_POINT_AT_BOND ,
    AI_Return ,

    AI_Label , lblNext ,
        AI_SetMyFlags2 , FLAGS2_DONT_POINT_AT_BOND ,
        AI_TRYThrowingGrenade , lblDone ,

    AI_Label , lblNext ,
        AI_SetNewRandom ,AI_IFRandomGreaterThan , 10 , lblNext ,
        AI_TRYFiringRoll , lblDone ,

    AI_Label , lblNext ,
        AI_SetNewRandom ,AI_IFRandomGreaterThan , 10 , lblNext ,
        AI_TRYSideHopping , lblDone ,

    AI_Label , lblNext ,
        AI_SetNewRandom ,AI_IFRandomGreaterThan , 25 , lblNext ,
        AI_TRYSidestepping , lblDone ,

    AI_Label , lblNext ,
        AI_SetNewRandom ,AI_IFRandomGreaterThan , 50 , lblNext ,
        AI_TRYSideRunning , lblDone ,

    AI_Label , lblNext ,
        AI_SetNewRandom ,AI_IFRandomGreaterThan , 64 , lblNext ,
        AI_TRYFiringWalk , lblDone ,

    AI_Label , lblNext ,
        AI_IFRandomGreaterThan , 127 , lblNext ,
        AI_TRYFiringRun , lblDone ,

    AI_Label , lblNext ,
        AI_SetNewRandom ,AI_IFRandomGreaterThan , 127 , lblKneel ,
        AI_TRYFireOrAimAtTarget , ((0x0001) & 0xFF00) >> 8, (0x0001) & 0x00FF , ((0) & 0xFF00) >> 8, (0) & 0x00FF , lblDone ,

    AI_Label , lblKneel ,
        AI_TRYFireOrAimAtTargetKneel , ((0x0001) & 0xFF00) >> 8, (0x0001) & 0x00FF , ((0) & 0xFF00) >> 8, (0) & 0x00FF , lblDone ,

    AI_Label , lblDone ,


    AI_Return ,
    AI_EndList
};
# 382 "<stdin>"
u8 m_RunToBond[] = {
    AI_SetMyFlags2 , FLAGS2_DONT_POINT_AT_BOND ,
    AI_TRYRunToBond , lblLoop ,
    AI_Return ,

    AI_Label , lblLoop , AI_Yield ,
        AI_IFImOnPatrolOrStopped , lblStoppedMoving ,
        AI_IFISeeBond , lblSeesBond ,
    AI_GotoFirst , lblLoop ,

    AI_Label , lblSeesBond ,
        AI_SetChrAiList , -3 , (((!((GAILIST_ATTACK_BOND) >= 4096) ? GAILIST_ATTACK_BOND : (('E','R','R','O','R',':',' ','A','I','_','L','I','S','T',' ','I','S',' ','A',' ','S','U','B','R','O','U','T','I','N','E',' ','O','R',' ','N','O','T',' ','F','O','R',' ','C','H','R'), 1))) & 0xFF00) >> 8, ((!((GAILIST_ATTACK_BOND) >= 4096) ? GAILIST_ATTACK_BOND : (('E','R','R','O','R',':',' ','A','I','_','L','I','S','T',' ','I','S',' ','A',' ','S','U','B','R','O','U','T','I','N','E',' ','O','R',' ','N','O','T',' ','F','O','R',' ','C','H','R'), 1))) & 0x00FF ,

    AI_Label , lblStoppedMoving ,
        AI_Return ,
        AI_EndList
};







u8 m_TryCloneSendOrRunToBond[] = {
    AI_SetMyFlags2 , FLAGS2_DONT_POINT_AT_BOND ,
    AI_IFIveNotBeenSeen , lblCloneContinue ,
    AI_SetChrAiList , -3 , (((!((GAILIST_RUN_TO_BOND) >= 4096) ? GAILIST_RUN_TO_BOND : (('E','R','R','O','R',':',' ','A','I','_','L','I','S','T',' ','I','S',' ','A',' ','S','U','B','R','O','U','T','I','N','E',' ','O','R',' ','N','O','T',' ','F','O','R',' ','C','H','R'), 1))) & 0xFF00) >> 8, ((!((GAILIST_RUN_TO_BOND) >= 4096) ? GAILIST_RUN_TO_BOND : (('E','R','R','O','R',':',' ','A','I','_','L','I','S','T',' ','I','S',' ','A',' ','S','U','B','R','O','U','T','I','N','E',' ','O','R',' ','N','O','T',' ','F','O','R',' ','C','H','R'), 1))) & 0x00FF ,

    AI_Label , lblCloneContinue ,
        AI_IFChrDoesNotExist , -7 , lblCloneContinue ,
        AI_SetChrAiList , -3 , (((!((GAILIST_STANDARD_GUARD) >= 4096) ? GAILIST_STANDARD_GUARD : (('E','R','R','O','R',':',' ','A','I','_','L','I','S','T',' ','I','S',' ','A',' ','S','U','B','R','O','U','T','I','N','E',' ','O','R',' ','N','O','T',' ','F','O','R',' ','C','H','R'), 1))) & 0xFF00) >> 8, ((!((GAILIST_STANDARD_GUARD) >= 4096) ? GAILIST_STANDARD_GUARD : (('E','R','R','O','R',':',' ','A','I','_','L','I','S','T',' ','I','S',' ','A',' ','S','U','B','R','O','U','T','I','N','E',' ','O','R',' ','N','O','T',' ','F','O','R',' ','C','H','R'), 1))) & 0x00FF ,

    AI_Label , lblCloneContinue ,
        AI_TRYCloningChr , -3 , ((GAILIST_STANDARD_CLONE) & 0xFF00) >> 8, (GAILIST_STANDARD_CLONE) & 0x00FF , lblNext ,
        AI_SetChrAiList , -3 , (((!((GAILIST_RUN_TO_BOND) >= 4096) ? GAILIST_RUN_TO_BOND : (('E','R','R','O','R',':',' ','A','I','_','L','I','S','T',' ','I','S',' ','A',' ','S','U','B','R','O','U','T','I','N','E',' ','O','R',' ','N','O','T',' ','F','O','R',' ','C','H','R'), 1))) & 0xFF00) >> 8, ((!((GAILIST_RUN_TO_BOND) >= 4096) ? GAILIST_RUN_TO_BOND : (('E','R','R','O','R',':',' ','A','I','_','L','I','S','T',' ','I','S',' ','A',' ','S','U','B','R','O','U','T','I','N','E',' ','O','R',' ','N','O','T',' ','F','O','R',' ','C','H','R'), 1))) & 0x00FF ,


    AI_Label , lblNext ,
        AI_SetChrAiList , -3 , (((!((GAILIST_STANDARD_GUARD) >= 4096) ? GAILIST_STANDARD_GUARD : (('E','R','R','O','R',':',' ','A','I','_','L','I','S','T',' ','I','S',' ','A',' ','S','U','B','R','O','U','T','I','N','E',' ','O','R',' ','N','O','T',' ','F','O','R',' ','C','H','R'), 1))) & 0xFF00) >> 8, ((!((GAILIST_STANDARD_GUARD) >= 4096) ? GAILIST_STANDARD_GUARD : (('E','R','R','O','R',':',' ','A','I','_','L','I','S','T',' ','I','S',' ','A',' ','S','U','B','R','O','U','T','I','N','E',' ','O','R',' ','N','O','T',' ','F','O','R',' ','C','H','R'), 1))) & 0x00FF ,

    AI_EndList
};






u8 m_StandardClone[] = {
    AI_SetReturnAiList , ((GAILIST_STANDARD_GUARD) & 0xFF00) >> 8, (GAILIST_STANDARD_GUARD) & 0x00FF ,AI_SetChrAiList , -3 , (((!((GAILIST_RUN_TO_BOND) >= 4096) ? GAILIST_RUN_TO_BOND : (('E','R','R','O','R',':',' ','A','I','_','L','I','S','T',' ','I','S',' ','A',' ','S','U','B','R','O','U','T','I','N','E',' ','O','R',' ','N','O','T',' ','F','O','R',' ','C','H','R'), 1))) & 0xFF00) >> 8, ((!((GAILIST_RUN_TO_BOND) >= 4096) ? GAILIST_RUN_TO_BOND : (('E','R','R','O','R',':',' ','A','I','_','L','I','S','T',' ','I','S',' ','A',' ','S','U','B','R','O','U','T','I','N','E',' ','O','R',' ','N','O','T',' ','F','O','R',' ','C','H','R'), 1))) & 0x00FF ,
    AI_EndList
};







u8 m_SimpleGuard[] = {

    AI_Label , lblLoop , AI_Yield ,
        AI_IFISeeBond , lblSeesBond ,
        AI_IFIWasShotRecently , lblBuddyShot ,
        AI_IFIHeardBondRecently , lblBuddyShot ,
        AI_IFBondMissedMe , lblBuddyShot ,
        AI_IFISeeSomeoneShot , lblBuddyShot ,
        AI_IFISeeSomeoneDie , lblBuddyShot ,
    AI_GotoFirst , lblLoop ,

    AI_Label , lblSeesBond ,
        AI_SetReturnAiList , ((GAILIST_STANDARD_GUARD) & 0xFF00) >> 8, (GAILIST_STANDARD_GUARD) & 0x00FF ,AI_SetChrAiList , -3 , (((!((GAILIST_ATTACK_BOND) >= 4096) ? GAILIST_ATTACK_BOND : (('E','R','R','O','R',':',' ','A','I','_','L','I','S','T',' ','I','S',' ','A',' ','S','U','B','R','O','U','T','I','N','E',' ','O','R',' ','N','O','T',' ','F','O','R',' ','C','H','R'), 1))) & 0xFF00) >> 8, ((!((GAILIST_ATTACK_BOND) >= 4096) ? GAILIST_ATTACK_BOND : (('E','R','R','O','R',':',' ','A','I','_','L','I','S','T',' ','I','S',' ','A',' ','S','U','B','R','O','U','T','I','N','E',' ','O','R',' ','N','O','T',' ','F','O','R',' ','C','H','R'), 1))) & 0x00FF ,

    AI_Label , lblBuddyShot ,
        AI_SetReturnAiList , ((GAILIST_STANDARD_GUARD) & 0xFF00) >> 8, (GAILIST_STANDARD_GUARD) & 0x00FF ,AI_SetChrAiList , -3 , (((!((GAILIST_RUN_TO_BOND) >= 4096) ? GAILIST_RUN_TO_BOND : (('E','R','R','O','R',':',' ','A','I','_','L','I','S','T',' ','I','S',' ','A',' ','S','U','B','R','O','U','T','I','N','E',' ','O','R',' ','N','O','T',' ','F','O','R',' ','C','H','R'), 1))) & 0xFF00) >> 8, ((!((GAILIST_RUN_TO_BOND) >= 4096) ? GAILIST_RUN_TO_BOND : (('E','R','R','O','R',':',' ','A','I','_','L','I','S','T',' ','I','S',' ','A',' ','S','U','B','R','O','U','T','I','N','E',' ','O','R',' ','N','O','T',' ','F','O','R',' ','C','H','R'), 1))) & 0x00FF ,

    AI_Label , lblCloneContinue ,
        AI_SetReturnAiList , ((GAILIST_STANDARD_GUARD) & 0xFF00) >> 8, (GAILIST_STANDARD_GUARD) & 0x00FF ,AI_SetChrAiList , -3 , (((!((GAILIST_TRY_CLONE_SEND_OR_RUN_TO_BOND) >= 4096) ? GAILIST_TRY_CLONE_SEND_OR_RUN_TO_BOND : (('E','R','R','O','R',':',' ','A','I','_','L','I','S','T',' ','I','S',' ','A',' ','S','U','B','R','O','U','T','I','N','E',' ','O','R',' ','N','O','T',' ','F','O','R',' ','C','H','R'), 1))) & 0xFF00) >> 8, ((!((GAILIST_TRY_CLONE_SEND_OR_RUN_TO_BOND) >= 4096) ? GAILIST_TRY_CLONE_SEND_OR_RUN_TO_BOND : (('E','R','R','O','R',':',' ','A','I','_','L','I','S','T',' ','I','S',' ','A',' ','S','U','B','R','O','U','T','I','N','E',' ','O','R',' ','N','O','T',' ','F','O','R',' ','C','H','R'), 1))) & 0x00FF ,
    AI_EndList
};
# 471 "<stdin>"
u8 m_SimpleGuardAlarmRaiser[] = {

    AI_Label , lblLoop , AI_Yield ,
        AI_IFImOnPatrolOrStopped , lblStoppedMoving ,

        AI_GotoFirst , lblLoop ,



        AI_Label , lblStoppedMoving ,
        AI_IFISeeBond , lblRunToObjective ,
        AI_IFMyNumCloseArghsGreaterThan , 0 , lblNearMiss ,
        AI_IFIHeardBondRecently , lblNearMiss ,
        AI_IFISeeSomeoneShot , lblNearMiss ,
        AI_IFISeeSomeoneDie , lblNearMiss ,
    AI_GotoFirst , lblLoop ,




    AI_Label , lblNearMiss ,
    AI_IFMyNumArghsGreaterThan , 0 , lblRunToObjective ,
    AI_LookSurprised ,

    AI_Label , lblWaiting , AI_Yield ,
        AI_IFImOnPatrolOrStopped , lblRunToObjective ,
    AI_GotoFirst , lblWaiting ,




    AI_Label , lblRunToObjective ,
    AI_RunToPad , ((9000) & 0xFF00) >> 8, (9000) & 0x00FF ,

    AI_Label , lblNext , AI_Yield ,
        AI_IFImOnPatrolOrStopped , lblDone ,
    AI_GotoFirst , lblNext ,




    AI_Label , lblDone ,
    AI_IFChrDistanceToPadGreaterThanDecimeter , -3 , (((u16)(1 * 10)) & 0xFF00) >> 8, ((u16)(1 * 10)) & 0x00FF , ((9000) & 0xFF00) >> 8, (9000) & 0x00FF , lblDone ,
    AI_TRYTriggeringAlarmAtPad , ((9000) & 0xFF00) >> 8, (9000) & 0x00FF , lblAlarmActivated ,
    AI_GotoNext , lblDone ,

    AI_Label , lblAlarmActivated , AI_Yield ,
        AI_IFImOnPatrolOrStopped , lblDone ,
    AI_GotoFirst , lblAlarmActivated ,




    AI_Label , lblDone ,
    AI_SetReturnAiList , ((GAILIST_STANDARD_GUARD) & 0xFF00) >> 8, (GAILIST_STANDARD_GUARD) & 0x00FF ,AI_SetChrAiList , -3 , (((!((GAILIST_RUN_TO_BOND) >= 4096) ? GAILIST_RUN_TO_BOND : (('E','R','R','O','R',':',' ','A','I','_','L','I','S','T',' ','I','S',' ','A',' ','S','U','B','R','O','U','T','I','N','E',' ','O','R',' ','N','O','T',' ','F','O','R',' ','C','H','R'), 1))) & 0xFF00) >> 8, ((!((GAILIST_RUN_TO_BOND) >= 4096) ? GAILIST_RUN_TO_BOND : (('E','R','R','O','R',':',' ','A','I','_','L','I','S','T',' ','I','S',' ','A',' ','S','U','B','R','O','U','T','I','N','E',' ','O','R',' ','N','O','T',' ','F','O','R',' ','C','H','R'), 1))) & 0x00FF ,
    AI_EndList
};






u8 m_StartleAndRunToBond[] = {
    AI_LookSurprised ,
    AI_Label , lblLoop , AI_Yield ,
        AI_IFImOnPatrolOrStopped , lblDone ,
    AI_GotoFirst , lblLoop ,

    AI_Label , lblDone ,
        AI_SetChrAiList , -3 , (((!((GAILIST_RUN_TO_BOND) >= 4096) ? GAILIST_RUN_TO_BOND : (('E','R','R','O','R',':',' ','A','I','_','L','I','S','T',' ','I','S',' ','A',' ','S','U','B','R','O','U','T','I','N','E',' ','O','R',' ','N','O','T',' ','F','O','R',' ','C','H','R'), 1))) & 0xFF00) >> 8, ((!((GAILIST_RUN_TO_BOND) >= 4096) ? GAILIST_RUN_TO_BOND : (('E','R','R','O','R',':',' ','A','I','_','L','I','S','T',' ','I','S',' ','A',' ','S','U','B','R','O','U','T','I','N','E',' ','O','R',' ','N','O','T',' ','F','O','R',' ','C','H','R'), 1))) & 0x00FF ,
    AI_EndList
};






u8 m_RunToBondPersistent[] = {


    AI_Label , lblRun ,
        AI_TRYRunToBond , lblRunning ,
       
# 788 "<stdin>"
#pragma endregion Private Members
# 555 "<stdin>"
        PRINT
