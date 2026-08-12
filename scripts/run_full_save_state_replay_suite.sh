#!/usr/bin/env bash

set -euo pipefail

step="${1:-all}"
replay="${2:-}"
replay_dir="tests/replays/us"
save_state_built=0

steps=(regular default long cameras near-end cold-restart)

usage() {
    echo "usage: $0 [all|regular|default|long|cameras|near-end|cold-restart] [REPLAY]" >&2
}

if [ "$step" != "all" ]; then
    valid=0
    for candidate in "${steps[@]}"; do
        if [ "$step" = "$candidate" ]; then
            valid=1
            break
        fi
    done
    if [ "$valid" -eq 0 ]; then
        usage
        exit 2
    fi
fi

if [ -n "$replay" ]; then
    replay="$(basename "$replay" .ram)"
    if [ ! -f "$replay_dir/$replay.ram" ]; then
        echo "error: US replay fixture not found: $replay" >&2
        exit 2
    fi
fi

run_regular() {
    echo "=== full replay suite step: regular ==="
    just test-practice-replays us build/practice-replay-results "$replay"
}

run_save_state() {
    local selected_step="$1"
    local selected_replay="$replay"
    local -a environment=(REPLAY_SAVE_STATE_JOBS=3)

    if [ "$save_state_built" -ne 0 ]; then
        environment+=(REPLAY_SAVE_STATE_SKIP_BUILD=1)
    fi

    case "$selected_step" in
        default)
            ;;
        long)
            environment+=(
                REPLAY_SAVE_STATE_SPACING_SECONDS=10
                REPLAY_SAVE_STATE_WAIT_FRAMES=4
                REPLAY_SAVE_STATE_DURATION_SECONDS=30
            )
            ;;
        cameras)
            environment+=(
                REPLAY_SAVE_STATE_GRENADE_CAM=1
                REPLAY_SAVE_STATE_HOSTAGE_CAM=1
            )
            ;;
        near-end)
            environment+=(REPLAY_SAVE_STATE_END_MARGIN_FRAMES=30)
            if [ -z "$selected_replay" ]; then
                selected_replay="02-facility"
            fi
            ;;
        cold-restart)
            environment+=(REPLAY_SAVE_STATE_RESTART_BETWEEN_LOADS=1)
            ;;
    esac

    if [ -n "$selected_replay" ]; then
        environment+=(REPLAY_SAVE_STATE_FILTER="$selected_replay")
    fi

    echo "=== full replay suite step: $selected_step ==="
    env "${environment[@]}" just test-replay-save-states us "$replay_dir"
    save_state_built=1
}

for selected_step in "${steps[@]}"; do
    if [ "$step" != "all" ] && [ "$step" != "$selected_step" ]; then
        continue
    fi
    if [ "$selected_step" = "regular" ]; then
        run_regular
    else
        run_save_state "$selected_step"
    fi
done
