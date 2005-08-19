/* Minimal main program -- everything is loaded from the library */

#include "Python.h"
#include <iostream>

extern "C" int Py_Main(int argc, char** argv);
extern "C" void initexample();
extern "C" void Py_Initialize();
int
main(int argc, char** argv)
	{
	std::cout << "Greetings. Type from example import *; test()" << std::endl;
	Py_Initialize();
	initexample();
	return Py_Main(argc, argv);
	}
