import tempfile
import unittest
from pathlib import Path

from scripts import run_sc64_save_state_tests


class SaveStateParameterTests(unittest.TestCase):
    def test_standard_parameters_match_suite(self):
        replay = Path("unused.ram")
        self.assertEqual(
            run_sc64_save_state_tests.test_param("default", replay),
            1 | (3 << 8) | (1 << 16),
        )
        self.assertEqual(
            run_sc64_save_state_tests.test_param("long", replay),
            10 | (4 << 8) | (30 << 16),
        )
        self.assertEqual(
            run_sc64_save_state_tests.test_param("cameras", replay),
            1 | (3 << 8) | (1 << 16) | (3 << 24),
        )

    def test_checkpoint_marker_is_parsed(self):
        match = run_sc64_save_state_tests.CHECKPOINT_RE.search(
            "RUNWAY_RESTART_SEGMENT_COMPLETE timestamp=123 elapsed=456\n"
        )
        self.assertEqual(match.groups(), ("123", "456"))


class ReplaySelectionTests(unittest.TestCase):
    def test_named_replay_accepts_ram_suffix(self):
        with tempfile.TemporaryDirectory() as directory:
            fixture_dir = Path(directory)
            fixture = fixture_dir / "03-runway.ram"
            fixture.touch()
            original = run_sc64_save_state_tests.FIXTURE_DIR
            try:
                run_sc64_save_state_tests.FIXTURE_DIR = fixture_dir
                self.assertEqual(
                    run_sc64_save_state_tests.replay_fixtures("03-runway.ram"),
                    [fixture],
                )
            finally:
                run_sc64_save_state_tests.FIXTURE_DIR = original


if __name__ == "__main__":
    unittest.main()
