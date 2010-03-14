#!/bin/sh
export PYTHONPATH=${BUILDER_TOP_DIR}/Editor/PythonBEmacs:${BUILDER_TOP_DIR}/Editor/obj-pybemacs
export BEMACS_EMACS_LIBRARY=${BUILDER_TOP_DIR}/Kits/MacOSX/pkg/Barry\'s\ Emacs-Devel.app/Contents/Resources/emacs_library

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
