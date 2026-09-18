# ct test image: Ubuntu 24.04
#   GCC 13 (default) / 12, Clang 18, Boost 1.83, Python 3.12, CMake 3.28
# The "newest" edge: Boost 1.83's refactored boost.python init API and a
# stricter toolchain.
FROM ubuntu:24.04

ENV DEBIAN_FRONTEND=noninteractive

# The Boost.Python dev package name is distro-specific (libboost-python1.83*
# on noble); resolve it by search instead of hard-coding.
RUN apt-get update \
    && apt-get install -y --no-install-recommends \
        build-essential \
        cmake \
        g++-12 \
        clang \
        python3 python3-dev python3-venv \
        libboost-program-options-dev \
    && BOOST_PY_DEV=$(apt-cache search '^libboost-python[0-9].*-dev$' | awk '{print $1}' | sort -V | tail -1) \
    && echo "installing $BOOST_PY_DEV" \
    && apt-get install -y --no-install-recommends "$BOOST_PY_DEV" \
    && rm -rf /var/lib/apt/lists/*

# pytest + numpy for the boost.python interop tests (PEP-668-safe venv)
RUN python3 -m venv /venv \
    && /venv/bin/pip install -q --no-cache-dir pytest numpy

COPY run_tests.sh /opt/ct-test/run_tests.sh
RUN chmod +x /opt/ct-test/run_tests.sh

ENTRYPOINT ["/opt/ct-test/run_tests.sh"]
