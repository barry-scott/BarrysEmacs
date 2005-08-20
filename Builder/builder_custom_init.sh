#!/bin/echo Usage: . $0

export RB_WORKINGDIR=$(while [ ! -d Builder ]; do cd ..; done; pwd)
export RB_CFG_PLATFORM=$(uname -s)

case ${RB_CFG_PLATFORM} in

Darwin)
    export RB_CFG_PLATFORM=MacOSX
    ;;

Linux)
    if [ -e /etc/redhat-release ]
    then
        case "$(head /etc/redhat-release)" in

        "Fedora Core release 3 "*)
            export RB_CFG_PLATFORM=LinuxFC3
            ;;

        "Fedora Core release 4 "*)
            export RB_CFG_PLATFORM=LinuxFC4
            ;;

        *)
            echo Error: Unknown RedHat Linux
            ;;
        esac
    else
        echo Error: Unknown RedHat Linux        
    fi
    ;;

*)
    # no need to change
    ;;
esac

echo Info: WorkingDir: ${RB_WORKINGDIR}
echo Info: Config Platform: ${RB_CFG_PLATFORM}
