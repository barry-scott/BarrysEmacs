#!/bin/sh
(pushd ~/wc/svn/Emacs/Editor && ./build.sh && popd && ./bemacs.sh "$@" )2>&1 | tee a.log
