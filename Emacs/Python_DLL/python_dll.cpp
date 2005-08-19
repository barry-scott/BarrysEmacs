// python_dll.cpp
#include <em_user.h>

#ifdef _DEBUG
	#undef _DEBUG
	#include <Python.h>
	#define _DEBUG
#else
	#include <Python.h>
#endif

extern "C++"
	{
	__declspec( dllexport ) EmacsCallBackStatus bemacs_python( EmacsCallBackInterface &call_back );
	__declspec( dllexport ) EmacsCallBackStatus bemacs_python2( EmacsCallBackInterface &call_back );
	};

__declspec( dllexport ) EmacsCallBackStatus bemacs_python( EmacsCallBackInterface &bemacs )
	{
	if( bemacs.checkArgs( 1, 1 ) != EMACS__K_SUCCESS )
		return EMACS__K_SUCCESS;

	char *command = NULL;
	if( bemacs.stringArg( 1, &command ) != EMACS__K_SUCCESS )
		return EMACS__K_SUCCESS;

	if( !Py_IsInitialized() )
		Py_Initialize();

	PyObject *m = PyImport_AddModule("__main__");
	if (m == NULL)
		return EMACS__K_SUCCESS;

	PyObject *d = PyModule_GetDict(m);
	PyObject *v = PyRun_String(command, Py_file_input, d, d);
	if (v == NULL)
		{
		PyErr_Print();
		return EMACS__K_SUCCESS;
		}

	PyObject *result = PyObject_Str( v );

	char *string = PyString_AsString( result );
	bemacs.stringResult( string );

	Py_DECREF(result);
	Py_DECREF(v);
	if (Py_FlushLine())
		PyErr_Clear();

	return EMACS__K_SUCCESS;
	}

__declspec( dllexport ) EmacsCallBackStatus bemacs_python2( EmacsCallBackInterface &call_back )
	{
	call_back.stringResult( "This is bemacs_python2" );
	return EMACS__K_SUCCESS;
	}
