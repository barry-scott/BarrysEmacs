//
//  Copyright (c) 2009-2010 Barry A. Scott
//
//
//  pybemacs.cpp
//
//  This module defines a single function.
//
#include <emacs.h>

#include "bemacs_python.hpp"

#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
static EmacsInitialisation emacs_initialisation( __DATE__ " " __TIME__, THIS_FILE );

#include <pwd.h>
#include <unistd.h>
#include <time.h>

#include <iostream>

extern int execute_package( const EmacsString &package );


void qqq()
{
}

EmacsString name_arg;

static struct timeval emacs_start_time;
static EmacsString image_path;

extern void init_fncs( void );
extern void init_var( void );
extern void init_bf( void );
extern void init_scheduled_timeout( void );
extern void init_display( void );
extern void init_dsp( void );
extern void init_win( void );
extern void init_srch( void );
extern void init_undo( void );
extern void init_lisp( void );
extern void init_abs( void );
extern void init_key( void );
extern void init_fncs2( void );

class BemacsEditor;

void init_python_terminal( BemacsEditor &editor );

class BemacsEditor: public Py::PythonClass< BemacsEditor >
{
public:
    BemacsEditor( Py::PythonClassInstance *self, Py::Tuple &args, Py::Dict &kwds )
    : Py::PythonClass< BemacsEditor >::PythonClass( self, args, kwds )
    , m_value( "default value" )
    {
        // record the start time
        gettimeofday( &emacs_start_time, NULL );
        EmacsInitialisation::setup_version_string();

    }

    virtual ~BemacsEditor()
    {
    }

    static void init_type(void)
    {
        behaviors().name( "BemacsEditor" );
        behaviors().doc( "documentation for BemacsEditor" );
        behaviors().supportGetattro();
        behaviors().supportSetattro();

        PYCXX_ADD_NOARGS_METHOD( initEditor, "initEditor" );
        PYCXX_ADD_VARARGS_METHOD( processCommandLine, "processCommandLine" );

        PYCXX_ADD_NOARGS_METHOD( executeEnterHooks, "executeEnterHooks" );
        PYCXX_ADD_NOARGS_METHOD( executeExitHooks, "executeExitHooks" );

        PYCXX_ADD_NOARGS_METHOD( processKeys, "processKeys" );

        PYCXX_ADD_NOARGS_METHOD( modifiedFilesExist, "modifiedFilesExist" );

        PYCXX_ADD_VARARGS_METHOD( inputChar, "inputChar( char, shift )" );
        PYCXX_ADD_VARARGS_METHOD( geometryChange, "geometryChange( width, height )" );

        PYCXX_ADD_KEYWORDS_METHOD( BemacsEditor_func_keyword, "BemacsEditor_func_keyword" );

        // Call to make the type ready for use
        behaviors().readyType();
    }

    void reportException( const char *fn_name, Py::Exception &e )
    {
        std::cerr << "Error: " << fn_name << " exception" << std::endl;
        std::cerr << " type=" << Py::type( e ) << std::endl;
        std::cerr << "value=" << Py::value( e ) << std::endl;
        std::cerr << "trace=" << Py::trace( e ) << std::endl;
        e.clear();
    }

    //------------------------------------------------------------
    Py::Object initEditor( void )
    {
        init_memory();
        init_display();                         // " the core display system

        init_fncs();                            // initialise the key bindings
        init_var();                             // " the variables
        init_bf();                              // " the buffer system
        init_scheduled_timeout();
        init_display();                         // " the core display system
        init_python_terminal( *this );          // " the terminal
        init_dsp();
        init_win();                             // " the window system
        init_srch();                            // " the search commands
        init_undo();                            // " the undo facility
        init_lisp();                            // " the MLisp system
        init_abs();                             // " the current directory name
        init_key();                             // " commands that deal with options
        current_global_map = global_map;

        init_fncs2();                           // Finish off init of functions

        EmacsWorkItem::enableWorkQueue( true );
        start_async_io();

        return Py::None();
    }
    PYCXX_NOARGS_METHOD_DECL( BemacsEditor, initEditor )

    //------------------------------------------------------------
    Py::Object processCommandLine( const Py::Tuple &args )
    {
        Py::List py_argv( args[0] );

        char **argv = reinterpret_cast<char **>( EMACS_MALLOC( (py_argv.length()+1) * sizeof( char * ), malloc_type_char ) );

        for( unsigned int argc=0; argc < py_argv.length(); ++argc )
        {
            Py::String py_arg( py_argv[ argc ] );
            std::string std_arg( py_arg.as_std_string( "utf-8" ) );

            char *arg = reinterpret_cast<char *>( EMACS_MALLOC( std_arg.size() + 1, malloc_type_char ) );
            memcpy( arg, std_arg.c_str(), std_arg.size() );
            arg[ std_arg.size() ] = 0;

            argv[ argc ] = arg;
        }
        argv[ py_argv.length() ] = NULL;

        process_args( py_argv.length(), argv );

        for( int argc=0; argc < py_argv.length(); ++argc )
        {
            EMACS_FREE( argv[ argc ] );
        }
        EMACS_FREE( argv );

        execute_package( command_line_arguments.argument(0).value() );

        return Py::None();
    }
    PYCXX_VARARGS_METHOD_DECL( BemacsEditor, processCommandLine )

    //------------------------------------------------------------
    void process_args( int argc, char **argv )
    {
        command_line_arguments.setArguments( argc, argv );
        command_line_arguments.setArgument( 0, "emacs", false );

        int arg = 1;

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

                if( key_string.commonPrefix( "package" ) > 2 )
                {
                    command_line_arguments.setArgument( 0, val_string, false );
                }
                else if( key_string.commonPrefix( "name" ) > 3 )
                {
                    name_arg = val_string;
                }
                else
                {
                    delete_this_arg = false;
                }

                if( delete_this_arg )
                {
                    command_line_arguments.deleteArgument( arg );
                }
                else
                {
                    arg++;
                }
            }
            else
            {
                arg++;
            }
        }
    }
    //------------------------------------------------------------

    //------------------------------------------------------------
    Py::Object executeEnterHooks( void )
    {
        if( user_interface_hook_proc != NULL )
            execute_bound_saved_environment( user_interface_hook_proc );
            // ignore result of user_interface_hook_proc

        if( enter_emacs_proc != NULL )
            execute_bound_saved_environment( enter_emacs_proc );

        return Py::None();
    }
    PYCXX_NOARGS_METHOD_DECL( BemacsEditor, executeEnterHooks )

    Py::Object executeExitHooks( void )
    {
        if( exit_emacs_proc != NULL )
            execute_bound_saved_environment( exit_emacs_proc );

        return Py::None();
    }
    PYCXX_NOARGS_METHOD_DECL( BemacsEditor, executeExitHooks )

    Py::Object processKeys( void )
    {
        return Py::Long( process_keys() );
    }
    PYCXX_NOARGS_METHOD_DECL( BemacsEditor, processKeys )

    Py::Object modifiedFilesExist( void )
    {
        return Py::Boolean( mod_exist() );
    }
    PYCXX_NOARGS_METHOD_DECL( BemacsEditor, modifiedFilesExist )
    //------------------------------------------------------------

    //------------------------------------------------------------
    void termCheckForInput()
    {
        static char fn_name[] = "termCheckForInput";
        try
        {
            Py::Object self( selfPtr() );
            Py::Callable py_fn( self.getAttr( fn_name ) );
            Py::Tuple args( 0 );
            py_fn.apply( args );
        }
        catch( Py::Exception &e )
        {
            reportException( fn_name, e );
        }
    }

    //------------------------------------------------------------
    int termWaitForActivity( void )
    {
        static char fn_name[] = "termWaitForActivity";

        try
        {
            Py::Object self( selfPtr() );
            Py::Callable py_waitForActivity( self.getAttr( fn_name ) );
            Py::Tuple args( 0 );
            Py::Object rc( py_waitForActivity.apply( args ) );
            Py::Long code( rc );
            return long( code );
        }
        catch( Py::Exception &e )
        {
            reportException( fn_name, e );
        }
        return -1;
    }

    void termTopos( int x, int y )
    {
        static char fn_name[] = "termTopos";
        try
        {
            Py::Object self( selfPtr() );
            Py::Callable py_fn( self.getAttr( fn_name ) );
            Py::Tuple args( 2 );
            args[0] = Py::Long( x );
            args[1] = Py::Long( y );
            py_fn.apply( args );
        }
        catch( Py::Exception &e )
        {
            reportException( fn_name, e );
        }
    }

    void termReset( void )
    {
        static char fn_name[] = "termReset";
        try
        {
            Py::Object self( selfPtr() );
            Py::Callable py_fn( self.getAttr( fn_name ) );
            Py::Tuple args( 0 );
            py_fn.apply( args );
        }
        catch( Py::Exception &e )
        {
            reportException( fn_name, e );
        }
    }

    void termInit( void )
    {
        static char fn_name[] = "termInit";
        try
        {
            Py::Object self( selfPtr() );
            Py::Callable py_fn( self.getAttr( fn_name ) );
            Py::Tuple args( 0 );
            py_fn.apply( args );
        }
        catch( Py::Exception &e )
        {
            reportException( fn_name, e );
        }
    }

    void termBeep( void )
    {
        static char fn_name[] = "termBeep";
        try
        {
            Py::Object self( selfPtr() );
            Py::Callable py_fn( self.getAttr( fn_name ) );
            Py::Tuple args( 0 );
            py_fn.apply( args );
        }
        catch( Py::Exception &e )
        {
            reportException( fn_name, e );
        }
    }

    bool termUpdateBegin( void )
    {
        static char fn_name[] = "termUpdateBegin";
        try
        {
            Py::Object self( selfPtr() );
            Py::Callable py_fn( self.getAttr( fn_name ) );
            Py::Tuple args( 0 );
            Py::Boolean rc( py_fn.apply( args ) );
            return bool( rc );
        }
        catch( Py::Exception &e )
        {
            reportException( fn_name, e );
        }
        return false;
    }

    void termUpdateEnd( void )
    {
        static char fn_name[] = "termUpdateEnd";
        try
        {
            Py::Object self( selfPtr() );
            Py::Callable py_fn( self.getAttr( fn_name ) );
            Py::Tuple args( 0 );
            py_fn.apply( args );
        }
        catch( Py::Exception &e )
        {
            reportException( fn_name, e );
        }
    }

    Py::Object convertEmacsLine( EmacsLinePtr line )
    {
        if( line.isNull() )
        {
            return Py::None();
        }

        int line_length = line->line_length;

        Py::List attr;
        DisplayAttr_t *values = line->line_attr;
        for( int i=0; i<line_length; ++i )
        {
            attr.append( Py::Long( *values++ ) );
        }
        Py::Tuple t( 2 );
        t[0] = Py::String( reinterpret_cast<const char *>( line->line_body ), line_length );
        t[1] = attr;

        return t;
    }

    void termUpdateLine( EmacsLinePtr oldl, EmacsLinePtr newl, int ln )
    {
        static char fn_name[] = "termUpdateLine";
        try
        {
            Py::Object self( selfPtr() );
            Py::Callable py_fn( self.getAttr( fn_name ) );

            Py::Tuple args( 3 );
            args[0] = convertEmacsLine( oldl );
            args[1] = convertEmacsLine( newl );
            args[2] = Py::Long( ln );

            py_fn.apply( args );
        }
        catch( Py::Exception &e )
        {
            reportException( fn_name, e );
        }
    }

    void termMoveLine( int from_line, int to_line )
    {
        static char fn_name[] = "termMoveLine";
        try
        {
            Py::Object self( selfPtr() );
            Py::Callable py_fn( self.getAttr( fn_name ) );

            Py::Tuple args( 2 );
            args[0] = Py::Long( from_line );
            args[1] = Py::Long( to_line );

            py_fn.apply( args );
        }
        catch( Py::Exception &e )
        {
            reportException( fn_name, e );
        }
    }

    void termDisplayActivity( unsigned char ch )
    {
        static char fn_name[] = "termDisplayActivity";
        try
        {
            Py::Object self( selfPtr() );
            Py::Callable py_fn( self.getAttr( fn_name ) );
            Py::Tuple args( 1 );
            args[0] = Py::Long( ch );
            py_fn.apply( args );
        }
        catch( Py::Exception &e )
        {
            reportException( fn_name, e );
        }
    }

    //------------------------------------------------------------
    Py::Object inputChar( const Py::Tuple &args )
    {
        Py::String py_ch( args[0] );
        Py::Boolean shift( args[1] );

        std::string ch( py_ch );
        theActiveView->k_input_char( ch[0], shift );
        return Py::None();
    }
    PYCXX_VARARGS_METHOD_DECL( BemacsEditor, inputChar )

    Py::Object geometryChange( const Py::Tuple &args )
    {
        Py::Long width( args[0] );
        Py::Long height( args[1] );

        theActiveView->t_width = int( long( width ) );
        theActiveView->t_length = int( long( height ) );
        theActiveView->t_geometry_change();
        return Py::None();
    }
    PYCXX_VARARGS_METHOD_DECL( BemacsEditor, geometryChange )

    //------------------------------------------------------------
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
        //
        // init types used by this module
        //
        BemacsEditor::init_type();
        BemacsVariables::init_type();
        BemacsFunctions::init_type();
        BemacsMarker::init_type();
        BemacsWindowRing::init_type();
        BemacsArray::init_type();
        BemacsBuffersDict::init_type();
        BemacsBuffer::init_type();
        BemacsBufferSyntax::init_type();
        BemacsBufferData::init_type();

        //
        //    Add the methods of this module
        //
        add_varargs_method( "call_function", &BemacsModule::call_bemacs_function, "call bemasc function" );
        add_varargs_method( "report_error", &BemacsModule::report_error,
            "report_error\n"
            "\n"
            "report_error( error_message_string )\n"
            "Emacs will report the error_message_string as the\n"
            "result of the currently executing Python code"
        );

        // after initialize the moduleDictionary will exist
        initialize( "Barry's Emacs" );

        Py::Dict d( moduleDictionary() );
        Py::Object x( BemacsEditor::type() );
        d["BemacsEditor"] = x;
        d["variable"] = Py::Object( new BemacsVariables );
        d["function"] = Py::Object( new BemacsFunctions );
        d["buffers"] = Py::Object( new BemacsBuffersDict );
    }

    virtual ~BemacsModule()
    {}

    Py::Object report_error( const Py::Tuple &args )
    {
        args.verify_length(1);
        Py::String error_message( args[0] );

        return Py::Object();
    }

    Py::Object call_bemacs_function( const Py::Tuple &args )
    {
        Py::String py_fn_name( args[0] );
        std::string std_fn_name( py_fn_name );
        EmacsString name( std_fn_name.c_str() );

        BoundName *fn_binding = BoundName::find( name );
        if( fn_binding == NULL || !fn_binding->isBound() )
            throw Py::NameError(std_fn_name);

        ProgramNodeNode prog_node( fn_binding, args.size() - 1 );

        for( unsigned int arg=1; arg<args.size(); arg++ )
        {
            Py::Object x( args[arg] );
            Expression expr( convertPyObjectToEmacsExpression( x ) );

            // must new the ProgramNodeExpression as its deleted via the NodeNode d'tor
            prog_node.pa_node[arg-1] = new ProgramNodeExpression( expr );
        }


        exec_prog( &prog_node );
        if( ml_err )
        {
            ml_err = 0;
            throw Py::RuntimeError( error_message_text.sdata() );
        }

        Py::Object result = convertEmacsExpressionToPyObject( ml_value );

        return result;
    }

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

int ui_edit_copy()
{
    return 0;
}

int ui_edit_paste()
{
    return 0;
}

int ui_frame_state()
{
    ml_value = Expression( "normal" );

    return 0;
}

int ui_frame_minimize()
{
    return 0;
}

int ui_frame_restore()
{
    return 0;
}

int ui_add_to_recent_file_list()
{
    return 0;
}

class TerminalControl_Python: public EmacsView
{
public:
    TerminalControl_Python( BemacsEditor &editor )
    : EmacsView()
    , m_editor( editor )
    {
    t_il_mf = 1;
    t_il_ov = 1;
    t_ic_ov = MISSINGFEATURE;
    t_dc_ov = MISSINGFEATURE;
    t_baud_rate = 1000000;
    }

    virtual ~TerminalControl_Python()
    {
    }

    //
    //    Keyboard routines
    //
    virtual void k_check_for_input()   // check for any input
    {
        m_editor.termCheckForInput();
    }

    // move the cursor to the indicated (row,column); (1,1) is the upper left
    virtual void t_topos( int row, int column )
    {
        m_editor.termTopos( row, column );
    }

    // reset terminal (screen is in unknown state, convert it to a known one)
    virtual void t_reset()
    {
        m_editor.termReset();
    }

    virtual bool t_update_begin()
    {
        return m_editor.termUpdateBegin();
    }

    // Routine to call to update a line
    virtual void t_update_line( EmacsLinePtr oldl, EmacsLinePtr newl, int ln )
    {
        m_editor.termUpdateLine( oldl, newl, ln );
    }

    // set the screen window so that IDline operations only affect the first n lines of the screen
    virtual void t_move_line( int from_line, int to_line )
    {
        m_editor.termMoveLine( from_line, to_line );
    }

    virtual void t_update_end()
    {
        m_editor.termUpdateEnd();
    }

    // initialize terminal settings
    virtual void t_init()
    {
        m_editor.termInit();
    }

    // erase to the end of the line
    virtual void t_wipe_line( int n )
    {
    }

    // Flash the screen -- not set if this terminal type won't support it.
    virtual void t_flash()
    {
    }

    virtual void t_display_activity( unsigned char ch )
    {
        m_editor.termDisplayActivity( ch );
    }

    virtual void t_beep()
    {
        m_editor.termBeep();
    }

    BemacsEditor &m_editor;
};


void init_python_terminal( BemacsEditor &editor )
{
    term_is_terminal = 3;
    theActiveView = new TerminalControl_Python( editor );
}

int ui_frame_to_foreground(void)
{
    return 0;
}

int wait_for_activity(void)
{
    return reinterpret_cast<TerminalControl_Python *>( theActiveView )->m_editor.termWaitForActivity();
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
    char *value = getenv( name.sdata() );

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
