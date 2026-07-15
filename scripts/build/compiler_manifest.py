#!/usr/bin/env python3
"""Write and validate the C translation-unit build manifest."""

import argparse
import json
from pathlib import Path


def parse_args():
    parser = argparse.ArgumentParser()
    parser.add_argument("--output", required=True, type=Path)
    parser.add_argument("--build-dir", required=True)
    parser.add_argument("--gcc-version", required=True)
    parser.add_argument("--gcc-practice-flags", required=True)
    parser.add_argument("--gcc-original-flags", required=True)
    parser.add_argument("--original", nargs="*", default=[])
    parser.add_argument("--practice", nargs="*", default=[])
    parser.add_argument("--generated-assets", nargs="*", default=[])
    parser.add_argument("--region-code", required=True, choices=("u", "e", "j"))
    return parser.parse_args()


def object_path(build_dir, source):
    path = Path(source)
    if path.parent.name in ("u", "e", "j"):
        path = path.parent.parent / path.name
    return str(Path(build_dir) / path.with_suffix(".o"))


def main():
    args = parse_args()
    original = set(args.original)
    practice = set(args.practice)
    generated_assets = set(args.generated_assets)

    overlap = original & practice
    if overlap:
        raise SystemExit(
            "sources classified as both original and practice: "
            + ", ".join(sorted(overlap))
        )

    selected_assets = {}
    for source in sorted(generated_assets):
        path = Path(source)
        if path.parent.name in ("u", "e", "j") and path.parent.name != args.region_code:
            continue
        if path.parent.name in ("u", "e", "j"):
            common_path = path.parent.parent / path.name
            binary_candidates = (path.with_suffix(".bin"), common_path.with_suffix(".bin"))
        else:
            region_path = path.parent / args.region_code / path.name
            binary_candidates = (region_path.with_suffix(".bin"), path.with_suffix(".bin"))
        if any(candidate.is_file() for candidate in binary_candidates):
            continue
        output = object_path(args.build_dir, source)
        existing = selected_assets.get(output)
        if existing is None or path.parent.name == args.region_code:
            selected_assets[output] = source
    units = []
    for source in sorted(original | practice | set(selected_assets.values())):
        if not Path(source).is_file():
            raise SystemExit(f"classified C source does not exist: {source}")
        units.append(
            {
                "source": source,
                "object": object_path(args.build_dir, source),
                "compiler": "gcc",
                "kind": (
                    "practice"
                    if source in practice
                    else "generated_asset"
                    if source in selected_assets.values()
                    else "original"
                ),
            }
        )

    expected = original | practice | set(selected_assets.values())
    classified = {unit["source"] for unit in units}
    if classified != expected:
        missing = sorted(expected - classified)
        raise SystemExit("unclassified linked C sources: " + ", ".join(missing))
    objects = [unit["object"] for unit in units]
    if len(objects) != len(set(objects)):
        raise SystemExit("multiple C sources map to the same linked object")

    kind_counts = {
        kind: sum(unit["kind"] == kind for unit in units)
        for kind in ("original", "practice", "generated_asset")
    }
    document = {
        "schema_version": 2,
        "build_dir": args.build_dir,
        "compiler": {"name": "gcc", "version": args.gcc_version},
        "flags": {
            "gcc_practice": args.gcc_practice_flags,
            "gcc_original": args.gcc_original_flags,
        },
        "counts": {
            "translation_units": len(units),
            "by_compiler": {"gcc": len(units)},
            "by_kind": kind_counts,
        },
        "translation_units": units,
    }

    rendered = json.dumps(document, indent=2, sort_keys=True) + "\n"
    args.output.parent.mkdir(parents=True, exist_ok=True)
    temporary = args.output.with_suffix(args.output.suffix + ".tmp")
    temporary.write_text(rendered, encoding="utf-8")
    if args.output.exists() and args.output.read_text(encoding="utf-8") == rendered:
        temporary.unlink()
    else:
        temporary.replace(args.output)


if __name__ == "__main__":
    main()
