#!/usr/bin/env python3
"""Discover, balance, and run one serial shard of the full test suite."""

import argparse
import os
import re
import struct
import subprocess
import sys
import time
import xml.etree.ElementTree as ET
from dataclasses import dataclass
from pathlib import Path
from typing import Optional


ROOT = Path(__file__).resolve().parents[1]
TESTS_FILE = ROOT / "src/practice/practice_tests.c"
REPLAY_ROOT = ROOT / "tests/replays"
PRACTICE_RUNNER = ROOT / "scripts/run_practice_tests.py"
SAVE_STATE_RUNNER = ROOT / "scripts/run_replay_save_state_tests.sh"
PATCH_ROM_SCRIPT = ROOT / "scripts/patch_practice_rom.py"
HOST_REPLAY_RUNNER = ROOT / "ares/tests/n64-replay/run.py"

REGIONS = {
    "us": ("US", "u"),
    "eu": ("EU", "e"),
    "jp": ("JP", "j"),
}
REPLAY_HEADER = struct.Struct(">IHHII4BHHQQII")
REPLAY_MAGIC = 0x47455250
REPLAY_OFFSETS = (0x280, 0x600)
SHORT_TEST_ESTIMATE_FRAMES = 600
SAVE_STATE_ESTIMATE_MULTIPLIERS = {
    "default": 2,
    "long": 6,
    "cameras": 2,
    "near-end": 2,
    "cold-restart": 6,
}
MULTIPLEXED_SAVE_STATE_VARIANTS = ("default", "cameras")
MANUAL_PRACTICE_TESTS = {"REPLAY_RUNWAY_SAVE_STATES"}
PRACTICE_REPLAY_FIXTURES = {
    "US": {
        "REPLAY_DAM",
        "REPLAY_RUNWAY",
        "REPLAY_FRIGATE",
        "REPLAY_GRENADE_CAM",
        "REPLAY_ARCHIVES",
        "REPLAY_ARCHIVES_04X",
        "REPLAY_ARCHIVES_HOTKEYS",
    },
    "EU": {
        "REPLAY_ARCHIVES",
        "REPLAY_ARCHIVES_04X",
        "REPLAY_ARCHIVES_HOTKEYS",
    },
    "JP": {"REPLAY_DAM", "REPLAY_RUNWAY"},
}


@dataclass(frozen=True)
class TestTask:
    task_id: str
    kind: str
    region: str
    estimate_frames: int
    test_case: Optional[str] = None
    replay: Optional[Path] = None
    variant: Optional[str] = None


@dataclass
class TaskResult:
    task: TestTask
    passed: bool
    duration: float
    output: str


def parse_shard(value: str) -> tuple[int, int]:
    match = re.fullmatch(r"([1-9][0-9]*)/([1-9][0-9]*)", value)
    if match is None:
        raise ValueError("shard must have the form INDEX/COUNT, for example 5/12")
    index, count = (int(part) for part in match.groups())
    if index > count:
        raise ValueError("shard index must not exceed shard count")
    return index - 1, count


def read_practice_test_cases() -> list[str]:
    start_marker = "// --- start test cases ---"
    end_marker = "// --- end test cases ---"
    define_pattern = re.compile(r"^\s*#define\s+([A-Za-z_][A-Za-z0-9_]*)\b")
    cases = []
    inside = False

    for line in TESTS_FILE.read_text(encoding="utf-8").splitlines():
        if line.strip() == start_marker:
            inside = True
            continue
        if line.strip() == end_marker:
            break
        if inside:
            match = define_pattern.match(line)
            if match:
                cases.append(match.group(1))
    if not cases:
        raise ValueError(f"no test cases found in {TESTS_FILE}")
    return cases


def replay_duration_frames(path: Path) -> int:
    data = path.read_bytes()
    replay_offset = next(
        (
            offset
            for offset in REPLAY_OFFSETS
            if len(data) >= offset + REPLAY_HEADER.size
            and struct.unpack_from(">I", data, offset)[0] == REPLAY_MAGIC
        ),
        None,
    )
    if replay_offset is None:
        raise ValueError(f"{path}: replay header is missing")
    header = REPLAY_HEADER.unpack_from(data, replay_offset)
    version, header_size, duration = header[1], header[2], header[13]
    if version != 1 or header_size != REPLAY_HEADER.size or duration < 1:
        raise ValueError(f"{path}: unsupported replay header")
    return duration


def practice_test_available(test_case: str, version: str) -> bool:
    if not test_case.startswith("REPLAY_"):
        return True
    return test_case in PRACTICE_REPLAY_FIXTURES[version]


def discover_tasks() -> list[TestTask]:
    tasks = []
    practice_cases = [
        case
        for case in read_practice_test_cases()
        if case not in MANUAL_PRACTICE_TESTS
    ]

    for region, (version, _) in REGIONS.items():
        for test_case in practice_cases:
            if practice_test_available(test_case, version):
                tasks.append(
                    TestTask(
                        f"practice/{region}/{test_case}",
                        "practice",
                        region,
                        SHORT_TEST_ESTIMATE_FRAMES,
                        test_case=test_case,
                    )
                )

        replays = sorted((REPLAY_ROOT / region).glob("*.ram"))
        if not replays:
            raise ValueError(f"no replay fixtures found for {region}")
        total_duration = 0
        for replay in replays:
            duration = replay_duration_frames(replay)
            total_duration += duration
            tasks.append(
                TestTask(
                    f"replay/{region}/{replay.stem}",
                    "replay",
                    region,
                    duration,
                    replay=replay,
                )
            )
            for variant in ("long", "cold-restart"):
                tasks.append(
                    TestTask(
                        f"save-state/{region}/{variant}/{replay.stem}",
                        "save-state",
                        region,
                        duration * SAVE_STATE_ESTIMATE_MULTIPLIERS[variant],
                        replay=replay,
                        variant=variant,
                    )
                )
        for variant in MULTIPLEXED_SAVE_STATE_VARIANTS:
            tasks.append(
                TestTask(
                    f"save-state/{region}/{variant}",
                    "save-state",
                    region,
                    total_duration * SAVE_STATE_ESTIMATE_MULTIPLIERS[variant],
                    variant=variant,
                )
            )

        near_end_replay = next(
            (replay for replay in replays if replay.stem == "02-facility"),
            replays[0],
        )
        tasks.append(
            TestTask(
                f"save-state/{region}/near-end/{near_end_replay.stem}",
                "save-state",
                region,
                replay_duration_frames(near_end_replay)
                * SAVE_STATE_ESTIMATE_MULTIPLIERS["near-end"],
                replay=near_end_replay,
                variant="near-end",
            )
        )

    return sorted(tasks, key=lambda task: task.task_id)


def balance_tasks(tasks: list[TestTask], shard_count: int) -> list[list[TestTask]]:
    if shard_count < 1:
        raise ValueError("shard count must be positive")
    shards = [[] for _ in range(shard_count)]
    loads = [0] * shard_count
    for task in sorted(tasks, key=lambda item: (-item.estimate_frames, item.task_id)):
        shard_index = min(range(shard_count), key=lambda index: (loads[index], index))
        shards[shard_index].append(task)
        loads[shard_index] += task.estimate_frames
    for shard in shards:
        shard.sort(key=lambda task: task.task_id)
    return shards


def estimate_label(task: TestTask) -> str:
    if task.kind == "practice":
        return "short"
    return f"{task.estimate_frames} frames"


def task_command(task: TestTask, artifacts: Path) -> tuple[list[str], dict[str, str]]:
    version, code = REGIONS[task.region]
    environment = os.environ.copy()
    environment["GITHUB_ACTIONS"] = ""
    environment.pop("GITHUB_STEP_SUMMARY", None)
    environment["PRACTICE_TEST_VIDEO_DIR"] = str(
        artifacts / "videos" / task.task_id.replace("/", "__")
    )

    if task.kind == "practice":
        return (
            [
                sys.executable,
                str(PRACTICE_RUNNER),
                "--skip-build",
                "--build-mode",
                "release",
                "--version",
                version,
                "--test",
                task.test_case,
                "--timeout",
                "180",
                "--jobs",
                "1",
                "--color",
                "never",
            ],
            environment,
        )

    if task.kind == "replay":
        assert task.replay is not None
        rom = ROOT / f"build/{code}/ge007.{code}.z64"
        patch = subprocess.run(
            [sys.executable, str(PATCH_ROM_SCRIPT), str(rom), "--flags", "1"],
            cwd=ROOT,
        )
        if patch.returncode != 0:
            raise RuntimeError(f"could not patch {version} replay ROM")
        return (
            [
                sys.executable,
                str(HOST_REPLAY_RUNNER),
                "--ares",
                os.environ.get("ARES", "ares"),
                "--rom",
                str(rom),
                "--elf",
                str(ROOT / f"build/{code}/ge007.{code}.elf"),
                "--fixture-dir",
                str(REPLAY_ROOT),
                "--region",
                task.region,
                "--replay",
                task.replay.name,
                "--jobs",
                "1",
                "--artifacts",
                str(artifacts / "replays" / task.region),
            ],
            environment,
        )

    environment.update(
        {
            "REPLAY_SAVE_STATE_BUILD_MODE": "release",
            "REPLAY_SAVE_STATE_JOBS": "1",
            "REPLAY_SAVE_STATE_SKIP_BUILD": "1",
        }
    )
    if task.replay is not None:
        environment["REPLAY_SAVE_STATE_FILTER"] = task.replay.stem
    if task.variant == "long":
        environment.update(
            {
                "REPLAY_SAVE_STATE_SPACING_SECONDS": "10",
                "REPLAY_SAVE_STATE_WAIT_FRAMES": "4",
                "REPLAY_SAVE_STATE_DURATION_SECONDS": "30",
            }
        )
    elif task.variant == "cameras":
        environment.update(
            {
                "REPLAY_SAVE_STATE_GRENADE_CAM": "1",
                "REPLAY_SAVE_STATE_HOSTAGE_CAM": "1",
            }
        )
    elif task.variant == "near-end":
        environment["REPLAY_SAVE_STATE_END_MARGIN_FRAMES"] = "30"
    elif task.variant == "cold-restart":
        environment["REPLAY_SAVE_STATE_RESTART_BETWEEN_LOADS"] = "1"
    return (
        [str(SAVE_STATE_RUNNER), str(REPLAY_ROOT / task.region), version],
        environment,
    )


def run_task(task: TestTask, artifacts: Path) -> TaskResult:
    print(
        f"\n=== {task.task_id} (estimated {estimate_label(task)}) ===",
        flush=True,
    )
    started = time.monotonic()
    try:
        command, environment = task_command(task, artifacts)
        process = subprocess.Popen(
            command,
            cwd=ROOT,
            env=environment,
            stdout=subprocess.PIPE,
            stderr=subprocess.STDOUT,
            text=True,
            bufsize=1,
        )
        output = []
        assert process.stdout is not None
        try:
            for line in process.stdout:
                output.append(line)
                print(line, end="", flush=True)
            return_code = process.wait()
        except KeyboardInterrupt:
            process.terminate()
            process.wait()
            raise
    except (OSError, RuntimeError) as error:
        output = [f"runner error: {error}\n"]
        print(output[0], end="", flush=True)
        return_code = 1
    duration = time.monotonic() - started
    log_path = artifacts / "logs" / f"{task.task_id.replace('/', '__')}.log"
    log_path.parent.mkdir(parents=True, exist_ok=True)
    output_text = "".join(output)
    log_path.write_text(output_text, encoding="utf-8")
    return TaskResult(
        task,
        return_code == 0,
        duration,
        "" if return_code == 0 else output_text,
    )


def write_junit(results: list[TaskResult], output: Path) -> None:
    suite = ET.Element(
        "testsuite",
        {
            "name": "full-test-shard",
            "tests": str(len(results)),
            "failures": str(sum(not result.passed for result in results)),
            "errors": "0",
            "time": f"{sum(result.duration for result in results):.3f}",
        },
    )
    for result in results:
        case = ET.SubElement(
            suite,
            "testcase",
            {
                "classname": result.task.kind,
                "name": result.task.task_id,
                "time": f"{result.duration:.3f}",
            },
        )
        if not result.passed:
            failure = ET.SubElement(
                case,
                "failure",
                {"message": "test command failed", "type": "TestShardFailure"},
            )
            failure.text = result.output
    tree = ET.ElementTree(suite)
    ET.indent(tree, space="  ")
    output.parent.mkdir(parents=True, exist_ok=True)
    tree.write(output, encoding="utf-8", xml_declaration=True)


def write_github_summary(shard: str, results: list[TaskResult]) -> None:
    summary_path = os.environ.get("GITHUB_STEP_SUMMARY")
    if not summary_path:
        return
    passed = sum(result.passed for result in results)
    lines = [
        f"### Full test shard {shard}",
        "",
        f"**{passed} passed, {len(results) - passed} failed**",
        "",
        "| Test | Estimate | Result | Duration |",
        "| --- | ---: | --- | ---: |",
    ]
    for result in results:
        status = "✅ Passed" if result.passed else "❌ Failed"
        lines.append(
            f"| `{result.task.task_id}` | {estimate_label(result.task)} "
            f"| {status} | {result.duration:.2f}s |"
        )
    with open(summary_path, "a", encoding="utf-8") as summary:
        summary.write("\n".join(lines) + "\n")


def print_plan(shards: list[list[TestTask]]) -> None:
    for index, tasks in enumerate(shards, 1):
        estimate = sum(task.estimate_frames for task in tasks)
        print(f"shard {index}/{len(shards)}: {estimate} estimated frames, {len(tasks)} tests")
        for task in tasks:
            label = "short" if task.kind == "practice" else str(task.estimate_frames)
            print(f"  {label:>7}  {task.task_id}")


def parse_args() -> argparse.Namespace:
    parser = argparse.ArgumentParser(
        description="Run a deterministic, duration-balanced full-suite shard."
    )
    parser.add_argument(
        "--shard",
        default=os.environ.get("SHARD", "1/1"),
        help="one-based shard selector INDEX/COUNT (default: $SHARD or 1/1)",
    )
    parser.add_argument(
        "--list",
        action="store_true",
        help="show the deterministic assignment for every shard without running",
    )
    parser.add_argument(
        "--task",
        action="append",
        default=[],
        metavar="TASK_ID",
        help="run a specific task instead of its complete shard (repeatable)",
    )
    parser.add_argument(
        "--artifacts",
        type=Path,
        default=ROOT / "build/test-shards",
        help="logs and JUnit output directory",
    )
    return parser.parse_args()


def main() -> int:
    args = parse_args()
    try:
        shard_index, shard_count = parse_shard(args.shard)
        tasks = discover_tasks()
        shards = balance_tasks(tasks, shard_count)
    except (OSError, ValueError, struct.error) as error:
        print(f"error: {error}", file=sys.stderr)
        return 2

    if args.list:
        print_plan(shards)
        return 0

    selected = shards[shard_index]
    if args.task:
        tasks_by_id = {task.task_id: task for task in tasks}
        unknown = sorted(set(args.task) - tasks_by_id.keys())
        if unknown:
            print(f"error: unknown task(s): {', '.join(unknown)}", file=sys.stderr)
            return 2
        selected = [tasks_by_id[task_id] for task_id in dict.fromkeys(args.task)]
    estimate = sum(task.estimate_frames for task in selected)
    print(
        f"Running shard {args.shard}: {len(selected)} tests, "
        f"{estimate} estimated frames",
        flush=True,
    )
    results = [run_task(task, args.artifacts) for task in selected]
    output_dir = args.artifacts / args.shard.replace("/", "-of-")
    write_junit(results, output_dir / "results.xml")
    write_github_summary(args.shard, results)

    passed = sum(result.passed for result in results)
    print(f"\nShard summary: {passed} passed, {len(results) - passed} failed")
    if os.environ.get("GITHUB_ACTIONS"):
        for result in results:
            if not result.passed:
                print(f"::error title=Full test shard failed::{result.task.task_id}")
    return 0 if passed == len(results) else 1


if __name__ == "__main__":
    raise SystemExit(main())
