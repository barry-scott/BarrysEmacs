//
//    win_emacs.cpp : Defines the class behaviors for the application.
//

#include <emacs.h>
#include <mem_man.h>
#include <win_emacs.h>
#include <win_registry.h>

#include <win_main_frame.h>
#include <win_doc.h>
#include <win_view.h>
#include <win_opt.h>
#include <win_toolbar.h>

#include <process.h>

#include <crtdbg.h>

#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
static EmacsInitialisation emacs_initialisation( __DATE__ " " __TIME__, THIS_FILE );

void EmacsInitialisation::os_specific_init()
{
    // turn off the iritating debug code
    _CrtSetDbgFlag( 0 );
    AfxEnableMemoryTracking( 0 );
}




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
    ON_COMMAND(ID_FILE_PRINT_SETUP, OnEmacsFilePrintSetup)
    // Global help commands
    ON_COMMAND(ID_HELP_INDEX, CWinApp::OnHelpIndex)
    ON_COMMAND(ID_HELP_USING, CWinApp::OnHelpUsing)
    ON_COMMAND(ID_HELP, CWinemacsApp::OnEmacsHelp)
    ON_COMMAND(ID_CONTEXT_HELP, CWinApp::OnContextHelp)
    ON_COMMAND(ID_DEFAULT_HELP, CWinApp::OnHelpIndex)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CWinemacsApp construction

CWinemacsApp::CWinemacsApp()
    : CWinApp("Barry's Emacs")
    , restore_arg("emacs$user:")
    , package_arg("emacs")
    , m_tick(false)
{
    // turn off the iritating debug code
    _CrtSetDbgFlag( 0 );
    AfxEnableMemoryTracking( 0 );
}

/////////////////////////////////////////////////////////////////////////////
// The one and only CWinemacsApp object

CWinemacsApp theApp;

// This identifier was generated to be statistically unique for your app.
// You may change it if you prefer to choose a specific identifier.

// {4B62A436-701D-11D1-8C8B-0000C07229D3}
static const CLSID app_unique_clsid =
{ 0x4b62a436, 0x701d, 0x11d1, { 0x8c, 0x8b, 0x0, 0x0, 0xc0, 0x72, 0x29, 0xd3 } };

/////////////////////////////////////////////////////////////////////////////
// CWinemacsApp initialization

extern void test_func( DWORD version, const char *windows_version );

DWORD main_thread_id;

BOOL CWinemacsApp::InitInstance()
{
    // only run on one process
    ::SetProcessAffinityMask( ::GetCurrentProcess(), 1 );
    // Initialize OLE libraries
    if (!AfxOleInit())
    {
        AfxMessageBox(IDP_OLE_INIT_FAILED);
        return FALSE;
    }

    //
    //    Create the save environment object at the earlest opertunity
    //
    EmacsSaveRestoreEnvironmentObject = EMACS_NEW EmacsSaveRestoreEnvironment;

    int test_mode = 0;    // true if debugging emacs

//    _dbg_msg( u_str("1st m_nCmdShow is %d"), m_nCmdShow );
    init_memory();

    // do we need to add emacs as arg
{
    EmacsString line("emacs "); line.append( m_lpCmdLine );
    command_line_arguments.setArguments( line );


    DebugPrintf( FormatString( "Command line: %s" ) << line );

//    ::MessageBox( NULL, m_lpCmdLine, "Debug Command Line", MB_OK );

    int arg=1;
    while( arg<command_line_arguments.argumentCount() )
    {
        EmacsArgument argument( command_line_arguments.argument( arg ) );

        if( argument.isQualifier() )
        {
            EmacsString str( argument.value() );
            EmacsString key_string;
            EmacsString val_string;

            int equal_pos = str.first( '=' );
            if( equal_pos > 0 )
            {
                key_string = str( 1, equal_pos );
                val_string = str( equal_pos+1, str.length() );
            }
            else
            {
                key_string = str( 1, str.length() );
            }

            bool delete_this_arg = true;

            if( key_string.commonPrefix( "restore" ) > 2 )
                restore_arg = val_string;
            else if( key_string.commonPrefix( "norestore" ) > 4 )
                restore_arg = EmacsString::null;
            else if( key_string.commonPrefix( "package" ) > 2 )
            {
                package_arg = val_string;
                command_line_arguments.setArgument( 0, package_arg, false );
            }
            else if( key_string.commonPrefix( "test" ) > 3 )
                test_mode = 1;
            else if( key_string.caseBlindCompare( "embedding" ) == 0 )
                qual_embedded = true;
            else if( key_string.caseBlindCompare( "automation" ) == 0 )
                qual_automation = true;
            else if( key_string.caseBlindCompare( "regserver" ) == 0 )
                qual_regserver = true;
            else if( key_string.caseBlindCompare( "unregserver" ) == 0 )
                qual_unregserver = true;
            else
                delete_this_arg = false;

            if( delete_this_arg )
                command_line_arguments.deleteArgument( arg );
            else
                arg++;
        }
        else
            arg++;
    }

}

#if _MFC_VER == 0x0600
#ifdef _AFXDLL
    Enable3dControls();        // Call this when using MFC in a shared DLL
#else
    Enable3dControlsStatic();    // Call this when linking to MFC statically
#endif
#endif

    // Standard initialization
    // If you are not using these features and wish to reduce the size
    //  of your final executable, you should remove from the following
    //  the specific initialization routines you do not need.


    // the Company name
    SetRegistryKey( "Barry Scott" );
    // have to use _tcsdup as MFC decided to "free" m_pszProfileName
    m_pszProfileName = _tcsdup( test_mode ? "Testing Barry's Emacs V7" : "Barry's Emacs V7" );

#if _MFC_VER == 0x0600
    SetDialogBkColor();        // set dialog background color to gray
#endif
    LoadStdProfileSettings( 10 );  // Load standard INI file options (including MRU)

    // Register the application's document templates.  Document templates
    //  serve as the connection between documents, frame windows and views.

    CMultiDocTemplate* pDocTemplate =
        new CMultiDocTemplate
            (
            IDR_WINEMATYPE,
            RUNTIME_CLASS( CWinemacsDoc ),
            RUNTIME_CLASS( CMDIChildWnd ),        // standard MDI child frame
            RUNTIME_CLASS( CWinemacsView )
            );
    AddDocTemplate( pDocTemplate );

    // Connect the COleTemplateServer to the document template.
    //  The COleTemplateServer creates new documents on behalf
    //  of requesting OLE containers by using information
    //  specified in the document template.
    //m_server.ConnectTemplate(clsid, pDocTemplate, FALSE);


    main_thread_id = GetCurrentThreadId();
    unsigned thread_result = _beginthreadex
        (
        NULL,                // void *security,
        0,                // unsigned stack_size,
        atl_thread_mainline,        // unsigned ( __stdcall *start_address )( void * ),
        this,                // void *arglist,
        0,                // unsigned initflag,
        &atl_thread_id            // unsigned *thrdaddr
        );
    if( thread_result == -1 )
    {
        DebugPrintf("Failed to create thread\n");
        throw( int(10) );
    }

    atl_thread_handle = HANDLE( thread_result );

    // get handles to the standard mouse cursors used by the views
    normal_text_cursor = LoadStandardCursor( IDC_IBEAM );
    busy_cursor = LoadStandardCursor( IDC_WAIT );

    // create main MDI Frame window
    CMainFrame* pMainFrame = new CMainFrame;

    pMainFrame->m_bAutoMenuEnable = FALSE;

    if( !pMainFrame->LoadFrame( IDR_MAINFRAME ) )
        return FALSE;

    // register us
    // But do not register in an OLE environment
    //RegisterShellFileTypes();

    // we want to be able to receive open DDE commands
    EnableShellOpen();

    // quit now if all we are doing is a register
    if( qual_regserver || qual_unregserver)
    {
        DebugPrintf( "Waiting for the ATL thread to exit" );

        // wait for the thread to do its job for a few seconds
        WaitForSingleObject( atl_thread_handle, 20*1000 );

        // close the handle left by _endthreadex()
        CloseHandle( atl_thread_handle );

        //if( qual_regserver )
        //    ::MessageBox( NULL, "BEmacs Server has been registered", "BEmacs Server setup", MB_OK );
        //else
        //    ::MessageBox( NULL, "BEmacs Server has been unregistered", "BEmacs Server uninstall", MB_OK );
        return FALSE;
    }

    // load settings from registry
{
    EmacsWinRegistry reg;    reg.loadRegistrySettings();

    EmacsWinColours colours( reg.displayColours() );

    window_rendition = colours.windowText().colourAsString();
    region_rendition = colours.highlightText().colourAsString();
    mode_line_rendition = colours.modeLine().colourAsString();
    syntax_keyword1_rendition = colours.syntaxKeyword1().colourAsString();
    syntax_keyword2_rendition = colours.syntaxKeyword2().colourAsString();
    syntax_keyword3_rendition = colours.syntaxKeyword3().colourAsString();
    syntax_word_rendition = colours.syntaxWord().colourAsString();
    syntax_string1_rendition = colours.syntaxString1().colourAsString();
    syntax_string2_rendition = colours.syntaxString2().colourAsString();
    syntax_string3_rendition = colours.syntaxString3().colourAsString();
    syntax_quote_rendition = colours.syntaxQuote().colourAsString();
    syntax_comment1_rendition = colours.syntaxComment1().colourAsString();
    syntax_comment2_rendition = colours.syntaxComment2().colourAsString();
    syntax_comment3_rendition = colours.syntaxComment3().colourAsString();
    user_1_rendition = colours.user1().colourAsString();
    user_2_rendition = colours.user2().colourAsString();
    user_3_rendition = colours.user3().colourAsString();
    user_4_rendition = colours.user4().colourAsString();
    user_5_rendition = colours.user5().colourAsString();
    user_6_rendition = colours.user6().colourAsString();
    user_7_rendition = colours.user7().colourAsString();
    user_8_rendition = colours.user8().colourAsString();
}

    //
    //    Restore the last position of the window
    //
    CString buf = GetProfileString( "WindowPositions", "WindowFrame", "" );

//    _dbg_msg( FormatString("1st m_nCmdShow is %d") << m_nCmdShow );

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
        where.flags = 0;
        if( !pMainFrame->SetWindowPlacement( &where ) )
        {
            _dbg_msg( FormatString("SetWindowPlacement failed  %d") << ::GetLastError() );
        }
        else
            // only use the saved state if this is a normal window state start
            if( m_nCmdShow == SW_NORMAL
            || m_nCmdShow == SW_SHOWDEFAULT )
            {
                m_nCmdShow = where.showCmd;

                if( m_nCmdShow == SW_MINIMIZE
                || m_nCmdShow == SW_SHOWMINNOACTIVE )
                    m_nCmdShow = SW_NORMAL;
            }
    }

//    _dbg_msg( FormatString("2nd m_nCmdShow is %d") << m_nCmdShow );


    pMainFrame->m_toolbars->loadToolBarsState();
    pMainFrame->ShowWindow(m_nCmdShow);
    pMainFrame->UpdateWindow();
    pMainFrame->DragAcceptFiles();
    m_pMainWnd = pMainFrame;

    operating_system_name = "Windows";

    int is_windows_nt = 0;
    int is_windows_95 = 0;

    OSVERSIONINFO os_info;
    os_info.dwOSVersionInfoSize = sizeof( OSVERSIONINFO );

    // this call may fail on 3.1 with old win32s
    if( GetVersionEx( &os_info ) )
    {
        is_windows_95 = os_info.dwPlatformId == VER_PLATFORM_WIN32_WINDOWS;
        is_windows_nt = os_info.dwPlatformId == VER_PLATFORM_WIN32_NT;

        operating_system_version = FormatString("V%d.%d %s") <<
             os_info.dwMajorVersion << os_info.dwMinorVersion <<
             (is_windows_nt ? "NT" : is_windows_95 ? "Win95" : "Win16");
    }
    else
    {
        DWORD version = GetVersion();
        is_windows_nt = (version&0x80000000) == 0;

        operating_system_version = FormatString("V%d.%d %s") <<
             int(version&0xff) << int((version>>8)&0xff) << ((version&0x80000000) == 0 ? "NT" : "Win16");
    }

    if( is_windows_nt )
        default_environment_file = "emacs$user:emacs7_nt.env";
    else if( is_windows_95 )
        default_environment_file = "emacs$user:emacs7_95.env";
    else
        default_environment_file = "emacs$user:emacs7_32.env";

    return TRUE;
}

int CWinemacsApp::ExitInstance()
{
    if( !qual_regserver && !qual_unregserver )
    {
        CMainFrame *frame = (CMainFrame *)m_pMainWnd;
        frame->SaveState();
    }


    // tell the ATL thread to exit
    ::PostThreadMessage( atl_thread_id, WM_QUIT, 0, 0);

    // Wait for the thread to exit
    WaitForSingleObject( atl_thread_handle, 30*1000 );

    CWinApp::ExitInstance();

    // we are always successful
    return 0;
}


void CWinemacsApp::emacs_new_document()
{
    OnFileNew();
}

afx_msg void CWinemacsApp::OnEmacsHelp()
{
    EmacsString html_file;
    expand_and_default
        (
        "emacs$library:..\\documentation\\emacs-documentation.htm",
        EmacsString::null,
        html_file
        );

    EmacsString quoted_html_file;
    quoted_html_file.append("\"");
    quoted_html_file.append( html_file );
    quoted_html_file.append("\"");

    HINSTANCE status = ShellExecute
        (
        NULL,            // parent window
        "open",            // open
        html_file,        // the HTML
        NULL,            // with no parmaters
        ".",            // and default dir of here
        SW_SHOWNORMAL        // and show the window
        );
    if( int(status) < 32 )
        error( FormatString("Failed to view HTML documentation. Error %d") << int(status) );

}

void CWinemacsApp::GetEmacsPrinterSetup()
{
    BYTE *dev_names = NULL; UINT dev_names_size = 0;
    BYTE *dev_mode = NULL;  UINT dev_mode_size = 0;

    if( !GetProfileBinary( "Print", "print_dev_names", &dev_names, &dev_names_size )
    ||  !GetProfileBinary( "Print", "print_dev_mode", &dev_mode, &dev_mode_size ) )
    {
        delete dev_names;
        delete dev_mode;
        return;
    }

    HANDLE h_dev_names = GlobalAlloc( GMEM_MOVEABLE, dev_names_size );
    void *new_dev_names = GlobalLock( h_dev_names );
    memcpy( new_dev_names, dev_names, dev_names_size );
    GlobalUnlock( h_dev_names );

    HANDLE h_dev_mode = GlobalAlloc( GMEM_MOVEABLE, dev_mode_size );
    void *new_dev_mode = GlobalLock( h_dev_mode );
    memcpy( new_dev_mode, dev_mode, dev_mode_size );
    GlobalUnlock( h_dev_mode );

    SelectPrinter( h_dev_names, h_dev_mode );
}

bool CWinemacsApp::SaveEmacsPrinterSetup()
{
    int status;
    if( m_hDevNames == NULL || m_hDevMode == NULL )
    {
        unsigned char empty[1];
        status = WriteProfileBinary( "Print", "print_dev_names", empty, 0 );
        if( !status )
            return false;
        status = WriteProfileBinary( "Print", "print_dev_mode", empty, 0 );
        return status != 0;
    }

    void *dev_names = GlobalLock( m_hDevNames );
    status = WriteProfileBinary( "Print", "print_dev_names", (BYTE *)dev_names, GlobalSize( m_hDevNames ) );
    GlobalUnlock( m_hDevNames );
    if( !status )
        return FALSE;
    void *dev_mode = GlobalLock( m_hDevMode );
    status = WriteProfileBinary( "Print", "print_dev_mode", (BYTE *)dev_mode, GlobalSize( m_hDevMode ) );
    GlobalUnlock( m_hDevMode );
    return status != 0;
}

void CWinemacsApp::OnEmacsFilePrintSetup()
{
    GetEmacsPrinterSetup();

    CPrintDialog pd(TRUE);
    DoPrintDialog(&pd);

    SaveEmacsPrinterSetup();
}

bool CWinemacsApp::CreateEmacsPrinterDC( CDC &printer_dc )
{
    GetEmacsPrinterSetup();
    UpdatePrinterSelection( FALSE );
    SaveEmacsPrinterSetup();

    return CreatePrinterDC( printer_dc ) != 0;
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
    CStatic    m_emacs_version;
    CStatic    m_memory_used;
    //}}AFX_DATA

// Implementation
protected:
    virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
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

    //DDX_Control(pDX, IDC_ABOUT_VERSION, m_emacs_version);

    //{{AFX_DATA_MAP(CAboutDlg)
    DDX_Control(pDX, IDC_ABOUT_MEMORY, m_memory_used);
    //}}AFX_DATA_MAP
}

extern long int bytes_allocated;

BOOL CAboutDlg::OnInitDialog()
{
    CDialog::OnInitDialog();

    //m_emacs_version.SetWindowText( "Version " EMACS_VERSION );

    static char units[] = " Bytes";
    enum { digit_space = 10*4 };
    char buf[ digit_space + sizeof(units) ];
    strcpy( &buf[digit_space], units );

    int pos=digit_space-1;
    int n = bytes_allocated;
    while( n > 0 )
    {
        buf[pos] = char(n%10 + '0'); pos--;
        n /= 10;
        if( (digit_space-pos)%4 == 0 && n > 0 )
        { buf[pos] = ','; pos--; }
    }
    m_memory_used.SetWindowText( &buf[pos+1] );

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
#if _MFC_VER == 0x0600
    if (m_pMainWnd == NULL)
{
        TRACE0("Warning: 'm_pMainWnd' is NULL in CWinApp::Run"
                " - quitting application\n");
        ::PostQuitMessage(0);
}
#else
    if (m_pMainWnd == NULL && AfxOleGetUserCtrl())
{
        // Not launched /Embedding or /Automation, but has no main window!
        TRACE(traceAppMsg, 0, "Warning: m_pMainWnd is NULL in CWinApp::Run - quitting application.\n");
        AfxPostQuitMessage(0);
}
#endif

    //afxMemDF |= checkAlwaysMemDF;

    //
    // A new installation will not have an emacs060.ini file
    // First default the entries then test that the defaulting
    // will allow emacs to start up. If there is a problem get
    // the user to figure out where the files are.
    //
{
    // The Options Directory handling will default the device names for us
    CDirOpt dir_opt;

    // Save the defaults
    dir_opt.save_options();

    // init memory so that fopenp can use malloc
    init_memory();

    // if emacs_profile can be opened emacs should init
    EmacsFile test;
    test.fio_open_using_path( EMACS_PATH, "emacs_profile.ml", FIO_READ, ".ml");
    if( !test.fio_is_open() )
    {
        // get the user to figure out where the files are
        win_error( "Emacs cannot find its library file - please configure Emacs" );
        if( dir_opt.DoModal() == IDOK )
            dir_opt.save_options();
    }
}

    //
    // if embedding then stop emacs processing
    // the command line or memory file
    //
    if( qual_embedded )
    {
        touched_command_args = true;
    }


    // start up the editor application code
    ::emacsMain( restore_arg, u_str("gui"), u_str("") );

    return ExitInstance();
}

//
//    This function is called when Emacs needs to wait for an event
//
#if _MFC_VER == 0x0600
int CWinemacsApp::process_message( int can_wait )
{
    static bIdle = TRUE;

    // Acquire and dispatch messages until a WM_QUIT message is received.
    if( can_wait )
    {
        LONG lIdleCount = 0;

        // phase1: check to see if we can do idle work
        while (bIdle &&
            !::PeekMessage(&m_msgCur, NULL, NULL, NULL, PM_NOREMOVE))
        {
            // call OnIdle while in bIdle state
            if (!OnIdle(lIdleCount++))
                bIdle = FALSE; // assume "no idle" state
        }

        do
        {
            // pump message, but quit on WM_QUIT
            if (!PumpMessage())
                return -1;

            // reset "no idle" state after pumping "normal" message
            if (IsIdleMessage(&m_msgCur))
            {
                bIdle = TRUE;
                lIdleCount = 0;
            }
        }
        while (::PeekMessage(&m_msgCur, NULL, NULL, NULL, PM_NOREMOVE));
    }
    else
    {
        while( ::PeekMessage(&m_msgCur, NULL, NULL, NULL, PM_NOREMOVE) )
        {
            if( !PumpMessage() )
                return -1;
            // reset "no idle" state after pumping "normal" message
            if (IsIdleMessage(&m_msgCur))
            {
                bIdle = TRUE;
            }
        }
    }

    if( win_emacs_quit )
        return -1;
    return 0;
}
#else
int CWinemacsApp::process_message( int can_wait )
{
    static BOOL bIdle = TRUE;

    _AFX_THREAD_STATE* pState = AfxGetThreadState();

    // Acquire and dispatch messages until a WM_QUIT message is received.
    if( can_wait )
    {
        LONG lIdleCount = 0;

        // phase1: check to see if we can do idle work
        while (bIdle &&
            !::PeekMessage(&(pState->m_msgCur), NULL, NULL, NULL, PM_NOREMOVE))
        {
            // call OnIdle while in bIdle state
            if (!OnIdle(lIdleCount++))
                bIdle = FALSE; // assume "no idle" state
        }

        do
        {
            // pump message, but quit on WM_QUIT
            if (!PumpMessage())
                return -1;

            // reset "no idle" state after pumping "normal" message
            if (IsIdleMessage(&(pState->m_msgCur)))
            {
                bIdle = TRUE;
                lIdleCount = 0;
            }
        }
        while (::PeekMessage(&(pState->m_msgCur), NULL, NULL, NULL, PM_NOREMOVE));
    }
    else
    {
        while( ::PeekMessage(&(pState->m_msgCur), NULL, NULL, NULL, PM_NOREMOVE) )
        {
            if( !PumpMessage() )
                return -1;
            // reset "no idle" state after pumping "normal" message
            if (IsIdleMessage(&(pState->m_msgCur)))
            {
                bIdle = TRUE;
            }
        }
    }

    if( win_emacs_quit )
        return -1;
    return 0;
}
#endif

int wait_for_activity(void)
{
    // process messages... wait if no channel io pending
    int result = theApp.process_message( 1 );
    // result will hold the state of the quit flag
    // we turn quit off so that emacs can abort a quit
    win_emacs_quit = 0;
    // return the quit state
    return result;
}

bool check_for_interrupt_key(void)
{
    theApp.process_message( 0 );

    return interrupt_key_struck != 0;
}

extern EmacsString g_initialCurrentDirectory;

bool CWinemacsApp::wait_for_command_line(void)
{
    if( !qual_embedded )
        // no need to wait
        return true;

    DebugPrintf( FormatString("wait_for_command_line() g_initialCurrentDirectory=%s") << g_initialCurrentDirectory);

    // while command line not found...
    while( g_initialCurrentDirectory.isNull() )
    {
        int status = process_message( 1 );
        if( status == -1 )
            // used to tell emacs to quit
            return false;
    }

    DebugPrintf( FormatString("wait_for_command_line() done g_initialCurrentDirectory=%s") << g_initialCurrentDirectory);

    chdir_and_set_global_record( g_initialCurrentDirectory );

    // found command line
    return true;
}

bool emacs_internal_init_done_event(void)
{
    return theApp.wait_for_command_line();
}

int CHECK_FOR_ACTIVITY_INTERVAL = 2000;
void CWinemacsView::k_check_for_input()
{
    static check_counter = 0;
    check_counter++;
    if( check_counter > CHECK_FOR_ACTIVITY_INTERVAL )
    {
        check_counter = 0;
        theApp.process_message( 0 );
        EmacsWorkItem::processQueue();
    }
}

/////////////////////////////////////////////////////////////////////////////
// CFatalError dialog

void win_error( EmacsString text )
{
    HWND wnd = NULL;

    if( theApp.m_pMainWnd != NULL )
        wnd = theApp.m_pMainWnd->m_hWnd;

    ::MessageBox( wnd, text, "Error", MB_ICONSTOP );
}

void fatal_error( int code )
{
    CFatalError dlg;

    char code_as_string[34];
    dlg.m_error_code = _itoa( code, code_as_string, 10 );
    dlg.DoModal();

    exit(0);
}

CFatalError::CFatalError(CWnd* pParent /*=NULL*/)
    : CDialog(CFatalError::IDD, pParent)
{
    //{{AFX_DATA_INIT(CFatalError)
    m_error_code = _T("");
    //}}AFX_DATA_INIT
}

void CFatalError::DoDataExchange(CDataExchange* pDX)
{
    CDialog::DoDataExchange(pDX);
    //{{AFX_DATA_MAP(CFatalError)
    DDX_Text(pDX, IDC_ERROR_CODE, m_error_code);
    DDV_MaxChars(pDX, m_error_code, 31);
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

int win_yes_or_no( int def_yes, const EmacsString &prompt )
{
    int result;
    UINT what = MB_YESNO|MB_ICONQUESTION;

    if( !def_yes )
        // true if no is the default
        what |= MB_DEFBUTTON2;
    result = ::MessageBox( theApp.m_pMainWnd->m_hWnd, prompt, "Emacs", what );

    return result == IDYES;
}


extern EmacsString bemacs_process_name;

void UI_update_window_title( void )
{
    char new_title[256];

    if( bemacs_process_name.isNull() || bemacs_process_name == "main" )
        strcpy( new_title, "Emacs " EMACS_VERSION );
    else
        strcpy( new_title, bemacs_process_name.sdata() );
    strcat( new_title, " [" );
    strcat( new_title, current_directory.sdata() );
    strcat( new_title, "]" );

    theApp.m_pMainWnd->SetWindowText( new_title );
    // ::MessageBox( NULL, new_title, "Current directory change notification", MB_OK|MB_ICONINFORMATION );
}

class dde_work_item : public EmacsWorkItem
{
public:
    dde_work_item() : EmacsWorkItem(), string() {}

    void add_dde_item( const char *command );

    virtual void workAction(void);

private:
    EmacsString string;
};
static dde_work_item dde_request;

void dde_work_item::workAction(void)
{
    BoundName *proc = BoundName::find( "UI-DDE-command" );
    if( proc == NULL )
        return;

    if( !proc->isBound() )
        return;

    ProgramNodeNode prog_node( proc, 1 );
    // must new the ProgramNodeString as its deleted via the NodeNode d'tor
    prog_node.pa_node[0] = new ProgramNodeString( string );

    exec_prog( &prog_node );

    //theActiveView->do_dsp(0);
}

void dde_work_item::add_dde_item( const char *command )
{
    string = command;

    addItem();
}

BOOL CWinemacsApp::OnDDECommand(char* pszCommand)
{
    EmacsString command_line;

    BoundName *proc = BoundName::find( "UI-DDE-command" );
    if( proc == NULL )
        return FALSE;

    if( !proc->isBound() )
        return FALSE;

    dde_request.add_dde_item( pszCommand );

    return TRUE;
}


void CWinemacsApp::OnOptionsPrinting()
{
    if( theActiveView == NULL )
        return;

    CDC print_dc;
    if( !theApp.CreateEmacsPrinterDC( print_dc ) )
    {
        theApp.m_pMainWnd->MessageBox
            (
            "Unable to find a printer to use.\n"
            "Use Print Setup to fix the problem."
            "No printer available",
            MB_OK
            );

        return;
    }


    CPrintOpt print_opt( print_dc, theApp.m_pMainWnd );

    if( print_opt.DoModal() == IDOK )
        print_opt.save_options();
}

void CWinemacsApp::OnUpdateOptionsPrinting(CCmdUI* /*pCmdUI*/)
{
    // TODO: Add your command update UI handler code here

}


extern int ui_file_save_as(void);

bool UI_mod_write( bool ask_before_save )
{
    EmacsBufferRef old( bf_cur );
    bool write_errors = false;

    // force the global buffer data into the emacs_buffer structure
    old.set_bf();

    for( EmacsBuffer *b = buffers; !ml_err && b != NULL; b = b->b_next )
    {
        b->set_bf();

        bool ask_before_save_this_file = ask_before_save;

        if( b->b_kind == FILEBUFFER
        && b->b_modified != 0 )
        {
            bool save_resolved = false;
            while( !save_resolved )
            {
                b->set_bf();

                DWORD save_action = IDC_SAVE_FILE;
                if( ask_before_save_this_file )
                {
                    CSaveQuestion save_question;
                    save_question.m_buffer_name = makeCString( b->b_buf_name );
                    save_question.m_file_name = makeCString( b->b_fname );

                    save_action = save_question.DoModal();
                }

                switch( save_action )
                {
                case IDC_SAVE_FILE:
                    if( write_this( EmacsString::null ) )
                        save_resolved = true;
                    break;

                case IDC_SAVE_AS_FILE:
                {
                    if( b->b_fname.isNull() )
                        ui_save_as_file_name = b->b_buf_name;
                    else
                        ui_save_as_file_name = b->b_fname;

                    ui_file_save_as();
                    if( ml_value.asInt() )
                        if( write_this( ui_save_as_file_name.asString() ) == 0 )
                        {
                            win_error( FormatString("Failed to write to %s") << ui_save_as_file_name.asString() );
                            write_errors = true;
                        }
                }

                case IDC_NO_SAVE:
                    save_resolved = true;
                    break;

                case IDCANCEL:
                    write_errors = true;
                    goto cancel;
                }

                ask_before_save_this_file = true;
            }
        }
    }
cancel:
    old.set_bf();

    // ok if no ml_err and no write error
    return !ml_err && !write_errors;
}

bool UI_quit_emacs()
{
    if( mod_exist() )
    {
        CSaveBeforeQuit dlg;

        DWORD rc = dlg.DoModal();

        switch( rc )
        {
        case IDC_QUIT_SAVE_ALL:
            // Save modified files - quit if UI_mod_write succeeds
            return UI_mod_write( false );

        case IDC_QUIT_NO_SAVE:
            // Just quit and lose changes
            return true;

        case IDC_QUIT_SAVE_INTERACTIVE:
            // Save modified files - quit if UI_mod_write succeeds
            return UI_mod_write( true );

        case IDCANCEL:
            // cancel the quit
            return false;
        }
    }

    // otherwise may quit

    return true;
}


/////////////////////////////////////////////////////////////////////////////
// CQuitEmacs dialog


CQuitEmacs::CQuitEmacs(CWnd* pParent /*=NULL*/)
    : CDialog(CQuitEmacs::IDD, pParent)
{
    //{{AFX_DATA_INIT(CQuitEmacs)
        // NOTE: the ClassWizard will add member initialization here
    //}}AFX_DATA_INIT
}


void CQuitEmacs::DoDataExchange(CDataExchange* pDX)
{
    CDialog::DoDataExchange(pDX);
    //{{AFX_DATA_MAP(CQuitEmacs)
        // NOTE: the ClassWizard will add DDX and DDV calls here
    //}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CQuitEmacs, CDialog)
    //{{AFX_MSG_MAP(CQuitEmacs)
        // NOTE: the ClassWizard will add message map macros here
    //}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CQuitEmacs message handlers
/////////////////////////////////////////////////////////////////////////////
// CSaveBeforeQuit dialog


CSaveBeforeQuit::CSaveBeforeQuit(CWnd* pParent /*=NULL*/)
    : CDialog(CSaveBeforeQuit::IDD, pParent)
{
    //{{AFX_DATA_INIT(CSaveBeforeQuit)
        // NOTE: the ClassWizard will add member initialization here
    //}}AFX_DATA_INIT
}


BOOL CSaveBeforeQuit::OnInitDialog()
{
    CDialog::OnInitDialog();

    BOOL status;
    status = m_list.AddColumn( "Buffer Name", 0 );
    status = m_list.AddColumn( "File Name", 1 );

    int width_of_name_column = m_list.GetColumnWidth( 0 );
    int width_of_value_column = m_list.GetColumnWidth( 1 );

    bf_cur->set_bf();

    EmacsBuffer *b = buffers;
    for( int value_index=0; b != NULL;b = b->b_next )
    {
        if( b->b_modified != 0
        &&  b->b_kind == FILEBUFFER )
        {
            m_list.AddItem( value_index, 0, b->b_buf_name );
            int width = m_list.GetStringWidth( b->b_buf_name );
            if( width_of_name_column < width )
                width_of_name_column = width;

            m_list.AddItem( value_index, 1, b->b_fname );
            width = m_list.GetStringWidth( b->b_fname );
            if( width_of_value_column < width )
                width_of_value_column = width;

             value_index++;
        }
    }

    m_list.SetColumnWidth( 0, width_of_name_column + 10 );
    m_list.SetColumnWidth( 1, width_of_value_column + 30 );

    return TRUE;  // return TRUE unless you set the focus to a control
                  // EXCEPTION: OCX Property Pages should return FALSE
}


void CSaveBeforeQuit::DoDataExchange(CDataExchange* pDX)
{
    CDialog::DoDataExchange(pDX);
    //{{AFX_DATA_MAP(CSaveBeforeQuit)
    DDX_Control(pDX, IDC_LIST_MODIFIED_BUFFERS, m_list);
    //}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CSaveBeforeQuit, CDialog)
    //{{AFX_MSG_MAP(CSaveBeforeQuit)
    ON_BN_CLICKED(IDC_QUIT_NO_SAVE, OnQuitNoSave)
    ON_BN_CLICKED(IDC_QUIT_SAVE_ALL, OnQuitSaveAll)
    ON_BN_CLICKED(IDC_QUIT_SAVE_INTERACTIVE, OnQuitSaveInteractive)
    //}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CSaveBeforeQuit message handlers

void CSaveBeforeQuit::OnQuitNoSave()
{
    EndDialog( IDC_QUIT_NO_SAVE );
}

void CSaveBeforeQuit::OnQuitSaveAll()
{
    EndDialog( IDC_QUIT_SAVE_ALL );
}

void CSaveBeforeQuit::OnQuitSaveInteractive()
{
    EndDialog( IDC_QUIT_SAVE_INTERACTIVE );
}
/////////////////////////////////////////////////////////////////////////////
// CSaveQuestion dialog


CSaveQuestion::CSaveQuestion(CWnd* pParent /*=NULL*/)
    : CDialog(CSaveQuestion::IDD, pParent)
{
    //{{AFX_DATA_INIT(CSaveQuestion)
    m_buffer_name = _T("");
    m_file_name = _T("");
    //}}AFX_DATA_INIT
}


void CSaveQuestion::DoDataExchange(CDataExchange* pDX)
{
    CDialog::DoDataExchange(pDX);
    //{{AFX_DATA_MAP(CSaveQuestion)
    DDX_Text(pDX, IDC_SAVE_QUESTION_BUFFER_NAME, m_buffer_name);
    DDX_Text(pDX, IDC_SAVE_QUESTION_FILE_NAME, m_file_name);
    //}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CSaveQuestion, CDialog)
    //{{AFX_MSG_MAP(CSaveQuestion)
    ON_BN_CLICKED(IDC_NO_SAVE, OnNoSave)
    ON_BN_CLICKED(IDC_SAVE_AS_FILE, OnSaveAsFile)
    ON_BN_CLICKED(IDC_SAVE_FILE, OnSaveFile)
    //}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CSaveQuestion message handlers

void CSaveQuestion::OnNoSave()
{
    EndDialog( IDC_NO_SAVE );
}

void CSaveQuestion::OnSaveAsFile()
{
    EndDialog( IDC_SAVE_AS_FILE );
}

void CSaveQuestion::OnSaveFile()
{
    EndDialog( IDC_SAVE_FILE );
}
