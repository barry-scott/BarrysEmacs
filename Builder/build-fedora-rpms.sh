#!/bin/bash
set -e

echo "Info: Branding file..."

make -f linux.mak brand 1>/dev/null

cd ${BUILDER_TOP_DIR}/Kits/Linux

./build_fedora_rpms.py "$@"
