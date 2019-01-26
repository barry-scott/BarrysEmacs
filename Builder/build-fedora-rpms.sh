#!/bin/bash
set -e

make -f linux.mak brand

cd ${BUILDER_TOP_DIR}/Kits/Linux

chmod +x build-fedora-rpms.sh
./build-fedora-rpms.sh "$@"
