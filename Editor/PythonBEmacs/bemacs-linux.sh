#!/bin/sh
export PYTHONPATH=${BUILDER_TOP_DIR}/Editor/PythonBEmacs:${BUILDER_TOP_DIR}/Editor/obj-pybemacs
case "${BUILDER_CFG_PLATFORM}" in
Linux-Fedora)
    export BEMACS_EMACS_LIBRARY=${BUILDER_TOP_DIR}/Kits/Linux/RPM/ROOT/usr/local/bemacs8/lib/bemacs
    ;;
Linux-Ubuntu)
    export BEMACS_EMACS_LIBRARY=${BUILDER_TOP_DIR}/Kits/Linux/DPKG/tree/usr/local/bemacs8/lib/bemacs
    ;;
*)
    exit 1
    ;;
esac

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
