#!/usr/bin/env bash

set -euo pipefail

replay_dir="${1:?replay directory required}"
version="${2:-US}"
filter="${REPLAY_SAVE_STATE_FILTER:-}"
exclude="${REPLAY_SAVE_STATE_EXCLUDE:-}"
skip_build="${REPLAY_SAVE_STATE_SKIP_BUILD:+--skip-build}"
matched=0

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
        --timeout 1200 \
        $skip_build
    skip_build="--skip-build"
    matched=$((matched + 1))
done

if [ "$matched" -eq 0 ]; then
    echo "error: no replay matched the requested filter" >&2
    exit 2
fi
