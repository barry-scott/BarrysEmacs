#!/bin/bash
case ${BUILDER_CFG_PLATFORM:? run builder_init} in
Linux-Fedora)
    cd ${BUILDER_TOP_DIR}/Kits/Linux/RPM/ROOT
    ;;

Linux-Ubuntu)
    cd ${BUILDER_TOP_DIR}/Kits/Linux/DPKG/tree
    ;;

*)
    echo "Error: Unknown BUILDER_CFG_PLATFORM of ${BUILDER_CFG_PLATFORM}"
    exit 1
esac

tar czf /tmp/bemacs-tmp.tar.gz usr
sudo tar xzf /tmp/bemacs-tmp.tar.gz -C /
