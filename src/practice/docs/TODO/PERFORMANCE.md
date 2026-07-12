We need the performance of the practice ROM to pretty closely match the original ROM given how important high and low framerate are for speed and certain tricks in speedruns.

## State

Use `just profile-release-us` to get stats for the current practice ROM.

All these results are collected using:
- Release build of US practice ROM
- Running a deterministic Runway replay
- Inside a custom ares v148
- Instrumented to produce Goldeneye performance profiling stats

Notes:
- "Total code size" is the sum of the linked `.code`, `.inflate`, and `.game` sections
- Frame cycles are the game completing the update loop from physics to render
- TLB loads are software code-page loads and the main source of slowdown
- The base decomp baseline is measured from the original ROM with only the deterministic Runway replay added on the `base-decomp-replay` branch

| Migration point | Total code size (bytes) | Average TLB loads/frame | Average frame cycles | Difference from base decomp | Frames |
| --- | ---: | ---: | ---: | ---: | ---: |
| Base decomp (no practice ROM changes; original code IDO `-O2`) | 1,071,808 | 0.66 | 1,156,528 | 0.00% | 689 |
| Pre-migration baseline after Phase 0 (original code IDO `-O2`, practice GCC `-Os`) | 1,171,568 | 4.48 | 1,614,545 | +39.60% | 689 |
| Current release US | 1,170,336 | 6.95 | 1,872,725 | +61.93% | 689 |
| 103 TLB pages | 1,170,336 | 0.27 | 1,184,755 | +2.44% | 689 |

## ares profiler

A fork of the ares emulator source is included as the `ares` submodule. This
version of ares profiles both the practice ROM and original ROM without adding
instrumentation to either ROM. See `ares/ares/n64/PROFILER.md` and run
`just profile-ares /path/to/custom/ares`.

It uses debugging symbols to understand the locations of all functions and tracks how often every function is called during gameplay. The `base-decomp-replay` branch contains the original ROM code with minimal modifications to be able to deterministically playback a recording made using the practice ROM.

## TODO

- Finish GCC migration and compile with `-Os`
- Optimize existing game memory usage to add ~100kb of TLB virtual caches
- Find out which TLB pages are hot and cold so we can prioritize keeping hot pages (will change over time and need to be regularly recalculated)
- Optimize game code? (eg. `debmenuDraw` is using 1.4% of frame cycles in US ROM, but it was removed from PAL already)
