# CPU profiling

## Current gap and performance direction

On the deterministic 689-frame Runway replay, the release practice ROM
(original engine IDO `-O2`, practice code GCC `-O2`) averages 1,783,641 cycles
per frame versus 1,156,528 for the matching base decomp: a 627,113-cycle
(54%) regression. Expensive software code-page loads are the dominant measured
cause. The release practice ROM adds 5.95 loads and 514,265 directly measured
loader cycles per frame, accounting for 82% of the gap before the downstream
cost of running original engine code after a full instruction-cache
invalidation.

The current improvement ideas, in recommended order, are:

1. Compile normal practice code for size. GCC `-Os` produced the best measured
   result: 1,603,238 cycles and 4.85 page loads per frame, versus 1,783,641 and
   6.61 with `-O2`.
2. Exclude tests, diagnostics, and other unavailable cold implementations from
   normal builds rather than merely disabling their call sites.
3. Profile loads by virtual 8 KiB page, then group hot per-frame functions and
   separate menus, save/load internals, and diagnostics onto cold pages.
4. Evaluate a recency-aware replacement policy from the captured page trace.
   Plain free-first plus round-robin regressed badly and should not be used.
5. Consider a modest cache increase only after measuring stage/permanent-pool,
   mema, graphics, audio, and stack high-water marks across memory-heavy levels.
   Each additional slot costs 8 KiB of RDRAM.
6. Investigate a verified index-based targeted I-cache invalidation. A simple
   hit invalidation through the KSEG0 backing-page alias crashed and is unsafe.
7. Test selective size compilation for large cold IDO files or migrate suitable
   fully decompiled files to GCC `-Os`; avoid slowing hot engine routines
   globally without paired measurements.

The profiler is retained long term but compiled only with
`PROFILE_PRACTICE=1`. `just profile-runway` enables this automatically. Normal
ROM and ordinary test builds compile all profiling calls and TLB counters out,
so the diagnostic code does not introduce hot-path calls or page loads.

The deterministic 1x Runway replay can record per-rendered-frame CPU timings:

```sh
just profile-runway
```

This runs `REPLAY_RUNWAY_1X` in ares and writes
[`profile_runway_practice.csv`](profile_runway_practice.csv). To choose another
path, run `just profile-runway path/to/output.csv`.

### Capturing the original decomp

The minimal replay/profiler implementation for the unmodified game lives on
the `base-decomp-replay-profile` branch. The documentation and checked-in
results are maintained here on master; the comparison branch only needs to
retain the code, replay fixture, and capture script.

Use a separate worktree so the master changes do not need to be switched out:

```sh
git worktree add ../007-base-profile base-decomp-replay-profile
cd ../007-base-profile
docker run --rm -v "$(pwd):/home/dev" goldeneye make clean
docker run --rm -v "$(pwd):/home/dev" goldeneye \
  make -j8 PROFILE_REPLAY=1 COMPARE=0
ARES=/path/to/ares \
ARES_ARGS="--no-file-prompt --setting Audio/Driver=None --setting Input/Driver=None" \
  python3 scripts/profile_replay.py
```

The clean is required because the older base-decomp Makefile does not track
build-variable changes. The script boots Runway, copies the shared
`tests/replays/runway.ram` fixture beside the ROM, captures 689 frames, and
writes `docs/profile_runway_base.csv` in that worktree. After validating the
capture, copy it to master's checked-in
[`profile_runway_base.csv`](profile_runway_base.csv).

The original capture must use its normal `FINAL=YES` IDO `-O2` build. Compare
it with the practice release build, where original engine code is also IDO
`-O2` and practice code is GCC `-O2`; do not use the `DEV=1` practice capture
as the release regression.

### Replay determinism checks

Both profilers restore the replay's initial RNG state, difficulty, options,
controller samples, and recorded frame deltas. When the fixture contains
`REPLAY_FLAG_FRAME_SEEDS`, they compare both 64-bit streams (`g_randomSeed`
and `g_chrObjRandomSeed`) against the recorded values before each frame and do
not force those per-frame values.

The base profiler logs `TEST_FAILED replay diverged` and stops immediately on
a mismatch. The practice replay records and logs the actual and expected
values, and `REPLAY_RUNWAY_1X` emits `TEST_FAILED` when playback ends. The
capture scripts treat either result as failure, so a CSV from a divergent run
must not be used.

Profiling is compiled only for captures requested with `--profile-csv`, then
enabled only while replay playback is active. Each timer uses the N64 CP0
Count register via `osGetCount()`. The instrumentation retains only the
current frame and cumulative TLB counters, and logs the completed row after
all measured regions have ended.

## Columns

- `frame`: zero-based rendered replay frame.
- `delta_frames`: replay-recorded 60 Hz simulation ticks consumed by the frame.
- `tick_cycles`: CPU time from acquiring the master display list through level
  simulation, level rendering, and debug-menu display-list construction.
- `physics_cycles`: the `lvlViewMoveTick` player simulation portion. Values are
  accumulated for all local players.
- `render_cycles`: the complete `lvlRender` call.
- `practice_ui_cycles`: `practice_ui_render`; this is a subset of
  `render_cycles`.
- `physics_setup`, `physics_core`, and `physics_practice`: viewport/player
  setup, `MoveBond`/frozen movement, and the practice-only physics hooks.
- `render_setup`, `render_visibility`, `render_bg`, `render_actors`,
  `render_hud`, and `render_practice`: nested portions of `lvlRender`.
- `tlb_loads`: expensive software code-page loads serviced by the TLB thread.
- `tlb_cycles`, `tlb_dma_cycles`, and `tlb_icache_cycles`: total loader time
  and its ROM DMA and instruction-cache invalidation portions.

The render measurements cover CPU display-list construction. They do not
measure asynchronous RSP/RDP execution. The few `osGetCount()` calls add a
small fixed overhead, so comparisons should use identical instrumentation and
build settings on both ROMs.

## Development diagnostic capture

Captured on 2026-07-11 with the US development build and ares using the checked
in `tests/replays/runway.ram` fixture. This capture diagnoses the effect of
unoptimized GCC practice code; it is not the release-to-original comparison.

| Metric | Mean cycles | Minimum | Maximum |
| --- | ---: | ---: | ---: |
| Entire tick | 2,399,885 | 682,388 | 12,456,809 |
| Player physics | 396,400 | 93,843 | 8,515,399 |
| Level render | 1,830,428 | 516,713 | 9,671,845 |
| Practice UI | 55,130 | 25,432 | 437,421 |

## Release-to-original TLB result

| Metric | Base IDO `-O2` | Release practice | Added |
| --- | ---: | ---: | ---: |
| Entire tick cycles/frame | 1,156,528 | 1,783,641 | 627,113 |
| Software page loads/frame | 0.66 | 6.61 | 5.95 |
| TLB loader cycles/frame | 57,243 | 571,508 | 514,265 |
| ROM DMA cycles/frame | 54,829 | 550,348 | 495,519 |
| I-cache invalidation cycles/frame | 1,474 | 13,941 | 12,467 |

The additional directly measured loader time accounts for 82% of the release
tick regression. This remains a lower bound on TLB-induced cost because it
does not include the subsequent execution of original engine code with a cold
instruction cache.

## Development TLB diagnostic

The practice capture performs 8,203 software code-page loads, averaging 11.91
loads and 1,027,563 loader cycles per rendered frame. ROM DMA accounts for
989,409 cycles, while the two full instruction-cache invalidation calls take
24,442 cycles directly. Each page load costs about 86,309 cycles, including
83,104 cycles of DMA.

The matching base-decomp capture performs only 452 loads: 0.66 loads and
57,243 loader cycles per frame. Relative to that optimized base, the
development ROM adds 11.25 page loads and 970,320 directly measured loader
cycles per frame. This development-only comparison illustrates the extra cost
of GCC `-g`; it is not the release regression. Paired per-frame TLB and tick
deltas have correlation 0.979.

## TLB optimization experiments

These diagnostic variants completed the same 689-frame replay unless noted.
None is adopted by the current source.

| Variant | Tick cycles | Page loads/frame | TLB cycles/frame |
| --- | ---: | ---: | ---: |
| Development GCC `-g` | 2,399,885 | 11.91 | 1,027,563 |
| Practice code GCC `-O2` | 1,783,641 | 6.61 | 571,508 |
| Practice code GCC `-Os` | 1,603,238 | 4.85 | 419,648 |
| Free-first then round-robin | 3,845,401 | 25.95 | 2,246,916 |

`-Os` is 796,647 cycles faster than `-g` and 180,403 cycles faster than
`-O2`. It reduces `practice.o` text from 130,940 bytes under `-O2` to 99,460
bytes, a 24% reduction equivalent to roughly three 8 KiB pages. The `-Os`
diagnostic profiler arrays accumulated instead of resetting, so its per-region
values above were recovered by differencing consecutive rows; its independent
TLB snapshot fields remained per-frame and the replay completed without RNG
divergence.

Filling unused slots first and then using simple round-robin replacement was a
large regression: it more than doubled page loads relative to random
replacement. This indicates that deterministic cyclic eviction aligns badly
with the Runway working set and is not a suitable final policy by itself.

Replacing the two global I-cache invalidations with a hit invalidation of only
the reused 8 KiB KSEG0 slot crashed before replay profiling began. The physical
slot alias does not invalidate every cached instruction fetched through the
`0x7f...` mapping. Any narrower invalidation experiment needs a dedicated,
verified index-based cache routine or equivalent alias-safe mechanism.

The capture contains 689 rendered frames. Its recorded delta distribution is:

| Delta frames | Rendered frames |
| ---: | ---: |
| 1 | 1 |
| 2 | 84 |
| 3 | 211 |
| 4 | 268 |
| 5 | 94 |
| 6 | 19 |
| 7 | 4 |
| 8 | 4 |
| 13 | 1 |
| 14 | 1 |
| 16 | 2 |

The CSV is the source of truth; this summary should be regenerated whenever
the fixture, build mode, profiling boundaries, or relevant code changes.
