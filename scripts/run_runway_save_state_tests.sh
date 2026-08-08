#!/usr/bin/env bash

set -euo pipefail

exec ./scripts/run_practice_tests.py \
    --test REPLAY_RUNWAY_SAVE_STATES \
    --replay-fixture tests/replays/runway.ram \
    --timeout 1200 \
    "$@"
