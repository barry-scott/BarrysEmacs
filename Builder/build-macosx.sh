#!/bin/sh
echo "Info: build-macosx.sh Builder - start"
make -f macosx.mak PYTHON=$PYTHON clean build
echo "Info: build-macosx.sh Builder - end"
