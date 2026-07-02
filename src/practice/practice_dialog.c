#include "practice_dialog.h"
#include "lvl_text.h"
#include <str.h>

#define BONDVIEW_HUD_MSG_TOP_BUFFER_LENGTH 0x97
#define PRACTICE_DIALOG_QUEUE_LENGTH 2
#define PRACTICE_DIALOG_INVALID_TEXT_ID 0xffff

#if defined(LEFTOVERDEBUG)
extern char
    stringbuffer_top[PRACTICE_DIALOG_QUEUE_LENGTH]
                    [BONDVIEW_HUD_MSG_TOP_BUFFER_LENGTH];
#define UPPER_TEXT_BUFFERS stringbuffer_top
#else
extern char
    dword_CODE_bss_80079DC8[PRACTICE_DIALOG_QUEUE_LENGTH]
                           [BONDVIEW_HUD_MSG_TOP_BUFFER_LENGTH];
#define UPPER_TEXT_BUFFERS dword_CODE_bss_80079DC8
#endif

static u16 g_PracticeDialogTextIds[PRACTICE_DIALOG_QUEUE_LENGTH] = {
    PRACTICE_DIALOG_INVALID_TEXT_ID, PRACTICE_DIALOG_INVALID_TEXT_ID};

void practice_dialog_reset(void) {
  s32 i;

  for (i = 0; i < PRACTICE_DIALOG_QUEUE_LENGTH; i++) {
    g_PracticeDialogTextIds[i] = PRACTICE_DIALOG_INVALID_TEXT_ID;
  }
}

void practice_dialog_enqueue(s32 buffer_index, u16 text_id) {
  if (buffer_index >= 0 && buffer_index < PRACTICE_DIALOG_QUEUE_LENGTH) {
    g_PracticeDialogTextIds[buffer_index] = text_id;
  }
}

void practice_dialog_remove(s32 buffer_index) {
  if (buffer_index >= 0 && buffer_index < PRACTICE_DIALOG_QUEUE_LENGTH) {
    g_PracticeDialogTextIds[buffer_index] = PRACTICE_DIALOG_INVALID_TEXT_ID;
  }
}

void practice_dialog_save(StateStream *stream) {
  s32 i;

  for (i = 0; i < PRACTICE_DIALOG_QUEUE_LENGTH; i++) {
    write_u16(stream, g_PracticeDialogTextIds[i]);
  }
}

void practice_dialog_load(StateStream *stream) {
  s32 i;

  for (i = 0; i < PRACTICE_DIALOG_QUEUE_LENGTH; i++) {
    u16 text_id = read_u16(stream);
    char *text = NULL;

    g_PracticeDialogTextIds[i] = text_id;

    if (text_id != PRACTICE_DIALOG_INVALID_TEXT_ID) {
      text = langGet(text_id);
    }

    if (text != NULL) {
      strncpy(UPPER_TEXT_BUFFERS[i], text,
              BONDVIEW_HUD_MSG_TOP_BUFFER_LENGTH - 1);
      UPPER_TEXT_BUFFERS[i][BONDVIEW_HUD_MSG_TOP_BUFFER_LENGTH - 1] = '\0';
    } else {
      UPPER_TEXT_BUFFERS[i][0] = '\0';
    }
  }
}
