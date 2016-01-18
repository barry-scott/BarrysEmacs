#!/usr/bin/env python3
'''
 ====================================================================
 Copyright (c) 2003-2016 Barry A Scott.  All rights reserved.

 This software is licensed as described in the file LICENSE.txt,
 which you should have received as part of this distribution.

 ====================================================================

    be_command_line_handler_posix.py


'''
import pwd
import select

class CommandLineHandlerPosix:
    def __init__( self, app ):
        self.app = app
        
    def processCommandLines( self ):
        self.app._debugApp( 'CommandLineHandlerPosix' )

        fifo_name = os.environ.get( 'BEMACS_FIFO', '.bemacs8/.emacs_command' )

        if fifo_name.startswith( '/' ):
            server_fifo = fifo_name

        else:
            e = pwd.getpwuid( os.geteuid() )

            server_fifo = '/tmp/%s/%s' % (e.pw_name, fifo_name)

        client_fifo = '%s_response' % (server_fifo,)

        if self.opt_name is not None:
            server_fifo += '_' + self.opt_name
            client_fifo += '_' + self.opt_name

        fifo_dir = os.path.dirname( server_fifo )

        if not os.path.exists( fifo_dir ):
            os.makedirs( fifo_dir )

        self.__makeFifo( server_fifo )
        self.__makeFifo( client_fifo )

        try:
            emacs_server_read_fd = os.open( server_fifo, os.O_RDONLY|os.O_NONBLOCK );

        except OSError:
            self.log.error( 'Failed to open %s for read' % (server_fifo,) )
            return

        try:
            emacs_server_write_fd = os.open( server_fifo, os.O_WRONLY|os.O_NONBLOCK );
        except OSError:
            self.log.error( 'Failed to open %s for write' % (server_fifo,) )
            return

        self.app._debugApp( 'CommandLineHandlerPosix before read loop' )
        while True:
            r, w, x = select.select( [emacs_server_read_fd], [], [], 1.0 )
            reply = b' '
            if emacs_server_read_fd in r:
                reply = b'R' b'Unknown client command'

                client_command = os.read( emacs_server_read_fd, 16384 )
                self.app._debugApp( 'CommandLineHandlerPosix command %r' % (client_command,) )
                if len( client_command ) > 0:
                    if client_command[0] == ord('C'):
                        new_argv = [b.decode('utf-8') for b in client_command[1:].split( b'\x00' )]
                        self.app.handleClientCommand( new_argv )
                        reply = b' '

                emacs_client_write_fd = os.open( client_fifo, os.O_WRONLY|os.O_NONBLOCK );
                if emacs_client_write_fd < 0:
                    return

                self.app._debugApp( 'CommandLineHandlerPosix response %r' % (reply,) )
                os.write( emacs_client_write_fd, reply )
                os.close( emacs_client_write_fd )

    def __makeFifo( self, fifo_name ):
        if os.path.exists( fifo_name ):
            stats = os.stat( fifo_name )
            if not stat.S_ISFIFO( stats.st_mode ):
                self.log.error( '%s is not a fifo' % (fifo_name,) )

            elif stats.st_size == 0:
                return

            os.remove( fifo_name )

        os.mkfifo( fifo_name, stat.S_IRUSR|stat.S_IWUSR )


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

    handler = CommandLineHandlerPosix( FakeApp() )
    handler.processCommandLines()
