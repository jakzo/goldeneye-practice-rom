# IDO-to-GCC migration plan

## Goal and scope

Move every C translation unit used by the ROM from IDO 5.3 to GCC while keeping the ROM playable and the practice features reliable throughout the migration. Assembly sources, RSP microcode, linker scripts, and binary assets remain assembled/linked by their existing tools; “IDO-free” means no build step invokes IDO or the IDO recompilation tool.

Exact byte matching to the retail ROM is not a realistic acceptance condition once GCC compiles original game code. The migration should preserve behavior, ABI, data layout, linker symbols, and memory use. The existing IDO build remains the behavioral and layout reference until the final phase.

### Mandatory phase-close profiling

After every migration phase, rebuild the US release profiler with
`PROFILE_PRACTICE=1` and `ORIGINAL_CC=mixed`, run `REPLAY_ARCHIVES`, then run
`scripts/migration/profile_summary.py` against that exact ELF/CSV pair, using
1,156,528 as `--base-average-frame-cycles`. Append one row to the table above
with total code size, average TLB loads, average frame cycles, percentage
difference from base decomp, and frame count. Archive the raw CSV and JSON
summary with the phase's compiler manifest and ELF/map report. Do not close a
phase if the replay diverges, captures fewer than 689 frames, or a material
regression is unexplained.

## Current state and constraints

As of this inventory:

- `src/practice/**` is compiled with `$(CC_GCC)` and linked with `libgcc`.
- Root `src/*.c`, `src/game/*.c`, `src/inflate/inflate.c`, asset-generated C, and the selected libultra/libultrare C sources are compiled with `$(CC)` (IDO).
- There are 130 game C files, 30 root C files, and about 150 C files under `src/libultra` and `src/libultrare` (the libultra makefile selects a subset). Game C alone is about 267,000 lines.
- 46 C files contain `GLOBAL_ASM`; 38 of those are under `src/game`. GCC migration must preserve the asm-processor path for these files.
- The linker already accepts mixed GCC/IDO objects and includes `libgcc`, so file-by-file migration is possible.
- Original game code relies on IDO-era C, permissive diagnostics, fixed BSS/data placement, generated includes, weak aliases, and some ABI-sensitive code. GCC can change stack size, register allocation, floating-point evaluation, signedness, tentative definitions, switch tables, and section placement even when the C looks portable.

The project should pin one GCC/binutils version in the Docker image. A compiler upgrade during this work would otherwise make regressions and size changes difficult to attribute.

## Migration rules

1. **Select the compiler per object, not per directory.** Maintain an explicit `GCC_MIGRATED_C_FILES` manifest (or equivalent object manifest). IDO is the default initially; GCC becomes the default only near the end.
2. **Make every cohort bisectable.** Migrate small, related groups and keep each commit buildable. Avoid mixing compiler plumbing, broad warning cleanup, and a large cohort.
3. **Do not rewrite code for byte matching.** Only make source changes required for defined, equivalent behavior under both compilers. Prefer local casts, declarations, and compatibility macros over algorithm changes.
4. **Preserve boundaries.** Confirm function calling conventions, structure/union sizes and offsets, enum widths, global symbol sizes, section ownership, alignment, and linker-defined symbol declarations.
5. **Track scarce resources.** Record ROM size, `.text/.data/.rodata/.bss` sizes, remaining heap at representative level loads, and maximum stack use where practical. A successful boot is not sufficient.
6. **Test both builds until cutover.** CI should build the IDO baseline and GCC-mixed variant from the same revision. Runtime tests run on the mixed variant; selected deterministic tests should compare logs or state hashes against the IDO build.
7. **Optimize every GCC translation unit for space.** Existing and new practice ROM code and every migrated original source compile with `-Os`, including `DEV=1` builds. Unmigrated original sources retain their reference IDO optimization until they migrate; debug information is additive and must not replace `-Os`.
8. **Profile at every phase boundary.** Follow the mandatory phase-close procedure above and update the performance summary before declaring the phase complete.

## Phase 0: build the migration harness

Before moving game files, make the mixed build intentional and observable.

### Build-system work

- Add `ORIGINAL_CC=ido|gcc|mixed` (names are illustrative). `mixed` uses the explicit manifest; `ido` provides the reference build; `gcc` is the future end state.
- Add a GCC rule for original sources that retains the existing `GLOBAL_ASM` preprocess/post-process steps. Prove this rule on a tiny file containing `GLOBAL_ASM` before relying on it for a large cohort.
- Give GCC original-code flags their own variable, separate from practice flags. Start from the known-working practice flags, then explicitly decide at least:
  - optimization: `-Os` for all GCC-compiled practice and migrated original code;
  - C dialect (prefer `gnu90` initially because the source is IDO-era C);
  - `-fno-pic -mno-abicalls -G 0 -march=vr4300 -mabi=32`;
  - `-fno-strict-aliasing`, until aliasing violations are audited;
  - `-fno-common` with duplicate tentative definitions fixed, or temporary `-fcommon` with a tracked removal gate;
  - signedness of plain `char` and enum width;
  - freestanding/builtin policy, especially `memcpy`, `memset`, math, and 64-bit helpers;
  - VR4300 errata/toolchain flags supported by the pinned compiler.
- Ensure the build-config stamp includes the migration mode, GCC manifest, compiler version, and all GCC flags so switching cohorts cannot reuse stale objects.
- Emit a machine-readable compiler manifest mapping every linked C object to IDO or GCC. Fail CI if an object is unclassified.

### Verification tooling

- Add a map/ELF report that compares IDO and mixed builds: section sizes, symbol address/size changes, undefined symbols, duplicate symbols, and largest function/global changes.
- Add compile-only targets for one file and one cohort to shorten iteration.
- Add static assertions for ABI-critical types shared across compiler boundaries (`sizeof`, alignment, and important `offsetof` values).
- Capture an IDO baseline for all existing practice tests plus deterministic replay/RNG output and representative level-load memory readings.

### Exit gate

An empty mixed manifest produces the current build; adding/removing one file rebuilds only the expected objects; both IDO and mixed modes build all supported release regions (US/EU/JP); and the compiler manifest/map report is archived by CI.

## Phase 1: tiny leaf and initialization files

Start with files that have few functions, little control flow, no `GLOBAL_ASM`, no inline assembly, and no ownership of important runtime state. This validates the mechanism and header compatibility with low gameplay risk.

### Cohort 1A: null, setters, and simple initializers

Initial candidates:

```text
src/game/initnull_0009D0.c
src/game/initnull_000BC0.c
src/game/null_007970.c
src/game/setguscale.c
src/game/initplayergaitobject.c
src/game/initunk_0009E0.c
src/game/initweaponanigroups.c
src/game/cleanup_alarms.c
src/game/cleanwindowpieces.c
src/game/cobjdata.c
src/game/gobjdata.c
src/game/pobjdata.c
```

The three `*objdata.c` files are small but include generated asset data. Treat them as a separate sub-cohort and verify symbol sizes, alignment, and section placement rather than assuming their low line count means low layout risk.

### Cohort 1B: small cleanup/init modules

After 1A is stable:

```text
src/game/cleanup_guard_data.c
src/game/cleanup_objectives.c
src/game/cleanup_object_sounds.c
src/game/cleanupSFXRelated.c
src/game/cleanplayersound.c
src/game/alloc_window_pieces.c
src/game/initcheattext.c
src/game/initimages.c
src/game/initintromatrices.c
src/game/inititemslots.c
src/game/initmenus.c
src/game/initmttex.c
src/game/initunk_007290.c
src/game/playerstats_007770.c
src/game/compiletime.c
src/game/game_debug.c
```

### Exit gate

All regions build, section/symbol changes are understood, the full practice test suite passes, and representative cold boots/level loads show no new crash or memory regression. Keep cohorts split if an included-data file causes broad address movement.

## Phase 2: pure computation and low-state utilities

This phase deliberately exercises arithmetic codegen before GCC reaches core simulation code.

### Cohort 2A: integer and byte-oriented utilities

```text
src/game/crc.c
src/game/decompress.c
src/game/ejectedcartridges.c
src/game/bg2.c
src/game/chr_b.c
src/game/indy_comms.c
src/game/unk_01BAE0.c
```

`decompress.c` has a `GLOBAL_ASM` function, making it a useful early proof of the GCC asm-processor route. Validate decompressed output byte-for-byte.

### Cohort 2B: math primitives

```text
src/game/math_ceil.c
src/game/math_floor.c
src/game/math_asinfacosf.c
src/game/math_atan2f.c
src/game/math_asinacos.c
src/game/math_unk_05A9E0.c
src/game/quaternion.c
```

Do not accept these solely because they compile. Build a host or ROM-side vector test covering normal values, signed zero, boundary angles, infinities/NaNs where relevant, and bit-level outputs when gameplay depends on determinism. GCC constant folding and floating-point reassociation are migration risks.

### Cohort 2C: matrix helpers

Start with `src/game/matrixmath_misc.c`, then the C portions of `src/game/matrixmath.c`. Both contain `GLOBAL_ASM`, and `matrixmath.c` is much larger, so migrate them separately. Add transform-vector golden tests before moving the larger file.

### Exit gate

Golden vectors and decompression checks match the reference, deterministic replay tests do not diverge, and the suite passes in optimized release mode as well as `DEV=1`.

## Phase 3: isolated subsystems and data loaders

Move medium-sized modules with recognizable ownership and testable boundaries. Suggested order:

1. **Objective/status:** `objective.c`, `objective_status2.c`, then `objective_status.c` (the last uses a weak alias; verify GCC/linker semantics explicitly).
2. **Viewport/render helpers:** `viewport.c`, `speedgraphrenderer.c`, `blood_animation.c`, `blood_decrypt.c`, `radar.c`, `fog.c`.
3. **Initialization/data setup:** `initgamedata.c`, `initguards.c`, `initexplosioncasing.c`, `initpathtablesomething.c`, `initunk_005520.c`, `initobjects.c`, `initBondDATAdefaults.c`, `initBondDATA.c`, `chraidata.c`, `image_bank.c`.
4. **File/text loading:** `file.c`, `lvl_text.c`, `loadobjectmodel.c`, then `file2.c`.
5. **Music/microcode control:** `music_0D2720.c`, `mp_music.c`, `rsp.c`, `othermodemicrocode.c`.

These groups are not independent in the link graph; “isolated” means their behavior can be targeted. Add tests for objective transitions, text lookup in every region, model loading, viewport setup, and audio start/stop before migrating each corresponding group.

Avoid `zlib.c` until its global decompression state and bit-exact behavior have dedicated tests. Avoid `rsp.c`/microcode control until DMA buffer alignment and linker symbol declarations have been audited.

### Exit gate

Each subsystem has at least one focused regression test, all regions build/run, and visual/audio smoke checks have been performed for modules not fully observable in automated logs.

## Phase 4: root engine services

Migrate root `src/*.c` as service-oriented cohorts, after their consumers have already exercised mixed-compiler calls:

1. **Memory/string/random:** `str.c`, `token.c`, `mema.c`, `memp.c`, `random.c`, `tlb_random.c`, `c_data_filler.c`.
2. **Input/platform:** `joy.c`, `motor.c`, `pi.c`, `usb.c`, `ramrom.c`.
3. **Audio/video/scheduler:** `music.c`, `snd.c`, `audi.c`, `cfb.c`, `vi.c`, `sched.c`.
4. **Boot/debug/monitoring last:** `boot.c`, `init.c`, `boss.c`, `deb.c`, `debugmenu.c`, `crash.c`, `fr.c`, `rmon.c`, `tlb_manage.c`, `sprintf.c`.

Rationale for this ordering:

- memory allocators, RNG, controller input, scheduling, and DMA have enormous dependency fan-out despite modest file size;
- `random.c`, `mema.c`, `memp.c`, `joy.c`, `snd.c`, `fr.c`, and `boot.c` contain `GLOBAL_ASM` and need special handling;
- `sprintf.c`/`rmon.c` use varargs, which are ABI-sensitive across compilers;
- `usb.c` contains an IDO-specific manual alignment workaround that should remain until GCC-only cutover, then be simplified separately;
- boot, scheduler, interrupt-facing, crash, and TLB code can fail before useful diagnostics exist.

Add allocator stress/invariant tests, fixed-seed RNG sequences, controller recording/replay, audio DMA smoke tests, and long-running level transitions before this phase.

### Exit gate

No original root C service still requires IDO, deterministic replay remains stable or every intentional difference is documented, and extended emulator soak tests cover repeated level loads/restarts and audio/video operation.

## Phase 5: gameplay subsystem cohorts

Migrate by subsystem, generally from leaf/helper to owner. Within each group, move files without `GLOBAL_ASM` first.

1. **Player/inventory:** `bondwalk2.c`, `player_2.c`, `bondhead.c`, `bondinv.c`, then `player.c`.
2. **Menus/front end/cheats:** `debugmenu_08FE00.c`, `debugmenu_handler.c`, `cheat_buttons.c`, `title.c`, then `front.c`, `watch.c`, `mp_watch.c`.
3. **Objects/props/explosions:** cleanup/init helpers first, then `lightfixture.c`, `dyn.c`, `objecthandler_2.c`, `objecthandler.c`, `explosions.c`, `ob.c`, `prop.c`.
4. **Characters/AI:** `chrObjRandom.c`, `initactorpropstuff.c`, `initanitable.c`, `initpathtablelinks.c`, `chrai.c`, `chr.c`, `chrprop.c`, `chrlv.c`, then `chrobjhandler.c`.
5. **Weapons/rendering/world:** `mp_weapon.c`, `image.c`, `model.c`, `stan.c`, `bg.c`, `bondview_r.c`, `bondview.c`, `gun.c`, `spectrum.c`.
6. **Level orchestration last:** `ramromreplay.c`, `lvl.c`, plus remaining `unk_*.c` modules placed with the subsystem indicated by their callers.

The final files are intentionally the largest and/or most coupled. Examples include `gun.c` (~26.7k lines), `spectrum.c` (~26.2k), `bondview.c` (~20.5k), `bg.c` (~13.5k), `chrobjhandler.c` (~40.8k), and several 5k–13k character/menu/world modules. They also contain much of the remaining `GLOBAL_ASM` surface. Migrating one of these is a project milestone, not a routine cohort.

For every gameplay subsystem, add state hashes at stable frame boundaries and compare IDO versus mixed builds from identical recorded input. Hash semantic state (positions, health, object/character state, RNG, objectives), not raw pointer values or padding. Use visual frame captures only as a supplement.

### Exit gate

All game C objects use GCC. Automated coverage includes single-player gameplay, combat, doors/props, AI, menus/watch, multiplayer setup, multiple levels, save states, replays, and region-specific text. Complete manual tests on emulator and real hardware before progressing.

## Phase 6: libultra and libultrare

Do this after game code unless an earlier file is needed to solve a compiler-runtime issue. SDK code is small but hardware- and ABI-sensitive; migrating it early adds risk without proving much game behavior.

Suggested order:

1. **Pure GU math:** `normalize`, `coss`, `sins`, matrix construction helpers. Verify binary output matrices.
2. **Simple libc:** `string`, conversions, then 64-bit helpers. Confirm whether GCC emits calls to repo helpers or `libgcc` and avoid duplicate/incompatible implementations.
3. **Audio library:** leaf functions, heap/filter/sequence code, synthesizer, then Rare-modified driver/reverb code.
4. **Message/thread convenience functions:** queue, send/receive, timer wrappers.
5. **I/O managers and hardware-facing code last:** PI/SI/VI/SP/AI, controller/PFS/EEPROM, device managers, initialization, and interrupt-adjacent code.

Assembly SDK functions remain assembly and do not block IDO removal. Preserve the explicit libultra source list; add a compiler column/manifest rather than replacing it with a broad wildcard.

### Exit gate

Every selected libultra/libultrare C object is GCC-built, audio/controller/video/save-device tests and long soaks pass on ares, and smoke testing succeeds on real N64 hardware/flashcart.

## Phase 7: asset C and final IDO removal

Move C used only to define asset/font/table data after code section placement is no longer changing rapidly. Verify every exported symbol's size, alignment, section, and consumer assumptions. Preserve the `AI_PRINT` conversion pipeline for setup assets, changing only the compiler invoked after conversion.

Then:

- invert the manifest so GCC is the default and any temporary IDO exception fails CI;
- remove `IDO_RECOMP`, `QEMU_IRIX`, `IRIX_ROOT`, IDO warning flags, and the IDO tool dependency;
- remove compatibility flags one at a time (`-fcommon`, permissive warnings, aliasing workarounds), with full tests after each;
- retain an archived reproducible IDO baseline/container and final comparison reports for future regression investigation;
- update build documentation and CI images.

### Final acceptance criteria

- No build command invokes IDO or `tools/ido5.3_recomp`.
- US, EU, and JP release builds and the development/practice build succeed from clean checkouts.
- No C object is absent from the compiler manifest.
- Full automated practice tests, deterministic input/state comparisons, long emulator soaks, and manual real-hardware tests pass.
- ROM/RAM/stack changes are measured and within agreed budgets; no unexplained ABI, symbol, or section-layout differences remain.

## Per-cohort checklist

Use this checklist in every migration PR:

- [ ] List the exact files and why they form one cohort.
- [ ] Compile with GCC at release optimization and `DEV=1`; fix warnings deliberately.
- [ ] Confirm `GLOBAL_ASM`, weak symbols, generated-input transformations, and linker symbols still work where applicable.
- [ ] Compare ELF/map reports: symbol size/address, section size, BSS, undefined/duplicate symbols.
- [ ] Run focused unit/golden tests and the full practice test suite.
- [ ] Run deterministic recorded-input/state comparison against IDO where the cohort affects simulation.
- [ ] Run the 689-frame migration profiler, archive its raw CSV/JSON summary, and append the phase metrics to the performance table at the start of this plan.
- [ ] Check representative level-load memory and stack risk.
- [ ] Build all release regions; run region-specific tests when data/text differs.
- [ ] Record any intentional behavioral or layout difference.
- [ ] Keep the IDO path compiling until the GCC-default cutover.

## Recommended first implementation slice

The first PR should contain only Phase 0 plumbing plus Cohort 1A excluding the generated-data `*objdata.c` files. A follow-up PR can migrate those three data files with explicit ELF symbol checks. This produces a meaningful end-to-end proof while keeping the first behavioral surface extremely small.
