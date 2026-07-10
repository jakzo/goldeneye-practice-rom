#include "practice_replay.h"
#include "file.h"
#include "file2.h"
#include "game/lvl.h"
#include "practice_config.h"
#include "practice_sram.h"
#include "practice_ui.h"
#include "state/practice_states.h"
#include "watch.h"
#include <bondconstants.h>
#include <os_extension.h>
#include <ultra64.h>

#define REPLAY_DMA_BUFFER_SIZE 256
#define REPLAY_INPUT_FRAME_SIZE_WITH_SEEDS 21
#define REPLAY_INPUT_FRAME_SIZE_WITHOUT_SEEDS 5
#define REPLAY_OPTIONS_FRAME_SIZE 3
#define REPLAY_ENTRY_OPTIONS 0

extern s32 g_CurrentStageToLoad;
extern u64 g_randomSeed;
extern u64 g_chrObjRandomSeed;
extern s32 speedgraphframes;
extern void *memcpy(void *dst, const void *src, size_t count);
extern int sprintf(char *dst, const char *fmt, ...);

typedef struct ReplayFrame {
  u64 random_seed;
  u64 chr_obj_random_seed;
  u16 buttons;
  s8 stick_x;
  s8 stick_y;
  u8 delta_frames;
} ReplayFrame;

typedef struct ReplayDmaWriter {
  u8 buffers[2][REPLAY_DMA_BUFFER_SIZE];
  OSMesgQueue queue;
  OSMesg message;
  OSIoMesg io;
  u32 next_sram_offset;
  u32 position;
  s32 current_buffer;
  s32 dma_pending;
  s32 failed;
} ReplayDmaWriter;

typedef struct ReplayDmaReader {
  /* Keep state beyond both writer DMA buffers while sharing the stream union. */
  u8 buffers[2][REPLAY_DMA_BUFFER_SIZE];
  u32 current_page_offset;
  u32 total_size;
  u32 position;
  s32 failed;
} ReplayDmaReader;

typedef union ReplayDmaStream {
  ReplayDmaWriter writer;
  ReplayDmaReader reader;
} ReplayDmaStream;

static ReplayHeader g_SavedReplayHeader __attribute__((aligned(16)));
static ReplayHeader g_ActiveReplayHeader __attribute__((aligned(16)));
static ReplayDmaStream g_ReplayDma __attribute__((aligned(16)));
static ReplayFrame g_PlaybackFrame;
static u32 g_ReplayFrameIndex;
static u32 g_ReplayTimestamp;
static u32 g_PlaybackEntryCount;
static u32 g_PlaybackOptionsFrameIndex;
static u16 g_LastRecordedSettingsFlags;
static s32 g_PlaybackFrameLoaded;
static s32 g_ReplayDiverged;
static s32 g_ReplayDivergenceLogged;
static s32 g_RequestRecording;
static s32 g_RequestFrameSeeds;
static s32 g_RequestPlayback;

bool g_ReplayIsRecording = FALSE;
bool g_ReplayIsPlaying = FALSE;

static void practice_replay_record_callback(struct contsample *samples,
                                            s32 start_idx, s32 end_idx);
static s32 practice_replay_playback_callback(struct contsample *samples,
                                             s32 last_idx);

static u32 replay_frame_size(u8 flags) {
  return flags & REPLAY_FLAG_FRAME_SEEDS
             ? REPLAY_INPUT_FRAME_SIZE_WITH_SEEDS
             : REPLAY_INPUT_FRAME_SIZE_WITHOUT_SEEDS;
}

static void format_timestamp(u32 video_frames, char *buffer) {
  u32 timer_ticks;
  u32 minutes;
  u32 seconds;
  u32 hundredths;

#if defined(VERSION_EU)
  timer_ticks = video_frames * 60 / 50;
#else
  timer_ticks = video_frames;
#endif
  minutes = timer_ticks / 60 / 60;
  seconds = timer_ticks / 60 % 60;
  hundredths = (timer_ticks % 60) * 100 / 60;
  sprintf(buffer, "%d:%02d.%02d", minutes, seconds, hundredths);
}

static s32 replay_header_is_valid(const ReplayHeader *header) {
  u32 expected_size;
  u32 maximum_size = SRAM_SIZE_BYTES - SAVE_STATE_SRAM_OFFSET;
  u32 maximum_frames;

  if (header->magic != REPLAY_MAGIC || header->version != REPLAY_VERSION ||
      header->header_size != sizeof(ReplayHeader) ||
      header->region != REPLAY_REGION || header->frame_count == 0 ||
      header->options_frame_count == 0 ||
      (header->flags & ~REPLAY_KNOWN_FLAGS) != 0) {
    return FALSE;
  }

  maximum_frames =
      (maximum_size - sizeof(ReplayHeader) - REPLAY_OPTIONS_FRAME_SIZE) /
      replay_frame_size(header->flags);
  if (header->frame_count > maximum_frames ||
      header->duration_video_frames < header->frame_count ||
      header->duration_video_frames > header->frame_count * 255) {
    return FALSE;
  }

  expected_size = sizeof(ReplayHeader) +
                  header->frame_count * replay_frame_size(header->flags) +
                  header->options_frame_count * REPLAY_OPTIONS_FRAME_SIZE;
  return header->total_size == expected_size && expected_size <= maximum_size;
}

static u16 current_settings_flags(void) {
  u16 flags = 0;

  if (get_cur_player_look_vertical_inverted())
    flags |= OPTION_INVERTLOOK;
  if (cur_player_get_autoaim())
    flags |= OPTION_AUTOAIM;
  if (cur_player_get_aim_control())
    flags |= OPTION_AIMCONTROL;
  if (cur_player_get_sight_onscreen_control())
    flags |= OPTION_SIGHTONSCREEN;
  if (cur_player_get_lookahead())
    flags |= OPTION_LOOKAHEAD;
  if (cur_player_get_ammo_onscreen_setting())
    flags |= OPTION_DISPLAYAMMO;
  if (cur_player_get_screen_setting() == SCREEN_SIZE_WIDESCREEN)
    flags |= OPTION_SCREENWIDE;
  else if (cur_player_get_screen_setting() == SCREEN_SIZE_CINEMA)
    flags |= OPTION_SCREENCINEMA;
  if (get_screen_ratio() != SCREEN_RATIO_NORMAL)
    flags |= OPTION_SCREENRATIO;
  flags |= ((u16)(cur_player_get_control_type() << 8)) & OPTION_CONTROLTYPE;
  return flags;
}

static void apply_settings_flags(u16 flags) {
  cur_player_set_control_type((flags & OPTION_CONTROLTYPE) >> 8);
  set_cur_player_look_vertical_inverted((flags & OPTION_INVERTLOOK) != 0);
  cur_player_set_autoaim((flags & OPTION_AUTOAIM) != 0);
  cur_player_set_aim_control((flags & OPTION_AIMCONTROL) != 0);
  cur_player_set_sight_onscreen_control((flags & OPTION_SIGHTONSCREEN) != 0);
  cur_player_set_lookahead((flags & OPTION_LOOKAHEAD) != 0);
  cur_player_set_ammo_onscreen_setting((flags & OPTION_DISPLAYAMMO) != 0);

  if (flags & OPTION_SCREENCINEMA)
    cur_player_set_screen_setting(SCREEN_SIZE_CINEMA);
  else if (flags & OPTION_SCREENWIDE)
    cur_player_set_screen_setting(SCREEN_SIZE_WIDESCREEN);
  else
    cur_player_set_screen_setting(SCREEN_SIZE_FULLSCREEN);
  set_screen_ratio((flags & OPTION_SCREENRATIO) != 0);
}

static void writer_wait(ReplayDmaWriter *writer) {
  if (writer->dma_pending) {
    osRecvMesg(&writer->queue, NULL, OS_MESG_BLOCK);
    writer->dma_pending = FALSE;
  }
}

static void writer_submit(ReplayDmaWriter *writer) {
  u8 *buffer;

  writer_wait(writer);
  buffer = writer->buffers[writer->current_buffer];
  if (sram_start_dma(OS_WRITE, writer->next_sram_offset, buffer,
                     REPLAY_DMA_BUFFER_SIZE, &writer->io,
                     &writer->queue) != 0) {
    writer->failed = TRUE;
    return;
  }

  writer->dma_pending = TRUE;
  writer->next_sram_offset += REPLAY_DMA_BUFFER_SIZE;
  writer->current_buffer ^= 1;
  writer->position = 0;
  bzero(writer->buffers[writer->current_buffer], REPLAY_DMA_BUFFER_SIZE);
}

static void writer_init(ReplayDmaWriter *writer) {
  bzero(writer, sizeof(*writer));
  osCreateMesgQueue(&writer->queue, &writer->message, 1);
  writer->next_sram_offset = SAVE_STATE_SRAM_OFFSET;
}

static void writer_write(ReplayDmaWriter *writer, const void *src, u32 size) {
  const u8 *bytes = src;

  while (size > 0 && !writer->failed) {
    u32 available = REPLAY_DMA_BUFFER_SIZE - writer->position;
    u32 chunk = size < available ? size : available;
    memcpy(&writer->buffers[writer->current_buffer][writer->position], bytes,
           chunk);
    writer->position += chunk;
    bytes += chunk;
    size -= chunk;
    if (writer->position == REPLAY_DMA_BUFFER_SIZE)
      writer_submit(writer);
  }
}

static void writer_finish(ReplayDmaWriter *writer) {
  if (!writer->failed && writer->position > 0)
    writer_submit(writer);
  writer_wait(writer);
}

static void reader_init(ReplayDmaReader *reader, u32 total_size) {
  bzero(reader, sizeof(*reader));
  reader->current_page_offset = SAVE_STATE_SRAM_OFFSET;
  reader->total_size = total_size;
  reader->position = sizeof(ReplayHeader);
  if (sram_read(reader->current_page_offset, reader->buffers[0],
                REPLAY_DMA_BUFFER_SIZE) != 0) {
    reader->failed = TRUE;
  }
}

static void reader_advance_page(ReplayDmaReader *reader) {
  u32 next_offset = reader->current_page_offset + REPLAY_DMA_BUFFER_SIZE;

  if (next_offset >= SAVE_STATE_SRAM_OFFSET + reader->total_size ||
      sram_read(next_offset, reader->buffers[0],
                REPLAY_DMA_BUFFER_SIZE) != 0) {
    reader->failed = TRUE;
    return;
  }

  reader->current_page_offset = next_offset;
  reader->position = 0;
}

static void reader_read(ReplayDmaReader *reader, void *dst, u32 size) {
  u8 *bytes = dst;
  u32 stream_position =
      reader->current_page_offset - SAVE_STATE_SRAM_OFFSET + reader->position;

  if (stream_position > reader->total_size ||
      size > reader->total_size - stream_position) {
    reader->failed = TRUE;
    return;
  }

  while (size > 0 && !reader->failed) {
    u32 available = REPLAY_DMA_BUFFER_SIZE - reader->position;
    u32 chunk = size < available ? size : available;
    memcpy(bytes, &reader->buffers[0][reader->position], chunk);
    reader->position += chunk;
    bytes += chunk;
    size -= chunk;
    if (reader->position == REPLAY_DMA_BUFFER_SIZE && size > 0)
      reader_advance_page(reader);
  }
}

static void write_frame(ReplayDmaWriter *writer, const ReplayFrame *frame) {
  writer_write(writer, &frame->delta_frames, sizeof(frame->delta_frames));
  if (g_ActiveReplayHeader.flags & REPLAY_FLAG_FRAME_SEEDS) {
    writer_write(writer, &frame->random_seed, sizeof(frame->random_seed));
    writer_write(writer, &frame->chr_obj_random_seed,
                 sizeof(frame->chr_obj_random_seed));
  }
  writer_write(writer, &frame->buttons, sizeof(frame->buttons));
  writer_write(writer, &frame->stick_x, sizeof(frame->stick_x));
  writer_write(writer, &frame->stick_y, sizeof(frame->stick_y));
}

static void read_frame(ReplayDmaReader *reader, ReplayFrame *frame,
                       u8 delta_frames) {
  frame->delta_frames = delta_frames;
  if (g_ActiveReplayHeader.flags & REPLAY_FLAG_FRAME_SEEDS) {
    reader_read(reader, &frame->random_seed, sizeof(frame->random_seed));
    reader_read(reader, &frame->chr_obj_random_seed,
                sizeof(frame->chr_obj_random_seed));
  }
  reader_read(reader, &frame->buttons, sizeof(frame->buttons));
  reader_read(reader, &frame->stick_x, sizeof(frame->stick_x));
  reader_read(reader, &frame->stick_y, sizeof(frame->stick_y));
}

static u32 replay_stream_size(void) {
  return sizeof(ReplayHeader) +
         g_ReplayFrameIndex * replay_frame_size(g_ActiveReplayHeader.flags) +
         g_ActiveReplayHeader.options_frame_count * REPLAY_OPTIONS_FRAME_SIZE;
}

static s32 replay_can_append(u32 size) {
  return replay_stream_size() + size <=
         SRAM_SIZE_BYTES - SAVE_STATE_SRAM_OFFSET;
}

static void write_options_frame(u16 settings_flags) {
  u8 entry_type = REPLAY_ENTRY_OPTIONS;

  if (!replay_can_append(REPLAY_OPTIONS_FRAME_SIZE)) {
    practiceLogWarn("Replay: SRAM is full");
    practice_replay_stop_recording();
    return;
  }
  writer_write(&g_ReplayDma.writer, &entry_type, sizeof(entry_type));
  writer_write(&g_ReplayDma.writer, &settings_flags, sizeof(settings_flags));
  if (g_ReplayDma.writer.failed) {
    practiceLogWarn("Replay: SRAM write failed");
    practice_replay_stop_recording();
    return;
  }
  g_ActiveReplayHeader.options_frame_count++;
  g_LastRecordedSettingsFlags = settings_flags;
}

static void start_recording(s32 stage_id, s32 force_frame_seeds) {
  u32 invalid_magic = 0;
  ReplayDmaWriter *writer = &g_ReplayDma.writer;

  practice_replay_invalidate_saved();
  g_HasSavedState = FALSE;
  sram_write(SAVE_STATE_SRAM_OFFSET, &invalid_magic, sizeof(invalid_magic));

  bzero(&g_ActiveReplayHeader, sizeof(g_ActiveReplayHeader));
  g_ActiveReplayHeader.version = REPLAY_VERSION;
  g_ActiveReplayHeader.header_size = sizeof(ReplayHeader);
  g_ActiveReplayHeader.region = REPLAY_REGION;
  g_ActiveReplayHeader.stage_id = stage_id;
  g_ActiveReplayHeader.difficulty = lvlGetSelectedDifficulty();
  if (force_frame_seeds)
    g_ActiveReplayHeader.flags |= REPLAY_FLAG_FRAME_SEEDS;
#if DEV
  if (practice.record_replay_seeds)
    g_ActiveReplayHeader.flags |= REPLAY_FLAG_FRAME_SEEDS;
#endif
  g_ActiveReplayHeader.random_seed = g_randomSeed;
  g_ActiveReplayHeader.chr_obj_random_seed = g_chrObjRandomSeed;

  writer_init(writer);
  writer_write(writer, &g_ActiveReplayHeader, sizeof(g_ActiveReplayHeader));
  g_ReplayFrameIndex = 0;
  g_ReplayIsRecording = TRUE;
}

static void start_playback(void) {
  g_ActiveReplayHeader = g_SavedReplayHeader;
  g_randomSeed = g_ActiveReplayHeader.random_seed;
  g_chrObjRandomSeed = g_ActiveReplayHeader.chr_obj_random_seed;
  set_selected_difficulty(g_ActiveReplayHeader.difficulty);
  lvlSetSelectedDifficulty(g_ActiveReplayHeader.difficulty);
  reader_init(&g_ReplayDma.reader, g_ActiveReplayHeader.total_size);
  if (g_ReplayDma.reader.failed) {
    practiceLogWarn("Replay: Failed to read SRAM");
    return;
  }

  g_ReplayFrameIndex = 0;
  g_ReplayTimestamp = 0;
  g_PlaybackEntryCount = 0;
  g_PlaybackOptionsFrameIndex = 0;
  g_PlaybackFrameLoaded = FALSE;
  g_ReplayDiverged = FALSE;
  g_ReplayDivergenceLogged = FALSE;
  g_ReplayIsPlaying = TRUE;
}

static s32 load_playback_frame(void) {
  ReplayDmaReader *reader = &g_ReplayDma.reader;

  while (g_ReplayFrameIndex < g_ActiveReplayHeader.frame_count) {
    u8 delta_or_type;

    reader_read(reader, &delta_or_type, sizeof(delta_or_type));
    if (reader->failed)
      return FALSE;
    if (g_PlaybackEntryCount++ == 0 && delta_or_type != REPLAY_ENTRY_OPTIONS) {
      reader->failed = TRUE;
      return FALSE;
    }

    if (delta_or_type == REPLAY_ENTRY_OPTIONS) {
      u16 settings_flags;

      reader_read(reader, &settings_flags, sizeof(settings_flags));
      if (reader->failed || ++g_PlaybackOptionsFrameIndex >
                                g_ActiveReplayHeader.options_frame_count) {
        reader->failed = TRUE;
        return FALSE;
      }
      apply_settings_flags(settings_flags);
      continue;
    }

    bzero(&g_PlaybackFrame, sizeof(g_PlaybackFrame));
    read_frame(reader, &g_PlaybackFrame, delta_or_type);
    if (reader->failed)
      return FALSE;
    g_PlaybackFrameLoaded = TRUE;
    return TRUE;
  }

  return FALSE;
}

void practice_replay_init(void) {
  sram_read(SAVE_STATE_SRAM_OFFSET, &g_SavedReplayHeader,
            sizeof(g_SavedReplayHeader));
  if (!replay_header_is_valid(&g_SavedReplayHeader))
    bzero(&g_SavedReplayHeader, sizeof(g_SavedReplayHeader));
}

void practice_replay_request_recording(void) {
  g_RequestRecording = TRUE;
  g_RequestFrameSeeds = FALSE;
}

void practice_replay_request_seeded_recording(void) {
  g_RequestRecording = TRUE;
  g_RequestFrameSeeds = TRUE;
}

void practice_replay_request_playback(void) { g_RequestPlayback = TRUE; }

void practice_replay_before_stage_load(s32 stage_id) {
  s32 mode = practice.replay_mode;
  s32 force_frame_seeds = g_RequestFrameSeeds;

  practice_replay_stop_recording();
  practice_replay_stop_playback();

  if (stage_id == LEVELID_TITLE)
    return;

  if (g_RequestRecording)
    mode = PRACTICE_REPLAY_RECORD;
  else if (g_RequestPlayback)
    mode = PRACTICE_REPLAY_PLAYBACK;
  g_RequestRecording = FALSE;
  g_RequestFrameSeeds = FALSE;
  g_RequestPlayback = FALSE;

  if (practice.replay_mode != PRACTICE_REPLAY_DISABLED) {
    practice.replay_mode = PRACTICE_REPLAY_DISABLED;
    practice_config_save();
  }

  if (mode == PRACTICE_REPLAY_RECORD) {
    start_recording(stage_id, force_frame_seeds);
  } else if (mode == PRACTICE_REPLAY_PLAYBACK) {
    if (!practice_replay_can_play(stage_id)) {
      practiceLogWarn("Replay: No recording for this level");
      return;
    }
    start_playback();
  }
}

void practice_replay_on_stage_load(void) {
  if (g_ReplayIsRecording) {
    if (cur_player_get_control_type() >= CONTROLLER_CONFIG_PLENTY) {
      practiceLogWarn("Replay: Dual-controller styles are not supported");
      practice_replay_stop_recording();
      return;
    }
    write_options_frame(current_settings_flags());
    if (!g_ReplayIsRecording)
      return;
    joySetRecordFunc(practice_replay_record_callback);
    practiceLogInfo("Replay: Recording started (%s seeds)",
                    g_ActiveReplayHeader.flags & REPLAY_FLAG_FRAME_SEEDS
                        ? "with"
                        : "without");
  } else if (g_ReplayIsPlaying) {
    if (!load_playback_frame()) {
      practiceLogWarn("Replay: Invalid frame data");
      practice_replay_stop_playback();
      return;
    }
    joySetPlaybackFunc(practice_replay_playback_callback, 1);
    joySetContDataIndex(1);
    practiceLogInfo("Replay: Playback started");
  }
}

void practice_replay_stop_recording(void) {
  ReplayDmaWriter *writer;
  char duration[16];

  if (!g_ReplayIsRecording)
    return;

  joySetRecordFunc(NULL);
  writer = &g_ReplayDma.writer;
  writer_finish(writer);
  g_ReplayIsRecording = FALSE;

  if (writer->failed || g_ReplayFrameIndex == 0) {
    practiceLogWarn("Replay: Recording failed");
    practice_replay_invalidate_saved();
    return;
  }

  g_ActiveReplayHeader.magic = REPLAY_MAGIC;
  g_ActiveReplayHeader.frame_count = g_ReplayFrameIndex;
  g_ActiveReplayHeader.total_size = replay_stream_size();
  if (sram_write(SAVE_STATE_SRAM_OFFSET, &g_ActiveReplayHeader,
                 sizeof(g_ActiveReplayHeader)) != 0) {
    practiceLogWarn("Replay: Failed to finalize recording");
    practice_replay_invalidate_saved();
    return;
  }

  g_SavedReplayHeader = g_ActiveReplayHeader;
  format_timestamp(g_ActiveReplayHeader.duration_video_frames, duration);
  practiceLogInfo("Replay: Recording stopped (%s, %dKB)", duration,
                  (g_ActiveReplayHeader.total_size + 1023) / 1024);
}

void practice_replay_stop_playback(void) {
  char timestamp[16];
  char duration[16];

  if (!g_ReplayIsPlaying)
    return;
  g_ReplayIsPlaying = FALSE;
  g_PlaybackFrameLoaded = FALSE;
  joySetPlaybackFunc(NULL, -1);
  joySetContDataIndex(0);
  format_timestamp(g_ReplayTimestamp, timestamp);
  format_timestamp(g_ActiveReplayHeader.duration_video_frames, duration);
  practiceLogInfo("Replay: Playback stopped (%s/%s)", timestamp, duration);
}

s32 practice_replay_can_play(s32 stage_id) {
  return replay_header_is_valid(&g_SavedReplayHeader) &&
         g_SavedReplayHeader.stage_id == stage_id;
}

void practice_replay_invalidate_saved(void) {
  bzero(&g_SavedReplayHeader, sizeof(g_SavedReplayHeader));
}

s32 practice_replay_had_divergence(void) { return g_ReplayDiverged; }

u32 practice_replay_get_timestamp(void) { return g_ReplayTimestamp; }

u32 practice_replay_get_duration(void) {
  return g_ActiveReplayHeader.duration_video_frames;
}

s32 practice_replay_override_delta(s32 delta_frames) {
  if (!g_ReplayIsPlaying)
    return delta_frames;

  if (g_ReplayFrameIndex >= g_ActiveReplayHeader.frame_count) {
    practice_replay_stop_playback();
    return delta_frames;
  }

  if (!g_PlaybackFrameLoaded) {
    if (!load_playback_frame()) {
      char timestamp[16];
      format_timestamp(g_ReplayTimestamp, timestamp);
      practiceLogWarn("Replay: Invalid frame data at %s", timestamp);
      practice_replay_stop_playback();
      return delta_frames;
    }
    g_PlaybackFrameLoaded = TRUE;
  }
  return g_PlaybackFrame.delta_frames;
}

void practice_replay_on_frame_start(void) {
  if (!g_ReplayIsPlaying || !g_PlaybackFrameLoaded)
    return;

  if ((g_ActiveReplayHeader.flags & REPLAY_FLAG_FRAME_SEEDS) &&
      (g_randomSeed != g_PlaybackFrame.random_seed ||
       g_chrObjRandomSeed != g_PlaybackFrame.chr_obj_random_seed)) {
    g_ReplayDiverged = TRUE;
    if (!g_ReplayDivergenceLogged) {
      char timestamp[16];
      format_timestamp(g_ReplayTimestamp, timestamp);
      practiceLogError("Replay diverged at %s", timestamp);
      g_ReplayDivergenceLogged = TRUE;
    }
  }
}

static void practice_replay_record_callback(struct contsample *samples,
                                            s32 start_idx, s32 end_idx) {
  ReplayFrame frame;
  u16 settings_flags;

  if (!g_ReplayIsRecording)
    return;
  if (speedgraphframes < 0 || speedgraphframes > 255) {
    practiceLogWarn("Replay: Frame delta is out of range");
    practice_replay_stop_recording();
    return;
  }
  if (cur_player_get_control_type() >= CONTROLLER_CONFIG_PLENTY) {
    practiceLogWarn("Replay: Dual-controller styles are not supported");
    practice_replay_stop_recording();
    return;
  }

  settings_flags = current_settings_flags();
  if (settings_flags != g_LastRecordedSettingsFlags) {
    write_options_frame(settings_flags);
    if (!g_ReplayIsRecording)
      return;
  }

  if (speedgraphframes == 0)
    return;

  if (!replay_can_append(replay_frame_size(g_ActiveReplayHeader.flags))) {
    practiceLogWarn("Replay: SRAM is full");
    practice_replay_stop_recording();
    return;
  }

  frame.random_seed = g_randomSeed;
  frame.chr_obj_random_seed = g_chrObjRandomSeed;
  frame.buttons = samples[end_idx].pads[0].button | g_SimulatedButtons;
  frame.stick_x = g_SimulatedStickEnabled ? g_SimulatedStickX
                                          : samples[end_idx].pads[0].stick_x;
  frame.stick_y = g_SimulatedStickEnabled ? g_SimulatedStickY
                                          : samples[end_idx].pads[0].stick_y;
  frame.delta_frames = speedgraphframes;
  write_frame(&g_ReplayDma.writer, &frame);
  if (g_ReplayDma.writer.failed) {
    practiceLogWarn("Replay: SRAM write failed");
    practice_replay_stop_recording();
    return;
  }
  g_ReplayFrameIndex++;
  g_ActiveReplayHeader.duration_video_frames += frame.delta_frames;
}

static s32 practice_replay_playback_callback(struct contsample *samples,
                                             s32 last_idx) {
  u16 real_pressed;
  u16 trigger = practice.left_trigger_hotkeys ? L_TRIG : R_TRIG;
  s32 next_idx;
  s32 i;

  joySetContDataIndex(0);
  real_pressed = joyGetButtonsPressedThisFrame(0, ANY_BUTTON);
  joySetContDataIndex(1);
  if (real_pressed & ~trigger) {
    char timestamp[16];
    format_timestamp(g_ReplayTimestamp, timestamp);
    practiceLogInfo("Replay: Playback cancelled at %s", timestamp);
    practice_replay_stop_playback();
    return last_idx;
  }

  if (!g_ReplayIsPlaying || !g_PlaybackFrameLoaded)
    return last_idx;

  next_idx = (last_idx + 1) % CONTSAMPLE_LEN;
  for (i = 0; i < MAXCONTROLLERS; i++) {
    samples[next_idx].pads[i].button = 0;
    samples[next_idx].pads[i].stick_x = 0;
    samples[next_idx].pads[i].stick_y = 0;
    samples[next_idx].pads[i].errno = 0;
  }
  samples[next_idx].pads[0].button = g_PlaybackFrame.buttons;
  samples[next_idx].pads[0].stick_x = g_PlaybackFrame.stick_x;
  samples[next_idx].pads[0].stick_y = g_PlaybackFrame.stick_y;

  g_ReplayFrameIndex++;
  g_ReplayTimestamp += g_PlaybackFrame.delta_frames;
  g_PlaybackFrameLoaded = FALSE;
  return next_idx;
}
