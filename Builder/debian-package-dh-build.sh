#!/bin/bash
#
#   script called from debian/rules make file
#
set -x
set -e
export DESTDIR="${1:? arg 1 must be DESTDIR}"
shift
export PYQT_VERSION="${1:? arg 2 must be the PyQt version}"
shift

export BUILDER_TOP_DIR=$(pwd)
export PYTHON=/usr/bin/python3

echo "Info: debian bemacs builder BUILDER_TOP_DIR ${BUILDER_TOP_DIR} DESTDIR ${DESTDIR} PYTHON ${PYTHON}"

cd Builder

${PYTHON} ./build_bemacs.py gui \
    --pyqt-version=${PYQT_VERSION} \
    --no-warnings-as-errors \
    --system-hunspell \
    --system-pycxx \
    --system-sqlite

find "$DESTDIR" -name __pycache__ -prune -exec rm -rfv {} ';'

for page in bemacs.1 bemacs-cli.1 bemacs_server.1
do
    mkdir -p "${DESTDIR}/usr/share/man/man1"
    gzip -c "${BUILDER_TOP_DIR}/Kits/Linux/${page}" > "${DESTDIR}/usr/share/man/man1/${page}.gz"
done

mkdir -p "${DESTDIR}/usr/share/bemacs"
cp "${BUILDER_TOP_DIR}/Editor/PyQt6/org.barrys-emacs.editor.png" "${DESTDIR}/usr/share/bemacs/org.barrys-emacs.editor.png"
mkdir -p "${DESTDIR}/usr/share/applications"
cp "${BUILDER_TOP_DIR}/Kits/Linux/org.barrys-emacs.editor.desktop" "${DESTDIR}/usr/share/applications/org.barrys-emacs.editor.desktop"
