'''
 ====================================================================
 Copyright (c) 2003-2015 Barry A Scott.  All rights reserved.

 This software is licensed as described in the file LICENSE.txt,
 which you should have received as part of this distribution.

 ====================================================================

    be_emacs_panel.py

'''
import sys
import logging

from PyQt5 import QtWidgets
from PyQt5 import QtGui
from PyQt5 import QtCore

#import be_ids
import be_exceptions
import be_version
import be_images
import be_debug
import be_config

def T( boolean ):
    if boolean:
        return 'T'
    else:
        return '_'

def B( n ):
    return 1 << n

MSCREENWIDTH            = 1024
MSCREENLENGTH           = 512

SYNTAX_DULL             = (        0  );        # 000 a dull (punctuation) character
SYNTAX_WORD             = (      B(1) );        # 002 a word character for ESC-F and friends

SYNTAX_STRING_SHIFT     = (        2  );
SYNTAX_STRING_MASK      = ( B(2)+B(3) );        # 00c its a string
SYNTAX_TYPE_STRING1     = ( B(2)      );        # 004 its a string type 1
SYNTAX_TYPE_STRING2     = (      B(3) );        # 008 its a string type 2
SYNTAX_TYPE_STRING3     = ( B(2)+B(3) );        # 00c its a string type 3

SYNTAX_COMMENT_SHIFT    = (        4  );
SYNTAX_COMMENT_MASK     = ( B(4)+B(5) );        # 030 its a comment
SYNTAX_TYPE_COMMENT1    = ( B(4)      );        # 010 its a comment type 1
SYNTAX_TYPE_COMMENT2    = (      B(5) );        # 020 its a comment type 2
SYNTAX_TYPE_COMMENT3    = ( B(4)+B(5) );        # 030 its a comment type 3

SYNTAX_KEYWORD_SHIFT    = (        6  );
SYNTAX_KEYWORD_MASK     = ( B(6)+B(7) );        # 0c0 its a keyword
SYNTAX_TYPE_KEYWORD1    = ( B(6)      );        # 040 its a keyword type 1
SYNTAX_TYPE_KEYWORD2    = (      B(7) );        # 080 its a keyword type 2
SYNTAX_TYPE_KEYWORD3    = ( B(6)+B(7) );        # 0c0 its a keyword type 3

SYNTAX_FIRST_FREE       = (      B(8) );        # 100
SYNTAX_PREFIX_QUOTE     = (      B(8) );        # 100 like \ in C
SYNTAX_BEGIN_PAREN      = (      B(9) );        # 200 a begin paren: (<[
SYNTAX_END_PAREN        = (     B(10) );        # 400 an end paren: )>]    end
SYNTAX_LAST_BIT         = (     B(10) );

LINE_ATTR_MODELINE      = SYNTAX_FIRST_FREE     # 100
LINE_ATTR_USER          = SYNTAX_FIRST_FREE<<1  # 200

LINE_M_ATTR_HIGHLIGHT   = SYNTAX_LAST_BIT       # 400
LINE_M_ATTR_USER        = LINE_ATTR_USER|(15)   # the 8 user colours

fg_colours = {
    SYNTAX_DULL:            QtGui.QColor(  0,  0,  0),
    LINE_M_ATTR_HIGHLIGHT:  QtGui.QColor(  0,  0,  0),
    SYNTAX_WORD:            QtGui.QColor(  0,  0,  0),
    SYNTAX_TYPE_STRING1:    QtGui.QColor(  0,128,  0),
    SYNTAX_TYPE_STRING2:    QtGui.QColor(  0,128,  0),
    SYNTAX_TYPE_STRING3:    QtGui.QColor(  0,128,  0),
    SYNTAX_TYPE_COMMENT1:   QtGui.QColor(  0,128,  0),
    SYNTAX_TYPE_COMMENT2:   QtGui.QColor(  0,128,  0),
    SYNTAX_TYPE_COMMENT3:   QtGui.QColor(  0,128,  0),
    SYNTAX_TYPE_KEYWORD1:   QtGui.QColor(  0,  0,255),
    SYNTAX_TYPE_KEYWORD2:   QtGui.QColor(255,  0,  0),
    SYNTAX_TYPE_KEYWORD3:   QtGui.QColor(255,  0,  0),
    LINE_ATTR_MODELINE:     QtGui.QColor(255,255,128),
    LINE_ATTR_USER+1:       QtGui.QColor(255,  0,  0),
    LINE_ATTR_USER+2:       QtGui.QColor(  0,255,  0),
    LINE_ATTR_USER+3:       QtGui.QColor(  0,  0,255),
    LINE_ATTR_USER+4:       QtGui.QColor(255,255,  0),
    LINE_ATTR_USER+5:       QtGui.QColor(255,  0,255),
    LINE_ATTR_USER+6:       QtGui.QColor(  0,255,255),
    LINE_ATTR_USER+7:       QtGui.QColor(255,255,255),
    LINE_ATTR_USER+8:       QtGui.QColor(255,255,255),
    }

bg_colours = {
    SYNTAX_DULL:            QtGui.QColor(255,255,255),
    LINE_M_ATTR_HIGHLIGHT:  QtGui.QColor(255,204,102),
    SYNTAX_WORD:            QtGui.QColor(255,255,255),
    SYNTAX_TYPE_STRING1:    QtGui.QColor(255,255,255),
    SYNTAX_TYPE_STRING2:    QtGui.QColor(255,255,255),
    SYNTAX_TYPE_STRING3:    QtGui.QColor(255,255,255),
    SYNTAX_TYPE_COMMENT1:   QtGui.QColor(255,255,255),
    SYNTAX_TYPE_COMMENT2:   QtGui.QColor(255,255,255),
    SYNTAX_TYPE_COMMENT3:   QtGui.QColor(255,255,255),
    SYNTAX_TYPE_KEYWORD1:   QtGui.QColor(255,255,255),
    SYNTAX_TYPE_KEYWORD2:   QtGui.QColor(255,255,255),
    SYNTAX_TYPE_KEYWORD3:   QtGui.QColor(000,255,255),
    LINE_ATTR_MODELINE:     QtGui.QColor(  0,  0,255),
    LINE_ATTR_USER+1:       QtGui.QColor(255,255,255),
    LINE_ATTR_USER+2:       QtGui.QColor(255,255,255),
    LINE_ATTR_USER+3:       QtGui.QColor(255,255,255),
    LINE_ATTR_USER+4:       QtGui.QColor(255,255,255),
    LINE_ATTR_USER+5:       QtGui.QColor(255,255,255),
    LINE_ATTR_USER+6:       QtGui.QColor(255,255,255),
    LINE_ATTR_USER+7:       QtGui.QColor(192,192,192),
    LINE_ATTR_USER+8:       QtGui.QColor(255,255,255),
    }

default_binding =   '\uef00'
prefix_key =        '\uef01'
prefix_mouse =      '\uef02'
prefix_menu =       '\uef03'
key_base =          0xef20
mouse_base =        0xef20

def __nextKeyMapping():
    global key_base
    mapping = prefix_key + chr( key_base )
    key_base += 1
    return mapping

def __nextMouseMapping():
    global mouse_base
    mapping = prefix_mouse + chr( mouse_base )
    mouse_base += 1
    return mapping

keys_mapping = {
    'default':                      default_binding,
    'key-prefix':                   prefix_key,
    'mouse-prefix':                 prefix_mouse,
    'menu-prefix':                  prefix_menu,
    'menu':                         prefix_menu,

    'ss3':                          '\033O',
    'csi':                          '\033[',

    'tab':                          '\t',
    'shift-tab':                    __nextKeyMapping(),

    'backspace':                    '\x7f',
    'ctrl-backspace':               __nextKeyMapping(),

    'mouse-1-down':                 __nextMouseMapping(),
    'mouse-1-up':                   __nextMouseMapping(),
    'mouse-2-down':                 __nextMouseMapping(),
    'mouse-2-up':                   __nextMouseMapping(),
    'mouse-3-down':                 __nextMouseMapping(),
    'mouse-3-up':                   __nextMouseMapping(),
    'mouse-4-down':                 __nextMouseMapping(),
    'mouse-4-up':                   __nextMouseMapping(),

    'mouse-motion':                 __nextMouseMapping(),

    'mouse-wheel-neg':              __nextMouseMapping(),
    'ctrl-mouse-wheel-neg':         __nextMouseMapping(),
    'shift-mouse-wheel-neg':        __nextMouseMapping(),
    'ctrl-shift-mouse-wheel-neg':   __nextMouseMapping(),

    'mouse-wheel-pos':              __nextMouseMapping(),
    'ctrl-mouse-wheel-pos':         __nextMouseMapping(),
    'shift-mouse-wheel-pos':        __nextMouseMapping(),
    'ctrl-shift-mouse-wheel-pos':   __nextMouseMapping(),

    'insert':                       __nextKeyMapping(),
    'ctrl-insert':                  __nextKeyMapping(),
    'shift-insert':                 __nextKeyMapping(),
    'ctrl-shift-insert':            __nextKeyMapping(),

    'delete':                       __nextKeyMapping(),
    'ctrl-delete':                  __nextKeyMapping(),
    'shift-delete':                 __nextKeyMapping(),
    'ctrl-shift-delete':            __nextKeyMapping(),

    'home':                         __nextKeyMapping(),
    'ctrl-home':                    __nextKeyMapping(),
    'shift-home':                   __nextKeyMapping(),
    'ctrl-shift-home':              __nextKeyMapping(),

    'end':                          __nextKeyMapping(),
    'ctrl-end':                     __nextKeyMapping(),
    'shift-end':                    __nextKeyMapping(),
    'ctrl-shift-end':               __nextKeyMapping(),

    'up':                           __nextKeyMapping(),
    'ctrl-up':                      __nextKeyMapping(),
    'shift-up':                     __nextKeyMapping(),
    'ctrl-shift-up':                __nextKeyMapping(),

    'down':                         __nextKeyMapping(),
    'ctrl-down':                    __nextKeyMapping(),
    'shift-down':                   __nextKeyMapping(),
    'ctrl-shift-down':              __nextKeyMapping(),

    'left':                         __nextKeyMapping(),
    'ctrl-left':                    __nextKeyMapping(),
    'shift-left':                   __nextKeyMapping(),
    'ctrl-shift-left':              __nextKeyMapping(),

    'right':                        __nextKeyMapping(),
    'ctrl-right':                   __nextKeyMapping(),
    'shift-right':                  __nextKeyMapping(),
    'ctrl-shift-right':             __nextKeyMapping(),

    'page-down':                    __nextKeyMapping(),
    'ctrl-page-down':               __nextKeyMapping(),
    'shift-page-down':              __nextKeyMapping(),
    'ctrl-shift-page-down':         __nextKeyMapping(),

    'page-up':                      __nextKeyMapping(),
    'ctrl-page-up':                 __nextKeyMapping(),
    'shift-page-up':                __nextKeyMapping(),
    'ctrl-shift-page-up':           __nextKeyMapping(),

    'f1':                           __nextKeyMapping(),
    'ctrl-f1':                      __nextKeyMapping(),
    'shift-f1':                     __nextKeyMapping(),
    'ctrl-shift-f1':                __nextKeyMapping(),

    'f2':                           __nextKeyMapping(),
    'ctrl-f2':                      __nextKeyMapping(),
    'shift-f2':                     __nextKeyMapping(),
    'ctrl-shift-f2':                __nextKeyMapping(),

    'f3':                           __nextKeyMapping(),
    'ctrl-f3':                      __nextKeyMapping(),
    'shift-f3':                     __nextKeyMapping(),
    'ctrl-shift-f3':                __nextKeyMapping(),

    'f4':                           __nextKeyMapping(),
    'ctrl-f4':                      __nextKeyMapping(),
    'shift-f4':                     __nextKeyMapping(),
    'ctrl-shift-f4':                __nextKeyMapping(),

    'f5':                           __nextKeyMapping(),
    'ctrl-f5':                      __nextKeyMapping(),
    'shift-f5':                     __nextKeyMapping(),
    'ctrl-shift-f5':                __nextKeyMapping(),

    'f6':                           __nextKeyMapping(),
    'ctrl-f6':                      __nextKeyMapping(),
    'shift-f6':                     __nextKeyMapping(),
    'ctrl-shift-f6':                __nextKeyMapping(),

    'f7':                           __nextKeyMapping(),
    'ctrl-f7':                      __nextKeyMapping(),
    'shift-f7':                     __nextKeyMapping(),
    'ctrl-shift-f7':                __nextKeyMapping(),

    'f8':                           __nextKeyMapping(),
    'ctrl-f8':                      __nextKeyMapping(),
    'shift-f8':                     __nextKeyMapping(),
    'ctrl-shift-f8':                __nextKeyMapping(),

    'f9':                           __nextKeyMapping(),
    'ctrl-f9':                      __nextKeyMapping(),
    'shift-f9':                     __nextKeyMapping(),
    'ctrl-shift-f9':                __nextKeyMapping(),

    'f10':                          __nextKeyMapping(),
    'ctrl-f10':                     __nextKeyMapping(),
    'shift-f10':                    __nextKeyMapping(),
    'ctrl-shift-f10':               __nextKeyMapping(),

    'f11':                          __nextKeyMapping(),
    'ctrl-f11':                     __nextKeyMapping(),
    'shift-f11':                    __nextKeyMapping(),
    'ctrl-shift-f11':               __nextKeyMapping(),

    'f12':                          __nextKeyMapping(),
    'ctrl-f12':                     __nextKeyMapping(),
    'shift-f12':                    __nextKeyMapping(),
    'ctrl-shift-f12':               __nextKeyMapping(),

    'pause':                        __nextKeyMapping(),
    'print-screen':                 __nextKeyMapping(),
    'scroll-lock':                  __nextKeyMapping(),

    'kp-divide':                    __nextKeyMapping(),
    'kp-dot':                       __nextKeyMapping(),
    'kp-enter':                     __nextKeyMapping(),
    'kp-minus':                     __nextKeyMapping(),
    'kp-multiple':                  __nextKeyMapping(),
    'kp-plus':                      __nextKeyMapping(),
    'kp0':                          __nextKeyMapping(),
    'kp1':                          __nextKeyMapping(),
    'kp2':                          __nextKeyMapping(),
    'kp3':                          __nextKeyMapping(),
    'kp4':                          __nextKeyMapping(),
    'kp5':                          __nextKeyMapping(),
    'kp6':                          __nextKeyMapping(),
    'kp7':                          __nextKeyMapping(),
    'kp8':                          __nextKeyMapping(),
    'kp9':                          __nextKeyMapping(),
    'num-lock':                     __nextKeyMapping(),
}

cmd_to_ctrl_map = {
    ord('@'):   0,
    ord('a'):   1,
    ord('b'):   2,
    ord('c'):   3,
    ord('d'):   4,
    ord('e'):   5,
    ord('f'):   6,
    ord('g'):   7,
    ord('h'):   8,
    ord('i'):   9,
    ord('j'):   10,
    ord('k'):   11,
    ord('l'):   12,
    ord('m'):   13,
    ord('n'):   14,
    ord('o'):   15,
    ord('p'):   16,
    ord('q'):   17,
    ord('r'):   18,
    ord('s'):   19,
    ord('t'):   20,
    ord('u'):   21,
    ord('v'):   22,
    ord('w'):   23,
    ord('x'):   24,
    ord('y'):   25,
    ord('z'):   26,
    ord('['):   27,
    ord('\\'):  28,
    ord(']'):   29,
    ord('^'):   30,
    ord('_'):   31,
    ord('A'):   1,
    ord('B'):   2,
    ord('C'):   3,
    ord('D'):   4,
    ord('E'):   5,
    ord('F'):   6,
    ord('G'):   7,
    ord('H'):   8,
    ord('I'):   9,
    ord('J'):   10,
    ord('K'):   11,
    ord('L'):   12,
    ord('M'):   13,
    ord('N'):   14,
    ord('O'):   15,
    ord('P'):   16,
    ord('Q'):   17,
    ord('R'):   18,
    ord('S'):   19,
    ord('T'):   20,
    ord('U'):   21,
    ord('V'):   22,
    ord('W'):   23,
    ord('X'):   24,
    ord('Y'):   25,
    ord('Z'):   26,
    }

special_keys = {
#   Key code                    trans          shift_trans      ctrl_trans         ctrl_shift_trans
    QtCore.Qt.Key_Backspace:    ('backspace',  None,            'ctrl-backspace',  None),
    QtCore.Qt.Key_Tab:          ('tab',        'shift-tab',     'tab',             'shift-tab'),

    # function keys
    QtCore.Qt.Key_F1:           ('f1',         'shift-f1',      'ctrl-f1',         'ctrl-shift-f1'),
    QtCore.Qt.Key_F2:           ('f2',         'shift-f2',      'ctrl-f2',         'ctrl-shift-f2'),
    QtCore.Qt.Key_F3:           ('f3',         'shift-f3',      'ctrl-f3',         'ctrl-shift-f3'),
    QtCore.Qt.Key_F4:           ('f4',         'shift-f4',      'ctrl-f4',         'ctrl-shift-f4'),
    QtCore.Qt.Key_F5:           ('f5',         'shift-f5',      'ctrl-f5',         'ctrl-shift-f5'),
    QtCore.Qt.Key_F6:           ('f6',         'shift-f6',      'ctrl-f6',         'ctrl-shift-f6'),
    QtCore.Qt.Key_F7:           ('f7',         'shift-f7',      'ctrl-f7',         'ctrl-shift-f7'),
    QtCore.Qt.Key_F8:           ('f8',         'shift-f8',      'ctrl-f8',         'ctrl-shift-f8'),
    QtCore.Qt.Key_F9:           ('f9',         'shift-f9',      'ctrl-f9',         'ctrl-shift-f9'),
    QtCore.Qt.Key_F10:          ('f10',        'shift-f10',     'ctrl-f10',        'ctrl-shift-f10'),
    QtCore.Qt.Key_F11:          ('f11',        'shift-f11',     'ctrl-f11',        'ctrl-shift-f11'),
    QtCore.Qt.Key_F12:          ('f12',        'shift-f12',     'ctrl-f12',        'ctrl-shift-f12'),

    # enhanced keys
    QtCore.Qt.Key_PageUp:       ('page-up',    None,            'ctrl-page-up',    None),
    QtCore.Qt.Key_PageDown:     ('page-down',  None,            'ctrl-page-down',  None),
    QtCore.Qt.Key_End:          ('end',        None,            'ctrl-end',        None),
    QtCore.Qt.Key_Home:         ('home',       None,            'ctrl-home',       None),

    QtCore.Qt.Key_Left:         ('left',       None,            'ctrl-left',       None),
    QtCore.Qt.Key_Up:           ('up',         None,            'ctrl-up',         None),
    QtCore.Qt.Key_Right:        ('right',      None,            'ctrl-right',      None),
    QtCore.Qt.Key_Down:         ('down',       None,            'ctrl-down',       None),

    QtCore.Qt.Key_Insert:       ('insert',     'shift-insert',  'ctrl-insert',     'ctrl-shift-insert'),
    QtCore.Qt.Key_Delete:       ('delete',     'shift-delete',  'ctrl-delete',     'ctrl-shift-delete'),
    }

qt_key_names = {}
for name in dir(QtCore.Qt):
    if name.startswith( 'Key_' ):
        qt_key_names[ getattr( QtCore.Qt, name ) ] = name

class EmacsPanel(QtWidgets.QWidget, be_debug.EmacsDebugMixin):
    def __init__( self, app, parent ):
        QtWidgets.QWidget.__init__( self, parent )
        be_debug.EmacsDebugMixin.__init__( self )

        self.__debug_save_image_index = 0

        self.app = app
        self.log = app.log

        self._debugPanel( '__init__()' )

        self.setFocusPolicy( QtCore.Qt.StrongFocus )

        self.tw = be_exceptions.TryWrapperFactory( self.log )


        self.qp = None
        self.first_paint = True
        self.__all_term_ops = []
        self.editor_pixmap = None

        #qqq#self.eat_next_char = False
        self.cursor_x = 1
        self.cursor_y = 1
        self.window_size = 0

        self.all_vert_scroll_bars = []
        self.all_vert_scroll_bar_info = []
        self.all_horz_scroll_bars = []
        self.all_horz_scroll_bar_info = []

        self.__mouse_button_state = set()

        self.font = None

        self.client_padding = 3

        # the size of a char on the screen
        self.char_width = None
        self.char_length = None
        self.char_ascent = None

        # the size of the window
        self.pixel_width = None
        self.pixel_length = None
        # the size in chars of the window
        self.term_width = None
        self.term_length = None

        self.__pending_geometryChanged = False

        self.__initFromPreferences()
        self.__initFont()

    def newPreferences( self ):
        self.__initFromPreferences()
        self.__initFont()

        self.__geometryChanged()

        self.update( 0, 0, self.pixel_width, self.pixel_length )

    def __initFromPreferences( self ):
        font_pref = self.app.prefs.getFont()

        self.font = QtGui.QFont( font_pref.face, font_pref.point_size )
        self.log.info( 'Font family: %r %dpt' % (self.font.family(), self.font.pointSize()) )

    def __calculateWindowSize( self ):
        assert self.char_width is not None

        self.pixel_width = self.width()
        self.pixel_length = self.height()

        self.term_width  = min( MSCREENWIDTH,  (self.pixel_width  - 2*self.client_padding) // self.char_width )
        self.term_length = min( MSCREENLENGTH, (self.pixel_length - 2*self.client_padding) // self.char_length )

        self._debugPanel( '__calculateWindowSize char: %dpx x %dpx window: %dpx X %dpx -> text window: %d X %d' %
                        (self.char_width, self.char_length
                        ,self.pixel_width, self.pixel_length
                        ,self.term_width, self.term_length) )

        self._debugPanel( '__calculateWindowSize create editor_pixmap %d x %d' % (self.pixel_width, self.pixel_length) )
        self.editor_pixmap = QtGui.QPixmap( self.pixel_width, self.pixel_length )

        bg_brush = QtGui.QBrush( bg_colours[ SYNTAX_DULL ] )
        qp = QtGui.QPainter( self.editor_pixmap )
        qp.setBackgroundMode( QtCore.Qt.OpaqueMode )
        qp.setBackground( bg_brush )
        qp.fillRect( 0, 0, self.pixel_width, self.pixel_length, bg_brush )
        del qp

    def __pixelPoint( self, x, y ):
        return  (self.client_padding + self.char_width  * (x-1)
                ,self.client_padding + self.char_length * (y-1))
        

    def __geometryChanged( self ):
        self.__pending_geometryChanged = False

        self.__calculateWindowSize()

        if self.app.editor is None:
            self._debugPanel( '__geometryChanged no self.app.editor' )
            return

        if self.char_width is None:
            self._debugPanel( '__geometryChanged self.char_width is None' )
            return

        self.app.editor.guiGeometryChange( self.term_width, self.term_length )

    #--------------------------------------------------------------------------------
    #
    #   Event handlers
    #
    #--------------------------------------------------------------------------------
    def __initFont( self ):
        metrics = QtGui.QFontMetrics( self.font )

        self.char_width = metrics.width( 'i' )
        self.char_length = metrics.height()
        self._debugPanel( 'paintEvent first_paint i %d.%d' % (self.char_width, self.char_length) )

        self.char_width = metrics.width( 'M' )
        self.char_length = metrics.height()
        self._debugPanel( 'paintEvent first_paint M %d.%d' % (self.char_width, self.char_length) )

        self.char_ascent = metrics.ascent()

    def paintEvent( self, event ):
        self._debugSpeed( 'paintEvent() begin' )

        if self.first_paint:
            self._debugPanel( 'EmacsPanel.paintEvent() first paint' )
            self.first_paint = False

            qp = QtGui.QPainter( self )
            qp.fillRect( QtCore.QRect( 0, 0, self.pixel_width, self.pixel_length ), bg_colours[ SYNTAX_DULL ] )
            del qp

            self.__calculateWindowSize()

            # queue up this action until after the rest of GUI init has happend
            self.app.marshallToGuiThread( self.app.onEmacsPanelReady, () )

        elif self.editor_pixmap is not None:
            self._debugPanel( 'EmacsPanel.paintEvent() editor_pixmap %r' % (self.editor_pixmap,) )

            qp = QtGui.QPainter( self )

            qp.setBackgroundMode( QtCore.Qt.OpaqueMode )

            self._debugSpeed( 'drawPixmap() start' )
            qp.drawPixmap( 0, 0, self.pixel_width, self.pixel_length, self.editor_pixmap )
            self._debugSpeed( 'drawPixmap() end %d x %d' % (self.pixel_width, self.pixel_length) )

            c_x, c_y = self.__pixelPoint( self.cursor_x, self.cursor_y )

            # alpha blend the cursor
            cursor_colour = QtGui.QColor( 255, 0, 0, 64 )
            qp.fillRect( c_x, c_y, self.char_width, self.char_length, cursor_colour )

            # Draw scroll bar backgrounds
            # set the colour behind the scroll bars
            G = 240
            scroll_bar_bg = QtGui.QColor( G, G, G, 0 )

            #--- vert_scroll -----------------------------------------------------------
            for bar_info in self.all_vert_scroll_bar_info:
                if bar_info is None:
                    continue

                win_id, x, y, width, height, pos, total = bar_info
                x, y = self.__pixelPoint( x+1, y+1 )
                qp.fillRect( x, y, self.char_width * width, self.char_length * height, scroll_bar_bg )

            #--- horz_scroll -----------------------------------------------------------
            for bar_info in self.all_horz_scroll_bar_info:
                if bar_info is None:
                    continue

                win_id, x, y, width, height, pos = bar_info
                x, y = self.__pixelPoint( x+1, y+1 )
                qp.fillRect( x, y, self.char_width * width, self.char_length * height, scroll_bar_bg )

            del qp

            self._debugSpeed( 'at end' )

        else:
            self._debugPanel( 'EmacsPanel.paintEvent() Nothing to do' )

        self._debugSpeed( 'paintEvent() end' )

    def getKeysMapping( self ):
        return keys_mapping

    def resizeEvent( self, event ):
        self._debugPanel( 'EmacsPanel.resizeEvent()' )

        self.__geometryChanged()

    def keyPressEvent( self, event ):
        key = event.key()
        modifiers = int( event.modifiers() )

        # see http://doc.qt.io/qt-5/qt.html#KeyboardModifier-enum for details of mappings
        if sys.platform == 'darwin':
            shift = (modifiers & QtCore.Qt.ShiftModifier) != 0
            ctrl = (modifiers & QtCore.Qt.MetaModifier) != 0
            alt = (modifiers & QtCore.Qt.AltModifier) != 0
            meta = (modifiers & QtCore.Qt.MetaModifier) != 0
            cmd = (modifiers & QtCore.Qt.ControlModifier) != 0

        else:
            shift = (modifiers & QtCore.Qt.ShiftModifier) != 0
            ctrl = (modifiers & QtCore.Qt.ControlModifier) != 0
            alt = (modifiers & QtCore.Qt.AltModifier) != 0
            meta = (modifiers & QtCore.Qt.MetaModifier) != 0
            cmd = False

        self._debugTermKey( 'keyPressEvent %r key %d(0x%x) name %r  ctrl %s shift %s alt %s cmd %s meta %s' %
                            (event.text(), key, key, qt_key_names.get( key, 'unknown' ), T( ctrl ), T( shift ), T( alt ), T( cmd ), T( meta )) )

        if key in special_keys:
            trans, shift_trans, ctrl_trans, ctrl_shift_trans = special_keys[ key ]

            if ctrl and shift and ctrl_shift_trans is not None:
                translation = ctrl_shift_trans
                shift = False

            elif ctrl:
                translation = ctrl_trans

            elif shift and shift_trans is not None:
                translation = shift_trans
                shift = False

            else:
                translation = trans

            translation = keys_mapping[ translation ]

            for ch in translation:
                self.app.editor.guiEventChar( ch, shift )


            #qqq# Not needed now?
            #qqq# self.eat_next_char = True

        elif len(event.text()) > 0:
            self.handleNonSpecialKey( event )

    def keyReleaseEvent( self, event ):
        key = event.key()
        modifiers = int( event.modifiers() )
        # see http://doc.qt.io/qt-5/qt.html#KeyboardModifier-enum for details of mappings
        if sys.platform == 'darwin':
            shift = (modifiers & QtCore.Qt.ShiftModifier) != 0
            ctrl = (modifiers & QtCore.Qt.MetaModifier) != 0
            alt = (modifiers & QtCore.Qt.AltModifier) != 0
            meta = (modifiers & QtCore.Qt.MetaModifier) != 0
            cmd = (modifiers & QtCore.Qt.ControlModifier) != 0

        else:
            shift = (modifiers & QtCore.Qt.ShiftModifier) != 0
            ctrl = (modifiers & QtCore.Qt.ControlModifier) != 0
            alt = (modifiers & QtCore.Qt.AltModifier) != 0
            meta = (modifiers & QtCore.Qt.MetaModifier) != 0
            cmd = False

        self._debugTermKey( 'keyReleaseEvent %r key %d(0x%x) name %r ctrl %s shift %s alt %s cmd %s meta %s' %
                            (event.text(), key, key, qt_key_names.get( key, 'unknown' ), T( ctrl ), T( shift ), T( alt ), T( cmd ), T( meta )) )

    def handleNonSpecialKey( self, event ):
        #qqq#if self.eat_next_char:
        #qqq#    self._debugTermKey( 'handleNonSpecialKey eat_next_char' )

        #qqq#    self.eat_next_char = False
        #qqq#    return

        key = event.key()
        char = event.text()
        modifiers = int( event.modifiers() )
        # see http://doc.qt.io/qt-5/qt.html#KeyboardModifier-enum for details of mappings
        if sys.platform == 'darwin':
            shift = (modifiers & QtCore.Qt.ShiftModifier) != 0
            ctrl = (modifiers & QtCore.Qt.MetaModifier) != 0
            alt = (modifiers & QtCore.Qt.AltModifier) != 0
            meta = (modifiers & QtCore.Qt.MetaModifier) != 0
            cmd = (modifiers & QtCore.Qt.ControlModifier) != 0

        else:
            shift = (modifiers & QtCore.Qt.ShiftModifier) != 0
            ctrl = (modifiers & QtCore.Qt.ControlModifier) != 0
            alt = (modifiers & QtCore.Qt.AltModifier) != 0
            meta = (modifiers & QtCore.Qt.MetaModifier) != 0
            cmd = False

        self._debugTermKey( ('handleNonSpecialKey "%r" key %r name %r ctrl %s shift %s alt %s cmd %s meta %s' %
                            (char, key, qt_key_names.get( key, 'unknown' ), T( ctrl ), T( shift ), T( alt ), T( cmd ), T( meta ))) )

        if( sys.platform == 'darwin'
        and (cmd or ctrl)
        and char in cmd_to_ctrl_map ):
            self._debugTermKey( 'mapped %d to %d' % (char, cmd_to_ctrl_map[ char ]) )
            char = cmd_to_ctrl_map[ char ]

        elif( sys.platform.startswith( 'linux' )
        and ctrl
        and char in cmd_to_ctrl_map):
            char = cmd_to_ctrl_map[ char ]

        if ctrl and char == ord( ' ' ):
            char = 0

        self._debugSpeed( 'handleNonSpecialKey( %r )' % (char,), True )
        self.app.editor.guiEventChar( char, False )

    def mousePressEvent( self, event ):
        self._debugTermMouse( 'mousePressEvent   %r %r %r )' % (event.button(), int(event.buttons()), event.pos()) )

        if self.char_width is None:
            return

        # Calculate character cell position
        column = (event.x() - self.client_padding + (self.char_width/2)) // self.char_width + 1;
        line =   (event.y() - self.client_padding ) // self.char_length + 1;

        if event.button() == QtCore.Qt.LeftButton:
            translation = keys_mapping["mouse-1-down"]
            self.__mouse_button_state.add( 1 )

        elif event.button() == QtCore.Qt.MidButton:
            translation = keys_mapping["mouse-2-down"]
            self.__mouse_button_state.add( 2 )

        elif event.button() == QtCore.Qt.RightButton:
            translation = keys_mapping["mouse-3-down"]
            self.__mouse_button_state.add( 3 )

        if translation is not None:
            modifiers = int(self.app.keyboardModifiers())
            shift = (modifiers & QtCore.Qt.ShiftModifier) != 0
            self.app.editor.guiEventMouse( translation, shift, [line, column, shift] );

    def mouseReleaseEvent( self, event ):
        self._debugTermMouse( 'mouseReleaseEvent %r %r %r )' % (event.button(), int(event.buttons()), event.pos()) )

        if self.char_width is None:
            return

        column = (event.x() - self.client_padding + (self.char_width/2)) // self.char_width + 1;
        line =   (event.y() - self.client_padding ) // self.char_length + 1;

        if event.button() == QtCore.Qt.LeftButton:
            translation = keys_mapping["mouse-1-up"]
            self.__mouse_button_state.remove( 1 )

        elif event.button() == QtCore.Qt.MidButton:
            translation = keys_mapping["mouse-2-up"]
            self.__mouse_button_state.remove( 2 )

        elif event.button() == QtCore.Qt.RightButton:
            translation = keys_mapping["mouse-3-up"]
            self.__mouse_button_state.remove( 3 )

        if translation is not None:
            modifiers = int(self.app.keyboardModifiers())
            shift = (modifiers & QtCore.Qt.ShiftModifier) != 0
            self.app.editor.guiEventMouse( translation, shift, [line, column, shift] );

    def mouseMoveEvent( self, event ):
        self._debugTermMouse( 'mouseMoveEvent %r %r %r )' % (event.button(), int(event.buttons()), event.pos()) )

        if self.char_width is None:
            return

        column = (event.x() - self.client_padding + (self.char_width/2)) // self.char_width + 1;
        line =   (event.y() - self.client_padding ) // self.char_length + 1;

        translation = keys_mapping["mouse-motion"]

        modifiers = int(self.app.keyboardModifiers())
        shift = (modifiers & QtCore.Qt.ShiftModifier) != 0
        self.app.editor.guiEventMouse( translation, shift, [line, column] );

    def wheelEvent( self, event ):
        self._debugTermMouse( 'wheelEvent angle %r )' % (event.angleDelta().y(),) )

        if event.angleDelta().y() > 0:
            rotation = 1
        else:
            rotation = -1

        column = (event.x() - self.client_padding + (self.char_width/2)) // self.char_width + 1;
        line =   (event.y() - self.client_padding ) // self.char_length + 1;

        modifiers = int(self.app.keyboardModifiers())
        shift = (modifiers & QtCore.Qt.ShiftModifier) != 0
        control = (modifiers & QtCore.Qt.ControlModifier) != 0

        if shift and control:
            if rotation < 0:
                translation = keys_mapping["ctrl-shift-mouse-wheel-neg"]
            else:
                translation = keys_mapping["ctrl-shift-mouse-wheel-pos"]

        elif shift:
            if rotation < 0:
                translation = keys_mapping["shift-mouse-wheel-neg"]
            else:
                translation = keys_mapping["shift-mouse-wheel-pos"]

        elif control:
            if rotation < 0:
                translation = keys_mapping["ctrl-mouse-wheel-neg"]
            else:
                translation = keys_mapping["ctrl-mouse-wheel-pos"]

        else:
            if rotation < 0:
                translation = keys_mapping["mouse-wheel-neg"]
            else:
                translation = keys_mapping["mouse-wheel-pos"]

        self.app.editor.guiEventMouse( translation, shift, [abs(rotation), line, column] );

    #--------------------------------------------------------------------------------
    #
    #   terminal drawing API forwarded from bemacs editor
    #
    #--------------------------------------------------------------------------------
    def hookUserInterface( self, *args ):
        self.tw( self.editor.hookUserInterface )( *args )

    #--------------------------------------------------------------------------------
    #
    #   terminal drawing API forwarded from bemacs editor
    #
    #--------------------------------------------------------------------------------
    def termTopos( self, y, x ):
        self._debugTermCalls1( 'termTopos( y=%d, x=%d)' % (y, x) )
        self.cursor_x = x
        self.cursor_y = y

    def termReset( self ):
        self._debugTermCalls1( 'termReset()' )
        #self.__all_term_ops = [(self.__termReset, ())]
        #self.RefreshRect( (0,0,self.pixel_width,self.pixel_length), True )

    def __termReset( self ):
        self.qp.Clear()

    def termInit( self ):
        self._debugTermCalls1( 'termInit()' )

    def termBeep( self ):
        self._debugTermCalls1( 'termBeep()' )

    def termUpdateBegin( self ):
        self._debugSpeed( 'termUpdateBegin()' )
        self._debugPanel( 'termUpdateBegin() ------------------------------------------------------------' )

        # discard any ops that have not been processed
        self._debugPanel( 'num ops %d' % (len(self.__all_term_ops),) )
        self.__all_term_ops = []

        self.__all_term_ops.append( (self.__termUpdateBegin, ()) )

    def __termUpdateBegin( self ):
        self._debugTermCalls2( '__termUpdateBegin() ----------------------------------------------------------' )

    def termUpdateEnd( self, all_status_bar_values, all_horz_scroll_bars, all_vert_scroll_bars ):
        self._debugSpeed( 'termUpdateEnd() begin' )
        self._debugPanel( 'termUpdateEnd() start --------------------------------------------------------' )
        if self.editor_pixmap is None:
            self._debugTermCalls1( 'termUpdateEnd editor_pixmap is None' )
            return

        self._debugTermCalls1( 'termUpdateEnd() all_horz_scroll_bars %r' % (all_horz_scroll_bars,) )
        self._debugTermCalls1( 'termUpdateEnd() all_vert_scroll_bars %r' % (all_vert_scroll_bars,) )

        self.qp = QtGui.QPainter( self.editor_pixmap )
        self.qp.setBackground( bg_colours[ SYNTAX_DULL ] )
        self.qp.setBackgroundMode( QtCore.Qt.OpaqueMode )
        self.qp.setFont( self.font )
        self.qp.setPen( fg_colours[ SYNTAX_DULL ] )

        self.__executeTermOperations()

        c_x, c_y = self.__pixelPoint( self.cursor_x, self.cursor_y )

        self.app.setStatus( all_status_bar_values )

        if True:
            #--- vert_scroll -----------------------------------------------------------
            self.all_vert_scroll_bar_info = all_vert_scroll_bars

            index = 0

            for index, bar_info in enumerate( all_vert_scroll_bars ):
                if len( self.all_vert_scroll_bars ) <= index:
                    self.all_vert_scroll_bars.append( BemacsVerticalScrollBar( self ) )

                bar = self.all_vert_scroll_bars[ index ]
                if bar_info is None:
                    bar.hide()
                    self._debugTermCalls1( 'termUpdateEnd: v scroll hide %d' % (index,) )

                else:
                    win_id, x, y, width, height, pos, total = bar_info
                    bar.setWindowId( win_id )
                    bar.setMaximum( total )
                    bar.setValue( pos )

                    x, y = self.__pixelPoint( x+1, y+1 )

                    self.qp.fillRect( x, y, self.char_width * width, self.char_length * height, QtGui.QColor( 192, 192, 192 ) )

                    bar.resize( self.char_width * width, self.char_length * height - 2 )
                    bar.move( x, y+1 )

                    bar.show()
                    self._debugTermCalls1( 'termUpdateEnd: v scroll show %d' % (index,) )

            index += 1
            while index < len( self.all_vert_scroll_bars ):
                self.all_vert_scroll_bars[ index ].hide()
                self._debugTermCalls1( 'termUpdateEnd: h scroll hide %d extra' % (index,) )
                index += 1

            #--- horz_scroll -----------------------------------------------------------
            self.all_horz_scroll_bar_info = all_horz_scroll_bars
            for index, bar_info in enumerate( all_horz_scroll_bars ):
                if len( self.all_horz_scroll_bars ) <= index:
                    self.all_horz_scroll_bars.append( BemacsHorizontalScrollBar( self ) )

                bar = self.all_horz_scroll_bars[ index ]
                if bar_info is None:
                    bar.hide()
                    self._debugTermCalls1( 'termUpdateEnd: h scroll hide %d' % (index,) )

                else:
                    win_id, x, y, width, height, pos = bar_info
                    self._debugTermScroll( 'termUpdateEnd set scroll id %r value %r' % (win_id, pos) )
                    bar.setWindowId( win_id )
                    bar.setValue( pos )

                    x, y = self.__pixelPoint( x+1, y+1 )

                    self.qp.fillRect( x, y, self.char_width * width, self.char_length * height, QtGui.QColor( 192, 192, 192 ) )

                    bar.resize( self.char_width * width - 2, self.char_length * height )
                    bar.move( x+1, y )

                    bar.show()
                    self._debugTermCalls1( 'termUpdateEnd: h scroll show %d' % (index,) )

            index += 1
            while index < len( self.all_horz_scroll_bars ):
                self.all_horz_scroll_bars[ index ].hide()
                self._debugTermCalls1( 'termUpdateEnd: v scroll hode %d extra' % (index,) )
                index += 1

        del self.qp

        self.__debug_save_image_index += 1
        #qqq#self.editor_pixmap.save( '/home/barry/tmpdir/image-%03d.png' % (self.__debug_save_image_index,), 'PNG' )


        self.update( 0, 0, self.pixel_width, self.pixel_length )

        self._debugTermCalls1( 'termUpdateEnd() done ---------------------------------------------------------' )
        self._debugSpeed( 'termUpdateEnd() end' )

    def isTermUpdatePending( self ):
        return len(self.__all_term_ops) > 0

    def __executeTermOperations( self ):
        all_term_ops = self.__all_term_ops
        self.__all_term_ops = []

        for fn, args in all_term_ops:
            fn( *args )

    def termUpdateLine( self, old, new, row ):
        self._debugTermCalls2( 'termUpdateLine row=%d' % (row,) )
        if old != new:
            self.__all_term_ops.append( (self.__termUpdateLine, (old, new, row)) )

    def __termUpdateLine( self, old, new, row ):
        if old is not None:
            self._debugTermCalls2( '__termUpdateLine row=%d old %r' % (row, old[0].rstrip(),) )
        self._debugTermCalls2( '__termUpdateLine row=%d new %r' % (row, new[0].rstrip(),) )

        if new is None:
            new_line_contents = ''
            new_line_attrs = []

        else:
            new_line_contents = new[0]
            new_line_attrs = new[1]

        if old is None:
            old_line_contents = ''
            old_line_attrs = []

        else:
            old_line_contents = old[0]
            old_line_attrs = old[1]

        cur_attr = None
        cur_mode = None

        new_line_length = len( new_line_contents )
        old_line_length = len( old_line_contents )

        # optimise drawing by finding runs of chars with the same fg and bg.
        draw_chars = []
        draw_cols = []
        draw_modes = []

        for col in range( len( new_line_contents ) ):
            new_attr = new_line_attrs[ col ]
            new_ch = new_line_contents[ col ]
            if( col < len(old_line_contents) 
            and old_line_contents[ col ] == new_ch
            and old_line_attrs[ col ] == new_attr ):
                continue

            if cur_attr != new_attr:
                if (new_attr & LINE_M_ATTR_HIGHLIGHT) != 0:
                    mode = LINE_M_ATTR_HIGHLIGHT

                elif (new_attr & LINE_ATTR_MODELINE) != 0:
                    mode = LINE_ATTR_MODELINE

                elif (new_attr & LINE_ATTR_USER) != 0:
                    mode = new_attr&LINE_M_ATTR_USER

                else:
                    mode = new_attr

            draw_chars.append( new_ch )
            draw_cols.append( col )
            draw_modes.append( mode )

        draw_modes.append( None )
        draw_cols.append( 10000000 )

        cur_mode = None
        start = 0
        draw_last = len(draw_chars)
        while start < draw_last:
            end = start + 1

            while end <= draw_last:
                if( draw_modes[ start ] != draw_modes[ end ]
                or draw_cols[ end-1 ]+1 != draw_cols[ end ] ):
                    if cur_mode != draw_modes[ start ]:
                        cur_mode = draw_modes[ start ]
                        self.qp.setPen( QtGui.QPen( fg_colours[ cur_mode ] ) )
                        self.qp.setBackground( bg_colours[ cur_mode ] )

                    x, y = self.__pixelPoint( draw_cols[ start ] + 1, row )

                    self.qp.drawText( x, y+self.char_ascent, ''.join( draw_chars[ start:end ] ) )

                    start = end
                    end += 1

                    if end == draw_last:
                        break

                else:
                    end += 1

            if start == draw_last:
                break

        remaining_width = self.term_width - new_line_length
        if remaining_width > 0:
            bg_brush = QtGui.QBrush( bg_colours[ SYNTAX_DULL ] )
            x, y = self.__pixelPoint( new_line_length+1, row )
            self.qp.fillRect( x, y, remaining_width*self.char_width, self.char_length, bg_brush )

        self._debugTermCalls2( '__termUpdateLine %d done' % (row,) )

    def termMoveLine( self, from_line, to_line ):
        self._debugTermCalls1( 'termMoveLine( %r, %r )' % (from_line,to_line) )
        self.__all_term_ops.append( (self.__termMoveLine, (from_line, to_line)) )

    def __termMoveLine( self, from_line, to_line ):
        self._debugTermCalls2( '__termMoveLine( %r, %r )' % (from_line,to_line) )
        if self.first_paint:
            # Need to init move of the window
            return

        dst_x, dst_y = self.__pixelPoint( 1, to_line )
        src_x, src_y = self.__pixelPoint( 1, from_line )
        width = self.char_width * self.term_width
        height = self.char_length

        self._debugTermCalls2( '__termMoveLine dst_x %r, dst_y %r, width %r height %r src_x %r src_y %r' %
                (dst_x, dst_y
                ,width, height
                ,src_x, src_y) )

        self.qp.drawPixmap( dst_x, dst_y, width, height, self.editor_pixmap, src_x, src_y, width, height )

    def termDisplayActivity( self, ch ):
        self._debugTermCalls1( 'termDisplayActivity( %r )' % (ch,) )

class BemacsVerticalScrollBar(QtWidgets.QScrollBar):
    def __init__( self, panel ):
        super().__init__( QtCore.Qt.Vertical, panel )
        self.editor = panel.app.editor
        self.window_id = None

        self.setMinimum( 1 )
        self.setMaximum( 1 )
        self.setSingleStep( 40 )
        self.setPageStep( 1000 )
        self.setValue( 1 )

        self.actionTriggered.connect( self.handleActionTriggered )

    def setWindowId( self, window_id ):
        self.window_id = window_id

    def handleActionTriggered( self, action ):
        if action == self.SliderSingleStepAdd:
            self.editor.guiScrollChangeVert( self.window_id, +1 )

        elif action == self.SliderSingleStepSub:
            self.editor.guiScrollChangeVert( self.window_id, -1 )

        if action == self.SliderPageStepAdd:
            self.editor.guiScrollChangeVert( self.window_id, +2 )

        elif action == self.SliderPageStepSub:
            self.editor.guiScrollChangeVert( self.window_id, -2 )

        else:
            self.editor.guiScrollSetVert( self.window_id, self.value() )

class BemacsHorizontalScrollBar(QtWidgets.QScrollBar):
    def __init__( self, panel ):
        super().__init__( QtCore.Qt.Horizontal, panel )
        self.editor = panel.app.editor
        self.window_id = None

        self.setMinimum( 1 )
        self.setMaximum( 256 )
        self.setSingleStep( 1 )
        self.setPageStep( 4 )
        self.setValue( 1 )

        self.actionTriggered.connect( self.handleActionTriggered )

    def setWindowId( self, window_id ):
        self.window_id = window_id

    def handleActionTriggered( self, action ):
        if action == self.SliderSingleStepAdd:
            self._debugTermScroll( 'guiScrollChangeHorz id %r %r' % (self.window_id, 1) )
            self.editor.guiScrollChangeHorz( self.window_id, +1 )

        elif action == self.SliderSingleStepSub:
            self._debugTermScroll( 'guiScrollChangeHorz id %r %r' % (self.window_id, -1) )
            self.editor.guiScrollChangeHorz( self.window_id, -1 )

        if action == self.SliderPageStepAdd:
            self._debugTermScroll( 'guiScrollChangeHorz id %r %r' % (self.window_id, 4) )
            self.editor.guiScrollChangeHorz( self.window_id, +4 )

        elif action == self.SliderPageStepSub:
            self._debugTermScroll( 'guiScrollChangeHorz id %r %r' % (self.window_id, -4) )
            self.editor.guiScrollChangeHorz( self.window_id, -4 )

        else:
            self._debugTermScroll( 'guiScrollSetHorz id %r %r' % (self.window_id, self.value()) )
            self.editor.guiScrollSetHorz( self.window_id, self.value() )
