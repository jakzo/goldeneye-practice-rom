#!/usr/bin/env bash

set -euo pipefail

replay_dir="${1:?replay directory required}"
version="${2:-US}"
build_mode="${REPLAY_SAVE_STATE_BUILD_MODE:-release}"
filter="${REPLAY_SAVE_STATE_FILTER:-}"
exclude="${REPLAY_SAVE_STATE_EXCLUDE:-}"
skip_build="${REPLAY_SAVE_STATE_SKIP_BUILD:+--skip-build}"
spacing_seconds="${REPLAY_SAVE_STATE_SPACING_SECONDS:-1}"
wait_frames="${REPLAY_SAVE_STATE_WAIT_FRAMES:-3}"
duration_seconds="${REPLAY_SAVE_STATE_DURATION_SECONDS:-1}"
restart_between_loads="${REPLAY_SAVE_STATE_RESTART_BETWEEN_LOADS:+--restart-between-loads}"
end_margin_frames="${REPLAY_SAVE_STATE_END_MARGIN_FRAMES:-}"
jobs="${REPLAY_SAVE_STATE_JOBS:-1}"
camera_flags=0

case "$build_mode" in
    release) dev=0 ;;
    dev) dev=1 ;;
    *)
        echo "error: replay save-state build mode must be release or dev" >&2
        exit 2
        ;;
esac

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
if ((jobs < 1 || jobs > 3)); then
    echo "error: replay save-state jobs must be between 1 and 3" >&2
    exit 2
fi
if [ -n "$end_margin_frames" ]; then
    if ((end_margin_frames < 1 || end_margin_frames > 65535)); then
        echo "error: replay save-state end margin must be between 1 and 65535 frames" >&2
        exit 2
    fi
    if [ "$spacing_seconds" -ne 1 ] || [ "$duration_seconds" -ne 1 ] ||
       [ "$camera_flags" -ne 0 ] || [ -n "$restart_between_loads" ]; then
        echo "error: near-end mode cannot be combined with custom spacing, duration, cameras, or emulator restarts" >&2
        exit 2
    fi
fi

shopt -s nullglob
replays=("$replay_dir"/*.ram)
if [ "${#replays[@]}" -eq 0 ]; then
    echo "error: no .ram replays found in $replay_dir" >&2
    exit 2
fi

selected_replays=()
for replay in "${replays[@]}"; do
    name="$(basename "$replay" .ram)"
    if [ -n "$filter" ] && [ "$name" != "$filter" ]; then
        continue
    fi
    if [ -n "$exclude" ] && [ "$name" = "$exclude" ]; then
        continue
    fi

    selected_replays+=("$replay")
done

if [ "${#selected_replays[@]}" -eq 0 ]; then
    echo "error: no replay matched the requested filter" >&2
    exit 2
fi

should_multiplex_replays() {
    [ -z "$end_margin_frames" ] &&
        [ -z "$restart_between_loads" ] &&
        [ "$spacing_seconds" -eq 1 ] &&
        [ "$duration_seconds" -eq 1 ] &&
        [ "${#selected_replays[@]}" -gt 1 ]
}

run_replay() {
    local replay="$1"
    local build_arg="$2"
    local name
    local test_param
    local -a build_args=()

    name="$(basename "$replay" .ram)"
    if [ -n "$build_arg" ]; then
        build_args+=("$build_arg")
    fi
    if [ -n "$end_margin_frames" ]; then
        local frames_per_second=60
        if [ "$version" = "EU" ]; then
            frames_per_second=50
        fi
        local exact_duration
        exact_duration="$(python3 ./scripts/list_replay_save_state_frames.py \
            "$replay" \
            --spacing-frames "$frames_per_second" \
            --duration-before-end "$end_margin_frames")"
        test_param=$((exact_duration | wait_frames << 16 | 1 << 30))
    else
        test_param=$((spacing_seconds | wait_frames << 8 | duration_seconds << 16 | camera_flags << 24))
    fi

    echo "=== replay save/load states: $name ==="
    if [ -n "$restart_between_loads" ]; then
        ./scripts/run_practice_tests.py \
            --test REPLAY_RUNWAY_SAVE_STATES \
            --replay-fixture "$replay" \
            --version "$version" \
            --build-mode "$build_mode" \
            --restart-between-loads \
            --timeout 1200 \
            "${build_args[@]}"
    else
        ./scripts/run_practice_tests.py \
            --test REPLAY_RUNWAY_SAVE_STATES \
            --replay-fixture "$replay" \
            --version "$version" \
            --build-mode "$build_mode" \
            --test-param "$test_param" \
            --timeout 1200 \
            "${build_args[@]}"
    fi
}

run_multiplexed_replays() {
    local build_arg="$1"
    local test_param
    local timeout
    local replay
    local name
    local -a fixture_args=()
    local -a build_args=()

    if [ -n "$build_arg" ]; then
        build_args+=("$build_arg")
    fi
    test_param=$((spacing_seconds | wait_frames << 8 | duration_seconds << 16 | camera_flags << 24))
    timeout=$((1800 + 900 * ${#selected_replays[@]}))
    for replay in "${selected_replays[@]}"; do
        name="$(basename "$replay" .ram)"
        fixture_args+=(--replay-fixture "$replay")
        echo "=== multiplexed replay: $name ==="
    done
    echo "=== replay save/load states: ${#selected_replays[@]} fixtures ==="
    ./scripts/run_practice_tests.py \
        --test REPLAY_RUNWAY_SAVE_STATES \
        "${fixture_args[@]}" \
        --version "$version" \
        --build-mode "$build_mode" \
        --test-param "$test_param" \
        --timeout "$timeout" \
        "${build_args[@]}"
}

next_replay=0
if [ -z "$skip_build" ]; then
    build_jobs="${PRACTICE_TEST_BUILD_JOBS:-$(getconf _NPROCESSORS_ONLN 2>/dev/null || echo 1)}"
    echo "=== building replay save-state test ROM ==="
    make -j"$build_jobs" VERSION="$version" DEV="$dev" PRACTICE_TEST_ROM=1
    skip_build="--skip-build"
fi

if should_multiplex_replays; then
    run_multiplexed_replays "$skip_build"
    exit 0
fi

while [ "$next_replay" -lt "${#selected_replays[@]}" ]; do
    pids=()
    names=()
    for ((slot = 0; slot < jobs && next_replay < ${#selected_replays[@]}; slot++)); do
        replay="${selected_replays[$next_replay]}"
        name="$(basename "$replay" .ram)"
        run_replay "$replay" "$skip_build" 2>&1 | sed "s/^/[$name] /" &
        pids+=("$!")
        names+=("$name")
        next_replay=$((next_replay + 1))
    done

    failed=0
    for index in "${!pids[@]}"; do
        if ! wait "${pids[$index]}"; then
            echo "error: replay save/load state test failed: ${names[$index]}" >&2
            failed=1
        fi
    done
    if [ "$failed" -ne 0 ]; then
        exit 1
    fi
done
