// em_user_mfc_dll.cpp : Defines the initialization routines for the DLL.
//

#include "stdafx.h"
#include "em_user_mfc_dll.h"
#include <em_user.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CEm_user_mfc_dllApp

BEGIN_MESSAGE_MAP(CEm_user_mfc_dllApp, CWinApp)
	//{{AFX_MSG_MAP(CEm_user_mfc_dllApp)
		// NOTE - the ClassWizard will add and remove mapping macros here.
		//    DO NOT EDIT what you see in these blocks of generated code!
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CEm_user_mfc_dllApp construction

CEm_user_mfc_dllApp::CEm_user_mfc_dllApp()
{
	// TODO: add construction code here,
	// Place all significant initialization in InitInstance
}

/////////////////////////////////////////////////////////////////////////////
// The one and only CEm_user_mfc_dllApp object

CEm_user_mfc_dllApp theApp;


// int (*ef_function)(void *context, int (*call_back)(int,...), int restore_count);


extern "C"
	{
	__declspec( dllexport )
	int ext_1_test( void *, int (*call_back)(int,...), int );
	__declspec( dllexport )
	int ext_2_test( void *, int (*call_back)(int,...), int );
	};

__declspec( dllexport )
int ext_1_test( void *, int (*call_back)(int,...), int )
	{
	call_back( EMACS__K_STRING_RESULT, "This is ext_1_test" );
	return 1;
	}

__declspec( dllexport )
int ext_2_test( void *, int (*call_back)(int,...), int )
	{
	call_back( EMACS__K_STRING_RESULT, "This is ext_2_test" );
	return 1;
	}

