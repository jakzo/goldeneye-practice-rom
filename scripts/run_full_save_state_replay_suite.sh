#!/usr/bin/env bash

set -euo pipefail

step="${1:-all}"
replay="${2:-}"
region="${3:-us}"
region="$(printf '%s' "$region" | tr '[:upper:]' '[:lower:]')"
case "$region" in
    us) version=US; region_code=u ;;
    eu) version=EU; region_code=e ;;
    jp) version=JP; region_code=j ;;
    *)
        echo "error: region must be one of: us, eu, jp" >&2
        exit 2
        ;;
esac
replay_dir="tests/replays/$region"
save_state_built=0
suite_jobs="${REPLAY_SAVE_STATE_JOBS:-3}"

steps=(regular default long cameras near-end cold-restart)

usage() {
    echo "usage: $0 [all|regular|default|long|cameras|near-end|cold-restart] [REPLAY] [us|eu|jp]" >&2
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
        echo "error: $version replay fixture not found: $replay" >&2
        exit 2
    fi
fi

run_regular() {
    local -a replay_args=()

    echo "=== full replay suite step: regular ==="
    if [ -n "${FULL_SAVE_STATE_SUITE_DIRECT:-}" ]; then
        if [ -n "$replay" ]; then
            replay_args=(--replay "$replay")
        fi
        python3 scripts/patch_practice_rom.py \
            "build/$region_code/ge007.$region_code.z64" --flags 1
        python3 ares/tests/n64-replay/run.py \
            --ares "${ARES:-ares}" \
            --rom "build/$region_code/ge007.$region_code.z64" \
            --elf "build/$region_code/ge007.$region_code.elf" \
            --fixture-dir tests/replays \
            --region "$region" \
            --jobs "$suite_jobs" \
            --artifacts "build/practice-replay-results/$region" \
            "${replay_args[@]}"
    else
        just test-practice-replays "$region" build/practice-replay-results "$replay"
    fi
}

run_save_state() {
    local selected_step="$1"
    local selected_replay="$replay"
    local -a environment=(
        REPLAY_SAVE_STATE_BUILD_MODE=release
        REPLAY_SAVE_STATE_JOBS="$suite_jobs"
    )

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
                if [ -f "$replay_dir/02-facility.ram" ]; then
                    selected_replay="02-facility"
                else
                    local fallback_replay
                    for fallback_replay in "$replay_dir"/*.ram; do
                        selected_replay="$(basename "$fallback_replay" .ram)"
                        break
                    done
                fi
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
    if [ -n "${FULL_SAVE_STATE_SUITE_DIRECT:-}" ]; then
        env "${environment[@]}" ./scripts/run_replay_save_state_tests.sh \
            "$replay_dir" "$version"
    else
        env "${environment[@]}" just test-replay-save-states \
            "$region" "$replay_dir"
    fi
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
