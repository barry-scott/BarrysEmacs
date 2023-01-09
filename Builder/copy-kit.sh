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

Linux-Ubuntu|Linux-Debian)
    . /etc/os-release
    # assume that debian and ubuntu never use the same codename
    TARGET=/shared/Downloads/BEmacs/beta/${VERSION_CODENAME}
    mkdir -p ${TARGET}
    for DSC in tmp/*.dsc
    do
        if [[ -e "$DSC" ]]
        then
            cp -v "$DSC" ${TARGET}
            DEB="${DSC%%.dsc}_*.deb"
            cp -v ${DEB} ${TARGET}
        else
            colour-print "<>error Error: No .dsc file found<>"
        fi
    done
    if [[ "$1" = "--install" ]]
    then
        pushd /shared/Downloads/Debian
        ./make-apt-repos.sh update

        colour-print "<>info Info: apt update<>"
        sudo apt update
        colour-print "<>info Info: apt install bemacs<>"
        sudo apt install bemacs
    fi
    ;;

*)
    echo "Unsupported platform for copy"
    ;;
esac
