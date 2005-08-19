// mainfrm.cpp : implementation of the CMainFrame class
//

#include <stdafx.h>
#include <winemacs.h>

#include <mainfrm.h>
#include <winemdoc.h>
#include <winemvw.h>

#ifdef _DEBUG
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CMainFrame

IMPLEMENT_DYNAMIC(CMainFrame, CMDIFrameWnd)

BEGIN_MESSAGE_MAP(CMainFrame, CMDIFrameWnd)
	//{{AFX_MSG_MAP(CMainFrame)
	ON_WM_CREATE()
	ON_WM_CLOSE()
	ON_WM_DESTROY()
	ON_WM_DROPFILES()
	ON_UPDATE_COMMAND_UI(ID_STATUS_LINE, OnUpdateBufferPosition)
	ON_UPDATE_COMMAND_UI(ID_STATUS_COLUMN, OnUpdateBufferPosition)
	ON_UPDATE_COMMAND_UI(ID_STATUS_READ_ONLY, OnUpdateBufferPosition)
	ON_UPDATE_COMMAND_UI(ID_STATUS_REPLACE_MODE, OnUpdateBufferPosition)
	ON_UPDATE_COMMAND_UI(ID_STATUS_COLUMN, OnUpdateBufferPosition)
	ON_WM_ENDSESSION()
	ON_WM_QUERYENDSESSION()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// arrays of IDs used to initialize control bars

// toolbar buttons - IDs are command buttons
static UINT BASED_CODE buttons[] =
{
	// same order as in the bitmap 'toolbar.bmp'
	ID_STOP_EXECUTION,
		ID_SEPARATOR,
	ID_FILE_OPEN,
	ID_FILE_SAVE,
		ID_SEPARATOR,
	ID_EDIT_CUT,
	ID_EDIT_COPY,
	ID_EDIT_PASTE,
		ID_SEPARATOR,
	ID_FILE_PRINT,
	ID_APP_ABOUT,
	ID_CONTEXT_HELP,
};

static UINT BASED_CODE indicators[] =
{
	ID_SEPARATOR,			// status line indicator
	ID_INDICATOR_CAPS,
	ID_STATUS_REPLACE_MODE,
	ID_STATUS_READ_ONLY,
	ID_STATUS_LINE,
	ID_STATUS_COLUMN
};

/////////////////////////////////////////////////////////////////////////////
// CMainFrame construction/destruction

CMainFrame::CMainFrame()
{
}

CMainFrame::~CMainFrame()
{
}

int CMainFrame::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CMDIFrameWnd::OnCreate(lpCreateStruct) == -1)
		return -1;

	// clear the add to title property
	ModifyStyle( FWS_ADDTOTITLE, 0 );

	if (!m_wndToolBar.Create(this) ||
		!m_wndToolBar.LoadBitmap(IDR_MAINFRAME) ||
		!m_wndToolBar.SetButtons(buttons,
		  sizeof(buttons)/sizeof(UINT)))
	{
		TRACE("Failed to create toolbar\n");
		return -1;		// fail to create
	}

	if (!m_wndStatusBar.Create(this) ||
		!m_wndStatusBar.SetIndicators(indicators,
		  sizeof(indicators)/sizeof(UINT)))
	{
		TRACE("Failed to create status bar\n");
		return -1;		// fail to create
	}

	// TODO: Delete these three lines if you don't want the toolbar to
	//  be dockable
	m_wndToolBar.EnableDocking(CBRS_ALIGN_ANY);
	EnableDocking(CBRS_ALIGN_ANY);
	DockControlBar(&m_wndToolBar);

	// TODO: Remove this if you don't want tool tips
	m_wndToolBar.SetBarStyle(m_wndToolBar.GetBarStyle() |
		CBRS_TOOLTIPS | CBRS_FLYBY);

//	UINT nID, nStyle; int iWidth;

//	m_wndStatusBar.GetPaneInfo( 0, nID, nStyle, iWidth );
//	m_wndStatusBar.SetPaneInfo( 0, nID, SBPS_POPOUT, iWidth );
		
	return 0;
}

void CMainFrame::OnDropFiles(HDROP hDropInfo)
{
	SetActiveWindow();      // activate us first !
	theView->OnDropFiles( hDropInfo );
}

afx_msg void CMainFrame::OnUpdateBufferPosition(CCmdUI* pCmdUI)
	{
	char buf[32];
	CString pos;
	int value;
	
	switch( pCmdUI->m_nID )
		{
	case ID_STATUS_READ_ONLY:
		pCmdUI->SetText( "READ" );
		pCmdUI->Enable(bf_mode.md_readonly);
		return;
	case ID_STATUS_REPLACE_MODE:
		pCmdUI->SetText( "OVR" );
		pCmdUI->Enable( bf_mode.md_replace );
		return;
	case ID_STATUS_LINE:
		int n;

		value = 1;
		for( n=1; n<=dot - 1; n += 1 )
			if( char_at (n) == '\n' )
				value++;
		sprintf( buf, "%06d", value );
		break;
	case ID_STATUS_COLUMN:
		value = cur_col;
		sprintf( buf, "%03d", value );
		break;
		}
	pos = buf;        
	pCmdUI->Enable();
	pCmdUI->SetText( pos );
	}


/////////////////////////////////////////////////////////////////////////////
// CMainFrame diagnostics

#ifdef _DEBUG
void CMainFrame::AssertValid() const
{
	CMDIFrameWnd::AssertValid();
}

void CMainFrame::Dump(CDumpContext& dc) const
{
	CMDIFrameWnd::Dump(dc);
}

#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CMainFrame message handlers

void CMainFrame::OnClose()
{
	win_emacs_quit = 1;
}

void CMainFrame::OnDestroy()
{
	CMDIFrameWnd::OnDestroy();
}

void CMainFrame::OnEndSession(BOOL bEnding) 
{
	CMDIFrameWnd::OnEndSession(bEnding);
	
	win_emacs_quit = 1;
}

BOOL CMainFrame::OnQueryEndSession() 
{
	if (!CMDIFrameWnd::OnQueryEndSession())
		return FALSE;
	
	if( mod_exist() )
		if( !get_yes_or_no( 0, u_str("Modified buffers exist, do you really want to exit? ") ) )
			return FALSE;
	
	return TRUE;
}
