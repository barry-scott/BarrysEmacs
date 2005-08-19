#
#	example_common.mak
#
#	include this mak file after defining the variables it needs
#

OBJECTS=example.o range.o rangetest.o cxxsupport.o cxx_extensions.o cxxextensions.o IndirectPythonInterface.o

all: example.so

example.so: $(OBJECTS)
	$(LDSHARED) -o $@ $(OBJECTS) $(LDLIBS)

example.o: Demo/example.cxx
	$(CCC) $(CCCFLAGS) -o $@ $<

range.o: Demo/range.cxx
	$(CCC) $(CCCFLAGS) -o $@ $<

rangetest.o: Demo/rangetest.cxx
	$(CCC) $(CCCFLAGS) -o $@ $<

cxxsupport.o: Src/cxxsupport.cxx
	$(CCC) $(CCCFLAGS) -o $@ $<

cxx_extensions.o: Src/cxx_extensions.cxx
	$(CCC) $(CCCFLAGS) -o $@ $<

cxxextensions.o: Src/cxxextensions.c
	$(CC) -c $(CCCFLAGS) -o $@ $<

IndirectPythonInterface.o: Src/IndirectPythonInterface.cxx
	$(CCC) $(CCCFLAGS) -o $@ $< 

clean:
	rm -f *.o
	rm -f example.so

test: example.so
	$(PYTHON) -c "import example;example.test()"
