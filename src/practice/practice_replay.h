#ifndef PRACTICE_REPLAY_H
#define PRACTICE_REPLAY_H

#include "joy.h"
#include <bondtypes.h>
#include <ultra64.h>

#define REPLAY_MAGIC 0x47455250 /* "GERP" */
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

#define REPLAY_FLAG_FRAME_SEEDS 0x01
#define REPLAY_KNOWN_FLAGS REPLAY_FLAG_FRAME_SEEDS

enum PracticeReplayMode {
  PRACTICE_REPLAY_DISABLED,
  PRACTICE_REPLAY_RECORD,
  PRACTICE_REPLAY_PLAYBACK
};

typedef struct ReplayHeader {
  u32 magic;                 /* 0x00: REPLAY_MAGIC ("GERP") */
  u16 version;               /* 0x04: recording format version */
  u16 header_size;           /* 0x06: bytes before the first frame */
  u32 total_size;            /* 0x08: header and frames in bytes */
  u32 frame_count;           /* 0x0c */
  u8 region;                 /* 0x10: US, JP, or EU */
  u8 stage_id;               /* 0x11 */
  u8 difficulty;             /* 0x12 */
  u8 flags;                  /* 0x13: REPLAY_FLAG_* */
  u16 options_frame_count;   /* 0x14 */
  u16 reserved;              /* 0x16 */
  u64 random_seed;           /* 0x18: seed before level initialization */
  u64 chr_obj_random_seed;   /* 0x20: seed before level initialization */
  u32 duration_video_frames; /* 0x28: sum of frame delta values */
  u32 reserved2;             /* 0x2c */
} ReplayHeader;

/*
 * Entry encoding:
 *   u8 delta/type (0 = options frame, 1-255 = input frame)
 *   Options frame: u16 OPTION_* settings flags
 *   Input frame: optional u64 random seed and u64 chr/object random seed,
 *                u16 buttons, s8 stick X, s8 stick Y
 * Fields are streamed separately so no compiler padding enters the file.
 */

typedef char
    ReplayHeaderSizeMustStayStable[sizeof(ReplayHeader) == 48 ? 1 : -1];

extern bool g_ReplayIsRecording;
extern bool g_ReplayIsPlaying;

void practice_replay_init(void);
void practice_replay_before_stage_load(s32 stage_id);
void practice_replay_on_stage_load(void);

void practice_replay_request_recording(void);
void practice_replay_request_seeded_recording(void);
void practice_replay_request_playback(void);
void practice_replay_stop_recording(void);
void practice_replay_stop_playback(void);

s32 practice_replay_can_play(s32 stage_id);
void practice_replay_invalidate_saved(void);
s32 practice_replay_had_divergence(void);
u32 practice_replay_get_timestamp(void);
u32 practice_replay_get_duration(void);

s32 practice_replay_override_delta(s32 delta_frames);
void practice_replay_on_frame_start(void);

#endif /* PRACTICE_REPLAY_H */
