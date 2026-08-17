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
    cd ares && DEVELOPER_DIR="${DEVELOPER_DIR:-/Applications/Xcode.app/Contents/Developer}" cmake --preset macos -DARES_CORES=n64 -DARES_BUILD_OPTIONAL_TARGETS=OFF
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
    docker run --rm -v "$(pwd):/home/dev" -w /home/dev {{ test_image }} make -j{{ num_cpus() }} DEV=1 VERSION="$version" TEST_CASE="{{ TEST_CASE }}"
    python3 -c 'import sys; from pathlib import Path; from scripts.run_practice_tests import install_replay_fixture; install_replay_fixture(sys.argv[1], Path(sys.argv[2]), sys.argv[3])' "{{ TEST_CASE }}" "./build/$outcode/ge007.$outcode.z64" "$version"
    "{{ ares_bin }}" --setting Nintendo64/ExpansionPak=true --setting DebugServer/Enabled=true --setting DebugServer/UseIPv4=true --setting DebugServer/Port=9123 "./build/$outcode/ge007.$outcode.z64"

test TEST_CASE:
    if test -z "$(docker images -q {{ test_image }})"; then docker build --target test -t {{ test_image }} .; fi
    docker run --rm -v "$(pwd):/home/dev" {{ test_image }} bash ./scripts/run_practice_tests_docker.sh --test "{{ TEST_CASE }}"

test-runway-save-states:
    if test -z "$(docker images -q {{ test_image }})"; then docker build --target test -t {{ test_image }} .; fi
    docker run --rm -v "$(pwd):/home/dev" {{ test_image }} bash ./scripts/run_practice_tests_docker.sh --test-runway-save-states

test-replay-save-states REGION REPLAY_DIR:
    #!/usr/bin/env bash
    set -euo pipefail
    region="$(printf '%s' "{{ REGION }}" | tr '[:lower:]' '[:upper:]')"
    case "$region" in
        US|EU|JP) ;;
        *) echo "error: region must be one of: us, eu, jp" >&2; exit 2 ;;
    esac
    replay_dir="$(cd "{{ REPLAY_DIR }}" && pwd)"
    if test -z "$(docker images -q {{ test_image }})"; then docker build --target test -t {{ test_image }} .; fi
    docker run --rm -e REPLAY_SAVE_STATE_FILTER -e REPLAY_SAVE_STATE_EXCLUDE -e REPLAY_SAVE_STATE_SKIP_BUILD -e REPLAY_SAVE_STATE_SPACING_SECONDS -e REPLAY_SAVE_STATE_WAIT_FRAMES -e REPLAY_SAVE_STATE_DURATION_SECONDS -e REPLAY_SAVE_STATE_END_MARGIN_FRAMES -e REPLAY_SAVE_STATE_GRENADE_CAM -e REPLAY_SAVE_STATE_HOSTAGE_CAM -e REPLAY_SAVE_STATE_RESTART_BETWEEN_LOADS -e REPLAY_SAVE_STATE_JOBS -v "$(pwd):/home/dev" -v "$replay_dir:/replays:ro" {{ test_image }} bash ./scripts/run_practice_tests_docker.sh --test-replay-save-states /replays "$region"

# Run the complete regional save-state replay matrix, one step, or one replay.
test-save-state-replay-suite STEP="all" REPLAY="" REGION="us":
    bash ./scripts/run_full_save_state_replay_suite.sh "{{ STEP }}" "{{ REPLAY }}" "{{ REGION }}"

test-save-state-replay-suite-step STEP REPLAY="" REGION="us":
    just test-save-state-replay-suite "{{ STEP }}" "{{ REPLAY }}" "{{ REGION }}"

test-save-state-replay-suite-replay REPLAY REGION="us":
    just test-save-state-replay-suite all "{{ REPLAY }}" "{{ REGION }}"

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
    cp tests/replays/us/11-archives.ram build/u/ge007.u.ram
    ARES_N64_PROFILE_REPLAY=1 just profile-ares build/u/ge007.u.z64 build/u/ge007.u.elf "{{ OUTPUT }}"
    just profile-ares-flamegraph "$(python3 scripts/performance/select_profile_capture.py "{{ OUTPUT }}").folded" "{{ OUTPUT }}.html"
    docker run --rm -v "$(pwd):/home/dev" {{ image }} python3 scripts/performance/profile_summary.py --elf build/u/ge007.u.elf --csv "$(python3 scripts/performance/select_profile_capture.py "{{ OUTPUT }}")-game-frames.csv" --phase "{{ LABEL }}" --build-mode release --region US --baseline src/practice/docs/performance_baselines.json --output "{{ OUTPUT }}-performance.json"

# Render one profiler .folded capture as an interactive HTML flame graph.
profile-ares-flamegraph INPUT OUTPUT="":
    python3 ares/tools/n64-profiler-flamegraph.py "{{ INPUT }}" {{ if OUTPUT == "" { "" } else { '"' + OUTPUT + '"' } }}

test-all JOBS="":
    if test -z "$(docker images -q {{ test_image }})"; then docker build --target test -t {{ test_image }} .; fi
    docker run --rm -v "$(pwd):/home/dev" -e PRACTICE_TEST_JOBS="{{ JOBS }}" {{ test_image }} bash ./scripts/run_practice_tests_docker.sh --build-mode release

# Show or run one deterministic, duration-balanced full-suite shard.
test-shard-plan SHARDS="12":
    python3 scripts/run_test_shard.py --shard "1/{{ SHARDS }}" --list

test-shard SHARD: build-ares
    if test -z "$(docker images -q {{ image }})"; then just setup; fi
    for version in US EU JP; do docker run --rm -v "$(pwd):/home/dev" {{ image }} make -j{{ num_cpus() }} VERSION="$version" DEV=0 COMPARE=0 PRACTICE_TEST_ROM=1 TEST_CASE=; done
    ARES="{{ ares_bin }}" ARES_ARGS="--setting Audio/Driver=None --setting Input/Driver=None" python3 scripts/run_test_shard.py --shard "{{ SHARD }}"

# Run every regional SRAM replay against a release practice ROM using ares'
# host-side state comparison. Known practice-ROM divergences remain failures.
test-practice-replays REGION="us" ARTIFACTS="build/practice-replay-results" REPLAY="": build-ares
    #!/usr/bin/env bash
    set -euo pipefail
    region="$(printf '%s' "{{ REGION }}" | tr '[:upper:]' '[:lower:]')"
    case "$region" in
        us) version="US"; outcode="u" ;;
        eu) version="EU"; outcode="e" ;;
        jp) version="JP"; outcode="j" ;;
        *) echo "error: region must be one of: us, eu, jp" >&2; exit 2 ;;
    esac
    if test -z "$(docker images -q {{ test_image }})"; then docker build --target test -t {{ test_image }} .; fi
    docker run --rm -v "$(pwd):/home/dev" -w /home/dev {{ test_image }} make -j{{ num_cpus() }} DEV=0 VERSION="$version" COMPARE=0 PRACTICE_TEST_ROM=1 TEST_CASE=
    python3 scripts/patch_practice_rom.py "build/$outcode/ge007.$outcode.z64" --flags 1
    run_replays() {
        python3 ares/tests/n64-replay/run.py \
            --ares "{{ ares_bin }}" \
            --rom "build/$outcode/ge007.$outcode.z64" \
            --elf "build/$outcode/ge007.$outcode.elf" \
            --fixture-dir tests/replays \
            --region "$region" \
            --jobs 3 \
            --artifacts "{{ ARTIFACTS }}/$region" \
            "$@"
    }
    if [ -n "{{ REPLAY }}" ]; then
        run_replays --replay "{{ REPLAY }}"
    else
        run_replays
    fi

sc64-dev BOOT_LEVEL="TITLE":
    docker run --rm -v $(pwd):/home/dev {{ image }} make -j{{ num_cpus() }} DEV=1 PRACTICE_TEST_ROM=1 BOOT_LEVEL={{ BOOT_LEVEL }}
    sc64deployer upload --reboot build/u/ge007.u.z64
    sc64deployer debug

# Build a release-optimized ROM containing the test suite and SC64 reboot hook.
sc64-test-build:
    docker run --rm -v $(pwd):/home/dev {{ image }} make -j{{ num_cpus() }} DEV=0 COMPARE=0 PRACTICE_TEST_ROM=1 TEST_CASE=

# Run one test, or the ordinary test suite, on a connected SummerCart64.
sc64-test TEST_CASE="" INITIAL="false": sc64-test-build
    #!/usr/bin/env bash
    set -euo pipefail
    args=()
    if [ -n "{{ TEST_CASE }}" ]; then args+=(--test "{{ TEST_CASE }}"); fi
    if [ "{{ INITIAL }}" = "true" ]; then args+=(--initial-upload); fi
    python3 scripts/run_sc64_tests.py "${args[@]}"

# Run the US replay save-state matrix on a connected SummerCart64/N64.
sc64-save-state-suite STEP="all" REPLAY="" INITIAL="false": sc64-test-build
    #!/usr/bin/env bash
    set -euo pipefail
    args=(--step "{{ STEP }}")
    if [ -n "{{ REPLAY }}" ]; then args+=(--replay "{{ REPLAY }}"); fi
    if [ "{{ INITIAL }}" = "true" ]; then args+=(--initial-upload); fi
    python3 scripts/run_sc64_save_state_tests.py "${args[@]}"

# builds the rom and uploads it to a connected summercart64
sc64: make
    sc64deployer sd mkdir /CUSTOM || true
    sc64deployer sd upload build/u/ge007.u.z64 /CUSTOM/practice-rom_latest_ntsc_u.z64
    if [ -f build/e/ge007.e.z64 ]; then sc64deployer sd upload build/e/ge007.e.z64 /CUSTOM/practice-rom_latest_pal.z64; fi
    if [ -f build/j/ge007.j.z64 ]; then sc64deployer sd upload build/j/ge007.j.z64 /CUSTOM/practice-rom_latest_ntsc_j.z64; fi
