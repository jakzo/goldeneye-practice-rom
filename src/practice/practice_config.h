#ifndef PRACTICE_CONFIG_H
#define PRACTICE_CONFIG_H

#include <ultra64.h>

#define PRACTICE_ROM_CONFIG_OFFSET 0x00FFFFC0
#define PRACTICE_ROM_CONFIG_SIZE 64
#define PRACTICE_ROM_CONFIG_MAGIC 0x47505243
#define PRACTICE_ROM_CONFIG_VERSION 1
#define PRACTICE_ROM_CONFIG_CHECKSUM_SALT 0xC0DEC0DE

struct PracticeRomConfig {
  u32 magic;
  u32 version;
  u32 size;
  u32 checksum;
  s32 test_case;
  s32 boot_level;
  u32 flags;
  u8 reserved[PRACTICE_ROM_CONFIG_SIZE - sizeof(u32) * 7];
};

typedef char PracticeRomConfigMustMatchPatchedSize
    [sizeof(struct PracticeRomConfig) == PRACTICE_ROM_CONFIG_SIZE ? 1 : -1];

/*
 * Persistent config is append-only. Add new options to the end of this struct.
 * Never delete or reorder an option; rename unused options to deprecated_*.
 */
struct PracticeConfig {
  s32 skip_logos_on_startup;
  s32 left_trigger_hotkeys;
  s32 boot_level;
  s32 disable_intro_cutscenes;
  f32 log_message_duration;
  s32 show_hundredths_on_timer;
  s32 show_mission_timer;
  s32 grenade_cam;
  s32 splits_enabled;
  s32 gate_guard_status;
  s32 dam_gate_intro_enabled;
  s32 log_splits;
  s32 speedometer_enabled;
  s32 lag_estimate_enabled;
  s32 replay_mode;
  s32 record_replay_seeds;
  s32 dam_guard_cam;
  s32 frigate_hostage_cam;
  s32 frigate_hostage_progress;
};

extern struct PracticeConfig practice;

void practice_config_load(void);
void practice_config_save(void);
void practice_config_menu_reset(void);
void practice_config_menu_tick(s32 stage_id, s32 is_objectives_page);
s32 practice_config_menu_scroll_offset(void);
s32 practice_config_menu_view_bottom(void);
void practice_config_menu_set_objectives_bottom(s32 y);
Gfx *practice_config_menu_render(Gfx *gdl, s32 stage_id);

#endif /* PRACTICE_CONFIG_H */
