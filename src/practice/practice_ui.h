#ifndef PRACTICE_UI_H
#define PRACTICE_UI_H

#include <ultra64.h>

void practiceLogDebug(const char *fmt, ...);
void practiceLogInfo(const char *fmt, ...);
void practiceLogWarn(const char *fmt, ...);
void practiceLogError(const char *fmt, ...);

// Draws one offset shadow and one foreground copy. This is the practice-ROM
// replacement for textRenderGlow, which emits nine copies of the text.
Gfx *practice_render_text_with_shadow(Gfx *gdl, s32 *x, s32 *y, s8 *text,
                                      s32 font_chars, s32 font,
                                      s32 text_color, u32 shadow_color,
                                      s16 view_x, s16 view_y, s32 arg_a,
                                      s32 arg_b);

// Renders a text pill in the top-right of the screen, stacked below any pills
// already rendered this frame. Colors are 0xRRGGBBAA words; the alpha byte of
// pill_color controls the background opacity. Must be called during the
// practice_ui_render pass (which resets the stack each frame).
Gfx *practice_ui_render_pill(Gfx *gdl, const char *text, u32 text_color,
                             u32 pill_color);

Gfx *practice_ui_render(Gfx *gdl);

#endif /* PRACTICE_UI_H */
