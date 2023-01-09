#!/bin/bash
CMD=${1:-debian-test-build}

. /etc/os-release
TARGET=/shared/Downloads/BEmacs/beta/${VERSION_CODENAME}

python3 ./package_bemacs.py ${CMD} \
    --debian-repos=${TARGET} \
    --colour
