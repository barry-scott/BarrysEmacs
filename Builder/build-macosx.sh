#!/bin/bash
set -e

if ! which colour-print >/dev/null
then
    function colour-print {
        echo "$@"
    }
fi

colour-print "<>info Info:<> build-macosx.sh Builder - start"

colour-print "<>info Info:<> Checking for Python"

if [ "${PYTHON}" = "" ]
then
    colour-print "<>error Error: environment variable PYTHON not set<>"
    exit 1
fi

if ! which "${PYTHON}" >/dev/null
then
    colour-print "<>error Error: PYTHON program ${PYTHON} does not exist<>"
    exit 1
fi

if [ -e "/Volumes/Barry's Emacs" ]
then
    colour-print "<>info Info:<> unmount old kit dmg"
    umount "/Volumes/Barry's Emacs"
fi

if [ "${HOMEBREW_PREFIX}" = "" ]
then
    colour-print "<>error Error: HOMEBREW_PREFIX is not defined<> Hint run brew shellenv"
    exit 1
fi

# make sure that libssh is installed
brew install libssh

./build-venv.sh 2>&1 | ${PYTHON} -u build_tee.py build.log

export PYTHON=${PWD}/venv.tmp/bin/python
${PYTHON} ./build_bemacs.py gui --colour | ${PYTHON} -u build_tee.py -a build.log

if false
then
    colour-print "<>info Info:<> rebuild the OS X launch service database"
    CORE=/System/Library/Frameworks/CoreServices.framework/Versions/Current
    ${CORE}/Frameworks/LaunchServices.framework/Versions/Current/Support/lsregister \
     -kill \
     -r -domain local -domain system -domain user
fi

DMG="$( ls -1 tmp/dmg/*.dmg )"
colour-print "<>info Info:<> DMG ${DMG}"
if [ "$1" = "--install" ]
then
    # macOS knows about these extra copies of the emacs app
    # and will start all of them at the same time so delete
    rm -rf tmp/pkg
    rm -rf tmp/dmg/*.app
    open ${DMG}
fi
colour-print "<>info Info:<> build-macosx.sh Builder - end"
