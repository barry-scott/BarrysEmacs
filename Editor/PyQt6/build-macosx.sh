#!/bin/bash
set -e

if ! which colour-print >/dev/null
then
    function colour-print {
        echo "$@"
    }
fi

colour-print "<>info Info:<> build-macosx.sh in Editor/PyQtBEmacs - start $*"
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

# true = pyinstall, false = py2app
if true
then
    # true = use bemacs-pyinstall.spec, false = make a new one
    if true
    then
        ${BUILDER_TOP_DIR}/Builder/venv.tmp/bin/pyinstaller \
            --distpath ${DIST_DIR} \
                bemacs-pyinstaller.spec
    else
        ${BUILDER_TOP_DIR}/Builder/venv.tmp/bin/pyinstaller \
            --log-level INFO \
            --distpath ${DIST_DIR} \
            --name "Barry\\'s Emacs" \
            --paths ${PYTHONPATH} \
            --windowed \
            --icon bemacs.icns \
            --target-arch universal2
            --osx-bundle-identifier org.barrys-emacs.bemacs \
                be_main.py
    fi

    pushd "${DIST_DIR}/Barry's Emacs.app/Contents" >/dev/null

    mkdir -p "Resources/emacs_library"
    mkdir -p "Resources/documentation"
    mkdir -p "Resources/bin"

    ${PYTHON} "${SRC_DIR}/create_bemacs_client.py" "${SRC_DIR}" "Resources/bin/bemacs"

    # remove translations stuff
    rm MacOS/PyQt6/Qt6/translations
    rm -r Resources/PyQt6/Qt6/translations

    # 5. remove .sip files
    find Resources -name '*.sip' -delete

    popd >/dev/null

    codesign -s - --force --all-architectures --timestamp --deep \
        "/Users/barry/Projects/BarrysEmacs/Builder/tmp/pkg/Barry's Emacs.app"

else
    ${PYTHON} py2app-setup-macosx.py py2app --dist-dir ${DIST_DIR} --no-strip 1>${SRC_DIR}/build-macosx.log 2>&1
    pushd "${DIST_DIR}/Barry's Emacs-Devel.app/Contents" >/dev/null

    mkdir -p "Resources/emacs_library"
    mkdir -p "Resources/documentation"
    mkdir -p "Resources/bin"

    ${PYTHON} "${SRC_DIR}/create_bemacs_client.py" "${SRC_DIR}" "Resources/bin/bemacs_client"

    # fixup 3. only keep the frameworks that we need, saving space
    # Resources/lib/python3.5/lib-dynload/PyQt6 - QtXxx.so
    mkdir \
        Resources/lib/python${PYTHON_VERSION}/PyQt6/tmp
    mv \
        Resources/lib/python${PYTHON_VERSION}/PyQt6/Qt[A-Z]*.so \
        Resources/lib/python${PYTHON_VERSION}/PyQt6/tmp

    # Resources/lib/python3.5/PyQt6/Qt/lib - QtXxx.framework
    mkdir \
        Resources/lib/python${PYTHON_VERSION}/PyQt6/Qt6/lib/tmp
    mv \
        Resources/lib/python${PYTHON_VERSION}/PyQt6/Qt6/lib/Qt[A-Z]*.framework \
        Resources/lib/python${PYTHON_VERSION}/PyQt6/Qt6/lib/tmp

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
            Resources/lib/python${PYTHON_VERSION}/PyQt6/tmp/${LIBNAME}.abi3.so \
            Resources/lib/python${PYTHON_VERSION}/PyQt6
        mv \
            Resources/lib/python${PYTHON_VERSION}/PyQt6/Qt6/lib/tmp/${LIBNAME}.framework \
            Resources/lib/python${PYTHON_VERSION}/PyQt6/Qt6/lib
    done

    # fixup 4. remove the unused frameworks
    rm -r Resources/lib/python${PYTHON_VERSION}/PyQt6/tmp
    rm -r Resources/lib/python${PYTHON_VERSION}/PyQt6/Qt6/lib/tmp

    # remove qml stuff
    rm -r Resources/lib/python${PYTHON_VERSION}/PyQt6/Qt6/qml
    rm -r Resources/lib/python${PYTHON_VERSION}/PyQt6/Qt6/translations

    # 5. remove .sip files
    find Resources -name '*.sip' -delete

    popd >/dev/null
fi

if [ "$1" != "--package" ]
then
    cp \
        "${PKG_DIST_DIR}/Barry's Emacs-Devel.app/Contents/Resources/emacs_library"/* \
        "Resources/emacs_library"
    cp \
        "${PKG_DIST_DIR}/Barry's Emacs-Devel.app/Contents/Resources/documentation"/* \
        "Resources/documentation"
fi

colour-print "<>info Info:<> build-macosx.sh Editor/PyQtBEmacs - end"
