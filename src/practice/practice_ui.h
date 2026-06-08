#ifndef PRACTICE_UI_H
#define PRACTICE_UI_H

#include <ultra64.h>

void practiceLogDebug(const char *fmt, ...);
void practiceLogInfo(const char *fmt, ...);
void practiceLogWarn(const char *fmt, ...);
void practiceLogError(const char *fmt, ...);
Gfx *practice_ui_render(Gfx *gdl);

#endif /* PRACTICE_UI_H */
