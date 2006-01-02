// win_view.h : interface of the CWinemacsView class
//
/////////////////////////////////////////////////////////////////////////////


#define MENU_PREFIX "\202"


/////////////////////////////////////////////////////////////////////////////
// CScrollBarVertical window

class CScrollBarVertical : public CScrollBar
{
// Construction
public:
    CScrollBarVertical( EmacsWindowGroup &group );

// Attributes
public:
    EmacsWindow *win;
    EmacsWindowGroup &group;

// Operations
public:
    void setPosition(void);

// Overrides
    // ClassWizard generated virtual function overrides
    //{{AFX_VIRTUAL(CScrollBarVertical)
    //}}AFX_VIRTUAL

// Implementation
public:
    virtual ~CScrollBarVertical();

    // Generated message map functions
protected:
public:
    //{{AFX_MSG(CScrollBarVertical)
    afx_msg void OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
    //}}AFX_MSG

    DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
// CScrollBarHorizontal window

class CScrollBarHorizontal : public CScrollBar
{
// Construction
public:
    CScrollBarHorizontal( EmacsWindowGroup &group );

// Attributes
public:
    EmacsWindow *win;
    EmacsWindowGroup &group;

// Operations
public:
    void setPosition(void);

// Overrides
    // ClassWizard generated virtual function overrides
    //{{AFX_VIRTUAL(CScrollBarHorizontal)
    //}}AFX_VIRTUAL

// Implementation
public:
    virtual ~CScrollBarHorizontal();

    // Generated message map functions
protected:
public:
    //{{AFX_MSG(CScrollBarHorizontal)
    afx_msg void OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
    //}}AFX_MSG

    DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////
class EmacsWinPrintOptions;

class CWinemacsView : public CView, public EmacsView
{
protected: // create from serialization only
    CWinemacsView();
    DECLARE_DYNCREATE(CWinemacsView)

// Attributes
public:
    CWinemacsDoc* GetDocument();

// Operations
public:
    void *operator new( size_t size )
    { return CView::operator new( size ); }
    void operator delete(void *p)
    { CView::operator delete( p ); }

// Implementation
public:
    virtual ~CWinemacsView();
    virtual void OnDraw(CDC* pDC);
    virtual void OnPrint(CDC* pDC, CPrintInfo *pinfo );
    virtual void OnInitialUpdate();
    virtual BOOL OnCmdMsg(UINT nID, int nCode, void* pExtra, AFX_CMDHANDLERINFO* pHandlerInfo);

#ifdef _DEBUG
    virtual void AssertValid() const;
    virtual void Dump(CDumpContext& dc) const;
#endif

    //
    //    UI-window-XXX helpers
    //
    void activateNextView();
    void activatePreviousView();


    //
    //    TerminalControl overrides
    //
    //
    //    Screen control
    //
    virtual void t_topos(int, int);    // move the cursor to the indicated (row,column); (1,1) is the upper left
    virtual void t_reset();        // reset terminal (screen is in unkown state, convert it to a known one)
    virtual bool t_update_begin();
    virtual void t_update_end();
    virtual void t_insert_lines(int);    // insert n lines
    virtual void t_delete_lines(int);    // delete n lines
    virtual void t_wipe_line(int);    // erase to the end of the line
    virtual bool t_window(void) { return true; }
    virtual bool t_window(int);        // set the screen window so that IDline operations only affect the first n lines of the screen
    virtual void t_display_activity(  unsigned char );
    virtual void t_update_line(EmacsLinePtr oldl, EmacsLinePtr newl, int ln);    // Routine to call to update a line
    virtual void t_change_attributes();    // Routine to change attributes
    virtual void t_beep();
    virtual void t_io_printf( const char *, ... ) { emacs_assert(false); }

    //
    //    Keyboard routines
    //
    virtual void k_check_for_input();    // check for any input

    //
    //    Coordinate mapping
    //
    bool mapCoordinatedToWindows( int em_x, int em_y, long &win_x, long &win_y );
    bool mapCoordinatedToEmacs( long win_x, long win_y, int &em_x, int &em_y );


    // Printing support
    bool initialiseViewForPrinting( CDC *print_dc, EmacsWinPrintOptions &options );
    void tidyUpViewAfterPrinting( CDC *print_dc );
protected:
    virtual BOOL OnPreparePrinting(CPrintInfo* pInfo);
    virtual void OnBeginPrinting(CDC* pDC, CPrintInfo* pInfo);
    virtual void OnEndPrinting(CDC* pDC, CPrintInfo* pInfo);
    virtual void OnPrepareDC(CDC* pDC, CPrintInfo* pInfo = NULL);

    EmacsString ExpandPrintFormatString( const EmacsString &format, CPrintInfo* pInfo );

    void print_line( CDC &print_dc, EmacsLinePtr line, int row );

    afx_msg void OnEmacsFilePrint();
    afx_msg void OnEmacsFilePrintPreview();

public:
    enum { max_scroll_bars = 32 };
private:
    bool input_menu_string( UINT id, bool do_input );
    void fixup_scroll_bars(void);

    virtual BOOL PreCreateWindow(CREATESTRUCT& cs);

    void scroll_lines( int scroll_by );


    CScrollBarHorizontal *horiz_scroll[max_scroll_bars];
    CScrollBarVertical *vert_scroll[max_scroll_bars];

    struct scroll_bar_region_data_t
    {
        RGNDATAHEADER    header;
        RECT rects[max_scroll_bars*2];
    }
            scroll_bar_region_data;

    int        have_hdc;
    CDC        *pDC;

    int        have_focus;
    TEXTMETRIC    textmetric;
    int        nCharHeight;
    int        nCharWidth;
    int        nCharExtra;
    int        char_widths[MSCREENWIDTH];
    CFont        *old_font;
    HBRUSH        white_brush;
    UINT        last_window_size;

    BOOL        m_ignore_next_char;

    CFont        *emacs_normal_font;
    CFont        *emacs_italic_font;
//    HFONT        emacs_special_font;
    EmacsWinColour attr_high, attr_array[SYNTAX_LAST_BIT<<1];

    enum
    {
        MOUSE_BUTTON_1 = 1,
        MOUSE_BUTTON_2 = 2,
        MOUSE_BUTTON_3 = 4
    };
    int mouse_button_state;
    int last_mouse_x;
    int last_mouse_y;

    //
    // print variables
    //
    enum
    {
        PRINT_BORDER_WIDTH        = 72,    // the space taken up by the border
        PRINT_BORDER_LINE_WIDTH        = 36,    // the line width drawn for the border
        PRINT_HEADER_EXTRA_SPACE    = 72,    // extra space between the header line and the page text
        PRINT_FOOTER_EXTRA_SPACE    = 72,    // extra space between the page text and the footer text
        PRINT_LINE_NUMBERS_EXTRA_SPACE    = 72,    // extra space between line_numebers and page text
                            // the dividing line is in the middle of the extra space
        PRINT_MAX_PAGE            = 999
    };

    EmacsWinColour    print_attr_array[SYNTAX_LAST_BIT<<1];

    CFont        *printer_doc_normal_font;
    CFont        *printer_doc_italic_font;

    CFont        *printer_header_font;
    CFont        *printer_footer_font;

    CFont        *printer_old_font;

public:    // public so CPrintOptions can get at this stuff
    int        print_char_width;
    int        print_char_height;
    int        print_char_widths[MSCREENWIDTH];

    CRect        print_whole_page;        // the whole page include borders, headers, footers and text
    CRect        print_text_area;        // the area the buffer text is printed in
    CRect        print_header_area;        // the area the header text is printed in
    CRect        print_line_number_area;        // the area the line number text is printed in
    CRect        print_footer_area;        // the area the footer text is printed in

private:
    EmacsWinPrintOptions    *print_options;

    EmacsWindowGroup *print_window_group;
    EmacsWindow    *print_window;
    ModeSpecific    print_buffer_mode;        // modes used while printing

    int        print_page_pos[PRINT_MAX_PAGE+1];
    int        print_page_line_number[PRINT_MAX_PAGE+1];

// functions
public:
    void emacs_setup_dc(void);
    void emacs_reset_dc(void);
    void win_change_attributes(void);
    void win_reset(void);
    void win_update_line( EmacsLine *old_line, EmacsLine *new_line, int row );
    void win_display_activity( unsigned char ch );

private:
    int new_font();
public:
    afx_msg void OnDropFiles(HDROP hDropInfo);

    void OnMouseButton( UINT nFlags, CPoint point, int button );

    bool choosePrinterFont( CDC &print_dc, EmacsWinFont &font );
// Generated message map functions
protected:
    //{{AFX_MSG(CWinemacsView)
    afx_msg void OnFileOpen();
    afx_msg BOOL OnHelpInfo(HELPINFO* pHelpInfo);
    afx_msg BOOL OnMouseWheel(UINT nFlags, short zDelta, CPoint pt);
    afx_msg BOOL OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message);
    afx_msg void OnChar(UINT nChar, UINT nRepCnt, UINT nFlags);
    afx_msg void OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
    afx_msg void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
    afx_msg void OnKeyUp(UINT nChar, UINT nRepCnt, UINT nFlags);
    afx_msg void OnKillFocus(CWnd* pNewWnd);
    afx_msg void OnLButtonDblClk(UINT nFlags, CPoint point);
    afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
    afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
    afx_msg void OnMouseMove(UINT nFlags, CPoint point);
    afx_msg void OnOptionsColors();
    afx_msg void OnOptionsDirectories();
    afx_msg void OnOptionsFileParsing();
    afx_msg void OnOptionsFont();
    afx_msg void OnOptionsLogNames();
    afx_msg void OnRButtonDblClk(UINT nFlags, CPoint point);
    afx_msg void OnRButtonDown(UINT nFlags, CPoint point);
    afx_msg void OnRButtonUp(UINT nFlags, CPoint point);
    afx_msg void OnSetFocus(CWnd* pOldWnd);
    afx_msg void OnSize(UINT nType, int cx, int cy);
    afx_msg void OnStopExecution();
    afx_msg void OnUpdateEditReplaceCase(CCmdUI* pCmdUI);
    afx_msg void OnUpdateBufferCaseblindsearch(CCmdUI* pCmdUI);
    afx_msg void OnUpdateBufferNarrowregion(CCmdUI* pCmdUI);
    afx_msg void OnUpdateBufferShowwhitespace(CCmdUI* pCmdUI);
    afx_msg void OnUpdateBufferWidenregion(CCmdUI* pCmdUI);
    afx_msg void OnUpdateBufferWraplonglines(CCmdUI* pCmdUI);
    afx_msg void OnUpdateBuildNexterror(CCmdUI* pCmdUI);
    afx_msg void OnUpdateBuildPreviouserror(CCmdUI* pCmdUI);
    afx_msg void OnUpdateEditClear(CCmdUI* pCmdUI);
    afx_msg void OnUpdateEditCopy(CCmdUI* pCmdUI);
    afx_msg void OnUpdateEditCut(CCmdUI* pCmdUI);
    afx_msg void OnUpdateEditDelete(CCmdUI* pCmdUI);
    afx_msg void OnUpdateEditPaste(CCmdUI* pCmdUI);
    afx_msg void OnUpdateFileSave(CCmdUI* pCmdUI);
    afx_msg void OnUpdateSearchFind(CCmdUI* pCmdUI);
    afx_msg void OnUpdateSearchReplace(CCmdUI* pCmdUI);
    afx_msg void OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
    afx_msg void OnUpdateMacrosRecord(CCmdUI* pCmdUI);
    afx_msg void OnUpdateMacrosRun(CCmdUI* pCmdUI);
    afx_msg void OnUpdateMacrosStoprecording(CCmdUI* pCmdUI);
    afx_msg void OnUpdateViewReadonly(CCmdUI* pCmdUI);
    afx_msg void OnViewToolbar();
    afx_msg void OnUpdateFileSaveall(CCmdUI* pCmdUI);
    //}}AFX_MSG
    afx_msg void OnMButtonDown(UINT nFlags, CPoint point);
    afx_msg void OnMButtonUp(UINT nFlags, CPoint point);
    afx_msg void OnMButtonDblClk(UINT nFlags, CPoint point);
    DECLARE_MESSAGE_MAP()
};

#ifndef _DEBUG    // debug version in win_view.cpp
inline CWinemacsDoc* CWinemacsView::GetDocument()
{
    return (CWinemacsDoc*) m_pDocument;
}
#endif

extern const EmacsString &menu_id_to_id_string( unsigned int id );

/////////////////////////////////////////////////////////////////////////////
