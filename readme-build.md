# Build Readme

This file explains the details of the build process.

# Requirements

The build requires a MIPS GCC cross compiler and binutils. The provided Docker
image installs the pinned toolchain used by CI.

# Environment

The build uses the `US` version by default. Available options are `US`, `EU`, and `JP`. For example

    make clean VERSION=JP
    make VERSION=JP

# C Compiler

All C translation units are compiled by `mips-linux-gnu-gcc`. The two original
64-bit compiler-runtime C sources remain as reference material; their linked
implementations are exact standalone MIPS assembly files.

# Assembly Preprocessor

There is a (much forked) preprocessor used by many N64 projects, found in `tools/asm-processor/asm_processor.py`. This searches for a line
beginning with `GLOBAL_ASM(` and a subsequent line beginning with `)` and treats everything in between as assembly. This is bundled with any
c code in the file and sent to the compiler.

# Code build process

`src` and `src/game`: .c and .s files are compiled into .o files  

`src/libultra`: .c and .s files are compiled into .o files  

# Asset/data build process

Before compilation begins, assets are converted into .c files. This file is then compiled using the c compiler in the usual manner.
Once an .o file exists, it is converted to an .elf file using the toolchain `-ld` program, and a .ld file specification explaining
where the ELF sections should arranged in the file (and also which sections to exclude). The toolchain `-objcopy` program is
then used to dump the data in the .elf file into a similar .bin file.

The .bin file is then compressed using the standard compression program to produce a .rz file.

For compilation, having a .c file is not necessary as long the correct .bin file is available. This can be created from the extract script.

Each obseg asset category has it's own `Makefile` in the obseg folder.

Once the .rz files exist for an asset category, they can be bundled together into an .o file. All obseg assets are bundled
in `assets/obseg/ob_seg.s` and music is bundled in `assets/music/music.s`.

# Building the final ROM

Once all code and assets are compiled into .o files, these are combined into one .elf file. The layout of the object files
is given by the `ge007.*.ld` files in the root of the project. For a list of individual methods, assets, and files see
the map file in `build/[uje]/ge007.*.map` (where `*` is the country code, `u`, `e`, or `j`).

The toolchain `-objcopy` program is then used to create the bundled .bin of the entire ROM.

The final step is to run the `tools/n64cksum` program on the .bin file to create the final .z64.

## Build verification and performance tools

### Compiler manifest

`scripts/build/compiler_manifest.py` records the C inputs used by a ROM build.
The normal link runs it automatically; it can also be regenerated with:

```bash
docker run --rm -v "$(pwd):/home/dev" goldeneye make compiler-manifest
```

The default US output is `build/u/metadata/compiler-manifest.json`. Its main
fields are:

- `compiler`: the GCC executable name and pinned version.
- `flags`: the complete original-code and practice-code compiler flags.
- `counts.translation_units`: the total C compilation units represented.
- `counts.by_compiler`: the number built by each compiler; currently every
  entry is GCC.
- `counts.by_kind`: totals for original game, practice, and generated-asset C.
- `translation_units`: each source path, output object path, compiler, and
  kind.

A manifest-generation failure means a source is missing, one source was put in
two categories, or multiple sources resolve to the same object. Those are build
inventory errors and should not be bypassed. The manifest does not describe
standalone assembly, binary-only assets, or RSP microcode.

### ABI checks

`scripts/checks/abi_checks.c` is a compile-only set of assertions for the N64
ABI and important game structures. Run it with:

```bash
docker run --rm -v "$(pwd):/home/dev" goldeneye make abi-check
```

Success prints `GCC ABI assertions passed` and creates
`build/u/checks/abi_checks.gcc.o`. It does not run code. A failure appears as a
compile error whose typedef includes the failed check name, such as
`abi_assert_pointer_size` or `abi_assert_prop_pos_offset`. That means a compiler
flag, header, or structure layout changed incompatibly; changing the expected
number is only appropriate after auditing every binary consumer.

The adjacent `make gcc-global-asm-proof` target is not backed by another helper
script. It verifies that a C file containing `GLOBAL_ASM` still preprocesses,
assembles, and exports the expected symbol under GCC.

### Performance summary

`scripts/performance/profile_summary.py` converts an ares per-frame profiler
CSV plus the exact ROM ELF into a concise result. `just profile-release-us`
runs the normal release-US workflow. The script prints one Markdown table row
and, when `--output` is supplied, writes a JSON document containing:

- `frames`: captured frame count; compare this with the expected replay length
  before trusting the averages.
- `code_sections` and `total_code_size_bytes`: `.code`, `.inflate`, and `.game`
  sizes from the ELF, and their sum.
- `average_tlb_loads`: average TLB-load events per frame; lower is generally
  better when comparing the same replay.
- `average_frame_cycles`: average emulated CPU cycles per frame; lower means
  less CPU work for the same deterministic replay.
- `frame_cycles_difference_from_base_percent`: difference from the base decomp.
  Positive is slower and negative is faster.
- `frame_cycles_difference_from_previous_percent`: difference from the stored
  build for the same region/configuration when a baseline file is supplied.

`--summary` appends a human-readable Markdown comparison, while
`--max-base-regression-percent` makes the command fail when the absolute base
difference exceeds the limit. Results from a shortened, divergent, or stalled
replay are not valid performance evidence even if the script can parse them.

### Performance baseline updater

`scripts/performance/update_performance_baselines.py` replaces the recorded
build measurements in `src/practice/docs/performance_baselines.json`. It
requires exactly six profile-summary JSON files: release and DEV for US, EU,
and JP. CI runs it on master after collecting all regional results.

The command is intentionally quiet on success and updates the baseline file in
place. Review its Git diff: `base_decomp_replay` should remain unchanged, while
`builds.release` and `builds.dev` should contain one entry per region with code
size, TLB loads, frame cycles, base difference, and frame count. Missing,
duplicate, malformed, or incorrectly labelled result files cause a nonzero
exit without a partial update.
