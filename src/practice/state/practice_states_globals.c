#include "practice_states_globals.h"
#include <ultra64.h>

#define BONDVIEW_HUD_MSG_BOTTOM_BUFFER_LENGTH 0x65

extern char stringbuffer_lowerleft[0x5][BONDVIEW_HUD_MSG_BOTTOM_BUFFER_LENGTH];
extern s32 status_bar_text_buffer_index;
extern s32 display_statusbar;

#if defined(VERSION_JP) || defined(VERSION_EU)
extern s32 dword_CODE_bss_jp80079CEC[0x05];
extern s32 dword_CODE_bss_jp80079Cd8[0x05];
#else
extern s32 copy_1stfonttable;
extern s32 copy_2ndfonttable;
#endif

typedef struct {
    s32 status_bar_text_buffer_index;
    s32 display_statusbar;
    char stringbuffer_lowerleft[5][BONDVIEW_HUD_MSG_BOTTOM_BUFFER_LENGTH];
#if defined(VERSION_JP) || defined(VERSION_EU)
    s32 dword_CODE_bss_jp80079CEC[5];
    s32 dword_CODE_bss_jp80079Cd8[5];
#else
    s32 copy_1stfonttable;
    s32 copy_2ndfonttable;
#endif
} SavedGlobals;

static SavedGlobals g_SavedGlobals;

void save_global_state(void) {
    s32 i, j;
    g_SavedGlobals.status_bar_text_buffer_index = status_bar_text_buffer_index;
    g_SavedGlobals.display_statusbar = display_statusbar;
    for (i = 0; i < 5; i++) {
        for (j = 0; j < BONDVIEW_HUD_MSG_BOTTOM_BUFFER_LENGTH; j++) {
            g_SavedGlobals.stringbuffer_lowerleft[i][j] = stringbuffer_lowerleft[i][j];
        }
    }
#if defined(VERSION_JP) || defined(VERSION_EU)
    for (i = 0; i < 5; i++) {
        g_SavedGlobals.dword_CODE_bss_jp80079CEC[i] = dword_CODE_bss_jp80079CEC[i];
        g_SavedGlobals.dword_CODE_bss_jp80079Cd8[i] = dword_CODE_bss_jp80079Cd8[i];
    }
#else
    g_SavedGlobals.copy_1stfonttable = copy_1stfonttable;
    g_SavedGlobals.copy_2ndfonttable = copy_2ndfonttable;
#endif
}

void load_global_state(void) {
    s32 i, j;
    status_bar_text_buffer_index = g_SavedGlobals.status_bar_text_buffer_index;
    display_statusbar = g_SavedGlobals.display_statusbar;
    for (i = 0; i < 5; i++) {
        for (j = 0; j < BONDVIEW_HUD_MSG_BOTTOM_BUFFER_LENGTH; j++) {
            stringbuffer_lowerleft[i][j] = g_SavedGlobals.stringbuffer_lowerleft[i][j];
        }
    }
#if defined(VERSION_JP) || defined(VERSION_EU)
    for (i = 0; i < 5; i++) {
        dword_CODE_bss_jp80079CEC[i] = g_SavedGlobals.dword_CODE_bss_jp80079CEC[i];
        dword_CODE_bss_jp80079Cd8[i] = g_SavedGlobals.dword_CODE_bss_jp80079Cd8[i];
    }
#else
    copy_1stfonttable = g_SavedGlobals.copy_1stfonttable;
    copy_2ndfonttable = g_SavedGlobals.copy_2ndfonttable;
#endif
}
