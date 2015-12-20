'''
 ====================================================================
 Copyright (c) 2003-2015 Barry A Scott.  All rights reserved.

 This software is licensed as described in the file LICENSE.txt,
 which you should have received as part of this distribution.

 ====================================================================

    be_app.py

    Based on code from pysvn WorkBench

'''
import sys
import os
import stat
import types
import logging
import tempfile
import threading
import inspect
import gettext
import queue

from PyQt5 import QtWidgets
from PyQt5 import QtGui
from PyQt5 import QtCore

import be_main_window
import be_platform_specific
import be_preferences
import be_exceptions
import be_debug


class BemacsApp(QtWidgets.QApplication, be_debug.EmacsDebugMixin):
    MarshallToGuiThreadSignal = QtCore.pyqtSignal( name='MarshallToGuiThread' )

    def __init__( self, args ):
        be_debug.EmacsDebugMixin.__init__( self )

        self.may_quit = False

        self.args = args
        self.opt_name = None

        self.startup_dir = os.getcwd()

        be_platform_specific.setupPlatform( args[0] )

        # on the Mac the app's cwd is the resource folder
        if sys.platform == 'darwin':
            if 'PWD' in os.environ:
                os.chdir( os.environ['PWD'] )
            else:
                os.chdir( os.environ['HOME'] )

        self.__debug_noredirect = False
        self.__debug = True
        self.__mock_editor = False
        self.__log_stdout = False

        self.__callback_queue = queue.Queue()

        while len(args) > 1:
            arg = args[ 1 ]
            if arg.startswith( '-psn_' ):
                del args[ 1 ]

            elif arg.startswith( '--name=' ):
                self.opt_name = arg[len('--name='):]
                del args[ 1 ]

            elif arg == '--noredirect':
                self.__debug_noredirect = True
                del args[ 1 ]

            elif arg == '--log-stdout':
                self.__log_stdout = True
                del args[ 1 ]

            elif arg == '--debug' and len(args) > 2:
                self.__debug = True
                be_debug.setDebug( args[2] )
                del args[ 1 ]
                del args[ 1 ]

            elif arg == '--mock-editor':
                self.__mock_editor = True
                del args[ 1 ]

            elif arg == '--start-dir' and len(args) > 2:
                os.chdir( args[2]  )
                del args[1]
                del args[1]

            elif arg == '--':
                break

            else:
                break

        self.__call_gui_result = None
        self.__call_gui_result_event = threading.Event()

        self.editor = None

        self.main_thread = threading.currentThread()
        if self.__mock_editor:
            self.editor_thread = None
            self.command_line_thread = None
            self.editor = FakeEditor( self )

        else:
            self.editor_thread = threading.Thread( name='Editor', target=self.__runEditor )
            self.command_line_thread = threading.Thread( name='CommandLine', target=self.__runCommandLineHandler )

        self.progress_format = None
        self.progress_values = {}

        locale_path = be_platform_specific.getLocalePath( self )
        self.translation = gettext.translation(
                'bemacs',
                locale_path,
                # language defaults
                fallback=True )

        import builtins
        # T_( 'non plural' )
        builtins.__dict__['T_'] = self.translation.gettext
        # S_( 'singular', 'plural', n )
        builtins.__dict__['S_'] = self.translation.ngettext
        # U_( 'static string' )
        builtins.__dict__['U_'] = lambda s: s

        # Debug settings
        self.__last_client_error = []

        self.setupLogging()

        self.log.info( 'startup_dir %s' % (self.startup_dir,) )
        self.log.info( 'locale_path %s' % (locale_path,) )
        self.log.info( 'find %r' % (gettext.find( 'bemacs', locale_path ),) )
        self.log.info( 'info %r' % (self.translation.info(),) )
        self.log.info( T_("Barry's Emacs") )

        self.log.info( 'emacs_user %s' % (be_platform_specific.getUserDir(),) )
        self.log.info( 'emacs_library %s' % (be_platform_specific.getLibraryDir(),) )

        self.prefs = be_preferences.Preferences(
                self,
                be_platform_specific.getPreferencesFilename() )

        self.lock_ui = 0
        self.need_activate_app_action = False

        self.main_window = None

        QtWidgets.QApplication.__init__( self, [sys.argv[0]] )

        self.main_window = be_main_window.BemacsMainWindow( self )

        try_wrapper = be_exceptions.TryWrapperFactory( self.log )

        self.MarshallToGuiThreadSignal.connect( self.handleMarshallToGuiThread )

    def eventWrapper( self, function ):
        return EventScheduling( self, function )

    def isMainThread( self ):
        'return true if the caller is running on the main thread'
        return self.main_thread is threading.currentThread()

    def setupLogging( self ):
        self.log = logging.getLogger( 'bemacs' )

        if self.__debug:
            self.log.setLevel( logging.DEBUG )
        else:
            self.log.setLevel( logging.INFO )

        if self.__log_stdout:
            handler = StdoutLogHandler()
            formatter = logging.Formatter( '%(asctime)s %(levelname)s %(message)s' )
            handler.setFormatter( formatter )
            self.log.addHandler( handler )

        else:
            log_filename = be_platform_specific.getLogFilename()
            # keep 10 logs of 100K each
            handler = RotatingFileHandler( log_filename, 'a', 100*1024, 10 )
            formatter = logging.Formatter( '%(asctime)s %(levelname)s %(message)s' )
            handler.setFormatter( formatter )
            self.log.addHandler( handler )

        self.log.info( T_("Barry's Emacs starting") )

        self.log.debug( 'debug enabled' )

    def log_client_error( self, e, title='Error' ):
        # must run on the main thread
        if not self.isMainThread():
            self.marshallToGuiThread( self.log_client_error, (e, title) )
            return

        self.__last_client_error = []
        for message, _ in e.args[1]:
            self.__last_client_error.append( message )
            self.log.error( message )

        message = '\n'.join( self.__last_client_error )
        QtWidgets.QMessageBox.critical( self, title, message ).exec_()

    def log_error( self, e, title='Error' ):
        # must run on the main thread
        if not self.isMainThread():
            self.marshallToGuiThread( self.log_error, (e, title) )
            return

        message = str( e )
        self.log.error( message )

        QtWidgets.QMessageBox.critical( self, title, message ).exec_()

    def setStatus( self, all_values ):
        if self.main_window is not None:
            self.main_window.setStatus( all_values )

    def refreshFrame( self ):
        self.main_window.refreshFrame()

    def writePreferences( self ):
        self.prefs.writePreferences()

    # notify app that the emacs panel is ready for use
    def onEmacsPanelReady( self ):
        self._debugApp( 'BemacsApp.onEmacsPanelReady()' )
        self.marshallToGuiThread( self.__initEditorThread, () )

    def OnActivateApp( self, event ):
        if self.editor is None:
            # too early or too late
            return

        if self.lock_ui == 0:
            if event.GetActive():
                self.editor.guiHasFocus()
        else:
            if event.GetActive():
                self.need_activate_app_action = True

    def onCloseEditor( self ):
        self.log.debug( 'onCloseEditor()' )
        self.editor.guiCloseWindow()

    def callGuiFunction( self, function, args ):
        self.__call_gui_result_event.clear()
        self.marshallToGuiThread( self.executeCallGuiFunction, (function, args) )
        self.__call_gui_result_event.wait()
        return self.__call_gui_result

    def executeCallGuiFunction( self, function, args ):
        self.__call_gui_result = function( *args )
        self.__call_gui_result_event.set()

    def marshallToGuiThread( self, function, args ):
        m = MarshalledCall( function, args )
        self._debugCallback( 'marshallToGuiThread %r sent' % (m,) )
        self.__callback_queue.put( m )
        self.MarshallToGuiThreadSignal.emit()

    def handleMarshallToGuiThread( self ):
        m = self.__callback_queue.get( block=False )
        self._debugCallback( 'handleMarshallToGuiThread %r start' % (m,) )

        try:
            m.dispatch()

        except:
            self.log.exception( 'handleMarshallToGuiThread\n' )

        self._debugCallback( 'handleMarshallToGuiThread %r done' % (m,) )

    def debugShowCallers( self, depth ):
        if not self.__debug:
            return

        stack = inspect.stack()
        for index in range( 1, depth+1 ):
            if index >= len(stack):
                break

            caller = stack[ index ]
            filename = os.path.basename( caller[1] )
            self._debugApp( 'File: %s:%d, Function: %s' % (filename, caller[2], caller[3]) )
            del caller

        del stack

    def guiReportException( self, body, title ):
        QtWidgets.QMessageBox.critical( self.main_window, title, body ).exec_()

    #--------------------------------------------------------------------------------
    def __initCommandLineThread( self ):
        self._debugApp( 'BemacsApp.__initCommandLineThread()' )
        if self.__mock_editor:
            return

        self.command_line_thread.daemon = True
        self.command_line_thread.start()

    def __runCommandLineHandler( self ):
        try:
            if sys.platform.startswith( 'win' ):
                self.__windowsCommandLineHandler()

            else:
                # unix and OS X
                self.__posixCommandLineHandler()

        except Exception as e:
            self.log.exception( 'command line exception' )

            self.marshallToGuiThread( self.guiReportException, (str(e), 'Command line Exception') )

    def __windowsCommandLineHandler( self ):
        self._debugApp( '__windowsCommandLineHandler()' )

        import ctypes

        PIPE_UNLIMITED_INSTANCES = 255
        PIPE_ACCESS_DUPLEX = 3
        PIPE_TYPE_MESSAGE = 4
        FILE_FLAG_OVERLAPPED = 0x40000000
        INFINITE = -1
        ERROR_PIPE_CONNECTED = 535
        WAIT_OBJECT_0 = 0

        class OVERLAPPED(ctypes.Structure):
            _fields_ =  [('status', ctypes.c_uint)
                        ,('transfered', ctypes.c_uint)
                        ,('offset', ctypes.c_ulonglong)
                        ,('hevent', ctypes.c_uint)]

        self.__h_wait_stop = ctypes.windll.kernel32.CreateEventW( None, 0, 0, None )

        # We need to use overlapped IO for this, so we dont block when
        # waiting for a client to connect.  This is the only effective way
        # to handle either a client connection, or a service stop request.
        self.__overlapped = OVERLAPPED( 0, 0, 0, 0 )

        # And create an event to be used in the OVERLAPPED object.
        self.__overlapped.hevent = ctypes.windll.kernel32.CreateEventW( None, 0, 0, None )

        # We create our named pipe.
        pipe_name = "\\\\.\\pipe\\Barry's Emacs"

        h_pipe = ctypes.windll.kernel32.CreateNamedPipeA(
                        pipe_name,                  #  __in      LPCTSTR lpName,
                        PIPE_ACCESS_DUPLEX
                        | FILE_FLAG_OVERLAPPED,     #  __in      DWORD dwOpenMode,
                        PIPE_TYPE_MESSAGE,          #  __in      DWORD dwPipeMode,
                        PIPE_UNLIMITED_INSTANCES,   #  __in      DWORD nMaxInstances,
                        0,                          #  __in      DWORD nOutBufferSize,
                        0,                          #  __in      DWORD nInBufferSize,
                        100,                        #  __in      DWORD nDefaultTimeOut, (100ms)
                        None                        #  __in_opt  LPSECURITY_ATTRIBUTES lpSecurityAttributes
                        )

        # Loop accepting and processing connections
        while True:
            self._debugApp( '__windowsCommandLineHandler loop top' )

            hr = ctypes.windll.kernel32.ConnectNamedPipe( h_pipe, ctypes.byref( self.__overlapped ) )
            if hr == ERROR_PIPE_CONNECTED:
                # Client is fast, and already connected - signal event
                ctypes.windll.kernel32.SetEvent( self.__overlapped.hevent )

            self._debugApp( '__windowsCommandLineHandler connected to named pipe' )

            # Wait for either a connection, or a service stop request.
            wait_handles_t = ctypes.c_uint * 2
            wait_handles = wait_handles_t( self.__h_wait_stop, self.__overlapped.hevent )

            self._debugApp( '__windowsCommandLineHandler WaitForMultipleObjects...' )
            rc = ctypes.windll.kernel32.WaitForMultipleObjects( 2, ctypes.byref( wait_handles ), 0, INFINITE )

            if rc == WAIT_OBJECT_0:
                self._debugApp( '__windowsCommandLineHandler Stop event' )
                # Stop event
                break

            else:
                self._debugApp( '__windowsCommandLineHandler data ready for read' )

                # Pipe event - read the data, and write it back.
                buf_size = ctypes.c_uint( 32768 )
                buf_client = ctypes.create_string_buffer( buf_size.value )
                hr = ctypes.windll.kernel32.ReadFile( h_pipe, buf_client, buf_size, ctypes.byref( buf_size ), None )
                client_command = buf_client.raw[:buf_size.value]

                self._debugApp( '__windowsCommandLineHandler read command %r' % (client_command,) )

                reply = ctypes.create_string_buffer( 32 )
                if len( client_command ) > 0:
                    if client_command[0] == 'P':
                        reply.value = 'p%d' % (os.getpid(),)

                    elif client_command[0] == 'C':
                        self.marshallToGuiThread( self.guiClientCommandHandler, (client_command[1:],) )
                        reply.value = ' '

                    else:
                        reply.value = 'R'+'Unknown client command'

                    reply_size = ctypes.c_uint( len( reply.value ) )

                    hr = ctypes.windll.kernel32.WriteFile( h_pipe, reply, reply_size, ctypes.byref( reply_size ), None )

                # And disconnect from the client.
                ctypes.windll.kernel32.DisconnectNamedPipe( h_pipe )
                self._debugApp( '__windowsCommandLineHandler Disconnected named pipe' )

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

    def __posixCommandLineHandler( self ):
        self._debugApp( '__posixCommandLineHandler' )
        import pwd
        import select

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

        self._debugApp( '__posixCommandLineHandler before read loop' )
        while True:
            r, w, x = select.select( [emacs_server_read_fd], [], [], 1.0 )
            reply = ' '
            if emacs_server_read_fd in r:
                reply = 'R' 'Unknown client command'

                client_command = os.read( emacs_server_read_fd, 16384 )
                self._debugApp( '__posixCommandLineHandler command %r' % (client_command,) )
                if len( client_command ) > 0:
                    if client_command[0] == 'C':
                        self.marshallToGuiThread( self.guiClientCommandHandler, (client_command[1:],) )
                        reply = ' '                        

                emacs_client_write_fd = os.open( client_fifo, os.O_WRONLY|os.O_NONBLOCK );
                if emacs_client_write_fd < 0:
                    return

                self._debugApp( '__posixCommandLineHandler response %r' % (reply,) )
                os.write( emacs_client_write_fd, reply )
                os.close( emacs_client_write_fd )

    def guiClientCommandHandler( self, client_command ):
        all_client_args = [part.decode('utf-8') for part in client_command.split( '\x00' )]
        command_directory = all_client_args[0]
        command_args = all_client_args[1:]

        self.main_window.Raise()

        self._debugApp( 'guiClientCommandHandler: command_directory %r' % (command_directory,) )
        self._debugApp( 'guiClientCommandHandler: command_args %r' % (command_args,) )
        self.editor.guiClientCommand( command_directory, command_args )

    def __makeFifo( self, fifo_name ):
        if os.path.exists( fifo_name ):
            stats = os.stat( fifo_name )
            if not stat.S_ISFIFO( stats.st_mode ):
                self.log.error( '%s is not a fifo' % (fifo_name,) )

            elif stats.st_size == 0:
                return

            os.remove( fifo_name )

        os.mkfifo( fifo_name, stat.S_IRUSR|stat.S_IWUSR )

    #--------------------------------------------------------------------------------
    def __initEditorThread( self ):
        self._debugApp( 'BemacsApp.__initEditorThread()' )
        if self.__mock_editor:
            return

        self.editor_thread.daemon = True
        self.editor_thread.start()

    def __runEditor( self ):
        import be_editor

        try:
            self._debugApp( 'BemacsApp.__runEditor()' )

            self.editor = be_editor.BEmacs( self )
            self.editor.initEmacsProfile( self.main_window.emacs_panel )

            # now that emacs has init'ed and processed any command line
            # the command line handler can be started
            self.marshallToGuiThread( self.__initCommandLineThread, () )

            # stay in processKeys until editor quits
            while True:
                rc = self.editor.processKeys()
                self._debugApp( 'processKeys rc %r' % (rc,) )

                mod = self.editor.modifiedFilesExist()
                if not mod:
                    break

                can_exit = self.callGuiFunction( self.guiYesNoDialog, (False, 'Modifier files exist', 'Do you really want to quit Emacs?') );
                if can_exit:
                    break

        except Exception as e:
            self.log.exception( 'editor exception' )

            self.callGuiFunction( self.guiReportException, (str(e), 'Editor Exception') )

        self.marshallToGuiThread( self.quit, () )

    def guiYesNoDialog( self, default_to_yes, title, message ):
        #qqq# What is default for?
        if default_to_yes:
            default_button = QtWidgets.QMessageBox.Yes
        else:
            default_button = QtWidgets.QMessageBox.No

        rc = QtWidgets.QMessageBox.question( self.main_window, title, message, defaultButton=default_button )
        return rc == QtWidgets.QMessageBox.Yes

    def quit( self ):
        self._debugApp( 'quit()' )
        self.may_quit = True
        self.main_window.close()

    def BringWindowToFront( self ):
        try: # it's possible for this event to come when the frame is closed
            pass
            #self.GetTopWindow().Raise()
        except:
            pass

    def MacOpenFile( self, filename ):
        # Called for files droped on dock icon, or opened via finders context menu
        self.log.debug( 'MacOpenFile( %s )' % (filename,) )
        if self.editor is not None:
            self.editor.guiOpenFile( filename )
        self.BringWindowToFront()

    def MacReopenApp( self ):
        # Called when the doc icon is clicked, and ???
        self.BringWindowToFront()

    def MacNewFile( self ):
        pass

    def MacPrintFile( self, file_path ):
        pass

    def setWindowTitle( self, title_suffix ):
        if self.opt_name is None:
            title = "Barry's Emacs - %s" % (title_suffix,)
        else:
            title = "%s - %s" % (self.opt_name, title_suffix)

        self.main_window.setWindowTitle( title )

#--------------------------------------------------------------------------------
#
#    RotatingFileHandler - based on python lib class
#
#--------------------------------------------------------------------------------
class RotatingFileHandler(logging.FileHandler):
    def __init__(self, filename, mode="a", maxBytes=0, backupCount=0):
        """
        Open the specified file and use it as the stream for logging.

        By default, the file grows indefinitely. You can specify particular
        values of maxBytes and backupCount to allow the file to rollover at
        a predetermined size.

        Rollover occurs whenever the current log file is nearly maxBytes in
        length. If backupCount is >= 1, the system will successively create
        new files with the same pathname as the base file, but with extensions
        ".1", ".2" etc. appended to it. For example, with a backupCount of 5
        and a base file name of "app.log", you would get "app.log",
        "app.log.1", "app.log.2", ... through to "app.log.5". The file being
        written to is always "app.log" - when it gets filled up, it is closed
        and renamed to "app.log.1", and if files "app.log.1", "app.log.2" etc.
        exist, then they are renamed to "app.log.2", "app.log.3" etc.
        respectively.

        If maxBytes is zero, rollover never occurs.
        """
        logging.FileHandler.__init__(self, filename, mode)
        self.maxBytes = maxBytes
        self.backupCount = backupCount
        if maxBytes > 0:
            self.mode = "a"

    def doRollover(self):
        """
        Do a rollover, as described in __init__().
        """

        self.stream.close()
        if self.backupCount > 0:
            prefix, suffix = os.path.splitext( self.baseFilename )
            for i in range(self.backupCount - 1, 0, -1):
                sfn = "%s.%d%s" % (prefix, i, suffix)
                dfn = "%s.%d%s" % (prefix, i+1, suffix)
                if os.path.exists(sfn):
                    #print( "%s -> %s" % (sfn, dfn) )
                    if os.path.exists(dfn):
                        os.remove(dfn)
                    os.rename(sfn, dfn)
            dfn = self.baseFilename + ".1"
            if os.path.exists(dfn):
                os.remove(dfn)
            os.rename(self.baseFilename, dfn)
            #print( "%s -> %s" % (self.baseFilename, dfn) )
        self.stream = open(self.baseFilename, "w")

    def emit(self, record):
        """
        Emit a record.

        Output the record to the file, catering for rollover as described
        in setRollover().
        """
        if self.maxBytes > 0:                   # are we rolling over?
            msg = "%s\n" % self.format(record)
            try:
                self.stream.seek(0, 2)  #due to non-posix-compliant Windows feature
                if self.stream.tell() + len(msg) >= self.maxBytes:
                    self.doRollover()

            except ValueError:
                # on Windows we get "ValueError: I/O operation on closed file"
                # when a second copy of workbench is run
                self.doRollover()

        logging.FileHandler.emit(self, record)

class StdoutLogHandler(logging.Handler):
    def __init__( self ):
        logging.Handler.__init__( self )

    def emit( self, record ):
        try:
            msg = self.format( record ) + '\n'

            sys.stdout.write( msg )
            sys.stdout.flush()

        except:
            self.handleError( record )

class FakeEditor(be_debug.EmacsDebugMixin):
    def __init__( self, app ):
        be_debug.EmacsDebugMixin.__init__( self )

        be_debug._debug_editor = True

        self.app = app
        self.count = 0

        self.vert_scroll = []
        self.horz_scroll = []

        self.log = self

    def debug( self, msg ):
        self.__writeToScreen( 3, 'debug: %s' % (msg,) )

    def __writeToScreen( self, line, text ):
        p = self.app.main_window.emacs_panel

        attr = [0] * len(text)

        new = (text, attr)

        p.termUpdateBegin()
        p.termUpdateLine( None, new, line )
        p.termTopos( line, 10 )
        p.termUpdateEnd( {'readonly': True, 'overstrike': False, 'eol': 'LF', 'line': 4199, 'column': 9}, self.vert_scroll, self.horz_scroll )

    def guiCloseWindow( self, *args, **kwds ):
        self.app.marshallToGuiThread( self.app.quit, () )

    def guiGeometryChange( self, *args, **kwds ):
        pass

    def guiHasFocus( sefl, *args, **kwds ):
        pass

    def guiEventChar( self, char, shift ):
        print( 'guiEventChar( %r, %r' % (char, shift) )
        self.count += 1

        self.__writeToScreen( 1, '  %6d guiEventChar( %r, %r ) called' % (self.count, char, shift) )

        if char == 'c':
            self.app.marshallToGuiThread( self.uiHookEditCopy, ('edit-copy', 'quick brown fox') )

        elif char == 'v':
            self.app.marshallToGuiThread( self.uiHookEditPaste, ('edit-paste',) )

    def guiEventMouse( self, *args, **kwds ):
        self.count += 1

        self.__writeToScreen( 2, '  %6d guiEventMouse called' % (self.count,) )

    # Only used in __mock_editor mode
    def uiHookEditCopy( self, cmd, text ):
        self.clipboard().setText( text )

    # Only used in __mock_editor mode
    def uiHookEditPaste( self, cmd, use_primary=False ):
        self._debugEditor( 'uiHookEditPaste use_primary=%r' % (use_primary,) )

        if use_primary:
            text = self.clipboard().text( mode=QtGui.QClipBoard.Selection )

        else:
            text = self.clipboard().text( mode=QtGui.QClipBoard.Clipboard )

        text = text.replace( '\r\n', '\n' ).replace( '\r', '\n' )

        self._debugEditor( 'uiHookEditPaste text=%r' % (text,) )

class MarshalledCall:
    def __init__( self, function, args ):
        self.function = function
        self.args = args

    def dispatch( self ):
        self.function( *self.args )

    def __repr__( self ):
        return 'MarshalledCall: fn=%s nargs=%d' % (self.function.__name__,len(self.args))
