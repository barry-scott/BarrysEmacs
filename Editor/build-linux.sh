#!/bin/bash
if [ "$1" == "--enable-debug" ]
then
    shift
    SETUP_OPTIONS=--enable-debug
fi
if [ "${INSTALL_BEMACS_LIB_DIR}" != "" ]
then
    LIB_DIR_OPTION="--lib-dir=${INSTALL_BEMACS_LIB_DIR}"
else
    LIB_DIR_OPTION="--lib-dir=/usr/local/lib/bemacs"
fi
${PYTHON} setup.py linux Makefile ${SETUP_OPTIONS} ${LIB_DIR_OPTION}
NUM_PROC=$( grep -i ^processor /proc/cpuinfo |wc -l )
make -j ${NUM_PROC} -f Makefile "$@" 2>&1 | tee build.log
