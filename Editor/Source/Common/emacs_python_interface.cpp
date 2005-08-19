//
//	Emacs_python_interface.cpp
//
//	Copyright (c) 1999-2002 Barry A. Scott
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
static int python_watch_dog( void *param );
static DWORD WINAPI python_watch_dog_starter( LPVOID );

extern void init_bemacs_python(void);
extern Expression convertPyObjectToEmacsExpression( const Py::Object &obj );
extern Py::Object convertEmacsExpressionToPyObject( const Expression &expr );

SystemExpressionRepresentationString python_flags;

static bool python_initialised( false );

static PyThreadState *emacs_python_thread_save = NULL;
static HANDLE h_watch_dog_thread;
static HANDLE h_watch_dog_queued_event;

int python_init()
	{
	if( python_initialised )
		{
		ml_value = int(0);
		return 0;
		}

	if( !Py::InitialisePythonIndirectInterface() )
		{
#ifdef _DEBUG
		const char *debug_suffix = "_d";
#else
		const char *debug_suffix = "";
#endif
		error( FormatString("Python%s %d.%d is not installed on this computer")
			<< debug_suffix << PY_MAJOR_VERSION << PY_MINOR_VERSION );

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
			error( FormatString( "Unknown Python-flag \"%c\"" ) << flags[i] );
			return 0;
			}
			}
	}

	EmacsString new_python_path( calcNewPythonPath() );
	putenv( new_python_path.sdata() );

	Py_SetProgramName("bemacs");	// must be before init

	Py_Initialize();


	char *py_argv[2];
	py_argv[0] = "bemacs";
	py_argv[1] = NULL;
	PySys_SetArgv( 1, py_argv );	// must be after init

	// force threading support
	PyEval_InitThreads();

	init_bemacs_python();

	// allow other threads to run (expect there to be none at this point)
	emacs_python_thread_save = PyEval_SaveThread();


	h_watch_dog_queued_event = CreateEvent( NULL, FALSE, FALSE, NULL );

	h_watch_dog_thread = CreateThread
		(
		NULL,				// SEC_ATTRS SecurityAttributes,
		16*1024,			// ULONG StackSize,
		python_watch_dog_starter,	// SEC_THREAD_START StartFunction,
		NULL,				// PVOID ThreadParameter,
		0,				// ULONG CreationFlags,
		NULL				// PULONG ThreadId
		);


	// signal the event to say that there is no watch dog queued
	SetEvent( h_watch_dog_queued_event );

	python_initialised = true;

	ml_value = int(1);
	return 0;
	}

bool python_is_initialised_check()
	{
	if( !python_initialised )
		{
		error("Python has not been initialised");
		return false;
		}

	return true;
	}

//--------------------------------------------------------------------------------
//
//	Python Watch Dog
//
//--------------------------------------------------------------------------------
static DWORD WINAPI python_watch_dog_starter( LPVOID )
	{
	for(;;)
		{
		WaitForSingleObject( h_watch_dog_queued_event, INFINITE );
		Sleep( 50 );

		Py_AddPendingCall( python_watch_dog, NULL );
		}
	}


extern bool check_for_interrupt_key();

static int python_watch_dog( void * )
	{
	//
	// See if there is any input activity to process
	//
	theActiveView->k_check_for_input();

	if( check_for_interrupt_key() )
		{
		// check the interrupt state
		interrupt_key_struck = 0;
		// interrupt Python
		PyErr_SetInterrupt();
		}

	// signal we are out of the queue
	SetEvent( h_watch_dog_queued_event );
	
	return 0;
	}


//--------------------------------------------------------------------------------
//
//	Emacs Python Eval
//
//--------------------------------------------------------------------------------

//--------------------------------------------------------------------------------
//
//	Emacs Python Eval
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
	virtual void executeCommandImpl();
	EmacsString python_command;
	};

void EmacsPythonEvalCommand::executeCommandImpl()
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
		}
	}


int python_eval_string(void)
	{
	if( !python_is_initialised_check() )
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

	py_command.executeCommand();

	if( py_command.failed() )
		error( py_command.failureReason() );
	else
		ml_value = py_command.getResult();

	return 0;
	}

//--------------------------------------------------------------------------------
//
//	Emacs Python Exec
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
	virtual void executeCommandImpl();
	EmacsString python_command;
	};

void EmacsPythonExecCommand::executeCommandImpl()
	{
	runPythonStringInsideTryExcept( python_command.sdataHack() );
	}

int python_exec_string(void)
	{
	if( !python_is_initialised_check() )
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

	py_command.executeCommand();

	if( py_command.failed() )
		error( py_command.failureReason() );
	else
		ml_value = py_command.getResult();

	return 0;
	}

//--------------------------------------------------------------------------------
//
//	Emacs Python Import
//
//--------------------------------------------------------------------------------
int python_import_module(void)
	{
	if( !python_is_initialised_check() )
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

	py_command.executeCommand();

	if( py_command.failed() )
		error( py_command.failureReason() );
	else
		ml_value = py_command.getResult();

	return 0;
	}

//--------------------------------------------------------------------------------
//
//	Emacs Python Call
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

	virtual void executeCommandImpl();
	EmacsString python_function;
	enum { MAX_ARGS=20 };
	int num_args;
	Expression python_args[ MAX_ARGS ];
	};

void EmacsPythonCallCommand::executeCommandImpl()
	{
	try
		{
		Py::Module module( "__main__" );
		Py::Dict dict( module.getDict() );

		//
		//	create the tuple of arguments
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

		if( !failed() )
			{
			Py::Object py_result( dict[ "__bemacs_eval_tmp__" ] );
			commandSucceeded( convertPyObjectToEmacsExpression( py_result ) );
			}
		}
	catch( Py::Exception &e )
		{
		e.clear();
		}
	}

int python_call_function(void)
	{
	if( !python_is_initialised_check() )
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

	py_command.executeCommand();

	if( py_command.failed() )
		error( py_command.failureReason() );
	else
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
	emacs_path.append(';');	// simplify loop
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

//================================================================================
//
//	EmacsPythonCommand
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
	//_dbg_msg( "commandSucceeded: " );

	command_failed = false;
	result = _result;
	}

void EmacsPythonCommand::commandFailed( const EmacsString &_reason )
	{
	//_dbg_msg( FormatString( "commandFailed: reason: %s" ) << _reason );

	command_failed = true;
	failure_reason = _reason;
	}	


static EmacsString python_try_command_except_structure
	(
	"try:\n"
	"	__bemacs_error = None\n"	// No error yet
	"	%s\n"				// substitute command in here
	"except:\n"
	"	import sys, traceback, string\n"
	"	__bemacs_type, __bemacs_value, __bemacs_traceback_obj = sys.exc_info()\n"
	"	__bemacs_list = traceback.format_exception( __bemacs_type, __bemacs_value, __bemacs_traceback_obj )\n"
	"	__bemacs_error = __bemacs_list[-1] + string.join( __bemacs_list[:-1] )\n"
	"	__bemacs_traceback_obj = None\n"
	);

void EmacsPythonCommand::runPythonStringInsideTryExcept( const EmacsString &_command )
	{
	EmacsString command( _command );

	//_dbg_msg( FormatString( "runPythonStringInsideTryExcept: Command: %s" ) << command );

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
		//_dbg_msg( "runPythonStringInsideTryExcept: catch()" );
		e.clear();
		}
	}



bool EmacsPythonCommand::executeCommand()
	{
	// get back control of the python interpreter for the emacs thread to use
	PyEval_RestoreThread( emacs_python_thread_save );

	executeCommandImpl();

	// allow other threads to run
	emacs_python_thread_save = PyEval_SaveThread();

	return failed();
	}

#endif
