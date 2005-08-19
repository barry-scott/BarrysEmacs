// mainfrm.cpp : implementation of the CMainFrame class
//

#include <emacs.h>
#include <win_emacs.h>
#include <win_registry.h>

#include <win_main_frame.h>
#include <win_doc.h>
#include <win_view.h>
#include <win_toolbar.h>
#include <afxcmn.h>

#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
static EmacsInitialisation emacs_initialisation( __DATE__ " " __TIME__, THIS_FILE );

/////////////////////////////////////////////////////////////////////////////
// CMainFrame

IMPLEMENT_DYNAMIC(CMainFrame, CMDIFrameWnd)

BEGIN_MESSAGE_MAP(CMainFrame, CMDIFrameWnd)
	//{{AFX_MSG_MAP(CMainFrame)
	ON_WM_CREATE()
	ON_WM_TIMER()
	ON_WM_CLOSE()
	ON_WM_DESTROY()
	ON_WM_ENDSESSION()
	ON_WM_QUERYENDSESSION()
	ON_WM_DROPFILES()
	ON_UPDATE_COMMAND_UI(ID_STATUS_LINE, OnUpdateBufferPosition)
	ON_UPDATE_COMMAND_UI(ID_STATUS_READ_ONLY, OnUpdateBufferPosition)
	ON_UPDATE_COMMAND_UI(ID_STATUS_REPLACE_MODE, OnUpdateBufferPosition)
	ON_UPDATE_COMMAND_UI(ID_STATUS_COLUMN, OnUpdateBufferPosition)
	ON_UPDATE_COMMAND_UI(ID_STATUS_RECORD_TYPE, OnUpdateBufferPosition)
	ON_WM_DROPFILES()
	ON_COMMAND(ID_VIEW_CUSTOMISETOOLBAR, OnViewCustomisetoolbar)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// arrays of IDs used to initialize control bars

static UINT indicators[] =
	{
	ID_SEPARATOR,			// status line indicator
	ID_INDICATOR_CAPS,
	ID_STATUS_RECORD_TYPE,
	ID_STATUS_REPLACE_MODE,
	ID_STATUS_READ_ONLY,
	ID_STATUS_LINE,
	ID_STATUS_COLUMN
	};


/////////////////////////////////////////////////////////////////////////////
// CMainFrame construction/destruction

CMainFrame::CMainFrame()
	{ }

CMainFrame::~CMainFrame()
	{ }

struct EMACS_AFX_DLLVERSIONINFO
{
		DWORD cbSize;
		DWORD dwMajorVersion;                   // Major version
		DWORD dwMinorVersion;                   // Minor version
		DWORD dwBuildNumber;                    // Build number
		DWORD dwPlatformID;                     // DLLVER_PLATFORM_*
};

typedef HRESULT (CALLBACK* EMACS_AFX_DLLGETVERSIONPROC)(EMACS_AFX_DLLVERSIONINFO *);

DWORD AFXAPI Emacs_AfxGetComCtlVersion()
{
	static int Emacs_afxComCtlVersion = -1;

	// return cached version if already determined...
	if (Emacs_afxComCtlVersion != -1)
		return Emacs_afxComCtlVersion;

	// otherwise determine comctl32.dll version via DllGetVersion
	HINSTANCE hInst = ::GetModuleHandleA("COMCTL32.DLL");
	ASSERT(hInst != NULL);
	EMACS_AFX_DLLGETVERSIONPROC pfn;
	pfn = (EMACS_AFX_DLLGETVERSIONPROC)GetProcAddress(hInst, "DllGetVersion");
	DWORD dwVersion = VERSION_WIN4;
	if (pfn != NULL)
	{
		EMACS_AFX_DLLVERSIONINFO dvi;
		memset(&dvi, 0, sizeof(dvi));
		dvi.cbSize = sizeof(dvi);
		HRESULT hr = (*pfn)(&dvi);
		if (SUCCEEDED(hr))
		{
			ASSERT(dvi.dwMajorVersion <= 0xFFFF);
			ASSERT(dvi.dwMinorVersion <= 0xFFFF);
			dwVersion = MAKELONG(dvi.dwMinorVersion, dvi.dwMajorVersion);
		}
	}
	Emacs_afxComCtlVersion = dwVersion;
	return dwVersion;
}

int CMainFrame::OnCreate(LPCREATESTRUCT lpCreateStruct)
	{
	if (CMDIFrameWnd::OnCreate(lpCreateStruct) == -1)
		return -1;

	// clear the add to title property
	ModifyStyle( FWS_ADDTOTITLE, 0 );

	EnableDocking(CBRS_ALIGN_ANY);

	EmacsWinRegistry reg;		reg.loadRegistrySettings();

	EmacsWinToolBarsOptions bars( reg.toolBars() );

	// if supported use rebar
	if( Emacs_AfxGetComCtlVersion() >= VERSION_IE4 )
		m_toolbars = new EmacsWinReBarToolBars( *this );
	else
		m_toolbars = new EmacsWinMfcToolBars( *this );

	if( !m_toolbars->loadToolBars() )
		{
		TRACE0("Failed to create toolbars\n");
		return -1;      // fail to create
		}
		

	RecalcLayout();

	if( !m_wndStatusBar.CreateEx(this /*, SBT_TOOLTIPS*/) )
		{
		TRACE("Failed to create status bar\n");
		return -1;		// fail to create
		}
	if( !m_wndStatusBar.SetIndicators(indicators, sizeof(indicators)/sizeof(UINT)) )
		{
		TRACE("Failed to create status bar\n");
		return -1;		// fail to create
		}

	return 0;
	}

void CMainFrame::SaveState()
	{
	WINDOWPLACEMENT where;

	//
	//	Save the state of the tool bar and the status bar
	//
	m_toolbars->saveToolBarsState();

	//
	// 	Save the last position of the window
	//
	where.length = sizeof( where );
	if( GetWindowPlacement( &where ) )
		{
		char buf[128];

		wsprintf
		( buf, "%d %d %d %d %d %d %d %d %d",
		where.showCmd,
		where.ptMinPosition.x, where.ptMinPosition.y,
		where.ptMaxPosition.x, where.ptMaxPosition.y,
		where.rcNormalPosition.left, where.rcNormalPosition.top, 
		where.rcNormalPosition.right, where.rcNormalPosition.bottom
		);
		theApp.WriteProfileString( "WindowPositions", "WindowFrame", buf );
		}
	else
		{
		_dbg_msg( FormatString("GetWindowPlacement failed  %d") << GetLastError() );
		theApp.WriteProfileString( "WindowPositions", "WindowFrame", NULL );
		}
	}

void CMainFrame::OnDropFiles(HDROP hDropInfo)
	{
	SetActiveWindow();      // activate us first !

	char file_name[256];
	
	unsigned int num_files = ::DragQueryFile( hDropInfo, 0xffffffff, NULL, 0 );
	for( unsigned int i=0; i<num_files; i++ )
		{                                                        
		int len = ::DragQueryFile( hDropInfo, i, file_name, sizeof( file_name ) );
		file_name[len] = '\0';
		
		char command_line[300];
		strcpy( command_line, "(UI-drop-file \"" );
		char *op = &command_line[strlen(command_line)];
		char *ip = file_name;
		while( *ip )
			{
			*op++ = *ip;
			if( *ip++ == '\\' )
				*op++ = '\\';
			}
		*op = '\0';
		strcat( command_line, "\")" );

		execute_mlisp_string( u_str(command_line) );
		}
	::DragFinish(hDropInfo);
	if( theActiveView )
		theActiveView->windows.do_dsp();
	}


afx_msg void CMainFrame::OnUpdateBufferPosition(CCmdUI* pCmdUI)
	{
	switch( pCmdUI->m_nID )
		{
	case ID_STATUS_READ_ONLY:
		{
		pCmdUI->SetText( "READ" );
		pCmdUI->Enable( bf_cur != NULL && bf_cur->b_mode.md_readonly);
		return;
		}

	case ID_STATUS_REPLACE_MODE:
		{
		pCmdUI->SetText( "OVR" );
		pCmdUI->Enable( bf_cur != NULL && bf_cur->b_mode.md_replace );
		return;
		}

	case ID_STATUS_LINE:
		{
		static int status_line_dot;
		static EmacsBuffer *status_line_buffer;

		if( status_line_buffer == bf_cur
		&& status_line_dot == dot )
			return;

		int value = 1;
		if( bf_cur != NULL )
			for( int n=1; n<=dot - 1; n += 1 )
				if( bf_cur->char_at (n) == '\n' )
					value++;
		EmacsString pos( FormatString( "%07d" ) << value );
		
		status_line_buffer = bf_cur;
		status_line_dot = dot;

		pCmdUI->Enable();
		pCmdUI->SetText( pos );
		}
		return;

	case ID_STATUS_COLUMN:
		{
		static int status_col_dot;
		static EmacsBuffer *status_col_buffer;

		if( status_col_buffer == bf_cur
		&& status_col_dot == dot )
			return;

		int value = 1;
		if( bf_cur != NULL )
			value = cur_col();

		EmacsString pos( FormatString( "%04d" ) << value );

		status_col_buffer = bf_cur;
		status_col_dot = dot;

		pCmdUI->Enable();
		pCmdUI->SetText( pos );
		}
		return;

	case ID_STATUS_RECORD_TYPE:
		{
		if( bf_cur == NULL || bf_cur->b_kind != FILEBUFFER )
			{
			pCmdUI->Enable( FALSE );
			return;
			}

		switch( bf_cur->b_rms_attribute )
			{
		case FIO_RMS__Binary:	// literal read and write no
			pCmdUI->SetText(" BIN");
			break;
		case FIO_RMS__StreamCRLF:	// MS-DOS/Windows lines
			pCmdUI->SetText("CRLF");
			break;
		case FIO_RMS__StreamCR:	// Machintosh lines
			pCmdUI->SetText(" CR ");
			break;
		case FIO_RMS__StreamLF:// Unix lines
			pCmdUI->SetText(" LF ");
			break;
		case FIO_RMS__None:		// not known
		default:
			pCmdUI->SetText("UNKN");
			break;
			}

		pCmdUI->Enable( TRUE );
		return;
		}
		}

	}


/////////////////////////////////////////////////////////////////////////////
// CMainFrame diagnostics

#if defined(_DEBUG)
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

static void (*timeout_handle)(void);

void time_schedule_timeout( void (*time_handle_timeout)(void), const EmacsDateTime &time )
	{
	timeout_handle = time_handle_timeout;

	EmacsDateTime now( EmacsDateTime::now() );
	double d_delta = time.asDouble() - now.asDouble();
	// convert from Seconds units to mS units
	d_delta *= 1000.0;
	int i_delta = int(d_delta);
	// just in case it took a while to ask
	if( i_delta < 1 )
		// force to a positive delta
		i_delta = 100;

	//	start the timer
	UINT status = theApp.m_pMainWnd->SetTimer( 'A', i_delta, NULL );
	if( status == 0 )
		{
		TRACE("Failed to allocate timer\n");
		return;
		}
	}

void time_cancel_timeout(void)
	{
	theApp.m_pMainWnd->KillTimer( 'A' );
	}

void CMainFrame::OnTimer(UINT nIDEvent)
	{
	if( nIDEvent == 'Z' )
		theApp.m_tick = 1;
	else
		if( timeout_handle )
			timeout_handle();			

	CMDIFrameWnd::OnTimer(nIDEvent);
	}

void CMainFrame::OnViewCustomisetoolbar() 
	{
//	m_wndToolBars[0].Customize();
	}

#if 0
static int query_button = 0;
LRESULT CMainFrame::WindowProc( UINT message, WPARAM wParam, LPARAM lParam ) 
	{
	static CHAR szBuf[128];
	LPTOOLTIPTEXT lpToolTipText;

	if( message != WM_NOTIFY )
		return CMDIFrameWnd::WindowProc( message, wParam, lParam );

	LPNMHDR nmhdr = (LPNMHDR)lParam;
	int msg_code = nmhdr->code;

	
	ToolbarInfo *tbi = NULL;
	for( int toolbar=0; toolbar<num_toolbars; toolbar++ )
		if( toolbar_info[toolbar].toolbar_id == nmhdr->idFrom )
			tbi = &toolbar_info[toolbar];

//	DebugPrintf( FormatString("CMDIFrameWnd WM_NOTIFY code %d ") << msg_code );

	switch( msg_code )
		{
	case TTN_NEEDTEXT:    
//		DebugPrintf(" - TTN_NEEDTEXT\n");
		// Display the ToolTip text.
		lpToolTipText = (LPTOOLTIPTEXT)lParam;
		::LoadString
			(
			AfxGetResourceHandle(),
			lpToolTipText->hdr.idFrom,    // string ID == cmd ID
			szBuf,
			sizeof(szBuf)
			);
		lpToolTipText->lpszText = szBuf;
		return TRUE;
		break;

	case TBN_QUERYDELETE:
//		DebugPrintf(" - TBN_QUERYDELETE\n");
		// Toolbar customization--can we delete this button?
		return TRUE;
		break;

	case TBN_GETBUTTONINFO:
		{
		TBNOTIFY *info = (TBNOTIFY *)lParam;

		while( query_button < tbi->num_buttons )
			{
			int nId = tbi->buttons[query_button].idCommand;
			if( nId != 0 )
				{
				info->tbButton = tbi->buttons[query_button];
//				DebugPrintf(" - TBN_GETBUTTONINFO returning info\n");
				query_button++;
				return TRUE;
				}
			query_button++;
			}

		query_button = 0;
//		DebugPrintf(" - TBN_GETBUTTONINFO no info\n");

		// The toolbar needs information about a button.
		return FALSE;
		}
		break;

	case TBN_QUERYINSERT:
//		DebugPrintf(" - TBN_QUERYINSERT\n");
		// Can this button be inserted? Just say yo.
		return TRUE;
		break;

	case TBN_CUSTHELP:
//		DebugPrintf(" - TBN_CUSTHELP\n");
		// Need to display custom Help.
		AfxMessageBox("This Help is custom.");
		return TRUE;
		break;

	case TBN_BEGINDRAG:
//		DebugPrintf(" - TBN_BEGINDRAG\n");
		return TRUE;
		break;

	case TBN_ENDDRAG:
//		DebugPrintf(" - TBN_ENDDRAG\n");
		return TRUE;
		break;

	case TBN_BEGINADJUST:
//		DebugPrintf(" - TBN_BEGINADJUST\n");
		query_button = 0;
		return TRUE;
		break;

	case TBN_ENDADJUST:
//		DebugPrintf(" - TBN_ENDADJUST\n");
		return TRUE;
		break;

	case TBN_RESET:
//		DebugPrintf(" - TBN_RESET\n");
		return TRUE;
		break;

	case TBN_TOOLBARCHANGE:
//		DebugPrintf(" - TBN_TOOLBARCHANGE\n");
		return TRUE;
		break;

	case TBN_DROPDOWN:
//		DebugPrintf(" - TBN_DROPDOWN\n");
		return TRUE;
		break;

//	case TBN_CLOSEUP:
//		DebugPrintf(" - TBN_CLOSEUP\n");
//		return TRUE;
//		break;

	default:
//		DebugPrintf(" - default\n");
		return TRUE;
		break;
		}
	}
#endif
