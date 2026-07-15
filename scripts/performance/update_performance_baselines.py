#!/usr/bin/env python3
"""Merge current regional profiler results into the checked-in baseline."""

import argparse
import json
from pathlib import Path


STORED_FIELDS = (
    "total_code_size_bytes",
    "average_tlb_loads",
    "average_frame_cycles",
    "frame_cycles_difference_from_base_percent",
    "frames",
)
REGIONS = ("US", "EU", "JP")
BUILD_MODES = ("release", "dev")


def parse_args():
    parser = argparse.ArgumentParser()
    parser.add_argument("--baseline", required=True, type=Path)
    parser.add_argument("stats", nargs="+", type=Path)
    return parser.parse_args()


def main():
    args = parse_args()
    baseline = json.loads(args.baseline.read_text(encoding="utf-8"))
    if baseline.get("schema_version") != 1:
        raise SystemExit(f"unsupported baseline schema in {args.baseline}")

    results = {mode: {} for mode in BUILD_MODES}
    for path in args.stats:
        result = json.loads(path.read_text(encoding="utf-8"))
        region = result.get("region")
        build_mode = result.get("build_mode")
        if region not in REGIONS:
            raise SystemExit(f"{path} has invalid region {region!r}")
        if build_mode not in BUILD_MODES:
            raise SystemExit(f"{path} has invalid build mode {build_mode!r}")
        if region in results[build_mode]:
            raise SystemExit(
                f"multiple profiler results provided for {build_mode} {region}"
            )
        try:
            results[build_mode][region] = {
                field: result[field] for field in STORED_FIELDS
            }
        except KeyError as error:
            raise SystemExit(f"{path} is missing {error.args[0]}") from error

    missing = [
        f"{mode} {region}"
        for mode in BUILD_MODES
        for region in REGIONS
        if region not in results[mode]
    ]
    if missing:
        raise SystemExit("missing profiler results for: " + ", ".join(missing))

    baseline["builds"] = {
        mode: {region: results[mode][region] for region in REGIONS}
        for mode in BUILD_MODES
    }
    args.baseline.write_text(
        json.dumps(baseline, indent=2, sort_keys=True) + "\n",
        encoding="utf-8",
    )


if __name__ == "__main__":
    main()
