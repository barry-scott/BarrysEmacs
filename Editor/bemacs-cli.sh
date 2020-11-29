#!/usr/bin/env bash

TTY_SETTINGS=$(stty -g)

export EMACS_DEBUG=
export emacs_user="$HOME/bemacs"

echo "Info: set EMACS_TERM_DEVICE to the tty to use"
case "$( uname )" in
Darwin)
    export emacs_library="${BUILDER_TOP_DIR}/Builder/tmp/pkg/Barry's Emacs-Devel.app/Contents/Resources/emacs_library"
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

case "$1" in
--debug=*)
    echo "Turn on debug"
    export EMACS_DEBUG="${1#--debug=}"
    shift
    ;;
*)
    ;;
esac
case "$1" in
--file)
    export EMACS_DEBUG_FILE="$TMPDIR/bemacs.log"
    rm -f ${EMACS_DEBUG_FILE}
    shift
    ;;
*)
    ;;
esac

case "$1" in
--gdb)
    shift
    gdb --args exe-cli-bemacs/bemacs-cli "$@"
    ;;
--lldb)
    shift
    lldb exe-cli-bemacs/bemacs-cli
    ;;
*)
    exe-cli-bemacs/bemacs-cli "$@"
    ;;
esac
stty ${TTY_SETTINGS}
