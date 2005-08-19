//
//	Emacs_python_interface.h
//
//	Copyright (c) 1999 Barry A. Scott
//
class EmacsPythonCommand;

class EmacsPythonThread: public EmacsThread
	{
public:
	EmacsPythonThread( EmacsProcess *proc );
	virtual ~EmacsPythonThread();

	unsigned run();

	void writeToConsole( const EmacsString &string );

	// client uses these functions
	bool sendCommandToPython( EmacsPythonCommand *command );
	bool getResultFromPython( EmacsPythonCommand *command );

	// server uses these functions
	bool getCommandFromEmacs( EmacsPythonCommand **command );
	bool sendResultToEmacs( EmacsPythonCommand *command );

private:
	HANDLE command_event;			// the owner is allowed to get the command
	HANDLE result_event;			// the owner is allowed to get the result

	EmacsPythonCommand *the_command;
	EmacsProcess *proc;

	FILE *py_stdin;
	FILE *py_stdout;
	FILE *py_stderr;
	};


class EmacsPythonCommand
	{
public:
	EmacsPythonCommand();
	virtual ~EmacsPythonCommand();

	virtual bool executeCommand() = 0;	

	

	bool failed() const;
	const EmacsString &failureReason() const;
	const Expression &getResult() const;

	void commandSucceeded( const Expression &result );
	void commandFailed( const EmacsString &reason );

protected:
	void runPythonStringInsideTryExcept( const EmacsString &command );

private:
	bool command_failed;
	EmacsString failure_reason;
	Expression result;
	};


class EmacsPythonRecursionBarrier
	{
public:
	EmacsPythonRecursionBarrier();
	virtual ~EmacsPythonRecursionBarrier();

	bool barrierClosed();
private:
	bool owner;
	static HANDLE barrier_mutex;
	};
