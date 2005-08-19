// win_select_buffer.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CBufferSelectDlg dialog

class CBufferSelectDlg : public CDialog
{
// Construction
public:
	CBufferSelectDlg(CWnd* pParent = NULL);	// standard constructor

// Dialog Data
	//{{AFX_DATA(CBufferSelectDlg)
	enum { IDD = IDD_SWITCH_BUFFER };
	CComboBox	m_buffer_combo;
	CString	m_buffer_name;
	//}}AFX_DATA

	void InitParameters( const EmacsString &title, const EmacsString &buffer_name, const EmacsString &buffer_types );

// Implementation
protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support

	virtual BOOL OnInitDialog();

	// Generated message map functions
	//{{AFX_MSG(CBufferSelectDlg)
	afx_msg void OnDblclkSwitchBuffer();
	virtual void OnOK();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

	EmacsString dialog_title;
	int buffer_types_mask;
};
