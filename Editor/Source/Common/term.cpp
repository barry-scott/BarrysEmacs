//
//    V5.0 Emacs
//
#include <emacs.h>

#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
static EmacsInitialisation emacs_initialisation( __DATE__ " " __TIME__, THIS_FILE );


extern EmacsLine *phys_screen[MSCREENLENGTH + 1];
extern EmacsLine *desired_screen[MSCREENLENGTH + 1];

#ifdef vms
#include <descrip.h>
#include <ttdef.h>
#include <tt2def.h>
#endif

void init_dsp( void );
void init_terminal( const EmacsString &device_name, const EmacsString &term_type );
void startasyncIO( void );
void rst_dsp( void );
void set_protocol(int flag);
void alter_brd_mbx(int flag);

void set_activity_character(unsigned char ch);
void display_activity_character( void *param );

int complete_reinit = 1;

int sys_version;
SystemExpressionRepresentationIntPositive term_is_terminal( 1 );
SystemExpressionRepresentationDisplayBoolean term_cursor_mode( 0 );
SystemExpressionRepresentationIntPositive mouse_enable( 1 );
SystemExpressionRepresentationIntBoolean term_ansi( 1 );
SystemExpressionRepresentationIntBoolean term_app_keypad( 1 );
SystemExpressionRepresentationIntBoolean term_avo;
SystemExpressionRepresentationIntBoolean term_deccrt;
SystemExpressionRepresentationIntBoolean term_deccrt2;
SystemExpressionRepresentationIntBoolean term_deccrt3;
SystemExpressionRepresentationIntBoolean term_deccrt4;
SystemExpressionRepresentationIntBoolean term_edit( 1 );
SystemExpressionRepresentationIntBoolean term_eightbit( 1 );
SystemExpressionRepresentationScreenLength term_length;
SystemExpressionRepresentationIntBoolean term_nopadding;
SystemExpressionRepresentationTermOutputSize term_output_buffer_size;
SystemExpressionRepresentationIntBoolean term_regis;
SystemExpressionRepresentationScreenWidth term_width;

#ifdef vms
extern int lib$find_image_symbol
    ( struct dsc$descriptor *a, struct dsc$descriptor *b, int * );

#include <em_mess.h>
#define EMACS__INCOMPTERM EMACS$_INCOMPTERM
#else
#define EMACS__INCOMPTERM 1
#endif

void init_dsp( void )
{
    if( !term_is_terminal )
        return;

     theActiveView->t_select();
     theActiveView->t_init();

     theActiveView->t_select();
     theActiveView->t_change_attributes();

    term_width = theActiveView->t_width;
    term_length = theActiveView->t_length;

    screen_garbaged = 1;
    if( complete_reinit )
    {
         theActiveView->t_reset();
         theActiveView->t_window( 0 );
    }

#if BROADCAST
    reassign_mbx();
#endif
}

void TerminalControl::t_geometry_change()
{
    if( theActiveView != NULL
    && theActiveView->currentWindow() != NULL )
    {
        theActiveView->fit_windows_to_screen();
        screen_garbaged = 1;
        theActiveView->do_dsp();
    }
}

TerminalControl::TerminalControl()
    : t_length( 24 )
    , t_width( 80 )
    , k_input_is_enabled( theActiveView != NULL ? theActiveView->k_input_is_enabled : false )
{ }

TerminalControl::~TerminalControl()
{ }

void TerminalControl::k_input_enable()
{
    k_input_is_enabled = true;
}

void TerminalControl::k_input_disable()
{
    k_input_is_enabled = false;
}

SystemExpressionRepresentationCtrlXSwap swap_ctrl_x_char;

void SystemExpressionRepresentationCtrlXSwap::assign_value( ExpressionRepresentation *new_value )
{
    int value = new_value->asInt();

    if( value >= 1 && value <= 31 )
        exp_int = value;
    else
        throw EmacsExceptionVariableTabOutOfRange();
}

void SystemExpressionRepresentationCtrlXSwap::fetch_value(void)
{ }

void TerminalControl::t_cleanup()
{
    theActiveView->t_topos( theActiveView->t_length, 1 );
    theActiveView->t_wipe_line( 0 );
}

void TerminalControl::t_io_printf(const char *fmt, ...)
{
    va_list argp;

    char buf[256];

    va_start( argp, fmt );

    vsprintf( buf, fmt, argp );

    t_io_print( u_str(buf) );
}


//
//    Init a terminal and return the theActiveView structure initialised.
//
void init_terminal( const EmacsString &term_type, const EmacsString &device_name )
{
    int status;

    //
    //    Emacs recognises three types of terminal
    //    "file" - a batch processed stream
    //    "char" - an ANSI character terminal
    //    "gui"  - a graphical user interface
    //
    if( term_type.caseBlindCompare("file") == 0 )
    {
        term_is_terminal = 0;
        status = init_file_terminal( device_name );
    }
    else if( term_type.caseBlindCompare("char") == 0 )
    {
        term_is_terminal = 1;
        status = init_char_terminal( device_name );
    }
    else if( term_type.caseBlindCompare("gui") == 0 )
    {
        term_is_terminal = 3;
        status = init_gui_terminal( device_name );
    }
    else
    {
        _dbg_msg( FormatString( "Unknown terminal type %s") << term_type );
        status = 0;
    }
    if( ! status )
    {
        _dbg_msg( "Failed to init in init_terminal" );
        emacs_exit( EMACS__INCOMPTERM );
    }

}

class TerminalControl_FILE : public EmacsView
{
public:
    TerminalControl_FILE( const unsigned char *file );
    virtual ~TerminalControl_FILE();
    //
    //    Screen control
    //
    virtual void t_cleanup();    // clean up terminal settings
    virtual void t_io_printf( const char *, ... );
};


int init_file_terminal( const EmacsString &file )
{
    theActiveView = new TerminalControl_FILE( file );

    return 1;
}

TerminalControl_FILE::TerminalControl_FILE( const unsigned char *file )
    : EmacsView()
{
    //
    //    open the (message "...") stream
    //
#ifdef vms
    message_file.fio_create( u_str("SYS$OUTPUT"), 0, 0, u_str("emacs.lis"), default_rms_attribute );
#else
    message_file.fio_open( stdout, default_rms_attribute );
#endif
    if( !message_file.fio_is_open() )
        emacs_exit( errno );

    //
    //    open the get-tty- stream
    //
    if( file != NULL && file[0] != '\0' )
        command_file.fio_open( file, 0, EmacsString::null );
    else
        command_file.fio_open( stdin, FIO_RMS__None );
    if( !command_file.fio_is_open() )
        emacs_exit( errno );
}

TerminalControl_FILE::~TerminalControl_FILE()
{
    t_cleanup();
}

void TerminalControl_FILE::t_cleanup()
{
    command_file.fio_close();
}

void TerminalControl_FILE::t_io_printf(const char *fmt, ...)
{
    va_list argp;
    va_start( argp, fmt );

    char buf[256];
    int size = vsprintf( buf, fmt, argp );

    message_file.fio_put( (const unsigned char *)buf, size );
}

static int async_io;
void start_async_io( void )
{
    //
    //    Initialize VMS asynchronous Keyboard Input
    //
    if( async_io ) return;

    re_init_keyboard();

    input_pending += timer_interrupt_occurred;

    theActiveView->k_input_enable();
    async_io = 1;
}

void stop_async_io( void )
{
    theActiveView->k_input_disable();
    async_io = 0;
}


void rst_dsp( void )
{
    if( ! term_is_terminal )
        return;

    stop_async_io();
#if BROADCAST
    deassign_mbx();
#endif

    theActiveView->t_window( 0 );
    theActiveView->t_cleanup();
}

#ifdef vms
void set_protocol(int flag)
    {
    if( flag )
        theActiveView->t_cur_attributes.v_char_1 |= theActiveView$M_TTSYNC | theActiveView$M_HOSTSYNC;
    else
        theActiveView->t_cur_attributes.v_char_1 &= ~(theActiveView$M_TTSYNC | theActiveView$M_HOSTSYNC);

    theActiveView->t_change_attributes();
}
#else
void set_protocol(int PNOTUSED(flag))
    {
    theActiveView->t_change_attributes();
}
#endif


#ifdef vms
void alter_brd_mbx(int flag)
    {
    if( flag )
    {
        theActiveView->t_cur_attributes.v_char_1 |= theActiveView$M_NOBRDCST;
        theActiveView->t_cur_attributes.l_char_2 |= TT2$M_BRDCSTMBX;
    }
    else
    {
        theActiveView->t_cur_attributes.v_char_1 =
                (theActiveView->t_cur_attributes.v_char_1 & ~theActiveView$M_NOBRDCST)
            |
                (theActiveView->t_user_attributes.v_char_1 & theActiveView$M_NOBRDCST);
        theActiveView->t_cur_attributes.l_char_2 =
                (theActiveView->t_cur_attributes.l_char_2 & ~TT2$M_BRDCSTMBX)
            |
                (theActiveView->t_cur_attributes.l_char_2 & TT2$M_BRDCSTMBX);
    }

    theActiveView->t_change_attributes();
}
#endif



class ActivityTimer : public EmacsTimer
{
public:
    ActivityTimer() { };
    ~ActivityTimer() { };

    virtual EmacsString description() { return "ActivityTimer"; }
    virtual void timeOut();
};

ActivityTimer activity_timer;

void ActivityTimer::timeOut()
{
    if( !activity_indicator )
        return;

    theActiveView->t_display_activity( activity_character );
}

void set_activity_character( unsigned char it )
{
    if( !activity_indicator )
        return;

    // if its a new  activity char...
    if( activity_character != it )
    {
        activity_character = it;

        // only display a new char if the timer gets to expire
        activity_timer.scheduleTimeOut( 1.0 );
    }
}
