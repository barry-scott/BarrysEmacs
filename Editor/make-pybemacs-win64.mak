#
#	make-pybemacs-win64.mak
#
PYCXX_VER=6.2.6
CCC=cl
CC=cl

PYCXX=$(BUILDER_TOP_DIR)/Imports/pycxx-$(PYCXX_VER)
PYCXXSRC=$(BUILDER_TOP_DIR)/Imports/pycxx-$(PYCXX_VER)/Src
UCDDIR=$(BUILDER_TOP_DIR)/Imports/ucd

CCCFLAGS=-DPYBEMACS=1 -I$(PYCXX) -I$(PYCXXSRC) -I$(PYTHON_FRAMEWORK)/include/python$(PYTHON_VERSION) \
"-DOS_NAME=\"win32\"" "-DUI_TYPE=\"python\"" \
-DNDEBUG -DPYCXX_PYTHON_2TO3 \
/Zi

CCFLAGS=/W4 -I$(PYCXX) -I$(PYCXX)/Src -I$(PYTHON_FRAMEWORK)/include/python$(PYTHON_VERSION) /Zi
LDSHARED=$(CCC) /Zi
LDLIBS=

cc_flags =$(CCCFLAGS) -D_DEBUG -IInclude/pybemacs -IInclude/Common -c
cpp = $(CCC) /W4
cc = $(CC) /W4
cppfilt=type
ld_bintools=$(cpp)

#
#	main targets
#
all: pybemacs

install: install_pybemacs_images

# pull in the common bemacs make file
!include make-common-win64.mak

