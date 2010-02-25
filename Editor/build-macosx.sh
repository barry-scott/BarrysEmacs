#!/bin/sh
${PYTHON} setup.py macosx Makefile
make -f Makefile "$@"
