'''
 ====================================================================
 Copyright (c) 2003-2015 Barry A Scott.  All rights reserved.

 This software is licensed as described in the file LICENSE.txt,
 which you should have received as part of this distribution.

 ====================================================================

    be_debug.py

'''
import time

#
# be_app.py
#
_debug_app = False
_debug_callback = False

#
# be_editor
#
_debug_editor = False
_debug_queue = False

#
# be_emacs_panel.py
#
_debug_term_calls1 = False
_debug_term_calls2 = False
_debug_term_key = False
_debug_term_mouse = False
_debug_term_scroll = False
_debug_panel = False
_debug_speed = False

def setDebug( str_options ):
    for option in [s.strip().lower() for s in str_options.split(',')]:
        name = '_debug_%s' % (option,)
        if name in globals():
            globals()[ name ] = True
        else:
            print( 'Unknown debug option %s - see be_debug.py for available options' % (option,) )

class EmacsDebugMixin:
    def __init__( self ):
        self.__speed_start_time = time.time()
        self.__speed_last_event_time = self.__speed_start_time

    def _debugSpeed( self, msg, start_timer=False ):
        if _debug_speed:
            now = time.time()
            if start_timer:
                self.__speed_start_time = now
                self.__speed_last_event_time = now

            start_delta = now - self.__speed_start_time
            last_delta = now - self.__speed_last_event_time
            self.__speed_last_event_time = now

            self.log.debug( 'SPEED %.6f %.6f %s' % (start_delta, last_delta, msg,) )

    # be_app
    def _debugApp( self, msg ):
        if _debug_app:
            self.log.debug( 'APP %s' % (msg,) )

    def _debugCallback( self, msg ):
        if _debug_callback:
            self.log.debug( 'CALLBACK %s' % (msg,) )

    # be_emacs_panel
    def _debugTermCalls1( self, msg ):
        if _debug_term_calls1:
            self.log.debug( 'TERM %s' % (msg,) )

    def _debugTermCalls2( self, msg ):
        if _debug_term_calls2:
            self.log.debug( 'TERM %s' % (msg,) )

    def _debugTermKey( self, msg ):
        if _debug_term_key:
            self.log.debug( 'TERM %s' % (msg,) )

    def _debugTermMouse( self, msg ):
        if _debug_term_mouse:
            self.log.debug( 'TERM %s' % (msg,) )

    def _debugTermScroll( self, msg ):
        if _debug_term_scroll:
            self.log.debug( 'SCROLL %s' % (msg,) )

    def _debugPanel( self, msg ):
        if _debug_panel:
            self.log.debug( 'PANEL %s' % (msg,) )

    # be_editor
    def _debugEditor( self, msg ):
        if _debug_editor:
            self.log.debug( 'EDITOR %s' % (msg,) )

    def _debugQueue( self, msg ):
        if _debug_queue:
            self.log.debug( 'QUEUE %s' % (msg,) )

