#!/bin/bash
export PYTHONPATH=${BUILDER_TOP_DIR}/Editor/PyQtBEmacs:${BUILDER_TOP_DIR}/Editor/obj-pybemacs
case "${BUILDER_CFG_PLATFORM}" in
Linux-Fedora)
    export BEMACS_EMACS_LIBRARY=${BUILDER_TOP_DIR}/Kits/Linux/RPM/ROOT/usr/local/bemacs8/lib/bemacs
    ;;
Linux-Debian)
    export BEMACS_EMACS_LIBRARY=${BUILDER_TOP_DIR}/Kits/Linux/DPKG/tree/usr/local/bemacs8/lib/bemacs
    ;;
*)
    exit 1
    ;;
esac

if [ "$1" = "--gdb" ]
then
    shift 1
    echo
    echo "run -u be_main.py " "$@" >.gdbinit
    if [ -e init.gdb ]
    then
        cat init.gdb >.gdbinit
    fi
    echo
    gdb python${PYTHON_VERSION}

elif [ "$1" = "--valgrind" ]
then
    shift 1

    rm -f .gdbinit

    export PYTHONPATH=$PYTHONPATH:/usr/lib64/python${PYTHON_VERSION}:/usr/lib64/python${PYTHON_VERSION}/plat-linux2:/usr/lib64/python${PYTHON_VERSION}/lib-tk:/usr/lib64/python2.6/lib-old:/usr/lib64/python2.6/lib-dynload:/usr/lib64/python2.6/site-packages:/usr/lib64/python2.6/site-packages/PIL:/usr/lib64/python2.6/site-packages/gst-0.10:/usr/lib64/python2.6/site-packages/gtk-2.0:/usr/lib64/python2.6/site-packages/wx-2.8-gtk2-unicode:/usr/lib/python${PYTHON_VERSION}/site-packages

    if [ "$1" = "--gdb" ]
    then
        shift 1
        valgrind \
            --db-attach=yes \
            ${TMPDIR:-/tmp}/python -u be_main.py "$@"

    else
        valgrind \
            --log-file=bemacs-memcheck.log \
            ${TMPDIR:-/tmp}p/python -u be_main.py "$@"
    fi

else
    python${PYTHON_VERSION} -u be_main.py "$@"
fi
