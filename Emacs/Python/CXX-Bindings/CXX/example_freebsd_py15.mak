#
#	BUild the example on FreeBSD for version 1.5.2
#
CCC=g++ -c
CCCFLAGS=-fPIC -I/usr/local/include/python1.5 -I.
LDSHARED=g++ -shared
LDLIBS=
PYTHON=/usr/local/bin/python1.5

include example_common.mak
