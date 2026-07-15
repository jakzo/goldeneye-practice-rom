image := "goldeneye"
test_image := "goldeneye-test"
ares_bin := "ares/build_macos/desktop-ui/Release/ares.app/Contents/MacOS/ares"

_default:
    just -l

# cleans up all files related to building the rom
clean:
    docker run --rm -v $(pwd):/home/dev {{ image }} make nuke

# cleans everything to start from a clean slate
clean_all:
    just clean || true
    docker rmi {{ image }}
    echo "Make sure to run 'just setup' again."

# Full clean and re-extract of assets
reset:
    git clean -fdx -e '.vscode' -e 'baserom.u.z64' -e '**/*.c' -e '**/*.h'
    docker image rm {{ image }}
    docker build -t {{ image }} .
    docker run --rm -v "$(pwd):/home/dev" {{ image }} ./scripts/extract_baserom.u.sh

# run this to setup your local machine for development
setup:
    docker build -t {{ image }} .
    docker run --rm -v "$(pwd):/home/dev" {{ image }} ./scripts/extract_baserom.u.sh
    if [ -f baserom.e.z64 ]; then docker run --rm -v $(pwd):/home/dev {{ image }} ./scripts/extract_diff.e.sh; fi
    if [ -f baserom.j.z64 ]; then docker run --rm -v $(pwd):/home/dev {{ image }} ./scripts/extract_diff.j.sh; fi

# build the rom
make:
    if test -z "$(docker images -q {{ image }})"; then just setup; fi
    docker run --rm -v "$(pwd):/home/dev" {{ image }} make -j{{ num_cpus() }}
    if [ -f baserom.e.z64 ]; then docker run --rm -v "$(pwd):/home/dev" {{ image }} make -j{{ num_cpus() }} VERSION=EU; fi
    if [ -f baserom.j.z64 ]; then docker run --rm -v "$(pwd):/home/dev" {{ image }} make -j{{ num_cpus() }} VERSION=JP; fi

make-dev:
    docker run --rm -v "$(pwd):/home/dev" {{ image }} make -j{{ num_cpus() }} DEV=1
    if [ -f baserom.e.z64 ]; then docker run --rm -v "$(pwd):/home/dev" {{ image }} make -j{{ num_cpus() }} DEV=1 VERSION=EU; fi
    if [ -f baserom.j.z64 ]; then docker run --rm -v "$(pwd):/home/dev" {{ image }} make -j{{ num_cpus() }} DEV=1 VERSION=JP; fi

make-clean: reset
    docker run --rm -v $(pwd):/home/dev {{ image }} make -j{{ num_cpus() }}

# Incrementally build the custom ares submodule used by host-side recipes.
build-ares:
    if [ ! -f ares/build_macos/CMakeCache.txt ]; then cd ares && DEVELOPER_DIR="${DEVELOPER_DIR:-/Applications/Xcode.app/Contents/Developer}" cmake --preset macos; fi
    DEVELOPER_DIR="${DEVELOPER_DIR:-/Applications/Xcode.app/Contents/Developer}" cmake --build ares/build_macos --config Release --parallel {{ num_cpus() }}

ares: build-ares
    docker run --rm -v $(pwd):/home/dev {{ image }} make -j{{ num_cpus() }}
    "{{ ares_bin }}" ./build/u/ge007.u.z64

ares-dev BOOT_LEVEL="TITLE": build-ares
    docker run --rm -v $(pwd):/home/dev {{ image }} make -j{{ num_cpus() }} DEV=1 BOOT_LEVEL={{ BOOT_LEVEL }}
    "{{ ares_bin }}" --setting DebugServer/Enabled=true --setting DebugServer/UseIPv4=true --setting DebugServer/Port=9123 ./build/u/ge007.u.z64

test-debug TEST_CASE REGION="us": build-ares
    #!/usr/bin/env bash
    set -euo pipefail
    region="$(printf '%s' "{{ REGION }}" | tr '[:upper:]' '[:lower:]')"
    case "$region" in
        us) version="US"; outcode="u" ;;
        eu) version="EU"; outcode="e" ;;
        jp) version="JP"; outcode="j" ;;
        *) echo "error: region must be one of: us, eu, jp" >&2; exit 2 ;;
    esac
    docker run --rm -v "$(pwd):/home/dev" {{ image }} make -j{{ num_cpus() }} DEV=1 VERSION="$version" TEST_CASE="{{ TEST_CASE }}"
    "{{ ares_bin }}" --setting DebugServer/Enabled=true --setting DebugServer/UseIPv4=true --setting DebugServer/Port=9123 "./build/$outcode/ge007.$outcode.z64"

test TEST_CASE:
    if test -z "$(docker images -q {{ test_image }})"; then docker build --target test -t {{ test_image }} .; fi
    docker run --rm -v "$(pwd):/home/dev" {{ test_image }} bash ./scripts/run_practice_tests_docker.sh --test "{{ TEST_CASE }}"

# Profile the deterministic Archives replay and save per-frame CPU timings.
profile-archives OUTPUT="src/practice/docs/profile_archives_practice.csv":
    if test -z "$(docker images -q {{ test_image }})"; then docker build --target test -t {{ test_image }} .; fi
    docker run --rm -v "$(pwd):/home/dev" {{ test_image }} bash ./scripts/run_practice_tests_docker.sh --test REPLAY_ARCHIVES --timeout 480 --profile-csv "{{ OUTPUT }}"

# Run the symbol-aware ares profiler. Leave the level normally to flush the capture.
profile-ares ROM="build/u/ge007.u.z64" ELF="build/u/ge007.u.elf" OUTPUT="build/profile/ge007": build-ares
    test -x "{{ ares_bin }}"
    test -f "{{ ROM }}"
    test -f "{{ ELF }}"
    mkdir -p "$(dirname "{{ OUTPUT }}")"
    ARES_N64_PROFILE_SYMBOLS="$(pwd)/{{ ELF }}" ARES_N64_PROFILE_OUTPUT="$(pwd)/{{ OUTPUT }}" "{{ ares_bin }}" --no-file-prompt "$(pwd)/{{ ROM }}"

# Build and profile the release US ROM running the deterministic Archives replay.
profile-release-us OUTPUT="build/profile/archives-release-us" LABEL="Current release US":
    if test -z "$(docker images -q {{ image }})"; then just setup; fi
    docker run --rm -v "$(pwd):/home/dev" {{ image }} make -j{{ num_cpus() }} DEV=0 VERSION=US COMPARE=0 TEST_CASE=REPLAY_ARCHIVES
    cp tests/replays/archives.ram build/u/ge007.u.ram
    ARES_N64_PROFILE_REPLAY=1 just profile-ares build/u/ge007.u.z64 build/u/ge007.u.elf "{{ OUTPUT }}"
    just profile-ares-flamegraph "{{ OUTPUT }}-001.folded" "{{ OUTPUT }}-001.html"
    docker run --rm -v "$(pwd):/home/dev" {{ image }} python3 scripts/performance/profile_summary.py --elf build/u/ge007.u.elf --csv "{{ OUTPUT }}-001-game-frames.csv" --phase "{{ LABEL }}" --build-mode release --region US --baseline src/practice/docs/performance_baselines.json --output "{{ OUTPUT }}-performance.json"

# Render one profiler .folded capture as an interactive HTML flame graph.
profile-ares-flamegraph INPUT OUTPUT="":
    python3 ares/tools/n64-profiler-flamegraph.py "{{ INPUT }}" {{ if OUTPUT == "" { "" } else { '"' + OUTPUT + '"' } }}

test-all JOBS="":
    if test -z "$(docker images -q {{ test_image }})"; then docker build --target test -t {{ test_image }} .; fi
    docker run --rm -v "$(pwd):/home/dev" -e PRACTICE_TEST_JOBS="{{ JOBS }}" {{ test_image }} bash ./scripts/run_practice_tests_docker.sh --build-mode release

sc64-dev BOOT_LEVEL="TITLE":
    docker run --rm -v $(pwd):/home/dev {{ image }} make -j{{ num_cpus() }} DEV=1 BOOT_LEVEL={{ BOOT_LEVEL }}
    sc64deployer upload build/u/ge007.u.z64
    sc64deployer debug

# builds the rom and uploads it to a connected summercart64
sc64: make
    sc64deployer sd mkdir /CUSTOM || true
    sc64deployer sd upload build/u/ge007.u.z64 /CUSTOM/practice-rom_latest_ntsc_u.z64
    if [ -f build/e/ge007.e.z64 ]; then sc64deployer sd upload build/e/ge007.e.z64 /CUSTOM/practice-rom_latest_pal.z64; fi
    if [ -f build/j/ge007.j.z64 ]; then sc64deployer sd upload build/j/ge007.j.z64 /CUSTOM/practice-rom_latest_ntsc_j.z64; fi
