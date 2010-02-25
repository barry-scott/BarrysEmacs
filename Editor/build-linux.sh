#!/bin/sh
${PYTHON} setup.py linux Makefile
make -f Makefile "$@"
