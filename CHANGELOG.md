### Unreleased

# 0.5.0

## Practice tools and settings

- Added an optional speedometer to the in-level HUD
- Added settings for the on-screen timer, timer hundredths, intro cutscenes and split notifications
- Added an option to include or exclude Dam's gate intro cutscene from the pool of possible cutscenes
- Added indicators to the level settings menu showing which level and difficulty combinations have splits
- Improved the settings menu layout, scrolling and grouping of level and global options

## Save states

- Save states now restore substantially more gameplay state, including:
    - Mission objectives, alarms, background AI, dialogue, countdown timers, toxic gas and level exits
    - Music playback and fades, sky state, watch state and random number generator state
    - Bullet impacts, tracers, muzzle flashes and blood patches
    - Guard weapons, hats and concealed items
    - Thrown and embedded objects, including items collected or destroyed after saving
- Save states can now recreate missing guards and props, remove props created after saving and restore distant props correctly
- Fixed crashes and corrupt state when loading after restarting a level or resetting the console
- Fixed several save-state issues involving player state, guard attachments, Archives keys, projectiles and prop-list bookkeeping
- Fixed save-state feedback sounds and warning messages for unsupported hats

## Time scale and pausing

- Pausing or slowing time now also handles weapon fire rate, sound effects, music and random number generation consistently
- Fixed the game attempting to catch up all elapsed time after unpausing

## Grenade camera

- Expanded the grenade camera to follow throwing knives, mines and other thrown gadgets as well as grenades
- Improved camera framing, visibility and rendering of small or distant thrown items
- Fixed grenade camera rendering corrupting the main view or changing gameplay visibility

## Stability

- Fixed startup, DMA and scheduler stack issues which could cause hangs or crashes

## Testing and development

- Added automated practice-ROM tests in CI, including emulator logs and failure videos
- Added a dedicated test level and regression tests for movement speed, save states, time scaling and other gameplay systems
- Improved the local test runner and reduced test-build times

# 0.4.0

- Save state support for NPCs, explosions/smoke, bullet/explosion marks and now adds/remove props on load
- Dam door guard load state indicator
- UI for selecting options when starting a level
- Fixed memory issues causing crash on load of some levels

# 0.3.0

- Added a splits system
    - Has some initial splits for Runway
- Fixed bug with sound being disabled for the save on first boot
- Save state support for doors and various prop types
- Switch level/difficulty in menu using L and R triggers

# 0.2.0

- Added mission timer display on screen during level
- Added hundredths to the mission end screen timer
- Rewriting state saving system to be more thorough
- Persistent save states (transitioned save data storage from EEPROM to SRAM)

# 0.1.0

Initial version. Not super useful yet since the main features are not finished yet but here it is. Adds features:

- [alpha] **Save states**
    - During gameplay press `L + D-Down` to save current state
    - During gameplay press `L + D-Up` to load current state
    - Note that many bits of state do not yet get restored, like killed guards and destroyed props
- [alpha] **Grenade camera**
    - When throwing grenades, shows a picture-in-picture view that follows the grenade so you can see where it landed
- **Skip level intro cutscenes**
    - Starting a level goes straight into it, rather than playing the intro cutscene
- **Skips legal and logo screens on game startup**
- [beta] **Time scale control**
    - During gameplay press `L + D-Left` to decrease game speed by 10%
    - During gameplay press `L + D-Right` to increase game speed by 10%
- **Save automatically fully unlocked**
    - On booting the ROM for the first time the default save will have all levels and cheats unlocked
