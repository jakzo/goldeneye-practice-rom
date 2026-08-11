// clang-format off
#include "practice_sd_card.h"
#include "practice_fatfs_config.h" /* Must precede FatFs headers. */
#include "practice_libdragon_compat.h"
#include "../../../libdragon/src/fatfs/ff.h"
#include "../../../libdragon/src/fatfs/diskio.h"
#include "../../../libdragon/src/libcart/cart.h"
#include "../practice_ui.h"
// clang-format on

extern int sprintf(char *dst, const char *fmt, ...);

#define SAVE_STATE_DIRECTORY "0:/GEPRAC/STATES"
#define SAVE_STATE_PARENT_DIRECTORY "0:/GEPRAC"
#define SAVE_STATE_PATH_SIZE 128
#define GENERATED_SAVE_STATE_PATH_SIZE 64
#define SAVE_STATE_LEVEL_NAME_SIZE 9
#define SAVE_STATE_DIFFICULTY_NAME_SIZE 4
#define SAVE_STATE_MAX_NUMBER 99999999

typedef struct SdSaveDirectory {
  s32 count;
  s32 highest;
} SdSaveDirectory;

static FATFS g_SdCardFat;
static FIL g_SdCardFile;
static bool g_SdCardMounted;
static bool g_SdCardFileOpen;
static bool g_SdCardFileWriting;
static char g_SdCardCurrentWriteLevel[SAVE_STATE_LEVEL_NAME_SIZE];
static char g_SdCardCurrentWriteDifficulty[SAVE_STATE_DIFFICULTY_NAME_SIZE];
static s32 g_SdCardCurrentWriteNumber;
static s32 g_SdCardCurrentWriteLimit;

static DSTATUS sd_card_disk_initialize(void) {
  return practice_cart_card_init() == 0 ? 0 : STA_NOINIT;
}

DSTATUS disk_initialize(BYTE pdrv) {
  return pdrv == 0 ? sd_card_disk_initialize() : STA_NOINIT;
}

DSTATUS disk_status(BYTE pdrv) { return pdrv == 0 ? 0 : STA_NOINIT; }

DRESULT disk_read(BYTE pdrv, BYTE *buff, LBA_t sector, UINT count) {
  if (pdrv != 0 || practice_cart_card_rd_dram(buff, sector, count) != 0) {
    return RES_ERROR;
  }
  return RES_OK;
}

DRESULT disk_write(BYTE pdrv, const BYTE *buff, LBA_t sector, UINT count) {
  if (pdrv != 0 || practice_cart_card_wr_dram(buff, sector, count) != 0) {
    return RES_ERROR;
  }
  return RES_OK;
}

DRESULT disk_ioctl(BYTE pdrv, BYTE cmd, void *buff) {
  (void)buff;
  if (pdrv == 0 && cmd == CTRL_SYNC) {
    return RES_OK;
  }
  return RES_PARERR;
}

DWORD get_fattime(void) {
  /* Save states do not need a real timestamp. */
  return (DWORD)((2026 - 1980) << 25 | 1 << 21 | 1 << 16);
}

static bool sd_card_mount(void) {
  /* Probe SC64 first. EverDrive detection writes to its device register
   * ranges, which can overlap storage emulated by another flashcart. */
  static const s32 supported_cart_types[] = {CART_SC, CART_EDX, CART_ED};
  FRESULT result;
  s32 i;

  if (g_SdCardMounted) {
    return TRUE;
  }

  for (i = 0;
       i < sizeof(supported_cart_types) / sizeof(supported_cart_types[0]);
       i++) {
    if (practice_cart_init(supported_cart_types[i]) >= 0) {
      break;
    }
  }
  if (i == sizeof(supported_cart_types) / sizeof(supported_cart_types[0])) {
    cart_type = CART_NULL;
    return FALSE;
  }

  result = f_mount(&g_SdCardFat, "", 1);
  if (result != FR_OK) {
    practiceLogWarn("Could not mount flashcart SD card (%d)", result);
    return FALSE;
  }

  g_SdCardMounted = TRUE;
  return TRUE;
}

static bool char_equals_ignore_case(char value, char expected) {
  if (value >= 'a' && value <= 'z') {
    value -= 'a' - 'A';
  }
  return value == expected;
}

static bool has_save_extension(const char *name) {
  s32 i = 0;

  while (name[i] != '\0' && name[i] != '.') {
    i++;
  }

  return name[i] == '.' && char_equals_ignore_case(name[i + 1], 'S') &&
         char_equals_ignore_case(name[i + 2], 'A') &&
         char_equals_ignore_case(name[i + 3], 'V') && name[i + 4] == '\0';
}

static s32 parse_save_number(const char *name) {
  s32 number = 0;
  s32 digits = 0;

  while (name[digits] >= '0' && name[digits] <= '9' && digits < 8) {
    number = number * 10 + name[digits] - '0';
    digits++;
  }

  if (digits == 0 || !has_save_extension(&name[digits])) {
    return -1;
  }
  return number;
}

static bool copy_path_component(char *destination, s32 destination_size,
                                const char *source) {
  s32 i;

  for (i = 0; i < destination_size - 1 && source[i] != '\0'; i++) {
    destination[i] = source[i];
  }
  destination[i] = '\0';
  return source[i] == '\0';
}

static void format_level_directory(char *path, const char *level_name) {
  sprintf(path, "%s/%s", SAVE_STATE_DIRECTORY, level_name);
}

static void format_save_directory(char *path, const char *level_name,
                                  const char *difficulty_name) {
  sprintf(path, "%s/%s/%s", SAVE_STATE_DIRECTORY, level_name, difficulty_name);
}

static void format_save_path(char *path, const char *level_name,
                             const char *difficulty_name, s32 number) {
  sprintf(path, "%s/%s/%s/%03d.SAV", SAVE_STATE_DIRECTORY, level_name,
          difficulty_name, number);
}

static bool scan_save_directory(const char *directory_path,
                                SdSaveDirectory *saves) {
  DIR directory;
  FILINFO info;
  FRESULT result;

  saves->count = 0;
  saves->highest = -1;
  result = f_opendir(&directory, directory_path);
  if (result == FR_NO_PATH) {
    return TRUE;
  }
  if (result != FR_OK) {
    practiceLogWarn("Could not list SD card save states (%d)", result);
    return FALSE;
  }

  while ((result = f_readdir(&directory, &info)) == FR_OK &&
         info.fname[0] != '\0') {
    s32 number;

    if (info.fattrib & AM_DIR) {
      continue;
    }
    number = parse_save_number(info.fname);
    if (number >= 0) {
      saves->count++;
      if (number > saves->highest) {
        saves->highest = number;
      }
    }
  }

  if (f_closedir(&directory) != FR_OK && result == FR_OK) {
    result = FR_DISK_ERR;
  }
  if (result != FR_OK) {
    practiceLogWarn("Could not finish listing SD card save states (%d)",
                    result);
    return FALSE;
  }
  return TRUE;
}

static bool find_save_path(const char *directory_path, char *path) {
  DIR directory;
  FILINFO info;
  FRESULT result;
  s32 highest_number = -1;
  bool found_save = FALSE;
  bool found_numbered_save = FALSE;

  result = f_opendir(&directory, directory_path);
  if (result != FR_OK) {
    return FALSE;
  }
  while ((result = f_readdir(&directory, &info)) == FR_OK &&
         info.fname[0] != '\0') {
    s32 number;

    if ((info.fattrib & AM_DIR) || !has_save_extension(info.fname)) {
      continue;
    }

    number = parse_save_number(info.fname);
    if (number >= 0) {
      if (!found_numbered_save || number > highest_number) {
        sprintf(path, "%s/%s", directory_path, info.fname);
        highest_number = number;
      }
      found_numbered_save = TRUE;
      found_save = TRUE;
    } else if (!found_save) {
      sprintf(path, "%s/%s", directory_path, info.fname);
      found_save = TRUE;
    }
  }
  if (f_closedir(&directory) != FR_OK) {
    return FALSE;
  }
  return result == FR_OK && found_save;
}

static bool delete_lowest_numbered_save(const char *directory_path) {
  DIR directory;
  FILINFO info;
  FRESULT result;
  char lowest_name[13];
  s32 lowest_number = -1;

  result = f_opendir(&directory, directory_path);
  if (result != FR_OK) {
    return FALSE;
  }
  while ((result = f_readdir(&directory, &info)) == FR_OK &&
         info.fname[0] != '\0') {
    s32 number;

    if (info.fattrib & AM_DIR) {
      continue;
    }
    number = parse_save_number(info.fname);
    if (number >= 0 && (lowest_number < 0 || number < lowest_number)) {
      sprintf(lowest_name, "%s", info.fname);
      lowest_number = number;
    }
  }
  if (f_closedir(&directory) != FR_OK && result == FR_OK) {
    result = FR_DISK_ERR;
  }
  if (result != FR_OK) {
    return FALSE;
  }
  if (lowest_number < 0) {
    return FALSE;
  }

  {
    char path[SAVE_STATE_PATH_SIZE];
    sprintf(path, "%s/%s", directory_path, lowest_name);
    return f_unlink(path) == FR_OK;
  }
}

static bool trim_numbered_saves(const char *directory_path, s32 limit) {
  SdSaveDirectory saves;

  if (!scan_save_directory(directory_path, &saves)) {
    return FALSE;
  }
  while (saves.count > limit) {
    if (!delete_lowest_numbered_save(directory_path)) {
      practiceLogWarn("Could not delete oldest SD card save state");
      return FALSE;
    }
    saves.count--;
  }
  return TRUE;
}

bool practice_sd_card_is_available(void) { return sd_card_mount(); }

bool practice_sd_card_begin_write(const char *level_name,
                                  const char *difficulty_name,
                                  s32 max_save_states) {
  SdSaveDirectory saves;
  char level_directory_path[GENERATED_SAVE_STATE_PATH_SIZE];
  char directory_path[GENERATED_SAVE_STATE_PATH_SIZE];
  char path[GENERATED_SAVE_STATE_PATH_SIZE];
  FRESULT result;

  if (!sd_card_mount() || g_SdCardFileOpen) {
    return FALSE;
  }

  if (!copy_path_component(g_SdCardCurrentWriteLevel,
                           SAVE_STATE_LEVEL_NAME_SIZE, level_name)) {
    practiceLogWarn("SD card save level name is too long: %s", level_name);
    return FALSE;
  }
  if (!copy_path_component(g_SdCardCurrentWriteDifficulty,
                           SAVE_STATE_DIFFICULTY_NAME_SIZE, difficulty_name)) {
    practiceLogWarn("SD card save difficulty name is too long: %s",
                    difficulty_name);
    return FALSE;
  }

  result = f_mkdir(SAVE_STATE_PARENT_DIRECTORY);
  if (result != FR_OK && result != FR_EXIST) {
    practiceLogWarn("Could not create SD card save directory (%d)", result);
    return FALSE;
  }
  result = f_mkdir(SAVE_STATE_DIRECTORY);
  if (result != FR_OK && result != FR_EXIST) {
    practiceLogWarn("Could not create SD card save directory (%d)", result);
    return FALSE;
  }
  format_level_directory(level_directory_path, level_name);
  result = f_mkdir(level_directory_path);
  if (result != FR_OK && result != FR_EXIST) {
    practiceLogWarn("Could not create SD card level save directory (%d)",
                    result);
    return FALSE;
  }
  format_save_directory(directory_path, level_name, difficulty_name);
  result = f_mkdir(directory_path);
  if (result != FR_OK && result != FR_EXIST) {
    practiceLogWarn("Could not create SD card difficulty save directory (%d)",
                    result);
    return FALSE;
  }

  if (!scan_save_directory(directory_path, &saves)) {
    return FALSE;
  }
  if (max_save_states < 1) {
    max_save_states = 1;
  } else if (max_save_states > 99) {
    max_save_states = 99;
  }
  if (saves.highest >= SAVE_STATE_MAX_NUMBER) {
    practiceLogWarn("SD card save state number limit reached");
    return FALSE;
  }

  g_SdCardCurrentWriteNumber = saves.highest + 1;
  g_SdCardCurrentWriteLimit = max_save_states;
  format_save_path(path, level_name, difficulty_name,
                   g_SdCardCurrentWriteNumber);
  /* The state serializer seeks backward to patch counts and offsets. Its
   * paged stream must read the already-written page before changing only part
   * of it, so a newly created save needs read as well as write access. */
  result = f_open(&g_SdCardFile, path, FA_READ | FA_WRITE | FA_CREATE_NEW);
  if (result != FR_OK) {
    practiceLogWarn("Could not create SD card save state (%d): %s", result,
                    path);
    return FALSE;
  }

  g_SdCardFileOpen = TRUE;
  g_SdCardFileWriting = TRUE;
  return TRUE;
}

bool practice_sd_card_finish_write(bool success) {
  char directory_path[GENERATED_SAVE_STATE_PATH_SIZE];
  char path[SAVE_STATE_PATH_SIZE];
  FRESULT close_result;

  if (!g_SdCardFileOpen || !g_SdCardFileWriting) {
    return FALSE;
  }

  close_result = f_close(&g_SdCardFile);
  g_SdCardFileOpen = FALSE;
  g_SdCardFileWriting = FALSE;
  if (!success || close_result != FR_OK) {
    format_save_path(path, g_SdCardCurrentWriteLevel,
                     g_SdCardCurrentWriteDifficulty,
                     g_SdCardCurrentWriteNumber);
    f_unlink(path);
  }
  if (close_result != FR_OK) {
    practiceLogWarn("Could not close SD card save state (%d)", close_result);
    return FALSE;
  }
  if (!success) {
    return FALSE;
  }

  format_save_directory(directory_path, g_SdCardCurrentWriteLevel,
                        g_SdCardCurrentWriteDifficulty);
  return trim_numbered_saves(directory_path, g_SdCardCurrentWriteLimit);
}

bool practice_sd_card_begin_read(const char *level_name,
                                 const char *difficulty_name) {
  char directory_path[GENERATED_SAVE_STATE_PATH_SIZE];
  char path[SAVE_STATE_PATH_SIZE];
  FRESULT result;

  if (!sd_card_mount() || g_SdCardFileOpen) {
    return FALSE;
  }

  format_save_directory(directory_path, level_name, difficulty_name);
  if (!find_save_path(directory_path, path)) {
    return FALSE;
  }
  result = f_open(&g_SdCardFile, path, FA_READ);
  if (result != FR_OK) {
    practiceLogWarn("Could not open SD card save state (%d)", result);
    return FALSE;
  }

  g_SdCardFileOpen = TRUE;
  g_SdCardFileWriting = FALSE;
  return TRUE;
}

void practice_sd_card_finish_read(void) {
  if (!g_SdCardFileOpen || g_SdCardFileWriting) {
    return;
  }
  f_close(&g_SdCardFile);
  g_SdCardFileOpen = FALSE;
}

bool practice_sd_card_seek(u32 offset) {
  FRESULT result;
  if (!g_SdCardFileOpen) {
    return FALSE;
  }
  result = f_lseek(&g_SdCardFile, offset);
  if (result != FR_OK) {
    practiceLogWarn("Could not seek SD card save state (%d, offset %d)", result,
                    offset);
  }
  return result == FR_OK;
}

bool practice_sd_card_write(const void *data, u32 size) {
  FRESULT result;
  UINT written;
  if (!g_SdCardFileOpen || !g_SdCardFileWriting) {
    return FALSE;
  }
  result = f_write(&g_SdCardFile, data, size, &written);
  if (result != FR_OK || written != size) {
    practiceLogWarn("Could not write SD card save state (%d, %d/%d)", result,
                    written, size);
  }
  return result == FR_OK && written == size;
}

bool practice_sd_card_read(void *data, u32 size) {
  FRESULT result;
  UINT read;
  u32 i;
  if (!g_SdCardFileOpen) {
    return FALSE;
  }
  result = f_read(&g_SdCardFile, data, size, &read);

  /* A serializer backpatch can revisit the current final, partially written
   * page. FatFs correctly returns only the bytes up to EOF; the unwritten
   * remainder of a new save file is logically zero. Normal save-state loads
   * must still reject truncated files. */
  if (result == FR_OK && g_SdCardFileWriting && read < size) {
    for (i = read; i < size; i++) {
      ((u8 *)data)[i] = 0;
    }
    read = size;
  }

  if (result != FR_OK || read != size) {
    practiceLogWarn("Could not read SD card save state (%d, %d/%d)", result,
                    read, size);
  }
  return result == FR_OK && read == size;
}
