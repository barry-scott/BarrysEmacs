#!/bin/bash
export EMACS_DEBUG=
export EMACS_DEBUG_FILE="$TMPDIR/bemacs.log"
export emacs_user="$HOME/bemacs"

export PYTHONPATH=${BUILDER_TOP_DIR}/Editor/PyQtBEmacs:${BUILDER_TOP_DIR}/Editor/exe-pybemacs
export BEMACS_EMACS_LIBRARY=${BUILDER_TOP_DIR}/Builder/tmp/ROOT/usr/lib/bemacs
export BEMACS_EMACS_DOC=${BUILDER_TOP_DIR}/Builder/tmp/ROOT/usr/share/bemacs/doc/

if [ ! -e "${BEMACS_EMACS_LIBRARY}/emacslib.db" ]
then
    echo "Error: emacslib not found: ${BEMACS_EMACS_LIBRARY}/emacslib.db"
    exit 1
fi

case "$1" in
--debug=*)
    export EMACS_DEBUG="${1#--debug=}"
    rm -f ${EMACS_DEBUG_FILE}
    shift
    ;;
*)
    ;;
esac

if [ "$1" = "--gdb" ]
then
    shift 1
    echo
    echo >.gdbinit
    if [ -e init.gdb ]
    then
        cat init.gdb >>.gdbinit
    fi
    echo "b main" >>.gdbinit
    echo "run -u ${BUILDER_TOP_DIR}/Editor/PyQtBEmacs/be_main.py " "$@" >>.gdbinit
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
            ${TMPDIR:-/tmp}/python -u ${BUILDER_TOP_DIR}/Editor/PyQtBEmacs/be_main.py "$@"

    else
        valgrind \
            --log-file=bemacs-memcheck.log \
            ${TMPDIR:-/tmp}p/python -u ${BUILDER_TOP_DIR}/Editor/PyQtBEmacs/be_main.py "$@"
    fi

else
    python${PYTHON_VERSION} -u ${BUILDER_TOP_DIR}/Editor/PyQtBEmacs/be_main.py "$@"
fi
