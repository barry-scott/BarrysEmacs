#
#	make-pybemacs-Linux-Fedora.mak
#
PYCXX_VER=6.2.4
CCC=g++
CC=gcc

PYTHON=/usr/bin/python

PYCXX=../Imports/pycxx-$(PYCXX_VER)
PYCXXSRC=../Imports/pycxx-$(PYCXX_VER)/Src
UCDDIR=../Imports/ucd

CCCFLAGS=-DPYBEMACS=1 -Wall -fPIC -fexceptions -frtti -I$(PYCXX) -I$(PYCXXSRC) -I/usr/include/python${PYTHON_VERSION} \
-DNDEBUG -DDARWIN -DPYCXX_PYTHON_2TO3 "-DOS_NAME=\"unix\"" "-DUI_TYPE=\"python\"" \
-g -O0
CCFLAGS=-Wall -fPIC -I$(PYCXX) -I$(PYCXX)/Src -I/usr/include/python${PYTHON_VERSION} -g -O0
LDSHARED=$(CCC) -shared $(CCCFLAGS)
LDLIBS=

cc_flags =$(CCCFLAGS) -D_DEBUG -IInclude/pybemacs -IInclude/Common -c
cpp = $(CCC) -ansi -pedantic -Wall -Wno-long-long -frtti -D__std_cpp__ -D__has_bool__
cc = $(CC) -ansi -pedantic -Wall=pthread
cppfilt=c++filt
ld_bintools=$(cpp)

#
#	main targets
#
all: pybemacs

install: install_pybemacs_images

# pull in the common bemacs make file
include make-common.mak
