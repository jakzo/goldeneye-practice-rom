# Cross-level replay hop test status

Updated 2026-08-27. This is the handoff for save-state replay tests that keep
one emulator process alive while switching between replay fixtures and stages.

## Goal

Make the hop test pass across every US replay, with ordinary gameplay and all
props rendered, then run the remaining save-state replay suite. Test
orchestration must remain in `PRACTICE_TEST_ROM`; fixes to real save/load
behaviour belong in the normal practice save-state code.

## Current result

The six-replay long-hop reproducer now gets through the former Runway crash and
fails deterministically on Frigate. The full all-replay hop test and the later
suite steps have not passed yet.

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

## Latest reproduction

Build:

```sh
docker run --rm -v /Users/jfield/oss/007:/home/dev goldeneye \
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
    --version US --build-mode release --test-param 1966858 \
    --timeout 1200 --skip-build
```

It fails at Frigate replay timestamp 2459, after loading the state saved at
timestamp 2415 through other stage lifecycles. Runtime is about 95 seconds.

A focused Frigate-only run with the same parameter passes all four long hops
and completes in about 132 seconds. The remaining bug therefore requires a
cross-stage model/allocation lifecycle; it is not an ordinary same-stage load
failure.

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

### Mutable STAN data

The packed mutable STAN payload is saved from the first real tile through
`D_80040F60`, the actual final tile, and restored on load. `stanFillin` mutates
point/topology data during play, so rebuilding a stage alone was insufficient.
This fixed the earlier Runway guard line-of-sight divergence.

The payload is currently deliberately complete. Facility states can be about
311 KB, which is acceptable for the in-memory tests but should be reviewed
before relying on SRAM capacity. Compact only after correctness is established.

### Replay pause boundary

The playback callback consumes the current replay input when the frame delta
was already committed, even if a pause hotkey activates later in that frame.
The hop harness also pauses a fresh destination before loading and recognizes
that destination only in the test build.

## Remaining Frigate divergence

At the saved timestamp 2415, guard literal ID 36 restores exactly apart from
expected allocation addresses:

- the `ChrRecord`, `Model`, `PropRecord`, animation controller, action union,
  and model RW scalar data match;
- the full root RW record matches (`3f3fca66` diagnostic hash);
- the `ModelFileHeader`, root node/RO data, skeleton, and skeleton joints match;
- the only raw differences are the recreated model allocation pointer, model
  RW-data allocation pointer, and the attached-head RW-data allocation pointer.

The same comparison immediately after `chrlvActionTick`, before animation
movement, is still exact except for those three allocation pointers. The split
therefore occurs inside the following animation-position update, not in AI
action processing or the paused render:

```text
timestamp 2422 expected position x: 4336740a
timestamp 2422 restored position x: 43248a3d
```

The restored run remains RNG-identical through timestamp 2453. At timestamp
2459, the uninterrupted run's bullet hits prop slot 96 while the restored run
hits the background. That changes three subsequent RNG calls and triggers the
reported replay divergence (`20b259b4` versus `8b4c82b5`). The RNG mismatch is
downstream, not the root cause.

## Best next diagnostic

Instrument guard 36 around `chrPositionRelated7F020E40` for timestamp 2422 in
test builds only:

1. Log/compare `getsuboffset` before animation advancement.
2. Log the model controller and root immediately after
   `modelTickAnimQuarterSpeed`.
3. Log the root and returned offset immediately after `subcalcpos`.
4. If the decoded offset first differs there, inspect address-sensitive
   attached-head RW data and any global animation decode/cache state used by
   `sub_GAME_7F06D490`; the body definition and skeleton have already been
   ruled out.

Do not reintroduce prop suppression or alter gameplay. Any address-lifecycle or
serialization correction should be made in normal practice save-state code.

After fixing Frigate, rerun the six-replay reproducer, then the complete hop
test across all replays. If that passes, run `regular`, `default`, `long`,
`cameras`, `near-end`, and `cold-restart` using the suite commands in
`AGENTS.md`.
