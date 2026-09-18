#!/usr/bin/env bash
# Build the ct-test images. Usage: build.sh [image ...]
#   e.g. build.sh            -> all four
#        build.sh cuda12     -> just the nvidia/cuda:12.5.1-devel-ubuntu24.04-based one
set -euo pipefail
cd "$(dirname "$0")"

IMAGES=("$@")
[ ${#IMAGES[@]} -eq 0 ] && IMAGES=(ubuntu20.04 ubuntu22.04 ubuntu24.04 cuda12)

for img in "${IMAGES[@]}"; do
    echo "=== building ct-test:$img ==="
    docker build -t "ct-test:$img" -f "$img/Dockerfile" .
done
echo
echo "done: $(docker images --format '{{.Repository}}:{{.Tag}}' | grep '^ct-test:' | tr '\n' ' ')"
