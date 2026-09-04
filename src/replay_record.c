#include "replay.h"
#include "joy.h"
#include "game/file.h"
#include "game/file2.h"
#include "game/lvl.h"
#include <PR/os_internal.h>
#include <PR/rcp.h>

#ifdef REPLAY_RECORD

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
#define REPLAY_SRAM_SIZE (128 * 1024)
#define REPLAY_BUFFER_SIZE 256
#define REPLAY_FRAME_SIZE 21
#define REPLAY_OPTIONS_SIZE 3
#define REPLAY_PROFILE_VERSION 1

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

typedef char ReplayHeaderSizeMustStayStable[sizeof(ReplayHeader) == 48 ? 1 : -1];

typedef struct ReplayWriter {
    u8 buffers[2][REPLAY_BUFFER_SIZE];
    OSMesgQueue queue;
    OSMesg message;
    OSIoMesg io;
    u32 next_offset;
    u32 position;
    s32 buffer;
    s32 pending;
    s32 failed;
} ReplayWriter;

extern OSDevMgr __osPiDevMgr;
extern OSPiHandle *__osPiTable;
extern OSMesgQueue *osPiGetCmdQueue(void);
extern u64 g_randomSeed;
extern u64 g_chrObjRandomSeed;
extern s32 speedgraphframes;
extern void *memcpy(void *dst, const void *src, size_t count);
extern void fileSaveSettingsForFolder(save_data *save);
extern save_data *fileGetSaveForFoldernum(u32 folder);
extern s32 fileGetSaveStageDifficultyTime(save_data *save,
                                          LEVEL_SOLO_SEQUENCE level,
                                          DIFFICULTY difficulty);
extern void fileSetDifficultyStageTime(save_data *save,
                                       LEVEL_SOLO_SEQUENCE level,
                                       DIFFICULTY difficulty, s32 time);
extern void fileWriteSave(save_data *save);

static OSPiHandle g_SramHandle;
static ReplayHeader g_Header;
static ReplayWriter g_Writer;
static s32 g_Recording;

s32 replay_sram_read(u32 offset, void *destination, u32 size)
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
        return -1;
    }

    osRecvMesg(&queue, NULL, OS_MESG_BLOCK);
    return 0;
}

s32 replay_sram_write(u32 offset, void *source, u32 size)
{
    OSIoMesg io;
    OSMesgQueue queue;
    OSMesg message;

    osWritebackDCache(source, size);
    osCreateMesgQueue(&queue, &message, 1);
    io.hdr.type = OS_MESG_TYPE_EDMAWRITE;
    io.hdr.pri = OS_MESG_PRI_NORMAL;
    io.hdr.retQueue = &queue;
    io.dramAddr = source;
    io.devAddr = offset;
    io.size = size;
    io.piHandle = &g_SramHandle;

    if (!__osPiDevMgr.active ||
        osSendMesg(osPiGetCmdQueue(), (OSMesg)&io, OS_MESG_BLOCK) == -1) {
        return -1;
    }

    osRecvMesg(&queue, NULL, OS_MESG_BLOCK);
    return 0;
}

static void writer_wait(void)
{
    if (g_Writer.pending) {
        osRecvMesg(&g_Writer.queue, NULL, OS_MESG_BLOCK);
        g_Writer.pending = FALSE;
    }
}

static void writer_submit(void)
{
    u8 *buffer;

    writer_wait();
    buffer = g_Writer.buffers[g_Writer.buffer];
    osWritebackDCache(buffer, REPLAY_BUFFER_SIZE);
    g_Writer.io.hdr.type = OS_MESG_TYPE_EDMAWRITE;
    g_Writer.io.hdr.pri = OS_MESG_PRI_NORMAL;
    g_Writer.io.hdr.retQueue = &g_Writer.queue;
    g_Writer.io.dramAddr = buffer;
    g_Writer.io.devAddr = g_Writer.next_offset;
    g_Writer.io.size = REPLAY_BUFFER_SIZE;
    g_Writer.io.piHandle = &g_SramHandle;

    if (!__osPiDevMgr.active ||
        osSendMesg(osPiGetCmdQueue(), (OSMesg)&g_Writer.io,
                   OS_MESG_BLOCK) == -1) {
        g_Writer.failed = TRUE;
        return;
    }

    g_Writer.pending = TRUE;
    g_Writer.next_offset += REPLAY_BUFFER_SIZE;
    g_Writer.buffer ^= 1;
    g_Writer.position = 0;
    bzero(g_Writer.buffers[g_Writer.buffer], REPLAY_BUFFER_SIZE);
}

static void writer_write(const void *source, u32 size)
{
    const u8 *bytes = source;

    while (size && !g_Writer.failed) {
        u32 available = REPLAY_BUFFER_SIZE - g_Writer.position;
        u32 chunk = size < available ? size : available;

        memcpy(&g_Writer.buffers[g_Writer.buffer][g_Writer.position], bytes,
               chunk);
        g_Writer.position += chunk;
        bytes += chunk;
        size -= chunk;
        if (g_Writer.position == REPLAY_BUFFER_SIZE) {
            writer_submit();
        }
    }
}

static u32 stream_size(void)
{
    return sizeof(ReplayHeader) +
           g_Header.frame_count * REPLAY_FRAME_SIZE +
           g_Header.options_frame_count * REPLAY_OPTIONS_SIZE;
}

static s32 can_append(u32 size)
{
    return stream_size() + size <= REPLAY_SRAM_SIZE - REPLAY_SRAM_OFFSET;
}

static u16 current_options(void)
{
    save_data save;

    fileSaveSettingsForFolder(&save);
    return save.options;
}

static void stop_recording(void)
{
    if (!g_Recording) {
        return;
    }

    joySetRecordFunc(NULL);
    if (!g_Writer.failed && g_Writer.position) {
        writer_submit();
    }
    writer_wait();
    g_Recording = FALSE;

    if (g_Writer.failed || g_Header.frame_count == 0) {
        return;
    }

    g_Header.magic = REPLAY_MAGIC;
    g_Header.total_size = stream_size();
    replay_sram_write(REPLAY_SRAM_OFFSET, &g_Header, sizeof(g_Header));
}

static void write_options(u16 options)
{
    u8 type = 0;

    if (!can_append(REPLAY_OPTIONS_SIZE)) {
        stop_recording();
        return;
    }
    writer_write(&type, sizeof(type));
    writer_write(&options, sizeof(options));
    if (g_Writer.failed) {
        stop_recording();
        return;
    }
    g_Header.options_frame_count++;
}

static void record_frame(struct contsample *samples, s32 start, s32 end)
{
    u16 buttons;
    u8 delta;

    if (!g_Recording || speedgraphframes <= 0) {
        return;
    }
    if (speedgraphframes > 255 || !can_append(REPLAY_FRAME_SIZE)) {
        stop_recording();
        return;
    }

    delta = speedgraphframes;
    buttons = samples[end].pads[0].button;
    writer_write(&delta, sizeof(delta));
    writer_write(&g_randomSeed, sizeof(g_randomSeed));
    writer_write(&g_chrObjRandomSeed, sizeof(g_chrObjRandomSeed));
    writer_write(&buttons, sizeof(buttons));
    writer_write(&samples[end].pads[0].stick_x,
                 sizeof(samples[end].pads[0].stick_x));
    writer_write(&samples[end].pads[0].stick_y,
                 sizeof(samples[end].pads[0].stick_y));
    if (g_Writer.failed) {
        stop_recording();
        return;
    }
    g_Header.frame_count++;
    g_Header.duration_video_frames += delta;
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
}

void replay_init_profile(void)
{
    save_data blank = BLANKSAVEDATA;
    save_data *save;
    s32 level;
    s32 difficulty;
    s32 initialize_settings;

    save = fileGetSaveForFoldernum(FOLDER1);
    initialize_settings = !save;
    if (!save) {
        saves[SAVESLOT1] = blank;
        saves[SAVESLOT1].completion_bitflags =
            SAVEFLAGS_SET(FOLDER1, SAVESLOT1, BOND_BROSNAN, FALSE);
        save = &saves[SAVESLOT1];
    }

    if (save->padding != REPLAY_PROFILE_VERSION) {
        initialize_settings = TRUE;
    }

    if (initialize_settings) {
        save->music_vol = 0xff;
        save->sfx_vol = 0xff;
        save->options = (DEFAULT_OPTIONS | OPTION_INVERTLOOK |
                         ((CONTROLLER_CONFIG_SOLITARE << 8) &
                          OPTION_CONTROLTYPE)) &
                        ~OPTION_LOOKAHEAD;
        save->padding = REPLAY_PROFILE_VERSION;
    }

    for (level = SP_LEVEL_DAM; level < SP_LEVEL_MAX; level++) {
        for (difficulty = DIFFICULTY_AGENT;
             difficulty < DIFFICULTY_007; difficulty++) {
            if (fileGetSaveStageDifficultyTime(save, level, difficulty) == 0) {
                fileSetDifficultyStageTime(save, level, difficulty, 0x3ff);
            }
        }
    }

    save->unlocked_cheats_1 = 0xff;
    save->unlocked_cheats_2 = 0xff;
    save->unlocked_cheats_3 = 0xff;
    save->flag_007 |= 1;
    fileWriteSave(save);
}

void replay_before_stage_load(s32 stage)
{
    u32 invalid_magic = 0;

    stop_recording();
    if (stage == LEVELID_TITLE) {
        return;
    }

    replay_sram_write(REPLAY_SRAM_OFFSET, &invalid_magic,
                      sizeof(invalid_magic));
    bzero(&g_Header, sizeof(g_Header));
    g_Header.version = REPLAY_VERSION;
    g_Header.header_size = sizeof(ReplayHeader);
    g_Header.region = REPLAY_REGION;
    g_Header.stage_id = stage;
    g_Header.difficulty = lvlGetSelectedDifficulty();
    g_Header.flags = REPLAY_FLAG_FRAME_SEEDS;
    g_Header.random_seed = g_randomSeed;
    g_Header.chr_obj_random_seed = g_chrObjRandomSeed;

    bzero(&g_Writer, sizeof(g_Writer));
    osCreateMesgQueue(&g_Writer.queue, &g_Writer.message, 1);
    g_Writer.next_offset = REPLAY_SRAM_OFFSET;
    writer_write(&g_Header, sizeof(g_Header));
    g_Recording = TRUE;
}

void replay_on_stage_load(void)
{
    if (!g_Recording) {
        return;
    }

    write_options(current_options());
    if (g_Recording) {
        joySetRecordFunc(record_frame);
    }
}

#endif
