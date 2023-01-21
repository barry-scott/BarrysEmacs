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
    std::wcout << "named pipe client" << std::endl;

    wchar_t *pipe_name = L"\\\\.\\pipe\\Barry's Emacs 8.2";

    char *cmd = "Command from client";

    char buf_result[128];
    DWORD buf_size( sizeof( buf_result ) -1 );

    DWORD rc = CallNamedPipeW(
            pipe_name,
            cmd,
            strlen(cmd),
            buf_result,
            buf_size,
            &buf_size,
            0
            );
    if( rc == 0 )
    {
        std::wcerr << "CallNamedPipeA failed: " << __getLastErrorMessage().c_str() << std::endl;
    }
    else
    {
        buf_result[buf_size] = 0;
        std::wcerr << "Reply size " << buf_size << " \"" << buf_result << "\"" << std::endl;
    }

    return 0;
}
