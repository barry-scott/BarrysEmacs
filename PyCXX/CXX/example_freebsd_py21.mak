#
#	BUild the example on FreeBSD for version 2.1.1
#
CCC=g++ -c
CCCFLAGS=-fPIC -I/usr/local/include/python2.1 -I.
LDSHARED=g++ -shared
LDLIBS=
PYTHON=/usr/local/bin/python2.1

include example_common.mak
