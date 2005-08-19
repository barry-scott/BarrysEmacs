/****************************** Module Header ******************************\
* Module Name: session.h
*
* Copyright (c) 1991, Microsoft Corporation
*
* Remote shell session module header file
*
* History:
* 06-28-92 Davidc       Created.
\***************************************************************************/

inline void MyCloseHandle(HANDLE Handle, const char *handle_name)
	{
        if (CloseHandle(Handle) == FALSE)
		{
		_dbg_msg( FormatString("Close Handle failed for <%s>, error = %d\n") << handle_name << GetLastError() );
		emacs_assert(FALSE);
		}
	}

const int SHELL_BUFFER_SIZE = 1000;

//
// Define session thread notification values
//

typedef enum {
    ConnectError,
    DisconnectError,
    ClientDisconnected,
    ShellEnded
} SESSION_DISCONNECT_CODE, *PSESSION_NOTIFICATION_CODE;


//
// Function protoypes
//

HANDLE
CreateSession(
    VOID
    );

VOID
DeleteSession(
    HANDLE  SessionHandle
    );

HANDLE
ConnectSession(
    HANDLE  SessionHandle,
    EmacsProcess *
    );

SESSION_DISCONNECT_CODE
DisconnectSession(
    HANDLE  SessionHandle
    );

