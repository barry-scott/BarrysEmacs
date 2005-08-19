// em_user_dll.cpp
#include <em_user.h>


//
//	Define a pair of C functions
//
extern "C"
	{
	__declspec( dllexport )
	int ext_3_test( int (*call_back)(int,...) );
	__declspec( dllexport )
	int ext_4_test( int (*call_back)(int,...) );
	};

__declspec( dllexport )
int ext_3_test( int (*call_back)(int,...) )
	{
	call_back( EMACS__K_STRING_RESULT, "This is ext_3_test" );
	return EMACS__K_SUCCESS;
	}

__declspec( dllexport )
int ext_4_test( int (*call_back)(int,...) )
	{
	call_back( EMACS__K_STRING_RESULT, "This is ext_4_test" );
	return EMACS__K_SUCCESS;
	}


//
//	Define a pair of C++ Functions
//
__declspec( dllexport )
EmacsCallBackStatus ext_5_test( EmacsCallBackInterface &iEmacs )
	{
	iEmacs.stringResult( "This is ext_5_test" );
	return EMACS__K_SUCCESS;
	}

__declspec( dllexport )
EmacsCallBackStatus ext_6_test( EmacsCallBackInterface &iEmacs )
	{
	iEmacs.stringResult( "This is ext_6_test" );
	return EMACS__K_SUCCESS;
	}
