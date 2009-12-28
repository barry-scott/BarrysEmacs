#
#	make-darwin-pybemacs.mak
#
all: $(edit_obj)_bemacs.so
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

CXX_OBJECTS=$(edit_obj)cxxsupport.o \
$(edit_obj)cxx_extensions.o \
$(edit_obj)cxxextensions.o \
$(edit_obj)IndirectPythonInterface.o

os_specific_obj_files=$(edit_obj)pybemacs.o $(edit_obj)bemacs_python.o $(CXX_OBJECTS)

# pull in the common bemacs make file
include make-common.mak

all: $(edit_obj)_bemacs.so

$(edit_obj)_bemacs.so: $(obj_files)
	@echo Compile $@
	$(LDSHARED) -o $@ $(obj_files) $(LDLIBS)

$(edit_obj)pybemacs.o: Source/pybemacs/pybemacs.cpp Source/pybemacs/bemacs_python.hpp
	@echo Compile $@
	$(cpp) $(cc_flags) -o $@ $<

$(edit_obj)bemacs_python.o: Source/pybemacs/bemacs_python.cpp Source/pybemacs/bemacs_python.hpp
	@echo Compile $@
	$(cpp) $(cc_flags) -o $@ $<

$(edit_obj)cxxsupport.o: $(PYCXXSRC)/cxxsupport.cxx
	@echo Compile $@
	$(CCC) -c $(CCCFLAGS) -o $@ $<

$(edit_obj)cxx_extensions.o: $(PYCXXSRC)/cxx_extensions.cxx
	@echo Compile $@
	$(CCC) -c $(CCCFLAGS) -o $@ $<

$(edit_obj)cxxextensions.o: $(PYCXXSRC)/cxxextensions.c
	@echo Compile $@
	$(CC) -c $(CCFLAGS) -o $@ $<

$(edit_obj)IndirectPythonInterface.o: $(PYCXXSRC)/IndirectPythonInterface.cxx
	@echo Compile $@
	$(CCC) -c $(CCCFLAGS) -o $@ $< 

clean::
	rm -f $(edit_exe)_bemacs.so
