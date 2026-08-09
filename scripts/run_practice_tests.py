#!/usr/bin/env python3

import argparse
import os
import queue
import re
import shlex
import shutil
import signal
import subprocess
import sys
import tempfile
import threading
import time
import xml.etree.ElementTree as ET
from collections import deque
from concurrent.futures import ThreadPoolExecutor, as_completed
from dataclasses import dataclass
from pathlib import Path


WARNING_PREFIX = "WARN: "
ERROR_PREFIX = "ERROR: "
DEFAULT_TEST_TIMEOUT_SECONDS = 90
MINIMUM_TEST_TIMEOUT_SECONDS = {
    "FIRE_SLOWMO": 180,
    "REPLAY": 180,
    "REPLAY_DAM": 300,
    "REPLAY_FRIGATE": 360,
    "REPLAY_RUNWAY_SAVE_STATES": 600,
}
SRAM_SIZE_BYTES = 128 * 1024
REPLAY_HEADER_OFFSET = 0x280
LEGACY_REPLAY_HEADER_OFFSET = 0x600
REPLAY_MAGIC = 0x47455250
REPLAY_REGIONS = {"US": 1, "JP": 2, "EU": 3}
SAVE_STATE_MAGIC = 0x47455353
TEST_SAVE_STATE_SRAM_OFFSET = 0x280
TEST_REPLAY_ROM_OFFSET = 0x00FE0000
ROM_CONFIG_OFFSET = 0x00FFFFC0

ROOT = Path(__file__).resolve().parent.parent
TESTS_FILE = ROOT / "src/practice/practice_tests.c"
PATCH_ROM_SCRIPT = ROOT / "scripts/patch_practice_rom.py"
REPLAY_FIXTURES = {
    "US": {
        "REPLAY_DAM": ROOT / "tests/replays/us/01-dam.ram",
        "REPLAY_RUNWAY": ROOT / "tests/replays/us/03-runway.ram",
        "REPLAY_RUNWAY_SAVE_STATES": ROOT / "tests/replays/us/03-runway.ram",
        "REPLAY_FRIGATE": ROOT / "tests/replays/us/07-frigate.ram",
        "REPLAY_GRENADE_CAM": ROOT / "tests/replays/us/03-runway.ram",
        "REPLAY_ARCHIVES": ROOT / "tests/replays/us/11-archives.ram",
        "REPLAY_ARCHIVES_04X": ROOT / "tests/replays/us/11-archives.ram",
        "REPLAY_ARCHIVES_HOTKEYS": ROOT / "tests/replays/us/11-archives.ram",
    },
    "EU": {
        "REPLAY_ARCHIVES": ROOT / "tests/replays/eu/11-archives.ram",
        "REPLAY_ARCHIVES_04X": ROOT / "tests/replays/eu/11-archives.ram",
        "REPLAY_ARCHIVES_HOTKEYS": ROOT / "tests/replays/eu/11-archives.ram",
    },
    "JP": {
        "REPLAY_DAM": ROOT / "tests/replays/jp/01-dam.ram",
        "REPLAY_RUNWAY": ROOT / "tests/replays/jp/03-runway.ram",
    },
}
MANUAL_TEST_CASES = {"REPLAY_RUNWAY_SAVE_STATES"}
PRINT_LOCK = threading.Lock()
COLOR_RESET = "\033[0m"
TEST_COLOR_CODES = (
    39,
    214,
    82,
    207,
    220,
    75,
    203,
    170,
    45,
    113,
    141,
    208,
    51,
    197,
    118,
    111,
)
TEST_COLORS = {}
COLOR_OUTPUT = False
PREFIX_OUTPUT = True


@dataclass
class TestResult:
    name: str
    passed: bool
    detail: str
    duration: float
    output: str = ""


def parse_args():
    parser = argparse.ArgumentParser(
        description="Build and run GoldenEye practice ROM tests."
    )
    parser.add_argument(
        "--test",
        metavar="TEST_CASE",
        help="run only the named test case",
    )
    parser.add_argument(
        "--exclude",
        action="append",
        default=[],
        metavar="TEST_CASE",
        help="exclude a named test case (may be specified more than once)",
    )
    parser.add_argument(
        "--junit-xml",
        metavar="PATH",
        type=Path,
        help="write test results as JUnit XML",
    )
    parser.add_argument(
        "--wait-for-emulator-exit",
        action="store_true",
        help=(
            "wait for a successful emulator exit after the replay "
            "(used when ares must flush profiler output)"
        ),
    )
    parser.add_argument(
        "--build-mode",
        choices=("dev", "release"),
        default="dev",
        help="build mode for the test ROM (default: dev)",
    )
    parser.add_argument(
        "--skip-build",
        action="store_true",
        help="use an existing test ROM instead of building it",
    )
    parser.add_argument(
        "--rom-path",
        metavar="PATH",
        type=Path,
        help="test this ROM path (implies --skip-build)",
    )
    parser.add_argument(
        "--version",
        choices=("US", "EU", "JP"),
        default="US",
        help="ROM region version to build and test (default: US)",
    )
    parser.add_argument(
        "--test-param",
        type=int,
        default=0,
        metavar="VALUE",
        help="integer parameter exposed to the selected ROM test",
    )
    parser.add_argument(
        "--state-fixture",
        type=Path,
        metavar="PATH",
        help="inject a raw save-state fixture into the test SRAM image",
    )
    parser.add_argument(
        "--replay-fixture",
        type=Path,
        metavar="PATH",
        help="use an explicit replay SRAM fixture instead of the configured one",
    )
    parser.add_argument(
        "--output-state",
        type=Path,
        metavar="PATH",
        help="extract the raw save state written by a successful test",
    )
    parser.add_argument(
        "--output-state-dir",
        type=Path,
        metavar="PATH",
        help="extract every emitted interval state into this directory",
    )
    parser.add_argument(
        "--timeout",
        type=positive_int,
        default=DEFAULT_TEST_TIMEOUT_SECONDS,
        metavar="SECONDS",
        help=f"timeout for each test (default: {DEFAULT_TEST_TIMEOUT_SECONDS}s)",
    )
    parser.add_argument(
        "-j",
        "--jobs",
        type=positive_int,
        default=(
            os.environ.get("PRACTICE_TEST_JOBS")
            or default_test_job_count()
        ),
        help="number of tests to run in parallel (default: half the available CPUs)",
    )
    parser.add_argument(
        "--color",
        choices=("auto", "always", "never"),
        default="auto",
        help="color test prefixes (default: auto)",
    )
    return parser.parse_args()


def positive_int(value):
    try:
        parsed = int(value)
    except ValueError as error:
        raise argparse.ArgumentTypeError("must be an integer") from error
    if parsed < 1:
        raise argparse.ArgumentTypeError("must be at least 1")
    return parsed


def available_cpu_count():
    try:
        count = len(os.sched_getaffinity(0))
        return count or 1
    except (AttributeError, OSError):
        return os.cpu_count() or 1


def default_test_job_count():
    return max(1, available_cpu_count() // 2)


def configure_test_colors(test_cases, color_mode):
    global COLOR_OUTPUT, PREFIX_OUTPUT, TEST_COLORS

    PREFIX_OUTPUT = len(test_cases) > 1

    if color_mode == "always":
        COLOR_OUTPUT = True
    elif color_mode == "never" or "NO_COLOR" in os.environ:
        COLOR_OUTPUT = False
    else:
        term = os.environ.get("TERM", "")
        COLOR_OUTPUT = sys.stdout.isatty() or (term and term != "dumb")

    TEST_COLORS = {
        test_case: TEST_COLOR_CODES[index % len(TEST_COLOR_CODES)]
        for index, test_case in enumerate(test_cases)
    }


def read_test_cases():
    start_marker = "// --- start test cases ---"
    end_marker = "// --- end test cases ---"
    define_pattern = re.compile(r"^\s*#define\s+([A-Za-z_][A-Za-z0-9_]*)\b")
    test_cases = []
    inside_test_cases = False
    found_start = False
    found_end = False

    for line in TESTS_FILE.read_text().splitlines():
        if line.strip() == start_marker:
            found_start = True
            inside_test_cases = True
            continue
        if line.strip() == end_marker:
            found_end = True
            inside_test_cases = False
            break
        if inside_test_cases:
            match = define_pattern.match(line)
            if match:
                test_cases.append(match.group(1))

    if not found_start or not found_end:
        raise ValueError(
            f"test case markers are missing from {TESTS_FILE}"
        )
    if not test_cases:
        raise ValueError(f"no test case defines found in {TESTS_FILE}")
    if len(test_cases) != len(set(test_cases)):
        raise ValueError(f"duplicate test case defines found in {TESTS_FILE}")

    return test_cases


def emulator_command():
    extra_args = shlex.split(os.environ.get("ARES_ARGS", ""))
    configured_emulator = os.environ.get("ARES")
    if configured_emulator:
        return [configured_emulator, *extra_args]

    emulator = shutil.which("ares")
    if emulator:
        return [emulator, *extra_args]

    macos_emulator = Path("/Applications/ares.app/Contents/MacOS/ares")
    if macos_emulator.is_file():
        return [str(macos_emulator), *extra_args]

    return None


def build_tests(build_mode, version):
    jobs = available_cpu_count()
    command = ["make", f"-j{jobs}", f"VERSION={version}"]
    if build_mode == "dev":
        command.append("DEV=1")
    with tempfile.TemporaryFile(mode="w+") as build_log:
        try:
            result = subprocess.run(
                command,
                cwd=ROOT,
                stdout=build_log,
                stderr=subprocess.STDOUT,
            )
        except OSError as error:
            print(f"Could not start build: {error}", file=sys.stderr)
            return False
        if result.returncode == 0:
            return True

        build_log.seek(0)
        print("Build failed:", file=sys.stderr)
        for line in deque(build_log):
            print(line, end="", file=sys.stderr)
        return False


def print_test_line(test_case, line, *, file=sys.stdout):
    if not PREFIX_OUTPUT:
        with PRINT_LOCK:
            print(line, file=file, flush=True)
        return

    prefix = f"[{test_case}]"
    if COLOR_OUTPUT:
        color = TEST_COLORS[test_case]
        prefix = f"\033[38;5;{color}m{prefix}{COLOR_RESET}"
    with PRINT_LOCK:
        print(f"{prefix} {line}", file=file, flush=True)


def select_test(test_case, rom, test_param=0, boot_level=None):
    command = [
        sys.executable,
        str(PATCH_ROM_SCRIPT),
        str(rom),
        "--test-case",
        test_case,
        "--test-param",
        str(test_param),
    ]
    if boot_level is not None:
        command.extend(("--boot-level", str(boot_level)))
    result = subprocess.run(
        command,
        cwd=ROOT,
        stdout=subprocess.PIPE,
        stderr=subprocess.STDOUT,
        text=True,
    )
    for line in result.stdout.splitlines():
        print_test_line(test_case, line)
    return result.returncode == 0


def is_fixture_replay_test(test_case):
    return test_case.startswith("REPLAY_")


def replay_fixture_for(test_case, version):
    return REPLAY_FIXTURES[version].get(test_case)


def install_replay_fixture(
    test_case, rom, version, state_fixture=None, replay_fixture=None
):
    fixture = replay_fixture or replay_fixture_for(test_case, version)
    if fixture is None:
        if is_fixture_replay_test(test_case):
            raise ValueError(
                f"no {version} replay fixture is configured for {test_case}"
            )
        return None

    sram = fixture.read_bytes()
    if len(sram) != SRAM_SIZE_BYTES:
        raise ValueError(f"{fixture.name} replay SRAM fixture has the wrong size")
    replay_header_offset = next(
        (
            offset
            for offset in (REPLAY_HEADER_OFFSET, LEGACY_REPLAY_HEADER_OFFSET)
            if int.from_bytes(sram[offset : offset + 4], "big") == REPLAY_MAGIC
        ),
        None,
    )
    if replay_header_offset is None:
        raise ValueError(f"{fixture.name} does not contain a practice replay")
    replay_size = int.from_bytes(
        sram[replay_header_offset + 8 : replay_header_offset + 12], "big"
    )
    if replay_size == 0 or replay_header_offset + replay_size > len(sram):
        raise ValueError(f"{fixture.name} replay has an invalid size")
    actual_region = sram[replay_header_offset + 0x10]
    expected_region = REPLAY_REGIONS[version]
    if actual_region != expected_region:
        raise ValueError(
            f"{fixture.name} is region {actual_region}, expected {expected_region} "
            f"for {version}"
        )
    boot_level = sram[replay_header_offset + 0x11]
    if test_case == "REPLAY_RUNWAY_SAVE_STATES":
        if TEST_REPLAY_ROM_OFFSET + replay_size > ROM_CONFIG_OFFSET:
            raise ValueError(f"{fixture.name} is too large for the test ROM slot")
        with rom.open("r+b") as rom_file:
            rom_file.seek(TEST_REPLAY_ROM_OFFSET)
            rom_file.write(
                sram[replay_header_offset : replay_header_offset + replay_size]
            )

    if replay_header_offset != REPLAY_HEADER_OFFSET:
        relocated = bytearray(sram)
        relocated[
            REPLAY_HEADER_OFFSET : REPLAY_HEADER_OFFSET + replay_size
        ] = sram[replay_header_offset : replay_header_offset + replay_size]
        sram = bytes(relocated)

    if state_fixture is not None:
        replay_end = REPLAY_HEADER_OFFSET + replay_size
        if (
            test_case != "REPLAY_RUNWAY_SAVE_STATES"
            and replay_end > TEST_SAVE_STATE_SRAM_OFFSET
        ):
            raise ValueError(
                f"{fixture.name} replay data overlaps the test state region"
            )
        state = state_fixture.read_bytes()
        if len(state) < 16:
            raise ValueError(f"{state_fixture.name} save-state fixture is too small")
        if int.from_bytes(state[0:4], "big") != SAVE_STATE_MAGIC:
            raise ValueError(f"{state_fixture.name} has an invalid save-state magic")
        declared_size = int.from_bytes(state[12:16], "big")
        if declared_size == 0:
            declared_size = int.from_bytes(state[6:8], "big")
        if declared_size != len(state):
            raise ValueError(
                f"{state_fixture.name} declares {declared_size} bytes, "
                f"but contains {len(state)}"
            )
        state_end = TEST_SAVE_STATE_SRAM_OFFSET + len(state)
        if state_end > SRAM_SIZE_BYTES:
            raise ValueError(f"{state_fixture.name} does not fit in test SRAM")
        combined = bytearray(sram)
        combined[TEST_SAVE_STATE_SRAM_OFFSET:state_end] = state
        sram = bytes(combined)

    rom.with_suffix(".ram").write_bytes(sram)
    return boot_level


def extract_test_save_state(output, output_path):
    size = None
    encoded = []
    complete = False
    for line in output.splitlines():
        if line.startswith("RUNWAY_STATE_BEGIN size="):
            size = int(line.partition("=")[2])
            encoded = []
            complete = False
        elif size is not None and line.startswith("RUNWAY_STATE_DATA "):
            encoded.append(line.partition(" ")[2])
        elif size is not None and line == "RUNWAY_STATE_END":
            complete = True
            break

    if size is None or not complete:
        raise ValueError("test did not emit a complete save state")
    try:
        state = bytes.fromhex("".join(encoded))
    except ValueError as error:
        raise ValueError("test emitted invalid save-state hex") from error
    if len(state) != size:
        raise ValueError(f"test emitted {len(state)} bytes, expected {size}")
    if len(state) < 16 or int.from_bytes(state[0:4], "big") != SAVE_STATE_MAGIC:
        raise ValueError("test emitted an invalid save-state header")
    declared_size = int.from_bytes(state[12:16], "big")
    if declared_size == 0:
        declared_size = int.from_bytes(state[6:8], "big")
    if declared_size != size:
        raise ValueError("emitted save-state header has the wrong size")
    output_path.parent.mkdir(parents=True, exist_ok=True)
    output_path.write_bytes(state)


def extract_interval_save_states(output, output_dir):
    size = None
    encoded = []
    pending_state = None
    written = []
    marker = re.compile(
        r"^RUNWAY_STATE_CAPTURED nominal=(\d+) timestamp=(\d+)$"
    )

    for line in output.splitlines():
        if line.startswith("RUNWAY_STATE_BEGIN size="):
            size = int(line.partition("=")[2])
            encoded = []
        elif size is not None and line.startswith("RUNWAY_STATE_DATA "):
            encoded.append(line.partition(" ")[2])
        elif size is not None and line == "RUNWAY_STATE_END":
            pending_state = bytes.fromhex("".join(encoded))
            if len(pending_state) != size:
                raise ValueError(
                    f"test emitted {len(pending_state)} bytes, expected {size}"
                )
            size = None
        else:
            match = marker.match(line)
            if match:
                if pending_state is None:
                    raise ValueError("capture marker has no preceding state")
                nominal = int(match.group(1))
                path = output_dir / f"{nominal:04d}.state"
                path.parent.mkdir(parents=True, exist_ok=True)
                path.write_bytes(pending_state)
                written.append(path)
                pending_state = None

    if size is not None or pending_state is not None:
        raise ValueError("test emitted an incomplete interval state")
    if not written:
        raise ValueError("test emitted no interval states")
    return written


def stream_output(process, output_queue):
    assert process.stdout is not None
    for line in process.stdout:
        output_queue.put(line)
    output_queue.put(None)


def stop_emulator(process):
    if process.poll() is not None:
        return

    process.terminate()
    try:
        process.wait(timeout=5)
    except subprocess.TimeoutExpired:
        process.kill()
        process.wait()


def run_test(
    test_case,
    command,
    rom,
    runtime_dir,
    stop_event,
    timeout,
    wait_for_emulator_exit,
):
    environment = os.environ.copy()
    environment["XDG_CONFIG_HOME"] = str(runtime_dir / "config")
    environment["XDG_DATA_HOME"] = str(runtime_dir / "data")
    process = subprocess.Popen(
        [*command, str(rom)],
        cwd=ROOT,
        env=environment,
        stdout=subprocess.PIPE,
        stderr=subprocess.STDOUT,
        text=True,
        bufsize=1,
    )
    output_queue = queue.Queue()
    output_thread = threading.Thread(
        target=stream_output, args=(process, output_queue), daemon=True
    )
    output_thread.start()
    deadline = time.monotonic() + timeout
    output = []
    test_completed = False

    try:
        while True:
            if stop_event.is_set():
                return False, "interrupted", "".join(output)

            remaining = deadline - time.monotonic()
            if remaining <= 0:
                return (
                    False,
                    f"timed out after {timeout}s",
                    "".join(output),
                )

            try:
                line = output_queue.get(timeout=min(remaining, 0.25))
            except queue.Empty:
                continue

            if line is None:
                status = process.wait()
                if wait_for_emulator_exit and status == 0:
                    detail = (
                        "completed and emulator exited"
                        if test_completed
                        else "emulator exited after profiled replay"
                    )
                    return True, detail, "".join(output)
                return (
                    False,
                    f"emulator exited with status {status}",
                    "".join(output),
                )

            output.append(line)
            if not line.startswith("RUNWAY_STATE_DATA "):
                print_test_line(test_case, line.rstrip("\r\n"))
            if line.startswith(WARNING_PREFIX) or line.startswith(ERROR_PREFIX):
                return False, line.strip(), "".join(output)
            if "TEST_FAILED" in line:
                return False, "failed", "".join(output)
            if "CRASH_END" in line:
                return False, "crashed", "".join(output)
            if "TEST_COMPLETE" in line:
                if wait_for_emulator_exit:
                    test_completed = True
                else:
                    return True, "completed", "".join(output)
    finally:
        stop_emulator(process)
        output_thread.join(timeout=1)


def start_video_capture(test_case):
    video_dir = os.environ.get("PRACTICE_TEST_VIDEO_DIR")
    if not video_dir:
        return None, None

    display = os.environ.get("DISPLAY")
    ffmpeg = shutil.which("ffmpeg")
    if not display or not ffmpeg:
        print(
            "warning: video capture requires DISPLAY and ffmpeg",
            file=sys.stderr,
        )
        return None, None

    output_dir = Path(video_dir)
    output_dir.mkdir(parents=True, exist_ok=True)
    video_path = output_dir / f"{test_case}.mp4"
    video_size = os.environ.get("PRACTICE_TEST_VIDEO_SIZE", "640x480")
    process = subprocess.Popen(
        [
            ffmpeg,
            "-hide_banner",
            "-loglevel",
            "error",
            "-y",
            "-f",
            "x11grab",
            "-draw_mouse",
            "0",
            "-framerate",
            "60",
            "-video_size",
            video_size,
            "-i",
            display,
            "-an",
            "-c:v",
            "libx264",
            "-preset",
            "ultrafast",
            "-crf",
            "20",
            "-pix_fmt",
            "yuv420p",
            str(video_path),
        ],
        stdout=subprocess.DEVNULL,
        stderr=subprocess.DEVNULL,
    )
    return process, video_path


def stop_video_capture(process):
    if process is None or process.poll() is not None:
        return

    process.send_signal(signal.SIGINT)
    try:
        process.wait(timeout=10)
    except subprocess.TimeoutExpired:
        process.terminate()
        try:
            process.wait(timeout=5)
        except subprocess.TimeoutExpired:
            process.kill()
            process.wait()


def run_test_case(
    test_case,
    command,
    rom_path,
    temp_dir,
    stop_event,
    timeout,
    wait_for_emulator_exit,
    version,
    test_param=0,
    state_fixture=None,
    output_state=None,
    replay_fixture=None,
    output_state_dir=None,
):
    timeout = max(timeout, MINIMUM_TEST_TIMEOUT_SECONDS.get(test_case, 0))
    test_dir = temp_dir / test_case
    test_dir.mkdir()
    rom = test_dir / rom_path.name
    shutil.copyfile(rom_path, rom)

    try:
        boot_level = install_replay_fixture(
            test_case, rom, version, state_fixture, replay_fixture
        )
    except (OSError, ValueError) as error:
        return TestResult(test_case, False, f"fixture setup failed: {error}", 0.0)

    print_test_line(test_case, "=== patching ===")
    if not select_test(test_case, rom, test_param, boot_level):
        return TestResult(test_case, False, "ROM patch failed", 0.0)

    if stop_event.is_set():
        return TestResult(test_case, False, "interrupted", 0.0)

    print_test_line(test_case, "=== running ===")
    video_process, video_path = start_video_capture(test_case)
    started_at = time.monotonic()
    try:
        passed, detail, output = run_test(
            test_case,
            command,
            rom,
            test_dir,
            stop_event,
            timeout,
            wait_for_emulator_exit,
        )
    except OSError as error:
        passed = False
        detail = f"could not start emulator: {error}"
        output = ""
    finally:
        stop_video_capture(video_process)
    duration = time.monotonic() - started_at
    if passed and video_path:
        video_path.unlink(missing_ok=True)
    if passed and output_state is not None:
        try:
            extract_test_save_state(output, output_state)
        except (OSError, ValueError) as error:
            passed = False
            detail = f"state extraction failed: {error}"
    if output_state_dir is not None:
        try:
            extract_interval_save_states(output, output_state_dir)
        except (OSError, ValueError) as error:
            passed = False
            detail = f"interval state extraction failed: {error}"
    return TestResult(test_case, passed, detail, duration, output)


def print_summary(results):
    print("\nTest summary")
    print("------------")
    for result in results:
        status = "PASS" if result.passed else "FAIL"
        print(
            f"{status:4}  {result.name}: {result.detail} "
            f"({result.duration:.2f}s)"
        )

    passed_count = sum(result.passed for result in results)
    print(f"\n{passed_count} passed, {len(results) - passed_count} failed")


def write_junit_xml(results, output_path):
    failures = sum(not result.passed for result in results)
    total_time = sum(result.duration for result in results)
    suite = ET.Element(
        "testsuite",
        {
            "name": "practice",
            "tests": str(len(results)),
            "failures": str(failures),
            "errors": "0",
            "time": f"{total_time:.3f}",
        },
    )
    for result in results:
        case = ET.SubElement(
            suite,
            "testcase",
            {
                "classname": "practice",
                "name": result.name,
                "time": f"{result.duration:.3f}",
            },
        )
        if not result.passed:
            failure = ET.SubElement(
                case,
                "failure",
                {"message": result.detail, "type": "PracticeTestFailure"},
            )
            failure.text = result.output
        elif result.output:
            ET.SubElement(case, "system-out").text = result.output

    tree = ET.ElementTree(suite)
    ET.indent(tree, space="  ")
    output_path.parent.mkdir(parents=True, exist_ok=True)
    tree.write(output_path, encoding="utf-8", xml_declaration=True)


def write_github_summary(results):
    summary_path = os.environ.get("GITHUB_STEP_SUMMARY")
    if not summary_path:
        return

    passed_count = sum(result.passed for result in results)
    lines = [
        f"**{passed_count} passed, {len(results) - passed_count} failed**",
        "",
        "<details>",
        "<summary>View individual test results</summary>",
        "",
        "| Test | Result | Duration |",
        "| --- | --- | ---: |",
    ]
    for result in results:
        status = "✅ Passed" if result.passed else "❌ Failed"
        lines.append(
            f"| `{result.name}` | {status} | {result.duration:.2f}s |"
        )
    lines.extend(["", "</details>"])

    with open(summary_path, "a", encoding="utf-8") as summary:
        summary.write("\n".join(lines) + "\n")


def print_github_annotations(results):
    if not os.environ.get("GITHUB_ACTIONS"):
        return

    for result in results:
        if not result.passed:
            detail = result.detail.replace("%", "%25")
            detail = detail.replace("\r", "%0D").replace("\n", "%0A")
            print(
                f"::error title=Practice test failed::{result.name}: {detail}"
            )


def main():
    args = parse_args()

    try:
        test_cases = read_test_cases()
    except (OSError, ValueError) as error:
        print(f"error: {error}", file=sys.stderr)
        return 1

    unknown_exclusions = sorted(set(args.exclude) - set(test_cases))
    if unknown_exclusions:
        print(
            f"error: unknown excluded test case(s): "
            f"{', '.join(unknown_exclusions)}",
            file=sys.stderr,
        )
        return 2

    if args.test:
        if args.test not in test_cases:
            print(
                f"error: unknown test case {args.test!r}; "
                f"choose from: {', '.join(test_cases)}",
                file=sys.stderr,
            )
            return 2
        test_cases = [args.test]

    if (
        args.test_param
        or args.state_fixture
        or args.replay_fixture
        or args.output_state
        or args.output_state_dir
    ) and not args.test:
        print(
            "error: parameterized replay/save-state options require --test",
            file=sys.stderr,
        )
        return 2

    if args.version != "US" and (
        args.test_param
        or args.state_fixture
        or args.output_state
        or args.output_state_dir
    ):
        print(
            "error: test parameters and raw save-state options are US-only",
            file=sys.stderr,
        )
        return 2

    if args.state_fixture and not args.state_fixture.is_absolute():
        args.state_fixture = ROOT / args.state_fixture
    if args.replay_fixture and not args.replay_fixture.is_absolute():
        args.replay_fixture = ROOT / args.replay_fixture
    if args.output_state and not args.output_state.is_absolute():
        args.output_state = ROOT / args.output_state
    if args.output_state_dir and not args.output_state_dir.is_absolute():
        args.output_state_dir = ROOT / args.output_state_dir

    excluded_tests = set(args.exclude)
    if not args.test:
        excluded_tests.update(MANUAL_TEST_CASES)
    test_cases = [test for test in test_cases if test not in excluded_tests]

    unavailable_replays = [
        test
        for test in test_cases
        if is_fixture_replay_test(test)
        and args.replay_fixture is None
        and replay_fixture_for(test, args.version) is None
    ]
    if args.test and unavailable_replays:
        available = ", ".join(sorted(REPLAY_FIXTURES[args.version]))
        print(
            f"error: no {args.version} replay fixture is configured for "
            f"{args.test}; available regional replay tests: {available}",
            file=sys.stderr,
        )
        return 2
    if unavailable_replays:
        print(
            f"Skipping replay tests without {args.version} fixtures: "
            f"{', '.join(unavailable_replays)}"
        )
        unavailable_replays = set(unavailable_replays)
        test_cases = [test for test in test_cases if test not in unavailable_replays]

    if not test_cases:
        print("error: all selected test cases were excluded", file=sys.stderr)
        return 2

    configure_test_colors(test_cases, args.color)

    command = emulator_command()
    if command is None:
        print(
            "error: ares was not found; install it, add it to PATH, or set ARES",
            file=sys.stderr,
        )
        return 1

    country_code = {"US": "u", "EU": "e", "JP": "j"}[args.version]
    rom_path = args.rom_path or ROOT / f"build/{country_code}/ge007.{country_code}.z64"
    if not rom_path.is_absolute():
        rom_path = ROOT / rom_path
    if args.skip_build or args.rom_path:
        if not rom_path.is_file():
            print(f"error: test ROM does not exist: {rom_path}", file=sys.stderr)
            return 1
    else:
        print("=== building test ROM ===", flush=True)
        if not build_tests(args.build_mode, args.version):
            return 1

    results_by_name = {}
    stop_event = threading.Event()
    try:
        worker_count = min(args.jobs, len(test_cases))
        print(f"=== running {len(test_cases)} tests with {worker_count} jobs ===")
        with tempfile.TemporaryDirectory(prefix="practice-tests-") as temp:
            temp_dir = Path(temp)
            with ThreadPoolExecutor(max_workers=worker_count) as executor:
                try:
                    futures = {
                        executor.submit(
                            run_test_case,
                            test_case,
                            command,
                            rom_path,
                            temp_dir,
                            stop_event,
                            args.timeout,
                            args.wait_for_emulator_exit,
                            args.version,
                            args.test_param,
                            args.state_fixture,
                            args.output_state,
                            args.replay_fixture,
                            args.output_state_dir,
                        ): test_case
                        for test_case in test_cases
                    }
                    for future in as_completed(futures):
                        test_case = futures[future]
                        try:
                            results_by_name[test_case] = future.result()
                        except Exception as error:
                            results_by_name[test_case] = TestResult(
                                test_case,
                                False,
                                f"runner error: {error}",
                                0.0,
                            )
                except KeyboardInterrupt:
                    stop_event.set()
                    raise
    except KeyboardInterrupt:
        stop_event.set()
        print("\nTest run interrupted.", file=sys.stderr)
        return 130

    results = [results_by_name[test_case] for test_case in test_cases]

    print_summary(results)
    if args.junit_xml:
        write_junit_xml(results, args.junit_xml)
        print(f"JUnit XML written to {args.junit_xml}")
    write_github_summary(results)
    print_github_annotations(results)
    return 0 if all(result.passed for result in results) else 1


if __name__ == "__main__":
    sys.exit(main())
