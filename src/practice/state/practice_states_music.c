#include "practice_states_music.h"
#include "../practice_music.h"
#include "mp_music.h"
#include <bondconstants.h>
#include <music.h>
#include <PR/libaudio.h>

#define MUSIC_CHANNEL_COUNT 16
#define MUSIC_PROGRAM_PERCUSSION -1
#define MUSIC_PROGRAM_NONE -2

typedef struct MusicChannelState {
  s32 program;
  s32 bend_range;
  s32 fx_id;
  s32 pan;
  s32 priority;
  s32 volume;
  s32 fx_mix;
  s32 sustain;
  f32 pitch_bend;
} MusicChannelState;

typedef struct MusicTrackState {
  s32 track;
  u16 volume;
  s32 fade;
  u16 pre_fade_volume;
  s16 fade_to_volume;
  s32 fade_remaining_frames;
  s32 ticks;
  s32 was_playing;
  s32 microseconds_per_tick;
  MusicChannelState channels[MUSIC_CHANNEL_COUNT];
} MusicTrackState;

extern s32 g_musicXTrack1CurrentTrackNum;
extern s32 g_musicXTrack2CurrentTrackNum;
extern s32 g_musicXTrack3CurrentTrackNum;
extern u16 g_musicXTrack1PreFadeSavedVolume;
extern u16 g_musicXTrack2PreFadeSavedVolume;
extern u16 g_musicXTrack3PreFadeSavedVolume;
extern s16 g_musicXTrack1FadeToVolume;
extern s16 g_musicXTrack2FadeToVolume;
extern s16 g_musicXTrack3FadeToVolume;
extern s32 g_musicXTrack1FadeRemainingFrames;
extern s32 g_musicXTrack2FadeRemainingFrames;
extern s32 g_musicXTrack3FadeRemainingFrames;
extern ALCSeq g_musicXTrack1Seq;
extern ALCSeq g_musicXTrack2Seq;
extern ALCSeq g_musicXTrack3Seq;
extern ALCSPlayer *g_musicXTrack1SeqPlayer;
extern ALCSPlayer *g_musicXTrack2SeqPlayer;
extern ALCSPlayer *g_musicXTrack3SeqPlayer;
extern s32 musictrack1_playing;
extern s32 stageMusicID;
extern s32 dword_CODE_bss_8008C604;
extern s32 mission_state;
extern s32 music_slot_active_0[4];
extern s32 music_slot_minutes_0[4];
extern s32 music_slot_seconds_0[4];

static ALCSPlayer *get_music_player(s32 index) {
  switch (index) {
  case 0:
    return g_musicXTrack1SeqPlayer;
  case 1:
    return g_musicXTrack2SeqPlayer;
  default:
    return g_musicXTrack3SeqPlayer;
  }
}

static s32 get_channel_program(ALCSPlayer *player, s32 channel) {
  ALInstrument *instrument = player->chanState[channel].instrument;
  s32 i;

  if (instrument == NULL) {
    return MUSIC_PROGRAM_NONE;
  }
  if (instrument == player->bank->percussion) {
    return MUSIC_PROGRAM_PERCUSSION;
  }

  for (i = 0; i < player->bank->instCount; i++) {
    if (player->bank->instArray[i] == instrument) {
      return i;
    }
  }

  return MUSIC_PROGRAM_NONE;
}

static void get_music_channel_state(ALCSPlayer *player, s32 channel,
                                    MusicChannelState *state) {
  ALChanState *channel_state = &player->chanState[channel];

  state->program = get_channel_program(player, channel);
  state->bend_range = channel_state->bendRange;
  state->fx_id = channel_state->fxId;
  state->pan = channel_state->pan;
  state->priority = channel_state->priority;
  state->volume = channel_state->vol;
  state->fx_mix = channel_state->fxmix;
  state->sustain = channel_state->sustain;
  state->pitch_bend = channel_state->pitchBend;
}

static void get_music_track_state(s32 index, MusicTrackState *state) {
  ALCSPlayer *player = get_music_player(index);
  s32 channel;

  switch (index) {
  case 0:
    state->track = g_musicXTrack1CurrentTrackNum;
    state->volume = musicTrack1GetVolume();
    state->fade = g_musicXTrack1Fade;
    state->pre_fade_volume = g_musicXTrack1PreFadeSavedVolume;
    state->fade_to_volume = g_musicXTrack1FadeToVolume;
    state->fade_remaining_frames = g_musicXTrack1FadeRemainingFrames;
    state->ticks =
        state->track != M_NONE ? alCSeqGetTicks(&g_musicXTrack1Seq) : 0;
    break;
  case 1:
    state->track = g_musicXTrack2CurrentTrackNum;
    state->volume = musicTrack2GetVolume();
    state->fade = g_musicXTrack2Fade;
    state->pre_fade_volume = g_musicXTrack2PreFadeSavedVolume;
    state->fade_to_volume = g_musicXTrack2FadeToVolume;
    state->fade_remaining_frames = g_musicXTrack2FadeRemainingFrames;
    state->ticks =
        state->track != M_NONE ? alCSeqGetTicks(&g_musicXTrack2Seq) : 0;
    break;
  default:
    state->track = g_musicXTrack3CurrentTrackNum;
    state->volume = musicTrack3GetVolume();
    state->fade = g_musicXTrack3Fade;
    state->pre_fade_volume = g_musicXTrack3PreFadeSavedVolume;
    state->fade_to_volume = g_musicXTrack3FadeToVolume;
    state->fade_remaining_frames = g_musicXTrack3FadeRemainingFrames;
    state->ticks =
        state->track != M_NONE ? alCSeqGetTicks(&g_musicXTrack3Seq) : 0;
    break;
  }

  state->was_playing = practice_music_track_was_playing(index);
  state->microseconds_per_tick = player->uspt;
  for (channel = 0; channel < MUSIC_CHANNEL_COUNT; channel++) {
    get_music_channel_state(player, channel, &state->channels[channel]);
  }
}

static void write_music_track_state(StateStream *stream,
                                    MusicTrackState *state) {
  write_u32(stream, state->track);
  write_u16(stream, state->volume);
  write_u32(stream, state->fade);
  write_u16(stream, state->pre_fade_volume);
  write_u16(stream, state->fade_to_volume);
  write_u32(stream, state->fade_remaining_frames);
  write_u32(stream, state->ticks);
  write_u32(stream, state->was_playing);
  write_u32(stream, state->microseconds_per_tick);
  {
    s32 channel;
    for (channel = 0; channel < MUSIC_CHANNEL_COUNT; channel++) {
      MusicChannelState *channel_state = &state->channels[channel];
      write_u32(stream, channel_state->program);
      write_u32(stream, channel_state->bend_range);
      write_u32(stream, channel_state->fx_id);
      write_u32(stream, channel_state->pan);
      write_u32(stream, channel_state->priority);
      write_u32(stream, channel_state->volume);
      write_u32(stream, channel_state->fx_mix);
      write_u32(stream, channel_state->sustain);
      write_f32(stream, channel_state->pitch_bend);
    }
  }
}

static void read_music_track_state(StateStream *stream,
                                   MusicTrackState *state) {
  state->track = read_u32(stream);
  state->volume = read_u16(stream);
  state->fade = read_u32(stream);
  state->pre_fade_volume = read_u16(stream);
  state->fade_to_volume = read_u16(stream);
  state->fade_remaining_frames = read_u32(stream);
  state->ticks = read_u32(stream);
  state->was_playing = read_u32(stream);
  state->microseconds_per_tick = read_u32(stream);
  {
    s32 channel;
    for (channel = 0; channel < MUSIC_CHANNEL_COUNT; channel++) {
      MusicChannelState *channel_state = &state->channels[channel];
      channel_state->program = read_u32(stream);
      channel_state->bend_range = read_u32(stream);
      channel_state->fx_id = read_u32(stream);
      channel_state->pan = read_u32(stream);
      channel_state->priority = read_u32(stream);
      channel_state->volume = read_u32(stream);
      channel_state->fx_mix = read_u32(stream);
      channel_state->sustain = read_u32(stream);
      channel_state->pitch_bend = read_f32(stream);
    }
  }
}

static void stop_new_music_track(ALCSPlayer *player) {
  while (alCSPGetState(player) == AL_STOPPED) {
  }
  alCSPStop(player);
  while (alCSPGetState(player) != AL_STOPPED) {
  }
}

static void restore_music_channels(ALCSPlayer *player,
                                   MusicTrackState *state) {
  s32 channel;

  player->uspt = state->microseconds_per_tick;

  for (channel = 0; channel < MUSIC_CHANNEL_COUNT; channel++) {
    MusicChannelState *saved = &state->channels[channel];
    ALChanState *live = &player->chanState[channel];

    if (saved->program == MUSIC_PROGRAM_PERCUSSION) {
      live->instrument = player->bank->percussion;
    } else if (saved->program >= 0 &&
               saved->program < player->bank->instCount) {
      live->instrument = player->bank->instArray[saved->program];
    }

    live->bendRange = saved->bend_range;
    live->fxId = saved->fx_id;
    live->pan = saved->pan;
    live->priority = saved->priority;
    live->vol = saved->volume;
    live->fxmix = saved->fx_mix;
    live->sustain = saved->sustain;
    live->pitchBend = saved->pitch_bend;
  }
}

static void restore_music_track(s32 index, MusicTrackState *state) {
  ALCSeq *seq;
  ALCSPlayer *player = get_music_player(index);
  ALCSeqMarker marker;

  if (state->track < M_NONE || state->track >= NUM_MUSIC_TRACKS) {
    state->track = M_NONE;
    state->was_playing = FALSE;
  }

  switch (index) {
  case 0:
    musicTrack1Stop();
    if (state->track != M_NONE) {
      musicTrack1ApplySeqpVol(0);
      musicTrack1Play(state->track);
      stop_new_music_track(player);
      seq = &g_musicXTrack1Seq;
      alCSeqNewMarker(seq, &marker, state->ticks);
      alCSeqSetLoc(seq, &marker);
      restore_music_channels(player, state);
    }
    musicTrack1ApplySeqpVol(state->volume);
    g_musicXTrack1Fade = state->fade;
    g_musicXTrack1PreFadeSavedVolume = state->pre_fade_volume;
    g_musicXTrack1FadeToVolume = state->fade_to_volume;
    g_musicXTrack1FadeRemainingFrames = state->fade_remaining_frames;
    break;
  case 1:
    musicTrack2Stop();
    if (state->track != M_NONE) {
      musicTrack2ApplySeqpVol(0);
      musicTrack2Play(state->track);
      stop_new_music_track(player);
      seq = &g_musicXTrack2Seq;
      alCSeqNewMarker(seq, &marker, state->ticks);
      alCSeqSetLoc(seq, &marker);
      restore_music_channels(player, state);
    }
    musicTrack2ApplySeqpVol(state->volume);
    g_musicXTrack2Fade = state->fade;
    g_musicXTrack2PreFadeSavedVolume = state->pre_fade_volume;
    g_musicXTrack2FadeToVolume = state->fade_to_volume;
    g_musicXTrack2FadeRemainingFrames = state->fade_remaining_frames;
    break;
  default:
    musicTrack3Stop();
    if (state->track != M_NONE) {
      musicTrack3ApplySeqpVol(0);
      musicTrack3Play(state->track);
      stop_new_music_track(player);
      seq = &g_musicXTrack3Seq;
      alCSeqNewMarker(seq, &marker, state->ticks);
      alCSeqSetLoc(seq, &marker);
      restore_music_channels(player, state);
    }
    musicTrack3ApplySeqpVol(state->volume);
    g_musicXTrack3Fade = state->fade;
    g_musicXTrack3PreFadeSavedVolume = state->pre_fade_volume;
    g_musicXTrack3FadeToVolume = state->fade_to_volume;
    g_musicXTrack3FadeRemainingFrames = state->fade_remaining_frames;
    break;
  }

  practice_music_set_paused_track_state(index, state->was_playing);
}

void save_music_state(StateStream *stream) {
  MusicTrackState track_state;
  s32 i;

  for (i = 0; i < 3; i++) {
    get_music_track_state(i, &track_state);
    write_music_track_state(stream, &track_state);
  }

  write_u32(stream, musictrack1_playing);
  write_u32(stream, stageMusicID);
  write_u32(stream, dword_CODE_bss_8008C604);
  write_u32(stream, mission_state);
  for (i = 0; i < 4; i++) {
    write_u32(stream, music_slot_active_0[i]);
    write_u32(stream, music_slot_minutes_0[i]);
    write_u32(stream, music_slot_seconds_0[i]);
  }
}

void load_music_state(StateStream *stream) {
  MusicTrackState track_states[3];
  s32 i;

  for (i = 0; i < 3; i++) {
    read_music_track_state(stream, &track_states[i]);
  }

  musictrack1_playing = read_u32(stream);
  stageMusicID = read_u32(stream);
  dword_CODE_bss_8008C604 = read_u32(stream);
  mission_state = read_u32(stream);
  for (i = 0; i < 4; i++) {
    music_slot_active_0[i] = read_u32(stream);
    music_slot_minutes_0[i] = read_u32(stream);
    music_slot_seconds_0[i] = read_u32(stream);
  }

  for (i = 0; i < 3; i++) {
    restore_music_track(i, &track_states[i]);
  }
}
