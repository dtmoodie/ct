# ct platform test images

Small, standalone Docker images that configure, build, and run the full ct
test suite (googletest + boost.python interop via ctest/pytest) across a
variety of distros, compilers, and Boost versions. The library source is
mounted read-only at runtime, so the images stay small and always test the
current tree.

## Images

| Tag | Base | Compilers tested | Boost | Python | CMake |
|---|---|---|---|---|---|
| `ct-test:ubuntu20.04` | ubuntu:20.04 | GCC 9, 10; Clang 10 | 1.71 | 3.8 | 3.16 |
| `ct-test:ubuntu22.04` | ubuntu:22.04 | GCC 11, 12; Clang 14 | 1.74 | 3.10 | 3.22 |
| `ct-test:ubuntu24.04` | ubuntu:24.04 | GCC 13, 12; Clang 18 | 1.83 | 3.12 | 3.28 |
| `ct-test:cuda12` | nvidia/cuda:12.5.1-devel-ubuntu24.04 | GCC 13 (default; exercises the `CUDA_HOST_COMPILER` fallback to GCC 12), 12; Clang 18 | 1.83 | 3.12 | 3.28 |

CUDA conformance tests run only in `cuda12`; everywhere else `find_package(CUDA)`
fails quietly and they are skipped. OpenCV/cereal/cpgf are intentionally not
installed — those optional test variants are skipped.

## Usage

```bash
./build.sh                  # build all four images
./build.sh cuda12           # or just one

./test.sh                   # run the suite in every built image
./test.sh ubuntu24.04       # one image
COMPILERS="g++-13" ./test.sh cuda12   # restrict toolchains
```

Each image auto-detects installed compilers (`g++`, `g++-12`, `g++-13`,
`clang++`, ...) and runs configure+build+ctest once per compiler; the image
exits non-zero if any compiler fails, so `test.sh` is CI-friendly.
