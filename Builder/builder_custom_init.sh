#!/bin/echo Usage: . $0

export BUILDER_CFG_PLATFORM=$(uname -s)

# ether set to tag:<ver> or trunk:<ver>
export PYCXX_VER=tag7.1.2
export PYCXX_VER=trunk:7.1.2

case ${BUILDER_CFG_PLATFORM} in

Darwin)
    export PYTHON_VERSION=3.11
    export BUILDER_CFG_PLATFORM=MacOSX
    export PYTHON=python${PYTHON_VERSION}
    ;;

Linux)
    for version in 3.12 3.11 3.10 3.9
    do
        if [ -e /usr/bin/python${version} ]
        then
            export PYTHON_VERSION=${version}
            export PYTHON=/usr/bin/python${version}
            break
        fi
    done
    if [ -e /etc/fedora-release ]
    then
        export BUILDER_CFG_PLATFORM=Linux-Fedora

    elif [ -e /etc/centos-release ]
    then
        export BUILDER_CFG_PLATFORM=Linux-Fedora

    elif [ -e /etc/lsb-release ]
    then
        if [ "$( grep "DISTRIB_ID=Ubuntu" /etc/lsb-release )" = "DISTRIB_ID=Ubuntu" ]
        then
            export BUILDER_CFG_PLATFORM=Linux-Ubuntu
        fi

    elif [ -e /etc/os-release ]
    then
        if [ "$( grep "ID=debian" /etc/os-release )" = "ID=debian" ]
        then
            export BUILDER_CFG_PLATFORM=Linux-Debian
        fi
    fi
    ;;

NetBSD)
    for version in 3.11 3.10 3.9
    do
        python_candidate=/usr/pkg/bin/python${version}
        if [ -e $python_candiddate ]
        then
            export PYTHON_VERSION=${version}
            export PYTHON=$python_candidate
            break
        fi
    done
    export BUILDER_CFG_PLATFORM=NetBSD
    ;;

OpenBSD)
    for version in 3.11 3.10 3.9
    do
        python_candidate=/usr/local/bin/python${version}
        if [ -e $python_candidate ]
        then
            export PYTHON_VERSION=${version}
            export PYTHON=$python_candidate
            break
        fi
    done
    export BUILDER_CFG_PLATFORM=OpenBSD
    ;;

*)
    # no need to change
    ;;
esac

echo Info: WorkingDir: ${BUILDER_TOP_DIR}
echo Info: Config Platform: ${BUILDER_CFG_PLATFORM}
if [[ "${PYTHON_VERSION}" = "" ]]
then
    echo "ERROR: cannot find a suitable version of python."
    PYTHON='/bin/echo ERROR'
else
    echo Info: Python Version: ${PYTHON_VERSION}
fi
