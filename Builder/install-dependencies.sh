#!/bin/bash

if ! which colour-print >/dev/null
then
    function colour-print() {
        echo "${@}"
    }
fi

colour-print "<>info Info:<> Checking for missing dependencies"

function setup_system_info() {
    if [[ -e /etc/os-release ]]
    then
        SYSTEM=$( . /etc/os-release; echo $ID )
    else
        SYSTEM=$( uname )
    fi
    if [[ "$SYSTEM" = "Darwin" && -e /usr/bin/sw_vers ]]
    then
        if [[ "$( /usr/bin/sw_vers -productName )" = "Mac OS X" ]]
        then
            SYSTEM="macOS"
        fi
    fi
}

setup_system_info

colour-print "<>info Info:<> Import build dependencies for ${SYSTEM}"

case "$SYSTEM" in
macOS)
    colour-print "<>info Info:<> Import python packages from PyPI"
    ${PYTHON} -m pip install --user PyQt6
    ${PYTHON} -m pip install --user xml-preferences

    if [ "$1" = "" -o ! -d "$1" ]
    then
        colour-print "<>error Error:<> Expect \$1 to be a folder of dependencies"
        exit 1
    fi

    colour-print "<>info Info:<> Import pycxx"
    ./import-pycxx.sh "$1"/pycxx-*.tar.gz

    colour-print "<>info Info:<> Import sqlite"
    ./import-sqlite3.sh "$1"/sqlite-amalgamation-*.zip

    colour-print "<>info Info:<> Import hunspell"
    ./import-hunspell.sh "$1"/hunspell-*.tar.gz "$1"/hunspell-*.zip
    ;;

fedora)
    for PKG in \
        unicode-ucd \
        sqlite \
        sqlite-devel \
        hunspell \
        hunspell-devel \
        python3-pycxx-devel \
        ;
    do
        if ! rpm -q $PKG
        then
            sudo dnf install $PKG
        fi
    done
    ;;

ubuntu|debian)
    PKG_LIST=${TMPDIR:-/tmp}/installed.tmp
    apt list --installed >${PKG_LIST}
    for PKG in \
        libsqlite-dev \
        python3-pyqt6 \
        python3-pyqt6 \
        python3-pyqt6.qsci \
        python3-pip \
        ;
    do
        if ! grep -q "^$PKG/" ${PKG_LIST}
        then
            colour-print "<>info Info:<> Import $PKG"
            sudo apt import $PKG
        else
            colour-print "<>info Info:<> Found package $PKG"
        fi
    done

    ${PYTHON:-python3} -m pip install --user xml-preferences
    ;;

NetBSD)
    for PKG in \
        bash \
        gmake \
        sqlite \
        python37 \
        py37-pip \
        py37-sqlite \
        py37-qt5 \
        ;
    do
        if ! /usr/sbin/pkg_info -e $PKG >/dev/null
        then
            colour-print "<>info Info:<> Import $PKG"
            sudo /usr/sbin/pkd_add $PKG
        else
            colour-print "<>info Info:<> Found $PKG"
        fi
    done
    ;;

*)
    colour-print "Error: Unsupported system $SYSTEM"
    ;;
esac
colour-print "<>info Info:<> Done"
