// winemvw.h : interface of the CWinemacsView class
//
/////////////////////////////////////////////////////////////////////////////


#define MENU_PREFIX "\202"

class C_color_attr
{
public:
	C_color_attr();
	~C_color_attr();

public:
	COLORREF foreground;
	COLORREF background;
	BOOL italic;
	BOOL underline;
	CPen *pen;
	C_color_attr & operator=( unsigned char * );
	void C_color_attr::GetColor( unsigned char *str );
	};

class CWinemacsView : public CView
{
protected: // create from serialization only
	CWinemacsView();
	DECLARE_DYNCREATE(CWinemacsView)

// Attributes
public:
	CWinemacsDoc* GetDocument();

	int m_tick;

// Operations
public:

// Implementation
public:
	virtual ~CWinemacsView();
	virtual void OnDraw(CDC* pDC);
	virtual void OnPrint(CDC* pDC, CPrintInfo *pinfo );
	virtual void OnInitialUpdate();
	virtual BOOL OnCmdMsg(UINT nID, int nCode, void* pExtra,
		AFX_CMDHANDLERINFO* pHandlerInfo);

#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

	// Printing support
protected:
	virtual BOOL OnPreparePrinting(CPrintInfo* pInfo);
	virtual void OnBeginPrinting(CDC* pDC, CPrintInfo* pInfo);
	virtual void OnEndPrinting(CDC* pDC, CPrintInfo* pInfo);
	virtual void OnPrepareDC(CDC* pDC, CPrintInfo* pInfo = NULL);

private:
	CHOOSEFONT emacs_cf;
	LOGFONT emacs_lf;
	char font_name[LF_FACESIZE];
	char szStyle[LF_FACESIZE];

	int		have_hdc;
	CDC		*pDC;

	int		have_focus;
	TEXTMETRIC	textmetric;
	int		nCharHeight;
	int		nCharWidth;
	int		nCharExtra;
	int		char_widths[MSCREENWIDTH];
	CFont		*old_font;
	HBRUSH		white_brush;
	UINT		last_window_size;
	int		last_v_scroll_pos;
	int		last_h_scroll_pos;

	BOOL		m_ignore_next_char;

	CFont		*emacs_normal_font;
	CFont		*emacs_italic_font;
	C_color_attr	attr_high,
			attr_array[256];

	enum
		{
		MOUSE_BUTTON_1 = 1,
		MOUSE_BUTTON_2 = 2,
		MOUSE_BUTTON_3 = 4
		};
	int mouse_button_state;	

	// print variables
	CHOOSEFONT emacs_cf_print;
	LOGFONT emacs_lf_print;
	char print_font_name[LF_FACESIZE];
	char print_szStyle[LF_FACESIZE];

	CFont *printer_font, *printer_old_font;

	CPoint		print_top_left_margin;
	TEXTMETRIC	print_textmetric;
	int		print_char_width;
	int		print_char_height;
	struct window	print_window;
	enum { PRINT_MAX_PAGE = 999 };
	int		print_page_pos[PRINT_MAX_PAGE+1];

// functions
public:
	int update_begin( void );
	void update_end( void );
	void insert_lines( int n );
	void delete_lines( int n );
	void wipe_line(int line);
	void emacs_setup_dc(void);
	void emacs_reset_dc(void);
	void win_change_attributes(void);
	void win_reset( struct trmcontrol *tt );
	void win_update_line( struct emacs_line *old_line, struct emacs_line *new_line, int row );
	void win_display_activity( unsigned char ch );

private:
	int new_font();
public:
	afx_msg void OnDropFiles(HDROP hDropInfo);

	void OnMouseButton( UINT nFlags, CPoint point, int button );

// Generated message map functions
protected:
	//{{AFX_MSG(CWinemacsView)
	afx_msg void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
	afx_msg void OnStopExecution();
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnChar(UINT nChar, UINT nRepCnt, UINT nFlags);
	afx_msg void OnKeyUp(UINT nChar, UINT nRepCnt, UINT nFlags);
	afx_msg void OnEditCopy();
	afx_msg void OnEditCut();
	afx_msg void OnEditPaste();
	afx_msg void OnOptionsColors();
	afx_msg void OnOptionsDirectories();
	afx_msg void OnOptionsFont();
	afx_msg void OnSearchFind();
	afx_msg void OnSearchReplace();
	afx_msg void OnUpdateSearchFind(CCmdUI* pCmdUI);
	afx_msg void OnUpdateSearchReplace(CCmdUI* pCmdUI);
	afx_msg void OnSetFocus(CWnd* pOldWnd);
	afx_msg void OnKillFocus(CWnd* pNewWnd);
	afx_msg void OnUpdateEditPaste(CCmdUI* pCmdUI);
	afx_msg void OnUpdateEditCopy(CCmdUI* pCmdUI);
	afx_msg void OnUpdateEditCut(CCmdUI* pCmdUI);
	afx_msg void OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	afx_msg void OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	afx_msg void OnFileOpen();
	afx_msg void OnFileSave();
	afx_msg void OnFileSaveAs();
	afx_msg void OnUpdateFileSave(CCmdUI* pCmdUI);
	afx_msg void OnEditClear();
	afx_msg void OnUpdateEditClear(CCmdUI* pCmdUI);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnRButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnRButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnTimer(UINT nIDEvent);
	afx_msg BOOL OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message);
	afx_msg void OnFileSaveEnvironment();
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnLButtonDblClk(UINT nFlags, CPoint point);
	afx_msg void OnRButtonDblClk(UINT nFlags, CPoint point);
	afx_msg void OnOptionsFileParsing();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

#ifndef _DEBUG	// debug version in winemvw.cpp
inline CWinemacsDoc* CWinemacsView::GetDocument()
   { return (CWinemacsDoc*) m_pDocument; }
#endif

extern CWinemacsView *theView;

extern unsigned char *menu_id_to_id_string( unsigned int id );

/////////////////////////////////////////////////////////////////////////////
