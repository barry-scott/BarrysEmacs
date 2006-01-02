// mainfrm.h : interface of the CMainFrame class
//
/////////////////////////////////////////////////////////////////////////////

class EmacsWinToolBars;

class CMainFrame : public CMDIFrameWnd
{
    DECLARE_DYNAMIC(CMainFrame)
public:
    CMainFrame();

// Attributes
public:
    int m_tick;

// Operations
public:

// Implementation
public:
    virtual ~CMainFrame();
#ifdef _DEBUG
    virtual    void AssertValid() const;
    virtual    void Dump(CDumpContext& dc) const;
#endif

    void SaveState();

protected:    // control bar embedded members
    CStatusBar        m_wndStatusBar;
public:
    EmacsWinToolBars    *m_toolbars;

    // Generated message map functions
protected:
    //{{AFX_MSG(CMainFrame)
    afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
    afx_msg void OnTimer(UINT nIDEvent);
    afx_msg void OnClose();
    afx_msg void OnDestroy();
    afx_msg void OnEndSession(BOOL bEnding);
    afx_msg BOOL OnQueryEndSession();
    afx_msg void OnViewCustomisetoolbar();
    //}}AFX_MSG
    afx_msg void OnDropFiles(HDROP hDropInfo);
    afx_msg void OnUpdateBufferPosition(CCmdUI* pCmdUI);
#if 0
    virtual LRESULT WindowProc(UINT message, WPARAM wParam, LPARAM lParam);
#endif
    DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////
