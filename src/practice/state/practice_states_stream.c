#include "practice_states_stream.h"
#include "../practice_sram.h"
#include "../practice_ui.h"
#include <ultra64.h>

extern void *memcpy(void *dst, const void *src, size_t count);

static void sram_stream_flush_impl(StateStream *stream) {
  SramStream *sram = (SramStream *)stream;
  if (sram->is_write && sram->is_dirty) {
    sram_write(sram->current_page_addr, sram->page, SRAM_PAGE_SIZE);
    sram->is_dirty = FALSE;
  }
}

static void sram_stream_write_bytes_impl(StateStream *stream, const void *src,
                                         u32 size) {
  SramStream *sram = (SramStream *)stream;
  if (sram->current_page_addr + sram->page_offset + size > SRAM_SIZE_BYTES) {
    practiceLogDebug("SRAM write out of bounds: offset %d, size %d",
                     sram->current_page_addr + sram->page_offset, size);
    return;
  }
  const u8 *src_bytes = (const u8 *)src;
  u32 bytes_written = 0;
  while (bytes_written < size) {
    u32 space_left = SRAM_PAGE_SIZE - sram->page_offset;
    u32 chunk_size = size - bytes_written;
    if (chunk_size > space_left) {
      chunk_size = space_left;
    }

    memcpy(sram->page + sram->page_offset, src_bytes + bytes_written,
           chunk_size);
    sram->page_offset += chunk_size;
    bytes_written += chunk_size;
    stream->total_processed += chunk_size;
    sram->is_dirty = TRUE;

    if (sram->page_offset == SRAM_PAGE_SIZE) {
      sram_stream_flush_impl(stream);
      sram->current_page_addr += SRAM_PAGE_SIZE;
      sram->page_offset = 0;
    }
  }
}

static void sram_stream_read_bytes_impl(StateStream *stream, void *dst,
                                        u32 size) {
  SramStream *sram = (SramStream *)stream;
  if (sram->current_page_addr + sram->page_offset + size > SRAM_SIZE_BYTES) {
    practiceLogDebug("SRAM read out of bounds: offset %d, size %d",
                     sram->current_page_addr + sram->page_offset, size);
    return;
  }
  u8 *dst_bytes = (u8 *)dst;
  u32 bytes_read = 0;
  while (bytes_read < size) {
    u32 available = SRAM_PAGE_SIZE - sram->page_offset;
    u32 chunk_size = size - bytes_read;
    if (chunk_size > available) {
      chunk_size = available;
    }

    memcpy(dst_bytes + bytes_read, sram->page + sram->page_offset, chunk_size);
    sram->page_offset += chunk_size;
    bytes_read += chunk_size;
    stream->total_processed += chunk_size;

    if (sram->page_offset == SRAM_PAGE_SIZE) {
      sram->current_page_addr += SRAM_PAGE_SIZE;
      sram_read(sram->current_page_addr, sram->page, SRAM_PAGE_SIZE);
      sram->page_offset = 0;
    }
  }
}

static void sram_stream_seek_impl(StateStream *stream, u32 absolute_offset) {
  SramStream *sram = (SramStream *)stream;
  u32 intra_page = absolute_offset % SRAM_PAGE_SIZE;
  u32 page_start = absolute_offset - intra_page;

  if (sram->current_page_addr == page_start) {
    sram->page_offset = intra_page;
  } else {
    // Evict current page if dirty
    sram_stream_flush_impl(stream);

    // Load new page from SRAM (so partial edits in write mode don't corrupt the
    // rest of the page)
    sram->current_page_addr = page_start;
    sram_read(sram->current_page_addr, sram->page, SRAM_PAGE_SIZE);
    sram->page_offset = intra_page;
  }
  stream->total_processed = absolute_offset - stream->base_address;
}

void sram_stream_init_write(SramStream *stream, u32 sram_base) {
  stream->base.write_bytes = sram_stream_write_bytes_impl;
  stream->base.read_bytes = sram_stream_read_bytes_impl;
  stream->base.seek = sram_stream_seek_impl;
  stream->base.flush = sram_stream_flush_impl;
  stream->base.total_processed = 0;
  stream->base.base_address = sram_base;

  stream->current_page_addr = sram_base;
  stream->page_offset = 0;
  stream->is_write = TRUE;
  stream->is_dirty = FALSE;
}

void sram_stream_init_read(SramStream *stream, u32 sram_base) {
  stream->base.write_bytes = sram_stream_write_bytes_impl;
  stream->base.read_bytes = sram_stream_read_bytes_impl;
  stream->base.seek = sram_stream_seek_impl;
  stream->base.flush = sram_stream_flush_impl;
  stream->base.total_processed = 0;
  stream->base.base_address = sram_base;

  stream->current_page_addr = sram_base;
  stream->page_offset = 0;
  stream->is_write = FALSE;
  stream->is_dirty = FALSE;
  sram_read(stream->current_page_addr, stream->page, SRAM_PAGE_SIZE);
}
