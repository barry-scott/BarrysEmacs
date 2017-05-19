#!/bin/bash
set -e

if [ "$1" == "--enable-debug" ]
then
    shift
    SETUP_OPTIONS=--enable-debug
fi

if [ "$1" == "--no-bemacs-gui" ]
then
    shift
    SETUP_OPTIONS=--no-bemacs-gui
fi

if [ "${INSTALL_BEMACS_LIB_DIR}" != "" ]
then
    LIB_DIR_OPTION="--lib-dir=${INSTALL_BEMACS_LIB_DIR}"
else
    LIB_DIR_OPTION="--lib-dir=/usr/local/lib/bemacs"
fi
${PYTHON} setup.py linux Makefile ${SETUP_OPTIONS} ${LIB_DIR_OPTION}
if [ -e /proc/cpuinfo ]
then
    NUM_PROC=$( grep -i ^processor /proc/cpuinfo |wc -l )
else
    NUM_PROC=4
fi
make -j ${NUM_PROC} -f Makefile "$@" 2>&1 | tee build.log
