#!/bin/bash
cmd=${1:-mock-testing}
shift
fedora_rel=${1}
shift
arch=${1}
shift

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
    --default-font-name="Fira Mono" \
    --default-font-package=mozilla-fira-mono-fonts \
    --colour "${@}" \
        |& tee build-for-fedora-${fedora_rel}-${arch}.log
