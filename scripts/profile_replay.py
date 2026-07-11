#!/usr/bin/env python3
import os
import shutil
import subprocess
import sys
from pathlib import Path

ROOT = Path(__file__).resolve().parent.parent
ROM = ROOT / "build/u/ge007.u.z64"
FIXTURE = ROOT / "tests/replays/runway.ram"
OUTPUT = ROOT / "docs/profile_runway_base.csv"


def main():
    emulator = os.environ.get("ARES") or shutil.which("ares")
    if not emulator:
        raise SystemExit("ares was not found; set ARES to its executable")
    if not ROM.exists():
        raise SystemExit("build the ROM with PROFILE_REPLAY=1 first")

    shutil.copyfile(FIXTURE, ROM.with_suffix(".ram"))
    command = [emulator, *os.environ.get("ARES_ARGS", "").split(), str(ROM)]
    process = subprocess.Popen(
        command, cwd=ROOT, stdout=subprocess.PIPE, stderr=subprocess.STDOUT,
        text=True, errors="replace"
    )
    rows = []
    assert process.stdout is not None
    try:
        for raw_line in process.stdout:
            line = raw_line.replace("\0", "").rstrip()
            print(line, flush=True)
            if line.startswith("PROFILE_CSV,"):
                rows.append(line.removeprefix("PROFILE_CSV,"))
            if "TEST_FAILED" in line:
                raise RuntimeError(line)
            if "TEST_COMPLETE" in line:
                break
    finally:
        process.terminate()
        try:
            process.wait(timeout=5)
        except subprocess.TimeoutExpired:
            process.kill()
            process.wait()

    if len(rows) != 690:
        raise RuntimeError(f"expected header plus 689 frames, got {len(rows)} rows")
    OUTPUT.parent.mkdir(parents=True, exist_ok=True)
    OUTPUT.write_text("\n".join(rows) + "\n")
    print(f"wrote {OUTPUT.relative_to(ROOT)}")


if __name__ == "__main__":
    try:
        main()
    except (OSError, RuntimeError) as error:
        print(f"error: {error}", file=sys.stderr)
        raise SystemExit(1)
