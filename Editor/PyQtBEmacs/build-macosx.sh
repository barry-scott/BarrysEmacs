#!/bin/bash
set -e

if ! which colour-print >/dev/null
then
    function colour-print {
        echo "$@"
    }
fi

colour-print "<>info Info:<> build-macosx.sh Editor/PyQtBEmacs - start"
PKG_DIST_DIR=${BUILDER_TOP_DIR}/Builder/tmp/pkg

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

colour-print "<>info Info:<> Creating Application bundle - logging details to ${SRC_DIR}/build-macsosx.log"
export PYTHONPATH=${BUILDER_TOP_DIR}/Editor/exe-pybemacs
${PYTHON} setup-macosx.py py2app --dist-dir ${DIST_DIR} --no-strip 1>${SRC_DIR}/build-macosx.log 2>&1
pushd "${DIST_DIR}/Barry's Emacs-Devel.app/Contents" >/dev/null

mkdir -p "Resources/emacs_library"
mkdir -p "Resources/documentation"
mkdir -p "Resources/bin"

${PYTHON} "${SRC_DIR}/create_bemacs_client.py" "${SRC_DIR}" "Resources/bin/bemacs_client"

# fixup 3. only keep the frameworks that we need, saving space
# Resources/lib/python3.5/lib-dynload/PyQt5 - QtXxx.so
mkdir \
    Resources/lib/python${PYTHON_VERSION}/PyQt5/tmp
mv \
    Resources/lib/python${PYTHON_VERSION}/PyQt5/Qt[A-Z]*.so \
    Resources/lib/python${PYTHON_VERSION}/PyQt5/tmp

# Resources/lib/python3.5/PyQt5/Qt/lib - QtXxx.framework
mkdir \
    Resources/lib/python${PYTHON_VERSION}/PyQt5/Qt/lib/tmp
mv \
    Resources/lib/python${PYTHON_VERSION}/PyQt5/Qt/lib/Qt[A-Z]*.framework \
    Resources/lib/python${PYTHON_VERSION}/PyQt5/Qt/lib/tmp

for LIBNAME in \
    QtCore \
    QtDBus \
    QtGui \
    QtPrintSupport \
    QtSvg \
    QtWidgets \
    ;
do
    colour-print "<>info Info:<> Framework used ${LIBNAME}"
    mv \
        Resources/lib/python${PYTHON_VERSION}/PyQt5/tmp/${LIBNAME}.abi3.so \
        Resources/lib/python${PYTHON_VERSION}/PyQt5
    mv \
        Resources/lib/python${PYTHON_VERSION}/PyQt5/Qt/lib/tmp/${LIBNAME}.framework \
        Resources/lib/python${PYTHON_VERSION}/PyQt5/Qt/lib
done

# fixup 4. remove the unused frameworks
rm -rf Resources/lib/python${PYTHON_VERSION}/PyQt5/tmp
rm -rf Resources/lib/python${PYTHON_VERSION}/PyQt5/Qt/lib/tmp

# remove qml stuff
rm -rf Resources/lib/python${PYTHON_VERSION}/PyQt5/Qt/qml
rm -rf Resources/lib/python${PYTHON_VERSION}/PyQt5/Qt/translations
rm -rf Resources/lib/python${PYTHON_VERSION}/PyQt5/Qt/qsci

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
colour-print "<>info Info:<> build-macosx.sh Editor/PyQtBEmacs - end"
