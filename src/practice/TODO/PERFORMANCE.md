We need the performance of the practice ROM to pretty closely match the original ROM given how important high and low framerate are for speed and certain tricks in speedruns.

## ares

A fork of the ares emulator source is included as the `ares` submodule. This
version of ares profiles both the practice ROM and original ROM without adding
instrumentation to either ROM. See `ares/ares/n64/PROFILER.md` and run
`just profile-ares /path/to/custom/ares`.

It uses debugging symbols to understand the locations of all functions and tracks how often every function is called during gameplay. The `base-decomp-replay` branch contains the original ROM code with minimal modifications to be able to deterministically playback a recording made using the practice ROM.

## TODO

- [x] Fork ares
- [x] Add the ares submodule
- [x] Instrument profiling code in ares for:
    - Function call count and guest-cycle timings
    - TLB cache hits/misses
    - Per-8 KiB virtual TLB page usage
    - Frame start and stop timings
    - Average frame delta
    - Automatic start and stop on level start/stop
- [x] Generate an interactive HTML flame graph from the folded-stack data
