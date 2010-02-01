#!/bin/sh
TARGET=$( ../os_name.ksh )
export PYTHONPATH=$(pwd):$(cd ../${TARGET}/obj-pybemacs; pwd)
export emacs_library=~/pybemacs-kit
export EMACS_CONTROL_STRING_PROCESSING=0

if [ "$1" = "--gdb" ]
then
    shift 1
    echo
    echo "run -u be_main.py " "$@" >.gdbinit
    if [ -e init.gdb ]
    then
        cat init.gdb >.gdbinit
    fi
    echo
    gdb python${PYTHON_VERSION}

else
    python${PYTHON_VERSION} -u be_main.py "$@"
fi
