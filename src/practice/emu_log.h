#ifndef EMU_LOG_H
#define EMU_LOG_H

#include <ultra64.h>

/**
 * Initialize emulator logging via ISViewer protocol.
 * Probes for ISViewer presence (emulated by cen64, ares, simple64, dgb-n64,
 * etc.) by testing a magic register for read/write.
 *
 * If ISViewer is not available, subsequent emu_log_write calls are no-ops.
 */
void emu_log_init(void);

/**
 * Write a null-terminated string to the emulator's ISViewer console.
 * The entire string is flushed in one shot.
 * Safe to call before emu_log_init() — will auto-init.
 * Safe to call with NULL or empty string.
 */
void emu_log_write(const char *text);

#endif /* EMU_LOG_H */
