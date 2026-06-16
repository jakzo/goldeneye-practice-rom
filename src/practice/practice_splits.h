#ifndef PRACTICE_SPLITS_H
#define PRACTICE_SPLITS_H

#include <ultra64.h>

/**
 * 2D vertex for a split quad (bird's eye view: x,z plane).
 * y is the vertical axis in GoldenEye, so splits use x and z.
 */
typedef struct {
  f32 x;
  f32 z;
} SplitVertex;

/**
 * A single split: a named quad region that triggers when Bond enters it.
 */
typedef struct {
  const char *name;
  SplitVertex vertices[4];
} SplitArea;

/**
 * Split list for a specific level+difficulty combo.
 * Entries are terminated by a NULL name sentinel.
 */
typedef struct {
  s32 level_id;
  s32 difficulty;
  const SplitArea *splits; // NULL-terminated array
} SplitList;

/**
 * Initialize the splits system. Call once at startup.
 */
void splits_init(void);

/**
 * Tick function - call every frame during gameplay.
 * Checks if Bond has entered the next split quad and logs the time.
 */
void splits_tick(void);

/**
 * Handle the splits hotkey (trigger + C-down).
 * Logs Bond's current position as a debug message.
 * Returns TRUE if the hotkey was consumed.
 */
s32 splits_handle_hotkey(void);

#endif /* PRACTICE_SPLITS_H */
