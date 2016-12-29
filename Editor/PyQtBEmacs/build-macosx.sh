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

${PYTHON} <<EOF
import zipfile
with zipfile.ZipFile('Resources/lib/python35.zip', 'r') as old:
    with zipfile.ZipFile('Resources/lib/python35clean.zip', 'w' ) as new:
        for name in old.namelist():
            if not name.startswith( 'PyQt5' ):
                print( 'Copy %s' % (name,) )
                data = old.read( name )
                new.writestr( name, data )
EOF

mv Resources/lib/python35clean.zip Resources/lib/python35.zip

# copy all the installed PyQt5 files as py2app does not copy them all
cp -R \
    "/Library/Frameworks/Python.framework/Versions/${PY_VER}/lib/python${PY_VER}/site-packages/PyQt5" \
    "Resources/lib/python${PY_VER}/lib-dynload"

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
done

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
