#!/bin/bash
echo "Info: Checking for missing dependencies"

function setup_system_info() {
    if [[ -e /etc/os-release ]]
    then
        SYSTEM=$( . /etc/os-release; echo $ID )
    else
        SYSTEM=$( uname )
    fi
    if [[ "$SYSTEM" = "Darwin" && -e /usr/bin/sw_ver ]]
    then
        if [[ "$( /usr/bin/sw_ver -productName )" = "Mac OS X" ]]
        then
            SYSTEM="macOS"
        fi
    fi
}

setup_system_info

case "$SYSTEM" in
macOS)
    ${PYTHON} -m pip install --user PyQt5
    ${PYTHON} -m pip install --user xml-preferences
    ;;

fedora)
    if ! rpm -q sqlite3
    then
        sudo dnf install sqlite3
    fi
    ;;

ubuntu|debian)
    PKG_LIST=${TMPDIR:-/tmp}/installed.tmp
    apt list --installed >${PKG_LIST}
    for PKG in \
        libsqlite3-dev \
        python3-pyqt5 \
        python3-pyqt5 \
        python3-pyqt5.qsci \
        python3-pip \
        ;
    do
        if ! grep -q "^$PKG/" ${PKG_LIST}
        then
            echo "Info: Install $PKG"
            sudo apt install $PKG
        else
            echo "Info: Found package $PKG"
        fi
    done

    ${PYTHON:-python3} -m pip install --user xml-preferences
    ;;

NetBSD)
    for PKG in \
        bash \
        gmake \
        sqlite3 \
        python37 \
        py37-pip \
        py37-sqlite3 \
        py37-qt5 \
        ;
    do
        if ! /usr/sbin/pkg_info -e $PKG >/dev/null
        then
            echo "Info: Install $PKG"
            sudo /usr/sbin/pkd_add $PKG
        else
            echo "Info: Found $PKG"
        fi
    done
    ;;

*)
    echo "Error: Unsupported system $SYSTEM"
    ;;
esac
echo "Info: Done"
