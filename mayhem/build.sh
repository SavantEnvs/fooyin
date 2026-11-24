#!/usr/bin/env bash
# mayhem/build.sh — builds fooyin's fuzz harnesses (sanitized) and a subset of upstream's own GTest
# suite (normal flags) for mayhem/test.sh. Runs as `mayhem` in /mayhem; offline-safe (all deps are
# apt packages baked into the image; system QCoro/KDSingleApplication so no FetchContent runs).
set -euo pipefail
[ -n "${SOURCE_DATE_EPOCH:-}" ] || unset SOURCE_DATE_EPOCH
: "${SANITIZER_FLAGS=-fsanitize=address,undefined -fno-sanitize-recover=all -fno-omit-frame-pointer}"
: "${DEBUG_FLAGS:=-g -gdwarf-3}"
: "${CC:=clang}" ; : "${CXX:=clang++}" ; : "${LIB_FUZZING_ENGINE:=-fsanitize=fuzzer}"
: "${MAYHEM_JOBS:=$(nproc)}"
: "${COVERAGE_FLAGS=}"
export SANITIZER_FLAGS DEBUG_FLAGS CC CXX LIB_FUZZING_ENGINE MAYHEM_JOBS COVERAGE_FLAGS
: "${SRC:=/mayhem}"
cd "$SRC"

common_opts=(-G Ninja -DBUILD_SHARED_LIBS=OFF -DBUILD_TRANSLATIONS=OFF -DBUILD_CCACHE=OFF -DBUILD_ALSA=OFF
             -DBUILD_PCH=OFF -DFETCHCONTENT_FULLY_DISCONNECTED=ON)

# 1) Fuzz build: the whole fooyin core (and utils) instrumented with ASan+UBSan + SanCov, DWARF 3.
#    mayhem/lsan_off.cc (LSan off at build time) is linked into every fuzz + standalone binary.
#    build-* directories are gitignored upstream (/build*/), so `git clean -ffdX` removes them.
FLAGS="$SANITIZER_FLAGS -fsanitize=fuzzer-no-link $DEBUG_FLAGS"
cmake -S . -B build-fuzz "${common_opts[@]}" -DCMAKE_BUILD_TYPE=Debug -DBUILD_PLUGINS=OFF -DBUILD_TESTING=OFF \
    -DCMAKE_C_FLAGS="$FLAGS" -DCMAKE_CXX_FLAGS="$FLAGS" \
    -DCMAKE_PROJECT_fooyin_INCLUDE="$SRC/mayhem/project-hook.cmake" -DFOOYIN_FUZZERS=ON \
    -DFOOYIN_FUZZ_OUT="$SRC/build-fuzz/fuzzers" -DFOOYIN_LSAN_OFF="$SRC/mayhem/lsan_off.cc"
cmake --build build-fuzz -j"$MAYHEM_JOBS" --target fooyin_fuzzers

# 2) Test build: upstream's GTest suites for the parsers the harnesses reach, with normal flags.
cmake -S . -B build-test "${common_opts[@]}" -DCMAKE_BUILD_TYPE= -DBUILD_TESTING=ON \
    -DPLUGIN_SELECTION=lyrics \
    -DCMAKE_C_FLAGS="$COVERAGE_FLAGS" -DCMAKE_CXX_FLAGS="$COVERAGE_FLAGS"
cmake --build build-test -j"$MAYHEM_JOBS" --target \
    test_cueparser test_m3uparser test_plsparser test_lyricsparser test_scriptparser test_tagreader test_accurateripcore
