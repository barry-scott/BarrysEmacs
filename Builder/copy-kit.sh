#!/bin/bash
case "${BUILDER_CFG_PLATFORM}" in
MacOSX)
    DMG=$( echo tmp/dmg/*.dmg )
    if [ ! -e /Volumes/Downloads ]
    then
        echo "Mounting shares..."
        open -a mount-shares
    fi

    for ((i=1; i<=30; i++))
    do
        if [ -e /Volumes/Downloads/BEmacs/beta ]
        then
            cp -fv "$DMG" /Volumes/Downloads/BEmacs/beta
            exit $?
        else
            sleep 1
        fi
    done
    echo "Downloads is missing"
    ;;

*)
    echo "Unsupported platform for copy"
    ;;
esac
