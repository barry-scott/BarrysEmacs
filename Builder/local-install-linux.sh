#!/bin/bash
case ${BUILDER_CFG_PLATFORM:? run builder_init} in
Linux-Fedora)
    cd ${BUILDER_TOP_DIR}/Kits/Linux/RPM/ROOT
    ;;

Linux-Debian)
    cd ${BUILDER_TOP_DIR}/Kits/Linux/DPKG/tree
    ;;

*)
    echo "Error: Unknown BUILDER_CFG_PLATFORM of ${BUILDER_CFG_PLATFORM}"
    exit 1
esac

tar czf ${TMPDIR:-/tmp}/bemacs-tmp.tar.gz usr
sudo tar xzf ${TMPDIR:-/tmp}/bemacs-tmp.tar.gz -C /
