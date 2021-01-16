//
//Module Name: session.c
//
//Copyright (c) 1991, Microsoft Corporation
//
//Remote shell session module
//
//History:
//06-28-92 Davidc       Created.

#include <emacs.h>

#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
static EmacsInitialisation emacs_initialisation( __DATE__ " " __TIME__, THIS_FILE );

#include <nt_comm.h>

#include <io.h>
#include <process.h>

//
// Define standard handles
//

#define STDIN    0
#define STDOUT   1
#define STDERROR 2


//
// Define shell command line
//

//
// Private prototypes
//
DWORD SessionThreadFn( LPVOID Parameter );

//
// Define the structure used to describe each session
//



//
// Useful macros
//


EmacsProcessSessionData::EmacsProcessSessionData()
    : initialised(false)
    , ShellReadPipeHandle(NULL)
    , ShellWritePipeHandle(NULL)
    , ShellProcessHandle(NULL)
    , ShellReadAsyncHandle(NULL)
    , ShellReadPending(false)
    , ShellWriteAsyncHandle(NULL)
    , ShellWritePending(false)
    , SessionThreadHandle(NULL)
    , SessionThreadSignalEventHandle(NULL)
{
}

EmacsProcessSessionData::~EmacsProcessSessionData()
{
    //
    // Cleanup shell pipe handles
    //

    if (ShellReadPipeHandle != NULL)
        MyCloseHandle( ShellReadPipeHandle, "shell read pipe (session side)" );

    if (ShellWritePipeHandle != NULL)
        MyCloseHandle( ShellWritePipeHandle, "shell write pipe (session side)" );


    //
    // Cleanup async data
    //

    if (ShellReadAsyncHandle != NULL)
        DeleteAsync( ShellReadAsyncHandle );

    if (ShellWriteAsyncHandle != NULL)
        DeleteAsync( ShellWriteAsyncHandle );
}

/////////////////////////////////////////////////////////////////////////////
//
// CreateProcessSession
//
// Creates a new session. Involves creating the shell process and establishing
// pipes for communication with it.
//
// Returns a handle to the session or NULL on failure.
//
/////////////////////////////////////////////////////////////////////////////

bool EmacsProcessSessionData::createProcessSession(EmacsString &error_detail)
{
    BOOL Result;
    SECURITY_ATTRIBUTES SecurityAttributes;

    HANDLE ShellStdinPipe = NULL;
    HANDLE ShellStdoutPipe = NULL;

    process_session = true;

    try
    {
        //
        // Create the I/O pipes for the shell
        //
        SecurityAttributes.nLength = sizeof(SecurityAttributes);
        SecurityAttributes.lpSecurityDescriptor = NULL; // Use default ACL
        SecurityAttributes.bInheritHandle = TRUE; // Shell will inherit handles

        Result = MyCreatePipe
            (
            &ShellReadPipeHandle,
            &ShellStdoutPipe,
            &SecurityAttributes,
            0,            // Default pipe size
            0,            // Default timeout
            FILE_FLAG_OVERLAPPED,    // shell read flags
            0            // shell stdout flags
            );
        if (!Result)
        {
            error_detail = FormatString("Failed to create shell stdout pipe, error = %E") << GetLastError();
            DebugPrintf( error_detail );
            throw( int(1) );
        }

        Result = MyCreatePipe
            (
            &ShellStdinPipe,
            &ShellWritePipeHandle,
            &SecurityAttributes,
            0,            // Default pipe size
            0,            // Default timeout
            0,            // shell stdin flags
            FILE_FLAG_OVERLAPPED    // shell write flags
            );
        if (!Result)
        {
            error_detail = FormatString("Failed to create shell stdin pipe, error = %E") << GetLastError();
            DebugPrintf( error_detail );
            throw( int(2) );
        }


        //
        // Initialize async objects
        //

        ShellReadAsyncHandle = CreateAsync(FALSE);
        if (ShellReadAsyncHandle == NULL)
        {
            error_detail = FormatString("Failed to create shell read async object, error = %E") << GetLastError();
            DebugPrintf( error_detail );
            throw( int(3) );
        }

        ShellWriteAsyncHandle = CreateAsync(FALSE);
        if (ShellWriteAsyncHandle == NULL)
        {
            error_detail = FormatString("Failed to create shell write async object, error = %E") << GetLastError();
            DebugPrintf( error_detail );
            throw( int(4) );
        }

        ShellReadPending = FALSE;
        ShellWritePending = FALSE;

        //
        // Start the shell
        //

    {
        PROCESS_INFORMATION ProcessInformation;
        STARTUPINFO si;
        HANDLE ProcessHandle = NULL;


        //
        // Replace std handles with appropriate pipe handles and exec the
        // shell process. It will inherit our std handles and we can then
        // reset them to normal
        //


        //
        // Initialize process startup info
        //
        memset( &si, 0, sizeof( si ) );
        si.cb = sizeof(STARTUPINFO);

        // HIde the windows
        si.wShowWindow = SW_HIDE;

        // give the process our pipes are input and output
        si.hStdInput = ShellStdinPipe;
        si.hStdOutput = ShellStdoutPipe;
        si.hStdError = ShellStdoutPipe;

        si.dwFlags = STARTF_USESTDHANDLES|STARTF_USESHOWWINDOW;

        int status = CreateProcess
            (
            NULL,
            (char *)cli_name.sdata(),    // typically cmd.exe /q
            NULL,
            NULL,
            TRUE, // Inherit handles
            0,
            NULL,
            NULL,
            &si,
            &ProcessInformation
            );
        if( status )
        {
            ProcessHandle = ProcessInformation.hProcess;
            MyCloseHandle(ProcessInformation.hThread, "process thread");
        }
        else
        {
            error_detail = FormatString("Failed to execute shell, error = %E") << GetLastError();
            DebugPrintf( error_detail );
        }



        MyCloseHandle( ShellStdinPipe, "shell std in pipe" );
        ShellStdinPipe = NULL;
        MyCloseHandle( ShellStdoutPipe, "shell std out pipe" );
        ShellStdoutPipe = NULL;

        ShellProcessHandle = ProcessHandle;
    }

        //
        // Check  result of shell start
        //

        if (ShellProcessHandle == NULL)
        {
            throw( int(10) );
        }

        //
        // Success
        //

        return true;
    }
    catch( int )
    {
        if (ShellStdinPipe != NULL)
            MyCloseHandle(ShellStdinPipe, "shell stdin pipe (shell side)");

        if (ShellStdoutPipe != NULL)
            MyCloseHandle(ShellStdoutPipe, "shell stdout pipe (shell side)");
    }


    return false;
}

/////////////////////////////////////////////////////////////////////////////
//
// CreateThreadSession
//
// Creates a new session. Involves creating the new thread and establishing
// pipes for communication with it.
//
// Returns a handle to the session or NULL on failure.
//
/////////////////////////////////////////////////////////////////////////////
static unsigned __stdcall threadBootstrap( void *parm );

bool EmacsProcessSessionData::createThreadSession( EmacsThread *_thread_object, EmacsString &error_detail )
{
    BOOL Result;
    SECURITY_ATTRIBUTES SecurityAttributes;

    HANDLE ShellStdinPipe = NULL;
    HANDLE ShellStdoutPipe = NULL;

    process_session = false;
    thread_object = _thread_object;

    try
    {
        //
        // Create the I/O pipes for the shell
        //
        SecurityAttributes.nLength = sizeof(SecurityAttributes);
        SecurityAttributes.lpSecurityDescriptor = NULL; // Use default ACL
        SecurityAttributes.bInheritHandle = TRUE;    // Shell will inherit handles

        Result = MyCreatePipe
            (
            &ShellReadPipeHandle,
            &ShellStdoutPipe,
            &SecurityAttributes,
            0,            // Default pipe size
            0,            // Default timeout
            FILE_FLAG_OVERLAPPED,    // shell read flags
            0            // shell stdout flags
            );
        if (!Result)
        {
            error_detail = FormatString("Failed to create thread stdout pipe, error = %E") << GetLastError();
            DebugPrintf( error_detail );
            throw( int(1) );
        }

        Result = MyCreatePipe
            (
            &ShellStdinPipe,
            &ShellWritePipeHandle,
            &SecurityAttributes,
            0,            // Default pipe size
            0,            // Default timeout
            0,            // shell stdin flags
            FILE_FLAG_OVERLAPPED    // shell write flags
            );
        if (!Result)
        {
            error_detail = FormatString("Failed to create thread stdin pipe, error = %E") << GetLastError();
            DebugPrintf( error_detail );
            throw( int(2) );
        }


        //
        // Initialize async objects
        //

        ShellReadAsyncHandle = CreateAsync(FALSE);
        if (ShellReadAsyncHandle == NULL)
        {
            error_detail = FormatString("Failed to create thread read async object, error = %E") << GetLastError();
            DebugPrintf( error_detail );
            throw( int(3) );
        }

        ShellWriteAsyncHandle = CreateAsync(FALSE);
        if (ShellWriteAsyncHandle == NULL)
        {
            error_detail = FormatString("Failed to create thread write async object, error = %E") << GetLastError();
            DebugPrintf( error_detail );
            throw( int(4) );
        }

        ShellReadPending = FALSE;
        ShellWritePending = FALSE;

        //
        // Start the thread
        //
        thread_object->thread_handle = 0;
        thread_object->stdin_handle = ShellStdinPipe;
        thread_object->stdout_handle = ShellStdoutPipe;

        //
        // Creat the thread and pass it the pipes
        //
        if( !_beginthreadex
            (
            NULL,            // void *security,
            0,            // unsigned stack_size,
            threadBootstrap,    // unsigned ( __stdcall *start_address )( void * ),
            thread_object,        // void *arglist,
            0,            // unsigned initflag,
            (unsigned *)&thread_object->thread_handle
                        // unsigned *thrdaddr
            ) )
        {
            error_detail = "Failed to create thread";
            DebugPrintf( error_detail );
            throw( int(10) );
        }

        //
        // Success
        //

        return true;
    }
    catch( int )
    {
        if (ShellStdinPipe != NULL)
            MyCloseHandle(ShellStdinPipe, "shell stdin pipe (shell side)");

        if (ShellStdoutPipe != NULL)
            MyCloseHandle(ShellStdoutPipe, "shell stdout pipe (shell side)");
    }


    return false;
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

void EmacsProcessSessionData::deleteSession(void)
{
    BOOL Result;

    if( !initialised )
        return;

    //
    // Disconnect session first
    //
    disconnectSession();

    if( process_session )
    {
        //
        // Kill off the shell process
        //
        Result = TerminateProcess( ShellProcessHandle, 1 );
        if (!Result)
        {
            DebugPrintf(FormatString("Failed to terminate shell, error = %E\n") << GetLastError());
        }

        MyCloseHandle( ShellProcessHandle, "shell process" );
    }
    else
    {
        delete thread_object;
        thread_object = NULL;
    }

    //
    // Close the shell pipe handles
    //
    MyCloseHandle( ShellReadPipeHandle, "shell read pipe (session side)" );
    ShellReadPipeHandle = NULL;
    MyCloseHandle( ShellWritePipeHandle, "shell write pipe (session side)" );
    ShellWritePipeHandle = NULL;

    //
    // Cleanup async data
    //
    DeleteAsync( ShellReadAsyncHandle );
    ShellReadAsyncHandle = NULL;
    DeleteAsync( ShellWriteAsyncHandle );
    ShellWriteAsyncHandle = NULL;

    //
    // We're done
    //
    initialised = false;
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

bool EmacsProcessSessionData::connectSession( EmacsProcess *proc, EmacsString &error_detail )
{
    SECURITY_ATTRIBUTES SecurityAttributes;
    DWORD ThreadId;

    emacs_assert(proc != NULL);

    //
    // Create the thread signal event. We'll use this to tell the
    // thread to exit during disconnection.
    //
    SecurityAttributes.nLength = sizeof(SecurityAttributes);
    SecurityAttributes.lpSecurityDescriptor = NULL; // Use default ACL
    SecurityAttributes.bInheritHandle = FALSE; // No inheritance

    SessionThreadSignalEventHandle =
        CreateEvent
        (
        &SecurityAttributes,
        TRUE,        // Manual reset
        FALSE,        // Initially clear
        NULL        // No name
        );
    if( SessionThreadSignalEventHandle == NULL )
    {
        error_detail = FormatString("Failed to create thread signal event, error = %E") << GetLastError();
        DebugPrintf( error_detail );
        return false;
    }

    proc->proc_input_channel.chan_nt_event =
        CreateEvent
        (
        &SecurityAttributes,
        FALSE,    // Manual reset
        FALSE,    // Initially clear
        NULL
        );    // No name
    if( proc->proc_input_channel.chan_nt_event == NULL )
    {
        error_detail = FormatString("Failed to create thread signal event, error = %E") << GetLastError();
        DebugPrintf( error_detail );
        return false;
    }

    proc->proc_output_channel.chan_nt_event =
        CreateEvent
        (
        &SecurityAttributes,
        FALSE, // Manual reset
        FALSE, // Initially clear
        NULL
        ); // No name
    if( proc->proc_output_channel.chan_nt_event == NULL )
    {
        error_detail = FormatString("Failed to create thread signal event, error = %E") << GetLastError();
        DebugPrintf( error_detail );
        return false;
    }

    proc->proc_input_channel.chan_data_buffer = ShellReadBuffer;
    proc->proc_output_channel.chan_data_buffer = ShellWriteBuffer;

    //
    // Create the session thread
    //
    SessionThreadHandle =
        CreateThread
        (
        &SecurityAttributes,
        0,            // Default stack size
        (LPTHREAD_START_ROUTINE)SessionThreadFn,    // Start address
        (LPVOID)proc,        // Parameter
        0,            // Creation flags
        &ThreadId        // Thread id
        );
    if( SessionThreadHandle == NULL )
    {
        error_detail = FormatString("Failed to create session thread, error = %E") << GetLastError();
        DebugPrintf( error_detail );

        //
        // Close the thread signal event
        //
        MyCloseHandle( SessionThreadSignalEventHandle, "thread signal event" );
        MyCloseHandle( proc->proc_input_channel.chan_nt_event,"in chan event" );
        MyCloseHandle( proc->proc_output_channel.chan_nt_event,"out chan event" );
    }


    initialised = true;

    return true;
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

SESSION_DISCONNECT_CODE EmacsProcessSessionData::disconnectSession()
{
    DWORD TerminationCode;
    SESSION_DISCONNECT_CODE DisconnectCode;
    BOOL Result;
    DWORD WaitResult;

    if( !initialised )
        return ShellEnded;

    //
    // Signal the thread to terminate (if it hasn't already)
    //

    Result = SetEvent( SessionThreadSignalEventHandle );
    if (!Result)
    {
        DebugPrintf( FormatString("Failed to set thread signal event, error = %E\n") << GetLastError() );
    }

    //
    // Wait for the thread to terminate
    //

    DebugPrintf("Waiting for session thread to terminate...");

    WaitResult = WaitForSingleObject( SessionThreadHandle, INFINITE );
    if (WaitResult != 0)
    {
        DebugPrintf(FormatString("Unexpected result from infinite wait on thread handle, result = %d\n") << WaitResult);
    }

    DebugPrintf("done\n");


    //
    // Get the thread termination code
    //
    Result = GetExitCodeThread( SessionThreadHandle, &TerminationCode );
    if (!Result)
    {
        DebugPrintf(FormatString("Failed to get termination code for thread, error = %E\n") << GetLastError());
        TerminationCode = (DWORD)DisconnectError;
    }
    else
    {
        if (TerminationCode == STILL_ACTIVE)
        {
            DebugPrintf("Got termination code for thread, it's still active!\n");
            TerminationCode = (DWORD)DisconnectError;
        }
    }

    DisconnectCode = (SESSION_DISCONNECT_CODE)TerminationCode;

    //
    // Close the thread handle and thread signal event handle
    //
    MyCloseHandle( SessionThreadHandle, "session thread" );
    MyCloseHandle( SessionThreadSignalEventHandle, "thread signal event" );


    //
    // Reset the client pipe handle to signal that this session is disconnected
    // The pipe handle will have been closed by the session thread on exit
    //

    //
    // We're done
    //

    return DisconnectCode;
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

DWORD SessionThreadFn( LPVOID Parameter )
{
    EmacsProcess *proc = (EmacsProcess *)Parameter;
    EmacsProcessSessionData *Session = &proc->proc_nt_session;
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
        shell_read_complete = 0,
        shell_write_complete,
        emacs_read_ack,
        emacs_write_start,
        thread_exit,
        process_exit,
        wait_error = 0xffffffff
    }
            WaitResult;

    static struct _wait_num_names
    {
        wait_num num;
        const char *name;
    }
            wait_num_names[] =
    {
        thread_exit, "thread_exit",
        shell_read_complete, "shell_read_complete",
        shell_write_complete, "shell_write_complete",
        emacs_read_ack, "emacs_read_ack",
        emacs_write_start, "emacs_write_start",
        process_exit, "process_exit",
        wait_error, "wait_error"
    };

    if (Session->ShellWritePending)
    {
        DebugPrintf("SessionThread started - SHELL-WRITE-PENDING\n");
    }
    if (Session->ShellReadPending)
    {
        DebugPrintf("SessionThread started - SHELL-READ-PENDING\n");
    }

    //
    // Initialize the handle array we'll wait on
    //
    WaitHandles[thread_exit] =        Session->SessionThreadSignalEventHandle;
    WaitHandles[shell_read_complete] =    GetAsyncCompletionHandle(Session->ShellReadAsyncHandle);
    WaitHandles[shell_write_complete] =    GetAsyncCompletionHandle(Session->ShellWriteAsyncHandle);
    WaitHandles[emacs_write_start] =    ClientReadAsyncHandle;
    WaitHandles[emacs_read_ack] =        ClientWriteAsyncHandle;
    WaitHandles[process_exit] =        Session->ShellProcessHandle;
    int num_handles_to_wait_on = Session->process_session ? 6 : 5;

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
        WaitResult = (wait_num)WaitForMultipleObjects
            (
            num_handles_to_wait_on,
            WaitHandles,
            FALSE,
            INFINITE
            );

        if (WaitResult == wait_error)
        {
            DebugPrintf(FormatString("Session thread wait failed, error = %E\n") << GetLastError());
            ExitCode = (DWORD)ConnectError;
            break; // out of while
        }

#if DBG_PROCESS && DBG_TMP
        if( dbg_flags&(DBG_PROCESS|DBG_TMP) )
        {
            const char *name = "Unknown";
            for( int i=0; i<sizeof( wait_num_names )/sizeof( struct _wait_num_names ); i++ )
                if( wait_num_names[i].num == WaitResult )
                    name = wait_num_names[i].name;
            DebugPrintf( FormatString("SessionThreadFn thread WaitResult = %s(%d)\n") << name << WaitResult );
        }
#endif
        switch( WaitResult )
        {
        case thread_exit:
        {
            //
            // Our thread was signalled
            //
            ExitCode = (DWORD)ClientDisconnected;
            Done = TRUE;
            if( proc->proc_state != EmacsProcess::DEAD )
            {
                proc->proc_state = EmacsProcess::DEAD;
                interlock_inc( &pending_channel_io );
                interlock_inc( &terminating_process );
                wake_main_thread();
            }
            break; // out of switch
        }

        case process_exit:
        {
            if( proc->proc_state != EmacsProcess::DEAD )
            {
                proc->proc_state = EmacsProcess::DEAD;
                interlock_inc( &pending_channel_io );
                interlock_inc( &terminating_process );
                wake_main_thread();
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

            CompletionCode = GetAsyncResult( Session->ShellReadAsyncHandle, &BytesTransferred );

            if (CompletionCode != ERROR_SUCCESS)
            {
                // Its expected that this failure happends just before the process exits.
                DebugPrintf(FormatString("Async read from shell returned error, completion code = %d\n") << CompletionCode);
                break; // out of switch
            }

            //
            // Tell process code the data is here;
            //
            proc->proc_input_channel.chan_chars_left = (int)BytesTransferred;
            proc->proc_input_channel.chan_interrupt = 1;
            interlock_inc( &pending_channel_io );
            wake_main_thread();
            break; // out of switch
        }

        case shell_write_complete:
        {
            //
            // Shell write completed
            //

            Session->ShellWritePending = FALSE;

            CompletionCode = GetAsyncResult( Session->ShellWriteAsyncHandle, &BytesTransferred );

            if (CompletionCode != ERROR_SUCCESS)
            {
                DebugPrintf(FormatString("Async write to shell returned error, completion code = %d\n") << CompletionCode);
                ExitCode = (DWORD)ShellEnded;
                Done = TRUE;
                break; // out of switch
            }

            //
            // Tell Emacs the data has been read by the shell
            //
            proc->proc_output_channel.chan_interrupt = 1;
            interlock_inc( &pending_channel_io );
            wake_main_thread();

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
            Result = WriteFileAsync
                (
                Session->ShellWritePipeHandle,
                Session->ShellWriteBuffer,
                BytesTransferred,
                Session->ShellWriteAsyncHandle
                );
            if (!Result)
            {
                DebugPrintf(FormatString("Async write to shell failed, error = %E\n") << GetLastError());
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

            Result = ReadFileAsync
                (
                Session->ShellReadPipeHandle,
                Session->ShellReadBuffer,
                sizeof(Session->ShellReadBuffer),
                Session->ShellReadAsyncHandle
                );
            if (!Result)
            {
                // Its expected that this failure happends just before the process exits.
                DebugPrintf(FormatString("Async read from shell failed, error = %E\n") << GetLastError());
            }
            else
            {
                Session->ShellReadPending = TRUE;
            }

            break; // out of switch
        }

        default:
        {
            DebugPrintf(FormatString("Session thread, unexpected result from wait, result = %d\n") << WaitResult);
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

    return ExitCode;
}

static unsigned __stdcall threadBootstrap( void *parm )
{
    EmacsThreadCommon &thread = *(EmacsThreadCommon *)parm;

    return thread.run();
}
