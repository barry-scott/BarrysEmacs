#!/bin/bash
set -e
echo "Info: build-macosx.sh Editor/PyQtBEmacs - start"
PKG_DIST_DIR=${BUILDER_TOP_DIR}/Kits/MacOSX/pkg

SRC_DIR=${PWD}

if [ "$1" = "--package" ]
then
    DIST_DIR=${PKG_DIST_DIR}
else
    DIST_DIR=dist
fi

PY_VER=$( ${PYTHON} -c 'import sys;print( "%d.%d" % (sys.version_info.major, sys.version_info.minor) )' )

rm -rf build ${DIST_DIR}

mkdir -p ${DIST_DIR}
${PYTHON} make_be_images.py
export PYTHONPATH=${BUILDER_TOP_DIR}/Editor/exe-pybemacs:$HOME/wc/hg/macholib
${PYTHON} setup-macosx.py py2app --dist-dir ${DIST_DIR} --no-strip 2>&1 | tee a.log

set -x
pushd "${DIST_DIR}/Barry's Emacs-Devel.app/Contents" >/dev/null

${PYTHON} ${SRC_DIR}/build_fix_install_rpath.py fix Resources/lib/python${PY_VER}/lib-dynload/PyQt5/*.so

for LIBNAME in \
    QtCore \
    QtDBus \
    QtGui \
    QtPrintSupport \
    QtSvg \
    QtWidgets \
    ;
do
    echo "Info: Copy framework ${LIBNAME}"
    cp -R \
        "${BUILDER_QTDIR}/clang_64/lib/${LIBNAME}.framework" \
        "Frameworks"

    ${PYTHON} ${SRC_DIR}/build_fix_install_rpath.py show "Frameworks/${LIBNAME}.framework/${LIBNAME}"
done

echo "Info: remove Headers links"
find "Frameworks" -type l -name 'Headers' -exec rm -f {} ';'
echo "Info: remove Headers dirs"
find -d "Frameworks" -type d -name 'Headers' -exec rm -rf {} ';'

for PLUGIN in \
    imageformats/libqdds.dylib \
    imageformats/libqgif.dylib \
    imageformats/libqicns.dylib \
    imageformats/libqico.dylib \
    imageformats/libqjpeg.dylib \
    imageformats/libqsvg.dylib \
    imageformats/libqtga.dylib \
    imageformats/libqtiff.dylib \
    imageformats/libqwbmp.dylib \
    imageformats/libqwebp.dylib \
    platforms/libqcocoa.dylib \
    ;
do
    echo "Info: Copy plugin ${PLUGIN}"
    TARGET_DIR=$( dirname "Resources/plugins/${PLUGIN}" )
    mkdir -p "${TARGET_DIR}"
    cp \
        "${BUILDER_QTDIR}/clang_64/plugins/${PLUGIN}" \
        "${TARGET_DIR}"

    ${PYTHON} ${SRC_DIR}/build_fix_install_rpath.py fix "Resources/plugins/${PLUGIN}"
done

mkdir -p "Resources/emacs_library"
mkdir -p "Resources/documentation"

if [ "$1" != "--package" ]
then
    cp \
        "${PKG_DIST_DIR}/Barry's Emacs-Devel.app/Contents/Resources/emacs_library"/* \
        "Resources/emacs_library"
    cp \
        "${PKG_DIST_DIR}/Barry's Emacs-Devel.app/Contents/Resources/documentation"/* \
        "Resources/documentation"
fi
popd >/dev/null
echo "Info: build-macosx.sh Editor/PyQtBEmacs - end"
