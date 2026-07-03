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
from dataclasses import dataclass
from pathlib import Path


TEST_COMPLETE = "TEST_COMPLETE"
TEST_FAILED = "TEST_FAILED"
TEST_TIMEOUT_SECONDS = 90

ROOT = Path(__file__).resolve().parent.parent
ROM = ROOT / "build/u/ge007.u.z64"
TESTS_FILE = ROOT / "src/practice/practice_tests.c"
PATCH_ROM_SCRIPT = ROOT / "scripts/patch_practice_rom.py"


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
        "--junit-xml",
        metavar="PATH",
        type=Path,
        help="write test results as JUnit XML",
    )
    return parser.parse_args()


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
        return [configured_emulator, *extra_args, str(ROM)]

    emulator = shutil.which("ares")
    if emulator:
        return [emulator, *extra_args, str(ROM)]

    macos_emulator = Path("/Applications/ares.app/Contents/MacOS/ares")
    if macos_emulator.is_file():
        return [str(macos_emulator), *extra_args, str(ROM)]

    return None


def build_tests():
    jobs = os.cpu_count() or 1
    command = ["make", f"-j{jobs}", "DEV=1"]
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


def select_test(test_case):
    result = subprocess.run(
        [
            sys.executable,
            str(PATCH_ROM_SCRIPT),
            str(ROM),
            "--test-case",
            test_case,
        ],
        cwd=ROOT,
    )
    return result.returncode == 0


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


def run_test(command):
    process = subprocess.Popen(
        command,
        cwd=ROOT,
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
    deadline = time.monotonic() + TEST_TIMEOUT_SECONDS
    output = []

    try:
        while True:
            remaining = deadline - time.monotonic()
            if remaining <= 0:
                return (
                    False,
                    f"timed out after {TEST_TIMEOUT_SECONDS}s",
                    "".join(output),
                )

            try:
                line = output_queue.get(timeout=remaining)
            except queue.Empty:
                return (
                    False,
                    f"timed out after {TEST_TIMEOUT_SECONDS}s",
                    "".join(output),
                )

            if line is None:
                return (
                    False,
                    f"emulator exited with status {process.wait()}",
                    "".join(output),
                )

            output.append(line)
            print(line, end="", flush=True)
            if TEST_FAILED in line:
                return False, "failed", "".join(output)
            if TEST_COMPLETE in line:
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

    if args.test:
        if args.test not in test_cases:
            print(
                f"error: unknown test case {args.test!r}; "
                f"choose from: {', '.join(test_cases)}",
                file=sys.stderr,
            )
            return 2
        test_cases = [args.test]

    command = emulator_command()
    if command is None:
        print(
            "error: ares was not found; install it, add it to PATH, or set ARES",
            file=sys.stderr,
        )
        return 1

    print("=== building test ROM ===", flush=True)
    if not build_tests():
        return 1

    results = []
    try:
        for test_case in test_cases:
            print(f"\n=== {test_case}: patching ===", flush=True)
            if not select_test(test_case):
                results.append(
                    TestResult(test_case, False, "ROM patch failed", 0.0)
                )
                continue

            print(f"=== {test_case}: running ===", flush=True)
            video_process, video_path = start_video_capture(test_case)
            started_at = time.monotonic()
            try:
                passed, detail, output = run_test(command)
            except OSError as error:
                passed = False
                detail = f"could not start emulator: {error}"
                output = ""
            finally:
                stop_video_capture(video_process)
            duration = time.monotonic() - started_at
            if passed and video_path:
                video_path.unlink(missing_ok=True)
            results.append(
                TestResult(test_case, passed, detail, duration, output)
            )
    except KeyboardInterrupt:
        print("\nTest run interrupted.", file=sys.stderr)
        return 130

    print_summary(results)
    if args.junit_xml:
        write_junit_xml(results, args.junit_xml)
        print(f"JUnit XML written to {args.junit_xml}")
    write_github_summary(results)
    print_github_annotations(results)
    return 0 if all(result.passed for result in results) else 1


if __name__ == "__main__":
    sys.exit(main())
