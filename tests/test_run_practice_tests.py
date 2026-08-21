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

    def test_recording_replay_keeps_overall_timeout(self):
        previous = RUNNER.REPLAY_STATUS_TIMEOUT_SECONDS
        RUNNER.REPLAY_STATUS_TIMEOUT_SECONDS = 0.1
        try:
            passed, detail, _ = self.run_script(
                "REPLAY",
                "import time; time.sleep(0.2); "
                "print('TEST_COMPLETE', flush=True)",
                0.4,
            )
        finally:
            RUNNER.REPLAY_STATUS_TIMEOUT_SECONDS = previous

        self.assertTrue(passed)
        self.assertEqual(detail, "completed")


class ReplayPackTests(unittest.TestCase):
    def test_write_replay_pack_extends_rom_and_records_offsets(self):
        with tempfile.TemporaryDirectory() as directory:
            rom = Path(directory) / "test.z64"
            rom.write_bytes(b"\xff" * (RUNNER.ROM_CONFIG_OFFSET + 64))
            first = b"GERP" + b"\x01" * 44
            second = b"GERP" + b"\x02" * 60
            RUNNER.write_replay_pack(rom, [first, second])

            data = rom.read_bytes()
            self.assertGreaterEqual(len(data), RUNNER.TEST_REPLAY_PACK_ROM_SIZE)
            header = data[
                RUNNER.TEST_REPLAY_ROM_OFFSET : RUNNER.TEST_REPLAY_ROM_OFFSET + 16
            ]
            self.assertEqual(
                int.from_bytes(header[0:4], "big"), RUNNER.TEST_REPLAY_PACK_MAGIC
            )
            self.assertEqual(int.from_bytes(header[6:8], "big"), 2)
            self.assertEqual(
                int.from_bytes(header[8:12], "big"),
                RUNNER.TEST_REPLAY_PACK_DATA_OFFSET,
            )
            entries = data[
                RUNNER.TEST_REPLAY_ROM_OFFSET
                + 16 : RUNNER.TEST_REPLAY_ROM_OFFSET
                + 32
            ]
            self.assertEqual(int.from_bytes(entries[0:4], "big"), 0)
            self.assertEqual(int.from_bytes(entries[4:8], "big"), len(first))
            self.assertEqual(int.from_bytes(entries[8:12], "big"), 48)
            self.assertEqual(int.from_bytes(entries[12:16], "big"), len(second))
            payload_base = RUNNER.TEST_REPLAY_PACK_DATA_OFFSET
            self.assertEqual(data[payload_base : payload_base + len(first)], first)
            self.assertEqual(
                data[payload_base + 48 : payload_base + 48 + len(second)], second
            )


if __name__ == "__main__":
    unittest.main()
