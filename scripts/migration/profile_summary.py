#!/usr/bin/env python3
"""Summarize the deterministic migration profiler capture."""

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
    parser.add_argument("--phase", required=True)
    parser.add_argument("--base-average-frame-cycles", required=True, type=float)
    parser.add_argument("--tool-prefix", default="mips-linux-gnu-")
    parser.add_argument("--output", type=Path)
    return parser.parse_args()


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


def main():
    args = parse_args()
    if args.base_average_frame_cycles <= 0:
        raise SystemExit("--base-average-frame-cycles must be greater than zero")
    with args.csv.open(newline="", encoding="utf-8") as source:
        rows = list(csv.DictReader(source))
    if not rows:
        raise SystemExit("profile CSV has no frame rows")

    total_code_size, section_sizes = code_size(
        args.tool_prefix + "readelf", args.elf
    )
    average_frame_cycles = fmean(float(row["tick_cycles"]) for row in rows)
    difference_from_base = (
        (average_frame_cycles - args.base_average_frame_cycles)
        / args.base_average_frame_cycles
        * 100
    )
    result = {
        "schema_version": 1,
        "phase": args.phase,
        "elf": str(args.elf),
        "profile_csv": str(args.csv),
        "frames": len(rows),
        "total_code_size_bytes": total_code_size,
        "code_sections": section_sizes,
        "average_tlb_loads": fmean(float(row["tlb_loads"]) for row in rows),
        "average_frame_cycles": average_frame_cycles,
        "base_average_frame_cycles": args.base_average_frame_cycles,
        "frame_cycles_difference_from_base_percent": difference_from_base,
    }

    if args.output:
        args.output.parent.mkdir(parents=True, exist_ok=True)
        args.output.write_text(json.dumps(result, indent=2, sort_keys=True) + "\n")

    print(
        f"| {args.phase} | {result['total_code_size_bytes']:,} | "
        f"{result['average_tlb_loads']:.2f} | "
        f"{result['average_frame_cycles']:,.0f} | "
        f"{difference_from_base:+.2f}% | {result['frames']} |"
    )


if __name__ == "__main__":
    main()
