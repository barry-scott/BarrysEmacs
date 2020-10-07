// win_opt.h : header file
//
#include <afxcmn.h>

#include <win_list_ctrl_ex.h>

class CMainFrame;    // forward declaration

class CColorComboBox : public CComboBox
{
public:
// Operations
    void AddColorItem(COLORREF color);
    bool SetCurSelToColour(COLORREF colour);

// Implementation
    virtual void MeasureItem(LPMEASUREITEMSTRUCT lpMIS);
    virtual void DrawItem(LPDRAWITEMSTRUCT lpDIS);
    virtual int CompareItem(LPCOMPAREITEMSTRUCT lpCIS);
};


/////////////////////////////////////////////////////////////////////////////
// CDirOpt dialog

class CDirOpt : public CDialog
{
// Construction
public:
    CDirOpt(CWnd* pParent = NULL);    // standard constructor

// Dialog Data
    //{{AFX_DATA(CDirOpt)
    enum { IDD = IDD_OPTION_DIRECTORIES };
    CString    m_emacs_journal;
    CString    m_emacs_library;
    CString    m_emacs_local_library;
    CString    m_emacs_user_library;
    CString    m_sys_login;
    CString    m_sys_scratch;
    CString    m_emacs_path;
    CString    m_emacs_checkpoint;
    CString    m_emacs_memory;
    //}}AFX_DATA

    void save_options();

// Implementation
protected:
    virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

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

enum COLOR_ITEM_types
{
    COLOR_ITEM_MODE_LINE,
    COLOR_ITEM_NORMAL_TEXT,
    COLOR_ITEM_HIGHLIGHT_TEXT,
    COLOR_ITEM_SYNTAX_KEYWORD1,
    COLOR_ITEM_SYNTAX_KEYWORD2,
    COLOR_ITEM_SYNTAX_KEYWORD3,
    COLOR_ITEM_SYNTAX_WORD,
    COLOR_ITEM_SYNTAX_STRING1,
    COLOR_ITEM_SYNTAX_STRING2,
    COLOR_ITEM_SYNTAX_STRING3,
    COLOR_ITEM_SYNTAX_QUOTE,
    COLOR_ITEM_SYNTAX_COMMENT1,
    COLOR_ITEM_SYNTAX_COMMENT2,
    COLOR_ITEM_SYNTAX_COMMENT3,
    COLOR_ITEM_USER_1,
    COLOR_ITEM_USER_2,
    COLOR_ITEM_USER_3,
    COLOR_ITEM_USER_4,
    COLOR_ITEM_USER_5,
    COLOR_ITEM_USER_6,
    COLOR_ITEM_USER_7,
    COLOR_ITEM_USER_8,
    COLOR_ITEM_MAX
};

class CColourOpt : public CDialog
{
// Construction
public:
    CColourOpt(CWnd* pParent = NULL);    // standard constructor

    void save_options();

    int cur_item;

// Dialog Data
    //{{AFX_DATA(CColourOpt)
    enum { IDD = IDD_OPTION_COLOURS };
    CColorComboBox    m_background;
    CColorComboBox    m_foreground;
    CStatic        m_sample_text;
    CListBox    m_item_list;
    BOOL        m_italic;
    BOOL        m_underline;
    //}}AFX_DATA

    EmacsWinColour    colours[COLOR_ITEM_MAX];

// Implementation
protected:
    virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

    virtual BOOL OnInitDialog();

    // Generated message map functions
    //{{AFX_MSG(CColourOpt)
    virtual void OnOK();
    afx_msg void OnSelchangeOptcolItemlist();
    afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
    afx_msg void OnOptcolDefault();
    afx_msg void OnOptcolCustomise();
    afx_msg void OnSelchangeOptcolForeground();
    afx_msg void OnSelchangeOptcolBackground();
    //}}AFX_MSG
    DECLARE_MESSAGE_MAP()

    void getCustomColours( COLORREF *custom_colours );
    void saveCustomColours( COLORREF *custom_colours );

    void fillInColours( CColorComboBox &list_box, COLORREF *custom_colours );
};
/////////////////////////////////////////////////////////////////////////////
// CPrintOpt dialog

class CPrintOpt : public CDialog
{
// Construction
public:
    CPrintOpt( CDC &_print_dc, CWnd* pParent = NULL);    // standard constructor

    void save_options();
    void load_options();
    float round_margin_value( double margin );
// Dialog Data
    //{{AFX_DATA(CPrintOpt)
    enum { IDD = IDD_OPTION_PRINTER };
    CString    m_header_format;
    CString    m_footer_format;
    float    m_bottom_margin;
    float    m_left_margin;
    float    m_right_margin;
    float    m_top_margin;
    CString    m_margin_units;
    BOOL    m_borders;
    BOOL    m_line_numbers;
    BOOL    m_print_footer;
    BOOL    m_print_header;
    BOOL    m_print_in_colour;
    BOOL    m_print_syntax_colour;
    CString    m_header_font;
    CString    m_footer_font;
    CString    m_document_font;
    int    m_page_break_range;
    BOOL    m_wrap_long_lines;
    CString    m_page_size;
    CString    m_usable_area;
    //}}AFX_DATA

    double    twips_scaling;
    double  rounding_factor;    // 10.0 for 1 digit, 100.0 for 2 digits etc
    EmacsString    units_string;    // holds the measurements unit string for the locale

    double    page_width;        // page width and
    double    page_length;        // page length in measurement units

    EmacsWinRegistry    reg;
    EmacsWinPrintOptions    options;

    CDC &print_dc;
    CWinemacsView *view;

// Implementation
protected:
    void updateDialogSummary();

    virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

    // Generated message map functions
    //{{AFX_MSG(CPrintOpt)
    virtual void OnOK();
    afx_msg void OnPrinterSelectDocumentFont();
    afx_msg void OnPrinterSelectHeaderFont();
    afx_msg void OnPrinterSelectFooterFont();
    afx_msg void OnPrinterResetAll();
    afx_msg void OnKillfocusPrinterMarginBottom();
    //}}AFX_MSG
    DECLARE_MESSAGE_MAP()

    void save_options_from_dialog();
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
    int        m_disk[26];    // holds A: to Z: state

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
/////////////////////////////////////////////////////////////////////////////
// CLogNameOpt dialog

class CLogNameOpt : public CDialog
{
// Construction
public:
    CLogNameOpt(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
    //{{AFX_DATA(CLogNameOpt)
    enum { IDD = IDD_OPTION_LOGICAL_NAMES };
    CButton        m_delete_button;
    CButton        m_set_button;
    CListCtrlEx    m_list;
    CString        m_name;
    CString        m_value;
    //}}AFX_DATA


// Overrides
    // ClassWizard generated virtual function overrides
    //{{AFX_VIRTUAL(CLogNameOpt)
    protected:
    virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
    //}}AFX_VIRTUAL

// Implementation
protected:
    int cur_item;

    CString GetItemString( LPARAM param, int subItem = 0 ) const;

    PFNLVCOMPARE sortFunction;
    static int _stdcall SortByNameForward( LPARAM item1, LPARAM item2, LPARAM objPointer );
    static int _stdcall SortByNameReverse( LPARAM item1, LPARAM item2, LPARAM objPointer );
    static int _stdcall SortByValueForward( LPARAM item1, LPARAM item2, LPARAM objPointer );
    static int _stdcall SortByValueReverse( LPARAM item1, LPARAM item2, LPARAM objPointer );

    // Generated message map functions
    //{{AFX_MSG(CLogNameOpt)
    afx_msg void OnLogNameSet();
    afx_msg void OnLogNameDelete();
    virtual void OnOK();
    virtual BOOL OnInitDialog();
    afx_msg void OnItemchangedLogNameList(NMHDR* pNMHDR, LRESULT* pResult);
    afx_msg void OnColumnClickLogNameList(NMHDR* pNMHDR, LRESULT* pResult);
    //}}AFX_MSG
    DECLARE_MESSAGE_MAP()
};
/////////////////////////////////////////////////////////////////////////////
// COptionToolbar dialog

class COptionToolbar : public CDialog
{
// Construction
public:
    COptionToolbar( CMDIFrameWnd *_frame, CWnd* pParent = NULL);

// Dialog Data
    //{{AFX_DATA(COptionToolbar)
    enum { IDD = IDD_TOOLBAR_CHOOSER };
    BOOL    m_build;
    BOOL    m_case;
    BOOL    m_edit;
    BOOL    m_file;
    BOOL    m_fileAndEdit;
    BOOL    m_macro;
    BOOL    m_region;
    BOOL    m_search;
    BOOL    m_tools;
    BOOL    m_view;
    BOOL    m_window;
    //}}AFX_DATA


// Overrides
    // ClassWizard generated virtual function overrides
    //{{AFX_VIRTUAL(COptionToolbar)
    protected:
    virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
    //}}AFX_VIRTUAL

// Implementation
protected:

    CMainFrame *frame;
    // Generated message map functions
    //{{AFX_MSG(COptionToolbar)
    virtual void OnOK();
    //}}AFX_MSG
    DECLARE_MESSAGE_MAP()

    EmacsWinRegistry reg;
    EmacsWinToolBarsOptions bars;
};
