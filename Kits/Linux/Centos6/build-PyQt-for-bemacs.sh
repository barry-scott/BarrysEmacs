#!/bin/bash

python=python3.6
sip_kit=sip-4.19.14
pyqt_kit=PyQt5_gpl-5.12
qsci_kit=QScintilla_gpl-2.11.1

num_cores=$( lscpu -p | grep -e '^[0-9]' -c )

if [ "$1" = "" ]
then
    echo "Usage: $0 [all|sip|pyqt|qsci]"
    exit 1
fi

if [ "$1" = "sip" -o "$1" = "all" ]
then
    # sip
    set -e
    cd ~/Projects
    rm -rf ${sip_kit}
    tar xf ${sip_kit}.tar.gz
    cd ${sip_kit}
    ${python} configure.py --sip-module PyQt5.sip
    make --jobs=${num_cores}
    if ! sudo -n true 2>/dev/null
    then
        echo -e -n "\e[35;1mEnter sudo password when prompted. (press return to continue)\e[m "; read A
    fi
    sudo make install
fi

if [ "$1" = "pyqt" -o "$1" = "all" ]
then
    # PyQt
    cd ~/Projects
    rm -rf ${pyqt_kit}
    tar xf ${pyqt_kit}.tar.gz
    cd ${pyqt_kit}
    ${python} configure.py --qmake=/usr/bin/qmake-qt5 --confirm-license
    make --jobs=${num_cores}
    if ! sudo -n true 2>/dev/null
    then
        echo -e -n "\e[35;1mEnter sudo password when prompted. (press return to continue)\e[m "; read A
    fi
    sudo make install
fi

if [ "$1" = "qsci" -o "$1" = "all" ]
then
    # QScintilla
    cd ~/Projects
    rm -rf ${qsci_kit}
    tar  xf ${qsci_kit}.tar.gz
    cd ${qsci_kit}
    cd Qt4Qt5
    /usr/bin/qmake-qt5 qscintilla.pro
    make --jobs=${num_cores}
    if ! sudo -n true 2>/dev/null
    then
        echo -e -n "\e[35;1mEnter sudo password when prompted. (press return to continue)\e[m "; read A
    fi
    sudo make install
    cd ../Python
    ${python} configure.py --pyqt=PyQt5 --qmake=/usr/bin/qmake-qt5
    make --jobs=${num_cores}
    if ! sudo -n true 2>/dev/null
    then
        echo -e -n "\e[35;1mEnter sudo password when prompted. (press return to continue)\e[m "; read A
    fi
    sudo make install
fi
