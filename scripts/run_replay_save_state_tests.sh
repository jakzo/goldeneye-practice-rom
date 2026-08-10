#!/usr/bin/env bash

set -euo pipefail

replay_dir="${1:?replay directory required}"
version="${2:-US}"
filter="${REPLAY_SAVE_STATE_FILTER:-}"
exclude="${REPLAY_SAVE_STATE_EXCLUDE:-}"
skip_build="${REPLAY_SAVE_STATE_SKIP_BUILD:+--skip-build}"
spacing_seconds="${REPLAY_SAVE_STATE_SPACING_SECONDS:-1}"
wait_frames="${REPLAY_SAVE_STATE_WAIT_FRAMES:-3}"
duration_seconds="${REPLAY_SAVE_STATE_DURATION_SECONDS:-1}"
camera_flags=0
matched=0

if [ -n "${REPLAY_SAVE_STATE_GRENADE_CAM:-}" ]; then
    camera_flags=$((camera_flags | 1))
fi
if [ -n "${REPLAY_SAVE_STATE_HOSTAGE_CAM:-}" ]; then
    camera_flags=$((camera_flags | 2))
fi
if ((spacing_seconds < 1 || spacing_seconds > 255 ||
     wait_frames < 1 || wait_frames > 255 ||
     duration_seconds < 1 || duration_seconds > 255)); then
    echo "error: replay save-state timings must be between 1 and 255" >&2
    exit 2
fi
test_param=$((spacing_seconds | wait_frames << 8 | duration_seconds << 16 | camera_flags << 24))

shopt -s nullglob
replays=("$replay_dir"/*.ram)
if [ "${#replays[@]}" -eq 0 ]; then
    echo "error: no .ram replays found in $replay_dir" >&2
    exit 2
fi

for replay in "${replays[@]}"; do
    name="$(basename "$replay" .ram)"
    if [ -n "$filter" ] && [ "$name" != "$filter" ]; then
        continue
    fi
    if [ -n "$exclude" ] && [ "$name" = "$exclude" ]; then
        continue
    fi

    echo "=== replay save/load states: $name ==="
    ./scripts/run_practice_tests.py \
        --test REPLAY_RUNWAY_SAVE_STATES \
        --replay-fixture "$replay" \
        --version "$version" \
        --test-param "$test_param" \
        --timeout 1200 \
        $skip_build
    skip_build="--skip-build"
    matched=$((matched + 1))
done

if [ "$matched" -eq 0 ]; then
    echo "error: no replay matched the requested filter" >&2
    exit 2
fi
