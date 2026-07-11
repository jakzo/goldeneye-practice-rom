#!/usr/bin/env python3
"""Compare migration baseline/candidate ELF and map artifacts."""

import argparse
import hashlib
import json
import re
import subprocess
from collections import Counter
from pathlib import Path


SECTION_RE = re.compile(
    r"^\s*\[\s*\d+\]\s+(\S+)\s+\S+\s+([0-9a-fA-F]+)\s+"
    r"[0-9a-fA-F]+\s+([0-9a-fA-F]+)\s+"
)


def parse_args():
    parser = argparse.ArgumentParser()
    parser.add_argument("--baseline-elf", required=True, type=Path)
    parser.add_argument("--baseline-map", required=True, type=Path)
    parser.add_argument("--candidate-elf", required=True, type=Path)
    parser.add_argument("--candidate-map", required=True, type=Path)
    parser.add_argument("--tool-prefix", default="mips-linux-gnu-")
    parser.add_argument("--json", required=True, type=Path)
    parser.add_argument("--markdown", required=True, type=Path)
    parser.add_argument("--limit", type=int, default=25)
    return parser.parse_args()


def run(tool, *args):
    return subprocess.run(
        [tool, *map(str, args)], check=True, text=True, capture_output=True
    ).stdout


def sections(readelf, elf):
    result = {}
    for line in run(readelf, "-S", "-W", elf).splitlines():
        match = SECTION_RE.match(line)
        if match:
            name, address, size = match.groups()
            result[name] = {"address": int(address, 16), "size": int(size, 16)}
    return result


def symbols(nm, elf):
    result = {}
    names = []
    output = run(nm, "-g", "-S", "-n", "--defined-only", elf)
    for line in output.splitlines():
        fields = line.split(None, 3)
        if len(fields) != 4:
            continue
        address, size, symbol_type, name = fields
        try:
            entry = {
                "address": int(address, 16),
                "size": int(size, 16),
                "type": symbol_type,
            }
        except ValueError:
            continue
        names.append(name)
        result.setdefault(name, entry)
    duplicates = sorted(name for name, count in Counter(names).items() if count > 1)
    undefined = sorted(
        line.split()[-1]
        for line in run(nm, "-u", elf).splitlines()
        if line.split()
    )
    return result, duplicates, undefined


def artifact(path):
    data = path.read_bytes()
    return {
        "path": str(path),
        "size": len(data),
        "sha256": hashlib.sha256(data).hexdigest(),
    }


def signed(value):
    return f"{value:+d}"


def main():
    args = parse_args()
    for path in (
        args.baseline_elf,
        args.baseline_map,
        args.candidate_elf,
        args.candidate_map,
    ):
        if not path.is_file():
            raise SystemExit(f"missing comparison input: {path}")

    nm = args.tool_prefix + "nm"
    readelf = args.tool_prefix + "readelf"
    baseline_sections = sections(readelf, args.baseline_elf)
    candidate_sections = sections(readelf, args.candidate_elf)
    baseline_symbols, baseline_duplicates, baseline_undefined = symbols(
        nm, args.baseline_elf
    )
    candidate_symbols, candidate_duplicates, candidate_undefined = symbols(
        nm, args.candidate_elf
    )

    section_names = sorted(set(baseline_sections) | set(candidate_sections))
    section_changes = []
    for name in section_names:
        before = baseline_sections.get(name, {"address": 0, "size": 0})
        after = candidate_sections.get(name, {"address": 0, "size": 0})
        section_changes.append(
            {
                "name": name,
                "baseline_size": before["size"],
                "candidate_size": after["size"],
                "size_delta": after["size"] - before["size"],
                "baseline_address": before["address"],
                "candidate_address": after["address"],
                "address_delta": after["address"] - before["address"],
            }
        )

    common_symbols = set(baseline_symbols) & set(candidate_symbols)
    symbol_changes = []
    for name in common_symbols:
        before = baseline_symbols[name]
        after = candidate_symbols[name]
        size_delta = after["size"] - before["size"]
        address_delta = after["address"] - before["address"]
        if size_delta or address_delta:
            symbol_changes.append(
                {
                    "name": name,
                    "type": after["type"],
                    "baseline_size": before["size"],
                    "candidate_size": after["size"],
                    "size_delta": size_delta,
                    "baseline_address": before["address"],
                    "candidate_address": after["address"],
                    "address_delta": address_delta,
                }
            )

    largest_size = sorted(
        symbol_changes, key=lambda item: abs(item["size_delta"]), reverse=True
    )[: args.limit]
    largest_address = sorted(
        symbol_changes, key=lambda item: abs(item["address_delta"]), reverse=True
    )[: args.limit]
    added = sorted(set(candidate_symbols) - set(baseline_symbols))
    removed = sorted(set(baseline_symbols) - set(candidate_symbols))

    report = {
        "schema_version": 1,
        "artifacts": {
            "baseline_elf": artifact(args.baseline_elf),
            "baseline_map": artifact(args.baseline_map),
            "candidate_elf": artifact(args.candidate_elf),
            "candidate_map": artifact(args.candidate_map),
        },
        "sections": section_changes,
        "symbols": {
            "baseline_count": len(baseline_symbols),
            "candidate_count": len(candidate_symbols),
            "added": added,
            "removed": removed,
            "largest_size_changes": largest_size,
            "largest_address_changes": largest_address,
            "baseline_undefined": baseline_undefined,
            "candidate_undefined": candidate_undefined,
            "baseline_duplicates": baseline_duplicates,
            "candidate_duplicates": candidate_duplicates,
        },
    }

    args.json.parent.mkdir(parents=True, exist_ok=True)
    args.json.write_text(json.dumps(report, indent=2, sort_keys=True) + "\n")

    lines = [
        "# IDO vs mixed migration report",
        "",
        "## Sections",
        "",
        "| Section | Baseline | Candidate | Size delta | Address delta |",
        "| --- | ---: | ---: | ---: | ---: |",
    ]
    for change in section_changes:
        lines.append(
            f"| `{change['name']}` | {change['baseline_size']} | "
            f"{change['candidate_size']} | {signed(change['size_delta'])} | "
            f"{signed(change['address_delta'])} |"
        )

    lines.extend(
        [
            "",
            "## Symbol summary",
            "",
            f"- Added: {len(added)}",
            f"- Removed: {len(removed)}",
            f"- Undefined (baseline/candidate): "
            f"{len(baseline_undefined)}/{len(candidate_undefined)}",
            f"- Duplicate definitions (baseline/candidate): "
            f"{len(baseline_duplicates)}/{len(candidate_duplicates)}",
            "",
            "## Largest symbol size changes",
            "",
            "| Symbol | Baseline | Candidate | Delta |",
            "| --- | ---: | ---: | ---: |",
        ]
    )
    for change in largest_size:
        lines.append(
            f"| `{change['name']}` | {change['baseline_size']} | "
            f"{change['candidate_size']} | {signed(change['size_delta'])} |"
        )
    args.markdown.parent.mkdir(parents=True, exist_ok=True)
    args.markdown.write_text("\n".join(lines) + "\n", encoding="utf-8")


if __name__ == "__main__":
    main()
