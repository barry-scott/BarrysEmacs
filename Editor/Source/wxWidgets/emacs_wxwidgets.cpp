//
//  emacs_wxwidgets.cpp
//
//  Copyright 2008 Barry A. Scott
//

#include "emacs.h"
#include "emacs_wxwidgets.h"

#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
static EmacsInitialisation emacs_initialisation( __DATE__ " " __TIME__, THIS_FILE );

extern void init_scheduled_timeout(void);
extern void init_dsp(void);
extern void init_memory(void);
extern void init_lisp(void);
extern void init_abs(void);
extern void init_srch(void);
extern void init_terminal(const EmacsString &, const EmacsString &);
extern void restore_db(void);
extern void restore_vms(void);
extern void init_fncs(void);
extern void init_bf(void);
extern void init_display(void);
extern void init_undo(void);
extern void init_key(void);
extern void init_fncs2(void);
extern void init_var(void);
extern void restore_var(void);
extern void init_vms(void);
extern void restore_timer(void);
extern void init_win(void);

#define         DEFAULT_FONT "Swiss"

static int      x_pos = 0;                           // Startup X position
static int      y_pos = 0;                           // Startup Y position
static int      point_size = 12;                     // Font size

#if wxUSE_CLIPBOARD
    #include "wx/dataobj.h"
    #include "wx/clipbrd.h"
#endif

MainWindow *EmacsMainWindow = NULL;

TerminalControl_wxWidgets *theWxWidgetsGui = NULL;
bool is_wxWidgets = false;

int init_gui_terminal( const EmacsString &displayName )
{
    TerminalControl_wxWidgets *gui = new TerminalControl_wxWidgets( displayName );

    theWxWidgetsGui = gui;
    theActiveView = gui;
    theActiveView->t_il_mf = 1;
    theActiveView->t_il_ov = 1;
    theActiveView->t_ic_ov = MISSINGFEATURE;
    theActiveView->t_dc_ov = MISSINGFEATURE;
    theActiveView->t_baud_rate = 1000000;

    term_app_keypad = 1;
    term_edit = 1;
    term_eightbit = 1;

    is_wxWidgets = 1;

    if( !gui->create_windows( displayName ) )
        return false;

    gui->updateWindowTitle();

    return true;
}

IMPLEMENT_APP( EmacsApp )

bool EmacsApp::OnInit()
{
    EmacsInitialisation::setup_version_string();

    // setup the debug flags
    EmacsString p( get_config_env( "EMACS_DEBUG" ) );
    if( !p.isNull() )
    {
        dbg_flags = parse_dbg_flags( p );
        if( dbg_flags != 0 )
            _dbg_msg( FormatString("dbg_flags=%X") << dbg_flags );
    }

    //
    //    check to see if a restore operation is required and possible
    //
    int rv = 0;
    init_memory();

    EmacsMainWindow = new MainWindow
                            (
                            NULL,
                            wxID_ANY,
                            _T( "Barry's Emacs" ),
                            wxPoint( x_pos, y_pos ),
                            wxDefaultSize,
                            wxCAPTION|wxMINIMIZE_BOX|wxSYSTEM_MENU|wxCLOSE_BOX|wxFULL_REPAINT_ON_RESIZE
                            );

    EmacsMainWindow->canvas = new EmacsCanvas( EmacsMainWindow );

    EmacsMainWindow->Resize();
    EmacsMainWindow->Show( true );


    // init logic without a restore
    init_fncs();                            // initialise the key bindings
    init_var();                             // " the variables
    init_bf();                              // " the buffer system
    init_scheduled_timeout();
    init_display();                         // " the core display system
    init_terminal( u_str("gui"), u_str("") );     // Start the primary terminal IO system
    init_dsp();
    init_win();                             // " the window system
    init_srch();                            // " the search commands
    init_undo();                            // " the undo facility
    init_lisp();                            // " the MLisp system
    init_abs();                             // " the current directory name
#if defined( SUBPROCESSES )
    init_vms();                             // " VMS specific commands
#endif

    init_key();                             // " commands that deal with options
    current_global_map = global_map;
    init_fncs2();                           // Finish off init of functions

    //
    //    Emacs internals are now initrialised
    //    Before running the user MLisp code
    //    give the OS/GUI code a chance to do
    //    some work.
    //
    //    For example:
    //        On VMS read the restart message
    //        On Windows wait for the command to come from the client
    //        On Unix start the emacs server
    //
    //    Exit if the event handler detects an error
    //
    if( !emacs_internal_init_done_event() )
        return false;

#if 0
    start_async_io();

    must_checkpoint_on_crash = 1;

    if( is_restored == 0 )
    {
        gui_error = 1;
        rv = execute_mlisp_file( EMACS_INIT_PROFILE, 1 );
        gui_error = 0;
    }
    else
        rv = 0;

    //
    // do not run the enter-emacs-hook if we are building the default environment
    //
    if(( term_is_terminal || is_restored != 0)
    && rv == 0 )
    {
        if( user_interface_hook_proc != NULL )
            rv = execute_bound_saved_environment( user_interface_hook_proc );
            // ignore result of user_interface_hook_proc

        if( enter_emacs_proc != NULL )
            rv = execute_bound_saved_environment( enter_emacs_proc );
    }

    if( rv == 0 )
        rv = execute_package( command_line_arguments.argument(0).value() );

    if( rv == 0
    && !touched_command_args
    && !read_in_files() )
        read_emacs_memory_file();

    ml_err = 0;

    EmacsWorkItem::enableWorkQueue(true);

    if( term_is_terminal )
    {
        bool can_exit;
        do
        {
            if( rv == 0 )
                process_keys();
            rv = 0;
#ifdef WIN32
            can_exit = UI_quit_emacs();
#else
            can_exit = true;
            if( mod_exist() )
                can_exit = get_yes_or_no( 0, u_str("Modified buffers exist, do you really want to exit? ") );
#if defined( SUBPROCESSES )
            if( can_exit
            && ! silently_kill_processes
            && count_processes() )
                can_exit = get_yes_or_no( 1, u_str("You have processes still on the prowl, shall I chase them down for you? " ) );
#endif
#endif

        }
        while( ! can_exit );
    }
    else
        // execute sys$input as the command stream
        rv = execute_mlisp_file( device, 0 );

    if( exit_emacs_proc != 0 && rv == 0 )
        rv = execute_bound_saved_environment( exit_emacs_proc );

    write_emacs_memory_file();

#if defined( SUBPROCESSES )
    kill_processes();
#endif
    kill_checkpoint_files();
    EmacsBufferJournal::journal_exit();

    rst_dsp();
    emacs_is_exiting = 1;
#endif

    return true;
}

int EmacsApp::OnExit()
{
    return 0;
}

// Create the fonts
void MainWindow::CreateFonts()
{
    m_normalFont = wxTheFontList->FindOrCreateFont( point_size, wxSWISS, wxNORMAL, wxNORMAL );
    m_boldFont =   wxTheFontList->FindOrCreateFont( point_size, wxSWISS, wxNORMAL, wxBOLD );
    m_italicFont = wxTheFontList->FindOrCreateFont( point_size, wxSWISS, wxITALIC, wxNORMAL );
}

BEGIN_EVENT_TABLE( MainWindow, wxFrame )
    EVT_CLOSE( MainWindow::OnCloseWindow )
    EVT_CHAR( MainWindow::OnChar )
    EVT_MENU( wxID_ANY, MainWindow::OnPopup )
END_EVENT_TABLE()

MainWindow::MainWindow( wxFrame *frame, wxWindowID id, const wxString& title,
     const wxPoint& pos, const wxSize& size, long style ):
     wxFrame( frame, id, title, pos, size, style )
{
    CreateFonts();
}

MainWindow::~MainWindow()
{
}

// Find the size of the poem and resize the window accordingly
void MainWindow::Resize( void )
{
}


void MainWindow::OnCloseWindow( wxCloseEvent& WXUNUSED( event ) )
{
    this->Destroy();
}

void MainWindow::OnChar( wxKeyEvent& event )
{
    canvas->OnChar( event );
}

BEGIN_EVENT_TABLE( EmacsCanvas, wxWindow )
    EVT_MOUSE_EVENTS( EmacsCanvas::OnMouseEvent )
    EVT_CHAR( EmacsCanvas::OnChar )
    EVT_PAINT( EmacsCanvas::OnPaint )
END_EVENT_TABLE()

// Define a constructor for my canvas
EmacsCanvas::EmacsCanvas( wxFrame *frame )
: wxWindow( frame, wxID_ANY )
{
    m_popupMenu = new wxMenu;
    m_popupMenu->Append( EMACS_BIGGER_TEXT, _T( "Bigger text" ) );
    m_popupMenu->Append( EMACS_SMALLER_TEXT, _T( "Smaller text" ) );
    m_popupMenu->AppendSeparator();
    m_popupMenu->Append( EMACS_ABOUT, _T( "About wxPoem" ) );
    m_popupMenu->AppendSeparator();
    m_popupMenu->Append( EMACS_EXIT, _T( "Exit" ) );
}

EmacsCanvas::~EmacsCanvas()
{
    // Note: this must be done before the main window/canvas are destroyed
    // or we get an error ( no parent window for menu item button )
    delete m_popupMenu;
    m_popupMenu = NULL;
}

// Define the repainting behaviour
void EmacsCanvas::OnPaint( wxPaintEvent& WXUNUSED( event ) )
{
    wxPaintDC dc( this );
}

void EmacsCanvas::OnMouseEvent( wxMouseEvent& event )
{
    static int startPosX, startPosY, startFrameX, startFrameY;

    long x, y;
    event.GetPosition( &x, &y );

    if ( event.RightDown() )
    {
        // Versions from wxWin 1.67 are probably OK
        PopupMenu( m_popupMenu, ( int )x, ( int )y  );
    }
    else if ( event.LeftDown() )
    {
        this->CaptureMouse();
        int x1 = ( int )x;
        int y1 = ( int )y;
        ClientToScreen( &x1, &y1 );
        startPosX = x1;
        startPosY = y1;
        GetParent()->GetPosition( &startFrameX, &startFrameY );
    }
    else if ( event.LeftUp() )
    {
        if ( GetCapture() == this ) this->ReleaseMouse();
    }
    else if ( event.Dragging() && event.LeftIsDown() )
    {
        int x1 = ( int )x;
        int y1 = ( int )y;
        ClientToScreen( &x1, &y1 );

        int dX = x1 - startPosX;
        int dY = y1 - startPosY;
        GetParent()->Move( startFrameX + dX, startFrameY + dY );
    }
}

// Process characters
void EmacsCanvas::OnChar( wxKeyEvent& event )
{
    switch ( event.GetKeyCode() )
    {
        case WXK_ESCAPE:
            EmacsMainWindow->Close( true );
        default:
            break;
    }
}

void MainWindow::OnPopup( wxCommandEvent& event )
{
    switch ( event.GetId() )
    {
        case EMACS_BIGGER_TEXT:
            point_size ++;
            CreateFonts();
            EmacsMainWindow->Resize();
            break;
        case EMACS_SMALLER_TEXT:
            if ( point_size > 2 )
            {
                point_size --;
                CreateFonts();
                EmacsMainWindow->Resize();
            }
            break;
        case EMACS_ABOUT:
            ( void )wxMessageBox( _T( "wxPoem Version 1.1\nJulian Smart ( c ) 1995" ),
                               _T( "About wxPoem" ), wxOK, EmacsMainWindow );
            break;
        case EMACS_EXIT:
            // Exit
            EmacsMainWindow->Close( true );
            break;
        default:
            break;
    }
}

void time_schedule_timeout( void( *time_handle_timeout )(void ), const EmacsDateTime &when  )
{
}

void time_cancel_timeout(void)
{
}

bool emacs_internal_init_done_event(void)
{
    return true;
}

void UI_update_window_title( void )
{
}

int ui_frame_to_foreground(void)
{
    void_result();
    return 0;
}

TerminalControl_wxWidgets::TerminalControl_wxWidgets(EmacsString const&)
: EmacsView()
{
}

TerminalControl_wxWidgets::~TerminalControl_wxWidgets()
{
}

int TerminalControl_wxWidgets::create_windows(EmacsString const&)
{
    return 1;
}

void TerminalControl_wxWidgets::updateWindowTitle()
{
}

void *TerminalControl_wxWidgets::operator new(size_t size) // throw( std::bad_alloc )
{ 
    return malloc( size );
}

void TerminalControl_wxWidgets::operator delete(void *p) // throw()
{
    free( p );
}

void TerminalControl_wxWidgets::t_topos(int, int)
{
}

void TerminalControl_wxWidgets::t_reset()
{
}

bool TerminalControl_wxWidgets::t_update_begin()
{
    return false;
}

void TerminalControl_wxWidgets::t_update_end()
{
}

void TerminalControl_wxWidgets::t_insert_lines(int)
{
}

void TerminalControl_wxWidgets::t_delete_lines(int)
{
}

void TerminalControl_wxWidgets::t_wipe_line(int)
{
}

bool TerminalControl_wxWidgets::t_window(void)
{
    return false;
}

bool TerminalControl_wxWidgets::t_window(int)
{
    return false;
}

void TerminalControl_wxWidgets::t_display_activity(  unsigned char )
{
}

void TerminalControl_wxWidgets::t_update_line( EmacsLinePtr oldl, EmacsLinePtr newl, int ln )
{
}

void TerminalControl_wxWidgets::t_change_attributes()
{
}

void TerminalControl_wxWidgets::t_beep()
{
}

//
//    Keyboard routines
//
void TerminalControl_wxWidgets::k_check_for_input()
{
}

void TerminalControl_wxWidgets::t_wipe_screen()
{
}

void TerminalControl_wxWidgets::t_flash()
{
}

int TerminalControl_wxWidgets::k_input_event( unsigned char *, unsigned int )
{
    return 0;
}
