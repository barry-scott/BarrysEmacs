#!/bin/sh
(pushd ~/wc/svn/Emacs/Editor && ./build && popd && ./bemacs.sh "$@" )2>&1 | tee a.log
