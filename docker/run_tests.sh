#!/usr/bin/env bash
# Configure, build and run the ct test suite once per installed compiler.
#
# Runs inside the ct-test images with the ct source tree mounted read-only
# at /src. Build output goes to /build/<compiler> (container-local, discarded
# on exit).
#
# Env:
#   SRCDIR   source tree (default /src)
#   BUILDDIR build root   (default /build)
#   COMPILERS  space-separated cxx drivers to test; default: auto-detect
#   PYTHON_EXECUTABLE  python with pytest+numpy (default /venv/bin/python)
set -uo pipefail

SRCDIR=${SRCDIR:-/src}
BUILDDIR=${BUILDDIR:-/build}
PYTHON_EXECUTABLE=${PYTHON_EXECUTABLE:-/venv/bin/python}

if [ -n "${COMPILERS:-}" ]; then
    read -r -a COMPILERS <<<"$COMPILERS"
else
    COMPILERS=()
    for c in g++ g++-9 g++-10 g++-12 g++-13 clang++ clang++-10 clang++-14 clang++-16 clang++-18; do
        command -v "$c" >/dev/null 2>&1 && COMPILERS+=("$c")
    done
fi

[ -x "$PYTHON_EXECUTABLE" ] || PYTHON_EXECUTABLE=""

overall=0
for cxx in "${COMPILERS[@]}"; do
    # "clang++" contains "g++", so derive clang first
    case "$cxx" in
        clang*) cc="${cxx/++/}" ;;
        *)      cc="${cxx/g++/gcc}" ;;
    esac
    b="$BUILDDIR/$(echo "$cxx" | tr '+/-' '___')"
    echo
    echo "############## compiler: $cxx (cc: $cc) ##############"
    if ! cmake -S "$SRCDIR" -B "$b" \
        -DCMAKE_C_COMPILER="$cc" \
        -DCMAKE_CXX_COMPILER="$cxx" \
        -DCMAKE_BUILD_TYPE=Release \
        ${PYTHON_EXECUTABLE:+-DPython_EXECUTABLE="$PYTHON_EXECUTABLE"}; then
        echo "!!!! configure failed for $cxx"
        overall=1
        continue
    fi
    if ! make -C "$b" -j"$(nproc)"; then
        echo "!!!! build failed for $cxx"
        overall=1
        continue
    fi
    (
        cd "$b" && ctest --output-on-failure
    ) || {
        echo "!!!! tests failed for $cxx"
        overall=1
    }
done
exit $overall
