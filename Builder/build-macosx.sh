#!/bin/bash
set -e

echo "Info: build-macosx.sh Builder - start"

echo "Info: Checking for Qt"
if [ "${BUILDER_QTDIR}" = "" ]
then
    echo "Error: environment variable BUILDER_QTDIR not set"
    exit 1
fi

if [ ! -e "${BUILDER_QTDIR}" ]
then
    echo "Error: BUILDER_QTDIR folder ${BUILDER_QTDIR} does not exist"
    exit 1
fi

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
    echo "Error: PyQt5 is not installed for ${PYTHON}. Hint: build and install PyQt5 from source"
    exit 1
fi

echo "Info: checking for python library QScintilla"
if ! ${PYTHON} -c 'from PyQt5 import Qsci' 3>/dev/null
then
    echo "Error: QScintilla is not installed for ${PYTHON}. Hint: pip3 install QScintilla"
    exit 1
fi

echo "Info: make macosx.mak"
make -f macosx.mak PYTHON=$PYTHON clean build 2>&1 | tee build.log

echo "Info: rebuild the OS X launch service database"
CORE=/System/Library/Frameworks/CoreServices.framework/Versions/Current
${CORE}/Frameworks/LaunchServices.framework/Versions/Current/Support/lsregister \
 -kill \
 -r -domain local -domain system -domain user

DMG=$( find .. -name '*.dmg' )
echo "Info: DMG ${DMG}"
if [ "$1" = "--install" ]
then
    open ${DMG}
fi
echo "Info: build-macosx.sh Builder - end"
