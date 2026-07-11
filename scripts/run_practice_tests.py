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
SRAM_SIZE_BYTES = 128 * 1024

ROOT = Path(__file__).resolve().parent.parent
TESTS_FILE = ROOT / "src/practice/practice_tests.c"
PATCH_ROM_SCRIPT = ROOT / "scripts/patch_practice_rom.py"
RUNWAY_REPLAY_FIXTURE = ROOT / "tests/replays/runway.ram"
REPLAY_FIXTURE_TESTS = {
    "REPLAY_RUNWAY_1X",
    "REPLAY_RUNWAY_04X",
    "REPLAY_RUNWAY_HOTKEYS",
}
TEST_MIN_TIMEOUT_SECONDS = {"REPLAY_RUNWAY_04X": 180}
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
        "--version",
        choices=("US", "EU", "JP"),
        default="US",
        help="ROM region version to build and test (default: US)",
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
    global COLOR_OUTPUT, TEST_COLORS

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
    prefix = f"[{test_case}]"
    if COLOR_OUTPUT:
        color = TEST_COLORS[test_case]
        prefix = f"\033[38;5;{color}m{prefix}{COLOR_RESET}"
    with PRINT_LOCK:
        print(f"{prefix} {line}", file=file, flush=True)


def select_test(test_case, rom):
    result = subprocess.run(
        [
            sys.executable,
            str(PATCH_ROM_SCRIPT),
            str(rom),
            "--test-case",
            test_case,
        ],
        cwd=ROOT,
        stdout=subprocess.PIPE,
        stderr=subprocess.STDOUT,
        text=True,
    )
    for line in result.stdout.splitlines():
        print_test_line(test_case, line)
    return result.returncode == 0


def install_replay_fixture(test_case, rom):
    if test_case not in REPLAY_FIXTURE_TESTS:
        return

    sram = RUNWAY_REPLAY_FIXTURE.read_bytes()
    if len(sram) != SRAM_SIZE_BYTES:
        raise ValueError("Runway replay SRAM fixture has the wrong size")
    rom.with_suffix(".ram").write_bytes(sram)


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


def run_test(test_case, command, rom, runtime_dir, stop_event, timeout):
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
                return (
                    False,
                    f"emulator exited with status {process.wait()}",
                    "".join(output),
                )

            output.append(line)
            print_test_line(test_case, line.rstrip("\r\n"))
            if line.startswith(WARNING_PREFIX) or line.startswith(ERROR_PREFIX):
                return False, line.strip(), "".join(output)
            if "TEST_FAILED" in line:
                return False, "failed", "".join(output)
            if "CRASH_END" in line:
                return False, "crashed", "".join(output)
            if "TEST_COMPLETE" in line:
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


def run_test_case(test_case, command, rom_path, temp_dir, stop_event, timeout):
    test_dir = temp_dir / test_case
    test_dir.mkdir()
    rom = test_dir / rom_path.name
    shutil.copyfile(rom_path, rom)

    try:
        install_replay_fixture(test_case, rom)
    except (OSError, ValueError) as error:
        return TestResult(test_case, False, f"fixture setup failed: {error}", 0.0)

    timeout = max(timeout, TEST_MIN_TIMEOUT_SECONDS.get(test_case, 0))

    print_test_line(test_case, "=== patching ===")
    if not select_test(test_case, rom):
        return TestResult(test_case, False, "ROM patch failed", 0.0)

    if stop_event.is_set():
        return TestResult(test_case, False, "interrupted", 0.0)

    print_test_line(test_case, "=== running ===")
    video_process, video_path = start_video_capture(test_case)
    started_at = time.monotonic()
    try:
        passed, detail, output = run_test(
            test_case, command, rom, test_dir, stop_event, timeout
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
        "## Practice ROM test results",
        "",
        f"**{passed_count} passed, {len(results) - passed_count} failed**",
        "",
        "| Test | Result | Duration |",
        "| --- | --- | ---: |",
    ]
    for result in results:
        status = "✅ Passed" if result.passed else "❌ Failed"
        lines.append(
            f"| `{result.name}` | {status} | {result.duration:.2f}s |"
        )

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

    excluded_tests = set(args.exclude)
    test_cases = [test for test in test_cases if test not in excluded_tests]
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
    rom_path = ROOT / f"build/{country_code}/ge007.{country_code}.z64"
    if args.skip_build:
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
