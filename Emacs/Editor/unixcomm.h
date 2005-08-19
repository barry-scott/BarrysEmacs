//
//	unixcomm.h
//
//	Copyright (c) 1994 Barry A. Scott
//
#include <emacs_proc.h>	// for EmacsProcessCommon

class ProcessChannelOutput
	{
public:
	ProcessChannelOutput();
	~ProcessChannelOutput();
	int ch_fd;			// Channel index
	int ch_count;		// Count of remaining chars in data
	int ch_ccount;		// Cound of control chars
	unsigned char *ch_buf;	// Base buffer containing data
	unsigned char *ch_data;	// Pointer to next segment to send
	};

//
// Structure records pertinent information about channels open.
// There is one channel associated with each process.
//
class ProcessChannelInput
	{
public:
	ProcessChannelInput();
	~ProcessChannelInput();

	int ch_fd;
	unsigned char *ch_ptr;		// Pointer to next input character
	int ch_count;			// Count of characters remaining in buffer
	EmacsBuffer *ch_buffer;		// Process is bound to this buffer
	BoundName *ch_proc;		// Procedure which gets called on output
	};

// Structure for information needed for each sub process started

class EmacsPosixSignal;
class EmacsProcess : public EmacsProcessCommon
	{
public:
	EmacsProcess( const EmacsString &name, const EmacsString &command );
	virtual ~EmacsProcess();

	bool startProcess( EmacsPosixSignal &sig );
	void stopProcess();
	static void flushProcess( EmacsProcess *process );
	static EmacsProcess *findProcess( const EmacsString &name );
	static EmacsProcess *getNextProcess(void);
	static int countProcesses(void);
	inline bool activeProcess(void);
	static void killProcesses( void );

	static EmacsProcess *current_process;	// the one that we are current dealing with

	EMACS_OBJECT_FUNCTIONS( EmacsProcess )
	ProcessChannelInput chan_in;		// input channel
	ProcessChannelOutput chan_out;		// output channel
	EmacsString command;			// command that started process
	BoundName *term_proc;			// Terminate procedure
	XtInputId in_id;			// Input event
	XtInputId out_id;			// Output event
	pid_t p_id;				// process id (group id as well
	char p_flag;				// RUNNING, STOPPED, etc
	char p_reason;				// signal causing p_flag
	char out_id_valid;			// True if out_id is valid
	};
