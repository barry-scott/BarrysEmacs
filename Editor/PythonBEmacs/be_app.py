'''
 ====================================================================
 Copyright (c) 2003-2010 Barry A Scott.  All rights reserved.

 This software is licensed as described in the file LICENSE.txt,
 which you should have received as part of this distribution.

 ====================================================================

    wb_app.py

    Based on code from pysvn WorkBench

'''
import sys
import os
import types
import logging
import tempfile
import threading
import inspect
import gettext

import wx
import wx.lib
import wx.lib.newevent

import be_platform_specific
import be_frame
import be_preferences
import be_exceptions


AppCallBackEvent, EVT_APP_CALLBACK = wx.lib.newevent.NewEvent()

class BemacsApp(wx.App):
    def __init__( self, startup_dir, args ):
        self.args = args
        self.app_name = os.path.basename( args[0] )
        self.app_dir = os.path.dirname( args[0] )
        if self.app_dir == '':
            self.app_dir = startup_dir

        self.emacs_library_dir = os.path.abspath( os.path.join( self.app_dir, 'emacs_library' ) )
        self.emacs_library_dir = os.environ.get( 'BEMACS_EMACS_LIBRARY', self.emacs_library_dir )

        self.__debug_noredirect = False
        self.__debug = True
        self.__wx_raw_debug = False
        self.__log_stdout = False

        while len(args) > 1:
            arg = args[ 1 ]
            if arg == '--noredirect':
                self.__debug_noredirect = True
                del args[ 1 ]

            elif arg == '--log-stdout':
                self.__log_stdout = True
                del args[ 1 ]

            elif arg == '--debug':
                self.__debug = True
                del args[ 1 ]

            elif arg == '--wx-raw-debug':
                self.__wx_raw_debug = True
                del args[ 1 ]

            elif arg == '--':
                break

            else:
                break

        self.__call_gui_result = None
        self.__call_gui_result_event = threading.Event()

        self.editor = None

        self.main_thread = threading.currentThread()
        if self.__wx_raw_debug:
            self.editor_thread = None
            self.editor = FakeEditor( self )
        else:
            self.editor_thread = threading.Thread( name='Editor', target=self.__runEditor )

        self.progress_format = None
        self.progress_values = {}

        be_platform_specific.setupPlatform()

        locale_path = be_platform_specific.getLocalePath( self )
        self.translation = gettext.translation(
                'bemacs',
                locale_path,
                # language defaults 
                fallback=True )

        import __builtin__
        # T_( 'non plural' )
        __builtin__.__dict__['T_'] = self.translation.ugettext
        # S_( 'singular', 'plural', n )
        __builtin__.__dict__['S_'] = self.translation.ungettext
        # U_( 'static string' )
        __builtin__.__dict__['U_'] = lambda s: s

        # Debug settings
        self.__last_client_error = []

        self.setupLogging()

        self.log.info( 'app_name %s app_dir %s' % (self.app_name, self.app_dir) )
        self.log.info( 'locale_path %s' % locale_path )
        self.log.info( 'find %r' % gettext.find( 'pysvn_workbench', locale_path ) )
        self.log.info( 'info %r' % self.translation.info() )
        self.log.info( T_("Barry's Emacs") )

        if '--test' in args:
            self.prefs = be_preferences.Preferences(
                    self,
                    be_platform_specific.getPreferencesFilename() + '.test',
                    be_platform_specific.getOldPreferencesFilename() + '.test' )

        else:
            self.prefs = be_preferences.Preferences(
                    self,
                    be_platform_specific.getPreferencesFilename(),
                    be_platform_specific.getOldPreferencesFilename() )

        self.lock_ui = 0
        self.need_activate_app_action = False

        self.frame = None

        wx.App.__init__( self, 0 )

        try_wrapper = be_exceptions.TryWrapperFactory( self.log )

        wx.EVT_ACTIVATE_APP( self, try_wrapper( self.OnActivateApp ) )
        EVT_APP_CALLBACK( self, try_wrapper( self.OnAppCallBack ) )

    def eventWrapper( self, function ):
        return EventScheduling( self, function )

    def isMainThread( self ):
        'return true if the caller is running on the main thread'
        return self.main_thread is threading.currentThread()

    def setupLogging( self ):
        self.log = logging.getLogger( 'BEmacs' )

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
            self.onGuiThread( self.log_client_error, (e, title) )
            return

        self.__last_client_error = []
        for message, _ in e.args[1]:
            self.__last_client_error.append( message )
            self.log.error( message )

        wx.MessageBox( '\n'.join( self.__last_client_error ), title, style=wx.OK|wx.ICON_ERROR );

    def log_error( self, e, title='Error' ):
        # must run on the main thread
        if not self.isMainThread():
            self.onGuiThread( self.log_error, (e, title) )
            return

        message = str( e )
        self.log.error( message )

        wx.MessageBox( message, title, style=wx.OK|wx.ICON_ERROR );

    def refreshFrame( self ):
        self.frame.refreshFrame()

    def savePreferences( self ):
        self.prefs.writePreferences()

    def exitAppNow( self ):
        if self.lock_ui > 0:
            # return False to veto a close
            return False

        self.frame.savePreferences()
        self.prefs.writePreferences()
        self.frame = None

        return True

    def OnInit(self):
        self.frame = be_frame.BemacsFrame( self )
        self.frame.Show( True )
        self.SetTopWindow( self.frame )

        return True

    # notify app that the emacs panel is ready for use
    def onEmacsPanelReady( self ):
        self.log.debug( 'BemacsApp.onEmacsPanelReady()' )
        self.onGuiThread( self.__initEditorThread, () )

    def OnActivateApp( self, event ):
        if self.frame is None:
            # too early or too late
            return

        if self.lock_ui == 0:
            self.frame.OnActivateApp( event.GetActive() )
        else:
            if event.GetActive():
                self.need_activate_app_action = True

    def onCloseEditor( self ):
        self.log.info( 'onCloseEditor()' )
        self.editor.guiCloseWindow()

    def callGuiFunction( self, function, args ):
        self.__call_gui_result_event.clear()
        self.onGuiThread( self.executeCallGuiFunction, (function, args) )
        self.__call_gui_result_event.wait()
        return self.__call_gui_result

    def executeCallGuiFunction( self, function, args ):
        self.__call_gui_result = function( *args )
        self.__call_gui_result_event.set()

    def onGuiThread( self, function, args ):
        wx.PostEvent( self, AppCallBackEvent( callback=function, args=args ) )

    def OnAppCallBack( self, event ):
        try:
            event.callback( *event.args )
        except:
            self.log.exception( 'OnAppCallBack<%s.%s>\n' %
                (event.callback.__module__, event.callback.__name__ ) )

    def debugShowCallers( self, depth ):
        if not self.__debug:
            return

        stack = inspect.stack()
        for index in range( 1, depth+1 ):
            if index >= len(stack):
                break

            caller = stack[ index ]
            filename = os.path.basename( caller[1] )
            self.log.debug( 'File: %s:%d, Function: %s' % (filename, caller[2], caller[3]) )
            del caller

        del stack

    def __initEditorThread( self ):
        self.log.debug( 'BemacsApp.__initEditorThread()' )
        if self.__wx_raw_debug:
            return

        self.editor_thread.start()

    def __runEditor( self ):
        import be_editor

        try:

            self.log.debug( 'BemacsApp.__runEditor()' )
            self.editor = be_editor.BEmacs( self )
            self.editor.initEmacsProfile( self.frame.emacs_panel )
            
            # stay in processKeys until editor quits
            while True:
                rc = self.editor.processKeys()
                self.log.info( 'processKeys rc %r' % (rc,) )

                mod = self.editor.modifiedFilesExist()
                if not mod:
                    break

                can_exit = self.callGuiFunction( self.guiYesNoDialog, (False,) );
                if can_exit:
                    break
        except Exception, e:
            self.log.exception( 'editor exception' )
            dlg = wx.MessageDialog(
                       self.frame,
                        str(e),
                        'Editor Exception',
                        wx.ICON_EXCLAMATION
                        )
            rc = dlg.ShowModal()
            dlg.Destroy()

        self.onGuiThread( self.quit, () )

    def guiYesNoDialog( self, default ):
        dlg = wx.MessageDialog(
                   self.frame,
                    'Do you really want to quit Emacs?',
                    'Modifier files exist',
                    wx.YES_NO | wx.NO_DEFAULT | wx.ICON_EXCLAMATION
                    )
        rc = dlg.ShowModal()
        dlg.Destroy()

        return rc == wx.ID_YES

    def quit( self ):
        self.log.info( 'quit()' )
        self.frame.Destroy()

    def BringWindowToFront( self ):
        try: # it's possible for this event to come when the frame is closed
            pass
            #self.GetTopWindow().Raise()
        except:
            pass

    def MacOpenFile( self, filename ):
        # Called for files droped on dock icon, or opened via finders context menu
        self.log.info( 'MacOpenFile( %s )' % (filename,) )
        print "%s dropped on app" % (filename,) #code to load filename goes here.
        self.editor.guiOpenFile( filename )
        self.BringWindowToFront()

    def MacReopenApp( self ):
        # Called when the doc icon is clicked, and ???
        self.BringWindowToFront()

    def MacNewFile( self ):
        pass
    
    def MacPrintFile( self, file_path ):
        pass

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
                    #print "%s -> %s" % (sfn, dfn)
                    if os.path.exists(dfn):
                        os.remove(dfn)
                    os.rename(sfn, dfn)
            dfn = self.baseFilename + ".1"
            if os.path.exists(dfn):
                os.remove(dfn)
            os.rename(self.baseFilename, dfn)
            #print "%s -> %s" % (self.baseFilename, dfn)
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

class FakeEditor:
    def __init__( self, app ):
        self.app = app
        self.count = 0

    def guiCloseWindow( self, *args, **kwds ):
        self.app.onGuiThread( self.app.quit, () )

    def guiGeometryChange( self, *args, **kwds ):
        pass

    def guiEventChar( self, *args, **kwds ):
        p = self.app.frame.emacs_panel

        self.count += 1

        text = '  %d guiEventChar called' % (self.count,)
        attr = [0] * len(text)

        new = (text, attr)

        p.termUpdateBegin()
        p.termUpdateLine( None, new, 1 )
        p.termTopos( 1, 7 )
        p.termUpdateEnd()

    def guiEventMouse( self, *args, **kwds ):
        p = self.app.frame.emacs_panel

        self.count += 1

        text = '  %d guiEventMouse called' % (self.count,)
        attr = [0] * len(text)

        new = (text, attr)

        p.termUpdateBegin()
        p.termUpdateLine( None, new, 1 )
        p.termTopos( 1, 10 )
        p.termUpdateEnd()
