//
// cli-app.cpp : Defines the entry point for the console application.
//

#include <WinSDKVer.h>
#define _WIN32_WINNT 0x601      // Windows 7 - read SDKDDKVer for defs
#include <SDKDDKVer.h>

#define WIN32_LEAN_AND_MEAN     // Exclude rarely-used stuff from Windows headers
#include <Windows.h>

#include <cstdlib>
#include <cstdio>
#include <iostream>
#include <tchar.h>
#include <Strsafe.h>
#include <exception>
#include <sstream>


std::string __getLastErrorMessage()
{
    DWORD err = GetLastError();

    char errmsg[ 256 ];
    DWORD errmsg_size( sizeof( errmsg ) );

    DWORD rc = FormatMessageA(
        FORMAT_MESSAGE_FROM_SYSTEM|FORMAT_MESSAGE_IGNORE_INSERTS, // __in      DWORD dwFlags,
        nullptr,           // __in_opt  LPCVOID lpSource,
        err,            // __in      DWORD dwMessageId,
        0,              // __in      DWORD dwLanguageId,
        errmsg,         // __out     LPTSTR lpBuffer,
        errmsg_size,    // __in      DWORD nSize,
        nullptr            // __in_opt  va_list *Arguments
        );
    if( rc == 0 )
    {
        return std::string( "Unknown error" );
    }

    return std::string( errmsg, errmsg_size );
}

int wmain( int argc, wchar_t **argv, wchar_t **envp )
{
    std::wcout << "named pipe server" << std::endl;

    HANDLE h_wait_stop = CreateEvent( nullptr, 0, 0, nullptr );

    // We need to use overlapped IO for this, so we dont block when
    // waiting for a client to connect.  This is the only effective way
    // to handle either a client connection, or a service stop request.
    OVERLAPPED overlapped;
    overlapped.Internal = 0;
    overlapped.InternalHigh = 0;
    overlapped.Pointer = 0;

    std::wcout << "sizeof( overlapperd ) " << sizeof( overlapped ) << std::endl;
    std::wcout << "sizeof( overlapperd.Internal ) " << sizeof( overlapped.Internal ) << std::endl;
    std::wcout << "sizeof( overlapperd.InternalHigh ) " << sizeof( overlapped.InternalHigh ) << std::endl;
    std::wcout << "sizeof( overlapperd.Pointer ) " << sizeof( overlapped.Pointer ) << std::endl;
    std::wcout << "sizeof( overlapperd.hEvent ) " << sizeof( overlapped.hEvent ) << std::endl;

    // And create an event to be used in the OVERLAPPED object.
    overlapped.hEvent = CreateEventW( nullptr, 0, 0, nullptr );

    // We create our named pipe.
    char *pipe_name = "\\\\.\\pipe\\Barry's Emacs 8.2";

    HANDLE h_pipe = CreateNamedPipeA(
                    pipe_name,                      //  __in      LPCTSTR lpName,
                    PIPE_ACCESS_DUPLEX
                    | FILE_FLAG_OVERLAPPED,         //  __in      DWORD dwOpenMode,
                    PIPE_TYPE_MESSAGE
                    | PIPE_READMODE_MESSAGE
                    | PIPE_REJECT_REMOTE_CLIENTS,   //  __in      DWORD dwPipeMode,
                    PIPE_UNLIMITED_INSTANCES,       //  __in      DWORD nMaxInstances,
                    0,                              //  __in      DWORD nOutBufferSize,
                    0,                              //  __in      DWORD nInBufferSize,
                    100,                            //  __in      DWORD nDefaultTimeOut, (100ms)
                    nullptr                            //  __in_opt  LPSECURITY_ATTRIBUTES lpSecurityAttributes
                    );
    if( h_pipe == 0 )
    {
        std::wcerr << "Failed to CreateNamedPipeA: " << __getLastErrorMessage().c_str() << std::endl;
        return 1;
    }

    // Loop accepting and processing connections
    for(;;)
    {
        std::wcout << "__windowsCommandLineHandler loop top" << std::endl;

        DWORD hr = ConnectNamedPipe( h_pipe, &overlapped );
        if( hr == ERROR_PIPE_CONNECTED )
        {
            // Client is fast, and already connected - signal event
            SetEvent( overlapped.hEvent );
        }

        std::wcout << "__windowsCommandLineHandler connected to named pipe" << std::endl;

        // Wait for either a connection, or a service stop request.
        HANDLE wait_handles[2];
        wait_handles[0] = h_wait_stop;
        wait_handles[1] = overlapped.hEvent;

        std::wcout << "__windowsCommandLineHandler WaitForMultipleObjects..." << std::endl;
        DWORD rc = WaitForMultipleObjects( 2, wait_handles, 0, INFINITE );

        if( rc == WAIT_OBJECT_0 )
        {
            std::wcout << "__windowsCommandLineHandler Stop event" << std::endl;
            // Stop event
            break;
        }
        else
        {
            std::wcout << "__windowsCommandLineHandler data ready for read" << std::endl;

            // Pipe event - read the data, and write it back.
            char buf_client[32768];
            DWORD buf_size( sizeof( buf_client )-1 );
            hr = ReadFile( h_pipe, buf_client, buf_size, &buf_size, nullptr );
            if( !hr )
            {
                std::wcout << "ReadFile failed: " << __getLastErrorMessage().c_str() << std::endl;
            }
            else
            {
                buf_client[ buf_size ] = 0;
                std::wcout << "__windowsCommandLineHandler read command size " << buf_size << " \"" << buf_client << "\"" << std::endl;

                if( buf_size > 0 )
                {
                    char *reply = "Reply";
                    DWORD reply_size = strlen( reply );

                    hr = WriteFile( h_pipe, reply, reply_size, &reply_size, nullptr );
                    if( !hr )
                    {
                        std::wcout << "WriteFile failed: " << __getLastErrorMessage().c_str() << std::endl;
                    }
                }
            }
            // And disconnect from the client.
            DisconnectNamedPipe( h_pipe );
            std::wcout << "__windowsCommandLineHandler Disconnected named pipe" << std::endl;
        }
    }
    return 0;
}
