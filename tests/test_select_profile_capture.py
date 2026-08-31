import os
import tempfile
import unittest
from pathlib import Path

from scripts.performance.select_profile_capture import select_profile_capture


class SelectProfileCaptureTest(unittest.TestCase):
    def test_selects_newest_nonempty_numbered_capture(self):
        with tempfile.TemporaryDirectory() as temp_dir:
            output_prefix = Path(temp_dir) / "archives"
            older = Path(f"{output_prefix}-002-game-frames.csv")
            newer = Path(f"{output_prefix}-001-game-frames.csv")
            older.write_text("frame\n", encoding="utf-8")
            newer.write_text("frame\n", encoding="utf-8")
            # Use whole-second timestamps: Windows filesystems may round
            # nanosecond values closely enough that 1ns and 2ns compare equal.
            os.utime(older, ns=(1_000_000_000, 1_000_000_000))
            os.utime(newer, ns=(2_000_000_000, 2_000_000_000))

            self.assertEqual(
                select_profile_capture(output_prefix),
                Path(f"{output_prefix}-001"),
            )

    def test_ignores_empty_and_malformed_files(self):
        with tempfile.TemporaryDirectory() as temp_dir:
            output_prefix = Path(temp_dir) / "archives"
            Path(f"{output_prefix}-001-game-frames.csv").touch()
            Path(f"{output_prefix}-abc-game-frames.csv").write_text(
                "frame\n", encoding="utf-8"
            )

            with self.assertRaises(FileNotFoundError):
                select_profile_capture(output_prefix)


if __name__ == "__main__":
    unittest.main()
