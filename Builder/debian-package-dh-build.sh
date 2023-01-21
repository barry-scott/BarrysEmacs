#!/bin/bash
#
#   script called from debian/rules make file
#
export DESTDIR="$1"
export BUILDER_TOP_DIR=$(pwd)

# figure out the explicit python executable
#version=$( /usr/bin/python3 -c 'import sys;print("%d.%d" % (sys.version_info.major, sys.version_info.minor))' )
#export PYTHON=/usr/bin/python${version}

export PYTHON=/usr/bin/python3

echo "Info: debian bemacs builder BUILDER_TOP_DIR ${BUILDER_TOP_DIR} DESTDIR ${DESTDIR} PYTHON ${PYTHON}"

cd Builder

${PYTHON} ./build_bemacs.py gui \
    --no-warnings-as-errors \
    --system-hunspell \
    --system-pycxx \
    --system-sqlite

find "$DESTDIR" -name __pycache__ -exec rm -rf {} ';'

for page in bemacs.1 bemacs-cli.1 bemacs_server.1
do
    mkdir -p "${DESTDIR}/usr/share/man/man1"
    gzip -c "${BUILDER_TOP_DIR}/Kits/Linux/${page}" > "${DESTDIR}/usr/share/man/man1/${page}.gz"
done

mkdir -p "${DESTDIR}/usr/share/bemacs"
cp "${BUILDER_TOP_DIR}/Editor/PyQt6/org.barrys-emacs.editor.png" "${DESTDIR}/usr/share/bemacs/org.barrys-emacs.editor.png"
mkdir -p "${DESTDIR}/usr/share/applications"
cp "${BUILDER_TOP_DIR}/Kits/Linux/org.barrys-emacs.editor.desktop" "${DESTDIR}/usr/share/applications/org.barrys-emacs.editor.desktop"
