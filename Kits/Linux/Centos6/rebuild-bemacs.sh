#!/bin/bash
scl enable rh-python35 rh-git29 bash <<EOF
    cd ~/Project/BarrysEmacs/Builder
    git pull --rebase
    . builder_init
    case "$1" in
    "el6")
        make -f linux.mak brand
        cd ~/Project/BarrysEmacs/Kits/Linux/Centos6
        chmod +x ./make-devel-src-tarball.sh
        ./make-devel-src-tarball.sh
        chmod +x ./bemacs-mock-build.sh
        ./bemacs-mock-build.sh centos6.3-x86_64
        ;;
    "el7")
        ./build-linux.sh
        ./local-install-linux.sh
        ;;
    *)
        echo "Usage: $0 [el6|el7]"
    esac
EOF
