// switchdl.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CSwitchDlg dialog

class CSwitchDlg : public CDialog
{
// Construction
public:
	CSwitchDlg(CWnd* pParent = NULL);	// standard constructor

// Dialog Data
	//{{AFX_DATA(CSwitchDlg)
	enum { IDD = IDD_SWITCH_BUFFER };
	CComboBox	m_buffer_combo;
	CString	m_buffer_name;
	//}}AFX_DATA

// Implementation
protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support

	virtual BOOL OnInitDialog();

	// Generated message map functions
	//{{AFX_MSG(CSwitchDlg)
	afx_msg void OnDblclkSwitchBuffer();
	virtual void OnOK();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};
