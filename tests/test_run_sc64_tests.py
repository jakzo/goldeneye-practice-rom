import tempfile
import unittest
from pathlib import Path
from unittest import mock

from scripts import patch_practice_rom
from scripts import run_sc64_tests


class SelectionTests(unittest.TestCase):
    def test_single_test_is_selected(self):
        self.assertEqual(
            run_sc64_tests.selected_tests("MIGRATION_MATH"), ["MIGRATION_MATH"]
        )

    def test_manual_restart_test_is_excluded_from_suite(self):
        self.assertNotIn(
            "REPLAY_RUNWAY_SAVE_STATES", run_sc64_tests.selected_tests()
        )


class UploadCommandTests(unittest.TestCase):
    def test_reboot_upload_uses_sram1m_save(self):
        command = run_sc64_tests.upload_command(
            "sc64deployer", Path("test.z64"), Path("test.ram"), True
        )
        self.assertEqual(command[:2], ["sc64deployer", "upload"])
        self.assertIn("--reboot", command)
        self.assertEqual(command[-2], "sram1m")

    def test_initial_upload_omits_reboot(self):
        command = run_sc64_tests.upload_command(
            "sc64deployer", Path("test.z64"), Path("test.ram"), False
        )
        self.assertNotIn("--reboot", command)

    def test_rom_only_upload_preserves_existing_sram(self):
        command = run_sc64_tests.upload_command(
            "sc64deployer", Path("test.z64"), None, True
        )
        self.assertNotIn("--save", command)
        self.assertIn("--reboot", command)


class PreparationTests(unittest.TestCase):
    def test_non_replay_gets_blank_sram_and_runtime_config(self):
        with tempfile.TemporaryDirectory() as directory:
            root = Path(directory)
            base_rom = root / "base.z64"
            base_rom.write_bytes(b"\xff" * (patch_practice_rom.ROM_CONFIG_OFFSET + 64))

            with mock.patch.object(
                run_sc64_tests.run_practice_tests,
                "install_replay_fixture",
                return_value=None,
            ):
                rom, save = run_sc64_tests.prepare_test_rom(
                    base_rom, root / "output", "MIGRATION_MATH"
                )

            self.assertEqual(save.stat().st_size, run_sc64_tests.SRAM_SIZE)
            config = rom.read_bytes()[
                patch_practice_rom.ROM_CONFIG_OFFSET:
                patch_practice_rom.ROM_CONFIG_OFFSET + 64
            ]
            self.assertNotEqual(config, b"\xff" * 64)


if __name__ == "__main__":
    unittest.main()
