#!/bin/bash

VER=${1?version}

if svn ls "http://liara/svn/barrys-sources/tags/Emacs/${VER}" >/dev/null 2>&1
then
    echo Error: tag already exists for ${VER}
    exit 1
fi

svn cp $2 \
    "http://liara/svn/barrys-sources/trunk/Emacs" \
    "http://liara/svn/barrys-sources/tags/Emacs/${VER}" \
    -m "Release ${VER}"
