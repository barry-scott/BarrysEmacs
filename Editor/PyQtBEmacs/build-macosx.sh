#!/bin/bash
set -e

printf "\033[32mInfo:\033[m build-macosx.sh Editor/PyQtBEmacs - start\n"
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

printf "\033[32mInfo:\033[m Creating Application bundle - logging details to ${SRC_DIR}/build-macsosx.log\\n"
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
    Resources/lib/python${PYTHON_VERSION}/PyQt5/Qt?*.so \
    Resources/lib/python${PYTHON_VERSION}/PyQt5/tmp

# Resources/lib/python3.5/PyQt5/Qt/lib - QtXxx.framework
mkdir \
    Resources/lib/python${PYTHON_VERSION}/PyQt5/Qt/lib/tmp
mv \
    Resources/lib/python${PYTHON_VERSION}/PyQt5/Qt/lib/Qt*.framework \
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
    printf "\033[32mInfo:\033[m Framework used ${LIBNAME}\n"
    mv \
            Resources/lib/python${PYTHON_VERSION}/PyQt5/tmp/${LIBNAME}.so \
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
printf "\033[32mInfo:\033[m build-macosx.sh Editor/PyQtBEmacs - end\n"
