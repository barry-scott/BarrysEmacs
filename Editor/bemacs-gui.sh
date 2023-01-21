#!/bin/bash
case $( uname ) in
Linux)
    exec PyQt6/bemacs-linux.sh "$@"
    ;;

*)
    exec PyQt6/bemacs-macosx.sh "$@"
    ;;
esac
