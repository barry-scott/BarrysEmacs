#!/bin/sh
set -e

if [ "$1" == "--enable-debug" ]
then
    shift
    SETUP_OPTIONS=--enable-debug
fi

TARGET=${1:-all}
shift

if [ "${INSTALL_BEMACS_LIB_DIR}" != "" ]
then
    LIB_DIR_OPTION="--lib-dir=${INSTALL_BEMACS_LIB_DIR}"
else
    LIB_DIR_OPTION="--lib-dir=/usr/local/lib/bemacs"
fi
${PYTHON} setup.py netbsd ${TARGET} Makefile-${TARGET} ${SETUP_OPTIONS} ${LIB_DIR_OPTION}
if [ -e /proc/cpuinfo ]
then
    NUM_PROC=$( grep -i ^processor /proc/cpuinfo |wc -l )
else
    NUM_PROC=4
fi
gmake -j ${NUM_PROC} -f Makefile-${TARGET} "$@" 2>&1 | tee build.log
