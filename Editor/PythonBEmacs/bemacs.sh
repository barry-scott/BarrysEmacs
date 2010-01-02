#!/bin/sh
TARGET=$( ../Editor/os_name.ksh )
export PYTHONPATH=$(pwd):$(cd ../Editor/${TARGET}/obj; pwd)
export emacs_library=~/pybemacs-kit

if [ "$1" = "--gdb" ]
then
    shift 1
    echo "run -u be_main.py " "$@"
    gdb python2.6

else
    python2.6 -u be_main.py "$@"
fi
