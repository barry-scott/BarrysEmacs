// win_emacs.h : main header file for the win_emacs application
//

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include <afxcmn.h>

#include <resource.h>		// main symbols

#include <win_list_ctrl_ex.h>

#if !defined(_NT)
extern int GetLastError(void);
#endif






/////////////////////////////////////////////////////////////////////////////
// CWinemacsApp:
// See win_emacs.cpp for the implementation of this class
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

	bool wait_for_command_line(void);

	void emacs_new_document(void);

	virtual BOOL PreTranslateMessage( MSG * );
	virtual int ExitInstance();
	virtual BOOL OnDDECommand(char* pszCommand);

	afx_msg void OnEmacsHelp();

	void GetEmacsPrinterSetup();
	bool SaveEmacsPrinterSetup();

	void OnEmacsFilePrintSetup();

	bool CreateEmacsPrinterDC( CDC &print_dc );

	COleTemplateServer m_server;

	HCURSOR normal_text_cursor;
	HCURSOR busy_cursor;

	EmacsString restore_arg;
	EmacsString package_arg;

	bool qual_embedded;
	bool qual_automation;
	bool qual_regserver;
	bool qual_unregserver;

	bool m_tick;

	// Implementation

	//{{AFX_MSG(CWinemacsApp)
	afx_msg void OnAppAbout();
	afx_msg void OnOptionsPrinting();
	afx_msg void OnUpdateOptionsPrinting(CCmdUI* pCmdUI);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

	//--ATL Support------------------------------------------------------------------------------
	static unsigned __stdcall atl_thread_mainline( void *parm );

	HANDLE atl_thread_handle;
	unsigned atl_thread_id;

	//--ATL Support------------------------------------------------------------------------------

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
	CString	m_error_code;
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
// CQuitEmacs dialog

class CQuitEmacs : public CDialog
{
// Construction
public:
	CQuitEmacs(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CQuitEmacs)
	enum { IDD = IDD_QUIT };
		// NOTE: the ClassWizard will add data members here
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CQuitEmacs)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CQuitEmacs)
		// NOTE: the ClassWizard will add member functions here
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};
/////////////////////////////////////////////////////////////////////////////
// CSaveBeforeQuit dialog

class CSaveBeforeQuit : public CDialog
{
// Construction
public:
	CSaveBeforeQuit(CWnd* pParent = NULL);   // standard constructor

	virtual BOOL OnInitDialog();

// Dialog Data
	//{{AFX_DATA(CSaveBeforeQuit)
	enum { IDD = IDD_SAVE_BEFORE_QUIT };
	CListCtrlEx	m_list;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CSaveBeforeQuit)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CSaveBeforeQuit)
	afx_msg void OnQuitNoSave();
	afx_msg void OnQuitSaveAll();
	afx_msg void OnQuitSaveInteractive();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};
/////////////////////////////////////////////////////////////////////////////
// CSaveQuestion dialog

class CSaveQuestion : public CDialog
{
// Construction
public:
	CSaveQuestion(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CSaveQuestion)
	enum { IDD = IDD_SAVE_QUESTION };
	CString	m_buffer_name;
	CString	m_file_name;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CSaveQuestion)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CSaveQuestion)
	afx_msg void OnNoSave();
	afx_msg void OnSaveAsFile();
	afx_msg void OnSaveFile();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};
