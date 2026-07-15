# Goldeneye 007

[![NTSC-Status][NTCS-badge]][NTCS-link]
[![JP-Status][JP-badge]][JP-link]
[![PAL-Status][PAL-badge]][PAL-link]

[NTCS-link]: https://kholdfuzion.github.io/goldeneyestatus/
[NTCS-badge]: ../../workflows/NTSC-Status/badge.svg

[JP-link]: https://kholdfuzion.github.io/goldeneyestatus/JPN.htm
[JP-badge]: ../../workflows/JP-Status/badge.svg

[PAL-link]: https://kholdfuzion.github.io/goldeneyestatus/EU.htm
[PAL-badge]: ../../workflows/EU-Status/badge.svg


This is a WIP decompilation of Goldeneye 007!

It builds the following ROMs:

* ge007.u.z64 `sha1: abe01e4aeb033b6c0836819f549c791b26cfde83`
* ge007.j.z64 `sha1: 2a5dade32f7fad6c73c659d2026994632c1b3174`
* ge007.e.z64 `sha1: 167c3c433dec1f1eb921736f7d53fac8cb45ee31`

**Note: This repository does not include all assets necessary for compiling the ROMs. A prior copy of the game is required to extract the assets.**

## Installation

### Linux (Native or under WSL / VM)

#### Step 1: Install build dependencies

The requirements for Debian / Ubuntu should be:

```bash
sudo apt-get update
sudo apt-get install binutils-mips-linux-gnu make git python3
sudo apt-get install libcapstone-dev pkg-config
```

If you don't have host development tools already installed then you will also need to install `build-essential`:

```bash
sudo apt-get install build-essential
```

#### Step 2: Clone the repository

Clone this repository where you wish to have the project, with a command such as:

```bash
git clone https://github.com/kholdfuzion/goldeneye_src
```

#### Step 3: Prepare baserom(s) for asset extraction

Place an unmodified copy of your existing NTSC (US) ROM inside the root of this repository with the name `baserom.u.z64`.

To extract the baserom assets run:

```bash
./scripts/extract_baserom.u.sh
```

For JP and PAL (EU) versions support, place each existing ROM in the root of this repository with the name `baserom.<VERSION>.z64` (where `<VERSION>` is the country code, `j`, or `e`).

Extracting NTSC (US) baserom assets is mandatory before extracting JP or PAL assets.

To extract JP assets run:

```bash
./scripts/extract_baserom.u.sh && ./scripts/extract_diff.j.sh
```

To extract PAL assets run:

```bash
./scripts/extract_baserom.u.sh && ./scripts/extract_diff.e.sh
```

Other options to extract baserom assets or extract diff:

```bash
./scripts/extract_baserom.u.sh /path_to/rom.n64 # ROM in another directory
./scripts/extract_baserom.u.sh /mnt/e/Goldeneye.n64 # ROM located on EverDrive
./scripts/extract_baserom.u.sh files # Extract files only
./scripts/extract_baserom.u.sh images # Extract images only
```

Note: If you are upgrading from an old repository, run:

```bash
./scripts/clean_baserom.sh && ./scripts/extract_baserom.u.sh && make clean
```

### Install using Docker (compatible with Apple silicon Macs)

Once the Docker service is running on your computer (you must have Docker installed), clone the GoldenEye repo where you'd like to have it (Step 2 of the previous section).

Build the image: `docker build -t goldeneye .`

`cd` to your cloned `goldeneye_src` repo's directory. You can change `$(pwd)` below to the absolute path of your directory if you don't want to do this.

Connect to the container: `docker run --rm -it -v $(pwd):/home/dev goldeneye`

All what's left is to go through Steps 3 and 4 of the previous section.

Be careful! If you previously compiled GoldenEye on another system (differente OS or CPU architecture), the binaries (gzip, n64cksum) that were compiled will be incompatible. You must delete them.

There may be a "dubious ownership" error from Git and it may say it fails to detect the Git repository. Running `git status`, it should tell you how to fix it.

## Build the ROM

Run `make` to build the ROM (defaults to `VERSION=US`).

```bash
make
```

If all goes well, resulting artifacts can be found in the `build` directory and the following text should be printed:

```bash
build/u/ge007.u.z64: OK
```

Other examples:

```bash
make VERSION=JP -j4       # build JP version instead with 4 jobs
make VERSION=EU COMPARE=0 # build PAL (EU) version but do not compare ROM hashes
```

The full list of configurable variables are listed below, with the default being the first listed:

* ``VERSION``: ``US``, ``JP``, ``EU``
* ``COMPARE``: ``1`` (compare ROM hash), ``0`` (do not compare ROM hash)
* ``FINAL``: ``YES`` (builds the optimized version), ``NO`` (debug)
* ``VERBOSE``: ``0`` (quiet), ``1``

Additional documentation of the build process can be found [here](readme-build.md).

## Project Structure

```
goldeneye_src
|-- .github/workflows: GitHub use only
├── assets: game assets
│   ├── font: font data
│   ├── images: image data
│   │   └── split: split image data
│   ├── music: music data
│   ├── obseg: animation data
│   │   ├── bg: bg data
│   │   ├── brief: briefing data
│   │   ├── chr: c model data
│   │   ├── gun: g model data
│   │   ├── prop: p model data
│   │   ├── setup: setup data
│   │   ├── stan: stan data
│   │   └── text: text data
│   └── ramrom: demo data
├── bin: files that haven't been touched
├── build: output directory
├── include: header files
├── rsp: Custom GBI code (Assembly) (C0 and 4Tri)
├── src: C source code for game
│   ├── game: core ge specific code 0x7f000000 range
│   ├── inflate: statically linked initial decompression code
│   ├── libultra: currently used libultra.s
│   └── libultrarare: libultra modified by Rare
└── tools: build tools
```

Documentation has been moved to https://github.com/kholdfuzion/goldeneye_docs/tree/master/notes

Style Guide is https://github.com/kholdfuzion/goldeneye_src/blob/AIListLogic/notes/StyleGuide.md

This decompilation was only made possible thanks to many awesome 00 Agents who will be revealed only if they wish.

GE and PD documentation made by Zoinkity.
