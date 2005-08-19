#
#	BUild the example on FreeBSD for version 2.0
#
CCC=g++ -c
CCCFLAGS=-fPIC -I/usr/local/include/python2.0 -I.
LDSHARED=g++ -shared
LDLIBS=
PYTHON=/usr/local/bin/python2.0

include example_common.mak
