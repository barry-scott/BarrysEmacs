#!/bin/bash
KITNAME=barrys-emacs-src-%(version)s

cd ..
git archive --format=tar --prefix=BarrysEmacs-%(version)s/ master >${TMPDIR:-/tmp}/${KITNAME}.tar

ls -l ${TMPDIR:-/tmp}/${KITNAME}.tar
