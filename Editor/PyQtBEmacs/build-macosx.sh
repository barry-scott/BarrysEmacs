#!/bin/bash
echo "Info: build-macosx.sh Editor/PythonBEmacs - start"
PKG_DIST_DIR=${BUILDER_TOP_DIR}/Kits/MacOSX/pkg

if [ "$1" = "--package" ]
then
    DIST_DIR=${PKG_DIST_DIR}
else
    DIST_DIR=dist
fi

rm -rf build ${DIST_DIR}

mkdir -p ${DIST_DIR}
${PYTHON} make_be_images.py
export PYTHONPATH=${BUILDER_TOP_DIR}/Editor/obj-pybemacs:$HOME/wc/hg/macholib
${PYTHON} setup-macosx.py py2app --dist-dir ${DIST_DIR} --no-strip 2>&1 | tee a.log

mkdir -p "${DIST_DIR}/Barry's Emacs-Devel.app/Contents/Resources/emacs_library"
mkdir -p "${DIST_DIR}/Barry's Emacs-Devel.app/Contents/Resources/documentation"

if [ "$1" != "--package" ]
then
    cp \
        "${PKG_DIST_DIR}/Barry's Emacs-Devel.app/Contents/Resources/emacs_library"/* \
        "${DIST_DIR}/Barry's Emacs-Devel.app/Contents/Resources/emacs_library"
    cp \
        "${PKG_DIST_DIR}/Barry's Emacs-Devel.app/Contents/Resources/documentation"/* \
        "${DIST_DIR}/Barry's Emacs-Devel.app/Contents/Resources/documentation"
fi
echo "Info: build-macosx.sh Editor/PythonBEmacs - end"
