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

#define MyCloseHandle(Handle, handle_name) \
        if (CloseHandle(Handle) == FALSE) { \
            _dbg_msg(u_str("Close Handle failed for <%s>, error = %d\n"), handle_name, GetLastError()); \
            assert(FALSE); \
        }

#define SHELL_BUFFER_SIZE   1000

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
    struct process_blk *
    );

SESSION_DISCONNECT_CODE
DisconnectSession(
    HANDLE  SessionHandle
    );

