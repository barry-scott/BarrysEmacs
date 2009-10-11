//
//  Copyright (c) 2009 Barry A. Scott
//
//
//  pybemacs.cpp
//
//  This module defines a single function.
//
#include <emacs.h>

#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
static EmacsInitialisation emacs_initialisation( __DATE__ " " __TIME__, THIS_FILE );

#include "CXX/Objects.hxx"
#include "CXX/Extensions.hxx"

#include <pwd.h>
#include <unistd.h>
#include <time.h>

static struct timeval emacs_start_time;
static EmacsString image_path;

extern void init_fncs( void );
extern void init_var( void );
extern void init_bf( void );
extern void init_scheduled_timeout( void );
extern void init_display( void );
extern void init_terminal( const EmacsString &device_name, const EmacsString &term_type );
extern void init_dsp( void );
extern void init_win( void );
extern void init_srch( void );
extern void init_undo( void );
extern void init_lisp( void );
extern void init_abs( void );
extern void init_key( void );
extern void init_fncs2( void );

void init_python_terminal();

class BemacsEditor: public Py::PythonClass< BemacsEditor >
{
public:
    BemacsEditor( Py::PythonClassInstance *self, Py::Tuple &args, Py::Dict &kwds )
    : Py::PythonClass< BemacsEditor >::PythonClass( self, args, kwds )
    , m_value( "default value" )
    {
        // record the start time
        gettimeofday( &emacs_start_time, NULL );

        init_fncs();                            // initialise the key bindings
        init_var();                             // " the variables
        init_bf();                              // " the buffer system
        init_scheduled_timeout();
        init_display();                         // " the core display system
        //init_terminal( "file", "" );          // Start the primary terminal IO system
        init_python_terminal();
        init_dsp();
        init_win();                             // " the window system
        init_srch();                            // " the search commands
        init_undo();                            // " the undo facility
        init_lisp();                            // " the MLisp system
        init_abs();                             // " the current directory name
        init_key();                             // " commands that deal with options
        current_global_map = global_map;
        init_fncs2();                           // Finish off init of functions
    }

    virtual ~BemacsEditor()
    {
        std::cout << "~BemacsEditor." << std::endl;
    }

    static void init_type(void)
    {
        behaviors().name( "BemacsEditor" );
        behaviors().doc( "documentation for BemacsEditor class" );
        behaviors().supportGetattro();
        behaviors().supportSetattro();

        PYCXX_ADD_NOARGS_METHOD( BemacsEditor_func_noargs, "docs for BemacsEditor_func_noargs" );
        PYCXX_ADD_VARARGS_METHOD( BemacsEditor_func_varargs, "docs for BemacsEditor_func_varargs" );
        PYCXX_ADD_KEYWORDS_METHOD( BemacsEditor_func_keyword, "docs for BemacsEditor_func_keyword" );

        // Call to make the type ready for use
        behaviors().readyType();
    }

    Py::Object BemacsEditor_func_noargs( void )
    {
        std::cout << "BemacsEditor_func_noargs Called." << std::endl;
        std::cout << "value ref count " << m_value.reference_count() << std::endl;
        return Py::None();
    }
    PYCXX_NOARGS_METHOD_DECL( BemacsEditor, BemacsEditor_func_noargs )

    Py::Object BemacsEditor_func_varargs( const Py::Tuple &args )
    {
        std::cout << "BemacsEditor_func_varargs Called with " << args.length() << " normal arguments." << std::endl;
        return Py::None();
    }
    PYCXX_VARARGS_METHOD_DECL( BemacsEditor, BemacsEditor_func_varargs )

    Py::Object BemacsEditor_func_keyword( const Py::Tuple &args, const Py::Dict &kwds )
    {
        std::cout << "BemacsEditor_func_keyword Called with " << args.length() << " normal arguments." << std::endl;
        Py::List names( kwds.keys() );
        std::cout << "and with " << names.length() << " keyword arguments:" << std::endl;
        for( Py::List::size_type i=0; i< names.length(); i++ )
        {
            Py::String name( names[i] );
            std::cout << "    " << name << std::endl;
        }
        return Py::None();
    }
    PYCXX_KEYWORDS_METHOD_DECL( BemacsEditor, BemacsEditor_func_keyword )

    Py::Object getattro( const Py::String &name_ )
    {
        std::string name( name_.as_std_string( "utf-8" ) );

        if( name == "value" )
        {
            return m_value;
        }
        else
        {
            return genericGetAttro( name_ );
        }
    }

    int setattro( const Py::String &name_, const Py::Object &value )
    {
        std::string name( name_.as_std_string( "utf-8" ) );

        if( name == "value" )
        {
            m_value = value;
            return 0;
        }
        else
        {
            return genericSetAttro( name_, value );
        }
    }


    Py::String m_value;
};

class BemacsModule: public Py::ExtensionModule<BemacsModule>
{
public:
    BemacsModule()
    : Py::ExtensionModule<BemacsModule>( "_bemacs" ) // this must be name of the file on disk e.g. bemacs.so or bemacs.pyd
    {
        BemacsEditor::init_type();
    

        // after initialize the moduleDictionary will exist
        initialize( "Barry's Emacs" );

        Py::Dict d( moduleDictionary() );
        Py::Object x( BemacsEditor::type() );
        d["editor"] = x;
    }

    virtual ~BemacsModule()
    {}

private:
};

#if defined( PY3 )
extern "C" PyObject *PyInit__bemacs()
{
#if defined(PY_WIN32_DELAYLOAD_PYTHON_DLL)
    Py::InitialisePythonIndirectPy::Interface();
#endif

    static BemacsModule *bemacs = new BemacsModule;
    return bemacs->module().ptr();
}

// symbol required for the debug version
extern "C" PyObject *PyInit__bemacs_d()
{ 
    return PyInit__bemacs();
}

#else
static BemacsModule *bemacs_module = NULL;

extern "C" void init_bemacs()
{
#if defined(PY_WIN32_DELAYLOAD_PYTHON_DLL)
    Py::InitialisePythonIndirectPy::Interface();
#endif

    bemacs_module = new BemacsModule;
}

// symbol required for the debug version
extern "C" void init_bemacs_d()
{ 
    init_bemacs();
}
#endif

// QQQ
SystemExpressionRepresentationIntBoolean force_redisplay;

SystemExpressionRepresentationInt ui_open_file_readonly;
SystemExpressionRepresentationString ui_open_file_name;
SystemExpressionRepresentationString ui_save_as_file_name;
SystemExpressionRepresentationString ui_filter_file_list;

SystemExpressionRepresentationString ui_search_string;
SystemExpressionRepresentationString ui_replace_string;

class TerminalControl_Python: public EmacsView
{
public:
    TerminalControl_Python()
    : EmacsView()
    {
    }

    virtual ~TerminalControl_Python()
    {
    }

    //void *operator new(size_t size);
    //void operator delete(void *p);

    void update_screen( int slow_update )
    {
    }

    //
    //    Keyboard routines
    //
    virtual void k_check_for_input()   // check for any input
    {
    }

    virtual int k_input_event( unsigned char *, unsigned int )
    {
        return 0;
    }
};


void init_python_terminal()
{
    term_is_terminal = 3;
    theActiveView = new TerminalControl_Python();
}

int ui_frame_to_foreground(void)
{
    return 0;
}

int wait_for_activity(void)
{
    return -1;
}


void emacs_sleep( int milli_seconds )
{
    struct timespec request;
    request.tv_sec = milli_seconds/1000;        // seconds
    request.tv_nsec = (milli_seconds%1000)*1000000;    // convert milli to nano
    int rc = nanosleep( &request, NULL );
    if( rc == 0 )
        return;
    emacs_assert( errno == EINTR );
}

void _dbg_msg( const EmacsString &msg )
{
    fprintf( stderr, "%s", msg.sdata() );
    if( msg[-1] != '\n' )
        fprintf( stderr, "\n" );
    fflush( stderr );
}

int interlock_dec( volatile int *cell )
{
    (*cell)--;
    if( *cell == 0 )
        return 0;
    if( *cell < 0 )
        return -1;
    else
        return 1;
}

int interlock_inc( volatile int *cell )
{
    (*cell)++;
    if( *cell == 0 )
        return 0;
    if( *cell < 0 )
        return -1;
    else
        return 1;
}

void conditional_wake(void)
{
    return;
}

void wait_abit(void)
{
}

int elapse_time()
{
    struct timeval now;
    gettimeofday( &now, NULL );

    //
    //    calculate the time since startup in mSec.
    //    we ignore the usec part of the start time
    //    (assuming its 0)
    //
    int elapse_time = (int)(now.tv_sec - emacs_start_time.tv_sec);
    elapse_time *= 1000;
    elapse_time += (int)(now.tv_usec/1000);

    return elapse_time;
}


extern void init_memory();

void EmacsInitialisation::os_specific_init()
{
    init_memory();
#ifdef SAVE_ENVIRONMENT
    //
    //    Create the save environment object at the earlest opertunity
    //
    EmacsSaveRestoreEnvironmentObject = EMACS_NEW EmacsSaveRestoreEnvironment;
#endif
}

EmacsString get_user_full_name()
{
    uid_t uid = geteuid();
    struct passwd *pw = getpwuid( uid );

    if( pw == NULL )
        return EmacsString::null;
    else
        return EmacsString( pw->pw_gecos );
}

EmacsString users_login_name()
{
    uid_t uid = geteuid();
    struct passwd *pw = getpwuid( uid );

    if( pw == NULL )
        return EmacsString::null;

    return EmacsString( pw->pw_name );
}


EmacsString get_system_name()
{
    char system_name[256];
    if( gethostname( system_name, sizeof( system_name ) ) == 0 )
        return EmacsString( system_name );
    else
        return EmacsString::null;
}

void fatal_error( int code )
{
    printf("\nFatal Error %d\n", code );
    exit(1);
}

EmacsDateTime EmacsDateTime::now(void)
{
    EmacsDateTime now;


    struct timeval t;
    gettimeofday(  &t, NULL );

    now.time_value = double( t.tv_usec ) / 1000000.0;
    now.time_value += double( t.tv_sec );

    return now;
}

EmacsString EmacsDateTime::asString(void) const
{
    double int_part, frac_part;

    frac_part = modf( time_value, &int_part );
    frac_part *= 1000.0;

    time_t clock = int( int_part );
    int milli_sec = int( frac_part );

    struct tm *tm = localtime( &clock );

    return FormatString("%4d-%2d-%2d %2d:%2d:%2d.%3.3d")
        << tm->tm_year + 1900 << tm->tm_mon + 1 << tm->tm_mday
        << tm->tm_hour << tm->tm_min << tm->tm_sec << milli_sec;
}

EmacsString os_error_code( unsigned int code )
{
    const char *error_string = strerror( code );
    if( error_string == NULL )
        return EmacsString( FormatString("Unix error code %d") << code );
    else
        return EmacsString( error_string );
}

bool emacs_internal_init_done_event(void)
{
    return true;
}

void UI_update_window_title( void )
{
}

int init_gui_terminal( const EmacsString & )
{
    return 0;
}

int init_char_terminal( const EmacsString & )
{
    return 0;
}

const int TIMER_TICK_VALUE( 50 );
static void( *timeout_handler )(void );
struct timeval timeout_time;

void time_schedule_timeout( void( *time_handle_timeout )(void ), const EmacsDateTime &when  )
{
    int delta = int( EmacsDateTime::now().asDouble() - when.asDouble() );

    struct timezone tzp;
    gettimeofday( &timeout_time, &tzp  );

    timeout_time.tv_sec += delta/1000;
    timeout_time.tv_usec +=( delta%1000 )*1000;
    if( timeout_time.tv_usec > 1000000  )
        {
            timeout_time.tv_sec += 1;
            timeout_time.tv_usec -= 1000000;
        }
    timeout_handler = time_handle_timeout;
}

void time_cancel_timeout(void)
{
    timeout_time.tv_sec = 0;
    timeout_time.tv_usec = 0;
    timeout_handler = NULL;
}

EmacsString get_config_env( const EmacsString &name )
{
    char *value = getenv( name );

    if( value != NULL )
        return value;

    static EmacsString env_emacs_path(  "emacs_user: emacs_library:" );
    if( name == "emacs_path" )
        return env_emacs_path;

    static EmacsString env_emacs_user(  "HOME:/bemacs" );
    if( name == "emacs_user" )
        return env_emacs_user;

    static EmacsString env_emacs_library(  image_path );
    if( name == "emacs_library" )
        return env_emacs_library;

    static EmacsString env_sys_login(  "HOME:/" );
    if( name == "sys_login" )
        return env_sys_login;

    return EmacsString::null;
}

void debug_invoke(void)
{
    return;
}

void debug_SER(void)
{
    return;
}

void debug_exception(void)
{
    return;
}


#undef NDEBUG
#include <assert.h>

void _emacs_assert( const char *exp, const char *file, unsigned line )
{
#if defined( __FreeBSD__ )
    // freebsd assert order
    __assert( "unknown", file, line, exp );

#elif defined( __APPLE_CC__ )
    #if __DARWIN_UNIX03
        __assert_rtn( __func__, __FILE__, __LINE__, exp );
    #else /* !__DARWIN_UNIX03 */
        __assert( exp, __FILE__, __LINE__ );
    #endif /* __DARWIN_UNIX03 */

#elif defined( __GNUC__ ) && __GNUC__ >= 3
    // unix assert order
    __assert( exp, file, line );

#else
    // unix assert order
    __assert( file, line, exp );

#endif
}
