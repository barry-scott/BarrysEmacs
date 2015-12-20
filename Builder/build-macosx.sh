#!/bin/bash
echo "Info: build-macosx.sh Builder - start"
make -f macosx.mak PYTHON=$PYTHON clean build 2>&1 | tee build.log
echo "Info: build-macosx.sh Builder - end"
