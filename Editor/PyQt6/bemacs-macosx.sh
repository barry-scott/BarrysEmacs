#!/bin/bash
export PYTHONPATH=${BUILDER_TOP_DIR}/Editor/PyQt6:${BUILDER_TOP_DIR}/Editor/exe-pybemacs
# use the library that was create in the last build
export BEMACS_EMACS_LIBRARY="${BUILDER_TOP_DIR}/Builder/tmp/dmg/Barry's Emacs.app/Contents/Resources/emacs_library"
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
    lldb -- ${BUILDER_TOP_DIR}/Builder/venv.tmp/bin/python -u ${BUILDER_TOP_DIR}/Editor/PyQt6/be_main.py "$@"

else
    which ${BUILDER_TOP_DIR}/Builder/venv.tmp/bin/python
    ${BUILDER_TOP_DIR}/Builder/venv.tmp/bin/python -u ${BUILDER_TOP_DIR}/Editor/PyQt6/be_main.py "$@"
fi
