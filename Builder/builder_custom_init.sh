#!/bin/echo Usage: . $0

export BUILDER_CFG_PLATFORM=$(uname -s)
export PYTHON_VERSION=${1:-3.6}

# ether set to tag:<ver> or trunk:<ver>
export PYCXX_VER=tag7.0.1
export PYCXX_VER=trunk:7.0.1

case ${BUILDER_CFG_PLATFORM} in

Darwin)
    export BUILDER_CFG_PLATFORM=MacOSX
    export PYTHON=python${PYTHON_VERSION}
    ;;

Linux)
    for version in ${PYTHON_VERSION} 3.6 3.5 3.4
    do
        if [ -e /usr/bin/python${version} ]
        then
            export PYTHON_VERSION=${version}
            export PYTHON=/usr/bin/python${version}
            break
        elif [ -e /opt/rh/rh-python35/root/usr/bin/python${version} ]
        then
            export PYTHON_VERSION=${version}
            export PYTHON=/opt/rh/rh-python35/root/usr/bin/python${version}
        fi
    done
    if [ -e /etc/fedora-release ]
    then
        export BUILDER_CFG_PLATFORM=Linux-Fedora

    elif [ -e /etc/centos-release ]
    then
        export BUILDER_CFG_PLATFORM=Linux-Fedora

    else
        # assume a debian type of system
        export BUILDER_CFG_PLATFORM=Linux-Debian

    fi
    if [ "$PYTHON" = "" ]
    then
        export PYTHON=python${PYTHON_VERSION}
    fi
    ;;
*)
    # no need to change
    ;;
esac

echo Info: WorkingDir: ${BUILDER_TOP_DIR}
echo Info: Config Platform: ${BUILDER_CFG_PLATFORM}
echo Info: Python Version: ${PYTHON_VERSION}
