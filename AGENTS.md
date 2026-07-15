## Project Overview

This repository contains a custom ROM for the N64 game **GoldenEye 007** with various utilities for practicing speedruns. It is a fork of a work-in-progress, byte-matching decompilation of the original N64 game. It compiles C source files and assembly files back into `.z64` ROM binaries.

---

## Directory Structure

Below is the high-level layout of the repository and where components are located:

```
goldeneye_src
├── assets/                  # Game assets extracted from baseroms (font, images, music, obseg models, briefs, text)
├── bin/                     # Raw binary components not yet matched or decompiled
├── build/                   # Compilation outputs and generated ROMs (e.g., build/u/ge007.u.z64)
├── include/                 # Game and engine header files (.h)
├── ld/                      # Memory layout/linker segment specification files (.ld.inc)
├── rsp/                     # Custom Reality Signal Processor (RSP) assembly microcode (C0 and 4Tri)
├── scripts/                 # Asset extraction, diffing, and environment helper scripts
├── src/                     # C and Assembly source files
│   ├── game/                # Core game-specific code (cheats, gun, player logic, objectives, text, etc.)
│   ├── practice/            # Utilities for practicing speedruns (new code goes here)
│   ├── inflate/             # Game decompression logic (zlib/inflate)
│   ├── libultra/            # N64 SDK library code
│   └── libultrare/          # N64 SDK library code modified by Rare
└── tools/                   # Build tools (SGI IRIX compiler, asm-processor, n64cksum, diff, asset utilities)
```

---

## Build Commands

All builds must be run inside Docker since the host environment is macOS. Use the `goldeneye` Docker image to compile the ROMs.

### Standard Compilations

- **Build NTSC-US ROM (with Parallel Jobs)**:
    ```bash
    docker run --rm -v $(pwd):/home/dev goldeneye make -j8
    ```
- **Clean Build Files**:
    ```bash
    docker run --rm -v $(pwd):/home/dev goldeneye make clean
    ```

### Build Variables

You can configure Makefile behavior by passing the following flags (e.g., `make VARIABLE=VALUE`):

- `DEV`: `1` (disable optimizations, add dev code), `0` (prod build, default)
- `VERSION`: `US` (default), `JP`, `EU`
- `COMPARE`: `1` (compare ROM checksum against targets), `0` (disable comparison for custom builds, default)
- `FINAL`: `YES` (default), `NO` (builds non-working debug version of the decomp)
- `VERBOSE`: `0` (quiet, default), `1` (prints full command lines)

---

## Guide to Key Source Files

When tasked with modification or analysis of gameplay, look here first:

- **Cheats & Inputs**:
    - `src/game/cheat_buttons.c` / `src/game/cheat_buttons.h`: Handles controller combination checks, cheat menu bindings, and cheat activations.
- **Practice**:
    - `src/practice`: Folder containing utilities for speedrun practice. All new logic goes in files here. This is not in the original game.
- **Weapon Logic**:
    - `src/game/gun.c` / `src/game/gun.h`: Massive file handling shooting, aiming, projectile calculations, reloading, and weapon state.
- **Player Physics & Camera**:
    - `src/game/player.c` / `src/game/player.h`: Player movement state machine, health, coordinates, and controls.
    - `src/game/bondview.c`: Heavy rendering, camera coordinates, and HUD logic.
- **Linker & Memory Map**:
    - `ge007.ld`: Top-level linker specification arranging code segments and overlays.
    - `ld/`: Segment definitions specifying BSS/text/rodata address overlays.

All new logic for the practice ROM is stored in the files under `src/practice`. If logic needs to be added to existing files, it should be added within an `#ifdef PRACTICE_ROM` block. If any lines are modified, the modified lines should exist in the `#ifdef` block with the original still existing, unmodified in the `#else` block. This is so that changes to the original code are easy to find and compare to original. `#include` directives do not need to be in `#ifdef` blocks.

Do NOT put practice code inside `#ifdef NONMATCHING` blocks as this code will not be built (the assembly is used instead).

Also make sure to not add large global variables, since they consume BSS size which takes the scarce memory away from the game and causes crashes.

---

## Building Features

In general all features should have a corresponding setting, or multiple if the feature has parameters that can be tweaked. Add the option to `practice_config.c` and to the menu in `practice_ui.c`. Features should also be briefly documented in the `README.md`. Do not add to the `CHANGELOG.md` unless explicitly asked (this will happen right before release of the next version).

---

## Save State

When implementing or fixing anything to do with save states, read through [INSTRUCTIONS.md](src/practice/state/docs/INSTRUCTIONS.md) and make sure to:

- See `practice_states_*.c` files for the current state of the code
- Remove the item from the "Remaining" section once fixed
- No need to update the changelog or SAVE_STATE_VERSION
- If you notice some other state that is not supported yet and should be, implement it immediately if simple, otherwise add it to the Remaining list to do it later
- If there is a case that might need to be considered but you are not sure (for example, if a field is normally null or a pointer but theoretically the object pointed to could have been freed, though there is no evidence of this) then add an invariant assertion and emit an error log if it fails, no need to handle the case gracefully
- Where practical, add a regression test or modify an existing one
- After fixing the bug, ask the user to manually test to ensure the bug is fixed
- Default to saving state of fields when you are not sure if they are needed or not (better to save too much than too little)
- If you notice any documentation (either in the `docs` files or in comments) is incorrect or too vague, tell me which docs you want to update and after I have confirmed that the fix worked, update the docs
- Also if the fix was narrow (e.g. targets a single prop type) check if the fix would also apply to other prop types

---

## Assembly & C Integration (GLOBAL_ASM)

The codebase heavily utilizes standard decompilation patterns to bridge C with yet-unmatched assembly code.

- If a function has not yet been decompiled to C, it is referenced via the preprocessor macro:
    ```c
    #pragma GLOBAL_ASM("asm/nonmatchings/game/some_file/some_func.s")
    ```
- The python preprocessor `tools/asm-processor/asm_processor.py` intercepts this during compile, compiles the `.s` file, and stabs the generated object code directly into the compiled C unit to allow matching builds.

---

## Debugging and verification

1. Run the test case:
    - `just test TEST_CASE_NAME`
    - See practice_debug.c for the implementation of each test case
    - Don't be afraid to add new temporary test cases for debugging to verify the state of things at runtime
    - If adding a new regression test, make sure to do red-green where it fails when your fix is inactive but passes when active
2. View logs
    - This will start the ares emulator and immediately do some action in a level
    - ares is nearly cycle-accurate so if it has an issue, even a low-level hardware or timing one, it is almost certainly an issue on real hardware as well
    - The test will emit logs to the emulator's STDOUT which you can use to see how far the test got and whether it succeeded or hung
    - It will also start a GDB server on localhost:9123 which you can connect to while the emulator is running
    - The emulator takes about 5 seconds to boot up and another 5 seconds to run the test case (depending on its implementation)
    - If needed you can use the `emu_log` function to log things to the emulator's STDOUT
    - If you stop this command the emulator will quit!
    - If you do run the emulator in the background remember to kill it later
3. If needed use the GDB MCP tools to connect
    - See the settings in `.vscode/launch.json` for how to connect
    - `DEV=1` GCC C objects include debug information, so source breakpoints and variables work for decompiled game and practice C
    - `GLOBAL_ASM` functions remain assembly-only; use function breakpoints and inspect argument registers/memory when debugging them
