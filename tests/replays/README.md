# Practice-ROM replay test fixtures

The `us`, `eu`, and `jp` directories contain the same regional SRAM recordings
used by the base-ROM ares tests. Every `.ram` recording has a `.state` sidecar
whose header tracks both random seeds plus the upstream character-selection,
intro-camera, global-timer, clock-timer, and frame-timing state. Its binary
payload contains the expected values for every frame. Keeping these compact
inputs alongside the seeds makes the first cause of a divergence visible
without recording larger state that can be derived from them.

## Replay guidelines

When recording a run keep these in mind:

- Make the recording as fast as possible
- No lookdown
- Let the intro cutscene, camera swirl and outro cutscene play for about 3 seconds each
- Play a difficulty with all the level objectives
- Nice to have but not required:
   - Destroy and disturb props
   - Load in assets by looking at unique characters and props
   - Equip all level weapons
   - Pick up collectibles like body armor
   - Visit all the parts of the level

## Whole-ROM replay tests

Run every fixture for one practice-ROM region through the new ares host-side
replay and state comparison:

```bash
just test-practice-replays us
just test-practice-replays eu
just test-practice-replays jp
```

The recipe builds the release practice ROM and matching ELF, then passes the
regional `.ram` and `.state` files to ares. Each run writes its observed state
and profiler reports under `build/practice-replay-results/<region>`. A populated
state sidecar replaces the replay's embedded seed check. The first differing
frame reports every differing variable with its actual and expected bytes.

These fixtures were recorded against the base ROM. Practice-ROM divergences are
reported as failures and are not suppressed by the harness.

### Regenerating `.state` fixtures

Regenerate the fixtures only when their text header (the tracked symbol/range
list on the first line) changes. Gameplay or save-state implementation changes
do not require regeneration when that header is unchanged.

First replace the headers and resize their placeholder payloads:

```bash
python3 scripts/set_replay_state_fields.py
```

Then run `ares/tests/n64-replay/run.py --regenerate-state` separately for each
region, supplying the untouched regional base ROM and its matching base-build
ELF. Never supply a modified practice ROM: the resulting payload is the
reference timeline against which the practice ROM is checked. The runner keeps
the header, records every listed range for every replay frame, and replaces the
tracked sidecar only after that replay reports `TEST_COMPLETE`. `--jobs 3` runs
three headed emulator instances concurrently.

After a save/load fix, run the ordinary practice-ROM replay suite against these
unchanged fixtures first. Once it passes, run the replay save/load-state suite
to find the next load-specific failure or regression.

## Balanced full-suite shards

The CI full suite combines release-build in-ROM tests, whole-ROM replays, and
all replay save/load variants into 12 deterministic shards. Each runner uses
one emulator at a time. In-ROM tests receive a fixed short-test estimate;
replay estimates come from the recorded replay duration and are scaled for the
extra playback or restart work in each save-state variant. Longest-estimated
tests are assigned to the currently lightest shard, with task IDs and shard
numbers used as stable tie-breakers.

Inspect the complete assignment or run one shard locally with:

```bash
just test-shard-plan 12
just test-shard 5/12
```

To reproduce one task without running the rest of its shard, use its ID from
the plan:

```bash
python3 scripts/run_test_shard.py \
  --task save-state/us/default/07-frigate
```

## Existing guest replay tests

The in-ROM replay feature tests remain available through commands such as:

```bash
just test REPLAY_DAM
just test-debug REPLAY_RUNWAY jp
```

`REPLAY_FIXTURES` in `scripts/run_practice_tests.py` maps those existing test
cases to the new regional recordings. The runner copies the selected SRAM image
beside its temporary ROM, validates its recorded region, and relocates legacy
`0x600` replay data to the practice replay slot when necessary. Full-suite runs
skip guest replay tests which have no fixture for that region; explicitly
requesting one is an error.

Guest replay tests use progress-based timeouts rather than a maximum replay
duration. The ROM reports `REPLAY_STARTED` when playback begins and emits a
`REPLAY_STATUS frame=current/total` heartbeat once per emulated second. The
runner stops ares if either playback does not start or no new heartbeat arrives
for 30 seconds, while allowing healthy long recordings to finish.

To add another whole-ROM fixture, add its `.ram` and populated `.state` files to
the appropriate regional directory. To add a new guest feature test as well,
add its test constant and behavior in `src/practice/practice_tests.c`, then map
the constant to the regional `.ram` file in `REPLAY_FIXTURES`.

## Replay save/load-state test

`REPLAY_RUNWAY_SAVE_STATES` embeds the selected replay in the ROM so save-state
SRAM writes cannot overwrite it. After one second of playback, the test repeats
this sequence until the replay completes:

1. Hold the pause-hotkey trigger for three frames, save, hold for three more
   frames, and release.
2. Play 60 replay frames, hold the trigger for one frame, then load while
   seeking the replay cursor back to the saved timestamp.
3. Hold the trigger for three more frames, release, replay the same 60 frames,
   then begin the next save cycle.

The test fails on replay RNG divergence, an incomplete replay, or a replay
cursor mismatch between the original and reloaded 60-frame segment. Run the
US Runway fixture with `just test-runway-save-states`, or
run every `.ram` file in one regional directory with:

```bash
just test-replay-save-states us tests/replays/us
just test-replay-save-states eu tests/replays/eu
just test-replay-save-states jp tests/replays/jp
```

The timings and optional external cameras can be varied through environment
variables. Values omitted here retain the one-second spacing, three-frame wait,
and one-second pre-load duration defaults:

```bash
REPLAY_SAVE_STATE_SPACING_SECONDS=10 \
REPLAY_SAVE_STATE_WAIT_FRAMES=4 \
REPLAY_SAVE_STATE_DURATION_SECONDS=30 \
just test-replay-save-states us tests/replays/us

REPLAY_SAVE_STATE_GRENADE_CAM=1 \
REPLAY_SAVE_STATE_HOSTAGE_CAM=1 \
just test-replay-save-states us tests/replays/us
```
