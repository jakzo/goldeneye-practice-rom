#!/usr/bin/env python3

import argparse
import struct
from pathlib import Path


REPLAY_MAGIC = 0x47455250
REPLAY_HEADER = struct.Struct(">IHHII4BHHQQII")
REPLAY_SRAM_OFFSET = 0x600


def main():
    parser = argparse.ArgumentParser(
        description="Extract a practice replay stream from a 128 KiB SRAM file."
    )
    parser.add_argument("sram", type=Path)
    parser.add_argument("output", type=Path)
    parser.add_argument(
        "--test-sram",
        action="store_true",
        help="create a test SRAM containing saves, config, and the replay",
    )
    args = parser.parse_args()

    sram = args.sram.read_bytes()
    header_end = REPLAY_SRAM_OFFSET + REPLAY_HEADER.size
    if len(sram) < header_end:
        parser.error("SRAM file is too small to contain a replay header")

    header = REPLAY_HEADER.unpack_from(sram, REPLAY_SRAM_OFFSET)
    magic, version, header_size, total_size, frame_count = header[:5]
    if magic != REPLAY_MAGIC:
        parser.error("SRAM does not contain a practice replay")
    if version != 1 or header_size != REPLAY_HEADER.size:
        parser.error(
            f"unsupported replay format: version={version}, header_size={header_size}"
        )
    if frame_count == 0 or total_size < header_size:
        parser.error("replay header has invalid sizes")

    replay_end = REPLAY_SRAM_OFFSET + total_size
    if replay_end > len(sram):
        parser.error("replay extends past the end of the SRAM file")

    args.output.parent.mkdir(parents=True, exist_ok=True)
    if args.test_sram:
        test_sram = bytearray(len(sram))
        test_sram[:replay_end] = sram[:replay_end]
        args.output.write_bytes(test_sram)
        print(
            f"Created {len(test_sram)}-byte test SRAM with {frame_count}-frame "
            f"replay at {args.output}"
        )
    else:
        args.output.write_bytes(sram[REPLAY_SRAM_OFFSET:replay_end])
        print(
            f"Extracted {total_size} bytes ({frame_count} frames) to {args.output}"
        )


if __name__ == "__main__":
    main()
