// finddlg.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CFindDlg dialog

class CFindDlg : public CDialog
{
// Construction
public:
	CFindDlg(CWnd* pParent = NULL, UINT id = IDD_FIND);	// standard constructor

// Dialog Data
	//{{AFX_DATA(CFindDlg)
	enum { IDD = IDD_FIND };
	CString	m_search_for;
	BOOL	m_re_search;
	BOOL	m_match_case;
	//}}AFX_DATA

	BOOL	m_string_found;
	int	m_search_direction;
	BOOL CreateModeless( UINT id );
// Implementation
protected:
	virtual BOOL OnInitDialog();
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support

	void FindString(void);

	// Generated message map functions
	//{{AFX_MSG(CFindDlg)
	virtual void OnCancel();
	afx_msg void OnClickedFindNext();
	afx_msg void OnClickedFindPrev();
	afx_msg void OnClickedFindMatchCase();
	afx_msg void OnClickedFindReSearch();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

private:
	void PostNcDestroy();
};

/////////////////////////////////////////////////////////////////////////////
// CFindReplaceDlg dialog

class CFindReplaceDlg : public CFindDlg
{
// Construction
public:
	CFindReplaceDlg(CWnd* pParent = NULL, UINT id = IDD_REPLACE);	// standard constructor

// Dialog Data
	//{{AFX_DATA(CFindReplaceDlg)
	enum { IDD = IDD_REPLACE };
	CButton	m_replace_all;
	CButton	m_replace;
	CString	m_replace_with;
	//}}AFX_DATA

// Implementation
protected:
	void ReplaceButtons();

	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support

	// Generated message map functions
	//{{AFX_MSG(CFindReplaceDlg)
	virtual void OnCancel();
	afx_msg void OnClickedFindNext();
	afx_msg void OnClickedFindPrev();
	afx_msg void OnClickedFindReSearch();
	afx_msg void OnClickedFindReplace();
	afx_msg void OnClickedFindReplaceAll();
	afx_msg void OnClickedFindMatchCase();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()                 

	void ReplaceString(void);

private:
	void PostNcDestroy();
};

extern CFindDlg *find_dlg;
