#
#	make-darwin-pybemacs.mak
#
PYCXX_VER=6.2.0
CCC=g++ -arch i386
CC=gcc -arch i386

PYTHON_FRAMEWORK=/Library/Frameworks/Python.framework/Versions/2.6
PYTHON=$(PYTHON_FRAMEWORK)/Resources/Python.app/Contents/MacOS/Python

PYCXX=../Imports/pycxx-$(PYCXX_VER)
PYCXXSRC=../Imports/pycxx-$(PYCXX_VER)/Src


CCCFLAGS=-DPYBEMACS=1 -Wall -fPIC -fexceptions -frtti -I$(PYCXX) -I$(PYCXXSRC) -I$(PYTHON_FRAMEWORK)/include/python2.6 -DNDEBUG -DDARWIN -DPYCXX_PYTHON_2TO3 -g -O0
CCFLAGS=-Wall -fPIC -I$(PYCXX) -I$(PYCXX)/Src -I$(PYTHON_FRAMEWORK)/include/python2.6 -g -O0
LDSHARED=$(CCC) -bundle -g -u _PyMac_Error -framework System $(PYTHON_FRAMEWORK)/Python -framework CoreFoundation -framework Kerberos -framework Security
LDLIBS=

#
#	bemacs
#
cc_flags =$(CCCFLAGS) -D_DEBUG -IInclude/pybemacs -IInclude/Common -c
cpp = $(CCC) -ansi -pedantic -Wall -Wno-long-long -frtti -D__std_cpp__ -D__has_bool__ -DUSING_CARBON
cc = $(CC) -ansi -pedantic -Wall=pthread
cppfilt=c++filt
ld_bintools=$(cpp)

# pull in the common bemacs make file
include make-common.mak

all: $(edit_obj)_bemacs.so bintools

install: install_pybemacs_images
