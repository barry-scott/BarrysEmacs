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
export PYTHONPATH=${BUILDER_TOP_DIR}/Editor/exe-pybemacs
${PYTHON} setup-macosx.py py2app --dist-dir ${DIST_DIR} --no-strip 2>&1 | tee a.log

set -x
pushd "${DIST_DIR}/Barry's Emacs-Devel.app/Contents" >/dev/null

mkdir -p "Resources/emacs_library"
mkdir -p "Resources/documentation"

# fixup 3. only keep the frameworks that we need, saving space
# Resources/lib/python3.5/lib-dynload/PyQt5 - QtXxx.so
mkdir \
    Resources/lib/python3.5/PyQt5/tmp
mv \
    Resources/lib/python3.5/PyQt5/Qt?*.so \
    Resources/lib/python3.5/PyQt5/tmp

# Resources/lib/python3.5/PyQt5/Qt/lib - QtXxx.framework
mkdir \
    Resources/lib/python3.5/PyQt5/Qt/lib/tmp
mv \
    Resources/lib/python3.5/PyQt5/Qt/lib/Qt*.framework \
    Resources/lib/python3.5/PyQt5/Qt/lib/tmp

for LIBNAME in \
    QtCore \
    QtDBus \
    QtGui \
    QtPrintSupport \
    QtSvg \
    QtWidgets \
    ;
do
    echo "Info: framework used ${LIBNAME}"
    mv \
            Resources/lib/python3.5/PyQt5/tmp/${LIBNAME}.so \
            Resources/lib/python3.5/PyQt5
    mv \
        Resources/lib/python3.5/PyQt5/Qt/lib/tmp/${LIBNAME}.framework \
        Resources/lib/python3.5/PyQt5/Qt/lib
done

# fixup 4. remove the unused frameworks
rm -rf Resources/lib/python3.5/PyQt5/tmp
rm -rf Resources/lib/python3.5/PyQt5/Qt/lib/tmp

# remove qml stuff
rm -rf Resources/lib/python3.5/PyQt5/Qt/qml
rm -rf Resources/lib/python3.5/PyQt5/Qt/translations
rm -rf Resources/lib/python3.5/PyQt5/Qt/qsci

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
