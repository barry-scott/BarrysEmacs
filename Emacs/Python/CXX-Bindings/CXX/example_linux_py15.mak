#
#	BUild the example on Linux for version 1.5.2
#
CCC=g++ -c
CCCFLAGS=-fPIC -I/usr/include/python1.5 -I.
LDSHARED=g++ -shared
LDLIBS=
PYTHON=python

include example_common.mak
