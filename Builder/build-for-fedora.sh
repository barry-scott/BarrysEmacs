#!/bin/bash
cmd=${1:-copr-testing}
rel=${2}
if [ "${rel}" = "" ]
then
    python3 ./package_bemacs.py ${cmd} \
        --colour \
        --hunspell \
        --system-ucd

else
    python3 ./package_bemacs.py ${cmd} \
        --mock-target=fedora-${rel}-$(arch) \
        --colour \
        --hunspell \
        --system-ucd
fi
