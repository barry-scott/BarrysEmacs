//
//  Copyright (c) 2009-2010 Barry A. Scott
//
//
//  pybemacs.cpp
//
#include <emacs.h>

#include "bemacs_python.hpp"

#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
static EmacsInitialisation emacs_initialisation( __DATE__ " " __TIME__, THIS_FILE );

//#include <pwd.h>
//#include <unistd.h>
//#include <time.h>

#include <iostream>

extern int execute_package( const EmacsString &package );


void qqq()
{
}

EmacsString name_arg;

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
extern void init_unicode( void );
extern void init_syntax( void );

class BemacsEditor;

void init_python_terminal( BemacsEditor &editor );

EmacsCommandLineServerWorkItem emacs_command_line_work_item;

BemacsEditorAccessControl editor_access_control;

EmacsString env_emacs_library;
EmacsString env_emacs_user;

class BemacsEditor: public Py::PythonClass< BemacsEditor >
{
public:
    BemacsEditor( Py::PythonClassInstance *self, Py::Tuple &args, Py::Dict &kwds )
    : Py::PythonClass< BemacsEditor >::PythonClass( self, args, kwds )
    , m_value( "default value" )
    {
        EmacsInitialisation::setup_version_string();

        Py::String py_emacs_user( args[0] );
        env_emacs_user = py_emacs_user.as_std_string( "utf-8" );

        Py::String py_emacs_library( args[1] );
        env_emacs_library = py_emacs_library.as_std_string( "utf-8" );
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
        PYCXX_ADD_VARARGS_METHOD( newCommandLine, "newCommandLine( current_directory, list_of_arg_strings )" );

        PYCXX_ADD_NOARGS_METHOD( executeEnterHooks, "executeEnterHooks" );
        PYCXX_ADD_NOARGS_METHOD( executeExitHooks, "executeExitHooks" );

        PYCXX_ADD_NOARGS_METHOD( processKeys, "processKeys" );

        PYCXX_ADD_NOARGS_METHOD( modifiedFilesExist, "modifiedFilesExist" );

        PYCXX_ADD_VARARGS_METHOD( inputChar, "inputChar( char, shift )" );
        PYCXX_ADD_VARARGS_METHOD( inputMouse, "inputMouse( keys, shift, all_params )" );

        PYCXX_ADD_VARARGS_METHOD( geometryChange, "geometryChange( width, height )" );
        PYCXX_ADD_VARARGS_METHOD( setKeysMapping, "setKeysMapping( keys_mapping )" );

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
        {
            PythonAllowThreads permission( editor_access_control );

            init_memory();
            init_unicode();
            init_syntax();
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
        }

        return Py::None();
    }
    PYCXX_NOARGS_METHOD_DECL( BemacsEditor, initEditor )

    //------------------------------------------------------------
    Py::Object newCommandLine( const Py::Tuple &args )
    {
        Py::String py_cwd( args[0] );
        Py::List py_argv( args[1] );

        if( py_argv.length() < 0 )
            throw Py::ValueError( "arg 2 list cannot be empty" );

        EmacsString current_directory( py_cwd.as_std_string( "utf-8" ) );

        EmacsCommandLine new_command_line;

        for( unsigned int argc=0; argc < py_argv.length(); ++argc )
        {
            Py::String py_arg( py_argv[ argc ] );
            std::string std_arg( py_arg.as_std_string( "utf-8" ) );
            new_command_line.addArgument( std_arg );
        }

        emacs_command_line_work_item.newCommandLine( current_directory, new_command_line );

        return Py::None();
    }
    PYCXX_VARARGS_METHOD_DECL( BemacsEditor, newCommandLine )

    //------------------------------------------------------------
    Py::Object executeEnterHooks( void )
    {
        {
            PythonAllowThreads permission( editor_access_control );
            if( user_interface_hook_proc != NULL )
                execute_bound_saved_environment( user_interface_hook_proc );
                // ignore result of user_interface_hook_proc

            if( enter_emacs_proc != NULL )
                execute_bound_saved_environment( enter_emacs_proc );
        }
        return Py::None();
    }
    PYCXX_NOARGS_METHOD_DECL( BemacsEditor, executeEnterHooks )

    Py::Object executeExitHooks( void )
    {
        {
            PythonAllowThreads permission( editor_access_control );
            if( exit_emacs_proc != NULL )
                execute_bound_saved_environment( exit_emacs_proc );
        }

        return Py::None();
    }
    PYCXX_NOARGS_METHOD_DECL( BemacsEditor, executeExitHooks )

    Py::Object processKeys( void )
    {
        int rc;

        {
            PythonAllowThreads permission( editor_access_control );
            rc = process_keys();
        }

        return Py::Long( rc );
    }
    PYCXX_NOARGS_METHOD_DECL( BemacsEditor, processKeys )

    Py::Object modifiedFilesExist( void )
    {
        bool rc;

        {
            PythonAllowThreads permission( editor_access_control );
            rc = mod_exist();
        }

        return Py::Boolean( rc );
    }
    PYCXX_NOARGS_METHOD_DECL( BemacsEditor, modifiedFilesExist )
    //------------------------------------------------------------

    //------------------------------------------------------------
    void hookUserInterface()
    {
        PythonDisallowThreads permission( editor_access_control );

        static char fn_name[] = "hookUserInterface";
        try
        {
            Py::Object self( selfPtr() );
            Py::Callable py_fn( self.getAttr( fn_name ) );

            Py::Tuple all_args( cur_exec->p_nargs );

            for( int i=1; i<=cur_exec->p_nargs; i++ )
            {
                {
                    PythonAllowThreads permission( editor_access_control );

                    if( !eval_arg( i ) )
                        return;
                }

                Py::Object arg( convertEmacsExpressionToPyObject( ml_value ) );
                all_args[ i-1 ] = arg;
            }

            Py::Object result( py_fn.apply( all_args ) );

            ml_value = convertPyObjectToEmacsExpression( result );
        }
        catch( Py::Exception &e )
        {
            ml_value = Expression();

            reportException( fn_name, e );
        }
    }

    //------------------------------------------------------------
    void termCheckForInput()
    {
        PythonDisallowThreads permission( editor_access_control );

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
        PythonDisallowThreads permission( editor_access_control );

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
        PythonDisallowThreads permission( editor_access_control );

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
        PythonDisallowThreads permission( editor_access_control );

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
        PythonDisallowThreads permission( editor_access_control );

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
        PythonDisallowThreads permission( editor_access_control );

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
        PythonDisallowThreads permission( editor_access_control );

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
        PythonDisallowThreads permission( editor_access_control );

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
        t[0] = Py::String( line->line_body, line_length );
        t[1] = attr;

        return t;
    }

    void termUpdateLine( EmacsLinePtr oldl, EmacsLinePtr newl, int ln )
    {
        PythonDisallowThreads permission( editor_access_control );

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
        PythonDisallowThreads permission( editor_access_control );

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
        PythonDisallowThreads permission( editor_access_control );

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
        int ch;

        Py::Object py_arg( args[0] );
        if( py_arg.isString() )
        {
            Py::String py_ch( py_arg );
            EmacsString e_ch( EmacsString::copy, py_ch.unicode_data(), py_ch.size() );
            ch = e_ch[0];
        }
        else
        {
            Py::Long py_ch( py_arg );
            ch = py_ch.as_long();
        }

        Py::Boolean py_shift( args[1] );

        bool shift = py_shift;

        {
            PythonAllowThreads permission( editor_access_control );

            theActiveView->k_input_char( ch, shift );
        }

        return Py::None();
    }
    PYCXX_VARARGS_METHOD_DECL( BemacsEditor, inputChar )

    Py::Object inputMouse( const Py::Tuple &args )
    {
        Py::String py_keys( args[0] );
        Py::Boolean py_shift( args[1] );
        Py::List py_all_params( args[2] );

        EmacsString keys( EmacsString::copy, py_keys.unicode_data(), py_keys.size() );
        bool shift = py_shift;
        std::vector<int> all_params;

        for( size_t i=0; i<py_all_params.size(); i++ )
        {
            Py::Long py_param( py_all_params[ i ] );
            long param( py_param );

            all_params.push_back( param );
        }

        {
            PythonAllowThreads permission( editor_access_control );

            theActiveView->k_input_mouse( keys, shift, all_params );
        }

        return Py::None();
    }
    PYCXX_VARARGS_METHOD_DECL( BemacsEditor, inputMouse )

    Py::Object geometryChange( const Py::Tuple &args )
    {
        Py::Long py_width( args[0] );
        Py::Long py_height( args[1] );
        int width = int( long( py_width ) );
        int height = int( long( py_height ) );

        {
            PythonAllowThreads permission( editor_access_control );

            theActiveView->t_width = width;
            theActiveView->t_length = height;
            theActiveView->t_geometry_change();
        }

        return Py::None();
    }
    PYCXX_VARARGS_METHOD_DECL( BemacsEditor, geometryChange )

    Py::Object setKeysMapping( const Py::Tuple &args )
    {
        Py::Dict keys_mapping( args[0] );
        Py::List all_keys( keys_mapping.keys() );

        for( size_t i=0; i < all_keys.size(); ++i )
        {
            Py::String py_key( all_keys[ i ] );
            Py::String py_value( keys_mapping[ py_key ] );

            EmacsString key( EmacsString::copy, py_key.unicode_data(), py_key.size() );
            EmacsString value( EmacsString::copy, py_value.unicode_data(), py_value.size() );

            // Do not need to allow threads here as this is fast operation
            PC_key_names.addMapping( key, value );
        }

        return Py::None();
    }
    PYCXX_VARARGS_METHOD_DECL( BemacsEditor, setKeysMapping )

    //------------------------------------------------------------
    Py::Object getattro( const Py::String &name_ )
    {
        std::string name( name_.as_std_string( "utf-8" ) );

        if( name == "value" )   // QQQ placeholder
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

        if( name == "value" )   // QQQ placeholder
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
        add_varargs_method( "call_function", &BemacsModule::call_bemacs_function, "call bemacs function" );
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

        return Py::None();
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

        {
            PythonAllowThreads permission( editor_access_control );

            exec_prog( &prog_node );
        }

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

extern "C" EXPORT_SYMBOL void init_bemacs()
{
#if defined(PY_WIN32_DELAYLOAD_PYTHON_DLL)
    Py::InitialisePythonIndirectPy::Interface();
#endif

    bemacs_module = new BemacsModule;
}

// symbol required for the debug version
extern "C" EXPORT_SYMBOL void init_bemacs_d()
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

    //
    //  User Interface routines
    //
    virtual void t_user_interface_hook()
    {
        m_editor.hookUserInterface();
    }

    //
    //  Screen routines
    //

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


int ui_python_hook()
{
    if( check_args( 1, 0 ) )
        return 0;

    theActiveView->t_user_interface_hook();
    return 0;
}

void EmacsCommandLineServerWorkItem::workAction()
{
    // save the current directory as the previous
    previous_directory = current_directory.asString();

    // first change directory
    chdir_and_set_global_record( m_command_current_directory );

    // set the command line arguments
    command_line_arguments = m_command_line;

    // try the package
    touched_command_args = false;
    int rv = execute_package( command_line_arguments.argument(0).value() );

    // if the the package did not touch the args read in files
    if( rv == 0 && !touched_command_args )
        read_in_files();

    // revert back to the previous directory
    // the package may have changed previous directory.
    // ignore it if its blank
    if( !previous_directory.isNull() )
    {
        EmacsString new_prev = current_directory.asString();
        chdir_and_set_global_record( previous_directory.asString() );

        // save package current dir in previous dir
        previous_directory = new_prev;
    }
}
