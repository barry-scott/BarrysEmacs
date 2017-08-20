#!/bin/bash
export EMACS_DEBUG=
export EMACS_DEBUG_FILE="$TMPDIR/bemacs.log"
export emacs_user="$HOME/bemacs"
case "$( uname )" in
Darwin)
    export emacs_library="${BUILDER_TOP_DIR}/Kits/MacOSX/pkg/Barry's Emacs-Devel.app/Contents/Resources/emacs_library"
   ;;
*)
    export emacs_library="${BUILDER_TOP_DIR:?builder_init}/Kits/Linux/ROOT/usr/local/lib/bemacs"
    ;;
esac

case "$1" in
--debug=*)
    export EMACS_DEBUG="${1#--debug=}"
    rm -f ${EMACS_DEBUG_FILE}
    shift
    ;;
*)
    ;;
esac

case "$1" in
--gdb)
    shift
    gdb exe-cli-bemacs/bemacs-cli
    ;;
--lldb)
    shift
    lldb exe-cli-bemacs/bemacs-cli
    ;;
*)
    exe-cli-bemacs/bemacs-cli "$@"
    ;;
esac
stty sane
