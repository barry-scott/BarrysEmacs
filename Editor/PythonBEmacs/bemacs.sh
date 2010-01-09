#!/bin/sh
TARGET=$( ../os_name.ksh )
export PYTHONPATH=$(pwd):$(cd ../${TARGET}/obj; pwd)
export emacs_library=~/pybemacs-kit
export EMACS_CONTROL_STRING_PROCESSING=0

if [ "$1" = "--gdb" ]
then
    shift 1
    echo
    echo "run -u be_main.py " "$@"
    echo
    gdb python${PYTHON_VERSION}

else
    python${PYTHON_VERSION} -u be_main.py "$@"
fi
