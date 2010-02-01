#
#	make-darwin-pybemacs.mak
#
PYCXX_VER=6.2.0
CCC=g++ -arch i386
CC=gcc -arch i386

PYTHON_FRAMEWORK=/Library/Frameworks/Python.framework/Versions/${PYTHON_VERSION}
PYTHON=$(PYTHON_FRAMEWORK)/Resources/Python.app/Contents/MacOS/Python

PYCXX=../Imports/pycxx-$(PYCXX_VER)
PYCXXSRC=../Imports/pycxx-$(PYCXX_VER)/Src
UCDDIR=../Imports/ucd

CCCFLAGS=-DPYBEMACS=1 \
-Wall -fPIC -fexceptions -frtti \
-I$(PYCXX) -I$(PYCXXSRC) -I$(PYTHON_FRAMEWORK)/include/python${PYTHON_VERSION} \
"-DOS_NAME=\"MacOSX\"" "-DUI_TYPE=\"python\"" \
-DNDEBUG -DDARWIN -DPYCXX_PYTHON_2TO3 \
-IInclude/pybemacs -IInclude/Common \
-g -O0

CCFLAGS=-Wall -fPIC -I$(PYCXX) -I$(PYCXX)/Src -I$(PYTHON_FRAMEWORK)/include/python${PYTHON_VERSION} -g -O0
LDSHARED=$(CCC) -bundle -g -u _PyMac_Error \
-framework System \
$(PYTHON_FRAMEWORK)/Python \
-framework CoreFoundation \
-framework Kerberos \
-framework Security
LDLIBS=

cc_flags =$(CCCFLAGS) -c
cpp = $(CCC) -ansi -pedantic -Wall -Wno-long-long -frtti
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

