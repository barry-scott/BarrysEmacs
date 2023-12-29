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

case "$CMD" in
--install-deps)
    sudo apt install \
        devscripts \
        libhunspell-dev python3-cxx-dev libsqlite3-dev libssh-dev unicode-data \
        hunspell hunspell-uk python3-pyqt6 \
        ;
    ;;

*)
python3 ./package_bemacs.py ${CMD} \
    --debian-repos=${TARGET} \
    --pyqt-version=${PYQT_VERSION} \
    --default-font-name="Noto Mono" \
    --default-font-package=fonts-noto-mono \
    --colour \
        |& tee build-for-debian.log
    ;;
esac
