#!/bin/bash
set -x
cmd=${1:-mock-testing}
arch=${2}
fedora_rel=${3}
if [ "$arch" = "" ]
then
    arch=$(arch)
fi

if [ "${fedora_rel}" = "" ]
then
    python3 ./package_bemacs.py ${cmd} \
        --system-hunspell \
        --colour

else
    python3 ./package_bemacs.py ${cmd} \
        --mock-target=fedora-${fedora_rel}-${arch} \
        --system-hunspell \
        --colour
fi
