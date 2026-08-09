# Practice-ROM replay test fixtures

The `us`, `eu`, and `jp` directories contain the same regional SRAM recordings
used by the base-ROM ares tests. Every `.ram` recording has a `.state` sidecar
whose header currently tracks `g_randomSeed` and `g_chrObjRandomSeed` and whose
binary payload contains their expected value for every frame.

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
2. Play 25 replay frames, hold the trigger for one frame, then load while
   seeking the replay cursor back to the saved timestamp.
3. Hold the trigger for three more frames, release, replay the same 25 frames,
   then begin the next save cycle.

The test fails on replay RNG divergence, an incomplete replay, or a replay
cursor mismatch between the original and reloaded 25-frame segment. Run the
Runway fixture headlessly in Docker with `just test-runway-save-states`, or run
all US `.ram` files in a directory with `just test-us-replay-save-states DIR`.
