// winemacs.cpp : Defines the class behaviors for the application.
//

#include "stdafx.h"
#include "winemacs.h"

#include "mainfrm.h"
#include "winemdoc.h"
#include "winemvw.h"
#include "win_opt.h"

#ifdef _DEBUG
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CWinemacsApp

BEGIN_MESSAGE_MAP(CWinemacsApp, CWinApp)
	//{{AFX_MSG_MAP(CWinemacsApp)
	ON_COMMAND(ID_APP_ABOUT, OnAppAbout)
	ON_COMMAND(ID_OPTIONS_PRINTING, OnOptionsPrinting)
	ON_UPDATE_COMMAND_UI(ID_OPTIONS_PRINTING, OnUpdateOptionsPrinting)
	//}}AFX_MSG_MAP
	// Standard file based document commands
	ON_COMMAND(ID_FILE_NEW, CWinApp::OnFileNew)
	ON_COMMAND(ID_FILE_OPEN, CWinApp::OnFileOpen)
	// Standard print setup command
	ON_COMMAND(ID_FILE_PRINT_SETUP, CWinApp::OnFilePrintSetup)
	// Global help commands
	ON_COMMAND(ID_HELP_INDEX, CWinApp::OnHelpIndex)
	ON_COMMAND(ID_HELP_USING, CWinApp::OnHelpUsing)
	ON_COMMAND(ID_HELP, CWinApp::OnHelp)
	ON_COMMAND(ID_CONTEXT_HELP, CWinApp::OnContextHelp)
	ON_COMMAND(ID_DEFAULT_HELP, CWinApp::OnHelpIndex)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CWinemacsApp construction

CWinemacsApp::CWinemacsApp() : CWinApp("Emacs " EMACS_VERSION)
{
	// TODO: add construction code here,
	// Place all significant initialization in InitInstance
}

/////////////////////////////////////////////////////////////////////////////
// The one and only CWinemacsApp object

CWinemacsApp NEAR theApp;

/////////////////////////////////////////////////////////////////////////////
// CWinemacsApp initialization

BOOL CWinemacsApp::InitInstance()
{
	int test_mode = 0;	/* true if debuging emacs */

//	_dbg_msg( u_str("1st m_nCmdShow is %d"), m_nCmdShow );
	init_memory();

	if (m_lpCmdLine[0] != '\0')
	{
		argc = 1;
		argv[0] = savestr(u_str("emacs"));
		argv[1] = NULL;

		unsigned char *p = u_str(m_lpCmdLine);
		while( *p )
			{
			while( isspace(*p) )
				p++;

			if( *p == '\0' )
				break;

			if( *p == '/' )
				{
				unsigned char *start = p;
				unsigned char *value = NULL;

				p++;
				while( *p && !isspace(*p) && *p != '/' )
					{
					if( value == NULL && (*p == '=' || *p == ':') )
						value = p+1;
					p++;
					}
				if( value == NULL )
					value = p;

				int key_length = value - start - 1;
				int val_length = p - value;

				if( key_length > 2 && _str_nicmp( u_str("/restore"), start, key_length ) == 0 )
					{
					_str_ncpy( restore_arg, value, val_length );
					restore_arg[val_length] ='\0';
					}
				else if( key_length > 2 && _str_nicmp( u_str("/package"), start, key_length ) == 0 )
					{
					_str_ncpy( package_arg, value, val_length );
					package_arg[val_length] ='\0';
					free( argv[0] );
					argv[0] = savestr( package_arg );
					}
				else if( key_length > 3 && _str_nicmp( u_str("/test"), start, key_length ) == 0 )
					{
					test_mode = 1;
					}
				else
					win_error( u_str( "Unknown option %.*s" ),
						key_length, start );
				}
			else
				{
				unsigned char *start = p;

				while( *p && !isspace( *p ) && *p != '/' )
					p++;

				int length = p - start;
				argv[argc] = malloc_ustr( length+1 );
				_str_ncpy( argv[argc], start, length );
				argv[argc][length] = '\0';
				argc++;
				}
			}
	}

	// Standard initialization
	// If you are not using these features and wish to reduce the size
	//  of your final executable, you should remove from the following
	//  the specific initialization routines you do not need.

	m_pszProfileName = test_mode ? "em_test.ini" : "emacs060.ini";

	SetDialogBkColor();        // set dialog background color to gray
	LoadStdProfileSettings();  // Load standard INI file options (including MRU)

	// Register the application's document templates.  Document templates
	//  serve as the connection between documents, frame windows and views.

	AddDocTemplate(new CMultiDocTemplate(IDR_WINEMATYPE,
			RUNTIME_CLASS(CWinemacsDoc),
			RUNTIME_CLASS(CMDIChildWnd),        // standard MDI child frame
			RUNTIME_CLASS(CWinemacsView)));

	// get handles to the standard mouse cursors used by the views
	normal_text_cursor = LoadStandardCursor( IDC_IBEAM );
	busy_cursor = LoadStandardCursor( IDC_WAIT );

	// create main MDI Frame window
	CMainFrame* pMainFrame = new CMainFrame;

	pMainFrame->m_bAutoMenuEnable = FALSE;

	if (!pMainFrame->LoadFrame(IDR_MAINFRAME))
		return FALSE;

	// we want to be able to receive open DDE commands
	EnableShellOpen();

	strcpy( s_str(window_rendition),
		GetProfileString( "Colours", "WindowText", "" ));
	strcpy( s_str( region_rendition ),
		GetProfileString( "Colours", "HighlightText", "1" ));
	strcpy( s_str( mode_line_rendition ),
		GetProfileString( "Colours", "ModeLine", "7" ));
	strcpy( s_str( syntax_keyword1_rendition ),
		GetProfileString( "Colours", "syntax_keyword1", s_str(window_rendition) ) );
	strcpy( s_str( syntax_keyword2_rendition ),
		GetProfileString( "Colours", "syntax_keyword2", s_str(window_rendition) ) );
	strcpy( s_str( syntax_word_rendition ),
		GetProfileString( "Colours", "syntax_word", s_str(window_rendition) ) );
	strcpy( s_str( syntax_string_rendition ),
		GetProfileString( "Colours", "syntax_string", s_str(window_rendition) ) );
	strcpy( s_str( syntax_quote_rendition ),
		GetProfileString( "Colours", "syntax_quote", s_str(window_rendition) ) );
	strcpy( s_str( syntax_comment_rendition ),
		GetProfileString( "Colours", "syntax_comment", s_str(window_rendition) ) );
	strcpy( s_str( user_1_rendition ),
		GetProfileString( "Colours", "user_1", s_str(window_rendition) ) );
	strcpy( s_str( user_2_rendition ),
		GetProfileString( "Colours", "user_2", s_str(window_rendition) ) );
	strcpy( s_str( user_3_rendition ),
		GetProfileString( "Colours", "user_3", s_str(window_rendition) ) );
	strcpy( s_str( user_4_rendition ),
		GetProfileString( "Colours", "user_4", s_str(window_rendition) ) );
	strcpy( s_str( user_5_rendition ),
		GetProfileString( "Colours", "user_5", s_str(window_rendition) ) );
	strcpy( s_str( user_6_rendition ),
		GetProfileString( "Colours", "user_6", s_str(window_rendition) ) );
	strcpy( s_str( user_7_rendition ),
		GetProfileString( "Colours", "user_7", s_str(window_rendition) ) );
	strcpy( s_str( user_8_rendition ),
		GetProfileString( "Colours", "user_8", s_str(window_rendition) ) );

	/*                         
	 *	Restore the last position of the window
	 */
	CString buf = GetProfileString( "WindowPositions", "WindowFrame", "" );

	WINDOWPLACEMENT where;
	if( sscanf
	( buf, "%d %d %d %d %d %d %d %d %d",
	&where.showCmd,
	&where.ptMinPosition.x, &where.ptMinPosition.y,
	&where.ptMaxPosition.x, &where.ptMaxPosition.y,
	&where.rcNormalPosition.left, &where.rcNormalPosition.top,
	&where.rcNormalPosition.right, &where.rcNormalPosition.bottom
	) == 9 )
		{
		where.length = sizeof( where );
		if( !pMainFrame->SetWindowPlacement( &where ) )
			{
			_dbg_msg( u_str("SetWindowPlacement failed  %d"), ::GetLastError() );
			}
		else
			/* honout minimize from who ever started us */
			if( m_nCmdShow != SW_MINIMIZE
			&& m_nCmdShow != SW_SHOWMINIMIZED
			&& m_nCmdShow != SW_SHOWMINNOACTIVE)
				m_nCmdShow = where.showCmd;
		}

//	_dbg_msg( u_str("2nd m_nCmdShow is %d"), m_nCmdShow );

	pMainFrame->ShowWindow(m_nCmdShow);
	pMainFrame->UpdateWindow();
	pMainFrame->DragAcceptFiles();
	m_pMainWnd = pMainFrame;

	_str_cpy( operating_system_name, "Windows" );

	int is_windows_nt = 0;
	int is_windows_95 = 0;

	OSVERSIONINFO os_info;
	os_info.dwOSVersionInfoSize = sizeof( OSVERSIONINFO );

	/* this call may fail on 3.1 with old win32s */
	if( GetVersionEx( &os_info ) )
		{
		is_windows_95 = os_info.dwPlatformId == VER_PLATFORM_WIN32_WINDOWS;
		is_windows_nt = os_info.dwPlatformId == VER_PLATFORM_WIN32_NT;
		
		sprintfl( operating_system_version, sizeof( operating_system_version ), u_str("V%d.%d %s"),
			 os_info.dwMajorVersion, os_info.dwMinorVersion,
			 is_windows_nt ? "NT" : is_windows_95 ? "Win95" : "Win16" );
		}
	else
		{
		DWORD version = GetVersion();
		is_windows_nt = (version&0x80000000) == 0;

		sprintfl( operating_system_version, sizeof( operating_system_version ), u_str("V%d.%d %s"),
			 (int)(version&0xff), (int)((version>>8)&0xff), (version&0x80000000) == 0 ? "NT" : "Win16" );
		}

	if( is_windows_nt )
		_str_cpy( default_environment_file, "emacs$user:emacs_nt.env" );
	else if( is_windows_95 )
		_str_cpy( default_environment_file, "emacs$user:emacs_95.env" );
	else
		_str_cpy( default_environment_file, "emacs$user:emacs_32.env" );

	return TRUE;
}

int CWinemacsApp::ExitInstance()
	{
	WINDOWPLACEMENT where;

	/*
	 *	Save the last position of the window
	 */
	where.length = sizeof( where );
	if( m_pMainWnd != NULL )
		if( m_pMainWnd->GetWindowPlacement( &where ) )
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
			WriteProfileString( "WindowPositions", "WindowFrame", buf );
			}
		else
			{
			_dbg_msg( u_str("GetWindowPlacement failed  %d"), GetLastError() );
			WriteProfileString( "WindowPositions", "WindowFrame", NULL );
			}

	if( theView != NULL )
		theView->DestroyWindow();

	if( m_pMainWnd != NULL )
		m_pMainWnd->DestroyWindow();

	CWinApp::ExitInstance();

	/* we are always successful */
	return 0;
	}

void CWinemacsApp::emacs_new_document()
	{
	OnFileNew();
	}

/////////////////////////////////////////////////////////////////////////////
// CAboutDlg dialog used for App About

class CAboutDlg : public CDialog
{
public:
	CAboutDlg();

// Dialog Data
	//{{AFX_DATA(CAboutDlg)
	enum { IDD = IDD_ABOUTBOX };
	CStatic	m_emacs_version;
	CStatic	m_memory_used;
	//}}AFX_DATA

// Implementation
protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support
	//{{AFX_MSG(CAboutDlg)
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialog(CAboutDlg::IDD)
{
	//{{AFX_DATA_INIT(CAboutDlg)
	//}}AFX_DATA_INIT
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CAboutDlg)
	DDX_Control(pDX, IDC_ABOUT_VERSION, m_emacs_version);
	DDX_Control(pDX, IDC_ABOUT_MEMORY, m_memory_used);
	//}}AFX_DATA_MAP
}
    
extern long int bytes_allocated;

BOOL CAboutDlg::OnInitDialog()
{
	CDialog::OnInitDialog();
	
	m_emacs_version.SetWindowText( "Version " EMACS_VERSION );

	char buf[32];
	sprintfl( u_str(buf), sizeof( buf ), u_str("%d KB"), bytes_allocated );
	m_memory_used.SetWindowText( buf );
	
	return TRUE;  // return TRUE  unless you set the focus to a control
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialog)
	//{{AFX_MSG_MAP(CAboutDlg)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

// App command to run the dialog
void CWinemacsApp::OnAppAbout()
{
	CAboutDlg aboutDlg;
	aboutDlg.DoModal();
}

/////////////////////////////////////////////////////////////////////////////
// CWinemacsApp commands

extern void init_memory(void);

int CWinemacsApp::Run()
{
	if (m_pMainWnd == NULL)
	{
		TRACE0("Warning: 'm_pMainWnd' is NULL in CWinApp::Run"
				" - quitting application\n");
		::PostQuitMessage(0);
	}

	//
	// A new installation will not have an emacs060.ini file
	// First default the entries then test that the defaulting
	// will allow emacs to start up. If there is a problem get
	// the user to figure out where the files are.
	//
	{
	// The Opions Directory handling will default the device names for us
	CDirOpt dir_opt;

	// Save the defaults
	dir_opt.save_options();

	// init memory so that fopenp can use malloc
	init_memory();

	// if emacs_profile can be opened emacs should init
	unsigned char fnb[BUFSIZ];
	FILE *test = fopenp( EMACS_PATH, u_str("emacs_profile.ml"), fnb, FIO_READ, u_str( ".ml" ));
	if( test != NULL )
		// looks good
		fio_close( test );
	else
		{
		// get the user to figure out where the files are
		win_error( u_str("Emacs cannot find its library file - please configure Emacs") );
		if( dir_opt.DoModal() == IDOK )
			dir_opt.save_options();
		}
	}

	// init elapse time
	fetch_elapse_time( NULL );

	// start up the editor application code
	::emacs( argc, argv, restore_arg, u_str("gui"), u_str("") );

	return ExitInstance();
}
/*
 *	This function is called when Emacs needs to wait for an event
 */
int CWinemacsApp::process_message( int can_wait )
	{
	// Acquire and dispatch messages until a WM_QUIT message is received.
	if( can_wait )
		{
		LONG lIdleCount = 0;
		// check to see if we can do idle work
		while (!::PeekMessage(&m_msgCur, NULL, NULL, NULL, PM_NOREMOVE) &&
			OnIdle(lIdleCount++))
		{
			// more work to do
		}

		// either we have a message, or OnIdle returned false
		if (!PumpMessage())
			return -1;
		}
	else
		{
		while( ::PeekMessage(&m_msgCur, NULL, NULL, NULL, PM_NOREMOVE) )
			if( !PumpMessage() )
				return -1;
		}

	if( win_emacs_quit )
		return -1;
	return 0;
	}

int wait_for_activity(void)
	{             
	// process messages...
	int result = theApp.process_message( 1 );
	// result will hold the state of the quit flag
	// we turn quit off so that emacs can abort a quit
	win_emacs_quit = 0;
	// return the quit state
	return result;
	}

#define CHECK_FOR_ACTIVITY_INTERVAL 200
void win_check_for_activity(struct trmcontrol *tt)
	{
	static check_counter = 0;
	check_counter++;
	check_counter %= CHECK_FOR_ACTIVITY_INTERVAL;
	if( check_counter == 0 )
		{
		theApp.process_message( 0 );
		work_process_queue();
		}
	}

/////////////////////////////////////////////////////////////////////////////
// CFatalError dialog

void win_error( unsigned char *fmt, ... )
	{
	char buf[128];
	int i;
	va_list argp;

	va_start( argp, fmt );

	i = ::do_print( (unsigned char *)fmt, &argp, (unsigned char *)buf, sizeof( buf ) );

	::MessageBox( NULL, buf, "Error", MB_ICONSTOP );
	}

void fatal_error( int code )
	{
	CFatalError dlg;

	dlg.DoModal();

	exit(0);
	}

CFatalError::CFatalError(CWnd* pParent /*=NULL*/)
	: CDialog(CFatalError::IDD, pParent)
{
	//{{AFX_DATA_INIT(CFatalError)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}

void CFatalError::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CFatalError)
	DDX_Control(pDX, IDC_ERROR_CODE, m_error_code);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CFatalError, CDialog)
	//{{AFX_MSG_MAP(CFatalError)
		// NOTE: the ClassWizard will add message map macros here
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CFatalError message handlers
/////////////////////////////////////////////////////////////////////////////
// CYesOrNo dialog

int win_yes_or_no( unsigned char *prompt, int def_yes )
	{
	int result;
	UINT what = MB_YESNO|MB_ICONQUESTION;

	if( !def_yes )
		/* true if no is the default */
		what |= MB_DEFBUTTON2;
	if( theApp.m_pMainWnd == NULL )
		return def_yes;

	result = ::MessageBox( theApp.m_pMainWnd->m_hWnd, s_str(prompt), "Emacs", what );

	return result == IDYES;
	}

CYesOrNo::CYesOrNo(CWnd* pParent /*=NULL*/)
	: CDialog(CYesOrNo::IDD, pParent)
{
	//{{AFX_DATA_INIT(CYesOrNo)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}

void CYesOrNo::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CYesOrNo)
	DDX_Control(pDX, IDC_YESORNO_QUESTION, m_question);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CYesOrNo, CDialog)
	//{{AFX_MSG_MAP(CYesOrNo)
		// NOTE: the ClassWizard will add message map macros here
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CYesOrNo message handlers

void UI_update_window_title( void )
	{
	char new_title[256];

	strcpy( new_title, "Emacs " EMACS_VERSION " [" );
	_str_cat( new_title, current_directory );
	strcat( new_title, "]" );

	theApp.m_pMainWnd->SetWindowText( new_title );
	}

BOOL CWinemacsApp::OnDDECommand(char* pszCommand)
{
	char command_line[300];

	int index = find_mac( u_str("UI-DDE-command") );
	if( index < 0 )
		return FALSE;

	if( mac_bodies[ index ]->b_bound.b_body == 0 )
		return FALSE;

	strcpy( command_line, "(UI-DDE-command \"" );
	char *op = &command_line[strlen(command_line)];
	char *ip = pszCommand;
	while( *ip )
		{
		if( *ip == '\\' || *ip == '"' )
			*op++ = '\\';
		*op++ = *ip++;
		}
	*op = '\0';
	strcat( command_line, "\")" );

	execute_mlisp_string( u_str(command_line) );

	do_dsp(0);

	return TRUE;
}


void CWinemacsApp::OnOptionsPrinting()
{
	CPrintOpt print_opt;
	
	if( print_opt.DoModal() == IDOK )
		print_opt.save_options();
}

void CWinemacsApp::OnUpdateOptionsPrinting(CCmdUI* pCmdUI)
{
	// TODO: Add your command update UI handler code here
	
}
