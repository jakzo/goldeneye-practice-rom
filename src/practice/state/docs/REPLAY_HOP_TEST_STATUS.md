# Cross-level replay hop test status

Updated 2026-08-29. This is the handoff for save-state replay tests that keep
one emulator process alive while switching between replay fixtures and stages.

## Goal

Make the hop test pass across every US replay, with ordinary gameplay and all
props rendered, then run the remaining save-state replay suite. Test
orchestration must remain in `PRACTICE_TEST_ROM`; fixes to real save/load
behaviour belong in the normal practice save-state code.

## Current result

The complete US save-state replay matrix passes with ordinary rendering and
gameplay on the final serialization format:

- Exact replay baselines: all six fixtures, 0 failures.
- Default one-second hops: all six fixtures (5720.05 seconds).
- Long hops (10-second spacing, four-frame wait, 30 seconds before load): all
  six fixtures (1002.28 seconds).
- Grenade and damaged-guard cameras: all six fixtures (5619.86 seconds).
- Facility near-end load: passed (215.33 seconds).
- Cold restart between every SRAM save and load: all six fixtures passed,
  including Dam across 206 boots, Facility across 87 boots, Surface across 128
  boots, Frigate across 133 boots, Runway across 45 boots, and Archives across
  11 boots.

The standard US practice ROM also builds successfully after the instrumented
suite.

The current tree is usable:

- Props are rendered normally. There is no prop suppression or gameplay
  special case used to make tests pass.
- The confirmed prop/model and STAN corrections are normal practice save-state
  code.
- Replay hopping, replay comparisons, and the remaining diagnostic logging are
  test-build-only.
- An experimental paused-render root snapshot did not change the Frigate
  failure and has been removed.
- The speculative first-person hand switch serialization has also been removed.

## Latest verification environment

The final matrix was run with the native Windows Ares build at
`ares/build_msvc/desktop-ui/rundir/ares.exe`, using normal Vulkan rendering on
the NVIDIA GPU. ROM compilation remained inside Docker as required.

Instrumented build:

```sh
docker run --rm -v "$PWD:/home/dev" goldeneye \
  make -j8 VERSION=US PRACTICE_TEST_ROM=1 DEV=0
```

Run:

```sh
env ARES=/Applications/ares.app/Contents/MacOS/ares \
  ARES_ARGS='--no-file-prompt --setting Audio/Driver=None --setting Input/Driver=None' \
  PRACTICE_REPLAY_STATUS_TIMEOUT=120 \
  python3 scripts/run_practice_tests.py \
    --test REPLAY_RUNWAY_SAVE_STATES \
    --replay-fixture tests/replays/us/01-dam.ram \
    --replay-fixture tests/replays/us/02-facility.ram \
    --replay-fixture tests/replays/us/03-runway.ram \
    --replay-fixture tests/replays/us/04-surface1.ram \
    --replay-fixture tests/replays/us/07-frigate.ram \
    --replay-fixture tests/replays/us/11-archives.ram \
    --version US --build-mode release --test-param 1967114 \
    --timeout 1200 --skip-build
```

This long-hop command passes all six fixtures with no replay divergence,
crash, or disabled rendering.

## Confirmed fixes retained

### Prop model recreation and rendering

`slot_matches_object` now verifies that an apparently matching prop slot has a
live model backed by the currently loaded `PitemZ` header. `proplvreset2`
clears every loaded header's `RootNode`; previously, a later-timeline saved
object could occupy the correct slot and model ID while pointing at that
unloaded header. The loader then skipped `create_object_prop` and its
`modelLoad`, causing the Runway projectile crash in
`chrobjGetBboxFromObjFile`. Invalid matches are now torn down and recreated.

Model render-node `BaseAddr` fields and display-list RW pointers are rebuilt
from the current loaded model definition, including hidden switch branches and
attached heads. Object model switch visibility is restored. This fixes the
missing-prop rendering class and the earlier Runway RSP/display-list hang while
keeping normal prop rendering enabled.

### Player hand-buffer equipment

Bond's third-person weapon object lives in a first-person hand buffer rather
than the stage model pool. Loading it as an ordinary `PitemZ` stage object made
repeated Dam/Frigate hops exhaust the stage heap. Player-buffer objects are now
deferred until `solo_char_load` regenerates the correct hand allocation; the
saved gameplay fields, model switches, prop binding, viewer equipment pointer,
attachments, and dual-weapon links are then restored onto that allocation.

### Mutable STAN data

The packed STAN is walked from the first real tile through its own eight-byte
zero terminator. `D_80040F60` is relocation scratch and can describe another
STAN processed later in stage loading; using it omitted Frigate's collision
tile at offset `0x6610`.

Stage loading deterministically rebuilds the packed geometry and links. The
runtime `stanFillin` traversal mutates the high visited bit in each tile ID, so
save states store a validated one-bit-per-tile snapshot and restore those bits
onto the freshly loaded STAN. The earlier complete-payload format made Archives
cold-restart states exceed the 128 KiB SRAM boundary (offset 86130 plus a 54584
byte write); the bitset retains the mutable traversal state without duplicating
tens of kilobytes of immutable collision geometry.

### Mutable pathfinder graph

Waypoint and waygroup distances, group assignments, neighbour arrays, waypoint
arrays, and their terminator positions are serialized after props. Surface AI
destructively shortened one neighbour list from 11 entries to 5; restoring only
the stage setup preserved that truncation and changed later AI/RNG behaviour.

Pathfinder values use exact variable-length integer encoding. This is lossless,
distinguishes null lists from empty lists, and keeps late Facility states inside
the 128 KiB cold-restart SRAM budget.

### Room render cache

The visible-room list stores only its valid prefix, room status flags are packed
at two bits each, mostly-negative room ownership/index arrays use exact signed
variable-length encoding, and room matrices use exact sparse word masks. No
render state is discarded or regenerated approximately. This removed the late
Facility SRAM overflow while preserving normal prop and room rendering.

### Cross-stage ownership and blood-pool lifecycle

Cleanup that follows live prop convenience pointers is restricted to same-stage
loads. After a hop, stale `ObjectRecord` pointers can alias the new stage setup;
clearing a projectile field previously overwrote a Surface AI command byte.
Stale CHR models are range/ownership-validated and destroyed as a complete pass
before any saved CHR is recreated.

The shared blood-vertex pool is reset only after that cross-stage destruction.
Destroying old models can release old blood clones, so resetting first corrupts
the new pool. Individual CHR loads no longer repeat the clone-clear pass after
the global release/reset. This fixes the late Frigate blood-patch restoration
failure without suppressing blood or prop rendering.

### Replay pause boundary

The playback callback consumes the current replay input when the frame delta
was already committed, even if a pause hotkey activates later in that frame.
The hop harness also pauses a fresh destination before loading and recognizes
that destination only in the test build.

## Remaining verification

Automated US replay verification is complete. Manually exercise ordinary
save/load gameplay, especially a late guard-heavy Facility state, before the
documentation is treated as release-final.
