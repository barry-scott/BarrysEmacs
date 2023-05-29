#!/bin/bash
export EMACS_DEBUG=
export EMACS_DEBUG_FILE="$TMPDIR/bemacs.log"
export emacs_user="$HOME/bemacs"

export BEMACS_EMACS_LIBRARY=${BUILDER_TOP_DIR}/Builder/tmp/ROOT/usr/lib/bemacs
export BEMACS_EMACS_DOC=${BUILDER_TOP_DIR}/Builder/tmp/ROOT/usr/share/bemacs/doc/
export PYTHONPATH=${BEMACS_EMACS_LIBRARY}:${BUILDER_TOP_DIR}/Editor/exe-pybemacs

RUN_GDB=0
RUN_VALGRIND=0
MOCK_EDITOR=0

while :
do
    case "$1" in
    --debug=*)
        export EMACS_DEBUG="${1#--debug=}"
        rm -f ${EMACS_DEBUG_FILE}
        shift
        ;;

    --debug-file=*)
        export EMACS_DEBUG_FILE="${1#--debug-file=}"
        shift
        ;;

    --gdb)
        RUN_GDB=1
        shift
        ;;

    --valgrind)
        RUN_VALGRIND=1
        shift
        ;;

    --mock-editor)
        MOCK_EDITOR=1
        break
        ;;

    *)
        break
        ;;
    esac
done

if [[ "$MOCK_EDITOR" = "0" && ! -e "${BEMACS_EMACS_LIBRARY}/emacslib.db" ]]
then
    echo "Error: emacslib not found: ${BEMACS_EMACS_LIBRARY}/emacslib.db"
    exit 1
fi

if [[ "${RUN_VALGRIND}" = "1" ]]
then
    rm -f .gdbinit

    if [[ "${RUN_GDB}" = "1" ]]
    then
        valgrind \
            --db-attach=yes \
            ${TMPDIR:-/tmp}/python -u ${BEMACS_EMACS_LIBRARY}/be_main.py "$@"

    else
        valgrind \
            --log-file=bemacs-memcheck.log \
            ${TMPDIR:-/tmp}p/python -u ${BEMACS_EMACS_LIBRARY}/be_main.py "$@"
    fi

elif [[ "${RUN_GDB}" = "1" ]]
then
    echo
    echo >.gdbinit
    if [ -e init.gdb ]
    then
        cat init.gdb >>.gdbinit
    fi
    echo "b main" >>.gdbinit
    echo "run -u ${BEMACS_EMACS_LIBRARY}/be_main.py " "$@" >>.gdbinit
    echo
    gdb python${PYTHON_VERSION}

else
    python${PYTHON_VERSION} -u ${BEMACS_EMACS_LIBRARY}/be_main.py "$@"
fi
