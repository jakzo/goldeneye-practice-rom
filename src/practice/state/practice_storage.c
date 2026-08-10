#include "practice_storage.h"
#include "practice_sd_card.h"
#include "../practice_sram.h"
#include "../practice_ui.h"
#include "emu_log.h"

extern void *memcpy(void *dst, const void *src, size_t count);

#ifdef SRAM_CONSISTENCY_CHECK
static u8 debug_data[20000];
static u8 debug_written[20000];
#endif

/**
 * Use an uncached KSEG1 window in the upper half of Expansion Pak memory. The
 * original game does not allocate from this region, and direct uncached copies
 * avoid needing cache maintenance for the volatile backend.
 */
static u8 *get_expansion_storage_start(void) {
  return (u8 *)0xa0600000;
}

bool storage_location_is_available(PracticeStorageLocation location) {
  if (location == PRACTICE_STORAGE_SRAM) {
    return TRUE;
  }
  if (location == PRACTICE_STORAGE_EXPANSION_RAM) {
    return osMemSize >= 8 * 1024 * 1024;
  }
  if (location == PRACTICE_STORAGE_FLASHCART_SD) {
    return practice_sd_card_is_available();
  }
  return FALSE;
}

u32 storage_location_size(PracticeStorageLocation location) {
  if (location == PRACTICE_STORAGE_SRAM) {
    return SRAM_SIZE_BYTES;
  }
  if (location == PRACTICE_STORAGE_EXPANSION_RAM &&
      storage_location_is_available(location)) {
    return osMemSize - 0x00600000;
  }
  if (location == PRACTICE_STORAGE_FLASHCART_SD &&
      storage_location_is_available(location)) {
    return 0xffffffff;
  }
  return 0;
}

bool storage_begin_save(PracticeStorageLocation location,
                        const char *level_name, s32 max_save_states) {
  if (location == PRACTICE_STORAGE_FLASHCART_SD) {
    return practice_sd_card_begin_write(level_name, max_save_states);
  }
  return storage_location_is_available(location);
}

bool storage_finish_save(PracticeStorageLocation location, bool success) {
  if (location == PRACTICE_STORAGE_FLASHCART_SD) {
    return practice_sd_card_finish_write(success);
  }
  return success;
}

bool storage_begin_load(PracticeStorageLocation location) {
  if (location == PRACTICE_STORAGE_FLASHCART_SD) {
    return practice_sd_card_begin_read();
  }
  return storage_location_is_available(location);
}

void storage_finish_load(PracticeStorageLocation location) {
  if (location == PRACTICE_STORAGE_FLASHCART_SD) {
    practice_sd_card_finish_read();
  }
}

void storage_cursor_init(StorageCursor *cur, PracticeStorageLocation location,
                         u32 base_offset) {
  cur->location = location;
  cur->offset = base_offset;
  cur->error = FALSE;
}

void storage_write(StorageCursor *cur, const void *data, u32 size) {
  u32 capacity = storage_location_size(cur->location);

  if (cur->offset > capacity || size > capacity - cur->offset) {
    practiceLogWarn("Storage write out of bounds: offset %d, size %d",
                    cur->offset, size);
    cur->error = TRUE;
    return;
  }

  if (cur->location == PRACTICE_STORAGE_SRAM) {
    /* sram_write's dramAddr is non-const because the PI DMA helper is
       bidirectional; the buffer is never modified for writes. */
    if (sram_write(cur->offset, (void *)data, size) != 0) {
      cur->error = TRUE;
      return;
    }
  } else if (cur->location == PRACTICE_STORAGE_EXPANSION_RAM) {
    memcpy(get_expansion_storage_start() + cur->offset, data, size);
  } else if (!practice_sd_card_seek(cur->offset) ||
             !practice_sd_card_write(data, size)) {
    cur->error = TRUE;
    return;
  }
  cur->offset += size;

#ifdef SRAM_CONSISTENCY_CHECK
  // Save data in memory to compare when reading
  if (cur->location == PRACTICE_STORAGE_SRAM &&
      cur->offset <= sizeof(debug_data)) {
    memcpy(&debug_data[cur->offset - size], data, size);

    // Mark bytes as written in this session
    u32 idx;
    u32 off = cur->offset - size;
    for (idx = 0; idx < size; idx++) {
      debug_written[off + idx] = 1;
    }
  }
#endif
}

void storage_read(StorageCursor *cur, void *data, u32 size) {
  u32 capacity = storage_location_size(cur->location);

  if (cur->offset > capacity || size > capacity - cur->offset) {
    practiceLogWarn("Storage read out of bounds: offset %d, size %d",
                    cur->offset, size);
    cur->error = TRUE;
    return;
  }

  if (cur->location == PRACTICE_STORAGE_SRAM) {
    if (sram_read(cur->offset, data, size) != 0) {
      cur->error = TRUE;
      return;
    }
  } else if (cur->location == PRACTICE_STORAGE_EXPANSION_RAM) {
    memcpy(data, get_expansion_storage_start() + cur->offset, size);
  } else if (!practice_sd_card_seek(cur->offset) ||
             !practice_sd_card_read(data, size)) {
    cur->error = TRUE;
    return;
  }
  cur->offset += size;

#ifdef SRAM_CONSISTENCY_CHECK
  // Check consistency of storage against data saved in memory
  if (cur->location == PRACTICE_STORAGE_SRAM &&
      cur->offset <= sizeof(debug_data)) {
    u32 i;
    u32 m = cur->offset - size;
    for (i = 0; i < size; i++) {
      if (debug_written[m + i]) {
        u8 a = debug_data[m + i];
        u8 b = ((u8 *)data)[i];
        if (a != b) {
          u32 j = (i >= 3) ? (i - 3) : 0;
          u32 e = (i + 3 < size) ? (i + 3) : (size - 1);
          for (; j <= e; j++) {
            u8 x = debug_data[m + j];
            u8 y = ((u8 *)data)[j];
            emu_log("%04x [%04d]: %02x %02x%s", m + j, j, x, y,
                    x == y ? "" : " <--- BAD");
          }
        }
      }
    }
  }
#endif
}
