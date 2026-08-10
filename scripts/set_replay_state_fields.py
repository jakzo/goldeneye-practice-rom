#!/usr/bin/env python3
"""Replace replay .state headers with the shared diagnostic field set.

The payload is resized to the correct frame count so n64-replay can validate
the template before --regenerate-state replaces it with base-ROM values.
"""

import argparse
from pathlib import Path


ROOT = Path(__file__).resolve().parents[1]
REPLAY_ROOT = ROOT / "tests" / "replays"
CHR_SIZE = 476
REPLAY_HEADER_OFFSET = 0x600

# Counts cover the CHR records whose common state is initialized in both the
# base and practice builds, plus the ten reserve slots omitted below. Facility
# slot 64 is allocator residue in the base build but is claimed by practice
# setup, so its portable initialized range ends one record earlier.
STAGE_CHR_COUNTS = {
    "01-dam": (46, 8),
    "02-facility": (74, 15),
    "03-runway": (20, 9),
    "04-surface1": (19, 12),
    "07-frigate": (57, 8),
    "11-archives": (40, 3),
}


def replay_frame_count(path: Path) -> int:
    data = path.read_bytes()
    header = data[REPLAY_HEADER_OFFSET : REPLAY_HEADER_OFFSET + 16]
    if len(header) < 16:
        raise ValueError(f"{path}: replay header is truncated")
    if header[:4] != b"GERP":
        raise ValueError(f"{path}: replay header is missing")
    return int.from_bytes(header[12:16], "big")


def state_fields(stage: str, chr_count: int, background_count: int) -> list[str]:
    fields = [
        "g_randomSeed[0,8]",
        "g_chrObjRandomSeed[0,8]",
        "current_random_body[0,4]",
        "current_random_male_head[0,4]",
        "current_random_female_head[0,4]",
        "camera_transition_timer[0,4]",
        "intro_camera_index[0,4]",
        "g_SetupIntroCameraCount[0,4]",
        "g_GlobalTimer[0,4]",
        "D_80048380[0,4]",
        "g_GlobalTimerDelta[0,4]",
        "g_ClockTimer[0,4]",
        "speedgraphframes[0,4]",
        "g_NumChrSlots[0,4]",
        "g_ActiveChrsCount[0,4]",
        "alarm_timer[0,4]",
        "objectiveregisters1[0,4]",
        "g_NumExplosionEntries[0,4]",
        "g_NumSmokeEntries[0,4]",
        "g_NumParticleEntries[0,4]",
        "max_particles[0,4]",
        "*g_CurrentPlayer[952,92]",
        "*g_CurrentPlayer[1052,24]",
        "*g_CurrentPlayer[1164,44]",
    ]

    # Surface's viewer prop is slot 183. Guard sight checks consume this world
    # position directly; tracking it exposes the first movement divergence
    # instead of the later randomGetNext symptom.
    if stage == "04-surface1":
        fields.append("pos_data_entry[9524,12]")

    # Keep only ranges initialized for every allocated slot. Most of the action
    # union and the tail of ChrRecord are left as allocator residue until a slot
    # becomes active, so recording those bytes produces false frame-zero
    # divergences between otherwise equivalent ROM layouts.
    chr_ranges = (
        # Omit hidden at 0x12: it contains one-render weapon/tracer latches,
        # which legitimately span a different checkpoint when a zero-tick
        # practice render is skipped. Keep identity/action and chrflags.
        (0, 18),
        (20, 4),
        (36, 8),
        (164, 64),
        (232, 28),
    )
    # alloc_init_GUARDdata_entries adds ten reserve slots and initializes only
    # their model pointers. Their remaining bytes are not state until a runtime
    # spawn claims one, so omit those allocator-residue records.
    for slot in range(chr_count - 10):
        for offset, size in chr_ranges:
            fields.append(f"*g_ChrSlots[{slot * CHR_SIZE + offset},{size}]")

    # Background AI records have no prop/model. Track their interpreter and
    # common scalar state without serializing the AI-list pointer at 0x104.
    background_ranges = ((0, 24), (232, 28), (264, 24))
    for slot in range(background_count):
        for offset, size in background_ranges:
            fields.append(f"*g_ActiveChrs[{slot * CHR_SIZE + offset},{size}]")
    return fields


def field_size(field: str) -> int:
    return int(field[field.rfind(",") + 1 : -1], 0)


def main() -> None:
    parser = argparse.ArgumentParser()
    parser.add_argument("states", nargs="*", type=Path)
    args = parser.parse_args()
    states = args.states or sorted(REPLAY_ROOT.glob("*/*.state"))
    for configured in states:
        state = configured if configured.is_absolute() else ROOT / configured
        replay = state.with_suffix(".ram")
        frames = replay_frame_count(replay)
        try:
            chr_count, background_count = STAGE_CHR_COUNTS[state.stem]
        except KeyError as exc:
            raise ValueError(f"{state}: unknown replay stage") from exc
        fields = state_fields(state.stem, chr_count, background_count)
        header = (",".join(fields) + "\n").encode()
        frame_size = sum(field_size(field) for field in fields)
        state.write_bytes(header + bytes(frames * frame_size))
        print(f"{state.relative_to(ROOT)}: {len(fields)} fields, "
              f"{frame_size} bytes/frame, {frames} frames")


if __name__ == "__main__":
    main()
