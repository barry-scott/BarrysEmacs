#!/bin/bash

# hunspell source code:
#   https://github.com/hunspell/hunspell/tags
#
# dictionaries for hunspell:
#   https://sourceforge.net/projects/wordlist/files/speller/

colour-print "<>info Info:<> install hunspell source"
rm -rf ../Imports/hunspell-*
mkdir -p ../Imports
tar -xf "${1? hunspell tar ball}" -C ../Imports

cd ../Imports
shift

ln -sf hunspell-* hunspell
cd hunspell

colour-print "<>info Info:<> installing dictionary kits"
for DICT in "$@"
do
    rm -rf tmp.dict
    mkdir tmp.dict
    unzip -q ${DICT} -d tmp.dict
    for FILE in tmp.dict/*.dic tmp.dict/*.aff
    do
        if [ -e "$FILE" ]
        then
            colour-print "<>info Info:<> Dictionary $FILE"
            NEWFILE=$(basename $FILE)
            NEWFILE=${NEWFILE/-large/}
        fi
    done
done
