# Cross-level replay test status

Updated 2026-08-22. Read this before resuming work on replay tests that retain
save states while switching levels in one emulator process.

## Goal and current status

The new replay strategy rotates through several replay fixtures without
restarting ares. A saved state remains available when the test leaves one
mission, loads another, and later returns to the first mission.

The focused three-level test does not pass yet. It progressed through several
Dam, Facility, and Runway switches before hanging after a cross-lifecycle
Runway state load. The requested `long`, `cameras`, `near-end`, and
`cold-restart` suite steps have not been run to completion.

The test now changes levels through the game's normal lifecycle:

1. Resume from the paused hotkey frame for one complete live frame.
2. Change to `MENU_LEGAL_SCREEN` and request `LEVELID_TITLE`.
3. Walk the title menus through file, mode, mission, difficulty, and briefing.
4. Request replay playback and enter the destination through `MENU_RUN_STAGE`.

Keep this route. Direct mission-to-mission calls to `bossSetLoadedStage` skip
normal title and menu ownership cleanup and produced misleading teardown
failures.

## Focused reproduction

Build the US test ROM in Docker:

```sh
docker run --rm -v /Users/jfield/oss/007:/home/dev -w /home/dev goldeneye-test \
  make -j8 DEV=0 VERSION=US COMPARE=0 PRACTICE_TEST_ROM=1 TEST_CASE=
```

Run headed ares with the three fixtures:

```sh
env \
  ARES=/Users/jfield/oss/007/ares/build_macos/desktop-ui/Release/ares.app/Contents/MacOS/ares \
  ARES_ARGS='--no-file-prompt --setting Audio/Driver=None --setting Input/Driver=None' \
  PRACTICE_REPLAY_STATUS_TIMEOUT=120 \
  python3 scripts/run_practice_tests.py \
    --test REPLAY_RUNWAY_SAVE_STATES \
    --replay-fixture tests/replays/us/01-dam.ram \
    --replay-fixture tests/replays/us/02-facility.ram \
    --replay-fixture tests/replays/us/03-runway.ram \
    --version US --build-mode release --test-param 50397953 \
    --timeout 300 --skip-build
```

ares exposes GDB on `localhost:9123`. Use the launch settings in
`.vscode/launch.json` or `/opt/homebrew/bin/gdb` when the log stops advancing.

## Last observed run

Before cleanup, the focused test reached this sequence:

- Initial saves in Dam, Facility, and Runway.
- A Dam load and save.
- A Runway load and save.
- A normal exit back to Dam.
- A second Dam load and save.
- A normal entry into Runway.
- `Rebuilding invalid Bond model 80232040` followed by `State loaded`.
- A hang while updating the rebuilt third-person Bond model. GDB stopped in
  model matrix work reached from `chrTickBeams`.

That run included experimental teardown guards which have since been removed,
so reproduce once before relying on the exact stopping point.

## Findings

- Normal menu transitions work across repeated mission changes. The test must
  wait for two stable frames on each menu. The legal screen must also finish
  stopping mission music before file select starts folder music.
- Replay playback is asynchronous across a level request. The test records a
  playback generation and accepts the destination only after both the stage ID
  and generation match.
- Replay difficulty belongs to the fixture header. The menu path must apply it
  before entering the briefing screen.
- A character body with an external head has mutable attachment metadata in
  two places. The shared HEAD placeholder has an `RwDataIndex`, and the model
  instance has a two-record `ModelRwData_HeadPlaceholderRecord` containing the
  head header and the offset of the head's RW data. A state load that reuses a
  character model must restore these values before loading the character
  payload.
- Generic model scalar traversal must follow the node type's canonical child:
  `LOD.Affects` for LOD nodes and `Switch.Controls` for switch nodes. It must
  stop at a HEAD placeholder because an attached head has separate model data.
- `Model::unk34`, `unk38`, `unk64`, and `unk68` are decoded animation cache
  slots. Clear them on load and let matrix calculation regenerate them.
- Third-person Bond is different from an ordinary guard. Its copied body and
  head headers can live in the player's first-person weapon buffers. A
  cross-lifecycle load rebuilds this model, and that rebuilt model is the
  remaining suspect.

## Code kept after cleanup

- `practice_tests.c` uses the normal title-menu route and waits for a new
  replay playback generation.
- `practice_replay.c` exposes the fixture stage and difficulty, tracks playback
  generations, and resets the frozen RNG baseline when a replay starts in a
  newly loaded stage.
- `practice_states_chr.c` validates RW indices, traverses LOD and switch nodes
  through their canonical fields, clears decoded animation caches, applies a
  character root transform after animation restoration, and serializes guard
  head attachment indices and offsets.
- `practice_states_props.c` restores saved external-head attachment metadata
  before reusing a compatible guard model.
- Cross-stage render state is reset through the existing practice render and
  state-load paths. Invalid old-arena render pointers are cleared rather than
  submitted in the forced paused frame.

## Experiments removed during cleanup

These changes helped locate the failure but are not suitable fixes:

- A 512-node traversal limit in `sub_GAME_7F050DE8`.
- Rewriting shared body and head node graphs from `cleanupGuardData`.
- Preserving the destination's live third-person Bond model across a raw
  player-block restore.
- Delaying three Runway renders and manually consuming 8, 4, then 2 RNG values.
- Skipping all RW scalar serialization for externally headed models.
- Test-only invalid-character render logging.

## Resume order

1. Build and run the focused reproduction above. Record the last replay marker
   and attach GDB if output stops for more than 120 seconds.
2. Inspect the rebuilt private Bond model after `load_viewer_players_state`.
   Check its body root parent, HEAD placeholder index, head header, head RW data
   offset, `Model::Type`, and every model node RW index before the first
   `chrTickBeams` call.
3. If the private Bond attachment differs from a freshly built one, serialize
   stable indices or rebuild it through the same game routine. Avoid persisting
   header or RW-data pointers across stage lifecycles.
4. Keep any fix in `src/practice` where possible. If original game code must
   change, preserve its original lines in the `#else` side of a
   `PRACTICE_ROM` conditional.
5. Once the focused test passes, run the requested suite steps in headed mode
   with three workers and `Input/Driver=None`: `long`, `cameras`, `near-end`,
   and `cold-restart`. `regular` and `default` are outside this request.

The full-suite commands are documented in the repository `AGENTS.md`. Set
`PRACTICE_REPLAY_STATUS_TIMEOUT=300` for headed runs because shader compilation
can leave the log quiet for more than the CI default of 30 seconds.
