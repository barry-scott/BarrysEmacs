/****************************** Module Header ******************************\
* Module Name: session.c
*
* Copyright (c) 1991, Microsoft Corporation
*
* Remote shell session module
*
* History:
* 06-28-92 Davidc       Created.
\***************************************************************************/

#include <emacs.h>
#include <nt_comm.h>

#include <io.h>

//
// Define standard handles
//

#define STDIN    0
#define STDOUT   1
#define STDERROR 2


//
// Define shell command line
//

// #define SHELL_COMMAND_LINE  TEXT("cmd /q")
#define	SHELL_COMMAND_LINE s_str(cli_name)
//
// Private prototypes
//

//
// Define the structure used to describe each session
//

typedef struct {

    //
    // These fields are filled in at session creation time
    //

    HANDLE  ShellReadPipeHandle;        // Handle to shell stdout pipe
    HANDLE  ShellWritePipeHandle;        // Handle to shell stdin pipe
    HANDLE  ShellProcessHandle;     // Handle to shell process

    //
    // These fields maintain the state of asynchronouse reads/writes
    // to the shell process across client disconnections. They
    // are initialized at session creation.
    //

    BYTE    ShellReadBuffer[SHELL_BUFFER_SIZE]; // Data for shell reads goes here
    HANDLE  ShellReadAsyncHandle;   // Object used for async reads from shell
    BOOL    ShellReadPending;

    BYTE    ShellWriteBuffer[SHELL_BUFFER_SIZE]; // Data for shell writes goes here
    HANDLE  ShellWriteAsyncHandle; // Object used for async writes to shell
    BOOL    ShellWritePending;

    //
    // These fields are filled in at session connect time and are only
    // valid when the session is connected
    //
    HANDLE  SessionThreadHandle;    // Handle to session thread
    HANDLE  SessionThreadSignalEventHandle; // Handle to event used to signal thread
} SESSION_DATA, *PSESSION_DATA;


HANDLE
StartShell(
    int StdinCrtHandle,
    int StdoutCrtHandle
    );

DWORD
SessionThreadFn(
    LPVOID Parameter
    );


//
// Useful macros
//


/////////////////////////////////////////////////////////////////////////////
//
// CreateSession
//
// Creates a new session. Involves creating the shell process and establishing
// pipes for communication with it.
//
// Returns a handle to the session or NULL on failure.
//
/////////////////////////////////////////////////////////////////////////////

HANDLE
CreateSession(
    VOID
    )
{
    PSESSION_DATA Session = NULL;
    BOOL Result;
    SECURITY_ATTRIBUTES SecurityAttributes;
    HANDLE ShellStdinPipe = NULL;
    HANDLE ShellStdoutPipe = NULL;
    int ShellStdinCrtHandle;
    int ShellStdoutCrtHandle;

    //
    // Allocate space for the session data
    //

    Session = (PSESSION_DATA)malloc(sizeof(SESSION_DATA),malloc_type_char);
    if (Session == NULL)
        return NULL;

    Session->ShellReadPipeHandle  = NULL;
    Session->ShellWritePipeHandle = NULL;
    Session->ShellReadAsyncHandle = NULL;
    Session->ShellWriteAsyncHandle = NULL;


    //
    // Create the I/O pipes for the shell
    //

    SecurityAttributes.nLength = sizeof(SecurityAttributes);
    SecurityAttributes.lpSecurityDescriptor = NULL; // Use default ACL
    SecurityAttributes.bInheritHandle = TRUE; // Shell will inherit handles

    Result = MyCreatePipe(&Session->ShellReadPipeHandle,
                          &ShellStdoutPipe,
                          &SecurityAttributes,
                          0,            // Default pipe size
                          0,            // Default timeout
                          FILE_FLAG_OVERLAPPED, // shell read flags
                          0              // shell stdout flags
                          );
    if (!Result)
	{
        DebugPrintf("Failed to create shell stdout pipe, error = %d\n", GetLastError());
        goto Failure;
	}

    Result = MyCreatePipe(&ShellStdinPipe,
                          &Session->ShellWritePipeHandle,
                          &SecurityAttributes,
                          0,            // Default pipe size
                          0,            // Default timeout
                          0,            // shell stdin flags
                          FILE_FLAG_OVERLAPPED // shell write flags
                          );
    if (!Result) {
        DebugPrintf("Failed to create shell stdin pipe, error = %d\n", GetLastError());
        goto Failure;
    }


    //
    // Initialize async objects
    //

    Session->ShellReadAsyncHandle = CreateAsync(FALSE);
    if (Session->ShellReadAsyncHandle == NULL) {
        DebugPrintf("Failed to create shell read async object, error = %d\n", GetLastError());
        goto Failure;
    }

    Session->ShellWriteAsyncHandle = CreateAsync(FALSE);
    if (Session->ShellWriteAsyncHandle == NULL) {
        DebugPrintf("Failed to create shell write async object, error = %d\n", GetLastError());
        goto Failure;
    }

    Session->ShellReadPending = FALSE;
    Session->ShellWritePending = FALSE;


    //
    // Create a runtime handle for shell pipes
    //

    ShellStdinCrtHandle = _open_osfhandle((long)ShellStdinPipe, 0);
    assert(ShellStdinCrtHandle != -1);
    ShellStdoutCrtHandle = _open_osfhandle((long)ShellStdoutPipe, 0);
    assert(ShellStdoutCrtHandle != -1);


    //
    // Start the shell
    //

    Session->ShellProcessHandle = StartShell(ShellStdinCrtHandle, ShellStdoutCrtHandle);

    //
    // We're finished with our copy of the shell pipe handles
    // Closing the runtime handles will close the pipe handles for us.
    //

    close(ShellStdinCrtHandle);
    ShellStdinPipe = NULL;
    close(ShellStdoutCrtHandle);
    ShellStdoutPipe = NULL;

    //
    // Check result of shell start
    //

    if (Session->ShellProcessHandle == NULL) {
        DebugPrintf("Failed to execute shell\n");
        goto Failure;
    }


    //
    // If any code is added here, remember to cleanup process handle
    // in failure code
    //


    //
    // The session is not connected, initialize variables to indicate that
    //


    //
    // Success, return the session pointer as a handle
    //

    return((HANDLE)Session);



Failure:

    //
    // We get here for any failure case.
    // Free up any resources and exit
    //


    //
    // Cleanup shell pipe handles
    //

    if (ShellStdinPipe != NULL) {
        MyCloseHandle(ShellStdinPipe, "shell stdin pipe (shell side)");
    }

    if (ShellStdoutPipe != NULL) {
        MyCloseHandle(ShellStdoutPipe, "shell stdout pipe (shell side)");
    }

    if (Session->ShellReadPipeHandle != NULL) {
        MyCloseHandle(Session->ShellReadPipeHandle, "shell read pipe (session side)");
    }

    if (Session->ShellWritePipeHandle != NULL) {
        MyCloseHandle(Session->ShellWritePipeHandle, "shell write pipe (session side)");
    }


    //
    // Cleanup async data
    //

    if (Session->ShellReadAsyncHandle != NULL) {
        DeleteAsync(Session->ShellReadAsyncHandle);
    }

    if (Session->ShellWriteAsyncHandle != NULL) {
        DeleteAsync(Session->ShellWriteAsyncHandle);
    }


    //
    // Free up our session data
    //

    free(Session);

    return(NULL);
}




/////////////////////////////////////////////////////////////////////////////
//
// DeleteSession
//
// Deletes the session specified by SessionHandle.
//
// Returns nothing
//
/////////////////////////////////////////////////////////////////////////////

VOID
DeleteSession(
    HANDLE  SessionHandle
    )
{
    PSESSION_DATA   Session = (PSESSION_DATA)SessionHandle;
    BOOL Result;

    //
    // Disconnect session first
    //

    DisconnectSession(SessionHandle);

    //
    // Kill off the shell process
    //

    Result = TerminateProcess(Session->ShellProcessHandle, 1);
    if (!Result) {
        DebugPrintf("Failed to terminate shell, error = %d\n", GetLastError());
    }

    MyCloseHandle(Session->ShellProcessHandle, "shell process");


    //
    // Close the shell pipe handles
    //

    MyCloseHandle(Session->ShellReadPipeHandle, "shell read pipe (session side)");
    MyCloseHandle(Session->ShellWritePipeHandle, "shell write pipe (session side)");


    //
    // Cleanup async data
    //

    DeleteAsync(Session->ShellReadAsyncHandle);
    DeleteAsync(Session->ShellWriteAsyncHandle);


    //
    // Free up the session structure
    //

    free(Session);

    //
    // We're done
    //

    return;
}




/////////////////////////////////////////////////////////////////////////////
//
// ConnectSession
//
// Connects the session specified by SessionHandle to a client
// on the other end of the pipe specified by PipeHandle
//
// Returns a session disconnect notification handle or NULL on failure.
// The returned handle will be signalled if the client disconnects or the
// shell terminates.
// Calling DisconnectSession will return the disconnect notification code.
//
/////////////////////////////////////////////////////////////////////////////

HANDLE
ConnectSession(
    HANDLE  SessionHandle,
    struct process_blk *proc
    )
{
    PSESSION_DATA   Session = (PSESSION_DATA)SessionHandle;
    SECURITY_ATTRIBUTES SecurityAttributes;
    DWORD ThreadId;

    assert(proc != NULL);

    //
    // Create the thread signal event. We'll use this to tell the
    // thread to exit during disconnection.
    //

    SecurityAttributes.nLength = sizeof(SecurityAttributes);
    SecurityAttributes.lpSecurityDescriptor = NULL; // Use default ACL
    SecurityAttributes.bInheritHandle = FALSE; // No inheritance

    Session->SessionThreadSignalEventHandle = CreateEvent(&SecurityAttributes,
                                                      TRUE, // Manual reset
                                                      FALSE, // Initially clear
                                                      NULL); // No name
    if (Session->SessionThreadSignalEventHandle == NULL) {
        DebugPrintf("Failed to create thread signal event, error = %d\n", GetLastError());
        return(NULL);
    }

    proc->proc_input_channel.chan_nt_event = CreateEvent(&SecurityAttributes,
                                                      FALSE,	// Manual reset
                                                      FALSE,	// Initially clear
                                                      NULL);	// No name
    if (proc->proc_input_channel.chan_nt_event == NULL) {
        DebugPrintf("Failed to create thread signal event, error = %d\n", GetLastError());
        return(NULL);
    }

    proc->proc_output_channel.chan_nt_event = CreateEvent(&SecurityAttributes,
                                                      FALSE, // Manual reset
                                                      FALSE, // Initially clear
                                                      NULL); // No name
    if (proc->proc_output_channel.chan_nt_event == NULL) {
        DebugPrintf("Failed to create thread signal event, error = %d\n", GetLastError());
        return(NULL);
    }

    proc->proc_nt_session = Session;
    proc->proc_input_channel.chan_data_buffer = Session->ShellReadBuffer;
    proc->proc_output_channel.chan_data_buffer = Session->ShellWriteBuffer;

    //
    // Create the session thread
    //

    Session->SessionThreadHandle = CreateThread(
                                     &SecurityAttributes,
                                     0,			// Default stack size
             (LPTHREAD_START_ROUTINE)SessionThreadFn,	// Start address
                             (LPVOID)proc,		// Parameter
                                     0,			// Creation flags
                                     &ThreadId		// Thread id
                                     );
    if (Session->SessionThreadHandle == NULL) {
        DebugPrintf("Failed to create session thread, error = %d\n", GetLastError());

        //
        // Close the thread signal event
        //

        MyCloseHandle(Session->SessionThreadSignalEventHandle, "thread signal event");
        MyCloseHandle(proc->proc_input_channel.chan_nt_event,"in chan event");
        MyCloseHandle(proc->proc_output_channel.chan_nt_event,"out chan event");
    }


    return(Session->SessionThreadHandle);
}




/////////////////////////////////////////////////////////////////////////////
//
// DisconnectSession
//
// Disconnects the session specified by SessionHandle for its client.
//
// Returns a disconnect notification code (DisconnectError on failure)
//
/////////////////////////////////////////////////////////////////////////////

SESSION_DISCONNECT_CODE
DisconnectSession(
    HANDLE  SessionHandle
    )
{
    PSESSION_DATA   Session = (PSESSION_DATA)SessionHandle;
    DWORD TerminationCode;
    SESSION_DISCONNECT_CODE DisconnectCode;
    BOOL Result;
    DWORD WaitResult;

    //
    // Signal the thread to terminate (if it hasn't already)
    //

    Result = SetEvent(Session->SessionThreadSignalEventHandle);
    if (!Result) {
        DebugPrintf("Failed to set thread signal event, error = %d\n", GetLastError());
    }

    //
    // Wait for the thread to terminate
    //

    DebugPrintf("Waiting for session thread to terminate...");

    WaitResult = WaitForSingleObject(Session->SessionThreadHandle, INFINITE);
    if (WaitResult != 0) {
        DebugPrintf("Unexpected result from infinite wait on thread handle, result = %d\n", WaitResult);
    }

    DebugPrintf("done\n");


    //
    // Get the thread termination code
    //

    Result = GetExitCodeThread(Session->SessionThreadHandle, &TerminationCode);
    if (!Result) {
        DebugPrintf("Failed to get termination code for thread, error = %d\n", GetLastError());
        TerminationCode = (DWORD)DisconnectError;
    } else {
        if (TerminationCode == STILL_ACTIVE) {
            DebugPrintf("Got termination code for thread, it's still active!\n");
            TerminationCode = (DWORD)DisconnectError;
        }
    }

    DisconnectCode = (SESSION_DISCONNECT_CODE)TerminationCode;



    //
    // Close the thread handle and thread signal event handle
    //

    MyCloseHandle(Session->SessionThreadHandle, "session thread");
    MyCloseHandle(Session->SessionThreadSignalEventHandle, "thread signal event");


    //
    // Reset the client pipe handle to signal that this session is disconnected
    // The pipe handle will have been closed by the session thread on exit
    //

    //
    // We're done
    //

    return(DisconnectCode);
}








/////////////////////////////////////////////////////////////////////////////
//
// StartShell
//
// Execs the shell with the specified handle as stdin, stdout/err
//
// Returns process handle or NULL on failure
//
/////////////////////////////////////////////////////////////////////////////

HANDLE
StartShell(
    int ShellStdinCrtHandle,
    int ShellStdoutCrtHandle
    )
{
    int StdInputHandle;
    int StdOutputHandle;
    int StdErrorHandle;
    int crtResult;
    PROCESS_INFORMATION ProcessInformation;
    STARTUPINFO si;
    HANDLE ProcessHandle = NULL;


    //
    // Replace std handles with appropriate pipe handles and exec the
    // shell process. It will inherit our std handles and we can then
    // reset them to normal
    //


    //
    // Store away our normal i/o handles
    //

    StdInputHandle = dup(STDIN);
//    assert(StdInputHandle != -1);
    StdOutputHandle = dup(STDOUT);
//    assert(StdOutputHandle != -1);
    StdErrorHandle = dup(STDERROR);
//    assert(StdErrorHandle != -1);

    //
    // Replace std handles with pipe handle.
    //

    crtResult = dup2(ShellStdinCrtHandle, STDIN);
    assert(crtResult == 0);
    crtResult = dup2(ShellStdoutCrtHandle, STDOUT);
    assert(crtResult == 0);
    crtResult = dup2(ShellStdoutCrtHandle, STDERROR);
    assert(crtResult == 0);

    //
    // Initialize process startup info
    //

    si.cb = sizeof(STARTUPINFO);
    memset( &si, 0, sizeof( si ) );
    si.lpReserved = NULL;
    si.lpTitle = NULL;
    si.lpDesktop = NULL;
    si.dwX = si.dwY = si.dwXSize = si.dwYSize = si.dwFlags = 0L;
    si.wShowWindow = SW_SHOW;
    si.lpReserved2 = NULL;
    si.cbReserved2 = 0;

    if( CreateProcess
	(
	NULL,
	SHELL_COMMAND_LINE,
	NULL,
	NULL,
	TRUE, // Inherit handles
	0,
	NULL,
	NULL,
	&si,
	&ProcessInformation
	) )
		{
		ProcessHandle = ProcessInformation.hProcess;
		MyCloseHandle(ProcessInformation.hThread, "process thread");
		}
	else
		{
		DebugPrintf("Failed to execute shell, error = %d\n", GetLastError());
		}



    //
    // Restore std handles to normal
    //

    crtResult = dup2(StdInputHandle, STDIN);
    assert(crtResult == 0);
    crtResult = dup2(StdOutputHandle, STDOUT);
    assert(crtResult == 0);
    crtResult = dup2(StdErrorHandle, STDERROR);
    assert(crtResult == 0);

    //
    // Close any handles we created
    //

    crtResult = close(StdInputHandle);
    assert(crtResult == 0);
    crtResult = close(StdOutputHandle);
    assert(crtResult == 0);
    crtResult = close(StdErrorHandle);
    assert(crtResult == 0);


    return(ProcessHandle);
}








/////////////////////////////////////////////////////////////////////////////
//
// SessionThreadFn
//
// This is the code executed by the session thread
//
// Waits for read or write from/to shell or client pipe and termination
// event. Handles reads or writes by passing data to either client or
// shell as appropriate. Any error or termination event being signalled
// causes the thread to exit with an appropriate exit code.
//
/////////////////////////////////////////////////////////////////////////////

extern volatile int terminating_process;

DWORD
SessionThreadFn(
    LPVOID Parameter
    )
    {
    struct process_blk *proc = (struct process_blk *)Parameter;
    PSESSION_DATA   Session = (PSESSION_DATA)proc->proc_nt_session;
    HANDLE  ClientReadAsyncHandle = proc->proc_output_channel.chan_nt_event;
    HANDLE  ClientWriteAsyncHandle = proc->proc_input_channel.chan_nt_event;
    DWORD   BytesTransferred;
    DWORD   CompletionCode;
    BOOL    Result;
    DWORD   ExitCode = 0;
    HANDLE  WaitHandles[6];
    BOOL    Done;
    DWORD   i;
    enum wait_num
	{
	thread_exit = 0,
	process_exit,
	shell_read_complete,
	shell_write_complete,
	emacs_read_ack,
	emacs_write_start,
	wait_error = 0xffffffff
	}
	    WaitResult;

    if (Session->ShellWritePending) {
	DebugPrintf("SessionThread started - SHELL-WRITE-PENDING\n");
    }
    if (Session->ShellReadPending) {
	DebugPrintf("SessionThread started - SHELL-READ-PENDING\n");
    }

    //
    // Initialize the handle array we'll wait on
    //

    WaitHandles[thread_exit] = Session->SessionThreadSignalEventHandle;
    WaitHandles[process_exit] = Session->ShellProcessHandle;
    WaitHandles[shell_read_complete] = GetAsyncCompletionHandle(Session->ShellReadAsyncHandle);
    WaitHandles[shell_write_complete] = GetAsyncCompletionHandle(Session->ShellWriteAsyncHandle);
    WaitHandles[emacs_write_start] = ClientReadAsyncHandle;
    WaitHandles[emacs_read_ack] = ClientWriteAsyncHandle;

    //
    // Wait on our handle array in a loop until an error occurs or
    // we're signalled to exit.
    //

    Done = FALSE;

    while (!Done)
	{
        //
        // Wait for one of our objects to be signalled.
        //
        WaitResult = (enum wait_num)WaitForMultipleObjects(6, WaitHandles, FALSE, INFINITE);

        if (WaitResult == wait_error)
	    {
            DebugPrintf("SessionThread: wait failed, error = %d\n", GetLastError());
            ExitCode = (DWORD)ConnectError;
            break; // out of while
	    }

	// DebugPrintf( "Session thread, waitResult = %d\n", WaitResult );
        switch( WaitResult )
	{
        case thread_exit:
	    {
            //
            // Our thread was signalled
            //
            ExitCode = (DWORD)ClientDisconnected;
            Done = TRUE;
	    if( proc->proc_state != PROC_K_DEAD )
		{
		proc->proc_state = PROC_K_DEAD;
		interlock_inc( &pending_channel_io );
		interlock_inc( &terminating_process );
		}
            break; // out of switch
	    }

	case process_exit:
	    {
	    if( proc->proc_state != PROC_K_DEAD )
		{
		proc->proc_state = PROC_K_DEAD;
		interlock_inc( &pending_channel_io );
		interlock_inc( &terminating_process );
		}
	    Done = TRUE;
	    ExitCode = (DWORD)ShellEnded;
	    break;
	    }

        case shell_read_complete:
	    {
            //
            // Shell read completed
            //

            Session->ShellReadPending = FALSE;

            CompletionCode = GetAsyncResult(Session->ShellReadAsyncHandle,
                                            &BytesTransferred);

            if (CompletionCode != ERROR_SUCCESS) {
                DebugPrintf("Async read from shell returned error, completion code = %d\n", CompletionCode);
                ExitCode = (DWORD)ShellEnded;
                Done = TRUE;
                break; // out of switch
            }

            //
            // Tell process code the data is here;
            //
	    proc->proc_input_channel.chan_chars_left = (int)BytesTransferred;
	    proc->proc_input_channel.chan_interrupt = 1;
	    interlock_inc( &pending_channel_io );
            break; // out of switch
	    }

        case shell_write_complete:
	    {
	    //
	    // Shell write completed
	    //

	    Session->ShellWritePending = FALSE;

	    CompletionCode = GetAsyncResult(Session->ShellWriteAsyncHandle,
					    &BytesTransferred);

	    if (CompletionCode != ERROR_SUCCESS) {
		DebugPrintf("Async write to shell returned error, completion code = %d\n", CompletionCode);
		ExitCode = (DWORD)ShellEnded;
		Done = TRUE;
		break; // out of switch
	    }

	    //
	    // Tell Emacs the data has been read by the shell
	    //
	    proc->proc_output_channel.chan_interrupt = 1;
	    interlock_inc( &pending_channel_io );

	    break; // out of switch
	    }

        case emacs_write_start:
	    {
            //
            // Emacs has data for us to send to the shell
            //
	    BytesTransferred = proc->proc_output_channel.chan_chars_left;

            //
            // Check for Ctrl-C from the client
            //
            for (i=0; i < BytesTransferred; i++)
		{
                if (Session->ShellWriteBuffer[i] == '\003')
		    {

                    //
                    // Generate a Ctrl-C if we have the technology
                    //

		    GenerateConsoleCtrlEvent(CTRL_C_EVENT, 0);

                    //
                    // Remove the Ctrl-C from the buffer
                    //

                    BytesTransferred --;

                    for (; i < BytesTransferred; i++)
                        Session->ShellWriteBuffer[i] = Session->ShellWriteBuffer[i+1];
		    }
		}

            //
            // Start an async write to shell
            //

            Result = WriteFileAsync(Session->ShellWritePipeHandle,
                                    Session->ShellWriteBuffer,
                                    BytesTransferred,
                                    Session->ShellWriteAsyncHandle);
            if (!Result)
		{
                DebugPrintf("Async write to shell failed, error = %d\n", GetLastError());
                ExitCode = (DWORD)ShellEnded;
                Done = TRUE;
		}
	    else
                Session->ShellWritePending = TRUE;

            break; // out of switch
	    }

        case emacs_read_ack:
	    {
            //
            // Emacs has taken the data
            // Start an async read from shell
            //

            Result = ReadFileAsync(Session->ShellReadPipeHandle,
                                   Session->ShellReadBuffer,
                                   sizeof(Session->ShellReadBuffer),
                                   Session->ShellReadAsyncHandle);
            if (!Result) {
                DebugPrintf("Async read from shell failed, error = %d\n", GetLastError());
                ExitCode = (DWORD)ShellEnded;
                Done = TRUE;
            } else {
                Session->ShellReadPending = TRUE;
            }

            break; // out of switch
	    }

        default:
		{
		DebugPrintf("Session thread, unexpected result from wait, result = %d\n", WaitResult);
		ExitCode = (DWORD)ConnectError;
		Done = TRUE;
		break;
		}
        }
	}



    //
    // Cleanup and exit
    //

    //
    // Return the appropriate exit code
    //

    return(ExitCode);
    }
