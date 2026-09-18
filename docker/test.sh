#!/usr/bin/env bash
# Run the ct test suite in each test image. The source tree is mounted
# read-only at /src; builds happen in container-local /build.
#
# Usage: test.sh [image ...]
#   e.g. test.sh                    -> all built ct-test images, default compilers
#        test.sh ubuntu24.04        -> one image
# COMPILERS can restrict the toolchains, e.g. COMPILERS="g++-13" test.sh cuda12
set -uo pipefail
cd "$(dirname "$0")"

CT_SRC=${CT_SRC:-$(cd .. && pwd)}

IMAGES=("$@")
if [ ${#IMAGES[@]} -eq 0 ]; then
    IMAGES=($(docker images --format '{{.Tag}}' ct-test | sort -r))
    [ ${#IMAGES[@]} -eq 0 ] && { echo "no ct-test images found; run build.sh first" >&2; exit 1; }
fi

overall=0
for img in "${IMAGES[@]}"; do
    echo
    echo "#################### $img ####################"
    docker run --rm \
        -v "$CT_SRC:/src:ro" \
        -e COMPILERS \
        "ct-test:$img" || overall=1
done
exit $overall
