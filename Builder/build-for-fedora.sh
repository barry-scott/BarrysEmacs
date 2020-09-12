#!/bin/bash
cmd=${1:-copr-testing}
rel=${2}
if [ "${rel}" = "" ]
then
    python3 ./package_bemacs.py ${cmd} \
        --colour \
        --system-hunspell \
        --system-ucd

else
    python3 ./package_bemacs.py ${cmd} \
        --mock-target=fedora-${rel}-$(arch) \
        --colour \
        --system-hunspell \
        --system-ucd
fi
