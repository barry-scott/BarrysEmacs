//
//    Emacs_python_interface.cpp
//
//    Copyright (c) 1999 Barry A. Scott
//

#include <emacs.h>

#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
static EmacsInitialisation emacs_initialisation( __DATE__ " " __TIME__, THIS_FILE );

#ifdef EMACS_PYTHON_EXTENSION
#include <nt_comm.h>
#include <emacs_python_interface.h>

#include <python.h>

#pragma warning( push, 3)
#include <CXX/Extensions.hxx>
#pragma warning( pop )

#include <stdio.h>
#include <fcntl.h>

static EmacsString calcNewPythonPath(void);

extern void init_bemacs_python(void);
extern Expression convertPyObjectToEmacsExpression( const Py::Object &obj );
extern Py::Object convertEmacsExpressionToPyObject( const Expression &expr );

SystemExpressionRepresentationString python_flags;

//================================================================================
//
//    EmacsPythonCommand
//
//================================================================================
EmacsPythonCommand::EmacsPythonCommand()
    : command_failed(false)
    , failure_reason()
    , result()
{}

EmacsPythonCommand::~EmacsPythonCommand()
{}

bool EmacsPythonCommand::failed() const
{
    return command_failed;
}

const EmacsString &EmacsPythonCommand::failureReason() const
{
    return failure_reason;
}

const Expression &EmacsPythonCommand::getResult() const
{
    return result;
}

void EmacsPythonCommand::commandSucceeded( const Expression &_result )
{
    command_failed = false;
    result = _result;
}

void EmacsPythonCommand::commandFailed( const EmacsString &_reason )
{
    command_failed = true;
    failure_reason = _reason;
}


static EmacsString python_try_command_except_structure
    (
    "try:\n"
    "    __bemacs_error = None\n"    // No error yet
    "    %s\n"                // substitute command in here
    "except:\n"
    "    import sys, traceback, string\n"
    "    __bemacs_type, __bemacs_value, __bemacs_traceback_obj = sys.exc_info()\n"
    "    __bemacs_list = traceback.format_exception( __bemacs_type, __bemacs_value, __bemacs_traceback_obj )\n"
    "    __bemacs_error = __bemacs_list[-1] + string.join( __bemacs_list[:-1] )\n"
    "    __bemacs_traceback_obj = None\n"
    );

void EmacsPythonCommand::runPythonStringInsideTryExcept( const EmacsString &_command )
{
    EmacsString command( _command );

    int pos=0;
    for(;;)
    {
        pos = command.index( '\n', pos );
        if( pos < 0 )
            break;
        command.insert( pos+1, '\t' );
        pos = pos + 2;
    }

    EmacsString wrapped_command( FormatString( python_try_command_except_structure ) << command );

    PyRun_SimpleString( wrapped_command.sdataHack() );

    try
    {
        Py::Module module( "__main__" );
        Py::Dict dict( module.getDict() );

        Py::Object error_reason( dict[ "__bemacs_error" ] );
        if( error_reason.isString() )
        {
            std::string reason_1 = Py::String( error_reason );
            commandFailed( EmacsString( EmacsString::copy, reason_1.c_str() ) );
        }
    }
    catch( Py::Exception &e )
    {
        e.clear();
    }
}

//================================================================================
//
//    EmacsPythonThread
//
//================================================================================
static EmacsPythonThread *py_thread = NULL;
EmacsPythonThread::EmacsPythonThread( EmacsProcess *_proc )
{
    emacs_assert( py_thread == NULL );
    proc = _proc;
    py_thread = this;
    command_event = CreateEvent( NULL, FALSE, FALSE, NULL );
    result_event = CreateEvent( NULL, FALSE, FALSE, NULL );
}

EmacsPythonThread::~EmacsPythonThread()
{
    emacs_assert( py_thread != NULL );

    // send NULL to ask for a shutdown
    sendCommandToPython( NULL );
    getResultFromPython( NULL );

    CloseHandle( command_event );
    CloseHandle( result_event );

    py_thread = NULL;
}

//
// command_mine the Python version
// PyWin_DLLVersionString is used during Py_Initialize
// to setup the sys.path, sys.prefix and sys.exec_prefix
//
//extern "C" {
//    const char * PyWin_DLLVersionString = "1.5";
//};
extern volatile int terminating_process;

unsigned EmacsPythonThread::run()
{
    if( !Py::InitialisePythonIndirectInterface() )
    {
        static const char not_installed[] = "Python 2.1 is not installed on this computer\n";
        DWORD bytes_written = 0;
        WriteFile( stdout_handle, not_installed, sizeof( not_installed ) - 1, &bytes_written, NULL );

        // allow message to be picked up
        Sleep( 1000 );

        // mark this thread "process" as dead so emacs will clean up
        proc->proc_state = EmacsProcess::DEAD;
        interlock_inc( &pending_channel_io );
        interlock_inc( &terminating_process );
        wake_main_thread();

        return 0;
    }

    // set the Python flags
{
    EmacsString flags( python_flags.asString() );
    for( int i=0; i<flags.length(); i++ )
        switch( flags[i] )
        {
        case 'v': Py::_Py_VerboseFlag()++; break;
        case 'O': Py::_Py_OptimizeFlag()++; break;
        case 'd': Py::_Py_DebugFlag()++; break;
        case 't': Py::_Py_TabcheckFlag()++; break;
        case 'S': Py::_Py_NoSiteFlag()++; break;
        case 'U': Py::_Py_UnicodeFlag()++; break;
        default:
        {
            EmacsString msg( FormatString( "Unknown Python-flag \"%c\"" ) << flags[i] );
            DWORD bytes_written = 0;
            WriteFile( stdout_handle, msg.sdata(), msg.length(), &bytes_written, NULL );
        }
        }
}
    // setup stdin, stdout and stderr
    int stdin_fd = _open_osfhandle ( (long)stdin_handle, O_RDONLY );
    int stdout_fd = _open_osfhandle ( (long)stdout_handle, O_APPEND );
    int stderr_fd = dup( stdout_fd );

    // open up in order stdin, stdout, stderr
    py_stdin = fdopen( stdin_fd, "r" );
    py_stdout = fdopen( stdout_fd, "w" );
    py_stderr = fdopen( stderr_fd, "w" );

    setvbuf( py_stdout, NULL, _IOLBF, 8192 );
    setvbuf( py_stderr, NULL, _IOLBF, 8192 );

    EmacsString new_python_path( calcNewPythonPath() );
    putenv( new_python_path.sdata() );

    Py_SetProgramName("bemacs");    // must be before init


    // Python will now be connected to the pipes
    Py_Initialize();


    char *py_argv[2];
    py_argv[0] = "bemacs";
    py_argv[1] = NULL;
    PySys_SetArgv( 1, py_argv );    // must be after init

    //++++++++++++++++++++++++++++++
    // copied from _PySysInit()
{
    Py::Module sys_module( "sys" );
    Py::Dict sysdict = sys_module.getDict();

    Py::Object sysin(  PyFile_FromFile( py_stdin,  "<stdin>",  "r", NULL ) );
    Py::Object sysout( PyFile_FromFile( py_stdout, "<stdout>", "w", NULL ) );
    Py::Object syserr( PyFile_FromFile( py_stderr, "<stderr>", "w", NULL ) );

    sysdict[ "stdin"  ] = sysin;
    sysdict[ "stdout" ] = sysout;
    sysdict[ "stderr" ] = syserr;

    // Make backup copies for cleanup
    sysdict[ "__stdin__"  ] = sysin;
    sysdict[ "__stdout__" ] = sysout;
    sysdict[ "__stderr__" ] = syserr;
}
    // end of _PySysInit() copied code
    //---------------------------------

    // force threading support
    PyEval_InitThreads();

    init_bemacs_python();
    PyRun_SimpleString("import bemacs\n");

    // test stdout and stderr
    fprintf( py_stdout, "Python version %s\n", Py_GetVersion() );
    //fprintf( py_stderr, "%s\n", Py_GetCopyright() );
    fflush( py_stdout );
    fflush( py_stderr );

    for(;;)
    {
        EmacsPythonCommand *command = NULL;

        Py_BEGIN_ALLOW_THREADS

        if( !getCommandFromEmacs( &command ) )
            break;

        if( command == NULL )
            // shutdown request
            break;

        // writeToConsole( "Processing command from Emacs\n");

        Py_END_ALLOW_THREADS

        command->executeCommand();

        fflush( py_stdout );
        fflush( py_stderr );

        // writeToConsole( "Done returning to Emacs\n");

        sendResultToEmacs( command );
    }

    // close this end of the pipes
    fclose( py_stdin );
    fclose( py_stdout );
    fclose( py_stderr );

    sendResultToEmacs( NULL );

    return 0;
}

void EmacsPythonThread::writeToConsole( const EmacsString &string )
{
    DWORD written = 0;

    if( !WriteFile( stdout_handle, string.data(), string.length(), &written, NULL ) )
        DebugPrintf(FormatString("Emacs_Python Failed to write, error = %d\n") << GetLastError());
}

// client uses these functions
bool EmacsPythonThread::sendCommandToPython( EmacsPythonCommand *command )
{
    the_command = command;

    // Pass control to Python
    SetEvent( command_event );
    return true;
}

extern bool check_for_interrupt_key(void);

bool EmacsPythonThread::getResultFromPython( EmacsPythonCommand *command )
{
    for(;;)
    {
        DWORD result = WaitForSingleObject( result_event, 50 );
        switch( result )
        {
        case WAIT_OBJECT_0:
            emacs_assert( the_command == command );
            the_command = NULL;
            return true;

        case WAIT_TIMEOUT:
            if( check_for_interrupt_key() )
            {
                // check the interrupt state
                interrupt_key_struck = 0;
                // interrupt Python
                PyErr_SetInterrupt();
            }
            if( pending_channel_io != 0 )
            {
                process_channel_interrupts();
            }
            break;
        default:
            // go around again
            break;
        }
    }
}

// server uses these functions
bool EmacsPythonThread::getCommandFromEmacs( EmacsPythonCommand **command )
{
    for(;;)
    {
        DWORD result = WaitForSingleObject( command_event, 100 );    // wait 100mS
        switch( result )
        {
        case WAIT_OBJECT_0:
            *command = the_command;
            return 1;    // return have a command

        case WAIT_TIMEOUT:
            fflush( py_stdout );
            fflush( py_stderr );
            break;        // loop around and try again

        default:
            return 0;    // return no command available
        }
    }
}

bool EmacsPythonThread::sendResultToEmacs( EmacsPythonCommand *command)
{
    emacs_assert( the_command == command );
    // pass control back to Emacs
    SetEvent( result_event );

    return true;
}

//================================================================================
//
//    Emacs Python commands
//
//================================================================================
static EmacsString start_python_error( "Python has not been started" );

//--------------------------------------------------------------------------------
//
//    Emacs Python Eval
//
//--------------------------------------------------------------------------------
class EmacsPythonEvalCommand : public EmacsPythonCommand
{
public:
    EmacsPythonEvalCommand( const EmacsString &_python_command )
        : python_command( _python_command )
    { }
    ~EmacsPythonEvalCommand()
    { }

private:
    virtual bool executeCommand();
    EmacsString python_command;
};

bool EmacsPythonEvalCommand::executeCommand()
{
    try
    {
        Py::Module module( "__main__" );
        Py::Dict dict( module.getDict() );

        runPythonStringInsideTryExcept( python_command );

        Py::Object py_result( dict[ "__bemacs_eval_tmp__" ] );

        if( !failed() )
            commandSucceeded( convertPyObjectToEmacsExpression( py_result ) );
    }
    catch( Py::Exception &e )
    {
        e.clear();

        return false;
    }

    return true;
}

int python_eval_string(void)
{
    EmacsPythonRecursionBarrier barrier;
    if( barrier.barrierClosed() )
        return 0;

    EmacsString expression;

    if( cur_exec == NULL )
    {
        EmacsString prompt( ": Python-eval " );

        expression = get_string_interactive( prompt );
    }
    else
    {
        if( check_args( 1, 1 ) )
            return 0;

        expression = get_string_mlisp();
    }

    EmacsString command( FormatString
        (
        "__bemacs_eval_tmp__ = %s"
        ) << expression );

    EmacsPythonEvalCommand py_command( command );

    py_thread->sendCommandToPython( &py_command );
    py_thread->getResultFromPython( &py_command );

    if( py_command.failed() )
    {
        Expression e( py_command.getResult() );
        if( e.exp_type() == ISSTRING )
            error( e.asString() );
        else
            error("Python-eval - error in expression");
    }
    else
        ml_value = py_command.getResult();

    return 0;
}

//--------------------------------------------------------------------------------
//
//    Emacs Python Exec
//
//--------------------------------------------------------------------------------
class EmacsPythonExecCommand : public EmacsPythonCommand
{
public:
    EmacsPythonExecCommand( const EmacsString &_python_command )
        : python_command( _python_command )
    { }
    ~EmacsPythonExecCommand()
    { }
private:
    virtual bool executeCommand();
    EmacsString python_command;
};

bool EmacsPythonExecCommand::executeCommand()
{
    runPythonStringInsideTryExcept( python_command.sdataHack() );

    if( failed() )
        error( failureReason() );

    return true;
}

int python_exec_string(void)
{
    EmacsPythonRecursionBarrier barrier;
    if( barrier.barrierClosed() )
        return 0;

    EmacsString command;

    if( cur_exec == NULL )
    {
        EmacsString prompt( ": Python-exec " );

        command = get_string_interactive( prompt );
    }
    else
    {
        if( check_args( 1, 1 ) )
            return 0;

        command = get_string_mlisp();
    }

    EmacsPythonExecCommand py_command( command );

    py_thread->sendCommandToPython( &py_command );
    py_thread->getResultFromPython( &py_command );

    return 0;
}

//--------------------------------------------------------------------------------
//
//    Emacs Python Import
//
//--------------------------------------------------------------------------------
int python_import_module(void)
{
    EmacsPythonRecursionBarrier barrier;
    if( barrier.barrierClosed() )
        return 0;

    EmacsString module_name;

    if( cur_exec == NULL )
    {
        EmacsString prompt( ": Python-import " );

        module_name = get_string_interactive( prompt );
    }
    else
    {
        if( check_args( 1, 1 ) )
            return 0;

        module_name = get_string_mlisp();
    }

    EmacsString command( FormatString
        (
        "import %s"
        ) << module_name );

    EmacsPythonExecCommand py_command( command );


    py_thread->sendCommandToPython( &py_command );
    py_thread->getResultFromPython( &py_command );

    return 0;
}

//--------------------------------------------------------------------------------
//
//    Emacs Python Call
//
//--------------------------------------------------------------------------------
class EmacsPythonCallCommand : public EmacsPythonCommand
{
public:
    EmacsPythonCallCommand()
        : python_function()
        , num_args(0)
    {}

    virtual ~EmacsPythonCallCommand()
    {}

    virtual bool executeCommand();
    EmacsString python_function;
    enum { MAX_ARGS=20 };
    int num_args;
    Expression python_args[ MAX_ARGS ];
};

bool EmacsPythonCallCommand::executeCommand()
{
    try
    {
        Py::Module module( "__main__" );
        Py::Dict dict( module.getDict() );

        //
        //    create the tuple of arguments
        //
        Py::Tuple args_tuple( num_args );
        for( int arg=0; arg < num_args; arg++ )
        {
            args_tuple[ arg ] = convertEmacsExpressionToPyObject( python_args[arg] );
        }

        dict[ "__bemacs_call_args__" ] = args_tuple;

        EmacsString command( FormatString
            (
            "__bemacs_eval_tmp__ = apply( %s, __bemacs_call_args__ )\n"
            ) << python_function );

        runPythonStringInsideTryExcept( command );

        Py::Object py_result( dict[ "__bemacs_eval_tmp__" ] );

        if( !failed() )
            commandSucceeded( convertPyObjectToEmacsExpression( py_result ) );
    }
    catch( Py::Exception &e )
    {
        e.clear();
    }

    return true;
}

int python_call_function(void)
{
    EmacsPythonRecursionBarrier barrier;
    if( barrier.barrierClosed() )
        return 0;

    if( check_args( 1, 1+EmacsPythonCallCommand::MAX_ARGS ) )
        return 0;

    EmacsPythonCallCommand py_command;
    if( !string_arg( 1 ) )
        return 0;
    py_command.python_function = ml_value.asString();

    const int first_arg = 2;
    py_command.num_args = cur_exec->p_nargs - 1;
    for( int arg=first_arg; !ml_err && arg<=cur_exec->p_nargs; arg++ )
    {
        if( !eval_arg( arg ) )
            return 0;

        switch( ml_value.exp_type() )
        {
        case ISINTEGER:
        case ISSTRING:
            py_command.python_args[ arg - first_arg ] = ml_value;
            break;

        case ISVOID:
        case ISMARKER:
        case ISWINDOWS:
        case ISARRAY:
            error( FormatString("Python-call - unsupported expression type for arg %d") << arg );
            return 0;
        }
    }

    py_thread->sendCommandToPython( &py_command );
    py_thread->getResultFromPython( &py_command );

    ml_value = py_command.getResult();

    return 0;
}


//================================================================================

static EmacsString calcNewPythonPath(void)
{
    EmacsString new_python_path("PYTHONPATH=");

    EmacsString python_path( get_config_env("PYTHONPATH") );

    if( !python_path.isNull() )
    {
        new_python_path.append( python_path );
        new_python_path.append( ';' );
    }

    EmacsString emacs_path( EMACS_PATH );
    emacs_path.append(';');    // simplify loop
    EmacsString part;
    int start_pos = 0;
    for(;;)
    {
        int index = emacs_path.index( ';', start_pos );
        if( index < 0 )
            break;
        part = emacs_path( start_pos, index );
        start_pos = index+1;

        EmacsString  new_part;
        expand_and_default( part, EmacsString::null, new_part );

        new_python_path.append( new_part );
        new_python_path.append( ';' );
    }

    // remove trailing ';'
    new_python_path.remove( new_python_path.length()-1, 1 );

    return new_python_path;
}

EmacsPythonRecursionBarrier::EmacsPythonRecursionBarrier()
    : owner( false )
{
    if( py_thread == NULL )
    {
        error( start_python_error );
        return;
    }

    DWORD status = WaitForSingleObject( barrier_mutex, 0 );
    if( status == WAIT_OBJECT_0 )
        owner = true;
    else
        error( "Cannot recursively call from emacs to python" );
}

EmacsPythonRecursionBarrier::~EmacsPythonRecursionBarrier()
{
    if( owner )
    {
        DWORD status = ReleaseMutex( barrier_mutex );
        emacs_assert( status != 0 );
    }
}

bool EmacsPythonRecursionBarrier::barrierClosed()
{
    return !owner;
}

HANDLE EmacsPythonRecursionBarrier::barrier_mutex = CreateMutex( NULL, false, NULL );


#endif
