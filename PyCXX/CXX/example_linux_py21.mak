#
#	BUild the example on Linux for version 2.1.1
#
CCC=g++ -c
CCCFLAGS=-fPIC -I/usr/include/python2.1 -I.
LDSHARED=g++ -shared
LDLIBS=
PYTHON=python2

include example_common.mak
