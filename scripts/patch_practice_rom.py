#!/usr/bin/env python3

import argparse
import re
import struct
from pathlib import Path


ROOT = Path(__file__).resolve().parent.parent
LEVELS_HEADER = ROOT / "src/bondconstants.h"
TESTS_FILE = ROOT / "src/practice/practice_tests.c"

ROM_CONFIG_OFFSET = 0x00FFFFC0
ROM_CONFIG_SIZE = 64
ROM_CONFIG_MAGIC = 0x47505243  # "GPRC"
ROM_CONFIG_VERSION = 1
NO_TEST = 0
NO_BOOT_LEVEL = -1
CHECKSUM_SALT = 0xC0DEC0DE


def parse_test_cases():
    values = {}
    pattern = re.compile(r"^\s*#define\s+([A-Z][A-Z0-9_]*)\s+(-?\d+)\s*$")
    inside = False

    for line in TESTS_FILE.read_text().splitlines():
        if line.strip() == "// --- start test cases ---":
            inside = True
            continue
        if line.strip() == "// --- end test cases ---":
            break
        if inside:
            match = pattern.match(line)
            if match:
                values[match.group(1)] = int(match.group(2))
    return values


def parse_level_ids():
    text = re.sub(r"/\*.*?\*/|//[^\n]*", "", LEVELS_HEADER.read_text(), flags=re.S)
    match = re.search(r"typedef\s+enum\s+LEVELID\s*\{(.*?)\}\s*LEVELID\s*;", text, re.S)
    if not match:
        raise ValueError("could not find LEVELID enum")

    values = {}
    current = -1
    for entry in match.group(1).split("\n"):
        entry = entry.strip(",").strip()
        if not entry or not entry.startswith("LEVELID_"):
            continue
        name, separator, expression = entry.partition("=")
        name = name.strip()
        if separator:
            expression = expression.strip()
            if re.fullmatch(r"-?(?:0x[0-9a-fA-F]+|\d+)", expression):
                current = int(expression, 0)
            elif expression in values:
                current = values[expression]
            else:
                # Expressions used only by multiplayer aliases are not needed
                # for normal boot-level selection.
                continue
        else:
            current += 1
        values[name] = current
    return values


def parse_selection(value, names, prefix, default):
    if not value:
        return default
    try:
        return int(value, 0)
    except ValueError:
        name = value.upper()
        if prefix and not name.startswith(prefix):
            name = prefix + name
        if name not in names:
            choices = ", ".join(sorted(name.removeprefix(prefix) for name in names))
            raise ValueError(f"unknown value {value!r}; expected one of: {choices}")
        return names[name]


def encode_config(test_case, boot_level):
    size = ROM_CONFIG_SIZE
    flags = 0
    checksum = (
        ROM_CONFIG_MAGIC
        ^ ROM_CONFIG_VERSION
        ^ size
        ^ (test_case & 0xFFFFFFFF)
        ^ (boot_level & 0xFFFFFFFF)
        ^ flags
        ^ CHECKSUM_SALT
    )
    return struct.pack(
        ">IIIIiiI36x",
        ROM_CONFIG_MAGIC,
        ROM_CONFIG_VERSION,
        size,
        checksum,
        test_case,
        boot_level,
        flags,
    )


def patch_rom(path, config):
    with path.open("r+b") as rom:
        rom.seek(0, 2)
        rom_size = rom.tell()
        if rom_size < ROM_CONFIG_OFFSET + ROM_CONFIG_SIZE:
            raise ValueError(
                f"{path} is too small for runtime config slot "
                f"0x{ROM_CONFIG_OFFSET:08x}"
            )

        rom.seek(ROM_CONFIG_OFFSET)
        previous = rom.read(ROM_CONFIG_SIZE)
        if previous == config:
            return False
        if previous != b"\xff" * ROM_CONFIG_SIZE and not previous.startswith(
            struct.pack(">I", ROM_CONFIG_MAGIC)
        ):
            raise ValueError(
                "runtime config slot is no longer unused; choose a new reserved "
                "ROM location instead of overwriting data"
            )

        rom.seek(ROM_CONFIG_OFFSET)
        rom.write(config)
        return True


def main():
    parser = argparse.ArgumentParser()
    parser.add_argument("rom", type=Path)
    parser.add_argument("--test-case", default="")
    parser.add_argument("--boot-level", default="")
    args = parser.parse_args()

    try:
        test_case = parse_selection(
            args.test_case, parse_test_cases(), "", NO_TEST
        )
        boot_level = parse_selection(
            args.boot_level, parse_level_ids(), "LEVELID_", NO_BOOT_LEVEL
        )
        changed = patch_rom(args.rom, encode_config(test_case, boot_level))
    except (OSError, ValueError) as error:
        parser.error(str(error))

    status = "patched" if changed else "unchanged"
    print(
        f"Runtime config {status}: test_case={test_case}, "
        f"boot_level={boot_level}"
    )


if __name__ == "__main__":
    main()
