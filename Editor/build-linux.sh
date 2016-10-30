#!/bin/bash
if [ "$1" == "--enable-debug" ]
then
    shift
    SETUP_OPTIONS=--enable-debug
fi
${PYTHON} setup.py linux Makefile ${SETUP_OPTIONS}
NUM_PROC=$( grep -i ^processor /proc/cpuinfo |wc -l )
make -j ${NUM_PROC} -f Makefile "$@" 2>&1 | tee build.log
