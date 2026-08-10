#include "practice_states_stream.h"
#include "../practice_ui.h"
#include <ultra64.h>

extern void *memcpy(void *dst, const void *src, size_t count);

static void memory_stream_write_bytes_impl(StateStream *stream,
                                           const void *src, u32 size) {
  StorageStream *storage = (StorageStream *)stream;
  StorageCursor cursor;
  storage_cursor_init(&cursor, storage->location, storage->current_page_addr);
  storage_write(&cursor, src, size);
  storage->error |= cursor.error;
  if (!cursor.error) {
    storage->current_page_addr += size;
    stream->total_processed += size;
  }
}

static void memory_stream_read_bytes_impl(StateStream *stream, void *dst,
                                          u32 size) {
  StorageStream *storage = (StorageStream *)stream;
  StorageCursor cursor;
  storage_cursor_init(&cursor, storage->location, storage->current_page_addr);
  storage_read(&cursor, dst, size);
  storage->error |= cursor.error;
  if (!cursor.error) {
    storage->current_page_addr += size;
    stream->total_processed += size;
  }
}

static void memory_stream_seek_impl(StateStream *stream, u32 absolute_offset) {
  StorageStream *storage = (StorageStream *)stream;
  storage->current_page_addr = absolute_offset;
  stream->total_processed = absolute_offset - stream->base_address;
}

static void memory_stream_flush_impl(StateStream *stream) { (void)stream; }

static void storage_stream_flush_impl(StateStream *stream) {
  StorageStream *storage = (StorageStream *)stream;
  if (storage->is_write && storage->is_dirty) {
    StorageCursor cursor;
    u32 capacity = storage_location_size(storage->location);
    u32 size = storage->location == PRACTICE_STORAGE_FLASHCART_SD
                   ? storage->page_offset
                   : STORAGE_PAGE_SIZE;

    if (storage->current_page_addr >= capacity) {
      storage->error = TRUE;
      return;
    }
    if (size > capacity - storage->current_page_addr) {
      size = capacity - storage->current_page_addr;
    }
    storage_cursor_init(&cursor, storage->location,
                        storage->current_page_addr);
    storage_write(&cursor, storage->page, size);
    storage->error |= cursor.error;
    storage->is_dirty = FALSE;
  }
}

static void storage_stream_write_bytes_impl(StateStream *stream,
                                            const void *src, u32 size) {
  StorageStream *storage = (StorageStream *)stream;
  u32 capacity = storage_location_size(storage->location);
  if (storage->current_page_addr > capacity ||
      storage->page_offset > capacity - storage->current_page_addr ||
      size > capacity - storage->current_page_addr - storage->page_offset) {
    practiceLogWarn("Storage write out of bounds: offset %d, size %d",
                    storage->current_page_addr + storage->page_offset, size);
    storage->error = TRUE;
    return;
  }
  const u8 *src_bytes = (const u8 *)src;
  u32 bytes_written = 0;
  while (bytes_written < size) {
    u32 space_left = STORAGE_PAGE_SIZE - storage->page_offset;
    u32 chunk_size = size - bytes_written;
    if (chunk_size > space_left) {
      chunk_size = space_left;
    }

    memcpy(storage->page + storage->page_offset, src_bytes + bytes_written,
           chunk_size);
    storage->page_offset += chunk_size;
    bytes_written += chunk_size;
    stream->total_processed += chunk_size;
    storage->is_dirty = TRUE;

    if (storage->page_offset == STORAGE_PAGE_SIZE) {
      storage_stream_flush_impl(stream);
      storage->current_page_addr += STORAGE_PAGE_SIZE;
      storage->page_offset = 0;

      // Load next page into memory in case we are doing a partial update
      if (bytes_written < size &&
          size - bytes_written < STORAGE_PAGE_SIZE) {
        StorageCursor cursor;
        storage_cursor_init(&cursor, storage->location,
                            storage->current_page_addr);
        storage_read(&cursor, storage->page, STORAGE_PAGE_SIZE);
        storage->error |= cursor.error;
      }
    }
  }
}

static void storage_stream_read_bytes_impl(StateStream *stream, void *dst,
                                           u32 size) {
  StorageStream *storage = (StorageStream *)stream;
  u32 capacity = storage_location_size(storage->location);
  if (storage->current_page_addr > capacity ||
      storage->page_offset > capacity - storage->current_page_addr ||
      size > capacity - storage->current_page_addr - storage->page_offset) {
    practiceLogWarn("Storage read out of bounds: offset %d, size %d",
                    storage->current_page_addr + storage->page_offset, size);
    storage->error = TRUE;
    return;
  }
  u8 *dst_bytes = (u8 *)dst;
  u32 bytes_read = 0;
  while (bytes_read < size) {
    u32 available = STORAGE_PAGE_SIZE - storage->page_offset;
    u32 chunk_size = size - bytes_read;
    if (chunk_size > available) {
      chunk_size = available;
    }

    memcpy(dst_bytes + bytes_read, storage->page + storage->page_offset,
           chunk_size);
    storage->page_offset += chunk_size;
    bytes_read += chunk_size;
    stream->total_processed += chunk_size;

    if (storage->page_offset == STORAGE_PAGE_SIZE) {
      StorageCursor cursor;
      storage->current_page_addr += STORAGE_PAGE_SIZE;
      if (storage->current_page_addr < capacity) {
        u32 page_size = STORAGE_PAGE_SIZE;
        if (page_size > capacity - storage->current_page_addr) {
          page_size = capacity - storage->current_page_addr;
        }
        storage_cursor_init(&cursor, storage->location,
                            storage->current_page_addr);
        storage_read(&cursor, storage->page, page_size);
        storage->error |= cursor.error;
      }
      storage->page_offset = 0;
    }
  }
}

static void storage_stream_seek_impl(StateStream *stream,
                                     u32 absolute_offset) {
  StorageStream *storage = (StorageStream *)stream;
  u32 intra_page = absolute_offset % STORAGE_PAGE_SIZE;
  u32 page_start = absolute_offset - intra_page;

  if (storage->current_page_addr == page_start) {
    storage->page_offset = intra_page;
  } else {
    StorageCursor cursor;
    // Evict current page if dirty
    storage_stream_flush_impl(stream);

    // Load the new page so partial edits don't corrupt the rest of it.
    storage->current_page_addr = page_start;
    storage_cursor_init(&cursor, storage->location, page_start);
    storage_read(&cursor, storage->page, STORAGE_PAGE_SIZE);
    storage->error |= cursor.error;
    storage->page_offset = intra_page;
  }
  stream->total_processed = absolute_offset - stream->base_address;
}

static void storage_stream_init(StorageStream *stream,
                                PracticeStorageLocation location,
                                u32 base_address, bool is_write) {
  StorageCursor cursor;

  stream->base.write_bytes = storage_stream_write_bytes_impl;
  stream->base.read_bytes = storage_stream_read_bytes_impl;
  stream->base.seek = storage_stream_seek_impl;
  stream->base.flush = storage_stream_flush_impl;
  stream->base.total_processed = 0;
  stream->base.base_address = base_address;

  stream->location = location;
  stream->current_page_addr = base_address;
  stream->page_offset = 0;
  stream->is_write = is_write;
  stream->is_dirty = FALSE;
  stream->error = !storage_location_is_available(location);

  if (location == PRACTICE_STORAGE_EXPANSION_RAM) {
    stream->base.write_bytes = memory_stream_write_bytes_impl;
    stream->base.read_bytes = memory_stream_read_bytes_impl;
    stream->base.seek = memory_stream_seek_impl;
    stream->base.flush = memory_stream_flush_impl;
    return;
  }

  if (!stream->error && (!is_write || base_address % STORAGE_PAGE_SIZE != 0)) {
    u32 capacity = storage_location_size(location);
    u32 size = STORAGE_PAGE_SIZE;
    if (size > capacity - base_address) {
      size = capacity - base_address;
    }
    storage_cursor_init(&cursor, location, base_address);
    storage_read(&cursor, stream->page, size);
    stream->error |= cursor.error;
  }
}

void storage_stream_init_write(StorageStream *stream,
                               PracticeStorageLocation location,
                               u32 base_address) {
  storage_stream_init(stream, location, base_address, TRUE);
}

void storage_stream_init_read(StorageStream *stream,
                              PracticeStorageLocation location,
                              u32 base_address) {
  storage_stream_init(stream, location, base_address, FALSE);
}
