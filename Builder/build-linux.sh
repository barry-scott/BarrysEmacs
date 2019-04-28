#!/bin/bash
set -e

echo "Info: build-linux.sh Builder - start"
case "$1" in
cli)
    target=cli
    ;;

gui)
    target=gui
    ;;

*)
    target=gui
    ;;
esac

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

if [ "$target" != "cli" ]
then
    echo "Info: checking for python library PyQt5"
    if ! ${PYTHON} -c 'from PyQt5 import QtWidgets, QtGui, QtCore' 2>/dev/null
    then
        echo "Error: PyQt5 is not installed for ${PYTHON}. Hint: dnf install PyQt5"
        exit 1
    fi
fi

make -f linux.mak PYTHON=${PYTHON:?missing python def} clean-${target} build-${target}
