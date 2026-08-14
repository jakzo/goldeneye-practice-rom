#!/usr/bin/env python3

import argparse
import queue
import shutil
import subprocess
import sys
import threading
import time
from dataclasses import dataclass
from pathlib import Path

ROOT = Path(__file__).resolve().parent.parent
if str(ROOT) not in sys.path:
    sys.path.insert(0, str(ROOT))

from scripts import patch_practice_rom
from scripts import run_practice_tests


DEFAULT_ROM = ROOT / "build/u/ge007.u.z64"
DEFAULT_OUTPUT_DIR = ROOT / "build/sc64-tests"
DEFAULT_TIMEOUT_SECONDS = 90
DEBUG_RETRY_SECONDS = 1
SRAM_SIZE = 128 * 1024


@dataclass
class HardwareTestResult:
    name: str
    passed: bool
    detail: str
    duration: float
    output: str = ""


def parse_args():
    parser = argparse.ArgumentParser(
        description="Run practice ROM tests on a USB-connected SummerCart64."
    )
    parser.add_argument("--rom", type=Path, default=DEFAULT_ROM)
    parser.add_argument("--output-dir", type=Path, default=DEFAULT_OUTPUT_DIR)
    parser.add_argument("--test", help="run only this test case")
    parser.add_argument(
        "--exclude", action="append", default=[], help="exclude a test case"
    )
    parser.add_argument(
        "--timeout", type=int, default=DEFAULT_TIMEOUT_SECONDS
    )
    parser.add_argument(
        "--initial-upload",
        action="store_true",
        help="do not request a cooperative reboot for the first upload",
    )
    parser.add_argument(
        "--deployer", default="sc64deployer", help="sc64deployer executable"
    )
    return parser.parse_args()


def selected_tests(test=None, excluded=()):
    tests = run_practice_tests.read_test_cases()
    unknown = sorted(set(excluded) - set(tests))
    if unknown:
        raise ValueError(f"unknown excluded test(s): {', '.join(unknown)}")
    if test:
        if test not in tests:
            raise ValueError(f"unknown test {test!r}")
        return [test]
    excluded = set(excluded) | run_practice_tests.MANUAL_TEST_CASES
    return [name for name in tests if name not in excluded]


def prepare_test_rom(
    base_rom,
    output_dir,
    test_case,
    test_param=0,
    replay_fixture=None,
    config_flags=0,
    output_name=None,
):
    test_dir = output_dir / (output_name or test_case)
    if test_dir.exists():
        shutil.rmtree(test_dir)
    test_dir.mkdir(parents=True)
    rom = test_dir / base_rom.name
    shutil.copyfile(base_rom, rom)

    boot_level = run_practice_tests.install_replay_fixture(
        test_case, rom, "US", replay_fixture=replay_fixture
    )
    if boot_level is not None:
        config_flags |= 1
    config = patch_practice_rom.encode_config(
        patch_practice_rom.parse_test_cases()[test_case],
        patch_practice_rom.NO_BOOT_LEVEL
        if boot_level is None
        else boot_level,
        test_param,
        config_flags,
    )
    patch_practice_rom.patch_rom(rom, config)

    save = rom.with_suffix(".ram")
    if not save.exists():
        save.write_bytes(bytes(SRAM_SIZE))
    return rom, save


def upload_command(deployer, rom, save, reboot):
    command = [
        deployer,
        "upload",
        str(rom),
        "--save-type",
        "sram1m",
    ]
    if save is not None:
        command[3:3] = ["--save", str(save)]
    if reboot:
        command.append("--reboot")
    return command


def stream_lines(process, lines):
    assert process.stdout is not None
    for line in process.stdout:
        lines.put(line)
    lines.put(None)


def stop_process(process):
    if process.poll() is not None:
        return
    process.terminate()
    try:
        process.wait(timeout=2)
    except subprocess.TimeoutExpired:
        process.kill()
        process.wait()


def monitor_test(test_case, deployer, timeout, quiet_state_progress=False):
    timeout = max(
        timeout,
        run_practice_tests.MINIMUM_TEST_TIMEOUT_SECONDS.get(test_case, 0),
    )
    deadline = time.monotonic() + timeout
    started = time.monotonic()
    replay_test = run_practice_tests.is_replay_test(test_case)
    replay_started = False
    output = []
    while True:
        process = subprocess.Popen(
            [deployer, "debug", "--no-writeback"],
            cwd=ROOT,
            stdin=subprocess.PIPE,
            stdout=subprocess.PIPE,
            stderr=subprocess.STDOUT,
            text=True,
            bufsize=1,
        )
        lines = queue.Queue()
        reader = threading.Thread(target=stream_lines, args=(process, lines))
        reader.daemon = True
        reader.start()
        try:
            while True:
                remaining = deadline - time.monotonic()
                if remaining <= 0:
                    if replay_test:
                        detail = (
                            "no replay status update received"
                            if replay_started
                            else "replay did not start"
                        )
                        return HardwareTestResult(
                            test_case,
                            False,
                            f"{detail} within "
                            f"{run_practice_tests.REPLAY_STATUS_TIMEOUT_SECONDS}s",
                            time.monotonic() - started,
                        )
                    return HardwareTestResult(
                        test_case, False, f"timed out after {timeout}s",
                        time.monotonic() - started,
                    )
                try:
                    line = lines.get(timeout=min(remaining, 0.25))
                except queue.Empty:
                    continue
                if line is None:
                    break
                quiet_prefixes = (
                    "REPLAY_STATUS ",
                    "State saved ",
                    "State loaded",
                    "RUNWAY_STATE_SAVED ",
                    "RUNWAY_STATE_LOADED ",
                )
                if not (
                    quiet_state_progress and line.startswith(quiet_prefixes)
                ):
                    print(f"[{test_case}] {line}", end="", flush=True)
                output.append(line)
                if replay_test and (
                    "REPLAY_STARTED" in line or "REPLAY_STATUS" in line
                ):
                    replay_started = True
                    deadline = (
                        time.monotonic()
                        + run_practice_tests.REPLAY_STATUS_TIMEOUT_SECONDS
                    )
                if "TEST_FAILED" in line:
                    return HardwareTestResult(
                        test_case, False, "failed", time.monotonic() - started,
                        "".join(output),
                    )
                if "CRASH_END" in line:
                    return HardwareTestResult(
                        test_case, False, "crashed", time.monotonic() - started,
                        "".join(output),
                    )
                if "TEST_COMPLETE" in line:
                    return HardwareTestResult(
                        test_case, True, "completed", time.monotonic() - started,
                        "".join(output),
                    )
        finally:
            stop_process(process)
            reader.join(timeout=1)

        remaining = deadline - time.monotonic()
        if remaining <= 0:
            if replay_test:
                detail = (
                    "no replay status update received"
                    if replay_started
                    else "replay did not start"
                )
                return HardwareTestResult(
                    test_case,
                    False,
                    f"{detail} within "
                    f"{run_practice_tests.REPLAY_STATUS_TIMEOUT_SECONDS}s",
                    time.monotonic() - started,
                )
            return HardwareTestResult(
                test_case, False, f"timed out after {timeout}s",
                time.monotonic() - started,
            )
        time.sleep(min(DEBUG_RETRY_SECONDS, remaining))


def run(args):
    base_rom = args.rom.resolve()
    if not base_rom.is_file():
        print(f"error: test ROM not found: {base_rom}", file=sys.stderr)
        return 2
    if shutil.which(args.deployer) is None:
        print(f"error: deployer not found: {args.deployer}", file=sys.stderr)
        return 2
    try:
        tests = selected_tests(args.test, args.exclude)
    except ValueError as error:
        print(f"error: {error}", file=sys.stderr)
        return 2

    results = []
    for index, test_case in enumerate(tests):
        try:
            rom, save = prepare_test_rom(base_rom, args.output_dir, test_case)
        except (OSError, ValueError) as error:
            results.append(
                HardwareTestResult(test_case, False, f"setup failed: {error}", 0)
            )
            break

        reboot = not (args.initial_upload and index == 0)
        command = upload_command(args.deployer, rom, save, reboot)
        print(f"[{test_case}] === uploading{' and rebooting' if reboot else ''} ===")
        upload = subprocess.run(command, cwd=ROOT)
        if upload.returncode != 0:
            detail = "upload/reboot failed"
            if reboot:
                detail += "; the running ROM may need a manual Reset"
            results.append(HardwareTestResult(test_case, False, detail, 0))
            break

        result = monitor_test(test_case, args.deployer, args.timeout)
        results.append(result)
        if not result.passed:
            break

    print("\nSummerCart64 hardware test summary")
    for result in results:
        status = "PASS" if result.passed else "FAIL"
        print(
            f"{status:4} {result.name:36} {result.duration:7.2f}s "
            f"{result.detail}"
        )
    passed = sum(result.passed for result in results)
    print(f"{passed}/{len(results)} completed successfully")
    return 0 if len(results) == len(tests) and passed == len(results) else 1


def main():
    return run(parse_args())


if __name__ == "__main__":
    sys.exit(main())
