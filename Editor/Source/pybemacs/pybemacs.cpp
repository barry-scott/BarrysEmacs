//
//  Copyright (c) 2009-2016 Barry A. Scott
//
//
//  pybemacs.cpp
//
#include <emacs.h>

#include "bemacs_python.hpp"

#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
static EmacsInitialisation emacs_initialisation( __DATE__ " " __TIME__, THIS_FILE );

#include <iostream>

extern int execute_package( const EmacsString &package );

extern SystemExpressionRepresentationIntBoolean synchronise_buffers_on_focus;

EmacsString name_arg;

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

extern int vertical_bar_width;

class BemacsEditor;
class BemacsModule;

static BemacsModule *bemacs_module = NULL;

void init_python_terminal( BemacsEditor &editor );

EmacsCommandLineServerWorkItem emacs_command_line_work_item;

class SynchroniseFilesWorkItem : public EmacsWorkItem
{
public:
    virtual void workAction(void)
    {
        synchronise_files();
    }
};

SynchroniseFilesWorkItem synchronise_files_work_item;

BemacsEditorAccessControl editor_access_control;

EmacsString env_emacs_library;
EmacsString env_emacs_user;

class BemacsEditor: public Py::PythonClass< BemacsEditor >
{
public:
    BemacsEditor( Py::PythonClassInstance *self, Py::Tuple &args, Py::Dict &kwds )
    : Py::PythonClass< BemacsEditor >::PythonClass( self, args, kwds )
    , m_value( "default value" )
    , m_display_scroll_bars( true )
    , m_display_status_bar( true )
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

        PYCXX_ADD_NOARGS_METHOD( initEditor, py_initEditor, "initEditor" );
        PYCXX_ADD_VARARGS_METHOD( newCommandLine, py_newCommandLine, "newCommandLine( current_directory, list_of_arg_strings )" );

        PYCXX_ADD_NOARGS_METHOD( executeEnterHooks, py_executeEnterHooks, "executeEnterHooks" );
        PYCXX_ADD_NOARGS_METHOD( executeExitHooks, py_executeExitHooks, "executeExitHooks" );

        PYCXX_ADD_NOARGS_METHOD( processKeys, py_processKeys, "processKeys" );

        PYCXX_ADD_NOARGS_METHOD( modifiedFilesExist, py_modifiedFilesExist, "modifiedFilesExist" );

        PYCXX_ADD_VARARGS_METHOD( inputChar, py_inputChar, "inputChar( char, shift )" );
        PYCXX_ADD_VARARGS_METHOD( inputMouse, py_inputMouse, "inputMouse( keys, shift, all_params )" );

        PYCXX_ADD_VARARGS_METHOD( scrollChangeVert, py_scrollChangeVert, "scrollChangeVert( win_id, change )" );
        PYCXX_ADD_VARARGS_METHOD( scrollChangeHorz, py_scrollChangeHorz, "scrollChangeHorz( win_id, change )" );
        PYCXX_ADD_VARARGS_METHOD( scrollSetVert, py_scrollSetVert, "scrollSetVert( win_id, value )" );
        PYCXX_ADD_VARARGS_METHOD( scrollSetHorz, py_scrollSetHorz, "scrollSetHorz( win_id, value )" );

        PYCXX_ADD_VARARGS_METHOD( geometryChange, py_geometryChange, "geometryChange( width, height )" );
        PYCXX_ADD_VARARGS_METHOD( setKeysMapping, py_setKeysMapping, "setKeysMapping( keys_mapping )" );

        PYCXX_ADD_VARARGS_METHOD( hasFocus, py_hasFocus, "hasFocus()" );

        // Call to make the type ready for use
        behaviors().readyType();
    }

    void reportException( const std::string &fn_name, Py::Exception &e )
    {
        std::cerr << "Error: " << fn_name << " exception" << std::endl;
        std::cerr << " type=" << Py::type( e ) << std::endl;
        std::cerr << "value=" << Py::value( e ) << std::endl;
        std::cerr << "trace=" << Py::trace( e ) << std::endl;
        e.clear();
    }


    //------------------------------------------------------------
    Py::Object py_initEditor( void )
    {
        // setup the debug flags
        EmacsString flags( get_config_env( "EMACS_DEBUG" ) );
        if( !flags.isNull() )
        {
            EmacsString debug_file( get_config_env( "EMACS_DEBUG_FILE" ) );
            if( !debug_file.isNull() )
            {
                freopen( debug_file, "a", stderr );
            }

            dbg_flags = parse_dbg_flags( flags );
            if( dbg_flags != 0 )
            {
                _dbg_msg( FormatString("dbg_flags=%X") << dbg_flags );
            }
        }

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
    PYCXX_NOARGS_METHOD_DECL( BemacsEditor, py_initEditor )

    //------------------------------------------------------------
    Py::Object py_hasFocus( const Py::Tuple &args )
    {
        if( synchronise_buffers_on_focus )
            synchronise_files_work_item.addItem();

        return Py::None();
    }
    PYCXX_VARARGS_METHOD_DECL( BemacsEditor, py_hasFocus )

    //------------------------------------------------------------
    Py::Object py_newCommandLine( const Py::Tuple &args )
    {
        Py::String py_cwd( args[0] );
        Py::List py_argv( args[1] );

        if( py_argv.length() == 0 )
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
    PYCXX_VARARGS_METHOD_DECL( BemacsEditor, py_newCommandLine )

    //------------------------------------------------------------
    Py::Object py_executeEnterHooks( void )
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
    PYCXX_NOARGS_METHOD_DECL( BemacsEditor, py_executeEnterHooks )

    Py::Object py_executeExitHooks( void )
    {
        {
            PythonAllowThreads permission( editor_access_control );
            if( exit_emacs_proc != NULL )
                execute_bound_saved_environment( exit_emacs_proc );
        }

        return Py::None();
    }
    PYCXX_NOARGS_METHOD_DECL( BemacsEditor, py_executeExitHooks )

    Py::Object py_processKeys( void )
    {
        int rc;

        {
            PythonAllowThreads permission( editor_access_control );
            rc = process_keys();
        }

        return Py::Long( rc );
    }
    PYCXX_NOARGS_METHOD_DECL( BemacsEditor, py_processKeys )

    Py::Object py_modifiedFilesExist( void )
    {
        bool rc;

        {
            PythonAllowThreads permission( editor_access_control );
            rc = mod_exist();
        }

        return Py::Boolean( rc );
    }
    PYCXX_NOARGS_METHOD_DECL( BemacsEditor, py_modifiedFilesExist )
    //------------------------------------------------------------

    //------------------------------------------------------------
    void hookUserInterface();
    
    int yesNoDialog( int yes, const EmacsString &prompt )
    {
        PythonDisallowThreads permission( editor_access_control );

        static std::string fn_name( "hookUserInterface" );
        try
        {
            Py::Boolean result( callOnSelf( fn_name, 
                                    Py::String( "yes-no-dialog" ),
                                    Py::Boolean( yes != 0 ),
                                    Py::String( "Barry's Emacs" ),
                                    Py::String( prompt.asPyString() ) ) );

            return result.isTrue() ? 1 : 0;
        }
        catch( Py::Exception &e )
        {
            ml_value = Expression();

            reportException( fn_name, e );

            return 0;
        }

    }

    void setWindowTitle( const EmacsString &title )
    {
        PythonDisallowThreads permission( editor_access_control );

        static std::string fn_name( "hookUserInterface" );
        try
        {
            callOnSelf( fn_name,
                        Py::String( "set-window-title" ),
                        Py::String( title.asPyString() ) );
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

        static std::string fn_name( "termCheckForInput" );
        try
        {
            callOnSelf( fn_name );
        }
        catch( Py::Exception &e )
        {
            reportException( fn_name, e );
        }
    }

    //------------------------------------------------------------
    int termWaitForActivity( double wait_until_time )
    {
        PythonDisallowThreads permission( editor_access_control );

        static std::string fn_name( "termWaitForActivity" );

        try
        {
            Py::Long code( callOnSelf( fn_name, Py::Float( wait_until_time ) ) );

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

        static std::string fn_name( "termTopos" );
        try
        {
            callOnSelf( fn_name, Py::Long( x ), Py::Long( y ) );
        }
        catch( Py::Exception &e )
        {
            reportException( fn_name, e );
        }
    }

    void termReset( void )
    {
        PythonDisallowThreads permission( editor_access_control );

        static std::string fn_name( "termReset" );
        try
        {
            callOnSelf( fn_name );
        }
        catch( Py::Exception &e )
        {
            reportException( fn_name, e );
        }
    }

    void termInit( void )
    {
        PythonDisallowThreads permission( editor_access_control );

        static std::string fn_name( "termInit" );
        try
        {
            callOnSelf( fn_name );
        }
        catch( Py::Exception &e )
        {
            reportException( fn_name, e );
        }
    }

    void termBeep( void )
    {
        PythonDisallowThreads permission( editor_access_control );

        static std::string fn_name( "termBeep" );
        try
        {
            callOnSelf( fn_name );
        }
        catch( Py::Exception &e )
        {
            reportException( fn_name, e );
        }
    }

    bool termUpdateBegin( void )
    {
        PythonDisallowThreads permission( editor_access_control );

        static std::string fn_name( "termUpdateBegin" );
        try
        {
            Py::Boolean rc( callOnSelf( fn_name ) );
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

        Py::Dict all_status_values;

        if( m_display_status_bar )
        {

            // line number
            int value = 1;
            if( bf_cur != NULL )
                for( int n=1; n<=dot - 1; n += 1 )
                    if( bf_cur->char_at (n) == '\n' )
                        value++;
            if( value > 9999999 )
                value = 9999999;

            all_status_values[ "line" ] = Py::Long( value );

            // column
            value = 1;
            if( bf_cur != NULL )
                value = cur_col();

            if( value > 9999 )
                value = 9999;

            all_status_values[ "column" ] = Py::Long( value );

            // read only
            all_status_values[ "readonly" ] = Py::Boolean( bf_cur != NULL && bf_cur->b_mode.md_readonly );

            // overstrike
            all_status_values[ "overstrike" ] = Py::Boolean( bf_cur != NULL && bf_cur->b_mode.md_replace );

            // record type
            const char *record_type = "unknown";
            if( bf_cur != NULL )
                switch( bf_cur->b_eol_attribute )
                {
                case FIO_EOL__Binary:       // literal read and write no
                    record_type = "binary";
                    break;
                case FIO_EOL__StreamCRLF:   // MS-DOS/Windows lines
                    record_type = "crlf";
                    break;
                case FIO_EOL__StreamCR:     // Machintosh lines
                    record_type = "cr";
                    break;
                case FIO_EOL__StreamLF:     // Unix lines
                    record_type = "lf";
                    break;

                default:
                    break;
                }

            all_status_values[ "eol" ] = Py::String( record_type );
        }

        Py::List horz_scroll_bar;
        Py::List vert_scroll_bar;

        if( m_display_scroll_bars )
        {
            EmacsWindow *w = theActiveView->windows.windows;

            int x = 0;
            int y = 0;

            EmacsWindow *old_window = theActiveView->windows.currentWindow();
            EmacsBufferRef old( bf_cur );

            // while windows and not the minibuffer window and there are scroll bars left
            while( w != NULL && w->w_next != NULL )
            {
                if( w->w_height > 5 )
                {
                    w->set_win();
                    vert_scroll_bar.append(
                            Py::TupleN(
                                Py::Long( w->w_window_id ),
                                // x, y
                                Py::Long( x + w->w_width ), Py::Long( y+1 ),
                                // width, height
                                Py::Long( vertical_bar_width ), Py::Long( w->w_height-1-2 ),
                                // pos, total
                                Py::Long( dot ), Py::Long( bf_cur->num_characters() ) ) );
                }
                else
                {
                    vert_scroll_bar.append( Py::None() );
                }

                if( w->w_width > 20 )
                {
                    horz_scroll_bar.append(
                            Py::TupleN(
                                Py::Long( w->w_window_id ),
                                // x, y
                                Py::Long( x + w->w_width - 10 ), Py::Long( y + w->w_height - 1 ),
                                // width, height
                                Py::Long( 10 ), Py::Long( 1 ),
                                // pos
                                Py::Long( w->w_horizontal_scroll ) ) );
                }
                else
                {
                    horz_scroll_bar.append( Py::None() );
                }

                //
                // adjust x and y
                //
                if( w->w_right != NULL )
                    // next is beside this one
                    x += w->w_width + vertical_bar_width;
                else
                    // next is below this one
                    x = 0, y += w->w_height;

                // step to the next window
                w = w->w_next;
            }

            old_window->set_win();
            old.set_bf();
        }

        static std::string fn_name( "termUpdateEnd" );
        try
        {
            callOnSelf( fn_name, all_status_values, horz_scroll_bar, vert_scroll_bar );
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

        static std::string fn_name( "termUpdateLine" );
        try
        {
            callOnSelf( fn_name,
                convertEmacsLine( oldl ),
                convertEmacsLine( newl ),
                Py::Long( ln ) );
        }
        catch( Py::Exception &e )
        {
            reportException( fn_name, e );
        }
    }

    void termMoveLine( int from_line, int to_line )
    {
        PythonDisallowThreads permission( editor_access_control );

        static std::string fn_name( "termMoveLine" );
        try
        {
            callOnSelf( fn_name,
                        Py::Long( from_line ),
                        Py::Long( to_line ) );
        }
        catch( Py::Exception &e )
        {
            reportException( fn_name, e );
        }
    }

    void termDisplayActivity( EmacsChar_t ch )
    {
        PythonDisallowThreads permission( editor_access_control );

        static std::string fn_name( "termDisplayActivity" );
        try
        {
            callOnSelf( fn_name, Py::Long( ch ) );
        }
        catch( Py::Exception &e )
        {
            reportException( fn_name, e );
        }
    }

    //------------------------------------------------------------
    Py::Object py_inputChar( const Py::Tuple &args )
    {
        int ch;

        Py::Object py_arg( args[0] );
        if( py_arg.isString() )
        {
            Py::String py_ch( py_arg );
            EmacsString e_ch( py_ch );
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
    PYCXX_VARARGS_METHOD_DECL( BemacsEditor, py_inputChar )

    //------------------------------------------------------------
    Py::Object py_scrollChangeVert( const Py::Tuple &args )
    {
        Py::Long py_window_id( args[0] );
        Py::Long py_value( args[1] );

        long window_id( py_window_id );
        long value( py_value );
        {
            PythonAllowThreads permission( editor_access_control );

            theActiveView->k_input_scroll_change_vert( window_id, value );
        }
        return Py::None();
    }
    PYCXX_VARARGS_METHOD_DECL( BemacsEditor, py_scrollChangeVert )

    Py::Object py_scrollChangeHorz( const Py::Tuple &args )
    {
        Py::Long py_window_id( args[0] );
        Py::Long py_value( args[1] );

        long window_id( py_window_id );
        long value( py_value );
        {
            PythonAllowThreads permission( editor_access_control );

            theActiveView->k_input_scroll_change_horz( window_id, value );
        }
        return Py::None();
    }
    PYCXX_VARARGS_METHOD_DECL( BemacsEditor, py_scrollChangeHorz )

    Py::Object py_scrollSetVert( const Py::Tuple &args )
    {
        Py::Long py_window_id( args[0] );
        Py::Long py_value( args[1] );

        long window_id( py_window_id );
        long value( py_value );
        {
            PythonAllowThreads permission( editor_access_control );

            theActiveView->k_input_scroll_set_vert( window_id, value );
        }
        return Py::None();
    }
    PYCXX_VARARGS_METHOD_DECL( BemacsEditor, py_scrollSetVert )

    Py::Object py_scrollSetHorz( const Py::Tuple &args )
    {
        Py::Long py_window_id( args[0] );
        Py::Long py_value( args[1] );

        long window_id( py_window_id );
        long value( py_value );
        {
            PythonAllowThreads permission( editor_access_control );

            theActiveView->k_input_scroll_set_horz( window_id, value );
        }
        return Py::None();
    }
    PYCXX_VARARGS_METHOD_DECL( BemacsEditor, py_scrollSetHorz )

    Py::Object py_inputMouse( const Py::Tuple &args )
    {
        Py::String py_keys( args[0] );
        Py::Boolean py_shift( args[1] );
        Py::List py_all_params( args[2] );

        EmacsString keys( py_keys );
        bool shift = py_shift;
        std::vector<int> all_params;

        for( Py_ssize_t i=0; i<py_all_params.size(); i++ )
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
    PYCXX_VARARGS_METHOD_DECL( BemacsEditor, py_inputMouse )

    Py::Object py_geometryChange( const Py::Tuple &args )
    {
        Py::Long py_width( args[0] );
        Py::Long py_height( args[1] );
        int width = int( long( py_width ) );
        int height = int( long( py_height ) );

        {
            PythonAllowThreads permission( editor_access_control );

            theActiveView->t_width = width - vertical_bar_width;
            theActiveView->t_length = height;
            theActiveView->t_geometry_change();
        }

        return Py::None();
    }
    PYCXX_VARARGS_METHOD_DECL( BemacsEditor, py_geometryChange )

    Py::Object py_setKeysMapping( const Py::Tuple &args )
    {
        Py::Dict keys_mapping( args[0] );
        Py::List all_keys( keys_mapping.keys() );

        for( Py_ssize_t i=0; i < all_keys.size(); ++i )
        {
            Py::String py_key( all_keys[ i ] );
            Py::String py_value( keys_mapping[ py_key ] );

            EmacsString key( py_key );
            EmacsString value( py_value );

            // Do not need to allow threads here as this is fast operation
            PC_key_names.addMapping( key, value );
        }

        return Py::None();
    }
    PYCXX_VARARGS_METHOD_DECL( BemacsEditor, py_setKeysMapping )

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

    //--------------------------------------------------------------------------------
    Py::String m_value;
    bool m_display_scroll_bars;
    bool m_display_status_bar;
};

PYCXX_USER_EXCEPTION_STR_ARG( EmacsError )
PYCXX_USER_EXCEPTION_STR_ARG( UserInterfaceError )

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

        // exceptions
        UserInterfaceError::init( *this );
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

// have to place after BemacsModule is defined as this function uses the module
void BemacsEditor::hookUserInterface()
{
    PythonDisallowThreads permission( editor_access_control );

    static std::string fn_name( "hookUserInterface" );
    try
    {
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

        Py::Object result( self().callMemberFunction( fn_name, all_args ) );

        ml_value = convertPyObjectToEmacsExpression( result );
    }
    catch( UserInterfaceError &e )
    {
        std::string value( Py::value( e ).str().as_std_string() );
        e.clear();

        error( value );
    }
    catch( Py::Exception &e )
    {
        ml_value = Expression();

        std::string type( Py::type( e ).str().as_std_string() );
        std::string value( Py::value( e ).str().as_std_string() );
        e.clear();

        error( FormatString("error calling \"%s\" - %s( %s )") << fn_name << type << value );
    }
}

#if defined( _WIN32 )
#define EXPORT_SYMBOL __declspec( dllexport )
#else
#define EXPORT_SYMBOL
#endif

#if PY_MAJOR_VERSION == 3
extern "C" EXPORT_SYMBOL PyObject *PyInit__bemacs()
{
#if defined(PY_WIN32_DELAYLOAD_PYTHON_DLL)
    Py::InitialisePythonIndirectPy::Interface();
#endif

    bemacs_module = new BemacsModule;
    return bemacs_module->module().ptr();
}

// symbol required for the debug version
extern "C" PyObject *PyInit__bemacs_d()
{
    return PyInit__bemacs();
}

#elif PY_MAJOR_VERSION == 2

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
#else
#error "Unsupported Python version"
#endif

class TerminalControl_Python: public EmacsView
{
public:
    TerminalControl_Python( BemacsEditor &editor )
    : EmacsView()
    , m_editor( editor )
    , m_check_input_time( EmacsDateTime::now().asDouble() + CHECK_FOR_INPUT_INTERVAL )
    {
        t_il_mf = 1;
        t_il_ov = 1;
        t_ic_ov = MISSINGFEATURE;
        t_dc_ov = MISSINGFEATURE;
        t_baud_rate = 1000000;

        vertical_bar_width = 2;
    }

    virtual ~TerminalControl_Python()
    {
    }

    //
    //    Keyboard routines
    //
    virtual void k_check_for_input()   // check for any input
    {
        double now = EmacsDateTime::now().asDouble();

        if( now > m_check_input_time )
        {
            m_check_input_time = now + CHECK_FOR_INPUT_INTERVAL;
            m_editor.termCheckForInput();
            // check for timeouts
            time_call_timeout_handler();
        }
    }

    //
    //  User Interface routines
    //
    virtual void t_user_interface_hook()
    {
        m_editor.hookUserInterface();
    }

    int t_yesNoDialog( int yes, const EmacsString &prompt )
    {
        return m_editor.yesNoDialog( yes, prompt );
    }

    void t_setWindowTitle( const EmacsString &title )
    {
        m_editor.setWindowTitle( title );
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

    virtual void t_display_activity( EmacsChar_t ch )
    {
        m_editor.termDisplayActivity( ch );
    }

    virtual void t_beep()
    {
        m_editor.termBeep();
    }

    BemacsEditor &m_editor;
    double m_check_input_time;
    static const double CHECK_FOR_INPUT_INTERVAL;
    // check for input every CHECK_FOR_INPUT_INTERVAL calls (about every 100ms)
};

const double TerminalControl_Python::CHECK_FOR_INPUT_INTERVAL( 0.1 );

void init_python_terminal( BemacsEditor &editor )
{
    term_is_terminal = 3;
    theActiveView = new TerminalControl_Python( editor );
}

TerminalControl_Python *thePythonActiveView()
{
    return dynamic_cast< TerminalControl_Python * >( theActiveView );
}

int ui_frame_to_foreground(void)
{
    return 0;
}

extern double time_getTimeoutTime();

int wait_for_activity(void)
{
    double timeout = time_getTimeoutTime();
    if( timeout == 0.0 )
    {
        timeout = EmacsDateTime( 60.0 ).asDouble();
    }
    return thePythonActiveView()->m_editor.termWaitForActivity( timeout );
}

void wait_abit(void)
{
    // wait 100ms - used for timing sit-for
    EmacsDateTime timeout( 0.100 );
    thePythonActiveView()->m_editor.termWaitForActivity( timeout.asDouble() );
}

bool emacs_internal_init_done_event(void)
{
    return true;
}

void UI_update_window_title( void )
{
    EmacsString title;

#if defined( WIN32 )
    EmacsString home( getenv("USERPROFILE") );
#else
    EmacsString home( getenv("HOME") );
#endif

    EmacsString cwd( current_directory.asString() );

    if( cwd.commonPrefix( home ) == home.length() )
    {
        title.append( "~" PATH_STR );
        cwd.remove( 0, home.length() );
        if( cwd.length() > 0 && cwd[0] == PATH_CH  )
            cwd.remove( 0, 1 );
        title.append( cwd );
    }
    else
    {
        title.append( cwd );
    }

    thePythonActiveView()->t_setWindowTitle( title );
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

    thePythonActiveView()->t_user_interface_hook();
    return 0;
}

int get_yes_or_no( int yes, const EmacsString &prompt )
{
    return thePythonActiveView()->t_yesNoDialog( yes, prompt );
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
