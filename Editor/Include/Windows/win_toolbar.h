//
//    win_toolbar.h
//        Copyright (c) 1998 Barry A. Scott
//
#ifndef    _EMACS_WIN_TOOLBAR_H_
#define _EMACS_WIN_TOOLBAR_H_
//
// the following lines taken from afximpl.h
//
#define VERSION_WIN4    MAKELONG(0, 4)
#define VERSION_IE3     MAKELONG(70, 4)
#define VERSION_IE4     MAKELONG(71, 4)
#define VERSION_IE401   MAKELONG(72, 4)
extern DWORD AFXAPI Emacs_AfxGetComCtlVersion();

class CMainFrame;

class EmacsWinToolBars
{
public:
    EmacsWinToolBars( CMainFrame &_frame );
    virtual ~EmacsWinToolBars();

    virtual bool loadToolBars() = 0;    // called from CMainFrame::OnCreate
    virtual void saveToolBarsState() = 0;    // called on shutdown
    virtual void loadToolBarsState() = 0;    // Called from CWinEMacs::InitInstance

    virtual BOOL bandIsVisible( UINT id ) = 0;
    virtual void showBand( UINT id, BOOL visible ) = 0;

protected:
    CMainFrame &frame;
};

class EmacsWinReBarToolBars : public EmacsWinToolBars
{
public:
    EmacsWinReBarToolBars( CMainFrame &_frame );
    virtual ~EmacsWinReBarToolBars();

    virtual bool loadToolBars();
    virtual void saveToolBarsState();
    virtual void loadToolBarsState();

    virtual BOOL bandIsVisible( UINT id );
    virtual void showBand( UINT id, BOOL visible );

    CToolBar    m_wndToolBars[11];
    CReBar        m_wndReBar;
};

class EmacsWinMfcToolBars : public EmacsWinToolBars
{
public:
    EmacsWinMfcToolBars( CMainFrame &_frame );
    virtual ~EmacsWinMfcToolBars();

    virtual bool loadToolBars();
    virtual void saveToolBarsState();
    virtual void loadToolBarsState();

    virtual BOOL bandIsVisible( UINT id );
    virtual void showBand( UINT id, BOOL visible );

    CToolBar    m_wndToolBars[11];
};
#endif
