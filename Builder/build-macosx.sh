#!/bin/sh
echo "Info: build-macosx.sh Builder - start"
make -f macosx.mak PYTHON=python2.6 clean build
echo "Info: build-macosx.sh Builder - end"
