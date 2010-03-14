#!/bin/sh
echo "Info: build-macosx.sh Editor - start"
${PYTHON} setup.py macosx Makefile
make -f Makefile "$@"
echo "Info: build-macosx.sh Editor - end"
