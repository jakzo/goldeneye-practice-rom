We need the performance of the practice ROM to pretty closely match the original ROM given how important high and low framerate are for speed and certain tricks in speedruns.

## State

Use `just profile-release-us` to get stats for the current practice ROM.

All these results are collected using:
- Release US builds
- Running a deterministic Archives replay at 1.0x time scale
- Inside a custom ares v148
- Instrumented to produce Goldeneye performance profiling stats

Notes:
- "Total code size" is the sum of the linked `.code`, `.inflate`, and `.game` sections
- Frame cycles are the game completing the update loop from physics to render
- TLB loads are software code-page loads and the main source of slowdown
- The base decomp baseline is measured from the original ROM with only the deterministic Archives replay added on the `base-decomp-replay` branch

| Migration point | Total code size (bytes) | Average TLB loads/frame | Average frame cycles | Difference from base decomp | Frames |
| --- | ---: | ---: | ---: | ---: | ---: |
| Base decomp | 1,070,832 | 2.14 | 1,854,468 | 0.00% | 202 |
| 90 TLB pages | 1,169,824 | 10.34 | 2,765,109 | +49.11% | 202 |
| 95 TLB pages | 1,169,824 | 3.29 | 2,042,063 | +10.12% | 202 |
| 100 TLB pages | 1,169,792 | 1.52 | 1,865,819 | +0.61% | 202 |

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
