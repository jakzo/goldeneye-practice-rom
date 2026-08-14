#!/usr/bin/env python3

import argparse
import re
import shutil
import subprocess
import sys
import time
from dataclasses import dataclass
from pathlib import Path

ROOT = Path(__file__).resolve().parent.parent
if str(ROOT) not in sys.path:
    sys.path.insert(0, str(ROOT))

from scripts import list_replay_save_state_frames
from scripts import patch_practice_rom
from scripts import run_practice_tests
from scripts import run_sc64_tests


TEST_CASE = "REPLAY_RUNWAY_SAVE_STATES"
TEST_CASE_ID = 29
BASE_REPLAY_FLAG = 0x1
RESTART_SAVE_STATE_FLAG = 0x2
REPLAY_STATE_EXACT_DURATION = 0x40000000
FIXTURE_DIR = ROOT / "tests/replays/us"
STEPS = ("default", "long", "cameras", "near-end", "cold-restart")
CHECKPOINT_RE = re.compile(
    r"^RUNWAY_RESTART_SEGMENT_COMPLETE timestamp=(\d+) elapsed=(\d+)$",
    re.MULTILINE,
)


@dataclass
class SuiteResult:
    step: str
    replay: str
    passed: bool
    detail: str
    duration: float


def parse_args():
    parser = argparse.ArgumentParser(
        description="Run save-state replay tests on a USB-connected SummerCart64."
    )
    parser.add_argument("--rom", type=Path, default=run_sc64_tests.DEFAULT_ROM)
    parser.add_argument(
        "--output-dir",
        type=Path,
        default=run_sc64_tests.DEFAULT_OUTPUT_DIR / "save-state-suite",
    )
    parser.add_argument("--step", choices=("all", *STEPS), default="all")
    parser.add_argument("--replay", help="run only one US replay fixture")
    parser.add_argument("--timeout", type=int, default=1200)
    parser.add_argument("--initial-upload", action="store_true")
    parser.add_argument("--deployer", default="sc64deployer")
    return parser.parse_args()


def replay_fixtures(selected=None):
    fixtures = sorted(FIXTURE_DIR.glob("*.ram"))
    if selected:
        name = Path(selected).stem
        fixtures = [fixture for fixture in fixtures if fixture.stem == name]
        if not fixtures:
            raise ValueError(f"US replay fixture not found: {name}")
    return fixtures


def test_param(step, replay):
    if step == "default":
        return 1 | (3 << 8) | (1 << 16)
    if step == "long":
        return 10 | (4 << 8) | (30 << 16)
    if step == "cameras":
        return 1 | (3 << 8) | (1 << 16) | (3 << 24)
    if step == "near-end":
        boundaries, duration = list_replay_save_state_frames.replay_frame_boundaries(
            replay
        )
        first_save = next(boundary for boundary in boundaries if boundary >= 60)
        exact_duration = duration - 30 - first_save
        if exact_duration < 1 or exact_duration > 0xFFFF:
            raise ValueError(f"near-end duration is out of range for {replay.name}")
        return exact_duration | (3 << 16) | REPLAY_STATE_EXACT_DURATION
    if step == "cold-restart":
        return 0
    raise ValueError(f"unknown save-state step: {step}")


def upload(deployer, rom, save, reboot):
    command = run_sc64_tests.upload_command(deployer, rom, save, reboot)
    return subprocess.run(command, cwd=ROOT).returncode == 0


def run_regular_step(args, step, replay, reboot):
    label = f"{step}/{replay.stem}"
    started = time.monotonic()
    rom, save = run_sc64_tests.prepare_test_rom(
        args.rom,
        args.output_dir,
        TEST_CASE,
        test_param=test_param(step, replay),
        replay_fixture=replay,
        output_name=label,
    )
    print(f"[{label}] === uploading and rebooting ===", flush=True)
    if not upload(args.deployer, rom, save, reboot):
        return SuiteResult(step, replay.stem, False, "upload/reboot failed", 0)
    result = run_sc64_tests.monitor_test(
        TEST_CASE, args.deployer, args.timeout, quiet_state_progress=True
    )
    return SuiteResult(
        step, replay.stem, result.passed, result.detail,
        time.monotonic() - started,
    )


def patch_restart_checkpoint(rom, boot_level, checkpoint):
    config = patch_practice_rom.encode_config(
        TEST_CASE_ID,
        boot_level,
        checkpoint,
        BASE_REPLAY_FLAG | RESTART_SAVE_STATE_FLAG,
    )
    patch_practice_rom.patch_rom(rom, config)


def run_cold_restart(args, replay, reboot):
    step = "cold-restart"
    label = f"{step}/{replay.stem}"
    started = time.monotonic()
    rom, save = run_sc64_tests.prepare_test_rom(
        args.rom,
        args.output_dir,
        TEST_CASE,
        replay_fixture=replay,
        config_flags=RESTART_SAVE_STATE_FLAG,
        output_name=label,
    )
    previous_timestamp = -1
    boot_count = 0

    while True:
        boot_count += 1
        print(
            f"[{label}] === hardware boot {boot_count} from "
            f"timestamp {max(previous_timestamp, 0)} ===",
            flush=True,
        )
        if not upload(args.deployer, rom, save if boot_count == 1 else None, reboot):
            return SuiteResult(
                step, replay.stem, False,
                f"boot {boot_count} upload/reboot failed",
                time.monotonic() - started,
            )
        reboot = True
        result = run_sc64_tests.monitor_test(
            TEST_CASE, args.deployer, args.timeout, quiet_state_progress=True
        )
        if not result.passed:
            return SuiteResult(
                step, replay.stem, False,
                f"boot {boot_count}: {result.detail}",
                time.monotonic() - started,
            )
        if "RUNWAY_STATE_REPLAY_COMPLETE" in result.output:
            return SuiteResult(
                step, replay.stem, True,
                f"completed after {boot_count} hardware boots",
                time.monotonic() - started,
            )
        match = CHECKPOINT_RE.search(result.output)
        if match is None:
            return SuiteResult(
                step, replay.stem, False,
                f"boot {boot_count} emitted no checkpoint",
                time.monotonic() - started,
            )
        timestamp, elapsed = (int(value) for value in match.groups())
        if timestamp <= previous_timestamp or timestamp > 0xFFFF or elapsed > 0xFFFF:
            return SuiteResult(
                step, replay.stem, False,
                f"boot {boot_count} emitted invalid checkpoint "
                f"timestamp={timestamp} elapsed={elapsed}",
                time.monotonic() - started,
            )
        previous_timestamp = timestamp
        checkpoint = timestamp | (elapsed << 16)
        boot_level = run_practice_tests.install_replay_fixture(
            TEST_CASE, rom, "US", replay_fixture=replay
        )
        patch_restart_checkpoint(rom, boot_level, checkpoint)


def print_summary(results):
    print("\nSummerCart64 save-state suite summary")
    for result in results:
        status = "PASS" if result.passed else "FAIL"
        print(
            f"{status:4} {result.step:12} {result.replay:16} "
            f"{result.duration:8.2f}s {result.detail}"
        )
    passed = sum(result.passed for result in results)
    print(f"{passed}/{len(results)} completed successfully")


def run(args):
    args.rom = args.rom.resolve()
    if not args.rom.is_file():
        print(f"error: test ROM not found: {args.rom}", file=sys.stderr)
        return 2
    if shutil.which(args.deployer) is None:
        print(f"error: deployer not found: {args.deployer}", file=sys.stderr)
        return 2
    try:
        fixtures = replay_fixtures(args.replay)
    except ValueError as error:
        print(f"error: {error}", file=sys.stderr)
        return 2

    steps = STEPS if args.step == "all" else (args.step,)
    results = []
    first_upload = True
    for step in steps:
        step_fixtures = fixtures
        if step == "near-end" and args.replay is None:
            step_fixtures = [FIXTURE_DIR / "02-facility.ram"]
        for replay in step_fixtures:
            reboot = not (args.initial_upload and first_upload)
            first_upload = False
            try:
                if step == "cold-restart":
                    result = run_cold_restart(args, replay, reboot)
                else:
                    result = run_regular_step(args, step, replay, reboot)
            except (OSError, ValueError, StopIteration) as error:
                result = SuiteResult(step, replay.stem, False, str(error), 0)
            results.append(result)
            if not result.passed:
                print_summary(results)
                return 1

    print_summary(results)
    return 0


if __name__ == "__main__":
    sys.exit(run(parse_args()))
