#!/bin/echo Usage: . $0

export RB_WORKINGDIR=$(while [ ! -d Builder ]; do cd ..; done; pwd)
export RB_CFG_PLATFORM=$(uname -s)

case ${RB_CFG_PLATFORM} in

Darwin)
    export MACOSX_DEPLOYMENT_TARGET=10.4
    echo Info: MACOSX_DEPLOYMENT_TARGET ${MACOSX_DEPLOYMENT_TARGET}
    export RB_CFG_PLATFORM=MacOSX
    ;;

Linux)
    if [ -e /etc/fedora-release ]
    then
        export RB_CFG_PLATFORM=Linux-Fedora
    elif [ -e /etc/lsb-release -a "$( grep "DISTRIB_ID=Ubuntu" /etc/lsb-release )" == "DISTRIB_ID=Ubuntu" ]
    then
        export RB_CFG_PLATFORM=Linux-Ubuntu
    fi
    ;;
*)
    # no need to change
    ;;
esac

echo Info: WorkingDir: ${RB_WORKINGDIR}
echo Info: Config Platform: ${RB_CFG_PLATFORM}
