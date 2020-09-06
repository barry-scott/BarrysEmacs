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

colour-print "<>info Info:<> installing dictionary kits"
for DICT in "$@"
do
    cp -v "${DICT}" .
done

ln -sf hunspell-* hunspell
cd hunspell

colour-print "<>info Info:<> build hunspell"
autoreconf -vfi
./configure
make -j
