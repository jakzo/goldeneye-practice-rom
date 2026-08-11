#ifndef PRACTICE_SD_CARD_H
#define PRACTICE_SD_CARD_H

#include <bondtypes.h>
#include <ultra64.h>

bool practice_sd_card_is_available(void);
bool practice_sd_card_begin_write(const char *level_name,
                                  const char *difficulty_name,
                                  s32 max_save_states);
bool practice_sd_card_finish_write(bool success);
bool practice_sd_card_begin_read(const char *level_name,
                                 const char *difficulty_name);
void practice_sd_card_finish_read(void);
bool practice_sd_card_seek(u32 offset);
bool practice_sd_card_write(const void *data, u32 size);
bool practice_sd_card_read(void *data, u32 size);

#endif /* PRACTICE_SD_CARD_H */
