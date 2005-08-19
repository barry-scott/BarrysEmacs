/*	Copyright (c) 1994 Barry A. Scott */
/* Strcutures, and definitions for nt_comm.c */
/* sub-process strcutures and defintions */

#pragma warning( disable: 4201 )
#include <windows.h>
#pragma warning( default: 4201 )

#include <emacs_proc.h>		// common emacs process definitions
#include <nt_async.h>
#include <nt_pipe.h>
#include <nt_proc.h>

const int CHAN_BUFFER_SIZE( 256 );		// Channel Local Buffer Size

class EmacsThread;

class ProcessChannel : public QueueEntry<ProcessChannel>
	{					// I/O channel structure
public:
	EMACS_OBJECT_FUNCTIONS( ProcessChannel )
	ProcessChannel( EmacsProcess * );
	virtual ~ProcessChannel();

	EmacsProcess *chan_process;	/* Process Block */
	EmacsBufferRef chan_buffer;	/* Buf Pointer */
	BoundName *chan_procedure;	/* Proc Pointer */
	int chan_chars_left;			/* # Chars left */
	int chan_num_reads_before_redisplay;	/* do_dsp inhibit */
	int chan_reads_to_go_before_redisplay;	/*	" Counter */
	int chan_maximum_buffer_size;		/* Delete Thresh. */
	int chan_buffer_reduction_size;		/* Amount to delete */
	unsigned chan_interrupt : 1;		/* interrupt occured */
	unsigned chan_read_channel : 1;		/* 1=read, 0=write */
	unsigned chan_channel_open : 1;		/* Channel is open */
	unsigned chan_local_buffer_has_data : 1;/* Data in local buf */
	unsigned chan_data_request : 1;		/* R/W request */
	HANDLE chan_nt_event;
	unsigned char *chan_data_buffer;
	};


class EmacsProcessSessionData
	{
public:
	EmacsProcessSessionData();
	~EmacsProcessSessionData();

	bool createProcessSession(EmacsString &error_detail);
	bool createThreadSession( EmacsThread *thread_object, EmacsString &error_detail );
	bool connectSession( EmacsProcess *proc, EmacsString &error_detail );
	SESSION_DISCONNECT_CODE disconnectSession();
	void deleteSession();

	bool initialised;	// true if createXXXSession worked
	bool process_session;	// true if a process session

	EmacsThread *thread_object;	// valid if process_session == false

	//
	// These fields are filled in at session creation time
	//
	HANDLE  ShellReadPipeHandle;		// Handle to shell stdout pipe
	HANDLE  ShellWritePipeHandle;		// Handle to shell stdin pipe
	HANDLE  ShellProcessHandle;		// Handle to shell process
	HANDLE  ShellThreadHandle;		// Handle to shell process

	//
	// These fields maintain the state of asynchronouse reads/writes
	// to the shell process across client disconnections. They
	// are initialized at session creation.
	//
	BYTE    ShellReadBuffer[SHELL_BUFFER_SIZE];	// Data for shell reads goes here
	HANDLE  ShellReadAsyncHandle;			// Object used for async reads from shell
	BOOL    ShellReadPending;

	BYTE    ShellWriteBuffer[SHELL_BUFFER_SIZE];	// Data for shell writes goes here
	HANDLE  ShellWriteAsyncHandle;			// Object used for async writes to shell
	BOOL    ShellWritePending;

	//
	// These fields are filled in at session connect time and are only
	// valid when the session is connected
	//
	HANDLE  SessionThreadHandle;			// Handle to session thread
	HANDLE  SessionThreadSignalEventHandle;		// Handle to event used to signal thread
	};

class EmacsProcess : public EmacsProcessCommon
	{
public:
	EMACS_OBJECT_FUNCTIONS( EmacsProcess )
	EmacsProcess( const EmacsString &name );
	virtual ~EmacsProcess();

	ProcessChannel proc_output_channel;		// Output Channel
	ProcessChannel proc_input_channel;		// Input Channel
	EmacsProcessSessionData proc_nt_session;	//

	BoundName *proc_procedure;			/* Termination Proc */
	long proc_time_state_was_entered;		/* When it changed */
	int proc_state;					/* Process State */

	int startProcess(EmacsString &error_detail);			// start up the process
	void stopProcess(void);			// stop the process

	// Process States
	// These are carefully ordered so that the following macros work!! */
	enum processStates
		{
		RUNNING = 0,		// Process Running
		PAUSED = 1,		// We Suspended it
		BLOCKED_FOR_INPUT = 2,	// Input Wait
		PAUSED_INPUTBLOCK = 3,	// " " (PAUSED)
		BLOCKED_FOR_OUTPUT = 4,	// Output Wait
		PAUSED_OUTPUTBLOCK = 5,	// " " (PAUSED)
		DEAD = 6		// It Exited
		};

	void PAUSED_TO_RUNNING() { proc_state &= ~PAUSED; }
	void RUNNING_TO_PAUSED() { proc_state |= PAUSED; }
	void RUNNING_TO_BLOCKED( processStates reason ) { proc_state += reason; }
	void BLOCKED_TO_RUNNING() { proc_state &= PAUSED; }
	bool IS_RUNNING() { return (proc_state & PAUSED) == 0; }
	bool IS_PAUSED() { return (proc_state & PAUSED) != 0; }
	bool IS_BLOCKED() { return (proc_state > PAUSED) != 0; }

	static bool nt_console;
	static HANDLE con_in_handle, con_out_handle;
	};


class EmacsThread : public EmacsThreadCommon
	{
public:
	EmacsThread();
	virtual ~EmacsThread();

	virtual unsigned run() = 0;

	HANDLE thread_handle;
	HANDLE stdin_handle;
	HANDLE stdout_handle;
	};
