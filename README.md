![Goldeneye Practice ROM logo](./images/practice_rom_logo_transparent.svg)

> Custom rom of Goldeneye 007 for Nintendo 64 with various tools for practicing speedruns.

## How to install

- Download the latest `.xdelta` patch from [the releases page](https://github.com/jakzo/goldeneneye-practice-rom/releases)
- Acquire an unmodified base ROM of the US NTSC version of Goldeneye
- Use a ROM patcher tool such as [this one](https://www.marcrobledo.com/RomPatcher.js/) and apply the patch to the base ROM
- Run your modified ROM on:
    - Emulator:
        - Any should work though I recommend [ares](https://ares-emu.net/download) for its accuracy
    - Nintendo 64:
        - [SummerCart64](https://summercart64.dev/)
        - [EverDrive 64](https://krikzz.com/our-products/cartridges/ed64x7.html)
        - Copy the ROM to either of these via the micro SD card or USB

## Features

- **Timer displayed during gameplay**
- **End screen timer displays hundredths**
- **Time scale control**
    - During gameplay press `L + D-Left` to decrease game speed by 10%
    - During gameplay press `L + D-Right` to increase game speed by 10%
    - Note that running the game slower will result in less lag compared to normal or faster speed so times are not perfectly comparable
- **Speedometer**
    - Displays Bond's current horizontal movement speed
    - Moving vertically (higher and lower) does not affect the displayed speed
- **Lag estimate**
    - Displays the amount of lag and estimated movement speed slowdown
    - The format looks like `+0.6 (2)` where:
        - `(2)` means there were 2 dropped frames before the current one was rendered
        - `+0.6` means that if you were strafe-running at max speed for 100 meters (slightly less than the length of the start of the runway to the plane in Runway) you would have taken 0.6 seconds longer than if there were 0 dropped frames
- **Lag impact summary**
    - Estimates the amount of time lost due to lag
    - While the mission timer is running during a level and Bond is diagonal-strafing, lag is tracked for these frames and the slowdown of Bond's speed is estimated
    - On the end-of-mission statistics screen the impact is shown next to the completion time as e.g. `Lag: 1.2% (3.45s of 67.89s)` where:
        - `67.89s` is the total time spent diagonal-strafing
        - `3.45s` is the estimated extra time spent diagonal-strafing from slower movement due to lag
        - `1.2%` is the percentage of extra lag time to total time
    - Movement speed slowdown is based on experimentally measured data of 100m runs using a variety of lag amounts
    - Forward-only running without strafe and paused time are not counted
    - Note that all time spent diagonal-strafing is counted, even when lag and movement speed do not affect the final time (e.g. during the Control protect sequence)
    - Some lag is unavoidable, this data is most useful as a comparison between runs of the same level and route
- **Skip level intro cutscenes**
    - Starting a level goes straight into it, rather than playing the intro cutscene
- **Skips legal and logo screens on game startup**
- **Save automatically fully unlocked**
    - On booting the ROM for the first time the default save will have all levels and cheats unlocked

### Level features

- **Dam**
    - Indicator for when the gate guard has loaded in
    - Option to never play the gate intro cutscene
- **Runway**
    - Splits

### ℹ️ Beta features

These features have some minor bugs or limitations but should be usable:

- **Save states**
    - During gameplay press `L + D-Down` to save current state
    - During gameplay press `L + D-Up` to load current state
    - There are several minor visual/audio pieces of state which are not saved or loaded but should not affect gameplay
        - If there is some state not saved that DOES affect gameplay, let me know (@jakzo on Discord)
    - Not fully stable, expect occassional crashes
        - If you can reliably reproduce the crash, let me know so I can fix it (@jakzo on Discord)
    - "Why don't things play out the exact same way when I load state then leave the game alone?"
        - The game doesn't run with the **exact** same performance each time after restoring state and this causes the random number generator to become out of sync which means anything using randomness in the game (everything from visuals to collisions) will diverge
- **Replays**
    - On a level's briefing screen, set `Replay next level` to `Record` to record the next attempt
    - After a recording exists for that level, set it to `Playback` to replay the saved inputs, frame timing, control option changes, and initial random seeds
    - Practice hotkeys remain available during playback; other button presses cancel playback and resume normal control
    - Replays and save states use the same SRAM space, so saving either one overwrites the other and only one replay can be saved at a time
    - 2.x control styles are not currently supported
- **Grenade camera**
    - When throwing most projectiles, shows a picture-in-picture view that follows it so you can see where it landed

## Development

See [readme-dev.md](readme-dev.md).
