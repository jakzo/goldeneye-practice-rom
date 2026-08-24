# Cross-level replay hop test status

Updated 2026-08-25. This is the handoff for save-state replay tests that keep
one emulator process alive while switching between replay fixtures and stages.

## Goal

Make the hop test pass across every US replay, with ordinary gameplay and prop
rendering, then run the remaining save-state replay suite. Test orchestration
must remain in the test build. Save-state corrections that apply to normal use
belong in the regular practice save-state code.

## Current state

The all-replay `default` step still fails, so the later suite steps were not
run. The workspace has been left in a usable state:

- All props are rendered normally. The temporary test-only graphics-command
  suppression and prop-category bisection have been removed.
- Temporary collision and model debug logging has been removed.
- The hop harness remains confined to the test build.
- Save-state changes retained in normal practice code address real model,
  allocation, prop, and render-state lifecycle problems.
- The test ROM and ordinary practice ROM both compile after cleanup. The 12
  host-side replay harness unit tests also pass.

A three-replay run passed while prop display lists were suppressed. That result
is useful evidence about the remaining graphics failure, but it is not a valid
test pass and the suppression is no longer present.

## Latest all-replay result

Run:

```sh
just test-save-state-replay-suite-step default
```

The six-fixture run progressed through Dam, Frigate, Runway, Frigate, Dam,
Surface 1, and back to Runway. During cross-stage teardown it trapped in
`sub_GAME_7F050DE8`, called by `objFree`, while traversing an invalid model
node/header. Subsequent fixes for transient character models and stale attached
props moved execution past that failure.

With those fixes and normal prop rendering, a later cross-lifecycle Runway load
can leave the graphics task deadlocked. GDB showed every CPU thread waiting,
with `curRSPTask` and `curRDPTask` both equal to `0x8004f2f0`. The RSP status was
`0xc0`, its PC was `0x567`, and the task referred to a graphics display list at
`0x800e39f0` of size `0x5a08`.

Temporary category bisection established:

- Rendering characters while skipping ordinary object props avoided that
  graphics deadlock long enough to reach a later failure.
- Enabling `PROP_TYPE_OBJ` rendering brought the deadlock back.
- The individual object prop or object subtype has not yet been isolated.

Do not restore the bisection as a fix. Ordinary props must render.

## Reduced reproduction

Build the test ROM:

```sh
docker run --rm -v /Users/jfield/oss/007:/home/dev -w /home/dev goldeneye-test \
  make -j8 DEV=0 VERSION=US COMPARE=0 PRACTICE_TEST_ROM=1 TEST_CASE=
```

Then run:

```sh
env \
  ARES=/Users/jfield/oss/007/ares/build_macos/desktop-ui/Release/ares.app/Contents/MacOS/ares \
  ARES_ARGS='--no-file-prompt --setting Audio/Driver=None --setting Input/Driver=None' \
  PRACTICE_REPLAY_STATUS_TIMEOUT=120 \
  python3 scripts/run_practice_tests.py \
    --test REPLAY_RUNWAY_SAVE_STATES \
    --replay-fixture tests/replays/us/01-dam.ram \
    --replay-fixture tests/replays/us/03-runway.ram \
    --replay-fixture tests/replays/us/02-facility.ram \
    --replay-fixture tests/replays/us/04-surface1.ram \
    --version US --build-mode release --test-param 66305 \
    --timeout 300 --skip-build
```

This approximately 18-second sequence is deterministic: Dam, Runway, Dam,
Surface 1, then Runway. ares exposes GDB on `localhost:9123`.

## Fixes retained

- A replay fixture is activated only after playback from the previous fixture
  has stopped, avoiding cross-stage ownership races.
- Replay stage, difficulty, and playback generation are tracked explicitly.
- Cross-stage loading uses a two-phase character teardown so shared body and
  head model definitions are normalized before saved characters are created.
- Character model graphs restore canonical parent, previous, LOD, switch, BSP,
  and external-head relationships instead of retaining pointers mutated by a
  previous stage lifecycle.
- Saved character allocation metadata identifies stable slot, body, head, and
  external-head RW-data positions.
- Model animation serialization handles models without usable RW data without
  dereferencing it.
- Transient character model shells are not passed to model traversal during
  teardown.
- Stale weapon or hat props still linked to a guard are detached before the
  original guard destructor traverses them. Valid attachments continue through
  the original gameplay path.
- Cross-stage state loads invalidate old render allocations and defer paused
  render refresh until the new lifecycle owns valid data.

These are normal save-state fixes, not test-only behavior.

## Secondary unresolved failure

Some reduced runs trap on a divide by zero in
`bondviewCalcUpdatePlayerCollision`, at the random out-of-bounds recovery
calculation. Instrumentation confirmed that the saved and restored Runway
collision tile offset, tile tail, and collision position matched exactly in
the observed runs. The collision pointer relocation theory is therefore not
supported. Treat this as unresolved and possibly downstream of the graphics or
lifecycle fault.

## Resume order

1. Run the reduced reproduction with all rendering enabled.
2. Use temporary test-only instrumentation to isolate the offending
   `PROP_TYPE_OBJ` slot and subtype. Remove that instrumentation afterward.
3. Inspect that object's model switch relations, deformation state, RW-data
   indices, and display-list pointers before the stuck task is submitted.
4. Put any genuine save-state lifecycle correction in non-test practice code.
   Do not suppress rendering or alter gameplay to make the test pass.
5. Re-run the reduced reproduction, then the complete `default` step.
6. If `default` passes, run `regular`, `long`, `cameras`, `near-end`, and
   `cold-restart` using the suite commands documented in `AGENTS.md`.

The remaining suite steps have not been run because `default` is still the
gate.
