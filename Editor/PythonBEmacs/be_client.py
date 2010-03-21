#!/usr/bin/python
#
# ====================================================================
# Copyright (c) 2010 Barry A Scott.  All rights reserved.
#
# This software is licensed as described in the file LICENSE.txt,
# which you should have received as part of this distribution.
#
# ====================================================================
#
#
#    be_client.py
#

import sys
import os
import types
import stat
import time

class ClientError(Exception):
    def __init__( self, text ):
        Exception.__init__( self, text )

def main( argv ):
    if sys.platform.startswith( 'win' ):
        client = ClientWindows()

    elif sys.platform == 'darwin':
        client = ClientMacOsX()

    else:
        client = ClientUnix()

    return client.main( argv )

class ClientBase:
    def __init__( self ):
        self.all_command_elements = []

        self.opt_debug = 'BEAMCS_CLIENT_DEBUG' in os.environ
        self.opt_name = None
        self.opt_wait = False
        self.opt_start_app = True

    def main( self, argv ):
        try:
            self.__parseArgsIntoCommandElements( argv )

            if not self.processCommand():
                self.startBemacsServer()

                while not self.processCommand():
                    time.sleep( 0.1 )

            self.bringTofront()

        except ClientError, e:
            print 'Error: %s' % (str(e),)

    def startBemacsServer( self ):
        raise NotImplementedError()

    def bringTofront( self ):
        raise NotImplementedError()

    def __parseArgsIntoCommandElements( self, _argv ):
        self.all_command_elements.append( os.getcwd() )
        # set the package name
        self.all_command_elements.append( 'emacs' )

        self.process_qualifers = True

        argv = iter( _argv )

        self.argv0 = argv.next()

        while True:
            try:
                arg = argv.next()

            except StopIteration:
                break

            try:
                if self.isQualifier( arg ):
                    if self.isNoMoreQualifiers( arg ):
                        self.process_qualifers = False

                    else:
                        name = self.getArgQualifierName( arg )

                        if name == 'name':
                            self.opt_name = self.getArgValue( arg, argv )

                        elif name == 'wait':
                            self.opt_wait = True
                            self.all_command_elements.append( arg )

                        elif name == 'package':
                            self.all_command_elements[1] = self.getArgValue( arg, argv )

                        elif name == 'nostart':
                            self.opt_start_app = False

                else:
                    self.all_command_elements.append( arg )

            except StopIteration:
                raise ClientError( '%s requires a value' % (arg,) )

    def processCommand( self ):
        raise NotImplementedError()

    def _getCommandString( self ):
        all_byte_elements = []
        for element in self.all_command_elements:
            if type(element) == types.UnicodeType:
                all_byte_elements.append( element.encode( 'utf-8' ) )

            else:
                all_byte_elements.append( element )

        return '\x00'.join( all_byte_elements )


    def getArgQualifierName( self, arg ):
        raise NotImplementedError()

    def getArgValue( self, arg, argv ):
        raise NotImplementedError()

class ClientPosix(ClientBase):
    def __init__( self ):
        ClientBase.__init__( self )

    def isQualifier( self, arg ):
        if not self.process_qualifers:
            return False

        return arg.startswith( '-' )

    def isNoMoreQualifiers( self, name ):
        return name == '--'

    def getArgQualifierName( self, arg ):
        if '=' in arg:
            arg, value = arg.split( '=', 1 )

        return arg[2:]

    def getArgValue( self, arg, argv ):
        if '=' in arg:
            arg, value = arg.split( '=', 1 )
            return value

        else:
            return argv.next()

    def processCommand( self ):
        import pwd

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
            fd_command = os.open( server_fifo, os.O_WRONLY|os.O_NONBLOCK )
        except OSError:
            return False

        fd_response = os.open( client_fifo, os.O_RDONLY|os.O_NONBLOCK )

        cmd = self._getCommandString()
        size = os.write( fd_command, cmd )
        if size != len(cmd):
            raise ClientError( 'write to command fifo failed' )

        os.close( fd_command )

        seen_ack = False

        try:
            while True:
                try:
                    response = os.read( fd_response, 16384 )
                except OSError:
                    response = ''

                if response == ' ':
                    seen_ack = True
                    if not self.opt_wait:
                        break

                elif seen_ack:
                    if response[0] == 'R':
                        break

                time.sleep( 0.1 )

            if len(response) > 1:
                print response[1:]

            return True

        except IOError:
            return False

    def __makeFifo( self, fifo_name ):
        if os.path.exists( fifo_name ):
            stats = os.stat( fifo_name )
            if not stat.S_ISFIFO( stats.st_mode ):
                print 'Error: %s is not a fifo' % (fifo_name,)

            elif stats.st_size == 0:
                return

            os.remove( fifo_name )

        os.mkfifo( fifo_name, stat.S_IRUSR|stat.S_IWUSR )


class ClientMacOsX(ClientPosix):
    def __init__( self ):
        ClientPosix.__init__( self )

    def startBemacsServer( self ):
        if self.opt_start_app:
            os.system( '/usr/bin/open -b org.barrys-emacs.bemacs-devel' )

    def bringTofront( self ):
        self.startBemacsServer()

class ClientUnix(ClientPosix):
    def __init__( self ):
        ClientPosix.__init__( self )

    def startBemacsServer( self ):
        print 'TBD start bemacs server'

    def startBemacsServer( self ):
        argv0 = sys.argv[0]

        if argv0.startswith( '/' ):
            app_dir = os.path.dirname( argv0 )

        elif '/' in argv0:
            app_dir = os.path.dirname( os.path.abspath( argv0 ) )

        else:
            for folder in [p.strip() for s in os.environ.get( 'PATH', '' ).split( ':' )]:
                app_path = os.path.abspath( os.path.join( folder, argv0 ) )
                if os.path.exists( app_path ):
                    app_dir = os.path.dirname( app_path )
                    break

        if app_dir == '':
            app_dir = os.getcwd()

        server_path = os.path.join( app_dir, 'bemacs_server' )

        if os.fork() == 0:
            os.execl( server_path, server_path )

    def bringTofront( self ):
        pass


class ClientWindows(ClientBase):
    def __init__( self ):
        ClientBase.__init__( self )

        self.editor_pid = 0

    def isQualifier( self, arg ):
        return arg.startswith( '/' )

    def isNoMoreQualifiers( self, name ):
        return False

    def getArgQualifierName( self, arg ):
        if ':' in arg:
            arg, value = arg.split( ':', 1 )

        return arg

    def getArgValue( self, arg, argv ):
        if ':' in arg:
            arg, value = arg.split( ':', 1 )
            return value

        else:
            return argv.next()

    def processCommand( self ):
        import ctypes

        reply = self.__sendCommand( 'p' )
        if reply is None:
            return False

        pid = int( reply[1:] )

        rc = ctypes.windll.user32.AllowSetForegroundWindow( pid )

        cmd = self._getCommandString()

        reply = self.__sendCommand( 'c' + cmd )
        return reply is not None

    def __sendCommand( self, cmd ):
        import ctypes

        pipe_name = "\\\\.\\pipe\\Barry's Emacs"

        buf_size = ctypes.c_int( 128 )
        buf_result = ctypes.create_string_buffer( buf_size.value )

        rc = ctypes.windll.kernel32.CallNamedPipeA(
                pipe_name,
                cmd,
                len(cmd),
                buf_result,
                buf_size,
                ctypes.byref( buf_size ),
                0
                )
        if rc == 0:
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
            print 'Error:',rc, err,repr(errmsg.value)
            return None

        else:
            return buf_result.raw[:buf_size.value]

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



    def startBemacsServer( self ):
        argv0 = sys.argv[0]

        if argv0.lower().startswith( 'c:\\' ):
            app_dir = os.path.dirname( argv0 )

        elif '\\' in argv0:
            app_dir = os.path.dirname( os.path.abspath( argv0 ) )

        else:
            app_dir = ''
            for folder in [s.strip() for s in os.environ.get( 'PATH', '' ).split( ';' )]:
                app_path = os.path.abspath( os.path.join( folder, argv0 ) )
                if os.path.exists( app_path ):
                    app_dir = os.path.dirname( app_path )
                    break

        if app_dir == '':
            app_dir = os.getcwd()

        server_path = os.path.join( app_dir, 'bemacs_server' )

        print 'Debug: server_path',server_path

    def bringTofront( self ):
        pass

if __name__ == '__main__':
    sys.exit( main( sys.argv ) )
