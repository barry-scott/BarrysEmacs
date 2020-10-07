#!/bin/bash

# hunspell source code:
#   https://github.com/hunspell/hunspell/tags
#
# dictionaries for hunspell:
#   https://sourceforge.net/projects/wordlist/files/speller/

if [ "${#}" -lt 2 ]
then
    colour-print "<>error Error:<> too few args"
    echo "Example: ./import-hunspell.sh /Volumes/Downloads/BEmacs/dependencies/hunspell-1.7.0.tar.gz /Volumes/Downloads/BEmacs/dependencies/*.zip"
    exit 1
fi

colour-print "<>info Info:<> install hunspell source"
rm -rf ../Imports/hunspell-*
mkdir -p ../Imports
tar -xf "${1? hunspell tar ball}" -C ../Imports

cd ../Imports
shift

ln -sf hunspell-* hunspell
cd hunspell
rm -rf tmp.dict
mkdir tmp.dict

colour-print "<>info Info:<> installing dictionary kits"
for DICT in "$@"
do
    unzip -q ${DICT} -d tmp.dict
    for FILE in tmp.dict/*.dic tmp.dict/*.aff
    do
        if [ -e "$FILE" ]
        then
            NEWFILE=${FILE/-large/}
            mv "$FILE" "$NEWFILE"
            colour-print "<>info Info:<> Dictionary $PWD/$NEWFILE"
        fi
    done
done

