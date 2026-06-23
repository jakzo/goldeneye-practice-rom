#ifndef EMU_LOG_H
#define EMU_LOG_H

#include <ultra64.h>

/**
 * Initialize logging via USB (real hardware) or ISViewer (emulators).
 *
 * Detection order:
 *   1. USB: probes for SummerCart64, 64Drive, or EverDrive by checking
 *      their hardware registers. If found, logs are sent via USB using
 *      the UNFLoader protocol (compatible with UNFLoader, g64drive, etc.)
 *   2. ISViewer: if no USB cart is detected, probes for an emulated
 *      ISViewer (cen64 -is-viewer, ares, simple64, dgb-n64).
 *
 * If neither is available, subsequent emu_log_write calls are no-ops.
 * Both channels can be active simultaneously (USB on hardware + ISViewer
 * in emulators — whichever is detected).
 */
void emu_log_init(void);

/**
 * Write a null-terminated string to the log output.
 * The entire string is flushed in one shot.
 * Safe to call before emu_log_init() — will auto-init.
 * Safe to call with NULL or empty string.
 */
void emu_log_write(const char *text);

/**
 * Write a formatted string to the log output.
 * Uses a 512-byte internal buffer and forwards to emu_log_write.
 * Safe to call before emu_log_init() — will auto-init.
 */
void emu_log(const char *fmt, ...);

#endif /* EMU_LOG_H */
