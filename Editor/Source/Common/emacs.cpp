//
//    Copyright (c) 1982-2010
//        Barry A. Scott
//
#include <emacs.h>
#include <mem_man.h>

#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
static EmacsInitialisation emacs_initialisation( __DATE__ " " __TIME__, THIS_FILE );


void emacs_exit(int code);
#ifdef vms
void read_restart_message( int set_gargs );
bool send_exit_message( const EmacsString &str );
#endif
int debug_emacs( void );
int argc_command( void );
int argv_command( void );
int argIsQualifier_command( void );
int emacs_version( void );
#if !defined( PYBEMACS )
#pragma message( "Defining emacsMain" )
int emacsMain( const EmacsString &rest_fn, const EmacsString &device, const EmacsString &term_type );
static void read_emacs_memory_file();
static void write_emacs_memory_file();
int read_in_files(void);
#endif
int execute_package( const EmacsString &package );

#ifdef vms
extern vms_restore_fail( int, unsigned char * );
extern vms_restoring_env( unsigned char * );
extern vms_debug_emacs( unsigned char * );
#endif
#ifdef __unix__
extern void start_emacs_server(void);
#endif
#ifdef WIN32
extern bool UI_quit_emacs();
#endif

extern void init_scheduled_timeout(void);
extern void restore_scheduled_timeout(void);
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

int dbg_flags;

EmacsFile message_file;     // FIO pointer to SYS$OUTPUT
EmacsFile command_file;     // FIO pointer to SYS$INPUT
int gui_error = 0;          // true if a message box is required for errors

extern EmacsString parent_path;
SystemExpressionRepresentationIntBoolean silently_kill_processes;
SystemExpressionRepresentationStringReadOnly ui_type( UI_TYPE );
SystemExpressionRepresentationStringReadOnly operating_system_name( OS_NAME );
SystemExpressionRepresentationStringReadOnly operating_system_version
#ifdef OS_VERSION
    ( OS_VERSION )
#endif
    ;
SystemExpressionRepresentationStringReadOnly CPU_type
#ifdef CPU_TYPE
    ( CPU_TYPE )
#endif
    ;

int must_checkpoint_on_crash;

int emacs_is_exiting;

// global versions of argv envp and argc, for
// use by MLisp functions!
EmacsCommandLine command_line_arguments;
//
// true iff the user has touched the
// command line arguments, this stops Emacs
// from doing the VisitFiles
//
bool touched_command_args = false;


BoundName *user_interface_hook_proc;

#if !defined( PYBEMACS )
//============================================================
//
// emacs main routine
//
//============================================================
int emacsMain
    (
    const EmacsString &rest_fn,
    const EmacsString &term_type,
    const EmacsString &device
    )
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

#ifdef SAVE_ENVIRONMENT
    EmacsString full_rest_fn;
    EmacsString cannot_restore_reason;
    rv = EmacsSaveRestoreEnvironmentObject->canRestore( rest_fn, full_rest_fn, cannot_restore_reason );
    if( rv < 0 )
    {
#if defined( vms )
        vms_restore_fail( 1, full_rest_fn );
#else
        EmacsString msg( FormatString("Unable to restore the environment file %s\n\n"
                    "Because %s\n\n"
                    "Please recreate it by using the save-environment command.")
            << full_rest_fn << cannot_restore_reason );
#ifdef _NT
        win_error( msg );
#else
        _dbg_msg( msg );
#endif
#endif
    }
    if( rv > 0 )
    {
        int status;
        init_display();                         // " the core display system
        init_lisp();                            // " the MLisp system
        init_abs();                             // " the current directory name
#ifdef vms
        vms_restoring_env( full_rest_fn.data() );
#endif
        if( ((status = EmacsSaveRestoreEnvironmentObject->DoRestore())&1) != 1 )
        {
#ifdef vms
            vms_restore_fail( status, full_rest_fn.data() );
#else
            _dbg_msg( FormatString("Unable to restore environment file %s - please recreate it") << full_rest_fn.data() );
            return 0;
#endif
        }

        init_srch();                            // " the search commands
        init_terminal( term_type, device );     // Start the primary terminal IO system
        init_dsp();
        EmacsWindowGroup::restore();            // " the window system
        restore_timer();
        restore_scheduled_timeout();
        restore_db();                           // Restart the database manager
        restore_vms();                          // Restart the sub-process code
        EmacsBufferJournal::restore_journal();  // Restore the journal system
    }
    else
#endif

    {
        // init logic without a restore
        init_fncs();                            // initialise the key bindings
        init_var();                             // " the variables
        init_bf();                              // " the buffer system
        init_scheduled_timeout();
        init_display();                         // " the core display system
        init_terminal( term_type, device );     // Start the primary terminal IO system
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
    }

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
        return EXIT_SUCCESS;

#ifdef vms
#error need to move this into the VMS emacs_internal_init_done_event rtn
    read_restart_message( 0 );
#endif

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
#ifdef vms
    // check for an exit DCL command, and send it to the mailbox if required
    if( !exit_emacs_dcl_command.isNull() )
        send_exit_message( exit_emacs_dcl_command );
#endif
    kill_checkpoint_files();
    EmacsBufferJournal::journal_exit();

    rst_dsp();
    emacs_is_exiting = 1;

    return EXIT_SUCCESS;
}

static void read_emacs_memory_file()
{
    EmacsString fn( MEMORY_FILE_STR );
    if( fn.isNull() )
        return;

    EmacsString memory = FormatString( fn ) << MEMORY_FILE_ARG;

    unsigned char combuf[300];

    {
    EmacsFile args;
    if( !args.fio_open( memory, FIO_READ, EmacsString::null ) )
        return;

    int saved_err = 0;
    while( args.fio_get_line( combuf, sizeof( combuf)) > 0 )
    {
        unsigned char *p = combuf;
        while( *p != 0 && *p >= ' ' )
            p++;

        int i = p[0];
        *p++ = 0;
        visit_file( EmacsString( EmacsString::copy, combuf ), 1, 1, EmacsString("") );
        saved_err = saved_err || ml_err;
        ml_err = 0;

        if( i == 1 )
        {
            i = 0;
            while( '0' <= *p && *p <= '9' )
                i = i * 10 + *p++ - '0';

            if( i > bf_cur->first_character()
            && i <= (bf_cur->num_characters()+1) )
                set_dot( i );
        }
    }

    ml_err = saved_err;
    }

    EmacsFile::fio_delete( memory );
}

static void write_emacs_memory_file()
{
    EmacsString fn( MEMORY_FILE_STR );
    if( fn.isNull() )
        return;

    EmacsString memory = FormatString( fn ) << MEMORY_FILE_ARG;

    EmacsFile args;
    EmacsWindow *w = theActiveView->windows.windows;
    while( w != 0 )
    {
        w->w_buf->set_bf();
        if( !bf_cur->b_fname.isNull() )
        {
            if( !args.fio_is_open() )
            {
                args.fio_create( memory, 1, FIO_STD, EmacsString(), (FIO_EOL_Attribute)(int)default_end_of_line_style );
                if( !args.fio_is_open() )
                    break;
            }
            EmacsString mem_line( FormatString("%s\001%d\n") << bf_cur->b_fname << dot );
            args.fio_put( mem_line, mem_line.length() );
        }
        w = w->w_next;
    }
}


int read_in_files(void)
{
    int done_any_visiting = 0;
    int saved_err = 0;

    if( ! touched_command_args )
        for( int i=1; i<command_line_arguments.argumentCount(); i++ )
        {
            saved_err = saved_err || ml_err;
            ml_err = 0;
            if( !command_line_arguments.argument(i).isQualifier() )
            {
                try
                {
                    // visit file can throw exceptions
                    // if the user is prompted and types ^G
                    visit_file
                    (
                    EmacsString( command_line_arguments.argument(i).value() ),
                    1, 1,
                    parent_path
                    );
                }
                catch( EmacsException )
                {
                    // no need to clean up
                }
            }
            done_any_visiting = 1;
        }

    touched_command_args = true;
    ml_err = ml_err || saved_err;
    return done_any_visiting;
}

extern int ui_frame_to_foreground(void);

void EmacsCommandLineServerWorkItem::workAction()
{
    // come to the fore
    ui_frame_to_foreground();

    // save the current directory as the previous
    previous_directory = current_directory.asString();

    // first change directory
    chdir_and_set_global_record( command_current_directory );

    EmacsString full_command_line("emacs ");
    full_command_line.append( command_line );

    command_line_arguments.setArguments( full_command_line );

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
                int colon_pos = str.first( ':' );
                if( colon_pos > 0 )
                {
                    key_string = str( 1, colon_pos );
                    val_string = str( colon_pos+1, str.length() );
                }
                else
                    key_string = str( 1, str.length() );
            }

            bool delete_this_arg = true;

            if( key_string.commonPrefix( "package" ) > 2 )
            {
                command_line_arguments.setArgument( 0, val_string, false );
            }
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
#endif

int execute_package( const EmacsString &package )
{
    int rv = 0;
    //
    // If Emacs is invoked by any name other than 'emacs' we try to load a
    // package by that name and then execute a command by that name; this
    // is so that things like rmail and dired can be invoked from the
    // command line.
    //
    if( package.isNull() )
        return rv;

    EmacsString package_name( package );
    if( package_name.length() > 4 && package_name( -4, INT_MAX ).toLower() == ".mlp" )
        package_name = package_name( 0, -4 );

    EmacsString package_filename( FormatString("%s.mlp") << package_name );
    int p = package_name.last( PATH_CH );
    if( p < 0 )
    {
        p = 0;
    }
    else
    {
        p++;
    }

    EmacsString package_command_name( FormatString("%s-com") << package_name( p, INT_MAX ) );
    //
    // See if the function is defined already
    //
    BoundName *proc = BoundName::find( package_command_name );
    if( proc != NULL )
        return execute_bound_saved_environment( proc );


    //
    // as the func is not defined load the .MLP file and try again
    //
    if( (rv = execute_mlisp_file( package_filename, 1)) != 0
    || ml_err
    || (proc = BoundName::find( package_command_name )) == NULL )
    {
        ml_err = 0;
        minibuf_body.clearMessageBody();
    }
    else
    {
        return execute_bound_saved_environment( proc );
    }

    return rv;
}

void emacs_exit(int code)
{
    emacs_is_exiting = 1;
    exit( code );
}

extern void dump_memory_since(void);

int debug_emacs(void)
{
    if( (arg_state == have_arg && interactive())
    || (! interactive() && cur_exec != 0 && cur_exec->p_nargs > 0) )
    {
        EmacsString cmd;
        if( cur_exec == NULL )
            cmd = get_string_interactive( ": debug-emacs " );
        else
            cmd = get_string_mlisp();
        if( cmd.isNull() )
            return 0;

#if DBG_ALLOC_CHECK
        if( cmd == "check_heap" )
            emacs_heap_check();
        else
#endif
        if( cmd.commonPrefix( "flags=" ) == 6 )
            dbg_flags = parse_dbg_flags( cmd( 6, 999 ) );

        else if( cmd.commonPrefix( "msg=" ) == 4 )
            _dbg_msg( FormatString( "debug-emacs %s" ) << cmd );

        else if( cmd == "dump_memory_since" )
            dump_memory_since();

        else if( cmd == "crash" )
        {
            void (*func)(unsigned int ) = 0;
            func( 0xdeaddead ); // die die die ...
        }
#ifdef vms
        else
            vms_debug_emacs( cmd );
#endif
    }
    else
        debug_invoke();

    return 0;
 }

int argc_command( void )
{                // return the value of argc
    ml_value = command_line_arguments.argumentCount();
    touched_command_args = true;
    return 0;
}

int argv_command( void )
{                // return the value of argv[i]
    int n = getnum( u_str(": argv index: ") );
    if( ! ml_err )
    {
        if( n >= command_line_arguments.argumentCount() || n < 0 )
        {
            error( FormatString("Argv cannot return the %d'th argument, there are only %d") <<
                n << command_line_arguments.argumentCount() );
        }
        else
        {
            ml_value = command_line_arguments.argument(n).value();
        }
    }

    touched_command_args = true;
    return 0;
}

int argIsQualifier_command( void )
{                // return the value of argv[i]
    int n = getnum( u_str(": arg-is-qualifier index: ") );
    if( ! ml_err )
    {
        if( n >= command_line_arguments.argumentCount() || n < 0 )
        {
            error( FormatString("Argv cannot return the %d'th argument, there are only %d") <<
                n << command_line_arguments.argumentCount() );
        }
        else
        {
            ml_value = int( command_line_arguments.argument(n).isQualifier() );
        }
    }

    touched_command_args = true;
    return 0;
}

int emacs_version( void )
{
    ml_value = version_string;

    return 0;
}

void TerminalControl::t_update_line(EmacsLinePtr, EmacsLinePtr line, int )
{
    _dbg_msg( FormatString( "%.*s\n") << line->line_length << line->line_body );
}


//
//    Parse the flags and return the value
//
//    flags is either a comma seperated list of option names
//    or its a number
//
int parse_dbg_flags( const EmacsString &flags )
{
    int flags_value = 0;
    if( flags[0] >= '0' && flags[0] <= '9' )
    {
        sscanf( flags.sdata(), "%i", &flags_value );
        return flags_value;
    }

    int start=0;
    int end=0;
    while( start < flags.length() )
    {
        end = flags.index( ',', start );
        if( end < 0 )
            end = flags.length();

        EmacsString keyword( flags( start, end ) );
        start = end+1;

        if( keyword == "dumptofile" )
            flags_value |= DBG_DUMPTOFILE|DBG_NO_DBG_MSG;
        else if( keyword == "alloc_check" )
            flags_value |= DBG_ALLOC_CHECK;
        else if( keyword == "exec" )
            flags_value |= DBG_EXEC;
        else if( keyword == "key" )
            flags_value |= DBG_KEY;
        else if( keyword == "display" )
            flags_value |= DBG_DISPLAY;
        else if( keyword == "queue" )
            flags_value |= DBG_QUEUE;
        else if( keyword == "buffer" )
            flags_value |= DBG_BUFFER;
        else if( keyword == "profile" )
            flags_value |= DBG_PROFILE;
        else if( keyword == "vector" )
            flags_value |= DBG_VECTOR;
        else if( keyword == "calc_ins_del" )
            flags_value |= DBG_CALC_INS_DEL;
        else if( keyword == "execfile" )
            flags_value |= DBG_EXECFILE;
        else if( keyword == "no_dbg_msg" )
            flags_value |= DBG_NO_DBG_MSG;
        else if( keyword == "syntax" )
            flags_value |= DBG_SYNTAX;
        else if( keyword == "emstring" )
            flags_value |= DBG_EMSTRING;
        else if( keyword == "ser" )
            flags_value |= DBG_SER;
        else if( keyword == "ml_parse" )
            flags_value |= DBG_ML_PARSE;
        else if( keyword == "ml_error" )
            flags_value |= DBG_ML_ERROR;
        else if( keyword == "process" )
            flags_value |= DBG_PROCESS;
#ifdef DBG_SYSLOG
        else if( keyword == "syslog" )
            flags_value |= DBG_SYSLOG;
#endif
        else if( keyword == "verbose" )
            flags_value |= DBG_VERBOSE;
        else if( keyword == "tmp" )
            flags_value |= DBG_TMP;
        else if( keyword == "timer" )
            flags_value |= DBG_TIMER;
        else if( keyword == "ext_search" )
            flags_value |= DBG_EXT_SEARCH;
        else
            _dbg_msg( FormatString("Unknown debug flag %s") << keyword );
    }

    return flags_value;
}



//
//    Emacs dynamic update of the command line
//
EmacsString g_initialCurrentDirectory;

void EmacsCommandLineServerWorkItem::newCommandLine
    ( const EmacsString &_current_directory, const EmacsString &_command_line )
{
    command_current_directory = _current_directory;
    command_line = _command_line;

    if( g_initialCurrentDirectory.isNull() )
    {
        //_dbg_msg( FormatString("newCommandLine: setting g_initialCurrentDirectory to %s") << command_current_directory );
        g_initialCurrentDirectory = command_current_directory;
    }

    addItem();
}
