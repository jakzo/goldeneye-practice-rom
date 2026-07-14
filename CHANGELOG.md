### Unreleased

# 0.6.1

## External cameras

- Added an optional third-person camera on Dam that follows the gate guard while Bond is near the gate area
- Added an optional third-person camera on Frigate that follows freed hostages until they escape or despawn
- Refactored the grenade camera into a shared external-camera system supporting up to three simultaneous picture-in-picture views
- External NPC cameras preserve normal off-screen movement so observing guards or hostages does not alter their gameplay behavior

## Frigate hostage progress

- Added an optional display that lists freed hostages in rescue order and shows their selected escape destination
- Added estimated route times and movement-state indicators, including whether each hostage is moving normally or using off-screen movement
- Added status reporting for hostages that reach a destination, fail to find or complete a route, die, or escape

## Interface and memory

- Added level-specific settings for the Dam guard camera, Frigate hostage camera and Frigate hostage progress display
- Disabled the existing Dam gate-guard status indicator by default
- Reclaimed TLB mapping-table memory for the new practice features

# 0.6.0

## Replays

- Added deterministic input recording and playback, including frame timing, control-option changes and initial random seeds
- Added replay controls to the level briefing settings; playback can be cancelled by pressing a normal controller button
- Fixed replay desynchronization, replay cancellation and restoration of controller state after playback
- Practice hotkeys now remain available during playback, including pause and time-scale controls
- Fixed replay pacing at changed time scales and prevented paused or held frames from advancing simulation or consuming random state
- Replays share SRAM storage with save states, so recording or saving one overwrites the other; 2.x control styles are not yet supported

## Lag analysis

- Added an optional in-level lag estimate showing dropped frames and their estimated effect on movement speed
- Added a mission-end summary estimating how much time was lost to lag while diagonal-strafing
- Added frame-counting and drift-analysis tools used to calibrate the lag estimates against measured runs

## Save states

- Save states now restore mines, tinted glass, bullet casings, embedded objects, deformed object vertices, shot lights and their glass shards, and Runway's plane animation
- Fixed crashes caused by stale room-to-prop registrations after loading, including loads from level-ending cutscenes
- Fixed PAL save states corrupting restored hand and weapon-model state
- Fixed state loading in production builds

## Grenade camera

- Increased the number of simultaneous projectile views from two to three
- Improved projectile tracking and camera framing, with a crosshair to make small items easier to locate
- Fixed explosion and smoke corruption, rendering-pool exhaustion, cameras lingering on stuck items, and stale camera state after loading a save state

## Interface and controls

- Improved the briefing settings layout and shortened option labels to prevent overlap
- Fixed the level-restart hotkey

## Performance

- Improved practice ROM performance to near parity with the original ROM, improving from about 0.6x speed to 0.9x speed in the Archives replay benchmark; see the [performance benchmarks](src/practice/docs/TODO/PERFORMANCE.md)

## Stability and development

- Fixed a PAL practice-ROM crash when first-person weapon state was updated
- Added a crash handler that reports diagnostic information when practice tests fail
- Added CPU, TLB and memory profiling, deterministic replay captures and interactive ares flame graphs for tracking performance regressions
- Added a validated GCC migration harness and expanded release automation across US, PAL and Japanese development and release builds
- Parallelized emulator tests and improved replay, save-state and lag regression coverage
- Added a project logo and refreshed the README presentation

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
