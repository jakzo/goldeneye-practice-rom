#!/usr/bin/env python3
"""Run or verify a deterministic practice replay in ares."""

import argparse
import os
import selectors
import shlex
import shutil
import struct
import subprocess
import sys
import time
from pathlib import Path


ROOT = Path(__file__).resolve().parent.parent
REPLAY_SRAM_OFFSET = 0x600
REPLAY_MAGIC = 0x47455250
REPLAY_VERSION = 1
REGION_BUILD_CODES = {1: "u", 2: "j", 3: "e"}
VERSION_REGIONS = {"US": 1, "JP": 2, "EU": 3}


def emulator_command():
    configured = os.environ.get("ARES")
    if configured:
        emulator = configured
    else:
        emulator = shutil.which("ares")
        if not emulator:
            app = Path("/Applications/ares.app/Contents/MacOS/ares")
            emulator = str(app) if app.is_file() else None
    if not emulator:
        raise RuntimeError("ares was not found; set ARES to its executable")
    return [emulator, *shlex.split(os.environ.get("ARES_ARGS", ""))]


def replay_header(fixture):
    with fixture.open("rb") as stream:
        stream.seek(REPLAY_SRAM_OFFSET)
        header = stream.read(18)
    if len(header) != 18:
        raise RuntimeError(f"replay fixture is too small: {fixture}")
    magic, version, frame_count = struct.unpack_from(">IH6xI", header)
    if magic != REPLAY_MAGIC or version != REPLAY_VERSION:
        raise RuntimeError(
            f"invalid replay header: magic={magic:08x} version={version}"
        )
    region = header[16]
    if region not in REGION_BUILD_CODES:
        raise RuntimeError(f"unsupported replay region: {region}")
    return region, frame_count


def verify_profile_capture(frame_count):
    prefix = os.environ.get("ARES_N64_PROFILE_OUTPUT")
    if not prefix or os.environ.get("ARES_N64_PROFILE_REPLAY") != "1":
        return
    capture = Path(f"{prefix}-001-game-frames.csv")
    if not capture.is_file():
        raise RuntimeError(f"profiler capture is missing: {capture}")
    captured_frames = sum(1 for _ in capture.open("rb")) - 1
    if captured_frames != frame_count:
        raise RuntimeError(
            f"profiler captured {captured_frames} of {frame_count} replay frames"
        )


def prepare_replay(fixture, rom, region):
    if not rom.is_file():
        raise RuntimeError(
            f"ROM is missing; build it with REPLAY_PLAYBACK=1 first: {rom}"
        )
    sram = bytearray(fixture.read_bytes())
    if len(sram) != 128 * 1024:
        raise RuntimeError(f"replay fixture has the wrong size: {fixture}")
    sram[REPLAY_SRAM_OFFSET + 0x10] = region
    rom.with_suffix(".ram").write_bytes(sram)


def run_headed(command, rom):
    return subprocess.call([*command, "--no-file-prompt", str(rom)], cwd=ROOT)


def verify(command, rom, timeout):
    process = subprocess.Popen(
        [*command, "--no-file-prompt", str(rom)],
        cwd=ROOT,
        stdout=subprocess.PIPE,
        stderr=subprocess.STDOUT,
        text=True,
        errors="replace",
        bufsize=1,
    )
    completed = False
    selector = selectors.DefaultSelector()
    assert process.stdout is not None
    selector.register(process.stdout, selectors.EVENT_READ)
    deadline = time.monotonic() + timeout

    try:
        while time.monotonic() < deadline:
            if process.poll() is not None:
                if (process.returncode == 0 and
                        os.environ.get("ARES_N64_PROFILE_REPLAY") == "1"):
                    completed = True
                    return
                break
            for key, _ in selector.select(timeout=0.25):
                line = key.fileobj.readline().replace("\0", "").rstrip()
                if not line:
                    continue
                print(line, flush=True)
                if "TEST_FAILED" in line:
                    raise RuntimeError(line)
                if "TEST_COMPLETE" in line:
                    completed = True
                    return
        if not completed:
            raise RuntimeError(f"replay did not complete within {timeout:g} seconds")
    finally:
        selector.close()
        process.terminate()
        try:
            process.wait(timeout=5)
        except subprocess.TimeoutExpired:
            process.kill()
            process.wait()


def main():
    parser = argparse.ArgumentParser()
    parser.add_argument("replay", type=Path, help="128 KiB replay SRAM fixture")
    parser.add_argument(
        "--version",
        choices=VERSION_REGIONS,
        help="target ROM region (defaults to the fixture header's region)",
    )
    parser.add_argument(
        "--rom",
        type=Path,
        help="replay-enabled ROM (defaults to the fixture header's region)",
    )
    parser.add_argument(
        "--headed",
        action="store_true",
        help="open ares normally and leave it running after replay completion",
    )
    parser.add_argument("--timeout", type=float, default=60)
    args = parser.parse_args()

    fixture = args.replay.resolve()
    if not fixture.is_file():
        raise RuntimeError(f"replay fixture is missing: {fixture}")
    fixture_region, frame_count = replay_header(fixture)
    region = VERSION_REGIONS[args.version] if args.version else fixture_region
    build_code = REGION_BUILD_CODES[region]
    rom = args.rom.resolve() if args.rom else (
        ROOT / f"build/{build_code}/ge007.{build_code}.z64"
    )
    prepare_replay(fixture, rom, region)
    command = emulator_command()
    if args.headed:
        return run_headed(command, rom)
    verify(command, rom, args.timeout)
    verify_profile_capture(frame_count)
    print("Replay completed without divergence.")
    return 0


if __name__ == "__main__":
    try:
        raise SystemExit(main())
    except (OSError, RuntimeError) as error:
        print(f"error: {error}", file=sys.stderr)
        raise SystemExit(1)
