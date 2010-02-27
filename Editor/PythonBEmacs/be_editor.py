'''
 ====================================================================
 Copyright (c) 2009-2010 Barry A Scott.  All rights reserved.

 This software is licensed as described in the file LICENSE.txt,
 which you should have received as part of this distribution.

 ====================================================================

    be_editor.py
    

'''
from __future__ import with_statement;

import sys
import time
import threading

import be_platform_specific

import _bemacs

import wx

class BEmacs(_bemacs.BemacsEditor):
    def __init__( self, app ):
        _bemacs.BemacsEditor.__init__( self,
                be_platform_specific.getUserDir(),
                be_platform_specific.getLibraryDir() )

        self.app = app
        self.log = app.log

        self.window = None

        self.__quit_editor = False
        self.__event_queue = Queue( self.log )

        self.__gui_result_event = threading.Event()

        self.__clipboard_data = None

        self.hook_ui_handlers = {
            "edit-copy":    self.uiHookEditCopy,
            "edit-paste":   self.uiHookEditPaste,
            "test1":        self.uiHookTest1,
            "test2":        self.uiHookTest2,
            }


    def initEmacsProfile( self, window ):
        self.log.debug( 'BEmacs.initEmacsProfile()' )
        assert window is not None
        self.window = window

        # initEditor will start calling termXxx functions - must have windows setup first
        self.initEditor()
        self.setKeysMapping( self.window.getKeysMapping() )
        self.log.debug( 'BEmacs.initEmacsProfile() geometryChange %r %r' %
                            (self.window.term_width, self.window.term_length) )
        self.geometryChange( self.window.term_width, self.window.term_length )

        self.log.debug( 'TESTING' )
        #_bemacs.function.debug_emacs( 'flags=key,exec,tmp' )
        #_bemacs.variable.error_messages_buffer = "error-messages"

        self.log.debug( 'BEmacs.initEmacsProfile() emacs_profile.ml' )
        _bemacs.function.execute_mlisp_file( 'emacs_library:emacs_profile.ml' )

        self.executeEnterHooks()

        self.processCommandLine( self.app.args );

    def guiCloseWindow( self ):
        self.__event_queue.put( (self.closeWindow, ()) )

    def closeWindow( self ):
        self.__quit_editor = True

    def openFile( self, filename ):
        self.log.info( 'openFile( %s ) start' % (filename,) )
        try:
            _bemacs.function.visit_file( filename )
            # force a redraw
            self.inputChar( -1, False );

            self.log.info( 'openFile done' )

        except Exception, e:
            self.log.error( 'openFile - %s' % (e,) )


    def guiEventChar( self, ch, shift ):
        self.__event_queue.put( (self.inputChar, (ch, shift)) )

    def guiEventMouse( self, keys, shift, all_params ):
        self.__event_queue.put( (self.inputMouse, (keys, shift, all_params)) )

    def guiGeometryChange( self, width, length ):
        self.__event_queue.put( (self.geometryChange, (width, length)) )

    def guiOpenFile( self, filename ):
        self.log.info( 'guiOpenFile %s' % (filename,) )
        self.__event_queue.put( (self.openFile, (filename,)) )

    #--------------------------------------------------------------------------------
    def uiHookTest1( self, cmd ):
        self.setGuiResultSuccess( 99 )

    def uiHookTest2( self, cmd, text1, text2 ):
        dlg = wx.MessageDialog(
                    self.window,
                    unicode(text1),
                    unicode(text2),
                    wx.YES_NO | wx.NO_DEFAULT | wx.ICON_EXCLAMATION
                    )
        rc = dlg.ShowModal()
        dlg.Destroy()

        if rc == wx.ID_YES:
            self.setGuiResultSuccess( 'yes' )
        else:
            self.setGuiResultSuccess( 'no' )


    def uiHookEditCopy( self, cmd, text ):
        self.__clipboard_data = wx.TextDataObject()
        self.__clipboard_data.SetText( text )
        if wx.TheClipboard.Open():
            wx.TheClipboard.SetData( self.__clipboard_data )
            wx.TheClipboard.Close()
            self.setGuiResultSuccess( None )

        else:
            self.setGuiResultError( ValueError( 'failed to set data on clipboard' ) )

    def uiHookEditPaste( self, cmd ):
        success = False
        do = wx.TextDataObject()
        if wx.TheClipboard.Open():
            success = wx.TheClipboard.GetData( do )
            wx.TheClipboard.Close()

        if success:
            text = do.GetText().replace( '\r', '\n' )
            self.setGuiResultSuccess( text )

        else:
            self.setGuiResultError( ValueError( 'clipboard is empty' ) )

    def hookUserInterface( self, *args ):
        self.log.debug( 'hookUserInterface( %r )' % (args,) )
        cmd = args[0]
        if cmd in self.hook_ui_handlers:
            self.log.debug( 'hookUserInterface calling handler' )
            self.app.onGuiThread( self.hook_ui_handlers[ cmd ], args )
            self.log.debug( 'hookUserInterface waiting for result' )

            error, value = self.getGuiResult()

            self.log.debug( 'hookUserInterface error %r value %r' % (error, value) )

            if error is not None:
                self.log.debug( 'hookUserInterface handler error return' )
                raise error

            else:
                self.log.debug( 'hookUserInterface handler normal return' )
                return value

        else:
            raise ValueError( 'Unknown command %r' % (cmd,) )

    def getGuiResult( self ):
        self.__gui_result_event.clear()
        self.__gui_result_event.wait()
        result = self.__gui_result
        self.__gui_result = None
        return result

    def setGuiResultSuccess( self, result ):
        self.__gui_result = (None, result)
        self.__gui_result_event.set()

    def setGuiResultError( self, error ):
        self.__gui_result = (error, None)
        self.__gui_result_event.set()

    #--------------------------------------------------------------------------------
    def termCheckForInput( self ):
        pass

    def termWaitForActivity( self ):
        try:
            event_hander_and_args = self.__event_queue.get()

            while event_hander_and_args is not None:
                handler, args = event_hander_and_args
                self.log.debug( 'waitForActivity: handler %r' % (handler,) )
                self.log.debug( 'waitForActivity: args %r' % (args,) )
                handler( *args )

                event_hander_and_args = self.__event_queue.getNoWait()

            if self.__quit_editor:
                self.__quit_editor = False
                self.log.debug( 'waitForActivity self.__quit_editor set' )
                return -1

            return 0

        except Exception, e:
            self.log.exception( 'Error: waitForActivity: %s' % (str(e),) )
            self.app.debugShowCallers( 5 )
            return -1

    def termTopos( self, y, x ):
        self.app.onGuiThread( self.window.termTopos, (y, x) )

    def termReset( self ):
        self.app.onGuiThread( self.window.termReset, () )

    def termInit( self ):
        self.log.debug( 'BEmacs.termInit() window %r' % (self.window,) )
        self.app.onGuiThread( self.window.termInit, () )

    def termBeep( self ):
        self.app.onGuiThread( self.window.termBeep, () )

    def termUpdateBegin( self ):
        self.app.onGuiThread( self.window.termUpdateBegin, () )
        return True

    def termUpdateEnd( self ):
        self.app.onGuiThread( self.window.termUpdateEnd, () )

    def termUpdateLine( self, old, new, line_num ):
        self.app.onGuiThread( self.window.termUpdateLine, (old, new, line_num) )

    def termMoveLine( self, from_line, to_line ):
        self.app.onGuiThread( self.window.termMoveLine, (from_line, to_line) )

    def termDisplayActivity( self, ch ):
        self.app.onGuiThread( self.window.termDisplayActivity, (ch,) )

class Queue:
    def __init__( self, log ):
        self.log = log

        self.__all_items = []
        self.__lock = threading.RLock()
        self.__condition = threading.Condition( self.__lock )

    def getNoWait( self ):
        with self.__lock:
            if len( self.__all_items ) > 0:
                item = self.get()
                return item

        return None

    def get( self ):
        with self.__condition:
            while len( self.__all_items ) == 0:
                self.__condition.wait()

            return self.__all_items.pop( 0 )

    def put( self, item ):
        with self.__condition:
            self.__all_items.append( item )
            self.__condition.notify()
