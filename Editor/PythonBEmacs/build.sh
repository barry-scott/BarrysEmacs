#!/bin/sh
PKG_DIST_DIR=${BUILDER_TOP_DIR}/Kits/MacOSX/pkg

if [ "$1" = "--package" ]
then
    DIST_DIR=${PKG_DIST_DIR}
else
    DIST_DIR=dist
fi

rm -rf build ${DIST_DIR}

mkdir ${DIST_DIR}
export PYTHONPATH=${BUILDER_TOP_DIR}/Editor/darwin/obj-pybemacs
python2.6 setup-macosx.py py2app --dist-dir  ${DIST_DIR} --no-strip 2>&1 | tee a.log

mkdir ${DIST_DIR}/be_main.app/Contents/Resources/emacs_library
mkdir ${DIST_DIR}/be_main.app/Contents/Resources/documentation

if [ "$1" != "--package" ]
then
    cp \
        ${PKG_DIST_DIR}/be_main.app/Contents/Resources/emacs_library/* \
        ${DIST_DIR}/be_main.app/Contents/Resources/emacs_library
    cp \
        ${PKG_DIST_DIR}/be_main.app/Contents/Resources/documentation/* \
        ${DIST_DIR}/be_main.app/Contents/Resources/documentation
fi
