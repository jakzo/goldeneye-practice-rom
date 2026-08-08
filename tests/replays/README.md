# Replay test fixtures

Replay tests boot directly into the stage recorded in an SRAM dump, play its
inputs and frame timings, and fail if the replay ends early or its recorded RNG
seeds diverge from the game state.

## Adding a replay test from an SRAM dump

1. Record the replay with seed recording enabled. Finish the recording cleanly,
   then close the emulator or dump the cartridge SRAM so all pending writes are
   flushed. The resulting file must be a complete 128 KiB SRAM image.
2. Copy the dump into this directory with a descriptive `.ram` name. Keep the
   original dump intact until the new fixture has passed.
3. Add a unique test constant between the test-case markers in
   `src/practice/practice_tests.c`.
4. In that file, include the new constant in:
   - the playback request in `practice_tests_set_case`;
   - the recorded stage in `practice_tests_boot_level`;
   - `practice_tests_should_disable_intro`, so the replay controls the normal
     stage startup; and
   - the fixture-replay completion check in `practice_tests_frame`.
5. Map the test constant to the `.ram` fixture for the recording's region in
   `REPLAY_FIXTURES` in `scripts/run_practice_tests.py`. If a replay is long
   enough to exceed the default test timeout, also add it to
   `MINIMUM_TEST_TIMEOUT_SECONDS`.
6. Run the test:

   ```bash
   just test REPLAY_YOUR_TEST
   ```

The runner copies the matching regional fixture beside its temporary ROM and
validates that the replay header identifies the selected ROM region. Full-suite
runs skip replay tests which have no fixture for that region; explicitly
requesting such a replay is an error. Playback restores the replay's initial
seeds, checks the per-frame seeds when the recording contains them, and reports
`TEST_FAILED` on divergence or incomplete playback. A passing run ends with
`TEST_COMPLETE`.

For interactive debugging, use `just test-debug REPLAY_YOUR_TEST`. To exercise a
specific ROM region, pass it as the second argument, for example
`just test-debug REPLAY_YOUR_TEST eu`.

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
