#!/bin/sh
(pushd ~/wc/svn/Emacs/Editor && ./build && popd && ./bemacs.sh  --noredirect  )2>&1 | tee a.log
