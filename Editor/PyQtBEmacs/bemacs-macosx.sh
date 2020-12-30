#!/bin/bash
export PYTHONPATH=${BUILDER_TOP_DIR}/Editor/PyQtBEmacs:${BUILDER_TOP_DIR}/Editor/exe-pybemacs
export BEMACS_EMACS_LIBRARY="${BUILDER_TOP_DIR}/Builder/tmp/pkg/Barry's Emacs-Devel.app/Contents/Resources/emacs_library"
export BEMACS_FIFO=.bemacs8devel/.emacs_command

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

if [ "$1" = "--lldb" ]
then
    shift 1
    echo
    rm -f .lldbinit
    if [ -e init.lldb ]
    then
        cat init.lldb >.lldbinit
    fi
    echo
    lldb -- python${PYTHON_VERSION} -u be_main.py "$@"

else
    which python${PYTHON_VERSION}
    python${PYTHON_VERSION} -u be_main.py "$@"
fi
