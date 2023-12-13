#!/bin/sh
set -e

echo "Info: Builder build-openbsd.sh Builder in $( pwd )"

echo "Info: Checking for Python"
if [ "${PYTHON}" = "" ]
then
    echo "Error: environment variable PYTHON not set"
    exit 1
fi

if ! which "${PYTHON}" >/dev/null
then
    echo "Error: PYTHON program ${PYTHON} does not exist"
    exit 1
fi

for cmd in pkg-config gmake hunspell
do
    if ! which $cmd >/dev/null
    then
        echo "Error: command $cmd must be installed"
        exit 1
    fi
done

target=${1:-cli}
shift

# no     --system-hunspell \
${PYTHON} build_bemacs.py \
    --system-sqlite \
    --no-sftp \
    --default-font-name="Liberation Mono" \
    ${target} "$@"
