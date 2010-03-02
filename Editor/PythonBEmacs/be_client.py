'''
 ====================================================================
 Copyright (c) 2010 Barry A Scott.  All rights reserved.

 This software is licensed as described in the file LICENSE.txt,
 which you should have received as part of this distribution.

 ====================================================================


    be_client.py


    Based on code from pysvn WorkBench

'''
import sys
import os
import types
import pwd
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
        self.opt_wait = False

    def main( self, argv ):
        try:
            self.parseArgsIntoCommandElements( argv )

            if not self.processCommand():
                self.startBemacsServer()

                self.processCommand()

        except ClientError, e:
            print 'Error: %s' % (str(e),)

    def parseArgsIntoCommandElements( self, _argv ):
        self.all_command_elements.append( os.getcwd() )

        argv = iter( _argv )

        self.argv0 = argv.next()

        while True:
            try:
                arg = argv.next()

            except StopIteration:
                break

            name = self.__getName( arg )

            try:
                if name == '--name':
                    self.opt_name = self.__getValue( arg, argv )

                elif arg == '--wait':
                    self.opt_wait = True
                    self.all_command_elements.append( arg )

                else:
                    self.all_command_elements.append( arg )

            except StopIteration:
                raise ClientError( '%s requires a value' % (arg,) )

    def processCommand( self ):
        fifo_name = os.environ.get( 'BEMACS_FIFO', '.bemacs/.emacs_command' )

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

        self.makeFifo( server_fifo )
        self.makeFifo( client_fifo )

        fd_command = os.open( server_fifo, os.O_WRONLY|os.O_NONBLOCK )
        if fd_command < 0:
            return False

        fd_response = os.open( client_fifo, os.O_RDONLY|os.O_NONBLOCK )
        if fd_response < 0:
            return False

        cmd = self.__getCommandString()
        size = os.write( fd_command, cmd )
        if size != len(cmd):
            raise ClientError( 'write to command fifo failed' )

        os.close( fd_command )

        seen_ack = False

        try:
            while True:
                response = os.read( fd_response, 16384 )
                if response == ' ':
                    seen_ack = True
                    if not self.opt_wait:
                        break

                elif seen_ack:
                    if response[0] == 'R':
                        break

                time.sleep( 1.0 )

            if len(response) > 1:
                print response[1:]

            return True

        except IOError:
            return False

    def __getCommandString( self ):
        all_byte_elements = []
        for element in self.all_command_elements:
            if type(element) == types.UnicodeType:
                all_byte_elements.append( element.encode( 'utf-8' ) )

            else:
                all_byte_elements.append( element )

        return '\x00'.join( all_byte_elements )

    def makeFifo( self, fifo_name ):
        if os.path.exists( fifo_name ):
            stats = os.stat( fifo_name )
            if not stat.S_ISFIFO( stats.st_mode ):
                print 'Error: %s is not a fifo' % (fifo_name,)

            elif stats.st_size == 0:
                return

            os.remove( fifo_name )

        os.mkfifo( fifo_name, stat.S_IRUSR|stat.S_IWUSR )

    def __getName( self, arg ):
        if '=' in arg:
            arg, value = arg.split( '=', 1 )

        return arg

    def __getValue( self, arg, argv ):
        if '=' in arg:
            arg, value = arg.split( '=', 1 )
            return value

        else:
            return argv.next()


class ClientWindows(ClientBase):
    def __init__( self ):
        ClientBase.__init__( self )

class ClientMacOsX(ClientBase):
    def __init__( self ):
        ClientBase.__init__( self )

class ClientUnix(ClientBase):
    def __init__( self ):
        ClientBase.__init__( self )




if __name__ == '__main__':
    sys.exit( main( sys.argv ) )
