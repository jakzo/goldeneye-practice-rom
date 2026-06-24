#ifndef PRACTICE_STATES_STREAM_H
#define PRACTICE_STATES_STREAM_H

#include <bondtypes.h>
#include <ultra64.h>

#ifndef __GNUC__
#define __attribute__(x)
#define inline
#endif

#define SRAM_PAGE_SIZE 512

typedef struct StateStream StateStream;

struct StateStream {
  void (*write_bytes)(StateStream *stream, const void *src, u32 size);
  void (*read_bytes)(StateStream *stream, void *dst, u32 size);
  void (*seek)(StateStream *stream, u32 absolute_offset);
  void (*flush)(StateStream *stream);
  u32 total_processed;
  u32 base_address;
};

typedef struct {
  StateStream base;
  u8 page[SRAM_PAGE_SIZE]
      __attribute__((aligned(16))); // 16 byte alignment for D-cache
  u32 page_offset;
  u32 current_page_addr;
  bool is_write;
  bool is_dirty;
} SramStream;

void sram_stream_init_write(SramStream *stream, u32 sram_base);
void sram_stream_init_read(SramStream *stream, u32 sram_base);

// Generic stream helpers
static inline void write_u32(StateStream *stream, u32 val) {
  stream->write_bytes(stream, &val, sizeof(u32));
}

static inline void write_u16(StateStream *stream, u16 val) {
  stream->write_bytes(stream, &val, sizeof(u16));
}

static inline void write_u8(StateStream *stream, u8 val) {
  stream->write_bytes(stream, &val, sizeof(u8));
}

static inline void write_f32(StateStream *stream, f32 val) {
  stream->write_bytes(stream, &val, sizeof(f32));
}

static inline void write_bytes(StateStream *stream, const void *src, u32 size) {
  stream->write_bytes(stream, src, size);
}

static inline u32 read_u32(StateStream *stream) {
  u32 val;
  stream->read_bytes(stream, &val, sizeof(u32));
  return val;
}

static inline u16 read_u16(StateStream *stream) {
  u16 val;
  stream->read_bytes(stream, &val, sizeof(u16));
  return val;
}

static inline u8 read_u8(StateStream *stream) {
  u8 val;
  stream->read_bytes(stream, &val, sizeof(u8));
  return val;
}

static inline f32 read_f32(StateStream *stream) {
  f32 val;
  stream->read_bytes(stream, &val, sizeof(f32));
  return val;
}

static inline void read_bytes(StateStream *stream, void *dst, u32 size) {
  stream->read_bytes(stream, dst, size);
}

static inline void stream_seek(StateStream *stream, u32 absolute_offset) {
  stream->seek(stream, absolute_offset);
}

static inline void stream_flush(StateStream *stream) { stream->flush(stream); }

#endif // PRACTICE_STATES_STREAM_H
