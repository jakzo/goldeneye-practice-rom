#ifndef PRACTICE_STORAGE_H
#define PRACTICE_STORAGE_H

#include <bondtypes.h>
#include <ultra64.h>

/**
 * Storage abstraction layer for save state persistence.
 *
 * Backed by cartridge SRAM, the extra 4 MiB supplied by an Expansion Pak, or a
 * save-state file on a supported flashcart SD card. Expansion RAM is volatile
 * and is only available when osMemSize reports the full 8 MiB of RDRAM.
 *
 * Data is read/written sequentially via a cursor that auto-advances.
 * The cursor tracks an absolute offset within the storage medium.
 */

typedef enum {
  PRACTICE_STORAGE_SRAM,
  PRACTICE_STORAGE_EXPANSION_RAM,
  PRACTICE_STORAGE_FLASHCART_SD
} PracticeStorageLocation;

typedef struct {
  PracticeStorageLocation location;
  u32 offset;
  bool error;
} StorageCursor;

bool storage_location_is_available(PracticeStorageLocation location);
u32 storage_location_size(PracticeStorageLocation location);
bool storage_begin_save(PracticeStorageLocation location,
                        const char *level_name, s32 max_save_states);
bool storage_finish_save(PracticeStorageLocation location, bool success);
bool storage_begin_load(PracticeStorageLocation location);
void storage_finish_load(PracticeStorageLocation location);

/**
 * Initialize a storage cursor at the given base offset.
 */
void storage_cursor_init(StorageCursor *cur, PracticeStorageLocation location,
                         u32 base_offset);

/**
 * Write `size` bytes from `data` to storage at the cursor's current
 * position, then advance the cursor.
 *
 * The source memory must be valid RDRAM (it is passed to the PI DMA
 * engine).  This means live game-state structs can be written directly
 * without an intermediate buffer — the DMA reads from RDRAM and writes
 * to the storage medium.
 */
void storage_write(StorageCursor *cur, const void *data, u32 size);

/**
 * Read `size` bytes from storage at the cursor's current position into
 * `data`, then advance the cursor.
 *
 * The destination memory must be valid RDRAM.  Live game-state structs
 * can be read into directly.
 */
void storage_read(StorageCursor *cur, void *data, u32 size);

#endif /* PRACTICE_STORAGE_H */
