#!/usr/bin/env python3

import argparse
import struct
from pathlib import Path


REPLAY_MAGIC = 0x47455250
REPLAY_HEADER = struct.Struct(">IHHII4BHHQQII")
REPLAY_SRAM_OFFSET = 0x280
LEGACY_REPLAY_SRAM_OFFSET = 0x600
REPLAY_FLAG_FRAME_SEEDS = 0x01


def replay_frame_boundaries(path):
    data = path.read_bytes()
    replay_offset = next(
        (
            offset
            for offset in (REPLAY_SRAM_OFFSET, LEGACY_REPLAY_SRAM_OFFSET)
            if len(data) >= offset + 4
            and struct.unpack_from(">I", data, offset)[0] == REPLAY_MAGIC
        ),
        REPLAY_SRAM_OFFSET,
    )
    header = REPLAY_HEADER.unpack_from(data, replay_offset)
    magic, version, header_size, total_size, frame_count = header[:5]
    flags = header[8]
    duration = header[13]
    if magic != REPLAY_MAGIC or version != 1 or header_size != REPLAY_HEADER.size:
        raise ValueError("unsupported replay header")

    position = replay_offset + header_size
    end = replay_offset + total_size
    boundaries = []
    timestamp = 0
    frames = 0
    while position < end and frames < frame_count:
        delta = data[position]
        position += 1
        if delta == 0:
            position += 2
            continue
        if flags & REPLAY_FLAG_FRAME_SEEDS:
            position += 16
        position += 4
        timestamp += delta
        boundaries.append(timestamp)
        frames += 1

    if frames != frame_count or timestamp != duration or position != end:
        raise ValueError("replay stream does not match its header")
    return boundaries, duration


def main():
    parser = argparse.ArgumentParser()
    parser.add_argument("replay_sram", type=Path)
    args = parser.parse_args()

    try:
        boundaries, duration = replay_frame_boundaries(args.replay_sram)
    except (OSError, ValueError, struct.error) as error:
        parser.error(str(error))

    snapshots = []
    boundary_index = 0
    for nominal in range(60, duration - 59, 60):
        while boundaries[boundary_index] < nominal:
            boundary_index += 1
        snapshots.append((nominal, boundaries[boundary_index]))

    for index, (nominal, actual) in enumerate(snapshots):
        segment_end = (
            snapshots[index + 1][1] if index + 1 < len(snapshots) else duration
        )
        print(nominal, actual, segment_end)


if __name__ == "__main__":
    main()
