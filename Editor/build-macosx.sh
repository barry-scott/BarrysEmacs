#!/bin/bash
if [ "$1" == "--enable-debug" ]
then
    shift
    SETUP_OPTIONS=--enable-debug
fi
echo "Info: build-macosx.sh Editor - start"
${PYTHON} setup.py macosx Makefile ${SETUP_OPTIONS}
make -j 8 -f Makefile "$@"
echo "Info: build-macosx.sh Editor - end"
