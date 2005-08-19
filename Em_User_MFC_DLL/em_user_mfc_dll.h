// em_user_mfc_dll.h : main header file for the EM_USER_MFC_DLL DLL
//

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"		// main symbols

/////////////////////////////////////////////////////////////////////////////
// CEm_user_mfc_dllApp
// See em_user_mfc_dll.cpp for the implementation of this class
//

class CEm_user_mfc_dllApp : public CWinApp
{
public:
	CEm_user_mfc_dllApp();

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CEm_user_mfc_dllApp)
	//}}AFX_VIRTUAL

	//{{AFX_MSG(CEm_user_mfc_dllApp)
		// NOTE - the ClassWizard will add and remove member functions here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};


/////////////////////////////////////////////////////////////////////////////
