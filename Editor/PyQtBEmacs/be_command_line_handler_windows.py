#!/usr/bin/env python3
'''
 ====================================================================
 Copyright (c) 2003-2016 Barry A Scott.  All rights reserved.

 This software is licensed as described in the file LICENSE.txt,
 which you should have received as part of this distribution.

 ====================================================================

    be_command_line_handler_windows.py


'''
import os
import ctypes

class CommandLineHandlerWindows:
    def __init__( self, app ):
        self.app = app
        
    def processCommandLines( self ):
        self.app._debugApp( 'CommandLineHandlerWindows()' )

        PIPE_UNLIMITED_INSTANCES = 255
        PIPE_ACCESS_DUPLEX = 3
        FILE_FLAG_OVERLAPPED = 0x40000000

        PIPE_TYPE_MESSAGE = 4
        PIPE_READMODE_MESSAGE = 2
        PIPE_REJECT_REMOTE_CLIENTS = 8

        INFINITE = -1
        ERROR_PIPE_CONNECTED = 535
        WAIT_OBJECT_0 = 0

        class OVERLAPPED(ctypes.Structure):
            _fields_ =  [('status', ctypes.c_ulonglong)
                        ,('transfered', ctypes.c_ulonglong)
                        ,('offset', ctypes.c_ulonglong)
                        ,('hevent', ctypes.c_ulonglong)]

        # QQQ: seems like a hang over from an older design...
        self.__h_wait_stop = ctypes.windll.kernel32.CreateEventW( None, 0, 0, None )

        # We need to use overlapped IO for this, so we dont block when
        # waiting for a client to connect.  This is the only effective way
        # to handle either a client connection, or a service stop request.
        self.__overlapped = OVERLAPPED( 0, 0, 0, 0 )

        # And create an event to be used in the OVERLAPPED object.
        self.__overlapped.hevent = ctypes.windll.kernel32.CreateEventW( None, 0, 0, None )

        # We create our named pipe.
        pipe_name = "\\\\.\\pipe\\Barry's Emacs 8.2"

        h_pipe = ctypes.windll.kernel32.CreateNamedPipeW(
                        pipe_name,                      #  __in      LPCTSTR lpName,
                        PIPE_ACCESS_DUPLEX
                        | FILE_FLAG_OVERLAPPED,         #  __in      DWORD dwOpenMode,
                        PIPE_TYPE_MESSAGE
                        | PIPE_READMODE_MESSAGE
                        | PIPE_REJECT_REMOTE_CLIENTS,   #  __in      DWORD dwPipeMode,
                        PIPE_UNLIMITED_INSTANCES,       #  __in      DWORD nMaxInstances,
                        0,                              #  __in      DWORD nOutBufferSize,
                        0,                              #  __in      DWORD nInBufferSize,
                        100,                            #  __in      DWORD nDefaultTimeOut, (100ms)
                        None                            #  __in_opt  LPSECURITY_ATTRIBUTES lpSecurityAttributes
                        )
        if h_pipe is None:
            self.app.log_client_log( 'Failed to CreateNamedPipeW( %s ): %s' %
                                    (pipe_name, self.__getLastErrorMessage()) )

        # Loop accepting and processing connections
        while True:
            self.app._debugApp( 'CommandLineHandlerWindows loop top' )

            hr = ctypes.windll.kernel32.ConnectNamedPipe( h_pipe, ctypes.byref( self.__overlapped ) )
            if hr == ERROR_PIPE_CONNECTED:
                # Client is fast, and already connected - signal event
                ctypes.windll.kernel32.SetEvent( self.__overlapped.hevent )

            self.app._debugApp( 'CommandLineHandlerWindows connected to named pipe' )

            # Wait for either a connection, or a service stop request.
            wait_handles_t = ctypes.c_uint64 * 2
            wait_handles = wait_handles_t( self.__h_wait_stop, self.__overlapped.hevent )

            self.app._debugApp( 'CommandLineHandlerWindows WaitForMultipleObjects...' )
            rc = ctypes.windll.kernel32.WaitForMultipleObjects( 2, ctypes.byref( wait_handles ), 0, INFINITE )

            if rc == WAIT_OBJECT_0:
                self.app._debugApp( 'CommandLineHandlerWindows Stop event' )
                # Stop event
                break

            else:
                self.app._debugApp( 'CommandLineHandlerWindows data ready for read' )

                # Pipe event - read the data, and write it back.
                buf_size = ctypes.c_uint( 32768 )
                buf_client = ctypes.create_string_buffer( buf_size.value )
                hr = ctypes.windll.kernel32.ReadFile( h_pipe, buf_client, buf_size, ctypes.byref( buf_size ), None )
                client_command = buf_client.raw[:buf_size.value]

                self.app._debugApp( 'CommandLineHandlerWindows read command %r' % (client_command,) )

                reply = ctypes.create_string_buffer( 32 )
                if len( client_command ) > 0:
                    if client_command[0] == ord('P'):
                        reply.value = b'p%d' % (os.getpid(),)

                    elif client_command[0] == ord('C'):
                        new_argv = [b.decode('utf-8') for b in client_command[1:].split( b'\x00' )]
                        self.app.handleClientCommand( new_argv )
                        reply.value = b' '

                    else:
                        reply.value = b'R'+b'Unknown client command'

                    reply_size = ctypes.c_uint( len( reply.value ) )

                    hr = ctypes.windll.kernel32.WriteFile( h_pipe, reply, reply_size, ctypes.byref( reply_size ), None )

                # And disconnect from the client.
                ctypes.windll.kernel32.DisconnectNamedPipe( h_pipe )
                self.app._debugApp( 'CommandLineHandlerWindows Disconnected named pipe' )

    def __getLastErrorMessage( self ):
        import ctypes

        err = ctypes.windll.kernel32.GetLastError()

        FORMAT_MESSAGE_FROM_SYSTEM = 0x00001000
        FORMAT_MESSAGE_IGNORE_INSERTS = 0x00000200

        errmsg_size = ctypes.c_int( 256 )
        errmsg = ctypes.create_string_buffer( errmsg_size.value + 1 )

        rc = ctypes.windll.kernel32.FormatMessageA(
            FORMAT_MESSAGE_FROM_SYSTEM|FORMAT_MESSAGE_IGNORE_INSERTS, # __in      DWORD dwFlags,
            None,           # __in_opt  LPCVOID lpSource,
            err,            # __in      DWORD dwMessageId,
            0,              # __in      DWORD dwLanguageId,
            errmsg,         # __out     LPTSTR lpBuffer,
            errmsg_size,    # __in      DWORD nSize,
            None            # __in_opt  va_list *Arguments
            )
        if rc == 0:
            return 'error 0x%8.8x' % (err,)

        return errmsg.value

if __name__ == '__main__':
    class FakeApp:
        def __init__( self ):
            self.log = self

        def error( self, msg ):
            print( 'Error: %s' % (msg,) )

        def _debugApp( self, msg ):
            print( 'Debug: %s' % (msg,) )

        def handleClientCommand( self, cmd ):
            print( 'Cmd: %r' % (cmd,) )

    handler = CommandLineHandlerWindows( FakeApp() )
    handler.processCommandLines()
