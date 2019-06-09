#!/bin/bash
set -e

printf "\033[32mInfo:\033[m build-macosx.sh Builder - start\n"

printf "\033[32mInfo:\033[m Checking for Python\n"

if [ "${PYTHON}" = "" ]
then
    printf -e "\033[31;1mError: environment variable PYTHON not set\033[m\n"
    exit 1
fi

if ! which "${PYTHON}" >/dev/null
then
    printf "Error: PYTHON program ${PYTHON} does not exist\n"
    exit 1
fi

if [ -e "/Volumes/Barry's Emacs" ]
then
    printf "\033[32mInfo:\033[m unmount old kit dmg\n"
    umount "/Volumes/Barry's Emacs"
fi

${PYTHON} ./build_bemacs.py gui --colour

if false
then
    printf "\033[32mInfo:\033[m rebuild the OS X launch service database\n"
    CORE=/System/Library/Frameworks/CoreServices.framework/Versions/Current
    ${CORE}/Frameworks/LaunchServices.framework/Versions/Current/Support/lsregister \
     -kill \
     -r -domain local -domain system -domain user
fi

DMG="$( ls -1 tmp/dmg/*.dmg )"
printf "\033[32mInfo:\033[m DMG ${DMG}\n"
if [ "$1" = "--install" ]
then
    # macOS knows about these extra copies of the emacs app
    # and will start all of them at the same time so delete
    rm -rf tmp/pkg
    rm -rf tmp/dmg/*.app
    open ${DMG}
fi
printf "\033[32mInfo:\033[m build-macosx.sh Builder - end\n"
