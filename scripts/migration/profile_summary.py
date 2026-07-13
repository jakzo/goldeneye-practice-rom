#!/usr/bin/env python3
"""Summarize a deterministic profiler capture."""

import argparse
import csv
import json
import re
import subprocess
from pathlib import Path
from statistics import fmean


SECTION_RE = re.compile(
    r"^\s*\[\s*\d+\]\s+(\S+)\s+\S+\s+[0-9a-fA-F]+\s+"
    r"[0-9a-fA-F]+\s+([0-9a-fA-F]+)\s+"
)
CODE_SECTIONS = (".code", ".inflate", ".game")


def parse_args():
    parser = argparse.ArgumentParser()
    parser.add_argument("--elf", required=True, type=Path)
    parser.add_argument("--csv", required=True, type=Path)
    parser.add_argument("--phase")
    parser.add_argument("--build-mode", choices=("release", "dev"))
    parser.add_argument("--region", choices=("US", "EU", "JP"))
    parser.add_argument("--baseline", type=Path)
    parser.add_argument("--base-average-frame-cycles", type=float)
    parser.add_argument("--max-base-regression-percent", type=float)
    parser.add_argument("--tool-prefix", default="mips-linux-gnu-")
    parser.add_argument("--output", type=Path)
    parser.add_argument(
        "--summary",
        type=Path,
        help="append a Markdown comparison table to this file",
    )
    args = parser.parse_args()
    if not args.phase and not args.region:
        parser.error("one of --phase or --region is required")
    if args.baseline and (not args.region or not args.build_mode):
        parser.error("--baseline requires --region and --build-mode")
    if (args.region or args.build_mode) and not args.baseline:
        parser.error("--region and --build-mode require --baseline")
    if args.base_average_frame_cycles is None and args.baseline is None:
        parser.error("--base-average-frame-cycles or --baseline is required")
    return args


def code_size(readelf, elf):
    output = subprocess.run(
        [readelf, "-S", "-W", str(elf)],
        check=True,
        text=True,
        capture_output=True,
    ).stdout
    sizes = {}
    for line in output.splitlines():
        match = SECTION_RE.match(line)
        if match and match.group(1) in CODE_SECTIONS:
            sizes[match.group(1)] = int(match.group(2), 16)
    missing = set(CODE_SECTIONS) - set(sizes)
    if missing:
        raise SystemExit("ELF is missing code sections: " + ", ".join(sorted(missing)))
    return sum(sizes.values()), sizes


def load_comparison(args):
    if args.baseline is None:
        return args.base_average_frame_cycles, None

    data = json.loads(args.baseline.read_text(encoding="utf-8"))
    if data.get("schema_version") != 1:
        raise SystemExit(f"unsupported baseline schema in {args.baseline}")
    try:
        base = data["base_decomp_replay"]
        recorded = data["builds"][args.build_mode][args.region]
        base_average = float(base["average_frame_cycles"])
        float(recorded["average_frame_cycles"])
    except (KeyError, TypeError, ValueError) as error:
        raise SystemExit(
            f"baseline is missing valid base/{args.build_mode}/{args.region} "
            f"frame-cycle stats: {error}"
        ) from error
    return base_average, recorded


def percent_difference(value, reference):
    return (value - reference) / reference * 100


def markdown_summary(result, recorded):
    region = result["region"]
    build_mode = result["build_mode"]
    lines = [
        f"## Performance — {region}",
        "",
        (
            "| Build | Total code size (bytes) | Average TLB loads/frame | "
            "Average frame cycles | Difference from base decomp | "
            "Difference from recorded release | Frames |"
        ),
        "| --- | ---: | ---: | ---: | ---: | ---: | ---: |",
        (
            f"| Recorded {build_mode} {region} | {recorded['total_code_size_bytes']:,} | "
            f"{recorded['average_tlb_loads']:.2f} | "
            f"{recorded['average_frame_cycles']:,.0f} | "
            f"{recorded['frame_cycles_difference_from_base_percent']:+.2f}% | "
            "0.00% | "
            f"{recorded['frames']} |"
        ),
        (
            f"| {result['phase']} | {result['total_code_size_bytes']:,} | "
            f"{result['average_tlb_loads']:.2f} | "
            f"{result['average_frame_cycles']:,.0f} | "
            f"{result['frame_cycles_difference_from_base_percent']:+.2f}% | "
            f"{result['frame_cycles_difference_from_recorded_percent']:+.2f}% | "
            f"{result['frames']} |"
        ),
        "",
    ]
    return "\n".join(lines)


def main():
    args = parse_args()
    base_average_frame_cycles, recorded = load_comparison(args)
    if base_average_frame_cycles <= 0:
        raise SystemExit("base average frame cycles must be greater than zero")
    if args.max_base_regression_percent is not None \
            and args.max_base_regression_percent < 0:
        raise SystemExit("--max-base-regression-percent cannot be negative")

    with args.csv.open(newline="", encoding="utf-8") as source:
        rows = list(csv.DictReader(source))
    if not rows:
        raise SystemExit("profile CSV has no frame rows")

    total_code_size, section_sizes = code_size(
        args.tool_prefix + "readelf", args.elf
    )
    average_frame_cycles = fmean(float(row["tick_cycles"]) for row in rows)
    difference_from_base = percent_difference(
        average_frame_cycles, base_average_frame_cycles
    )
    result = {
        "schema_version": 1,
        "phase": args.phase or f"Current {args.build_mode} {args.region}",
        "build_mode": args.build_mode,
        "region": args.region,
        "elf": str(args.elf),
        "profile_csv": str(args.csv),
        "frames": len(rows),
        "total_code_size_bytes": total_code_size,
        "code_sections": section_sizes,
        "average_tlb_loads": fmean(float(row["tlb_loads"]) for row in rows),
        "average_frame_cycles": average_frame_cycles,
        "base_average_frame_cycles": base_average_frame_cycles,
        "frame_cycles_difference_from_base_percent": difference_from_base,
    }
    if recorded is not None:
        result["frame_cycles_difference_from_recorded_percent"] = percent_difference(
            average_frame_cycles, float(recorded["average_frame_cycles"])
        )

    if args.output:
        args.output.parent.mkdir(parents=True, exist_ok=True)
        args.output.write_text(json.dumps(result, indent=2, sort_keys=True) + "\n")

    print(
        f"| {result['phase']} | {result['total_code_size_bytes']:,} | "
        f"{result['average_tlb_loads']:.2f} | "
        f"{result['average_frame_cycles']:,.0f} | "
        f"{difference_from_base:+.2f}% | {result['frames']} |"
    )
    if args.summary and recorded is not None:
        args.summary.parent.mkdir(parents=True, exist_ok=True)
        with args.summary.open("a", encoding="utf-8") as summary:
            summary.write(markdown_summary(result, recorded))

    if (
        args.max_base_regression_percent is not None
        and abs(difference_from_base) > args.max_base_regression_percent
    ):
        message = (
            f"release {args.region or ''} averages {difference_from_base:.2f}% more "
            f"frame cycles than base decomp (limit: "
            f"{args.max_base_regression_percent:.2f}%)"
        )
        print(f"error: {message}")
        return 1
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
