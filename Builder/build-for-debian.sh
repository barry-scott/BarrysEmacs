#!/bin/bash
release=$(( $(<debian_release.txt) + 1 ))
echo $release >debian_release.txt

python3 ./package_bemacs.py debian-source \
    --release=${release} \
    --system-hunspell \
    --system-ucd \
    --colour
