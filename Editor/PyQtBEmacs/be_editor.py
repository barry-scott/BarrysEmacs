'''
 ====================================================================
 Copyright (c) 2009-2015 Barry A Scott.  All rights reserved.

 This software is licensed as described in the file LICENSE.txt,
 which you should have received as part of this distribution.

 ====================================================================

    be_editor.py
    

'''
from __future__ import with_statement

import sys
import os
import time
import threading
import subprocess

import be_platform_specific
import be_debug

import _bemacs

from PyQt5 import QtWidgets
from PyQt5 import QtCore

class BEmacs(_bemacs.BemacsEditor, be_debug.EmacsDebugMixin):
    def __init__( self, app ):
        _bemacs.BemacsEditor.__init__( self,
                be_platform_specific.getUserDir(),
                be_platform_specific.getLibraryDir() )

        be_debug.EmacsDebugMixin.__init__( self )

        self.app = app
        self.log = app.log

        self.window = None

        self.__quit_editor = False
        self.__event_queue = Queue( self.log )

        self.__gui_result_event = threading.Event()

        self.__clipboard_data = None

        self.hook_ui_handlers = {
            "edit-copy":        self.uiHookEditCopy,
            "edit-paste":       self.uiHookEditPaste,
            "yes-no-dialog":    self.uiHookYesNoDialog,
            "set-window-title": self.uiHookSetWindowTitle,
            "filter-string":    self.uiHookFilterString,
            "test1":            self.uiHookTest1,
            "test2":            self.uiHookTest2,
            }

    def mayQuit( self ):
        return self.__quit_editor

    def initEmacsProfile( self, window ):
        self._debugEditor( 'BEmacs.initEmacsProfile()' )
        assert window is not None
        self.window = window

        # initEditor will start calling termXxx functions - must have windows setup first
        self.initEditor()
        self.setKeysMapping( self.window.getKeysMapping() )
        self._debugEditor( 'BEmacs.initEmacsProfile() geometryChange %r %r' %
                            (self.window.term_width, self.window.term_length) )
        self.geometryChange( self.window.term_width, self.window.term_length )

        self._debugEditor( 'TESTING' )
        #_bemacs.function.debug_emacs( 'flags=key,exec,tmp,ml_error' )
        #_bemacs.variable.error_messages_buffer = "error-messages"

        self._debugEditor( 'BEmacs.initEmacsProfile() emacs_profile.ml' )
        _bemacs.function.execute_mlisp_file( 'emacs_library:emacs_profile.ml' )

        self.executeEnterHooks()

        self.clientCommand( os.getcwd(), ['emacs'] + self.app.args[1:] )

    def guiCloseWindow( self ):
        self.__event_queue.put( (self.closeWindow, ()) )

    def closeWindow( self ):
        self.__quit_editor = True

    def openFile( self, filename ):
        self.log.info( 'openFile( %s ) start' % (filename,) )
        try:
            _bemacs.function.visit_file( filename )
            # force a redraw
            self.inputChar( -1, False )

            self.log.info( 'openFile done' )

        except Exception as e:
            self.log.error( 'openFile - %s' % (e,) )


    #--------------------------------------------------------------------------------
    def guiHasFocus( self ):
        self.__event_queue.put( (self.hasFocus, ()) )

    #--------------------------------------------------------------------------------
    def guiClientCommand( self, command_directory, command_args ):
        self.__event_queue.put( (self.clientCommand, (command_directory, command_args)) )

    def clientCommand( self, command_directory, command_args ):
        self.log.info( 'clientCommand dir  %r' % (command_directory,) )
        self.log.info( 'clientCommand args %r' % (command_args,) )

        assert len(command_args) > 0

        self.newCommandLine( command_directory, command_args )

    #--------------------------------------------------------------------------------
    def guiEventChar( self, ch, shift ):
        self.__event_queue.put( (self.inputChar, (ch, shift)) )

    def guiEventMouse( self, keys, shift, all_params ):
        self.__event_queue.put( (self.inputMouse, (keys, shift, all_params)) )

    def guiGeometryChange( self, width, length ):
        self.__event_queue.put( (self.geometryChange, (width, length)) )

    def guiOpenFile( self, filename ):
        self.log.info( 'guiOpenFile %s' % (filename,) )
        self.__event_queue.put( (self.openFile, (filename,)) )

    def guiScrollChangeHorz( self, window_id, change ):
        self.__event_queue.put( (self.scrollChangeHorz, (window_id, change) ) )

    def guiScrollSetHorz( self, window_id, position ):
        self.__event_queue.put( (self.scrollSetHorz, (window_id, position) ) )

    def guiScrollChangeVert( self, window_id, change ):
        self.__event_queue.put( (self.scrollChangeVert, (window_id, change) ) )

    def guiScrollSetVert( self, window_id, position ):
        self.__event_queue.put( (self.scrollSetVert, (window_id, position) ) )

    #--------------------------------------------------------------------------------
    def uiHookTest1( self, cmd ):
        self.setGuiResultSuccess( 99 )

    def uiHookTest2( self, cmd, text1, text2 ):
        rc = QtWidgets.QMessageBox.question( self, text1, text2 )
        if rc == QtWidgets.QMessageBox.Yes:
            self.setGuiResultSuccess( 'yes' )

        else:
            self.setGuiResultSuccess( 'no' )

    def uiHookSetWindowTitle( self, cmd, title ):
        self.app.setWindowTitle( title )
        self.setGuiResultSuccess( None )

    def uiHookEditCopy( self, cmd, text ):
        self.clipboard().setText( text )

    def uiHookEditPaste( self, cmd, use_primary=False ):
        self._debugEditor( 'uiHookEditPaste use_primary=%r' % (use_primary,) )

        self._debugEditor( 'uiHookEditPaste use_primary=%r' % (use_primary,) )

        if use_primary:
            text = self.clipboard().text( mode=QtGui.QClipBoard.ClipBoard )

        else:
            text = self.clipboard().text( mode=QtGui.QClipBoard.ClipBoard )

        text = text.replace( '\r\n', '\n' ).replace( '\r', '\n' )
        self._debugEditor( 'uiHookEditPaste text=%r' % (text,) )
        self.setGuiResultSuccess( text )
        self._debugEditor( 'uiHookEditPaste setGuiResultSuccess' )

    def uiHookYesNoDialog( self, cmd, default, title, message ):
        result = self.app.guiYesNoDialog( default, title, message )
        self.setGuiResultSuccess( result )

    def uiHookFilterString( self, cmd, cmd_line, input_string ):
        try:
            p = subprocess.Popen(
                    cmd_line,
                    shell=True,
                    stdin=subprocess.PIPE,
                    stdout=subprocess.PIPE,
                    stderr=subprocess.STDOUT )

            out, err = p.communicate( input_string.encode( 'utf-8' ) )
            self.setGuiResultSuccess( out.decode( 'utf-8' ) )

        except EnvironmentError as e:
            self.setGuiResultError( str(e) )

    def hookUserInterface( self, *args ):
        self._debugEditor( 'hookUserInterface( %r )' % (args,) )
        cmd = args[0]
        if cmd in self.hook_ui_handlers:
            self.initGuiResult()

            self._debugEditor( 'hookUserInterface calling handler' )
            self.app.marshallToGuiThread( self.hook_ui_handlers[ cmd ], args )

            self._debugEditor( 'hookUserInterface waiting for result' )
            error, value = self.getGuiResult()

            self._debugEditor( 'hookUserInterface error %r value %r' % (error, value) )

            if error is not None:
                self._debugEditor( 'hookUserInterface handler error return' )
                raise error

            else:
                self._debugEditor( 'hookUserInterface handler normal return' )
                return value

        else:
            raise ValueError( 'Unknown command %r' % (cmd,) )

    def initGuiResult( self ):
        self.__gui_result_event.clear()

    def getGuiResult( self ):
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
        try:
            event_hander_and_args = self.__event_queue.getNoWait()

            while event_hander_and_args is not None:
                handler, args = event_hander_and_args
                self._debugEditor( 'checkForInput: handler %r' % (handler,) )
                self._debugEditor( 'checkForInput: args %r' % (args,) )
                handler( *args )

                event_hander_and_args = self.__event_queue.getNoWait()

            return 0

        except Exception as e:
            self.log.exception( 'Error: checkForInput: %s' % (str(e),) )
            self.app.debugShowCallers( 5 )
            return -1
 
    def termWaitForActivity( self, wait_until_time ):
        self._debugQueue( 'termWaitForActivity( %r )' % (wait_until_time,) )

        try:
            wait_timeout = wait_until_time - time.time()
            self._debugEditor( 'termWaitForActivity %r' % (wait_timeout,) )
            if wait_timeout <= 0:
                event_hander_and_args = self.__event_queue.getNoWait()
            else:
                event_hander_and_args = self.__event_queue.get( wait_timeout )

            while event_hander_and_args is not None:
                handler, args = event_hander_and_args
                self._debugEditor( 'waitForActivity: handler %r' % (handler,) )
                self._debugEditor( 'waitForActivity: args %r' % (args,) )
                handler( *args )

                event_hander_and_args = self.__event_queue.getNoWait()

            if self.__quit_editor:
                self.__quit_editor = False
                self._debugEditor( 'waitForActivity self.__quit_editor set' )
                return -1

            return 0

        except Exception as e:
            self.log.exception( 'Error: waitForActivity: %s' % (str(e),) )
            self.app.debugShowCallers( 5 )
            return -1

    def termTopos( self, y, x ):
        self.app.marshallToGuiThread( self.window.termTopos, (y, x) )

    def termReset( self ):
        self.app.marshallToGuiThread( self.window.termReset, () )

    def termInit( self ):
        self._debugEditor( 'BEmacs.termInit() window %r' % (self.window,) )
        self.app.marshallToGuiThread( self.window.termInit, () )

    def termBeep( self ):
        self.app.marshallToGuiThread( self.window.termBeep, () )

    def termUpdateBegin( self ):
        self._debugEditor( 'termUpdateBegin' )
        self.app.marshallToGuiThread( self.window.termUpdateBegin, () )
        return True

    def termUpdateEnd( self, all_status_bar_values, all_horz_scroll_bars, all_vert_scroll_bars ):
        self._debugEditor( 'termUpdateEnd' )
        self.app.marshallToGuiThread( self.window.termUpdateEnd, (all_status_bar_values, all_horz_scroll_bars, all_vert_scroll_bars) )

    def termUpdateLine( self, old, new, line_num ):
        self.app.marshallToGuiThread( self.window.termUpdateLine, (old, new, line_num) )

    def termMoveLine( self, from_line, to_line ):
        self.app.marshallToGuiThread( self.window.termMoveLine, (from_line, to_line) )

    def termDisplayActivity( self, ch ):
        self.app.marshallToGuiThread( self.window.termDisplayActivity, (ch,) )

class Queue(be_debug.EmacsDebugMixin):
    def __init__( self, log ):
        be_debug.EmacsDebugMixin.__init__( self )

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

    def get( self, timeout=None ):
        self._debugQueue( 'Queue.get( %r )' % (timeout,) )

        if timeout is not None:
            if timeout > 10000:
                raise ValueError( 'Timeout too big' )

        with self.__condition:
            if timeout is None:
                while len( self.__all_items ) == 0:
                    self.__condition.wait()

            else:
                if len( self.__all_items ) == 0:
                    self.__condition.wait( timeout )

            self._debugQueue( 'Queue.get( %r )' % (timeout,) )
            if len( self.__all_items ) != 0:
                return self.__all_items.pop( 0 )

            else:
                return None

    def put( self, item ):
        with self.__condition:
            self.__all_items.append( item )
            self.__condition.notify()
