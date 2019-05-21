#!/bin/bash
set -e

printf "\033[32mInfo:\033[m Branding file...\n"
${PYTHON} brand_version.py version_info.txt ${BUILDER_TOP_DIR} >/dev/null

cd ${BUILDER_TOP_DIR}/Kits/Linux

./build_fedora_rpms.py "$@"
