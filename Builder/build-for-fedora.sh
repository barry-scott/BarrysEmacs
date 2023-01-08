#!/bin/bash
cmd=${1:-mock-testing}
arch=${2}
fedora_rel=${3}
if [ "$arch" = "" ]
then
    arch=$(arch)
fi

if [ "${fedora_rel}" = "" ]
then
    fedora_rel=$(</etc/system-release-cpe)
    fedora_rel=${fedora_rel##*:}
fi

python3 ./package_bemacs.py ${cmd} \
    --mock-target=fedora-${fedora_rel}-${arch} \
    --system-hunspell \
    --colour \
        |& tee build-for-fedora-${fedora_rel}-${arch}.log
