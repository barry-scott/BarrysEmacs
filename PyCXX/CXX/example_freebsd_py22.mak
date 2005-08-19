#
#	BUild the example on FreeBSD for version 2.2a1
#
CCC=g++ -c
CCCFLAGS=-fPIC -I/usr/local/include/python2.2 -I.
LDSHARED=g++ -shared
LDLIBS=
PYTHON=/usr/local/bin/python2.2

include example_common.mak
