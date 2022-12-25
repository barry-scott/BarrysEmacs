#!/usr/bin/env bash

TTY_SETTINGS=$(stty -g)

export EMACS_DEBUG=
export emacs_user="$HOME/bemacs"

echo "Info: set EMACS_TERM_DEVICE to the tty to use"
case "$( uname )" in
Darwin)
    export emacs_library="${BUILDER_TOP_DIR}/Builder/tmp/pkg/Barry's Emacs.app/Contents/Resources/emacs_library"
   ;;
*)
    export emacs_library="${BUILDER_TOP_DIR:?builder_init}/Builder/tmp/ROOT/usr/lib/bemacs"
    ;;
esac

if [ ! -e "${emacs_library}/emacslib.db" ]
then
    echo "Error: cannot find emacslib.db in ${emacs_library}"
    exit 1
fi

RUN_DEBUGGER=

while :
do
    case "$1" in
    --debug=*)
        echo "Turn on debug"
        export EMACS_DEBUG="${1#--debug=}"
        shift
        ;;

    --debug-file)
        export EMACS_DEBUG_FILE="$TMPDIR/bemacs.log"
        rm -f "${EMACS_DEBUG_FILE}"
        echo "Debug log file ${EMACS_DEBUG_FILE}"
        shift
        ;;

    --debug-file=*)
        export EMACS_DEBUG_FILE="${1#--debug-file=}"
        echo "Debug log file ${EMACS_DEBUG_FILE}"
        shift
        ;;

    --gdb)
        shift
        RUN_DEBUGGER=gdb
        ;;

    --lldb)
        shift
        RUN_DEBUGGER=lldb
        ;;

    *)
        break
        ;;
    esac
done

case "${RUN_DEBUGGER}" in
gdb)

    gdb --args exe-cli-bemacs/bemacs-cli "$@"
    ;;

lldb)
    lldb exe-cli-bemacs/bemacs-cli
    ;;

*)
    exe-cli-bemacs/bemacs-cli "$@"
    ;;
esac

stty ${TTY_SETTINGS}
