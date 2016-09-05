#!/bin/bash
export PYTHONPATH=${BUILDER_TOP_DIR}/Editor/PyQtBEmacs:${BUILDER_TOP_DIR}/Editor/exe-pybemacs
case "${BUILDER_CFG_PLATFORM}" in
Linux-Fedora)
    export BEMACS_EMACS_LIBRARY=${BUILDER_TOP_DIR}/Kits/Linux/RPM/ROOT/usr/local/bemacs8/lib/bemacs
    ;;
Linux-Debian)
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

elif [ "$1" = "--valgrind" ]
then
    shift 1

    rm -f .gdbinit


    if [ "$1" = "--gdb" ]
    then
        shift 1
        valgrind \
            --db-attach=yes \
            ${TMPDIR:-/tmp}/python -u be_main.py "$@"

    else
        valgrind \
            --log-file=bemacs-memcheck.log \
            ${TMPDIR:-/tmp}p/python -u be_main.py "$@"
    fi

else
    python${PYTHON_VERSION} -u be_main.py "$@"
fi
