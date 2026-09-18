# ct test image: CUDA 12.5 on Ubuntu 24.04
#   nvcc 12.5 with GCC 13 (distro default — exercises the
#   CUDA_HOST_COMPILER fallback in ct/CMakeLists.txt, which then uses the
#   installed GCC 12) / GCC 12, Clang 18, Boost 1.83, Python 3.12.
#
# The devel base image ships an apt repository for CUDA; it is removed
# below because apt would otherwise see it (and any newer CUDA toolkits it
# publishes later) and can pull cuda-toolkit-*-config-common packages that
# repoint /usr/local/cuda to a toolkit version that is not actually
# installed. The full 12.5 toolkit is already present in the image.
FROM nvidia/cuda:12.5.1-devel-ubuntu24.04

ENV DEBIAN_FRONTEND=noninteractive

# The Boost.Python dev package name is distro-specific; resolve it by search.
RUN rm -f /etc/apt/sources.list.d/cuda.list \
    && apt-get update \
    && apt-get install -y --no-install-recommends \
        cmake \
        g++-12 \
        clang \
        python3 python3-dev python3-venv \
        libboost-program-options-dev \
    && BOOST_PY_DEV=$(apt-cache search '^libboost-python[0-9].*-dev$' | awk '{print $1}' | sort -V | tail -1) \
    && echo "installing $BOOST_PY_DEV" \
    && apt-get install -y --no-install-recommends "$BOOST_PY_DEV" \
    && rm -rf /var/lib/apt/lists/*

# Pin /usr/local/cuda to the installed toolkit so FindCUDA finds nvcc no
# matter what the alternatives symlinks say.
RUN ln -sfn /usr/local/cuda-12.5 /usr/local/cuda \
    && /usr/local/cuda/bin/nvcc --version

# pytest + numpy for the boost.python interop tests
RUN python3 -m venv /venv \
    && /venv/bin/pip install -q --no-cache-dir pytest numpy

COPY run_tests.sh /opt/ct-test/run_tests.sh
RUN chmod +x /opt/ct-test/run_tests.sh

ENTRYPOINT ["/opt/ct-test/run_tests.sh"]
