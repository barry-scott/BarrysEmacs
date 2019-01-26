#!/bin/bash
case "${BUILDER_CFG_PLATFORM}" in
MacOSX)
    DMG=$( find .. -name '*.dmg' )
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

Linux-Fedora)
    cp -fv ../Kits/Linux/tmp/${KITNAME}*.x86_64.rpm /shared/Downloads/BEmacs/beta
    ;;

*)
    echo "Unsupport platform for copy"
    ;;
esac
