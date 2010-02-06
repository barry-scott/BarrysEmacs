#!/bin/sh
case ${BUILDER_CFG_PLATFORM} in
MacOSX)
    ${PYTHON} setup.py macosx Makefile
    ;;

Linux*)
    ${PYTHON} setup.py linux Makefile
    ;;
*)
    exit 1
    ;;
esac
make -f Makefile "$@"
