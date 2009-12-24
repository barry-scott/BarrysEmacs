'''
 ====================================================================
 Copyright (c) 2009 Barry A Scott.  All rights reserved.

 This software is licensed as described in the file LICENSE.txt,
 which you should have received as part of this distribution.

 ====================================================================

    wb_editor.py

    

'''
import time
import threading

import _bemacs

class BEmacs(_bemacs.BemacsEditor):
    def __init__( self, app ):
        _bemacs.BemacsEditor.__init__( self )

        self.app = app
        self.window = None

        self.__event_queue = Queue()

    def initEmacsProfile( self, window ):
        self.window = window
        self.geometryChange( window.term_width, window.term_length )
        _bemacs.function.execute_mlisp_file( 'emacs_library:emacs_profile.ml' )

    def guiEventChar( self, ch, shift ):
        self.__event_queue.put( (self.inputChar, (ch, shift)) )

    def termCheckForInput( self ):
        pass

    def termWaitForActivity( self ):
        print 'waitForActivity',1
        try:
            while True:
                event_hander_and_args = self.__event_queue.getNoWait()
                if event_hander_and_args is not None:
                    handler, args = event_hander_and_args
                    handler( *args )
                    return 0
                else:
                    time.sleep( 0.1 )

        except Exception, e:
            print 'Error: waitForActivity %s' % (str(e),)
            self.app.debugShowCallers( 5 )
            return -1

    def termTopos( self, x, y ):
        self.app.onGuiThread( self.window.termTopos, (x, y) )

    def termReset( self ):
        self.app.onGuiThread( self.window.termReset, () )

    def termInit( self ):
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

    def termWindow( self, size ):
        self.app.onGuiThread( self.window.termWindow, (size,) )

    def termInsertMode( self, mode ):
        self.app.onGuiThread( self.window.termInsertMode, (mode,) )

    def termHighlightMode( self, mode ):
        self.app.onGuiThread( self.window.termHighlightMode, (mode,) )

    def termInsertLines( self, num_lines ):
        self.app.onGuiThread( self.window.termInsertLines, (num_lines,) )

    def termDeleteLines( self, num_lines ):
        self.app.onGuiThread( self.window.termDeleteLines, (num_lines,) )

    def termDisplayActivity( self, ch ):
        self.app.onGuiThread( self.window.termDisplayActivity, (ch,) )

class Queue:
    def __init__( self ):
        self.__all_items = []
        self.__lock = threading.RLock()

    def getNoWait( self ):
        with self.__lock:
            if len( self.__all_items ) > 0:
                return self.__all_items.pop( 0 )

        return None

    def put( self, item ):
        with self.__lock:
            self.__all_items.append( item )
