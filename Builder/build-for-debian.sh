#!/bin/bash
CMD=${1:-debian-test-build}

. /etc/os-release
TARGET=/shared/Downloads/BEmacs/beta/${VERSION_CODENAME}

case "${VERSION_CODENAME}" in
jammy)
    PYQT_VERSION=5
    ;;

*)
    PYQT_VERSION=6
    ;;
esac

python3 ./package_bemacs.py ${CMD} \
    --debian-repos=${TARGET} \
    --pyqt-version=${PYQT_VERSION} \
    --colour
