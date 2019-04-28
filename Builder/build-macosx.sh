#!/bin/bash
set -e

echo "Info: build-macosx.sh Builder - start"

echo "Info: Checking for Python"
if [ "${PYTHON}" = "" ]
then
    echo "Error: environment variable PYTHON not set"
    exit 1
fi

if ! which "${PYTHON}" >/dev/null
then
    echo "Error: PYTHON program ${PYTHON} does not exist"
    exit 1
fi

echo "Info: checking for python library PyQt5"
if ! ${PYTHON} -c 'from PyQt5 import QtWidgets, QtGui, QtCore' 2>/dev/null
then
    echo "Error: PyQt5 is not installed for ${PYTHON}. Hint: ${PYTHON} -m pip install PyQt5"
    exit 1
fi

if [ -e "/Volumes/Barry's Emacs" ]
then
    echo "Info: unmount old kit dmg"
    umount "/Volumes/Barry's Emacs"
fi

echo "Info: make macosx.mak"
make -f macosx.mak PYTHON=$PYTHON clean build 2>&1 | tee build.log

if false
then
    echo "Info: rebuild the OS X launch service database"
    CORE=/System/Library/Frameworks/CoreServices.framework/Versions/Current
    ${CORE}/Frameworks/LaunchServices.framework/Versions/Current/Support/lsregister \
     -kill \
     -r -domain local -domain system -domain user
fi

DMG=$( find .. -name '*.dmg' )
echo "Info: DMG ${DMG}"
if [ "$1" = "--install" ]
then
    # macOS knows about these extra copies of the emacs app
    # and will start all of them at the same time so delete
    rm -rf ../Kits/MacOSX/pkg
    rm -rf ../Kits/MacOSX/tmp/BarrysEmacs-V*[0-9]/
    open ${DMG}
fi
echo "Info: build-macosx.sh Builder - end"
