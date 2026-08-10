#include "practice_sd_card.h"
#include "practice_fatfs_config.h"
#include "practice_libdragon_compat.h"
#include "../../../libdragon/src/fatfs/ff.h"
#include "../../../libdragon/src/fatfs/diskio.h"
#include "../../../libdragon/src/libcart/cart.h"
#include "../practice_ui.h"

extern int sprintf(char *dst, const char *fmt, ...);

#define SAVE_STATE_DIRECTORY "0:/GEPRAC/STATES"
#define SAVE_STATE_PARENT_DIRECTORY "0:/GEPRAC"
#define SAVE_STATE_PATH_SIZE 128
#define GENERATED_SAVE_STATE_PATH_SIZE 64
#define SAVE_NUMBER_COUNT 1000
#define SAVE_NUMBER_MAP_SIZE ((SAVE_NUMBER_COUNT + 7) / 8)

typedef struct SdSaveDirectory {
  u8 present[SAVE_NUMBER_MAP_SIZE];
  s32 count;
  s32 oldest;
  s32 newest;
  s32 next;
} SdSaveDirectory;

static FATFS g_SdCardFat;
static FIL g_SdCardFile;
static bool g_SdCardMounted;
static bool g_SdCardFileOpen;
static bool g_SdCardFileWriting;
static u16 g_SdCardCurrentWriteNumber;

static DSTATUS sd_card_disk_initialize(void) {
  return practice_cart_card_init() == 0 ? 0 : STA_NOINIT;
}

DSTATUS disk_initialize(BYTE pdrv) {
  return pdrv == 0 ? sd_card_disk_initialize() : STA_NOINIT;
}

DSTATUS disk_status(BYTE pdrv) { return pdrv == 0 ? 0 : STA_NOINIT; }

DRESULT disk_read(BYTE pdrv, BYTE *buff, LBA_t sector, UINT count) {
  if (pdrv != 0 ||
      practice_cart_card_rd_dram(buff, sector, count) != 0) {
    return RES_ERROR;
  }
  return RES_OK;
}

DRESULT disk_write(BYTE pdrv, const BYTE *buff, LBA_t sector, UINT count) {
  if (pdrv != 0 ||
      practice_cart_card_wr_dram(buff, sector, count) != 0) {
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

static s32 parse_save_number(const char *name) {
  s32 a = name[0] - '0';
  s32 b = name[1] - '0';
  s32 c = name[2] - '0';
  if (a >= 0 && a <= 9 && b >= 0 && b <= 9 && c >= 0 && c <= 9 &&
      name[3] == '.') {
    return a * 100 + b * 10 + c;
  }
  return -1;
}

static bool save_number_is_present(const SdSaveDirectory *saves, s32 number) {
  return saves->present[number / 8] & (1 << (number % 8));
}

static void set_save_number_present(SdSaveDirectory *saves, s32 number) {
  u8 *slot = &saves->present[number / 8];
  u8 mask = 1 << (number % 8);
  if (!(*slot & mask)) {
    *slot |= mask;
    saves->count++;
  }
}

static void calculate_save_order(SdSaveDirectory *saves) {
  s32 number;

  if (saves->count == 0) {
    saves->oldest = -1;
    saves->newest = -1;
    saves->next = 0;
    return;
  }

  if (save_number_is_present(saves, 999)) {
    /* Once numbering wraps, the contiguous run ending at 999 is the old end
     * of the sequence. Its lowest member is therefore the oldest save. */
    saves->oldest = 999;
    while (saves->oldest > 0 &&
           save_number_is_present(saves, saves->oldest - 1)) {
      saves->oldest--;
    }

    if (saves->oldest == 0 || !save_number_is_present(saves, 0)) {
      saves->newest = 999;
    } else {
      saves->newest = 0;
      while (saves->newest < 998 &&
             save_number_is_present(saves, saves->newest + 1)) {
        saves->newest++;
      }
    }
  } else {
    saves->oldest = -1;
    saves->newest = -1;
    for (number = 0; number < SAVE_NUMBER_COUNT; number++) {
      if (save_number_is_present(saves, number)) {
        if (saves->oldest < 0) {
          saves->oldest = number;
        }
        saves->newest = number;
      }
    }
  }

  saves->next = (saves->newest + 1) % SAVE_NUMBER_COUNT;
}

static bool scan_save_directory(SdSaveDirectory *saves) {
  DIR directory;
  FILINFO info;
  FRESULT result;
  s32 i;

  for (i = 0; i < SAVE_NUMBER_MAP_SIZE; i++) {
    saves->present[i] = 0;
  }
  saves->count = 0;
  result = f_opendir(&directory, SAVE_STATE_DIRECTORY);
  if (result == FR_NO_PATH) {
    calculate_save_order(saves);
    return TRUE;
  }
  if (result != FR_OK) {
    practiceLogWarn("Could not list SD-card save states (%d)", result);
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
      set_save_number_present(saves, number);
    }
  }

  if (f_closedir(&directory) != FR_OK && result == FR_OK) {
    result = FR_DISK_ERR;
  }
  if (result != FR_OK) {
    practiceLogWarn("Could not finish listing SD-card save states (%d)",
                    result);
    return FALSE;
  }

  calculate_save_order(saves);
  return TRUE;
}

static bool find_save_path(s32 number, char *path) {
  DIR directory;
  FILINFO info;
  FRESULT result;
  bool found = FALSE;

  result = f_opendir(&directory, SAVE_STATE_DIRECTORY);
  if (result != FR_OK) {
    return FALSE;
  }
  while ((result = f_readdir(&directory, &info)) == FR_OK &&
         info.fname[0] != '\0') {
    if (!(info.fattrib & AM_DIR) && parse_save_number(info.fname) == number) {
      sprintf(path, "%s/%s", SAVE_STATE_DIRECTORY, info.fname);
      found = TRUE;
      break;
    }
  }
  if (f_closedir(&directory) != FR_OK) {
    return FALSE;
  }
  return result == FR_OK && found;
}

static bool delete_oldest_saves(const SdSaveDirectory *saves,
                                s32 delete_count) {
  u8 delete_numbers[SAVE_NUMBER_MAP_SIZE];
  DIR directory;
  FILINFO info;
  FRESULT result;
  s32 number = saves->oldest;
  s32 i;

  for (i = 0; i < SAVE_NUMBER_MAP_SIZE; i++) {
    delete_numbers[i] = 0;
  }
  for (i = 0; i < SAVE_NUMBER_COUNT && delete_count > 0; i++) {
    if (save_number_is_present(saves, number)) {
      delete_numbers[number / 8] |= 1 << (number % 8);
      delete_count--;
    }
    number = (number + 1) % SAVE_NUMBER_COUNT;
  }
  if (delete_count != 0) {
    return FALSE;
  }

  result = f_opendir(&directory, SAVE_STATE_DIRECTORY);
  if (result != FR_OK) {
    return FALSE;
  }
  while ((result = f_readdir(&directory, &info)) == FR_OK &&
         info.fname[0] != '\0') {
    char path[SAVE_STATE_PATH_SIZE];
    number = parse_save_number(info.fname);
    if (!(info.fattrib & AM_DIR) && number >= 0 &&
        (delete_numbers[number / 8] & (1 << (number % 8)))) {
      sprintf(path, "%s/%s", SAVE_STATE_DIRECTORY, info.fname);
      if (f_unlink(path) != FR_OK) {
        result = FR_DISK_ERR;
        break;
      }
    }
  }
  if (f_closedir(&directory) != FR_OK && result == FR_OK) {
    result = FR_DISK_ERR;
  }
  if (result != FR_OK) {
    practiceLogWarn("Could not delete oldest SD-card save state (%d)", result);
    return FALSE;
  }
  return TRUE;
}

bool practice_sd_card_is_available(void) { return sd_card_mount(); }

bool practice_sd_card_begin_write(const char *level_name, s32 max_save_states) {
  SdSaveDirectory saves;
  char path[GENERATED_SAVE_STATE_PATH_SIZE];
  FRESULT result;

  if (!sd_card_mount() || g_SdCardFileOpen) {
    return FALSE;
  }

  (void)level_name;

  result = f_mkdir(SAVE_STATE_PARENT_DIRECTORY);
  if (result != FR_OK && result != FR_EXIST) {
    practiceLogWarn("Could not create SD-card save directory (%d)", result);
    return FALSE;
  }
  result = f_mkdir(SAVE_STATE_DIRECTORY);
  if (result != FR_OK && result != FR_EXIST) {
    practiceLogWarn("Could not create SD-card save directory (%d)", result);
    return FALSE;
  }

  if (!scan_save_directory(&saves)) {
    return FALSE;
  }
  if (max_save_states < 1) {
    max_save_states = 1;
  } else if (max_save_states > 99) {
    max_save_states = 99;
  }
  if (saves.count >= max_save_states &&
      !delete_oldest_saves(&saves, saves.count - max_save_states + 1)) {
    return FALSE;
  }

  sprintf(path, "%s/%03d.SAV", SAVE_STATE_DIRECTORY, saves.next);
  /* The state serializer seeks backward to patch counts and offsets. Its
   * paged stream must read the already-written page before changing only part
   * of it, so a newly created save needs read as well as write access. */
  result =
      f_open(&g_SdCardFile, path, FA_READ | FA_WRITE | FA_CREATE_NEW);
  if (result != FR_OK) {
    practiceLogWarn("Could not create SD-card save state (%d): %s", result,
                    path);
    return FALSE;
  }

  g_SdCardCurrentWriteNumber = saves.next;
  g_SdCardFileOpen = TRUE;
  g_SdCardFileWriting = TRUE;
  return TRUE;
}

bool practice_sd_card_finish_write(bool success) {
  char path[SAVE_STATE_PATH_SIZE];
  FRESULT close_result;

  if (!g_SdCardFileOpen || !g_SdCardFileWriting) {
    return FALSE;
  }

  close_result = f_close(&g_SdCardFile);
  g_SdCardFileOpen = FALSE;
  g_SdCardFileWriting = FALSE;
  if (!success || close_result != FR_OK) {
    if (find_save_path(g_SdCardCurrentWriteNumber, path)) {
      f_unlink(path);
    }
  }
  if (close_result != FR_OK) {
    practiceLogWarn("Could not close SD-card save state (%d)", close_result);
    return FALSE;
  }
  return success;
}

bool practice_sd_card_begin_read(void) {
  SdSaveDirectory saves;
  char path[SAVE_STATE_PATH_SIZE];
  FRESULT result;

  if (!sd_card_mount() || g_SdCardFileOpen) {
    return FALSE;
  }

  if (!scan_save_directory(&saves)) {
    return FALSE;
  }
  if (saves.count == 0 || !find_save_path(saves.newest, path)) {
    return FALSE;
  }
  result = f_open(&g_SdCardFile, path, FA_READ);
  if (result != FR_OK) {
    practiceLogWarn("Could not open SD-card save state (%d)", result);
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
    practiceLogWarn("Could not seek SD-card save state (%d, offset %d)", result,
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
    practiceLogWarn("Could not write SD-card save state (%d, %d/%d)", result,
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
    practiceLogWarn("Could not read SD-card save state (%d, %d/%d)", result,
                    read, size);
  }
  return result == FR_OK && read == size;
}
