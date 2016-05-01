#!/bin/bash
set -e

make -f linux.mak brand

cd ${BUILDER_TOP_DIR}/Kits/Linux

chmod +x make-devel-src-rpm.sh
./make-devel-src-rpm.sh "$@"
