#!/bin/sh
# Shared runner for an NW library's host-side harness. Called from the
# library's extras/test/run.sh with the binary name; compiles test_output.cpp
# in the caller's directory against the NW_Core stubs and sources, runs it,
# and diffs output.txt against baseline.txt.
# Usage: run_library.sh <binary>            (test)
#        run_library.sh <binary> --record   (rewrite baseline.txt)
NW_CORE="${NW_CORE:-../../../NW_Core}"
BIN="$1"; [ -n "$BIN" ] || { echo "usage: run_library.sh <binary> [--record]"; exit 2; }
g++ -std=c++17 -Wall -Wno-unused-function -I"$NW_CORE/extras/test" -I"$NW_CORE/src" -o "$BIN" test_output.cpp "$NW_CORE/src/NW_Device.cpp" || exit 1
./"$BIN" > output.txt || exit 1
if [ "$2" = "--record" ]; then cp output.txt baseline.txt; echo "baseline recorded"; exit 0; fi
if diff -u baseline.txt output.txt; then echo "OK: output identical to baseline"; else echo "FAIL: output differs from baseline"; exit 1; fi
