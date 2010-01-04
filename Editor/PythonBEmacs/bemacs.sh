#!/bin/sh
TARGET=$( ../os_name.ksh )
export PYTHONPATH=$(pwd):$(cd ../${TARGET}/obj; pwd)
export emacs_library=~/pybemacs-kit

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
