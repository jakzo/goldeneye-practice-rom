import importlib.util
import sys
import tempfile
import threading
import unittest
from pathlib import Path


ROOT = Path(__file__).resolve().parents[1]
MODULE_PATH = ROOT / "scripts" / "run_practice_tests.py"
SPEC = importlib.util.spec_from_file_location("run_practice_tests", MODULE_PATH)
RUNNER = importlib.util.module_from_spec(SPEC)
sys.modules[SPEC.name] = RUNNER
SPEC.loader.exec_module(RUNNER)


class ReplayTimeoutTests(unittest.TestCase):
    def run_script(self, test_case, source, timeout):
        with tempfile.TemporaryDirectory() as directory:
            runtime_dir = Path(directory)
            rom = runtime_dir / "test.z64"
            rom.touch()
            return RUNNER.run_test(
                test_case,
                [sys.executable, "-u", "-c", source],
                rom,
                runtime_dir,
                threading.Event(),
                timeout,
                False,
            )

    def test_replay_status_extends_deadline(self):
        previous = RUNNER.REPLAY_STATUS_TIMEOUT_SECONDS
        RUNNER.REPLAY_STATUS_TIMEOUT_SECONDS = 0.4
        try:
            passed, detail, _ = self.run_script(
                "REPLAY_FRIGATE",
                "import time; "
                "print('REPLAY_STARTED', flush=True); time.sleep(0.25); "
                "print('REPLAY_STATUS frame=1/2', flush=True); time.sleep(0.25); "
                "print('TEST_COMPLETE', flush=True)",
                0.4,
            )
        finally:
            RUNNER.REPLAY_STATUS_TIMEOUT_SECONDS = previous

        self.assertTrue(passed)
        self.assertEqual(detail, "completed")

    def test_replay_times_out_waiting_for_start(self):
        previous = RUNNER.REPLAY_STATUS_TIMEOUT_SECONDS
        RUNNER.REPLAY_STATUS_TIMEOUT_SECONDS = 0.1
        try:
            passed, detail, _ = self.run_script(
                "REPLAY_FRIGATE", "import time; time.sleep(1)", 1
            )
        finally:
            RUNNER.REPLAY_STATUS_TIMEOUT_SECONDS = previous

        self.assertFalse(passed)
        self.assertEqual(detail, "replay did not start within 0.1s")

    def test_replay_times_out_after_status_stops(self):
        previous = RUNNER.REPLAY_STATUS_TIMEOUT_SECONDS
        RUNNER.REPLAY_STATUS_TIMEOUT_SECONDS = 0.1
        try:
            passed, detail, _ = self.run_script(
                "REPLAY_FRIGATE",
                "import time; print('REPLAY_STARTED', flush=True); time.sleep(1)",
                1,
            )
        finally:
            RUNNER.REPLAY_STATUS_TIMEOUT_SECONDS = previous

        self.assertFalse(passed)
        self.assertEqual(
            detail, "no replay status update received within 0.1s"
        )

    def test_non_replay_keeps_overall_timeout(self):
        passed, detail, _ = self.run_script(
            "STATE_DOOR",
            "import time; print('still running', flush=True); time.sleep(1)",
            0.1,
        )

        self.assertFalse(passed)
        self.assertEqual(detail, "timed out after 0.1s")


if __name__ == "__main__":
    unittest.main()
