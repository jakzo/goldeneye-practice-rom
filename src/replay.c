#include "replay.h"
#include "joy.h"
#include "game/file.h"
#include "game/file2.h"
#include "game/lvl.h"
#include "game/bondview.h"
#include "game/player.h"
#include "game/watch.h"
#include <PR/os_internal.h>
#include <PR/rcp.h>
#include <stdarg.h>
#ifdef ENABLE_USB
#include "usb.h"
#endif

#ifdef REPLAY_PLAYBACK

#define REPLAY_MAGIC 0x47455250
#define REPLAY_VERSION 1
#define REPLAY_REGION_US 1
#define REPLAY_REGION_JP 2
#define REPLAY_REGION_EU 3

#if defined(VERSION_US)
#define REPLAY_REGION REPLAY_REGION_US
#elif defined(VERSION_JP)
#define REPLAY_REGION REPLAY_REGION_JP
#elif defined(VERSION_EU)
#define REPLAY_REGION REPLAY_REGION_EU
#else
#define REPLAY_REGION 0
#endif
#define REPLAY_FLAG_FRAME_SEEDS 1
#define REPLAY_SRAM_OFFSET 0x600
#define REPLAY_PAGE_SIZE 256
#define ISVIEWER_MAGIC 0x13ff0000
#define ISVIEWER_WRITE_PTR 0x13ff0014
#define ISVIEWER_BUFFER 0x13ff0020

typedef struct ReplayHeader {
    u32 magic;
    u16 version;
    u16 header_size;
    u32 total_size;
    u32 frame_count;
    u8 region;
    u8 stage_id;
    u8 difficulty;
    u8 flags;
    u16 options_frame_count;
    u16 reserved;
    u64 random_seed;
    u64 chr_obj_random_seed;
    u32 duration_video_frames;
    u32 reserved2;
} ReplayHeader;

typedef struct ReplayFrame {
    u64 random_seed;
    u64 chr_obj_random_seed;
    u16 buttons;
    s8 stick_x;
    s8 stick_y;
    u8 delta_frames;
} ReplayFrame;

typedef struct LogWriter {
    char *position;
    size_t remaining;
} LogWriter;

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
static u32 g_PageOffset;
static u32 g_StreamPosition;
static u32 g_FrameIndex;
static u32 g_Timestamp;
static s32 g_FrameLoaded;
static s32 g_Playing;
static s32 g_LogAvailable;
#ifdef ENABLE_USB
static u64 g_ProfileCycleTotal;
static u32 g_ProfileCycleStart;
static u32 g_ProfileFrameCount;
static u32 g_ProfileDroppedFrames;
static u32 g_ProfileRetraceFrames;
static u32 g_ProfileFullMoveRetraceFrames;
static u64 g_ProfileLagTimeNanoseconds;
static s32 g_ProfileFrameActive;
static s32 g_ProfileUpdateSeen;
#endif

static char *log_writer(char *arg, const char *src, size_t count)
{
    LogWriter *writer = (LogWriter *)arg;

    if (count > writer->remaining) {
        count = writer->remaining;
    }

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
    s32 length;
#ifndef ENABLE_USB
    s32 i;
    u32 word;
#endif

    if (!g_LogAvailable) {
        return;
    }

    writer.position = buffer;
    writer.remaining = sizeof(buffer) - 2;
    va_start(args, format);
    _Printf(log_writer, (char *)&writer, format, args);
    va_end(args);
    writer.position--;
    *writer.position++ = '\n';
    length = writer.position - buffer;

#ifdef ENABLE_USB
    usb_write_text((u8 *)buffer, length);
#else
    for (i = 0; i < length; i += 4) {
        word = (u32)(u8)buffer[i] << 24;
        if (i + 1 < length) {
            word |= (u32)(u8)buffer[i + 1] << 16;
        }
        if (i + 2 < length) {
            word |= (u32)(u8)buffer[i + 2] << 8;
        }
        if (i + 3 < length) {
            word |= (u32)(u8)buffer[i + 3];
        }
        IO_WRITE(ISVIEWER_BUFFER + i, word);
    }

    IO_WRITE(ISVIEWER_WRITE_PTR, length);
#endif
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
        osSendMesg(osPiGetCmdQueue(), (OSMesg)&io, OS_MESG_BLOCK) == -1) {
        return FALSE;
    }

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
        u32 page_position;
        u32 chunk;

        if (g_StreamPosition < g_PageOffset ||
            g_StreamPosition >= g_PageOffset + REPLAY_PAGE_SIZE) {
            reader_page(g_StreamPosition);
        }

        page_position = g_StreamPosition - g_PageOffset;
        chunk = REPLAY_PAGE_SIZE - page_position;
        if (chunk > size) {
            chunk = size;
        }

        memcpy(out, &g_Page[page_position], chunk);
        out += chunk;
        g_StreamPosition += chunk;
        size -= chunk;
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

    if (options & OPTION_SCREENCINEMA) {
        cur_player_set_screen_setting(SCREEN_SIZE_CINEMA);
    } else if (options & OPTION_SCREENWIDE) {
        cur_player_set_screen_setting(SCREEN_SIZE_WIDESCREEN);
    } else {
        cur_player_set_screen_setting(SCREEN_SIZE_FULLSCREEN);
    }

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

static void stop_playback(void)
{
    g_Playing = FALSE;
    g_FrameLoaded = FALSE;
    joySetPlaybackFunc(NULL, -1);
    joySetContDataIndex(0);
}

static s32 playback_callback(struct contsample *samples, s32 last_index)
{
    s32 next_index;
    s32 i;

    if (!g_Playing || !g_FrameLoaded) {
        return last_index;
    }

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

void replay_init(void)
{
    OSIntMask mask;

    IO_WRITE(PI_BSD_DOM2_LAT_REG, 5);
    IO_WRITE(PI_BSD_DOM2_PWD_REG, 12);
    IO_WRITE(PI_BSD_DOM2_PGS_REG, 13);
    IO_WRITE(PI_BSD_DOM2_RLS_REG, 2);
    bzero(&g_SramHandle, sizeof(g_SramHandle));
    g_SramHandle.type = DEVICE_TYPE_SRAM;
    g_SramHandle.baseAddress = PHYS_TO_K1(0x08000000);
    g_SramHandle.latency = 5;
    g_SramHandle.pulse = 12;
    g_SramHandle.pageSize = 13;
    g_SramHandle.relDuration = 2;
    g_SramHandle.domain = PI_DOMAIN2;
    mask = osSetIntMask(OS_IM_NONE);
    g_SramHandle.next = __osPiTable;
    __osPiTable = &g_SramHandle;
    osSetIntMask(mask);

#ifdef ENABLE_USB
    g_LogAvailable = TRUE;
#else
    IO_WRITE(ISVIEWER_MAGIC, 0x12345678);
    g_LogAvailable = IO_READ(ISVIEWER_MAGIC) == 0x12345678;
#endif
    sram_read(REPLAY_SRAM_OFFSET, &g_Header, sizeof(g_Header));
}

s32 replay_get_stage(void)
{
    if (g_Header.magic == REPLAY_MAGIC &&
        g_Header.version == REPLAY_VERSION &&
        g_Header.region == REPLAY_REGION &&
        g_Header.stage_id < LEVELID_MAX) {
        return g_Header.stage_id;
    }

    return LEVELID_TITLE;
}

void replay_before_stage_load(s32 stage)
{
    if (g_FrameIndex == g_Header.frame_count && g_Header.frame_count != 0) {
        return;
    }

    if (g_Header.magic != REPLAY_MAGIC || g_Header.version != REPLAY_VERSION ||
        g_Header.region != REPLAY_REGION || g_Header.stage_id != stage) {
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
    g_FrameIndex = 0;
    g_Timestamp = 0;
    g_FrameLoaded = FALSE;
    g_Playing = TRUE;
#ifdef ENABLE_USB
    g_ProfileCycleTotal = 0;
    g_ProfileCycleStart = 0;
    g_ProfileFrameCount = 0;
    g_ProfileDroppedFrames = 0;
    g_ProfileRetraceFrames = 1;
    g_ProfileFullMoveRetraceFrames = 0;
    g_ProfileLagTimeNanoseconds = 0;
    g_ProfileFrameActive = FALSE;
    g_ProfileUpdateSeen = FALSE;
#endif
}

void replay_on_stage_load(void)
{
    if (g_Playing && load_frame()) {
        joySetPlaybackFunc(playback_callback, 1);
        joySetContDataIndex(1);
        log_line("REPLAY_STARTED frames=%u duration=%u",
                 g_Header.frame_count, g_Header.duration_video_frames);
    }
}

s32 replay_override_delta(s32 delta_frames)
{
    if (!g_Playing) {
        return delta_frames;
    }

    if (g_FrameIndex >= g_Header.frame_count) {
#ifdef ENABLE_USB
        log_line("PERF_STATS frames=%u cycle_hi=%u cycle_lo=%u dropped_frames=%u",
                 g_ProfileFrameCount, (u32)(g_ProfileCycleTotal >> 32),
                 (u32)g_ProfileCycleTotal, g_ProfileDroppedFrames);
        log_line("LAG_STATS lag_ns_hi=%u lag_ns_lo=%u full_move_retrace_frames=%u mission_timer_ticks=%d",
                 (u32)(g_ProfileLagTimeNanoseconds >> 32),
                 (u32)g_ProfileLagTimeNanoseconds,
                 g_ProfileFullMoveRetraceFrames, mission_timer);
#endif
        log_line("TEST_COMPLETE frames=%u duration=%u", g_FrameIndex, g_Timestamp);
        stop_playback();
        return delta_frames;
    }

    if (!g_FrameLoaded && !load_frame()) {
        log_line("TEST_FAILED invalid replay data");
        stop_playback();
        return delta_frames;
    }

    return g_Frame.delta_frames;
}

#ifdef ENABLE_USB
void replay_profile_update_retrace(s32 delta_frames)
{
    if (!g_Playing) {
        return;
    }

    if (g_ProfileUpdateSeen) {
        g_ProfileRetraceFrames = delta_frames;
    } else {
        /* Do not charge the stage-load interval to the first rendered frame. */
        g_ProfileRetraceFrames = 1;
    }
    g_ProfileUpdateSeen = TRUE;
}

void replay_profile_lag_tick(void)
{
    f32 d;
    f32 poly_added;
    f32 lagged_100m_time;
    f32 lag_seconds;
    u32 dropped_frames;
    u32 lag_nanoseconds;

    if (!g_Playing || g_CurrentPlayer == NULL || !is_timer_active ||
        g_ClockTimer <= 0) {
        return;
    }

    if (g_ProfileRetraceFrames > 1) {
        g_ProfileDroppedFrames += g_ProfileRetraceFrames - 1;
    }

    if ((g_CurrentPlayer->speedforwards > -0.5f &&
         g_CurrentPlayer->speedforwards < 0.5f) ||
        (g_CurrentPlayer->speedsideways > -0.5f &&
         g_CurrentPlayer->speedsideways < 0.5f)) {
        return;
    }

    g_ProfileFullMoveRetraceFrames += g_ProfileRetraceFrames;
    dropped_frames = g_ProfileRetraceFrames - 1;
    if (dropped_frames == 0) {
        return;
    }

    d = (f32)dropped_frames;
    poly_added =
        d * (0.247800872f +
             d * (0.017377249f +
                  d * (-0.001018621f + d * 0.000013940f)));
    if (poly_added <= 0.0f) {
        return;
    }

    lagged_100m_time = 12.17f + poly_added;
    lag_seconds = ((f32)g_ProfileRetraceFrames / 60.0f) *
                  (poly_added / lagged_100m_time);

    /* Quantize each contribution once, then accumulate without float drift. */
    lag_nanoseconds = (u32)(lag_seconds * 1000000000.0f + 0.5f);
    g_ProfileLagTimeNanoseconds += lag_nanoseconds;
}

void replay_profile_frame_start(void)
{
    if (!g_Playing) {
        return;
    }

    g_ProfileCycleStart = osGetCount();
    g_ProfileFrameActive = TRUE;
}

void replay_profile_frame_end(void)
{
    if (!g_ProfileFrameActive) {
        return;
    }

    g_ProfileCycleTotal += (u32)(osGetCount() - g_ProfileCycleStart);
    g_ProfileFrameCount++;
    g_ProfileFrameActive = FALSE;
}
#endif

void replay_frame_start(void)
{
    if (!g_Playing || !g_FrameLoaded) {
        return;
    }

    if ((g_Header.flags & REPLAY_FLAG_FRAME_SEEDS) &&
        (g_randomSeed != g_Frame.random_seed ||
         g_chrObjRandomSeed != g_Frame.chr_obj_random_seed)) {
        log_line("TEST_FAILED replay diverged frame=%u", g_FrameIndex);
        stop_playback();
    }
}

#endif
