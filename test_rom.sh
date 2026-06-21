#!/usr/bin/env bash
# test_rom.sh - Test if the N64 ROM loads correctly by checking if updateFrameCounters is called
#
# Exit codes:
#   0 = ROM works (breakpoint hit within timeout)
#   1 = ROM is broken (breakpoint not hit within timeout)
#   2 = Setup/script error (not the ROM's fault)
#
# Usage: ./test_rom.sh

EMULATOR="/Applications/ares.app/Contents/MacOS/ares"
ROM="./build/u/ge007.u.z64"
GDB_PORT=9123
BREAKPOINT="bondviewIntroCameraTextTick"
TIMEOUT=10         # seconds to wait for breakpoint after GDB connects
GDB_SERVER_WAIT=30 # seconds to wait for emulator to start GDB server

GDB="/opt/homebrew/bin/mipsel-linux-gnu-gdb"
if [ ! -x "$GDB" ]; then
    GDB="mipsel-linux-gnu-gdb"
fi

# Create a secure unique temp file for GDB output to avoid stale file race conditions
GDB_OUT=$(mktemp /tmp/test_rom_gdb_XXXXXX.log)

# Colors for output
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[0;33m'
NC='\033[0m' # No Color

# Start the emulator in the background, passing through its stdout/stderr
echo "Starting emulator..."
"$EMULATOR" --system "Nintendo 64" "$ROM" &
EMULATOR_PID=$!

cleanup() {
    if kill -0 "$EMULATOR_PID" 2>/dev/null; then
        kill "$EMULATOR_PID" 2>/dev/null || true
        wait "$EMULATOR_PID" 2>/dev/null || true
    fi
    if kill -0 "$GDB_PID" 2>/dev/null; then
        kill "$GDB_PID" 2>/dev/null || true
        wait "$GDB_PID" 2>/dev/null || true
    fi
    rm -f "$GDB_OUT"
}
trap cleanup EXIT

# Wait for the emulator's GDB server to be available
echo -n "Waiting for GDB server on port $GDB_PORT..."
SERVER_READY=false
for i in $(seq 1 $GDB_SERVER_WAIT); do
    if nc -z localhost "$GDB_PORT" 2>/dev/null; then
        SERVER_READY=true
        break
    fi
    echo -n "."
    sleep 1
done

if [ "$SERVER_READY" = false ]; then
    echo ""
    echo -e "${YELLOW}SETUP ERROR: GDB server not available after ${GDB_SERVER_WAIT}s${NC}"
    echo "The emulator may have crashed or failed to start. Check the emulator output above."
    exit 2
fi
echo " ready"

# Small delay to let the emulator fully initialize
sleep 1

echo "Connecting GDB, setting breakpoint on '$BREAKPOINT'..."

# Run GDB in background.
# 1. Load the symbol file BEFORE connecting target remote to prevent interactive prompts.
# 2. Redirect stdin from /dev/null to prevent SIGTTIN suspension in background.
"$GDB" \
    -ex "file ./build/u/ge007.u.elf" \
    -ex "set pagination off" \
    -ex "set breakpoint pending on" \
    -ex "target remote localhost:${GDB_PORT}" \
    -ex "handle SIGSEGV nostop noprint pass" \
    -ex "handle SIGBUS nostop noprint pass" \
    -ex "handle SIGILL nostop noprint pass" \
    -ex "handle SIGFPE nostop noprint pass" \
    -ex "break ${BREAKPOINT}" \
    -ex "continue" \
    > "$GDB_OUT" 2>&1 < /dev/null &
GDB_PID=$!

# Wait for GDB to hit the breakpoint (it prints output and stops) or timeout
GDB_HIT=false
for i in $(seq 1 $TIMEOUT); do
    # Check if GDB hit the breakpoint
    if grep -q -E "Breakpoint [0-9]+,.*${BREAKPOINT}" "$GDB_OUT" 2>/dev/null; then
        GDB_HIT=true
        break
    fi
    sleep 1
done

echo -e "\nGDB output:"
cat "$GDB_OUT" | tail -20

# If GDB didn't hit the breakpoint, kill it
if [ "$GDB_HIT" = false ]; then
    kill "$GDB_PID" 2>/dev/null || true
    wait "$GDB_PID" 2>/dev/null || true
    echo -e "${RED}ROM: BROKEN (breakpoint '${BREAKPOINT}' not hit within ${TIMEOUT}s)${NC}"
    exit 1
fi

echo -e "${GREEN}ROM: WORKS${NC}"
exit 0
