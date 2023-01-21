#!/usr/bin/env python3
#
# ====================================================================
# Copyright (c) 2010-2016 Barry A Scott.  All rights reserved.
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
import math
import ctypes
import tempfile
import subprocess

_debug_client = False
_debug_log = None

PY3 = sys.version_info.major > 2
if PY3:
    def Next( it ):
        return it.__next__()

else:
    def Next( it ):
        return it.next()

def debugClient( msg ):
    if _debug_client:
        global _debug_log
        if _debug_log is None:
            if 'BEMACS_CLIENT_LOG' in os.environ:
                _debug_log = open( os.environ[ 'BEMACS_CLIENT_LOG' ], 'w' )
            else:
                _debug_log = sys.stderr

        now = time.time()
        frac = math.modf( now )[0]
        local_now = time.localtime( now )
        prefix = '%s.%03d' % (time.strftime( '%Y-%m-%d %H:%M:%S', local_now ), int( frac*1000 ))
        _debug_log.write( '%s CLIENT %s\n' % (prefix, msg) )

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

        global _debug_client
        _debug_client = 'BEMACS_CLIENT_DEBUG' in os.environ

        self.is_cli_supported = True

        self.opt_name = None
        self.opt_cli = False
        self.opt_wait = False
        self.opt_start_app = True

    def main( self, argv ):
        try:
            self._parseArgsIntoCommandElements( argv )

            if self.opt_cli:
                self.startBemacsCli()

            else:
                if not self.processCommand():
                    self.startBemacsServer()

                    while not self.processCommand():
                        time.sleep( 0.1 )

                self.bringTofront()

                if self.opt_wait:
                    self.waitCommand()

        except ClientError as e:
            print( 'Error: %s' % (str(e),) )

    def startBemacsCli( self ):
        raise NotImplementedError()

    def startBemacsServer( self ):
        raise NotImplementedError()

    def bringTofront( self ):
        raise NotImplementedError()

    def _parseArgsIntoCommandElements( self, _argv ):
        self.all_command_elements.append( os.getcwd() )
        # set the package name
        self.all_command_elements.append( 'emacs' )

        self.process_qualifers = True

        argv = iter( _argv )

        self.argv0 = Next( argv )

        while True:
            try:
                arg = Next( argv )

            except StopIteration:
                break

            debugClient( 'ARG: %r' % (arg,) )

            try:
                if self.isQualifier( arg ):
                    debugClient( 'isQualifier( %r ) TRUE' % (arg,) )

                    if self.isNoMoreQualifiers( arg ):
                        self.process_qualifers = False

                    else:
                        name = self.getArgQualifierName( arg )
                        debugClient( 'getArgQualifierName( %r ) => %r' % (arg, name) )

                        if self.is_cli_supported and name == 'cli':
                            self.opt_cli = True

                        elif name == 'name':
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

                else:
                    self.all_command_elements.append( arg )

            except StopIteration:
                raise ClientError( '%s requires a value' % (arg,) )

    def processCommand( self ):
        raise NotImplementedError()

    def waitCommand( self ):
        pass

    def _encodeCommandString( self ):
        all_byte_elements = []

        for element in self.all_command_elements:
            if type(element) == str:
                all_byte_elements.append( element.encode( 'utf-8' ) )

            else:
                all_byte_elements.append( element )

        return b'\x00'.join( all_byte_elements )


    def getArgQualifierName( self, arg ):
        raise NotImplementedError()

    def getArgValue( self, arg, argv ):
        raise NotImplementedError()

class ClientPosix(ClientBase):
    def __init__( self ):
        ClientBase.__init__( self )
        self.is_cli_supported = True

    def isQualifier( self, arg ):
        if not self.process_qualifers:
            return False

        return arg.startswith( '-' )

    def isNoMoreQualifiers( self, name ):
        return name == '--'

    def getArgQualifierName( self, arg ):
        if '=' in arg:
            arg, value = arg.split( '=', 1 )

        if arg.startswith( '--' ):
            return arg[2:]

        else:
            return arg[1:]

    def getArgValue( self, arg, argv ):
        if '=' in arg:
            arg, value = arg.split( '=', 1 )
            return value

        else:
            return Next( argv )

    def processCommand( self ):
        completed, reply = self._sendCommand( b'C'+self._encodeCommandString() )
        if len(reply) > 1:
            print( reply[1:] )

        return completed

    def waitCommand( self ):
        while True:
            debugClient( 'Poll for Wait' )
            completed, reply = self._sendCommand( b'W' )
            debugClient( '-> %r, %r' % (completed, reply) )
            if completed and reply[0] == 'w':
                if len(reply) > 1:
                    print( reply[1:] )

                return True

            time.sleep( 0.1 )

    def _sendCommand( self, cmd ):
        debugClient( '_sendCommand( %r )' % (cmd,) )
        import pwd

        fifo_name = os.environ.get( 'BEMACS_FIFO', '.bemacs8/.emacs_command' )

        if fifo_name.startswith( '/' ):
            server_fifo = fifo_name

        else:
            e = pwd.getpwuid( os.geteuid() )

            server_fifo = os.path.join( tempfile.gettempdir(), e.pw_name, fifo_name )

        client_fifo = '%s_response' % (server_fifo,)

        if self.opt_name is not None:
            server_fifo += '_' + self.opt_name
            client_fifo += '_' + self.opt_name

        debugClient( 'server_fifo %r' % (server_fifo,) )
        debugClient( 'client_fifo %r' % (client_fifo,) )

        fifo_dir = os.path.dirname( server_fifo )

        if not os.path.exists( fifo_dir ):
            os.makedirs( fifo_dir )

        self._makeFifo( server_fifo )
        self._makeFifo( client_fifo )

        try:
            fd_command = os.open( server_fifo, os.O_WRONLY|os.O_NONBLOCK )

        except OSError:
            debugClient( 'failed to open server_fifo' )
            return False, ''

        fd_response = os.open( client_fifo, os.O_RDONLY|os.O_NONBLOCK )

        debugClient( 'processCommand command %r' % (cmd,) )

        size = os.write( fd_command, cmd )
        if size != len(cmd):
            raise ClientError( 'write to command fifo failed' )

        os.close( fd_command )

        # first send off the command
        try:
            while True:
                try:
                    response = os.read( fd_response, 16384 )

                except OSError:
                    response = b''

                debugClient( 'processCommand response %r' % (response,) )

                if len(response) > 0:
                    break

                time.sleep( 0.1 )


            os.close( fd_response )
            return True, response.decode( 'utf-8' )

        except IOError:
            os.close( fd_response )
            return False, ''

    def _makeFifo( self, fifo_name ):
        if os.path.exists( fifo_name ):
            stats = os.stat( fifo_name )
            if not stat.S_ISFIFO( stats.st_mode ):
                print( 'Error: %s is not a fifo' % (fifo_name,) )

            elif stats.st_size == 0:
                return

            os.remove( fifo_name )

        os.mkfifo( fifo_name, stat.S_IRUSR|stat.S_IWUSR )

    def _findBemacsExecutable( self, name ):
        argv0 = sys.argv[0]

        app_dir = ''

        if argv0.startswith( '/' ):
            app_dir = os.path.dirname( argv0 )

        elif '/' in argv0:
            app_dir = os.path.dirname( os.path.abspath( argv0 ) )

        else:
            for folder in [p.strip() for p in os.environ.get( 'PATH', '' ).split( ':' )]:
                app_path = os.path.abspath( os.path.join( folder, argv0 ) )
                if os.path.exists( app_path ):
                    app_dir = os.path.dirname( app_path )
                    break

        if app_dir == '':
            app_dir = os.getcwd()

        return os.path.join( app_dir, name )

class ClientMacOsX(ClientPosix):
    def __init__( self ):
        ClientPosix.__init__( self )

    def startBemacsCli( self ):
        debugClient( 'startBemacsCli' )
        p = subprocess.run(
            ['osascript', '-e'
            ,'tell application "Finder" to POSIX path of '
                '(get application file id "org.barrys-emacs.bemacs" as alias)'],
            stdout=subprocess.PIPE )

        app_path = p.stdout.strip().decode('utf-8')

        bemacs_cli =  os.path.join(app_path, 'Contents/Resources/bin/bemacs-cli' )
        debugClient( 'bemacs_cli %r' % (bemacs_cli,) )

        args = [bemacs_cli]
        args.append('-package=%s' % (self.all_command_elements[1],))
        args.extend(self.all_command_elements[2:])

        debugClient( 'args %r' % (args,) )

        os.execv( bemacs_cli, args )

    def startBemacsServer( self ):
        debugClient( 'startBemacsServer' )

        if self.opt_start_app:
            os.system( '/usr/bin/open -b org.barrys-emacs.bemacs-devel' )

    def bringTofront( self ):
        debugClient( 'bringTofront' )
        self.startBemacsServer()

class ClientUnix(ClientPosix):
    def __init__( self ):
        ClientPosix.__init__( self )
        self.is_cli_supported = True

        # can only use the GUI bemacs_server is there is a display
        if 'DISPLAY' not in os.environ and 'WAYLAND_DISPLAY' not in os.environ:
            self.opt_cli = True

    def startBemacsCli( self ):
        debugClient( 'startBemacsCli' )

        bemacs_cli = self._findBemacsExecutable( 'bemacs-cli' )
        debugClient( 'bemacs_cli %r' % (bemacs_cli,) )

        args = [bemacs_cli]
        args.append('-package=%s' % (self.all_command_elements[1],))
        args.extend(self.all_command_elements[2:])

        debugClient( 'args %r' % (args,) )

        os.execv( bemacs_cli, args )

    def startBemacsServer( self ):
        debugClient( 'startBemacsServer' )

        server_path = self._findBemacsExecutable( 'bemacs_server' )
        debugClient( 'server_path %r' % (server_path,) )

        args = [server_path]
        if self.opt_name is not None:
            args.append( '--name=%s' % (self.opt_name,) )

        if os.fork() == 0:
            os.execv( server_path, args )

    def bringTofront( self ):
        pass


class ClientWindows(ClientBase):
    def __init__( self ):
        ClientBase.__init__( self )
        self.is_cli_supported = False

        self.editor_pid = 0

    def isQualifier( self, arg ):
        return arg.startswith( '/' ) or arg.startswith( '-' )

    def isNoMoreQualifiers( self, name ):
        return False

    def getArgQualifierName( self, arg ):
        if ':' in arg:
            arg, value = arg.split( ':', 1 )

        elif '=' in arg:
            arg, value = arg.split( '=', 1 )

        return arg[1:]

    def getArgValue( self, arg, argv ):
        if ':' in arg:
            arg, value = arg.split( ':', 1 )
            return value

        elif '=' in arg:
            arg, value = arg.split( '=', 1 )
            return value

        else:
            return Next( argv )

    def processCommand( self ):
        reply = self._sendCommand( b'P' )
        if reply is None:
            return False

        pid = int( reply[1:] )

        rc = ctypes.windll.user32.AllowSetForegroundWindow( pid )

        cmd = self._encodeCommandString()

        reply = self._sendCommand( b'C' + cmd )
        return reply is not None

    def _sendCommand( self, cmd ):
        debugClient( '_sendCommand cmd   %r' % (cmd,) )

        pipe_name = "\\\\.\\pipe\\Barry's Emacs 8.2"

        buf_size = ctypes.c_int( 128 )
        buf_result = ctypes.create_string_buffer( buf_size.value )

        rc = ctypes.windll.kernel32.CallNamedPipeW(
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
            if err == 2:
                return None

            errmsg  = self._getErrorMessage( err )
            print( 'Error: CallNamedPipeA rc=%d err=%d errmsg=%r' % (rc, err, errmsg) )
            return None

        else:
            reply = buf_result.raw[:buf_size.value]
            debugClient( '_sendCommand reply %r' % (reply,) )
            return reply

    def _getErrorMessage( self, err ):
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
        debugClient( 'startBemacsServer()' )

        if 'BEMACS_CLIENT_SERVER' in os.environ:
            server_path = os.environ['BEMACS_CLIENT_SERVER']

        else:
            argv0 = sys.argv[0]

            if '\\' in argv0:
                app_dir = os.path.dirname( os.path.abspath( argv0 ) )

            else:
                app_dir = ''
                for folder in [s.strip() for s in ['.']+os.environ.get( 'PATH', '' ).split( ';' )]:
                    app_path = os.path.abspath( os.path.join( folder, argv0 ) )
                    if os.path.exists( app_path ):
                        app_dir = os.path.dirname( app_path )
                        break

            if app_dir == '':
                app_dir = os.getcwd()

            server_path = os.path.join( app_dir, 'bemacs_server.exe' )

        if not os.path.exists( server_path ):
            raise ClientError( 'Expected to find BEmacs Server in %s' % (server_path,) )

        debugClient( 'server_path %s' % (server_path,) )

        cmd_line = ctypes.create_string_buffer( 128 )
        cmd_line.value = b''

        class STARTUPINFO(ctypes.Structure):
            _fields_ =  [('cb',                 ctypes.c_uint)
                        ,('lpReserved',         ctypes.c_wchar_p)
                        ,('lpDesktop',          ctypes.c_wchar_p)
                        ,('lpTitle',            ctypes.c_wchar_p)
                        ,('dwX',                ctypes.c_uint)
                        ,('dwY',                ctypes.c_uint)
                        ,('dwXSize',            ctypes.c_uint)
                        ,('dwYSize',            ctypes.c_uint)
                        ,('dwXCountChars',      ctypes.c_uint)
                        ,('dwYCountChars',      ctypes.c_uint)
                        ,('dwFillAttribute',    ctypes.c_uint)
                        ,('dwFlags',            ctypes.c_uint)
                        ,('wShowWindow',        ctypes.c_ushort)
                        ,('cbReserved2',        ctypes.c_ushort)
                        ,('lpReserved2',        ctypes.c_void_p)
                        ,('hStdInput',          ctypes.c_void_p)
                        ,('hStdOutput',         ctypes.c_void_p)
                        ,('hStdError',          ctypes.c_void_p)]


        class PROCESS_INFORMATION(ctypes.Structure):
            _fields_ =  [('hProcess',           ctypes.c_void_p)
                        ,('hThread',            ctypes.c_void_p)
                        ,('dwProcessId',        ctypes.c_uint)
                        ,('dwThreadId',         ctypes.c_uint)]


        s_info = STARTUPINFO()
        s_info.cb = ctypes.sizeof( s_info )

        p_info = PROCESS_INFORMATION( None, None, 0, 0 )

        rc = ctypes.windll.kernel32.CreateProcessW(
                        server_path,                # LPCTSTR lpApplicationName,
                        ctypes.byref( cmd_line ),   # LPTSTR lpCommandLine,
                        None,                       # LPSECURITY_ATTRIBUTES lpProcessAttributes,
                        None,                       # LPSECURITY_ATTRIBUTES lpThreadAttributes,
                        False,                      # BOOL bInheritHandles,
                        0,                          # DWORD dwCreationFlags,
                        None,                       # LPVOID lpEnvironment,
                        None,                       # LPCTSTR lpCurrentDirectory,
                        ctypes.byref( s_info ),     # LPSTARTUPINFO lpStartupInfo,
                        ctypes.byref( p_info )      # LPPROCESS_INFORMATION lpProcessInformation
                        )

    def bringTofront( self ):
        pass


def run():
    sys.exit( main( sys.argv ) )

if __name__ == '__main__':
    run()
