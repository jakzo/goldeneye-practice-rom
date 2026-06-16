#ifndef PRACTICE_REPLAY_H
#define PRACTICE_REPLAY_H

#include "joy.h"
#include <bondtypes.h>
#include <ultra64.h>

#define REPLAY_MAGIC 0x47455250 // "GERP"
#define REPLAY_VERSION 1
#define MAX_REPLAY_EVENTS 6000

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

typedef struct {
  u16 buttons;
  s8 stick_x;
  s8 stick_y;
} ReplayPadInput;

// TODO: Move to RLE event-based to save space?
// TODO: Assuming 1.x control style and current settings for now
typedef struct {
  u8 type;
  u8 delta_frames;
  u16 stand_tile_index;
  coord2d pos_in_stand_tile;
  ReplayPadInput pad1;
} ReplayEvent;

typedef struct {
  u32 magic;
  u8 version;
  u8 region;
  u8 stage_id;
  u8 difficulty;
  u64 random_seed;
  u64 chr_obj_random_seed;
  u32 event_count;
} ReplayHeader;

typedef struct {
  ReplayHeader header;
  ReplayEvent events[MAX_REPLAY_EVENTS];
} ReplaySession;

extern ReplaySession *g_ReplaySession;
extern bool g_ReplayIsRecording;

void practice_replay_on_stage_load(void);
void practice_replay_start_recording(void);
void practice_replay_stop_recording(void);
void practice_replay_record_callback(struct contsample *samples, s32 start_idx,
                                     s32 end_idx);

#endif /* PRACTICE_REPLAY_H */
