#!/usr/bin/env python3
"""Write and validate the C translation-unit compiler manifest."""

import argparse
import json
from pathlib import Path


def parse_args():
    parser = argparse.ArgumentParser()
    parser.add_argument("--output", required=True, type=Path)
    parser.add_argument("--mode", required=True, choices=("ido", "gcc", "mixed"))
    parser.add_argument("--build-dir", required=True)
    parser.add_argument("--gcc-version", required=True)
    parser.add_argument("--gcc-practice-flags", required=True)
    parser.add_argument("--gcc-original-flags", required=True)
    parser.add_argument("--ido-flags", required=True)
    parser.add_argument("--original", nargs="*", default=[])
    parser.add_argument("--practice", nargs="*", default=[])
    parser.add_argument("--selected-gcc", nargs="*", default=[])
    return parser.parse_args()


def object_path(build_dir, source):
    return str(Path(build_dir) / Path(source).with_suffix(".o"))


def main():
    args = parse_args()
    original = set(args.original)
    practice = set(args.practice)
    selected_gcc = set(args.selected_gcc)

    overlap = original & practice
    if overlap:
        raise SystemExit(
            "sources classified as both original and practice: "
            + ", ".join(sorted(overlap))
        )
    unknown = selected_gcc - original
    if unknown:
        raise SystemExit(
            "GCC migration manifest contains unclassified sources: "
            + ", ".join(sorted(unknown))
        )

    units = []
    for source in sorted(original | practice):
        if not Path(source).is_file():
            raise SystemExit(f"classified C source does not exist: {source}")
        if source in practice or source in selected_gcc:
            compiler = "gcc"
        else:
            compiler = "ido"
        units.append(
            {
                "source": source,
                "object": object_path(args.build_dir, source),
                "compiler": compiler,
                "kind": "practice" if source in practice else "original",
            }
        )

    expected = original | practice
    classified = {unit["source"] for unit in units}
    if classified != expected:
        missing = sorted(expected - classified)
        raise SystemExit("unclassified linked C sources: " + ", ".join(missing))
    objects = [unit["object"] for unit in units]
    if len(objects) != len(set(objects)):
        raise SystemExit("multiple C sources map to the same linked object")

    document = {
        "schema_version": 1,
        "original_cc_mode": args.mode,
        "build_dir": args.build_dir,
        "compiler_versions": {"gcc": args.gcc_version, "ido": "IDO 5.3"},
        "flags": {
            "gcc_practice": args.gcc_practice_flags,
            "gcc_original": args.gcc_original_flags,
            "ido_original": args.ido_flags,
        },
        "counts": {
            "translation_units": len(units),
            "gcc": sum(unit["compiler"] == "gcc" for unit in units),
            "ido": sum(unit["compiler"] == "ido" for unit in units),
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
