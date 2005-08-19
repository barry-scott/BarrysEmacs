#
#	BUild the example on Linux for version 2.0
#
CCC=g++ -c
CCCFLAGS=-fPIC -I/usr/include/python2.0 -I.
LDSHARED=g++ -shared
LDLIBS=
PYTHON=python2

include example_common.mak
