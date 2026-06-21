/**
 * emu_log.c
 *
 * Implements emulator logging via the ISViewer protocol, which is supported
 * by several N64 emulators (cen64, ares, simple64, dgb-n64).
 *
 * ISViewer memory layout (physical addresses):
 *   0x13FF0000 - Magic register (presence check)
 *   0x13FF0014 - Write pointer / length register
 *   0x13FF0020 - Circular buffer
 *
 * The implementation mirrors libdragon's approach: data is written in 32-bit
 * big-endian chunks to the buffer region, then flushed by writing the byte
 * count to the write pointer register. Emulators intercept these writes and
 * display the output in a debug console.
 */

#include "emu_log.h"
#include <PR/rcp.h> // IO_WRITE, IO_READ, PHYS_TO_K1
#include <ultra64.h>

/* ------------------------------------------------------------------ */
/* ISViewer physical addresses (as seen by the RCP / PI bus)          */
/* ------------------------------------------------------------------ */
#define ISVIEWER_PHYS_MAGIC 0x13FF0000
#define ISVIEWER_PHYS_WRITE_PTR 0x13FF0014
#define ISVIEWER_PHYS_BUFFER 0x13FF0020
#define ISVIEWER_BUFFER_LEN 512 /* typical emulated buffer size */

/* KSEG1 (uncached) virtual addresses */
#define ISVIEWER_MAGIC PHYS_TO_K1(ISVIEWER_PHYS_MAGIC)
#define ISVIEWER_WRITE_PTR PHYS_TO_K1(ISVIEWER_PHYS_WRITE_PTR)
#define ISVIEWER_BUFFER PHYS_TO_K1(ISVIEWER_PHYS_BUFFER)

/* Magic value written to ISVIEWER_PHYS_MAGIC to detect presence.
 * Using a value different from the real ISViewer's default to avoid
 * false positives on real hardware that happens to have that value
 * sitting in memory. */
#define ISVIEWER_MAGIC_VALUE 0x12345678

/* ------------------------------------------------------------------ */
/* State                                                              */
/* ------------------------------------------------------------------ */
static int g_emu_log_initialized = 0;
static int g_emu_log_available = 0;

/* ------------------------------------------------------------------ */
/* Probe ISViewer                                                     */
/* ------------------------------------------------------------------ */
static int isviewer_probe(void) {
  u32 test;

  /*
   * iQue (iQue Player / 64DD emulation) doesn't have the PI
   * address space mapped this way.  We could check sys_bbplayer()
   * if it were available, but this project doesn't have that
   * function.  We just let the probe fail gracefully.
   */

  /* Write inverted magic and verify it sticks */
  IO_WRITE(ISVIEWER_PHYS_MAGIC, ~ISVIEWER_MAGIC_VALUE);
  test = IO_READ(ISVIEWER_PHYS_MAGIC);
  if (test != ~ISVIEWER_MAGIC_VALUE)
    return 0;

  /* Write real magic and verify */
  IO_WRITE(ISVIEWER_PHYS_MAGIC, ISVIEWER_MAGIC_VALUE);
  test = IO_READ(ISVIEWER_PHYS_MAGIC);
  if (test != ISVIEWER_MAGIC_VALUE)
    return 0;

  return 1;
}

/* ------------------------------------------------------------------ */
/* Public API                                                         */
/* ------------------------------------------------------------------ */
void emu_log_init(void) {
  if (g_emu_log_initialized)
    return;
  g_emu_log_initialized = 1;
  g_emu_log_available = isviewer_probe();
}

void emu_log_write(const char *text) {
  int len;
  int offset;

  if (!text || text[0] == '\0')
    return;

  /* Auto-init on first use */
  if (!g_emu_log_initialized)
    emu_log_init();

  if (!g_emu_log_available)
    return;

  len = 0;
  while (text[len] != '\0')
    len++;

  /*
   * Write data to the ISViewer buffer in 32-bit big-endian words.
   * This matches what emulators expect (N64 is big-endian).
   */
  offset = 0;
  while (len > 0) {
    int chunk = (len < ISVIEWER_BUFFER_LEN) ? len : ISVIEWER_BUFFER_LEN;
    int i;

    /* Write 32-bit big-endian words (N64 native endianness) */
    for (i = 0; i < chunk; i += 4) {
      u32 word = 0;
      int b;

      for (b = 0; b < 4 && (i + b) < chunk; b++)
        word |= ((u32)(unsigned char)text[offset + i + b]) << (24 - b * 8);

      IO_WRITE(ISVIEWER_PHYS_BUFFER + i, word);
    }

    /* Flush this chunk: write the length to the write-pointer register.
     * Emulators interpret this as a "data ready" signal. */
    IO_WRITE(ISVIEWER_PHYS_WRITE_PTR, chunk);

    offset += chunk;
    len -= chunk;
  }
}
