#!/usr/bin/env bash

set -euo pipefail

display_number=99
display_socket="/tmp/.X11-unix/X${display_number}"
xvfb_log="/tmp/practice-tests-xvfb.log"
xvfb_pid=""

cleanup() {
    if [ -n "$xvfb_pid" ] && kill -0 "$xvfb_pid" 2>/dev/null; then
        kill "$xvfb_pid" 2>/dev/null || true
        wait "$xvfb_pid" 2>/dev/null || true
    fi
}
trap cleanup EXIT
trap 'exit 130' INT
trap 'exit 143' TERM

Xvfb ":${display_number}" \
    -screen 0 640x480x24 \
    -nolisten tcp \
    -ac \
    >"$xvfb_log" 2>&1 &
xvfb_pid=$!

for _ in $(seq 1 100); do
    if [ -S "$display_socket" ]; then
        break
    fi
    if ! kill -0 "$xvfb_pid" 2>/dev/null; then
        cat "$xvfb_log" >&2
        echo "error: Xvfb exited during startup" >&2
        exit 1
    fi
    sleep 0.1
done

if [ ! -S "$display_socket" ]; then
    cat "$xvfb_log" >&2
    echo "error: Xvfb did not become ready" >&2
    exit 1
fi

export DISPLAY=":${display_number}"
./scripts/run_practice_tests.py "$@"
