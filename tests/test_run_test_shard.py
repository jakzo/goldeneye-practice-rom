import importlib.util
import sys
import unittest
from pathlib import Path


ROOT = Path(__file__).resolve().parents[1]
MODULE_PATH = ROOT / "scripts" / "run_test_shard.py"
SPEC = importlib.util.spec_from_file_location("run_test_shard", MODULE_PATH)
RUNNER = importlib.util.module_from_spec(SPEC)
sys.modules[SPEC.name] = RUNNER
SPEC.loader.exec_module(RUNNER)


class ShardParsingTests(unittest.TestCase):
    def test_parses_one_based_shard(self):
        self.assertEqual(RUNNER.parse_shard("5/12"), (4, 12))

    def test_rejects_invalid_shards(self):
        for shard in ("0/12", "13/12", "1/0", "one/twelve", "1"):
            with self.subTest(shard=shard):
                with self.assertRaises(ValueError):
                    RUNNER.parse_shard(shard)


class BalancingTests(unittest.TestCase):
    def task(self, name, estimate):
        return RUNNER.TestTask(name, "practice", "us", estimate)

    def test_longest_tasks_fill_lightest_shard_deterministically(self):
        tasks = [
            self.task("d", 4),
            self.task("a", 8),
            self.task("c", 5),
            self.task("b", 7),
        ]

        shards = RUNNER.balance_tasks(tasks, 2)

        self.assertEqual(
            [[task.task_id for task in shard] for shard in shards],
            [["a", "d"], ["b", "c"]],
        )
        self.assertEqual([sum(task.estimate_frames for task in shard) for shard in shards], [12, 12])

    def test_every_discovered_task_is_assigned_once(self):
        tasks = RUNNER.discover_tasks()
        shards = RUNNER.balance_tasks(tasks, 12)

        assigned = [task.task_id for shard in shards for task in shard]
        loads = [sum(task.estimate_frames for task in shard) for shard in shards]
        self.assertEqual(len(assigned), len(set(assigned)))
        self.assertEqual(set(assigned), {task.task_id for task in tasks})
        self.assertTrue(all(shard for shard in shards))
        self.assertLessEqual(max(loads), min(loads) * 1.05)

    def test_estimates_mark_practice_tests_short_and_scale_replays(self):
        tasks = {task.task_id: task for task in RUNNER.discover_tasks()}
        practice = tasks["practice/us/STATE_DOOR"]
        replay = tasks["replay/us/03-runway"]
        cold_restart = tasks["save-state/us/cold-restart/03-runway"]

        self.assertEqual(practice.estimate_frames, RUNNER.SHORT_TEST_ESTIMATE_FRAMES)
        self.assertEqual(RUNNER.estimate_label(practice), "short")
        self.assertGreater(replay.estimate_frames, practice.estimate_frames)
        self.assertEqual(cold_restart.estimate_frames, replay.estimate_frames * 4)

    def test_save_state_tasks_force_one_emulator_and_release_build(self):
        task = next(
            task
            for task in RUNNER.discover_tasks()
            if task.task_id == "save-state/us/default/03-runway"
        )

        _, environment = RUNNER.task_command(task, ROOT / "build/test-artifacts")

        self.assertEqual(environment["REPLAY_SAVE_STATE_JOBS"], "1")
        self.assertEqual(environment["REPLAY_SAVE_STATE_BUILD_MODE"], "release")
        self.assertEqual(environment["REPLAY_SAVE_STATE_SKIP_BUILD"], "1")


if __name__ == "__main__":
    unittest.main()
