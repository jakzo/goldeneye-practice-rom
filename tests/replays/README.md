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
5. Map the test constant to the `.ram` fixture in `REPLAY_FIXTURES` in
   `scripts/run_practice_tests.py`. If a replay is long enough to exceed the
   default test timeout, also add it to `MINIMUM_TEST_TIMEOUT_SECONDS`.
6. Run the test:

   ```bash
   just test REPLAY_YOUR_TEST
   ```

The runner copies the fixture beside its temporary ROM and updates the replay
header's region byte for the selected ROM version. Playback itself validates the
header, restores the replay's initial seeds, checks the per-frame seeds when the
recording contains them, and reports `TEST_FAILED` on divergence or incomplete
playback. A passing run ends with `TEST_COMPLETE`.

For interactive debugging, use `just test-debug REPLAY_YOUR_TEST`. To exercise a
specific ROM region, pass it as the second argument, for example
`just test-debug REPLAY_YOUR_TEST eu`.
