#!/bin/bash
set -e

echo "Info: Builder build-linux.sh Builder in $( pwd )"

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

${PYTHON} build_bemacs.py "$@"
