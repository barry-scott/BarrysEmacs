#!/bin/bash

VER=${1?version}

case "${VER}" in
[0-9].[0-9].[0-9])
    if [ "$(git tag --list ${VER})" != "" ]
    then
        echo "Tag ${VER} already exists"
        exit 1
    else
        git tag -a ${VER} -m "Release ${VER}"
        git push --tags
    fi
    ;;
*)
    echo "Tag should be \\d+.\\d+.\\d+"
    exit 1
    ;;
esac
