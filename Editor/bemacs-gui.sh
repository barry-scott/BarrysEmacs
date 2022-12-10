#!/bin/bash
case $( uname ) in
Linux)
    exec PyQtBEmacs/bemacs-linux.sh "$@"
    ;;

*)
    exec PyQtBEmacs/bemacs-macosx.sh
    ;;
esac
