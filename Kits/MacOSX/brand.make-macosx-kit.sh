#!/bin/bash
set -e

printf "\033[32mInfo:\033[m brand.make-macosx-kit.sh Kits/MacOSX - start\n"
PKGNAME=BarrysEmacs-%(version)s
rm -rf tmp
mkdir tmp
mkdir tmp/${PKGNAME}

printf "\033[32mInfo:\033[m copy app\n"
cp -r "pkg/Barry's Emacs-Devel.app" "tmp/${PKGNAME}/Barry's Emacs.app"
mv \
    "tmp/${PKGNAME}/Barry's Emacs.app/Contents/MacOS/Barry's Emacs-Devel" \
    "tmp/${PKGNAME}/Barry's Emacs.app/Contents/MacOS/Barry's Emacs"

printf "\033[32mInfo:\033[m Create DMG\n"
# use 2.7 version as 3.5 version does not work yet (confuses bytes and str)
/Library/Frameworks/Python.framework/Versions/2.7/bin/dmgbuild \
    --settings dmg_build_settings.py  \
    "Barry's Emacs" \
    tmp/${PKGNAME}.dmg

printf "\033[32mInfo:\033[m brand.make-macosx-kit.sh Kits/MacOSX - end\n"
