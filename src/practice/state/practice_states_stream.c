#include "practice_states_stream.h"
#include "../practice_ui.h"
#include <ultra64.h>

extern void *memcpy(void *dst, const void *src, size_t count);

static void memory_stream_write_bytes_impl(StateStream *stream,
                                           const void *src, u32 size) {
  MemoryStream *storage = (MemoryStream *)stream;
  StorageCursor cursor;
  if (storage->current_offset > storage->capacity ||
      size > storage->capacity - storage->current_offset) {
    storage->error = TRUE;
    return;
  }
  storage_cursor_init(&cursor, storage->location, storage->current_offset);
  storage_write(&cursor, src, size);
  storage->error |= cursor.error;
  if (!cursor.error) {
    storage->current_offset += size;
    stream->total_processed += size;
  }
}

static void memory_stream_read_bytes_impl(StateStream *stream, void *dst,
                                          u32 size) {
  MemoryStream *storage = (MemoryStream *)stream;
  StorageCursor cursor;
  if (storage->current_offset > storage->capacity ||
      size > storage->capacity - storage->current_offset) {
    storage->error = TRUE;
    return;
  }
  storage_cursor_init(&cursor, storage->location, storage->current_offset);
  storage_read(&cursor, dst, size);
  storage->error |= cursor.error;
  if (!cursor.error) {
    storage->current_offset += size;
    stream->total_processed += size;
  }
}

static void memory_stream_seek_impl(StateStream *stream, u32 absolute_offset) {
  MemoryStream *storage = (MemoryStream *)stream;
  if (absolute_offset < stream->base_address ||
      absolute_offset > storage->capacity) {
    storage->error = TRUE;
    return;
  }
  storage->current_offset = absolute_offset;
  stream->total_processed = absolute_offset - stream->base_address;
}

static void memory_stream_flush_impl(StateStream *stream) { (void)stream; }

static void storage_stream_flush_impl(StateStream *stream) {
  StorageStream *storage = (StorageStream *)stream;
  if (storage->is_write && storage->is_dirty) {
    StorageCursor cursor;
    u32 capacity = storage->capacity;
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
  u32 capacity = storage->capacity;
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

      /* Fixed-size backends need the untouched bytes around a partial update.
       * An SD save is a new sequential write-only file, and its flush writes
       * only page_offset bytes, so preloading would both be unnecessary and
       * attempt an invalid read from the write-only FatFs handle. */
      if (storage->location != PRACTICE_STORAGE_FLASHCART_SD &&
          bytes_written < size &&
          size - bytes_written < STORAGE_PAGE_SIZE) {
        StorageCursor cursor;
        u32 page_size = STORAGE_PAGE_SIZE;

        if (page_size > capacity - storage->current_page_addr) {
          page_size = capacity - storage->current_page_addr;
        }
        storage_cursor_init(&cursor, storage->location,
                            storage->current_page_addr);
        storage_read(&cursor, storage->page, page_size);
        storage->error |= cursor.error;
      }
    }
  }
}

static void storage_stream_read_bytes_impl(StateStream *stream, void *dst,
                                           u32 size) {
  StorageStream *storage = (StorageStream *)stream;
  u32 capacity = storage->capacity;
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
  u32 relative_offset;
  u32 intra_page;
  u32 page_start;

  if (absolute_offset < stream->base_address ||
      absolute_offset > storage->capacity) {
    storage->error = TRUE;
    return;
  }

  /* Buffered pages are based at base_address, which is 0x280 for SRAM save
   * states rather than a natural 1 KiB boundary. Keep seeks on that same grid;
   * changing the grid after a header/count patch can make a later flush
   * overwrite bytes which were already written on the original grid. */
  relative_offset = absolute_offset - stream->base_address;
  intra_page = relative_offset % STORAGE_PAGE_SIZE;
  page_start = absolute_offset - intra_page;

  if (storage->current_page_addr == page_start) {
    /* A write which ended exactly at a page boundary has advanced the address
     * but has not loaded that next page. Reload it before seeking into its
     * middle, or subsequent partial writes would flush stale page contents. */
    if (storage->is_write && !storage->is_dirty &&
        storage->page_offset == 0 && intra_page != 0) {
      StorageCursor cursor;
      u32 size = STORAGE_PAGE_SIZE;

      if (size > storage->capacity - page_start) {
        size = storage->capacity - page_start;
      }
      storage_cursor_init(&cursor, storage->location, page_start);
      storage_read(&cursor, storage->page, size);
      storage->error |= cursor.error;
    }
    storage->page_offset = intra_page;
  } else {
    StorageCursor cursor;
    u32 size = STORAGE_PAGE_SIZE;

    // Evict current page if dirty
    storage_stream_flush_impl(stream);

    // Load the new page so partial edits don't corrupt the rest of it.
    storage->current_page_addr = page_start;
    if (size > storage->capacity - page_start) {
      size = storage->capacity - page_start;
    }
    storage_cursor_init(&cursor, storage->location, page_start);
    storage_read(&cursor, storage->page, size);
    storage->error |= cursor.error;
    storage->page_offset = intra_page;
  }
  stream->total_processed = absolute_offset - stream->base_address;
}

static void storage_stream_init(StorageStream *stream,
                                PracticeStorageLocation location,
                                u32 base_address, u32 size, bool is_write) {
  StorageCursor cursor;
  u32 location_capacity = storage_location_size(location);

  stream->base.write_bytes = storage_stream_write_bytes_impl;
  stream->base.read_bytes = storage_stream_read_bytes_impl;
  stream->base.seek = storage_stream_seek_impl;
  stream->base.flush = storage_stream_flush_impl;
  stream->base.total_processed = 0;
  stream->base.base_address = base_address;

  stream->location = location;
  stream->capacity = location_capacity;
  stream->current_page_addr = base_address;
  stream->page_offset = 0;
  stream->is_write = is_write;
  stream->is_dirty = FALSE;
  stream->error = !storage_location_is_available(location);

  if (!is_write) {
    if (base_address > location_capacity ||
        size > location_capacity - base_address) {
      stream->error = TRUE;
    } else {
      stream->capacity = base_address + size;
    }
  }

  if (!stream->error && (!is_write || base_address % STORAGE_PAGE_SIZE != 0)) {
    u32 capacity = stream->capacity;
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
  storage_stream_init(stream, location, base_address, 0, TRUE);
}

void storage_stream_init_read(StorageStream *stream,
                              PracticeStorageLocation location,
                              u32 base_address, u32 size) {
  storage_stream_init(stream, location, base_address, size, FALSE);
}

static void memory_stream_init(MemoryStream *stream,
                               PracticeStorageLocation location,
                               u32 base_address, u32 size, bool is_write) {
  u32 capacity = storage_location_size(location);

  stream->base.write_bytes = memory_stream_write_bytes_impl;
  stream->base.read_bytes = memory_stream_read_bytes_impl;
  stream->base.seek = memory_stream_seek_impl;
  stream->base.flush = memory_stream_flush_impl;
  stream->base.total_processed = 0;
  stream->base.base_address = base_address;
  stream->location = location;
  stream->capacity = capacity;
  stream->current_offset = base_address;
  stream->error = !storage_location_is_available(location);

  if (!is_write) {
    if (base_address > capacity || size > capacity - base_address) {
      stream->error = TRUE;
    } else {
      stream->capacity = base_address + size;
    }
  }
}

void memory_stream_init_write(MemoryStream *stream,
                              PracticeStorageLocation location,
                              u32 base_address) {
  memory_stream_init(stream, location, base_address, 0, TRUE);
}

void memory_stream_init_read(MemoryStream *stream,
                             PracticeStorageLocation location,
                             u32 base_address, u32 size) {
  memory_stream_init(stream, location, base_address, size, FALSE);
}
