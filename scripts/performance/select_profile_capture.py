#!/usr/bin/env python3
"""Print the prefix of the newest completed numbered profiler capture."""

import argparse
import re
from pathlib import Path


GAME_FRAMES_SUFFIX = "-game-frames.csv"


def select_profile_capture(output_prefix: Path) -> Path:
    pattern = re.compile(
        rf"^{re.escape(output_prefix.name)}-(\d{{3}}){re.escape(GAME_FRAMES_SUFFIX)}$"
    )
    candidates = []
    for path in output_prefix.parent.glob(
        f"{output_prefix.name}-???{GAME_FRAMES_SUFFIX}"
    ):
        match = pattern.fullmatch(path.name)
        if match and path.is_file() and path.stat().st_size:
            candidates.append((path.stat().st_mtime_ns, int(match.group(1)), path))

    if not candidates:
        raise FileNotFoundError(
            f"no completed profiler capture found for {output_prefix}"
        )

    path = max(candidates)[2]
    return path.with_name(path.name[: -len(GAME_FRAMES_SUFFIX)])


def parse_args() -> argparse.Namespace:
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("output_prefix", type=Path)
    return parser.parse_args()


def main() -> None:
    args = parse_args()
    try:
        print(select_profile_capture(args.output_prefix))
    except FileNotFoundError as error:
        raise SystemExit(str(error)) from error


if __name__ == "__main__":
    main()
