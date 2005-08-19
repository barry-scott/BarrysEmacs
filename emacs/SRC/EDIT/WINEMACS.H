// winemacs.h : main header file for the WINEMACS application
//

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include <resource.h>		// main symbols

#if !defined(_NT)
extern int GetLastError(void);
#endif

extern void win_check_for_activity(struct trmcontrol *);

/////////////////////////////////////////////////////////////////////////////
// CWinemacsApp:
// See winemacs.cpp for the implementation of this class
//

class CWinemacsApp : public CWinApp
{
public:
	CWinemacsApp();

// Overrides
	virtual BOOL InitInstance();

	// we will override run to cause the emacs editor to be started
	virtual int Run();

	virtual CDocument* OpenDocumentFile(LPCSTR lpszFileName);

	// and provide process_message to do the work that run would do
	int process_message(int can_wait);

	void emacs_new_document(void);

	enum { max_argv = 30 };
	int argc;
	unsigned char *argv[max_argv+1];

	virtual BOOL PreTranslateMessage( MSG * );
	virtual int ExitInstance();
	virtual BOOL OnDDECommand(char* pszCommand);

	HCURSOR normal_text_cursor;
	HCURSOR busy_cursor;

	unsigned char restore_arg[256];
	unsigned char package_arg[256];

// Implementation

	//{{AFX_MSG(CWinemacsApp)
	afx_msg void OnAppAbout();
	afx_msg void OnOptionsPrinting();
	afx_msg void OnUpdateOptionsPrinting(CCmdUI* pCmdUI);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

extern class CWinemacsApp theApp;

/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
// CFatalError dialog

class CFatalError : public CDialog
{
// Construction
public:
	CFatalError(CWnd* pParent = NULL);	// standard constructor

// Dialog Data
	//{{AFX_DATA(CFatalError)
	enum { IDD = IDD_ERROR };
	CStatic	m_error_code;
	//}}AFX_DATA

// Implementation
protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support

	// Generated message map functions
	//{{AFX_MSG(CFatalError)
		// NOTE: the ClassWizard will add member functions here
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};
/////////////////////////////////////////////////////////////////////////////
// CYesOrNo dialog

class CYesOrNo : public CDialog
{
// Construction
public:
	CYesOrNo(CWnd* pParent = NULL);	// standard constructor

// Dialog Data
	//{{AFX_DATA(CYesOrNo)
	enum { IDD = IDD_YESORNO };
	CStatic	m_question;
	//}}AFX_DATA

// Implementation
protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support

	// Generated message map functions
	//{{AFX_MSG(CYesOrNo)
		// NOTE: the ClassWizard will add member functions here
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};
