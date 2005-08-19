// win_opt.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CDirOpt dialog

class CDirOpt : public CDialog
{
// Construction
public:
	CDirOpt(CWnd* pParent = NULL);	// standard constructor

// Dialog Data
	//{{AFX_DATA(CDirOpt)
	enum { IDD = IDD_OPTION_DIRECTORIES };
	CString	m_emacs_journal;
	CString	m_emacs_library;
	CString	m_emacs_local_library;
	CString	m_emacs_user_library;
	CString	m_sys_login;
	CString	m_sys_scratch;
	CString	m_emacs_path;
	CString	m_emacs_checkpoint;
	CString	m_emacs_memory;
	//}}AFX_DATA

	void save_options();

// Implementation
protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support

	void set_emacs_path();

	void OnClickedOptdirBrowse( CString &str );
	// Generated message map functions
	//{{AFX_MSG(CDirOpt)
	afx_msg void OnClickedOptdirBrowse1();
	afx_msg void OnClickedOptdirBrowse2();
	afx_msg void OnClickedOptdirBrowse3();
	afx_msg void OnClickedOptdirBrowse4();
	afx_msg void OnClickedOptdirBrowse5();
	afx_msg void OnClickedOptdirBrowse6();
	virtual void OnOK();
	afx_msg void OnClickedOptdirBrowse7();
	afx_msg void OnClickedOptdirBrowse8();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};
/////////////////////////////////////////////////////////////////////////////
// CColourOpt dialog

#define COLOR_ITEM_MODE_LINE		0
#define COLOR_ITEM_NORMAL_TEXT		1
#define COLOR_ITEM_HIGHLIGHT_TEXT	2
#define	COLOR_ITEM_SYNTAX_KEYWORD1	3
#define	COLOR_ITEM_SYNTAX_KEYWORD2	4
#define	COLOR_ITEM_SYNTAX_WORD		5
#define	COLOR_ITEM_SYNTAX_STRING	6
#define	COLOR_ITEM_SYNTAX_QUOTE		7
#define	COLOR_ITEM_SYNTAX_COMMENT	8
#define COLOR_ITEM_USER_1		9
#define COLOR_ITEM_USER_2		10
#define COLOR_ITEM_USER_3		11
#define COLOR_ITEM_USER_4		12
#define COLOR_ITEM_USER_5		13
#define COLOR_ITEM_USER_6		14
#define COLOR_ITEM_USER_7		15
#define COLOR_ITEM_USER_8		16
#define COLOR_ITEM_MAX			17

class CColourOpt : public CDialog
{
// Construction
public:
	CColourOpt(CWnd* pParent = NULL);	// standard constructor

	void save_options();

	int cur_item;

// Dialog Data
	//{{AFX_DATA(CColourOpt)
	enum { IDD = IDD_OPTION_COLOURS };
	CStatic	m_sample_text;
	CListBox	m_item_list;
	BOOL	m_italic;
	BOOL	m_underline;
	//}}AFX_DATA

	C_color_attr colours[COLOR_ITEM_MAX];

// Implementation
protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support

	virtual BOOL OnInitDialog();

	// Generated message map functions
	//{{AFX_MSG(CColourOpt)
	virtual void OnOK();
	afx_msg void OnClickedOptcolBackground();
	afx_msg void OnClickedOptcolForeground();
	afx_msg void OnSelchangeOptcolItemlist();
	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};
/////////////////////////////////////////////////////////////////////////////
// CPrintOpt dialog

class CPrintOpt : public CDialog
{
// Construction
public:
	CPrintOpt(CWnd* pParent = NULL);	// standard constructor

	void save_options();
	void load_options();

// Dialog Data
	//{{AFX_DATA(CPrintOpt)
	enum { IDD = IDD_OPTION_PRINTER };
	CString	m_header_format;
	CString	m_font_name;
	int		m_font_size;
	CString	m_footer_format;
	float	m_bottom_margin;
	float	m_left_margin;
	float	m_right_margin;
	float	m_top_margin;
	//}}AFX_DATA
	double	twips_scaling;

// Implementation
protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support

	// Generated message map functions
	//{{AFX_MSG(CPrintOpt)
	virtual void OnOK();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};
/////////////////////////////////////////////////////////////////////////////
// COptionFilenameParsing dialog

class COptionFilenameParsing : public CDialog
{
// Construction
public:
	COptionFilenameParsing(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(COptionFilenameParsing)
	enum { IDD = IDD_OPTION_FILENAME_PARSING };
	//}}AFX_DATA
	int		m_disk[26];	// holds A: to Z: state

	void save_options(void);

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(COptionFilenameParsing)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(COptionFilenameParsing)
		// NOTE: the ClassWizard will add member functions here
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};
