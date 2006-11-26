#!/bin/echo Usage: . $0

export RB_WORKINGDIR=$(while [ ! -d Builder ]; do cd ..; done; pwd)
export RB_CFG_PLATFORM=$(uname -s)

case ${RB_CFG_PLATFORM} in

Darwin)
    export RB_CFG_PLATFORM=MacOSX
    ;;

*)
    # no need to change
    ;;
esac

echo Info: WorkingDir: ${RB_WORKINGDIR}
echo Info: Config Platform: ${RB_CFG_PLATFORM}
