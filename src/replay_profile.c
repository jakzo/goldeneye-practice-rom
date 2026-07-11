#include "replay_profile.h"
#include "joy.h"
#include "game/file.h"
#include "game/file2.h"
#include "game/lvl.h"
#include "game/watch.h"
#include "tlb_manage.h"
#include <PR/os_internal.h>
#include <PR/rcp.h>
#include <stdarg.h>

#define REPLAY_MAGIC 0x47455250
#define REPLAY_VERSION 1
#define REPLAY_REGION_US 1
#define REPLAY_FLAG_FRAME_SEEDS 1
#define REPLAY_SRAM_OFFSET 0x600
#define REPLAY_PAGE_SIZE 256
#define ISVIEWER_MAGIC 0x13ff0000
#define ISVIEWER_WRITE_PTR 0x13ff0014
#define ISVIEWER_BUFFER 0x13ff0020

typedef struct ReplayHeader {
    u32 magic; u16 version; u16 header_size; u32 total_size; u32 frame_count;
    u8 region; u8 stage_id; u8 difficulty; u8 flags;
    u16 options_frame_count; u16 reserved;
    u64 random_seed; u64 chr_obj_random_seed;
    u32 duration_video_frames; u32 reserved2;
} ReplayHeader;
typedef struct ReplayFrame {
    u64 random_seed; u64 chr_obj_random_seed;
    u16 buttons; s8 stick_x; s8 stick_y; u8 delta_frames;
} ReplayFrame;
typedef struct LogWriter { char *position; size_t remaining; } LogWriter;

extern OSDevMgr __osPiDevMgr;
extern OSPiHandle *__osPiTable;
extern OSMesgQueue *osPiGetCmdQueue(void);
extern u64 g_randomSeed;
extern u64 g_chrObjRandomSeed;
extern s32 _Printf(char *(*pfn)(char *, const char *, size_t), char *,
                   const char *, va_list);
extern void *memcpy(void *dst, const void *src, size_t count);

static OSPiHandle g_SramHandle;
static ReplayHeader g_Header;
static ReplayFrame g_Frame;
static u8 g_Page[REPLAY_PAGE_SIZE];
static u32 g_PageOffset, g_StreamPosition, g_FrameIndex, g_Timestamp;
static s32 g_FrameLoaded, g_Playing, g_LogAvailable;
static u32 g_ProfileFrame, g_ProfileStart[REPLAY_PROFILE_AREA_COUNT];
static u32 g_ProfileCycles[REPLAY_PROFILE_AREA_COUNT];
static s32 g_ProfileActive, g_ProfileDelta;
static u32 g_ProfileTlbStartLoads, g_ProfileTlbStartTotalCycles;
static u32 g_ProfileTlbStartDmaCycles, g_ProfileTlbStartIcacheCycles;

static char *log_writer(char *arg, const char *src, size_t count)
{
    LogWriter *writer = (LogWriter *)arg;
    if (count > writer->remaining) count = writer->remaining;
    memcpy(writer->position, src, count);
    writer->position += count;
    writer->remaining -= count;
    return arg;
}

static void log_line(const char *format, ...)
{
    char buffer[256];
    LogWriter writer;
    va_list args;
    s32 length, i;
    u32 word;
    if (!g_LogAvailable) return;
    writer.position = buffer;
    writer.remaining = sizeof(buffer) - 2;
    va_start(args, format);
    _Printf(log_writer, (char *)&writer, format, args);
    va_end(args);
    writer.position--;
    *writer.position++ = '\n';
    length = writer.position - buffer;
    for (i = 0; i < length; i += 4) {
        word = (u32)(u8)buffer[i] << 24;
        if (i + 1 < length) word |= (u32)(u8)buffer[i + 1] << 16;
        if (i + 2 < length) word |= (u32)(u8)buffer[i + 2] << 8;
        if (i + 3 < length) word |= (u32)(u8)buffer[i + 3];
        IO_WRITE(ISVIEWER_BUFFER + i, word);
    }
    IO_WRITE(ISVIEWER_WRITE_PTR, length);
}

static s32 sram_read(u32 offset, void *destination, u32 size)
{
    OSIoMesg io;
    OSMesgQueue queue;
    OSMesg message;
    osInvalDCache(destination, size);
    osCreateMesgQueue(&queue, &message, 1);
    io.hdr.type = OS_MESG_TYPE_EDMAREAD;
    io.hdr.pri = OS_MESG_PRI_NORMAL;
    io.hdr.retQueue = &queue;
    io.dramAddr = destination;
    io.devAddr = offset;
    io.size = size;
    io.piHandle = &g_SramHandle;
    if (!__osPiDevMgr.active ||
        osSendMesg(osPiGetCmdQueue(), (OSMesg)&io, OS_MESG_BLOCK) == -1)
        return FALSE;
    osRecvMesg(&queue, NULL, OS_MESG_BLOCK);
    return TRUE;
}

static void reader_page(u32 position)
{
    g_PageOffset = position & ~(REPLAY_PAGE_SIZE - 1);
    sram_read(REPLAY_SRAM_OFFSET + g_PageOffset, g_Page, sizeof(g_Page));
}

static void reader_read(void *destination, u32 size)
{
    u8 *out = destination;
    while (size) {
        u32 page_position, chunk;
        if (g_StreamPosition < g_PageOffset ||
            g_StreamPosition >= g_PageOffset + REPLAY_PAGE_SIZE)
            reader_page(g_StreamPosition);
        page_position = g_StreamPosition - g_PageOffset;
        chunk = REPLAY_PAGE_SIZE - page_position;
        if (chunk > size) chunk = size;
        memcpy(out, &g_Page[page_position], chunk);
        out += chunk; g_StreamPosition += chunk; size -= chunk;
    }
}

static void apply_options(u16 options)
{
    cur_player_set_control_type((options & OPTION_CONTROLTYPE) >> 8);
    set_cur_player_look_vertical_inverted((options & OPTION_INVERTLOOK) != 0);
    cur_player_set_autoaim((options & OPTION_AUTOAIM) != 0);
    cur_player_set_aim_control((options & OPTION_AIMCONTROL) != 0);
    cur_player_set_sight_onscreen_control((options & OPTION_SIGHTONSCREEN) != 0);
    cur_player_set_lookahead((options & OPTION_LOOKAHEAD) != 0);
    cur_player_set_ammo_onscreen_setting((options & OPTION_DISPLAYAMMO) != 0);
    if (options & OPTION_SCREENCINEMA)
        cur_player_set_screen_setting(SCREEN_SIZE_CINEMA);
    else if (options & OPTION_SCREENWIDE)
        cur_player_set_screen_setting(SCREEN_SIZE_WIDESCREEN);
    else
        cur_player_set_screen_setting(SCREEN_SIZE_FULLSCREEN);
    set_screen_ratio((options & OPTION_SCREENRATIO) != 0);
}

static s32 load_frame(void)
{
    u8 delta;
    while (g_FrameIndex < g_Header.frame_count) {
        reader_read(&delta, 1);
        if (delta == 0) {
            u16 options;
            reader_read(&options, sizeof(options));
            apply_options(options);
            continue;
        }
        g_Frame.delta_frames = delta;
        if (g_Header.flags & REPLAY_FLAG_FRAME_SEEDS) {
            reader_read(&g_Frame.random_seed, sizeof(g_Frame.random_seed));
            reader_read(&g_Frame.chr_obj_random_seed,
                        sizeof(g_Frame.chr_obj_random_seed));
        }
        reader_read(&g_Frame.buttons, sizeof(g_Frame.buttons));
        reader_read(&g_Frame.stick_x, sizeof(g_Frame.stick_x));
        reader_read(&g_Frame.stick_y, sizeof(g_Frame.stick_y));
        g_FrameLoaded = TRUE;
        return TRUE;
    }
    return FALSE;
}

static s32 playback_callback(struct contsample *samples, s32 last_index)
{
    s32 next_index, i;
    if (!g_Playing || !g_FrameLoaded) return last_index;
    next_index = (last_index + 1) % CONTSAMPLE_LEN;
    for (i = 0; i < MAXCONTROLLERS; i++) {
        samples[next_index].pads[i].button = 0;
        samples[next_index].pads[i].stick_x = 0;
        samples[next_index].pads[i].stick_y = 0;
        samples[next_index].pads[i].errno = 0;
    }
    samples[next_index].pads[0].button = g_Frame.buttons;
    samples[next_index].pads[0].stick_x = g_Frame.stick_x;
    samples[next_index].pads[0].stick_y = g_Frame.stick_y;
    g_FrameIndex++;
    g_Timestamp += g_Frame.delta_frames;
    g_FrameLoaded = FALSE;
    return next_index;
}

void replay_profile_init(void)
{
    OSIntMask mask;
    IO_WRITE(PI_BSD_DOM2_LAT_REG, 5);
    IO_WRITE(PI_BSD_DOM2_PWD_REG, 12);
    IO_WRITE(PI_BSD_DOM2_PGS_REG, 13);
    IO_WRITE(PI_BSD_DOM2_RLS_REG, 2);
    bzero(&g_SramHandle, sizeof(g_SramHandle));
    g_SramHandle.type = DEVICE_TYPE_SRAM;
    g_SramHandle.baseAddress = PHYS_TO_K1(0x08000000);
    g_SramHandle.latency = 5; g_SramHandle.pulse = 12;
    g_SramHandle.pageSize = 13; g_SramHandle.relDuration = 2;
    g_SramHandle.domain = PI_DOMAIN2;
    mask = osSetIntMask(OS_IM_NONE);
    g_SramHandle.next = __osPiTable;
    __osPiTable = &g_SramHandle;
    osSetIntMask(mask);
    IO_WRITE(ISVIEWER_MAGIC, 0x12345678);
    g_LogAvailable = IO_READ(ISVIEWER_MAGIC) == 0x12345678;
    sram_read(REPLAY_SRAM_OFFSET, &g_Header, sizeof(g_Header));
}

void replay_profile_before_stage_load(s32 stage)
{
    if (g_FrameIndex == g_Header.frame_count && g_Header.frame_count != 0)
        return;
    if (g_Header.magic != REPLAY_MAGIC || g_Header.version != REPLAY_VERSION ||
        g_Header.region != REPLAY_REGION_US || g_Header.stage_id != stage) {
        log_line("TEST_FAILED invalid replay header magic=%08x version=%u region=%u stage=%u expected=%u",
                 g_Header.magic, g_Header.version, g_Header.region,
                 g_Header.stage_id, stage);
        return;
    }
    g_randomSeed = g_Header.random_seed;
    g_chrObjRandomSeed = g_Header.chr_obj_random_seed;
    set_selected_difficulty(g_Header.difficulty);
    lvlSetSelectedDifficulty(g_Header.difficulty);
    g_StreamPosition = sizeof(ReplayHeader);
    reader_page(g_StreamPosition);
    g_FrameIndex = g_Timestamp = 0;
    g_FrameLoaded = FALSE;
    g_Playing = TRUE;
}

void replay_profile_on_stage_load(void)
{
    if (g_Playing && load_frame()) {
        joySetPlaybackFunc(playback_callback, 1);
        joySetContDataIndex(1);
        log_line("PROFILE_CSV,frame,delta_frames,tick_cycles,physics_cycles,render_cycles,practice_ui_cycles,physics_setup,physics_core,physics_practice,render_setup,render_visibility,render_bg,render_actors,render_hud,render_practice,tlb_loads,tlb_cycles,tlb_dma_cycles,tlb_icache_cycles");
    }
}

s32 replay_profile_override_delta(s32 delta_frames)
{
    if (!g_Playing) return delta_frames;
    if (g_FrameIndex >= g_Header.frame_count) {
        g_Playing = FALSE;
        joySetPlaybackFunc(NULL, -1);
        joySetContDataIndex(0);
        log_line("TEST_COMPLETE frames=%u duration=%u", g_FrameIndex, g_Timestamp);
        return delta_frames;
    }
    if (!g_FrameLoaded && !load_frame()) {
        g_Playing = FALSE;
        log_line("TEST_FAILED invalid replay data");
        return delta_frames;
    }
    return g_Frame.delta_frames;
}

void replay_profile_frame_start(s32 delta_frames)
{
    s32 i;
    g_ProfileActive = g_Playing && g_FrameLoaded;
    if (!g_ProfileActive) return;
    if ((g_Header.flags & REPLAY_FLAG_FRAME_SEEDS) &&
        (g_randomSeed != g_Frame.random_seed ||
         g_chrObjRandomSeed != g_Frame.chr_obj_random_seed)) {
        log_line("TEST_FAILED replay diverged frame=%u", g_FrameIndex);
        g_Playing = g_ProfileActive = FALSE;
        return;
    }
    g_ProfileDelta = delta_frames;
    for (i = 0; i < REPLAY_PROFILE_AREA_COUNT; i++) g_ProfileCycles[i] = 0;
    tlbmanageGetProfileTotals(&g_ProfileTlbStartLoads,
                              &g_ProfileTlbStartTotalCycles,
                              &g_ProfileTlbStartDmaCycles,
                              &g_ProfileTlbStartIcacheCycles);
}
void replay_profile_begin(ReplayProfileArea area)
{ if (g_ProfileActive) g_ProfileStart[area] = osGetCount(); }
void replay_profile_end(ReplayProfileArea area)
{ if (g_ProfileActive) g_ProfileCycles[area] += osGetCount() - g_ProfileStart[area]; }
void replay_profile_frame_end(void)
{
    u32 tlbLoads, tlbTotalCycles, tlbDmaCycles, tlbIcacheCycles;
    if (!g_ProfileActive) return;
    tlbmanageGetProfileTotals(&tlbLoads, &tlbTotalCycles, &tlbDmaCycles,
                              &tlbIcacheCycles);
    log_line("PROFILE_CSV,%u,%d,%u,%u,%u,0,%u,%u,%u,%u,%u,%u,%u,%u,%u,%u,%u,%u,%u",
             g_ProfileFrame, g_ProfileDelta,
             g_ProfileCycles[REPLAY_PROFILE_TICK],
             g_ProfileCycles[REPLAY_PROFILE_PHYSICS],
             g_ProfileCycles[REPLAY_PROFILE_RENDER],
             g_ProfileCycles[REPLAY_PROFILE_PHYSICS_SETUP],
             g_ProfileCycles[REPLAY_PROFILE_PHYSICS_CORE],
             g_ProfileCycles[REPLAY_PROFILE_PHYSICS_PRACTICE],
             g_ProfileCycles[REPLAY_PROFILE_RENDER_SETUP],
             g_ProfileCycles[REPLAY_PROFILE_RENDER_VISIBILITY],
             g_ProfileCycles[REPLAY_PROFILE_RENDER_BG],
             g_ProfileCycles[REPLAY_PROFILE_RENDER_ACTORS],
             g_ProfileCycles[REPLAY_PROFILE_RENDER_HUD],
             g_ProfileCycles[REPLAY_PROFILE_RENDER_PRACTICE],
             tlbLoads - g_ProfileTlbStartLoads,
             tlbTotalCycles - g_ProfileTlbStartTotalCycles,
             tlbDmaCycles - g_ProfileTlbStartDmaCycles,
             tlbIcacheCycles - g_ProfileTlbStartIcacheCycles);
    g_ProfileFrame++;
    g_ProfileActive = FALSE;
}
