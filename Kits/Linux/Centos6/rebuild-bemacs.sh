#!/bin/bash
scl enable rh-git29 bash <<EOF
    cd ~/Projects/BarrysEmacs/Builder
    git pull --rebase
    . builder_init
    case "$1" in
    "el6")
        make -f linux.mak brand
        cd ~/Projects/BarrysEmacs/Kits/Linux/Centos6
        chmod +x ./make-devel-src-tarball.sh
        ./make-devel-src-tarball.sh
        chmod +x ./bemacs-mock-build.sh
        ./bemacs-mock-build.sh ${3:-centos6.3-x86_64}
        ;;
    "el7")
        ./build-linux.sh $2
        ./local-install-linux.sh
        ;;
    *)
        echo "Usage: $0 el6|el7 cli|gui [<mock-chroot>]"
    esac

    echo "Info: recent changes"
    git log -n 10 '--format=format:%ai %f'
EOF
