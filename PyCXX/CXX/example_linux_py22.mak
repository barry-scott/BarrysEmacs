#
#	BUild the example on Linux for version 2.2a1
#
CCC=g++ -c
CCCFLAGS=-fPIC -I/usr/include/python2.2 -I.
LDSHARED=g++ -shared
LDLIBS=
PYTHON=python2.2

include example_common.mak
