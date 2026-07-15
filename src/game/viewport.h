#ifndef _VIEWPORT_H_
#define _VIEWPORT_H_

#include <ultra64.h>

#include <PR/gbi.h>

Gfx *zbufClearCurrentPlayer(Gfx *gdl);
Gfx *zbufInit(Gfx *gdl);

void zbufAllocate(void);
void zbufDeallocate(void);
void zbufSetBuffer(s32 buffer, s32 width, s32 height);

extern s32 z_buffer;
extern s32 z_buffer_width;
extern s32 z_buffer_height;

#endif
