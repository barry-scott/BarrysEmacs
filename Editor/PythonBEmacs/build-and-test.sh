#!/bin/bash
(pushd ~/wc/svn/Emacs/Editor && ./build.sh && popd && ./bemacs.sh "$@" )2>&1 | tee a.log
