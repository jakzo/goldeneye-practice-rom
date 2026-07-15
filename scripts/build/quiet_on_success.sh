#!/bin/sh

# Legacy game code produces a large number of known compiler warnings. Keep
# successful builds quiet, but replay every diagnostic if compilation fails.
diagnostics=$(mktemp "${TMPDIR:-/tmp}/goldeneye-build-diagnostics.XXXXXX") || exit 1
trap 'rm -f "$diagnostics"' 0

"$@" 2>"$diagnostics"
status=$?

if [ "$status" -eq 0 ]; then
    exit 0
fi

cat "$diagnostics" >&2
exit "$status"
