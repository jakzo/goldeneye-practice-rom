# IDO-to-GCC migration plan

## Goal and scope

Move every C translation unit used by the ROM from IDO 5.3 to GCC while keeping the ROM playable and the practice features reliable throughout the migration. Assembly sources, RSP microcode, linker scripts, and binary assets remain assembled/linked by their existing tools; “IDO-free” means no build step invokes IDO or the IDO recompilation tool.

**Migration status:** implementation complete through Phase 8. All C
translation units now use GCC and the build no longer contains or invokes IDO.
The unchecked items in the final acceptance list are release validation on
long emulator soaks and real hardware, not compiler-migration work.

Exact byte matching to the retail ROM is not a realistic acceptance condition once GCC compiles original game code. The migration should preserve behavior, ABI, data layout, linker symbols, and memory use. The existing IDO build remains the behavioral and layout reference until the final phase.

### Historical phase-close profiling (Phases 1-7)

Phases 1-7 rebuilt the US release profiler with
`PROFILE_PRACTICE=1` and `ORIGINAL_CC=mixed`, run `REPLAY_ARCHIVES`, then run
`scripts/performance/profile_summary.py` against that exact ELF/CSV pair, using
1,156,528 as `--base-average-frame-cycles`. Append one row to the table above
with total code size, average TLB loads, average frame cycles, percentage
difference from base decomp, and frame count. Archive the raw CSV and JSON
summary with the phase's compiler manifest and ELF/map report. Phase 8 removed
the `ORIGINAL_CC=mixed` mode with the IDO toolchain; its completion evidence is
recorded in the Phase 8 findings below.

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

### Phase 1 findings for later cohorts

- GCC rejects some token-pasting accepted by IDO. In particular, do not paste
  tokens immediately inside a macro invocation such as `SKELETON( ## NAME ## )`;
  use the standard `SKELETON(NAME)` spelling in a GCC-only definition and keep
  the original IDO definition verbatim in the `#else`. Generated-data macros
  can hide this issue until their including translation unit migrates.
- The `IS_EMPTY` helper in `include/CPPLib.h` token-pastes its argument and
  therefore cannot accept floating-point tokens under GCC. `PROPFILERECORD`
  always has an explicit scale in the current generated assets, so its GCC
  definition can use `SCALE` directly. Before applying the same workaround to
  another optional-argument macro, search every invocation and prove that the
  supposedly optional argument is never empty.
- GCC `-Os` enables constant merging and creates input sections such as
  `.rodata.cst4` and `.rodata.str1.4`. The fixed ROM linker layout does not place
  these sections and they can overlap the following fixed region at link time.
  Migrated original code therefore uses `-fno-merge-constants`; keep this flag
  until the linker script intentionally collects every GCC mergeable-constant
  section.
- GCC moves explicitly zero-initialized globals from `.data` to `.bss` by
  default. This moved the padding globals in `gobjdata.c`, increasing final BSS
  despite adding no state. Migrated original code uses
  `-fno-zero-initialized-in-bss` to preserve section ownership. Compare both
  object sections and final ELF sections; a successful link alone will not
  reveal this migration.
- The three generated object-data files are an effective early layout audit.
  Check them with `readelf -SW` and `nm -S` against IDO, not only with the final
  section totals. GCC reports the naturally padded `ModelNode` objects as 24
  bytes while IDO reports 23-byte symbol sizes, even though the following
  objects remain correctly aligned. Treat one-byte symbol-size changes in these
  records as compiler metadata/layout reporting to investigate, rather than
  immediately assuming one byte of live data was added.
- Expect legacy pointer/integer and structurally compatible pointer warnings in
  early cohorts. They are not link failures on the 32-bit ABI, but each warning
  should be reviewed before tightening diagnostics because it may expose a
  genuinely wrong declaration. Do not silence these warnings cohort-wide.
- When a migrated cohort becomes smaller, fixed ROM sections after `.game`
  shift earlier even if their sizes are unchanged. The Phase 1 mixed US build
  made `.game` 736 bytes smaller while preserving `.bss`, `.csegment`, symbol
  counts, undefined symbols, and duplicate-symbol counts. Record both size and
  address deltas so an expected downstream shift is not mistaken for content
  growth.
- At the Phase 1 close, the Docker `--profile-csv` path emitted exactly 45
  samples and then timed out after the replay logged `TEST_STARTED`. An
  `ORIGINAL_CC=ido` control build reproduced the same 45-sample timeout, while
  the ordinary release `REPLAY_ARCHIVES` test passed in the mixed build. This
  isolates the failure to the existing `PROFILE_PRACTICE=1` profiling path or
  runner, not the migrated cohort. Always rerun a profiling failure with an IDO
  control before bisecting the GCC manifest, and do not use a short/timed-out
  CSV as phase-close performance evidence.

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

### Phase 2 findings for later cohorts

- `asm-processor` dummy functions are ordinary C and are subject to the GCC
  optimizer before their ranges are replaced with assembly. A file with one
  small `GLOBAL_ASM` block can compile while a file with several or a large
  block fails with `Wrongly computed size for section .text`. GCC was deleting
  the dummy null-pointer stores and folding identical dummy functions together.
  Migrated original sources therefore use both
  `-fno-delete-null-pointer-checks` and `-fno-ipa-icf`. Keep these flags for all
  future `GLOBAL_ASM` cohorts; a successful single-block proof does not cover
  the multi-block case.
- The temporary `-fcommon` policy can move a tentative definition out of the
  expected input `.bss` section. `flt_CODE_bss_80075DA0` initially appeared in
  final `.scommon`, reduced reported `.bss`, and produced linker `dot moved
  backwards` warnings. Its GCC declaration now explicitly selects `.bss`,
  restoring the IDO section size. Audit every tentative global owned by a new
  cohort with `readelf -SW`/`nm`; do not assume an uninitialized declaration
  remains in fixed BSS while `-fcommon` is enabled.
  Phase 3 subsequently retired this temporary policy globally; migrated and
  practice sources now compile with `-fno-common`.
- GCC's array-pointer diagnostics exposed several expressions that had the
  right numeric address but the wrong type, such as passing `&result` where a
  decayed `result` pointer was required and assigning `&table_1` to `u16 *`.
  Remove the extra address-of operator instead of silencing the warning. For
  intentional 32-bit protocol or model-node representations, use a local,
  explicit cast and preserve the underlying address/field offset.
- The legacy trig lookup intentionally does not return exact mathematical
  endpoints: the IDO reference gives `acos(0x7fff) == 0x003a`, not zero. Derive
  golden values from the reference implementation before treating a surprising
  approximation as a GCC regression or trying to improve it.
- `MIGRATION_MATH` is now a permanent ROM-side golden test. It checks exact
  float bits for positive/negative rounding and signed zero, integer trig table
  boundaries, multiplication by an identity matrix, and identity quaternion
  conversion. It passes under both `ORIGINAL_CC=ido` and `mixed`. Add new
  arithmetic vectors here as later math cohorts migrate.
- `decompressdata` remains the original injected assembly; only its translation
  unit and adjacent C helper changed compiler. The all-region builds and the
  deterministic Runway/Archives replays load compressed level/model data and
  complete without divergence, providing an end-to-end decompression check.
  If the assembly implementation itself is ever replaced, add a standalone
  compressed-byte fixture before doing so.
- The final Phase 2 US comparison has no added/removed/undefined/duplicate
  symbols, preserves `.bss` exactly, makes `.game` 1,632 bytes smaller, and
  grows `.csegment` by 96 bytes. The latter shifts BSS by 96 bytes without
  changing its size; later reports should distinguish that expected address
  movement from BSS growth.
- Phase 2 closes the build and behavioral gates: US/EU/JP release builds pass,
  every migrated original file also compiles in `DEV=1` with `-Os`, ABI and
  multi-`GLOBAL_ASM` proofs pass, and the optimized suite passes 22/22. Formal
  phase-close performance evidence is still blocked by the Phase 1 profiler
  runner issue: both mixed and IDO controls stop after 45 samples rather than
  the required 689. Do not claim a performance result until that shared
  profiler path is repaired.

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

### Phase 3 findings for later cohorts

- Test each ownership group at boot before adding the next one. The full Phase
  3 cohort initially hung too early for the focused test to run; rebuilding
  successively larger manifest prefixes isolated the problem to `file2.c` much
  faster than debugging the complete cohort would have.
- Do not assume an enum is signed. GCC selected an unsigned representation for
  countdown variables whose enum contained only nonnegative values. Two
  `for (...; value >= 0; value--)` loops in `file2.c` therefore wrapped at zero
  and hung during save validation. GCC builds now use `s32` for those counters,
  while the original IDO declarations remain in the non-migration branch.
  Audit countdowns, negative sentinels, and range checks whenever an enum-heavy
  module moves.
- `-fcommon` is unsafe for this link layout even when it avoids duplicate-symbol
  errors. GCC placed the many tentative globals in `image_bank.c` into COMMON,
  losing the source/linker-script ordering expected by fixed-address state;
  Runway then crashed in `texSelect` after reading a poison pointer. Phase 3
  switched all GCC code to `-fno-common`. The final `image_bank.c` BSS symbols
  match their documented order and offsets, and `.bss` retains the IDO size.
  Fix future duplicate definitions rather than restoring `-fcommon`.
- GCC may reorder top-level declarations even at `-Os`. It reversed several AI
  arrays and tables in `chraidata.c`, so migrated original sources now use
  `-fno-toplevel-reorder`. After that change, the AI command arrays have the
  same offsets, sizes, and bytes as IDO. Remaining object-byte differences in
  the pointer/string tables are relocation addends caused by GCC's tighter
  string packing, not different AI commands.
- The legacy AI command DSL depends on IDO preprocessor behavior. GCC rejected
  token-pasted `DEFINED(...)` tests and comma-bearing expansions in `CALL` and
  fixed-arity `SWITCH`. Keep compiler-specific compatibility at the macro or
  call site, preserve the original IDO form in `#else`, and compare generated
  command bytes; do not broadly rewrite scripts merely to satisfy GCC.
- Linker-visible aliases need an explicit audit. GCC emits
  `objectiveGetStatus_WEAK` as a weak symbol at the exact address of
  `get_status_of_objective`. The IDO linker map also resolves both names to one
  address, but its global `nm` inventory does not expose the alias as a defined
  symbol. GCC does, so this is the one expected added symbol in the final
  report rather than an extra function.
- The RSP declarations use `long long int []`, and the linker continues to
  place `rspbootTextStart`, `rspbootTextEnd`, `gsp3DTextStart`, and
  `gsp3DDataStart` on 8-byte-or-better boundaries. Their boot/text/data spans
  are unchanged from IDO. `g_gfxDramStack` and `g_gfxYieldBuf` remain
  contiguous, correctly sized, and 0x100-aligned in the final image. Preserve
  these declaration types and re-audit the map if linker ordering changes.
- Use `BUILD_DIR_BASE=...` for isolated reference builds. Passing a nested
  `BUILD_DIR` directly leaves the linker preprocessor's `build` root unchanged
  and can accidentally pull objects from two trees, producing misleading
  duplicate practice symbols.
- `MIGRATION_PHASE3` is now a permanent ROM-side test. It covers objective
  registration/restoration, regional text lookup, a loaded object model,
  viewport/Z-buffer display-list setup, and music state stop/restart. Run it on
  a gameplay level such as Runway: the earlier initialization-only level had no
  object models and could not exercise the loader assertion.
- Phase 3 closes its build and behavioral gates: US/EU/JP release builds pass,
  all migrated sources compile at `-Os`, the release and `DEV=1` suites pass
  23/23, and automated Runway rendering/audio smoke and deterministic replays
  complete. The final US comparison preserves `.bss` exactly, makes `.game`
  12,032 bytes smaller and `.csegment` 1,120 bytes smaller, and has no
  undefined or duplicate symbols. Formal performance evidence remains blocked
  by the shared profiler runner issue recorded in Phase 2, so Phase 3 makes no
  performance claim.

### Exit gate

Each subsystem has at least one focused regression test, all regions build/run, and visual/audio smoke checks have been performed for modules not fully observable in automated logs.

## Phase 4: root engine services

Migrate root `src/*.c` as service-oriented cohorts, after their consumers have already exercised mixed-compiler calls:

1. **Memory/string/random:** `str.c`, `token.c`, `mema.c`, `memp.c`, `random.c`, `tlb_random.c`, `c_data_filler.c`.
2. **Input/platform:** `joy.c`, `motor.c`, `pi.c`, `usb.c`, `ramrom.c`.
3. **Audio/video/scheduler:** `music.c`, `snd.c`, `audi.c`, `cfb.c`, `vi.c`, `sched.c`, `speed_graph.c`.
4. **Boot/debug/monitoring last:** `boot.c`, `init.c`, `boss.c`, `deb.c`, `debugmenu.c`, `crash.c`, `fr.c`, `rmon.c`, `tlb_manage.c`, `sprintf.c`, `stacks.c`.

Rationale for this ordering:

- memory allocators, RNG, controller input, scheduling, and DMA have enormous dependency fan-out despite modest file size;
- `random.c`, `mema.c`, `memp.c`, `joy.c`, `snd.c`, `fr.c`, and `boot.c` contain `GLOBAL_ASM` and need special handling;
- `sprintf.c`/`rmon.c` use varargs, which are ABI-sensitive across compilers;
- `usb.c` contains an IDO-specific manual alignment workaround that should remain until GCC-only cutover, then be simplified separately;
- boot, scheduler, interrupt-facing, crash, and TLB code can fail before useful diagnostics exist.

Add allocator stress/invariant tests, fixed-seed RNG sequences, controller recording/replay, audio DMA smoke tests, and long-running level transitions before this phase.

### Phase 4 findings for later cohorts

- Audit every migrated object for compiler runtime helpers before treating a
  linker overflow as a section-layout problem. GCC lowered several `u64`
  divisions to `__divdi3`/`__udivdi3`; the lazily linked libgcc `.text` and
  `.eh_frame` became orphan sections after the fixed-address `.game` segment
  and overlapped font data. Use `mips-linux-gnu-nm -A -u` on the cohort
  objects. Constant timer conversions can use exact strength reductions (for
  example, one second is `osClockRate`), while genuinely variable 64-by-32
  division needs a small explicit helper such as the long-division routines
  now used by `memp.c` and `audi.c`. Recheck equivalence at boundary values.
- Do not assume an unused local array is actually unused in legacy code. IDO
  retained an 8,438-byte array adjacent to `struct huft hlist` in each music
  playback routine; the decompressor wrote its workspace beyond `hlist` into
  that array. GCC `-Os` removed the apparently unused array, shrinking the
  frame from about 8.5 KB to 48 bytes and allowing decompression to overwrite
  saved registers and the return address. The music routines now pass an
  explicit structure containing both the Huffman list and its scratch arena.
  When C passes a small local object to old assembly or a decompressor, inspect
  both compilers' stack frames and model any implicit adjacent storage
  explicitly. A clean compile and link cannot detect this class of failure.
- Bisect boot-time failures by manifest entry or service cohort. The combined
  audio/video cohort originally failed before the focused test could report
  anything; progressively enabling `cfb.c`, `vi.c`, and then `music.c`
  isolated the music stack corruption without speculative changes elsewhere.
  Manifest edits rebuild a broad dependency surface, so use the smallest
  reproducible prefix and inspect the failing object's disassembly.
- IDO accepts local array initializers such as `local_array = global_array`
  that GCC rejects. Preserve the IDO source in the non-GCC branch and choose
  deliberately between an element copy (when a mutable local snapshot was
  intended) and a pointer alias (when the code only reads the data). Phase 4
  needed both forms in audio and crash diagnostics.
- Varargs crossed the mixed-compiler o32 boundary correctly after callback
  types were made explicit. `MIGRATION_PHASE4` permanently checks a formatted
  string containing signed decimal and hexadecimal values, in addition to
  `strtol`, stack setup, fixed-seed game/TLB RNG vectors, and an actual music
  decompression/start/stop cycle. Keep this test when migrating later callers
  of `sprintf`, RNG, or music services.
- The initial Phase 4 inventory missed `speed_graph.c` and `stacks.c` because
  neither appeared in the service-oriented proposal even though both are root
  translation units. Before closing any directory-wide phase, compare the
  manifest mechanically with the build's complete source inventory rather
  than relying only on the planning list. Both omitted files are now migrated.
- Phase 4 closes the build and automated behavioral gates: every listed root
  service compiles with GCC `-Os`; US, EU, and JP release builds pass; the US
  `DEV=1` build and focused Phase 3/4 tests pass; and the optimized 24-case
  practice suite, including controller/replay, repeated save/load/restart, and
  audio/video operation, passes. The shared 689-frame profiler limitation
  recorded in Phase 2 remains, so this phase makes no formal performance
  claim. A real-hardware soak is still recommended before release.

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

### Phase 5 findings for later cohorts

- Mechanically compare the migration manifest with the directory inventory at
  phase close. Phase 5 is complete only after every root `src/game/*.c` object
  appears in the GCC compiler manifest; the original subsystem proposal did
  not enumerate every `unk_*.c` translation unit.
- An enum with only nonnegative named values is not necessarily signed under
  GCC. `ITEM_IDS` fields use `-1` as an undocumented invalid-item sentinel, but
  GCC selected an unsigned representation and optimized
  `weaponnum_watchmenu < 0` out of `gun.c`. The result skipped first-person
  weapon model placement and caused deterministic replay RNG divergence.
  `ITEM_IDS` now has a GCC-only `ITEM_INVALID = -1` enumerator, preserving all
  existing values while forcing the representation expected by the mixed C
  and assembly ABI. Audit enum-backed negative sentinels before migrating SDK
  or asset consumers; a successful size assertion does not prove signedness.
- Late-rodata alignment can become observable even when the injected assembly
  is unchanged. Moving `unk_0A1DA0.c` changed its input-section position, left
  the double constant used by `ldc1` only four-byte aligned, and crashed when
  the health/armour display ran. Use asm-processor's
  `.late_rodata_alignment 8` directive for blocks containing doubleword loads,
  and test the code path rather than checking relocation success alone.
- Legacy C and assembly sometimes treat a declared local as the beginning of a
  larger scratch area. `stanTileDistanceRelated` clears 64 bytes through a
  callback workspace whose visible C fields were smaller; IDO's stack layout
  happened to provide adjacent space, while GCC exposed the overwrite. Model
  the complete workspace explicitly under GCC and audit fixed-size clears,
  pointer walking, and assembly callbacks against the actual object size.
- GCC rejects several permissive IDO constructs in generated/gameplay macros:
  empty arguments in fixed-arity macros, nonstandard token pasting such as
  `& ## NAME`, and initializers whose aggregate shape is only implied. Keep the
  IDO spelling in the non-GCC branch, give GCC explicit zero arguments or
  standard token spelling, and compare generated data bytes and symbol sizes.
- Do not link the host toolchain's ABI/PIC-flavoured libgcc objects into the
  fixed N64 image merely to satisfy a conversion helper. `stan.c` can reuse the
  existing game-ABI `__f_to_ll` and `__ll_to_d` routines. Audit undefined
  symbols per object before accepting a new compiler-runtime dependency.
- Force-rebuild each newly selected cohort before bisecting runtime behavior.
  Stale IDO objects can make a manifest prefix appear healthy. Compiler stamp
  files and verbose command output must confirm `-Os` for the exact object
  under test. Also finish every phase with a build from an empty build
  directory. Phase 5's incremental build retained old `chrlv.o` and
  `chrobjhandler.o` files; a clean build exposed an IDO-only array initializer
  and mismatched unused-parameter types that the manifest alone could not
  detect.
- Deterministic replay seed checks were the highest-signal Phase 5 regression
  detector. They localized a missing pair of RNG calls several seconds before
  any visible failure; comparing the IDO/GCC call sequence then led directly
  to the optimized-away enum-sentinel branch. Preserve these tests for the SDK
  phases even when a replay looks visually correct.
- Phase 5 migrates every root game translation unit to GCC `-Os`, including
  the large character, world, rendering, weapon, and level-orchestration
  owners. The optimized suite passes 24/24, US/EU/JP release builds and US
  `DEV=1` build, and compiler stamps classify all 130 root game sources as GCC
  with `optimization=-Os`. The Phase 5 profiler again emitted exactly 45
  samples and then stalled after `TEST_STARTED`, reproducing the shared runner
  limitation recorded in Phases 1-4 rather than a replay divergence. Its
  memory report showed about 588 KB code/data/BSS, 137 KB total pool free,
  136 KB STAGE headroom, and 215 KB minimum MEMA free. No formal performance
  claim is made until the runner can capture the required 689 frames. A
  real-hardware soak remains recommended before release.
- The final US IDO comparison has no undefined or duplicate symbols. GCC makes
  `.game` 68,752 bytes smaller, root `.code` 9,232 bytes smaller, and
  `.csegment` 2,384 bytes smaller. Final `.bss` is 64 bytes smaller: `bg.o`,
  `joy.o`, `ramrom.o`, and `rmon.o` each lose 16 bytes of trailing object-level
  alignment padding while their live BSS symbols retain their sizes. The 42
  reported added globals are GCC-exposed string/rodata labels plus the already
  documented weak objective alias, not new allocations.

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

### Phase 6 findings for later cohorts

- The compiler manifest classifies 143 of the 145 selected SDK C translation
  units as GCC. The only SDK C exceptions are `libc/ll.c` and `libc/llcvt.c`;
  they remain IDO-built until the Phase 8 standalone-assembly conversion.
- GCC lowers 64-bit division, remainder, and conversion expressions into
  compiler-runtime calls. A helper which implements one of those operations
  can therefore recurse into itself when naively rebuilt with GCC. Migrated
  callers use the existing IDO helper ABI explicitly where necessary, and the
  final ELF has no undefined symbols. Do not add a host ABI/PIC libgcc object
  to solve these helpers.
- Preserve the mathematical value while avoiding unsupported 64-bit lowering.
  The EEPROM long-transfer delay expression `12000 * osClockRate / 1000000`
  is exactly `3 * osClockRate / 250`; passing the reduced numerator and
  denominator to `__ull_divremi` avoids overflow-prone GCC runtime lowering
  without changing integer truncation.
- Legacy SDK code sometimes clears a 64-byte `OSPifRam` by indexing one word
  beyond its declared 15-word `ramarray`, relying on the adjacent `pifstatus`
  field. Under GCC, bound the loop to the array and initialize `pifstatus`
  explicitly or through the immediately following assignment; preserve the
  original loop for the IDO reference. Equivalent structure layout does not
  make out-of-bounds array indexing defined C.
- `MIGRATION_PHASE6` permanently checks fixed-point GU identity-matrix output
  and the mixed GCC/IDO ABI for signed/unsigned 64-bit division and remainder.
  The optimized and `DEV=1` 25-case suites pass, as do US, EU, and JP release
  builds. The compiler manifest contains no other SDK IDO unit and the final
  ELF contains no undefined symbols.
- Against the clean US IDO baseline, the final mixed build has no undefined or
  duplicate symbols. GCC makes `.code` 21,632 bytes smaller, `.game` 80,912
  bytes smaller, `.csegment` 4,016 bytes smaller, and `.bss` 112 bytes smaller.
  The 45 added symbols are compiler-exposed local string/constant labels and
  the previously documented weak alias rather than new runtime allocations.
- The mandatory profiler still emits exactly 45 samples and stalls after
  `TEST_STARTED`, while ordinary `REPLAY_ARCHIVES` completes. This reproduces
  the profiler-path failure already demonstrated with an IDO control in Phase
  1, so the short capture is not performance evidence and Phase 6 makes no
  formal performance claim. Real-hardware controller, EEPROM/PFS, audio, and
  long-soak validation also remains required before release.

### Exit gate

Every selected libultra/libultrare C object except the tracked `ll.c` and
`llcvt.c` compiler-runtime exceptions is GCC-built. Audio/controller/video/
save-device tests and long soaks pass on ares, and smoke testing succeeds on
real N64 hardware/flashcart. Phase 8 replaces the two exceptions with normal
assembly sources before IDO is removed.

## Phase 7: asset C and GCC-default cutover

Move C used only to define asset/font/table data after code section placement is no longer changing rapidly. Verify every exported symbol's size, alignment, section, and consumer assumptions. Preserve the `AI_PRINT` conversion pipeline for setup assets, changing only the compiler invoked after conversion.

Then:

- invert the manifest so GCC is the default and only the two tracked compiler-runtime exceptions may use IDO;
- remove compatibility flags one at a time (`-fcommon`, permissive warnings, aliasing workarounds), with full tests after each;
- retain an archived reproducible IDO baseline/container and final comparison reports for future regression investigation;
- update build documentation and CI images.

### Phase 7 findings for Phase 8

- The final linked C-data cohort consists of `inflate.c`, the global image and
  animation tables, the English/Japanese character data, font display lists,
  Rareware logo, and the Gothic/Zurich font banks. All nine linked asset/font
  sections are byte-for-byte identical to the clean IDO baseline and retain
  their exact sizes and addresses.
- GCC reduces `.inflate` from 5,536 to 4,368 bytes without changing its public
  symbols or data. The only source compatibility fix removes an unnecessary
  pointer-to-`s32` round trip when advancing the compressed input by two
  bytes; the original expression remains in the IDO branch.
- `ORIGINAL_CC=gcc` is now the default. It selects GCC for 343 of 345 linked C
  translation units and reads the only permitted IDO sources from
  `config/ido_exception_sources.txt`. Make rejects an unknown, missing, or
  additional exception; `ORIGINAL_CC=ido` remains available for the archived
  baseline and `mixed` remains available for migration diagnostics.
- The compatibility flags retained at cutover are deliberate ABI/layout or
  freestanding requirements, not unreviewed warning suppression. In
  particular, `-fno-common` is already enabled; unsigned `char`, non-short
  enums, VR4300 errata handling, section-order controls, and the no-builtin/
  no-stack-protector policy remain required. Strict-aliasing and linker support
  for mergeable constants should be audited as separate post-cutover changes
  with their own behavioral and layout comparisons.
- US, EU, and JP GCC-default release builds pass. The optimized and `DEV=1`
  practice suites each pass 25/25, and an isolated empty-directory US build is
  byte-for-byte identical to the incremental release ROM. The final ELF has
  no undefined or duplicate symbols.
- The Phase 7 profiler again emits exactly 45 samples and stalls after
  `TEST_STARTED`; its raw CSV and explicitly incomplete JSON summary are
  archived under `build/profile/phase7-release-us*`. Ordinary
  `REPLAY_ARCHIVES` passes, so no performance claim is made from the short
  capture. Real-hardware validation remains required before release.

### Exit gate

GCC is the default for every linked C translation unit except the two tracked
compiler-runtime helpers, all linked asset data matches the IDO baseline, all
regions and both practice configurations build and pass automated validation,
and the exception allowlist is enforced. Phase 8 removes the final two IDO
invocations; the shared profiler defect and real-hardware soak remain release
gates rather than reasons to return asset data to IDO.

## Phase 8: compiler-runtime assembly cleanup

`src/libultra/libc/ll.c` and `src/libultra/libc/llcvt.c` are temporary IDO
exceptions during the SDK migration. They implement the o32 compiler-runtime
entry points for 64-bit integer arithmetic and integer/floating-point
conversion. When GCC compiles their current C expressions, it lowers those
expressions back into libgcc calls, including calls to the helpers being
implemented. Embedding replacement assembly in GCC-only C branches provides
no runtime advantage: GCC only forwards that assembly to the assembler, while
the large strings make delay slots, ABI conventions, and FPU edge cases harder
to review.

After the rest of the migration is stable:

- preserve the known-good IDO-generated implementations as normal `.s` files;
- add those assembly files to the existing explicit libultra source list and
  remove the corresponding C objects from the linked source list;
- keep the original C beside the assembly as reference documentation, but do
  not invoke IDO to build it;
- compare every exported helper's symbol type, size, alignment, calling
  convention, and boundary behavior against the archived IDO objects;
- retain ROM tests for signed and unsigned division/remainder, shifts,
  multiplication, and conversions around zero, signed limits, `2^63`, and
  floating-point exceptional values.

There should be no runtime performance change when the standalone assembly is
the same as the current IDO output. The benefit is build-system and maintenance
simplicity: the final build can remove IDO without disguising handwritten
assembly as GCC-compiled C.

After the assembly replacements are verified, remove `IDO_RECOMP`,
`QEMU_IRIX`, `IRIX_ROOT`, IDO warning flags, and the IDO tool dependency.

### Phase 8 completion findings

- `ll.c` and `llcvt.c` retain their historical implementations under
  `#ifndef __GNUC__`, but they are no longer linked C translation units.
  `ll.s` and `llcvt.s` contain standalone copies of the known-good MIPS III
  implementations. Their `.text` bytes and every exported symbol's type,
  offset, and size match the archived IDO objects; `llcvt.s` also preserves
  the exact constant-pool bytes and uses normal relocations for them.
- The build has one C compiler mode: GCC. The compiler manifest covers all
  527 US C translation units (313 original, 30 practice, and 184 generated
  asset units), with 527 classified as GCC and zero as IDO. The exception and
  migrated-source allowlists are gone.
- The bundled IRIX compiler/root, native IDO recompiler, QEMU-IRIX install,
  `IDO_RECOMP`, `ORIGINAL_CC`, `QEMU_IRIX`, and `IRIX_ROOT` selection paths
  have been removed from local builds, Docker, tooling, documentation, and
  release CI. Historical IDO C branches remain source references only.
- A clean build exposed two binary-format contracts hidden by incremental
  Phase 7 assets. Setup and stan resource loaders require their root headers
  at byte zero, so GCC marks those headers with dedicated sections and the
  asset linker scripts place those sections first. The test background also
  uses real `extern` forward declarations so its header remains first under
  `-fno-common`. These changes leave the historical compiler path unchanged.
- GCC legally coalesces repeated string literals inside generated setup and
  text units. Across the clean US resource binaries this reduces uncompressed
  setup data by 28,416 bytes and text data by 5,280 bytes; stan grows by 32
  bytes from explicit header alignment. After compression/linking, `.obseg`
  is 12,480 bytes smaller. All relocated root pointers are valid and the
  clean-ROM level, save-state, replay, text, and migration tests pass.
- Relative to the Phase 7 clean US ELF, `.code` is 16 bytes smaller and the
  following sections move down by 16 bytes without changing their sizes.
  The only function-size changes are clean GCC rebuilds of
  `__osDequeueThread` (-16), `osYieldThread` (-8), and
  `__osSetHWIntrRoutine` (-8); the standalone runtime helpers themselves are
  byte-identical. There are no added, removed, undefined, or duplicate
  symbols.
- Clean US, EU, and JP release builds and a clean US `DEV=1` build succeed.
  The clean release suite passes all 25 cases (22 together plus three
  test-level cases rerun singly after a parallel watchdog timeout), and the
  clean DEV suite passes 25/25 in one run. GCC ABI assertions and the GCC
  `GLOBAL_ASM` proof also pass. The Phase 1 profiler defect, long emulator
  soak, and real-hardware/flashcart validation remain release validation
  tasks; they do not require or invoke IDO.

### Final acceptance criteria

- [x] No build command invokes IDO or `tools/ido5.3_recomp`.
- [x] US, EU, and JP release builds and the development/practice build succeed
  from clean build directories.
- [x] No C object is absent from the compiler manifest.
- [x] Full automated practice tests and deterministic replay/state comparisons
  pass.
- [ ] Complete the long emulator soak and manual real-hardware/flashcart tests
  before release.
- [x] ROM/RAM changes are measured; no unexplained ABI, symbol, or section
  layout differences remain.

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
