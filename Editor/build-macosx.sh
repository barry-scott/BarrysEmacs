#!/bin/bash
echo "Info: build-macosx.sh Editor - start"
${PYTHON} setup.py macosx Makefile
make -j 8 -f Makefile "$@"
echo "Info: build-macosx.sh Editor - end"
