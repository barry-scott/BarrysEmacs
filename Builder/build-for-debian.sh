#!/bin/bash
CMD=${1:-debian-test-build}
release=$(( $(<debian_release.txt) + 1 ))
echo $release >debian_release.txt

python3 ./package_bemacs.py ${CMD} \
    --release=${release} \
    --colour
