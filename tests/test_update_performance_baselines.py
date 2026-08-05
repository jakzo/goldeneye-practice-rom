import json
import subprocess
import sys
import tempfile
import unittest
from pathlib import Path


ROOT = Path(__file__).resolve().parent.parent
SCRIPT = ROOT / "scripts/performance/update_performance_baselines.py"
STORED_FIELDS = {
    "total_code_size_bytes": 1,
    "average_tlb_loads": 2,
    "average_frame_cycles": 3,
    "frame_cycles_difference_from_base_percent": 4,
    "frames": 5,
}


class UpdatePerformanceBaselinesTest(unittest.TestCase):
    def test_preserves_regions_without_new_profiler_results(self):
        with tempfile.TemporaryDirectory() as temp_dir:
            temp = Path(temp_dir)
            baseline_path = temp / "baseline.json"
            baseline = {
                "schema_version": 1,
                "builds": {
                    mode: {
                        region: {**STORED_FIELDS, "average_frame_cycles": old_value}
                        for region, old_value in (("US", 10), ("EU", 20), ("JP", 30))
                    }
                    for mode in ("release", "dev")
                },
            }
            baseline_path.write_text(json.dumps(baseline), encoding="utf-8")

            stats_paths = []
            for mode in ("release", "dev"):
                for region, new_value in (("US", 100), ("EU", 200)):
                    stats_path = temp / f"stats-{mode}-{region}.json"
                    stats_path.write_text(
                        json.dumps(
                            {
                                **STORED_FIELDS,
                                "build_mode": mode,
                                "region": region,
                                "average_frame_cycles": new_value,
                            }
                        ),
                        encoding="utf-8",
                    )
                    stats_paths.append(stats_path)

            subprocess.run(
                [
                    sys.executable,
                    str(SCRIPT),
                    "--baseline",
                    str(baseline_path),
                    *(str(path) for path in stats_paths),
                ],
                check=True,
            )

            updated = json.loads(baseline_path.read_text(encoding="utf-8"))
            for mode in ("release", "dev"):
                self.assertEqual(
                    updated["builds"][mode]["US"]["average_frame_cycles"], 100
                )
                self.assertEqual(
                    updated["builds"][mode]["EU"]["average_frame_cycles"], 200
                )
                self.assertEqual(
                    updated["builds"][mode]["JP"]["average_frame_cycles"], 30
                )


if __name__ == "__main__":
    unittest.main()
