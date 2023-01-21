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
import time

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

POINT_SIZE_ORDERING = (6, 7, 8, 9, 10, 11, 12, 14, 16, 18, 20, 22, 24, 26, 28, 36, 48, 72)

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

SYNTAX_TYPE_PROBLEM     = (      B(8) );        # 100

SYNTAX_FIRST_FREE       = (      B(9) );        # 200
SYNTAX_PREFIX_QUOTE     = (     B(10) );        # 200 like \ in C
SYNTAX_BEGIN_PAREN      = (     B(11) );        # 400 a begin paren: (<[
SYNTAX_END_PAREN        = (     B(12) );        # 800 an end paren: )>]    end
SYNTAX_LAST_BIT         = (     B(12) );

LINE_ATTR_MODELINE      = SYNTAX_FIRST_FREE<<0  # 200
LINE_M_ATTR_HIGHLIGHT   = SYNTAX_FIRST_FREE<<1  # 400
LINE_ATTR_USER          = SYNTAX_FIRST_FREE<<2  # 800

LINE_M_ATTR_USER        = LINE_ATTR_USER|(7)   # the 8 user colours

# for i18n
def U_( s ):
    return s


class ColourInfo:
    def __init__( self, name, presentation_name, mask, fg, bg ):
        self.name = name
        self.presentation_name = presentation_name
        self.mask = mask
        self.fg = fg
        self.bg = bg

class Theme:
    def __init__( self, name, cursor_fg, all_colours ):
        self.name = name
        self.cursor_fg = cursor_fg
        self.all_colours = all_colours

theme_light = Theme(
    name='Light',
    cursor_fg=(128,  0,  0, 64),
    all_colours=(
        ColourInfo('LINE_ATTR_MODELINE',      U_('Mode line'),    LINE_ATTR_MODELINE,     (255,255,128),  (  0,  0,255)),
        ColourInfo('LINE_M_ATTR_HIGHLIGHT',   U_('Highlight'),    LINE_M_ATTR_HIGHLIGHT,  (  0,  0,  0),  (255,204,102)),
        ColourInfo('SYNTAX_TYPE_PROBLEM',     U_('Problem'),      SYNTAX_TYPE_PROBLEM,    (255,  0,  0),  (255,255,255)),
        ColourInfo('SYNTAX_DULL',             U_('Dull'),         SYNTAX_DULL,            (  0,  0,  0),  (255,255,255)),
        ColourInfo('SYNTAX_WORD',             U_('Word'),         SYNTAX_WORD,            (  0,  0,  0),  (255,255,255)),
        ColourInfo('SYNTAX_TYPE_STRING1',     U_('String 1'),     SYNTAX_TYPE_STRING1,    (128,128,  0),  (255,255,255)),
        ColourInfo('SYNTAX_TYPE_STRING2',     U_('String 2'),     SYNTAX_TYPE_STRING2,    (128,128,  0),  (255,255,255)),
        ColourInfo('SYNTAX_TYPE_STRING3',     U_('String 3'),     SYNTAX_TYPE_STRING3,    (128,128,  0),  (255,255,255)),
        ColourInfo('SYNTAX_TYPE_COMMENT1',    U_('Comment 1'),    SYNTAX_TYPE_COMMENT1,   (  0,128,  0),  (255,255,255)),
        ColourInfo('SYNTAX_TYPE_COMMENT2',    U_('Comment 2'),    SYNTAX_TYPE_COMMENT2,   (  0,128,  0),  (255,255,255)),
        ColourInfo('SYNTAX_TYPE_COMMENT3',    U_('Comment 3'),    SYNTAX_TYPE_COMMENT3,   (  0,128,  0),  (255,255,255)),
        ColourInfo('SYNTAX_TYPE_KEYWORD1',    U_('Keyword 1'),    SYNTAX_TYPE_KEYWORD1,   (  0,  0,255),  (255,255,255)),
        ColourInfo('SYNTAX_TYPE_KEYWORD2',    U_('Keyword 2'),    SYNTAX_TYPE_KEYWORD2,   (255,  0,  0),  (255,255,255)),
        ColourInfo('SYNTAX_TYPE_KEYWORD3',    U_('Keyword 3'),    SYNTAX_TYPE_KEYWORD3,   (255,  0,  0),  (000,255,255)),
        ColourInfo('LINE_ATTR_USER_1',        U_('User 1' ),      LINE_ATTR_USER+1,       (255,  0,  0),  (255,255,255)),
        ColourInfo('LINE_ATTR_USER_2',        U_('User 2' ),      LINE_ATTR_USER+2,       (  0,255,  0),  (255,255,255)),
        ColourInfo('LINE_ATTR_USER_3',        U_('User 3' ),      LINE_ATTR_USER+3,       (  0,  0,255),  (255,255,255)),
        ColourInfo('LINE_ATTR_USER_4',        U_('User 4' ),      LINE_ATTR_USER+4,       (255,255,  0),  (255,255,255)),
        ColourInfo('LINE_ATTR_USER_5',        U_('User 5' ),      LINE_ATTR_USER+5,       (255,  0,255),  (255,255,255)),
        ColourInfo('LINE_ATTR_USER_6',        U_('User 6' ),      LINE_ATTR_USER+6,       (  0,255,255),  (255,255,255)),
        ColourInfo('LINE_ATTR_USER_7',        U_('User 7' ),      LINE_ATTR_USER+7,       (255,255,255),  (192,192,192)),
        ColourInfo('LINE_ATTR_USER_8',        U_('User 8' ),      LINE_ATTR_USER+8,       (192,192,192),  (255,255,255)),
    ) )

theme_dark = Theme(
    name='Dark',
    cursor_fg=(255,255,255,128),
    all_colours=(
        ColourInfo('LINE_ATTR_MODELINE',      U_('Mode line'),    LINE_ATTR_MODELINE,     (255,255,128),  (  0,  0,255)),
        ColourInfo('LINE_M_ATTR_HIGHLIGHT',   U_('Highlight'),    LINE_M_ATTR_HIGHLIGHT,  (  0,  0,  0),  (255,204,102)),
        ColourInfo('SYNTAX_TYPE_PROBLEM',     U_('Problem'),      SYNTAX_TYPE_PROBLEM,    (255,  0,  0),  (255,255,255)),
        ColourInfo('SYNTAX_DULL',             U_('Dull'),         SYNTAX_DULL,            (255,255,255),  (  0,  0,  0)),
        ColourInfo('SYNTAX_WORD',             U_('Word'),         SYNTAX_WORD,            (255,255,255),  (  0,  0,  0)),
        ColourInfo('SYNTAX_TYPE_STRING1',     U_('String 1'),     SYNTAX_TYPE_STRING1,    (255,192, 48),  ( 48, 48, 48)),
        ColourInfo('SYNTAX_TYPE_STRING2',     U_('String 2'),     SYNTAX_TYPE_STRING2,    (255,192, 48),  ( 48, 48, 48)),
        ColourInfo('SYNTAX_TYPE_STRING3',     U_('String 3'),     SYNTAX_TYPE_STRING3,    (255,192, 48),  ( 48, 48, 48)),
        ColourInfo('SYNTAX_TYPE_COMMENT1',    U_('Comment 1'),    SYNTAX_TYPE_COMMENT1,   (  0,255,  0),  ( 48, 48, 48)),
        ColourInfo('SYNTAX_TYPE_COMMENT2',    U_('Comment 2'),    SYNTAX_TYPE_COMMENT2,   (  0,255,  0),  ( 48, 48, 48)),
        ColourInfo('SYNTAX_TYPE_COMMENT3',    U_('Comment 3'),    SYNTAX_TYPE_COMMENT3,   (  0,255,  0),  ( 48, 48, 48)),
        ColourInfo('SYNTAX_TYPE_KEYWORD1',    U_('Keyword 1'),    SYNTAX_TYPE_KEYWORD1,   (160,160,255),  (  0,  0,  0)),
        ColourInfo('SYNTAX_TYPE_KEYWORD2',    U_('Keyword 2'),    SYNTAX_TYPE_KEYWORD2,   (255,  0,  0),  (  0,  0,  0)),
        ColourInfo('SYNTAX_TYPE_KEYWORD3',    U_('Keyword 3'),    SYNTAX_TYPE_KEYWORD3,   ( 64, 64,255),  (  0,  0,  0)),
        ColourInfo('LINE_ATTR_USER_1',        U_('User 1' ),      LINE_ATTR_USER+1,       (255,  0,  0),  ( 48, 48, 48)),
        ColourInfo('LINE_ATTR_USER_2',        U_('User 2' ),      LINE_ATTR_USER+2,       (  0,255,  0),  ( 48, 48, 48)),
        ColourInfo('LINE_ATTR_USER_3',        U_('User 3' ),      LINE_ATTR_USER+3,       (160,160,255),  ( 48, 48, 48)),
        ColourInfo('LINE_ATTR_USER_4',        U_('User 4' ),      LINE_ATTR_USER+4,       (255,255,  0),  ( 48, 48, 48)),
        ColourInfo('LINE_ATTR_USER_5',        U_('User 5' ),      LINE_ATTR_USER+5,       (255,  0,255),  ( 48, 48, 48)),
        ColourInfo('LINE_ATTR_USER_6',        U_('User 6' ),      LINE_ATTR_USER+6,       (  0,255,255),  ( 48, 48, 48)),
        ColourInfo('LINE_ATTR_USER_7',        U_('User 7' ),      LINE_ATTR_USER+7,       (255,255,255),  ( 48, 48, 48)),
        ColourInfo('LINE_ATTR_USER_8',        U_('User 8' ),      LINE_ATTR_USER+8,       (192,192,192),  ( 48, 48, 48)),
    ) )

theme_name_default = 'Light'
all_themes = {
    theme_light.name: theme_light,
    theme_dark.name:  theme_dark,
    }

default_binding =   '\U0010ff00'
prefix_key =        '\U0010ff01'
prefix_mouse =      '\U0010ff02'
prefix_menu =       '\U0010ff03'
key_base =           0x0010ff20
mouse_base =         0x0010ff20

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
    QtCore.Qt.Key_Tab:          ('tab',        None,            None,              None),
    QtCore.Qt.Key_Backtab:      ('shift-tab',  'shift-tab',     'shift-tab',       'shift-tab'),

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
    ST_CUR_STEADY = 0
    ST_CUR_BLINK_SHOW = 1
    ST_CUR_BLINK_HIDE = 2
    ST_CUR_BLINK_NO_FOCUS = 3

    def __init__( self, app, parent ):
        QtWidgets.QWidget.__init__( self, parent )
        be_debug.EmacsDebugMixin.__init__( self )

        self.__debug_save_image_index = 0

        self.app = app
        self.log = app.log

        self.fg_colours = {}
        self.bg_colours = {}

        self.fg_pens = {}
        self.bg_brushes = {}

        self.cursor_brush = None
        self.cursor_timer = QtCore.QTimer( self )
        self.cursor_timer.setSingleShot( True )
        self.cursor_timer.timeout.connect( self.cursorBlinkTimeout )
        self.cursor_blink_interval = 600    # ms
        self.cursor_state = self.ST_CUR_STEADY
        self.cursor_width = 2

        self.__setupColours( app.getPrefs().window )

        self._debugPanel( '__init__()' )

        self.setFocusPolicy( QtCore.Qt.StrongFocus )

        self.tw = be_exceptions.TryWrapperFactory( self.log )

        self.qp = None
        self.first_paint = True

        self.__all_term_ops = []
        self.editor_pixmap = None

        self.cursor_highlighter = EmacsCursorHighlighter( self.app, self )

        # count the number of times the shift key is pressed and release without
        # any char being sent into emacs
        self.shift_pulses = 0

        self.cursor_x = 1
        self.cursor_y = 1
        self.window_size = 0

        self.all_vert_scroll_bars = []
        self.all_vert_scroll_bar_info = []
        self.all_horz_scroll_bars = []
        self.all_horz_scroll_bar_info = []

        self.__mouse_button_state = set()
        self.__last_wheel_event_time = time.time()

        self.font = None
        self.font_prefs_point_size = None
        self.font_temp_point_size = None

        self.client_padding = 3

        # the size of a char on the screen
        self.char_width = None
        self.char_height = None
        self.char_ascent = None

        # the size of the window
        self.pixel_width = None
        self.pixel_height = None
        # the size in chars of the window
        self.term_width = None
        self.term_height = None

        self.__pending_geometryChanged = False

        self.__use_fast_drawtext = False

        self.__initFromPreferences()
        self.__initFont()

    def newPreferences( self, temp_point_size=None ):
        self.font_temp_point_size = temp_point_size

        self.__initFromPreferences()
        self.__initFont()

        self.__geometryChanged()

        self.update( 0, 0, self.pixel_width, self.pixel_height )

    def __initFromPreferences( self ):
        prefs = self.app.getPrefs()
        self.__setupColours( prefs.window )

        font_pref = prefs.window.font

        self.font_prefs_point_size = font_pref.point_size
        if self.font_temp_point_size is None:
            point_size = self.font_prefs_point_size
        else:
            point_size = self.font_temp_point_size

        self.font = QtGui.QFont( font_pref.face, point_size )

        fi = QtGui.QFontInfo( self.font )
        self.log.info( 'Font family: %r %dpt' % (fi.family(), fi.pointSize()) )

    def changeFontPointSize( self, direction ):
        if direction == 0:
           self.newPreferences( None )

        else:
            if self.font_temp_point_size is None:
                point_size = self.font_prefs_point_size
            else:
                point_size = self.font_temp_point_size

            if direction > 0:
                for size in POINT_SIZE_ORDERING:
                    if size > point_size:
                        point_size = size
                        break
            else:
                for size in reversed( POINT_SIZE_ORDERING ):
                    if size < point_size:
                        point_size = size
                        break

            self.newPreferences( point_size )

    def __setupColours( self, win_prefs ):
        theme = all_themes[ win_prefs.theme.name ]
        for colour_info in theme.all_colours:
            colour_pref = win_prefs.getColour( colour_info.name )
            mask = colour_info.mask

            self.fg_colours[ mask ] = QtGui.QColor( *colour_pref.fg.getTuple() )
            self.fg_pens[ mask ] =    QtGui.QPen( self.fg_colours[ mask ] )
            self.bg_colours[ mask ] = QtGui.QColor( *colour_pref.bg.getTuple() )
            self.bg_brushes[ mask ] = QtGui.QBrush( self.bg_colours[ mask ] )

        self.cursor_colour = QtGui.QColor( *win_prefs.cursor.fg.getTuple() )
        self.cursor_brush = QtGui.QBrush( self.cursor_colour )

        if win_prefs.cursor_style.blink:
            self.cursor_state = self.ST_CUR_BLINK_SHOW
            self.cursor_blink_interval = win_prefs.cursor_style.interval

        else:
            self.cursor_state = self.ST_CUR_STEADY

    def __calculateWindowSize( self ):
        assert self.char_width is not None

        self.pixel_width = self.width()
        self.pixel_height = self.height()

        self.term_width  = max( 10, min( MSCREENWIDTH,  (self.pixel_width  - 2*self.client_padding) // self.char_width ) )
        self.term_height = max( 10, min( MSCREENLENGTH, (self.pixel_height - 2*self.client_padding) // self.char_height ) )

        self._debugPanel( '__calculateWindowSize char: %dpx x %dpx window: %dpx X %dpx -> text window: %d X %d' %
                        (self.char_width, self.char_height
                        ,self.pixel_width, self.pixel_height
                        ,self.term_width, self.term_height) )

        self._debugPanel( '__calculateWindowSize create editor_pixmap %d x %d' % (self.pixel_width, self.pixel_height) )
        self.editor_pixmap = QtGui.QPixmap( self.pixel_width, self.pixel_height )

        qp = QtGui.QPainter( self.editor_pixmap )
        qp.setBackgroundMode( QtCore.Qt.OpaqueMode )
        qp.setBackground( self.bg_brushes[ SYNTAX_DULL ] )
        qp.fillRect( 0, 0, self.pixel_width, self.pixel_height, self.bg_brushes[ SYNTAX_DULL ] )
        del qp

    def __pixelPoint( self, x, y ):
        return  (self.client_padding + self.char_width  * (x-1)
                ,self.client_padding + self.char_height * (y-1))


    def __geometryChanged( self ):
        self.__pending_geometryChanged = False

        self.__calculateWindowSize()

        self.cursor_highlighter.setHeight( self.char_height * 3 )

        if self.app.editor is None:
            self._debugPanel( '__geometryChanged no self.app.editor' )
            return

        if self.char_width is None:
            self._debugPanel( '__geometryChanged self.char_width is None' )
            return

        self.app.editor.guiGeometryChange( self.term_width, self.term_height )

    #--------------------------------------------------------------------------------
    #
    #   Event handlers
    #
    #--------------------------------------------------------------------------------
    def __initFont( self ):
        metrics = QtGui.QFontMetrics( self.font )

        # check that widths of chars make a regular grid
        # __termUpdateLineFast assumes that all strings are a multple
        # of char_width but with some OS and Qt versions this is not true
        self.__use_fast_drawtext = (metrics.width( 'i' ) == metrics.width( 'm' )
                              and metrics.width( 'i' )*2 == metrics.width( 'ii' )
                              and metrics.width( 'm' )*2 == metrics.width( 'mm' ))

        self.char_width = metrics.width( 'm' )
        # For some fonts leading() returns negative offsets
        # which make the lines overlap.
        self.char_height = metrics.height() + max(0, metrics.leading())
        self.char_ascent = metrics.ascent()
        self._debugPanel( 'paintEvent first_paint draw %r width %d height %d ascent %d' %
                            (self.__use_fast_drawtext and 'fast' or 'slow'
                            ,self.char_width, self.char_height, self.char_ascent) )

        p = self.app.getPrefs().window.cursor_style
        if p.shape == 'line':
            self.cursor_width = max( 1, self.char_width // 4 )

        else:
            self.cursor_width = self.char_width

    def paintEvent( self, event ):
        self._debugSpeed( 'paintEvent() begin' )

        if self.first_paint:
            self._debugPanel( 'EmacsPanel.paintEvent() first paint' )
            self.first_paint = False

            qp = QtGui.QPainter( self )
            qp.fillRect( QtCore.QRect( 0, 0, self.pixel_width, self.pixel_height ), self.bg_brushes[ SYNTAX_DULL ] )
            del qp

            self.__calculateWindowSize()

            # queue up this action until after the rest of GUI init has happend
            self.app.marshallToGuiThread( self.app.onEmacsPanelReady, () )

        elif self.editor_pixmap is not None:
            self._debugPanel( 'EmacsPanel.paintEvent() editor_pixmap %r' % (self.editor_pixmap,) )

            qp = QtGui.QPainter( self )
            qp.setClipRegion( event.region() )

            qp.setBackgroundMode( QtCore.Qt.OpaqueMode )

            self._debugSpeed( 'drawPixmap() start' )
            qp.drawPixmap( 0, 0, self.pixel_width, self.pixel_height, self.editor_pixmap )
            self._debugSpeed( 'drawPixmap() end %d x %d' % (self.pixel_width, self.pixel_height) )

            c_x, c_y = self.__pixelPoint( self.cursor_x, self.cursor_y )
            #self.cursor.drawCursor( c_x, c_y )
            if self.cursor_state in (self.ST_CUR_STEADY, self.ST_CUR_BLINK_SHOW, self.ST_CUR_BLINK_NO_FOCUS):
                # alpha blend the cursor
                qp.fillRect( c_x, c_y, self.cursor_width, self.char_height, self.cursor_brush )

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
                qp.fillRect( x, y, self.char_width * width, self.char_height * height, scroll_bar_bg )

            #--- horz_scroll -----------------------------------------------------------
            for bar_info in self.all_horz_scroll_bar_info:
                if bar_info is None:
                    continue

                win_id, x, y, width, height, pos = bar_info
                x, y = self.__pixelPoint( x+1, y+1 )
                qp.fillRect( x, y, self.char_width * width, self.char_height * height, scroll_bar_bg )

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

    def focusInEvent( self, event ):
        super().focusInEvent( event )
        self.app.guiHasFocus()
        if self.cursor_state == self.ST_CUR_BLINK_NO_FOCUS:
            self.cursor_state = self.ST_CUR_BLINK_HIDE

    def focusOutEvent( self, event ):
        super().focusInEvent( event )
        if self.cursor_state in (self.ST_CUR_BLINK_SHOW, self.ST_CUR_BLINK_HIDE):
            self.cursor_state = self.ST_CUR_BLINK_NO_FOCUS

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

        if (ctrl or cmd) and key == QtCore.Qt.Key_0:
            self.changeFontPointSize( 0 )
            return

        elif (ctrl or cmd) and key == QtCore.Qt.Key_Equal:
            self.changeFontPointSize( +1 )
            return

        elif (ctrl or cmd) and key == QtCore.Qt.Key_Minus:
            self.changeFontPointSize( -1 )
            return

        if key in special_keys:
            self._debugTermKey( 'keyPressEvent special_keys %r' % (special_keys[ key ],) )
            trans, shift_trans, ctrl_trans, ctrl_shift_trans = special_keys[ key ]

            if ctrl and shift and ctrl_shift_trans is not None:
                translation = ctrl_shift_trans
                shift = False

            elif ctrl and ctrl_trans is not None:
                translation = ctrl_trans

            elif shift and shift_trans is not None:
                translation = shift_trans
                shift = False

            else:
                translation = trans

            translation = keys_mapping[ translation ]

            for ch in translation:
                self.__guiEventChar( ch, shift )

        elif len(event.text()) > 0:
            self.handleNonSpecialKey( event )

        elif sys.platform == 'darwin' and key in cmd_to_ctrl_map:
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

        if key == QtCore.Qt.Key_Shift:
            self.shift_pulses += 1

        if self.shift_pulses >= 3:
            # clear the pulse trigger
            self.shift_pulses = 0

            c_x, c_y = self.__pixelPoint( self.cursor_x, self.cursor_y )
            # calc the center of the cursor char cell
            c_x += self.char_width//2
            c_y -= self.char_height
            self.cursor_highlighter.pulseCursor( c_x, c_y )

    def handleNonSpecialKey( self, event ):
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
        and key in cmd_to_ctrl_map ):
            self._debugTermKey( 'mapped %d to %d' % (key, cmd_to_ctrl_map[ key ]) )
            char = cmd_to_ctrl_map[ key ]

        if ctrl and char == ord( ' ' ):
            char = 0

        self._debugSpeed( 'handleNonSpecialKey( %r )' % (char,), True )
        self.__guiEventChar( char, False )

    def __guiEventChar( self, char, shift ):
        self.shift_pulses = 0
        self.app.editor.guiEventChar( char, shift )

    def __guiEventMouse( self, keys, shift, all_params ):
        self.shift_pulses = 0
        self.app.editor.guiEventMouse( keys, shift, all_params )

    def mousePressEvent( self, event ):
        self._debugTermMouse( 'mousePressEvent   %r %r %r )' % (event.button(), int(event.buttons()), event.pos()) )

        if self.char_width is None:
            return

        # Calculate character cell position
        column = ((event.x() - self.client_padding + (self.char_width/2)) // self.char_width) + 1;
        line =   ((event.y() - self.client_padding ) // self.char_height) + 1;

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
            self.__guiEventMouse( translation, shift, [line, column, shift] );

    def mouseReleaseEvent( self, event ):
        self._debugTermMouse( 'mouseReleaseEvent %r %r %r )' % (event.button(), int(event.buttons()), event.pos()) )

        if self.char_width is None:
            return

        column = ((event.x() - self.client_padding + (self.char_width/2)) // self.char_width) + 1;
        line =   ((event.y() - self.client_padding ) // self.char_height) + 1;

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
            self.__guiEventMouse( translation, shift, [line, column, shift] );

    def mouseMoveEvent( self, event ):
        self._debugTermMouse( 'mouseMoveEvent %r %r %r )' % (event.button(), int(event.buttons()), event.pos()) )

        if self.char_width is None:
            return

        column = ((event.x() - self.client_padding + (self.char_width/2)) // self.char_width) + 1;
        line =   ((event.y() - self.client_padding ) // self.char_height) + 1;

        translation = keys_mapping["mouse-motion"]

        modifiers = int(self.app.keyboardModifiers())
        shift = (modifiers & QtCore.Qt.ShiftModifier) != 0
        self.__guiEventMouse( translation, shift, [line, column] );

    def wheelEvent( self, event ):
        self._debugTermMouse( 'wheelEvent source %d, angle %r, pixel %r' %
                (event.source(), event.angleDelta().y(), event.pixelDelta().y()) )

        if event.angleDelta().y() == 0:
            return

        elif event.angleDelta().y() > 0:
            rotation = 1
        else:
            rotation = -1

        # there can be a storm of events from a track pad
        # that makes the scrolling jerky. Filter out excess
        # events. 40ms seems to work well on OS X
        this_event_time = time.time()
        if (this_event_time - self.__last_wheel_event_time) < 0.040:
            return

        self.__last_wheel_event_time = this_event_time

        column = ((event.x() - self.client_padding + (self.char_width/2)) // self.char_width) + 1;
        line =   ((event.y() - self.client_padding ) // self.char_height) + 1;

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

        self.__guiEventMouse( translation, shift, [abs(rotation), line, column] );

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
        #self.RefreshRect( (0,0,self.pixel_width,self.pixel_height), True )

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
        #self.qp.setRenderHint( self.qp.TextAntialiasing, False )

        self.qp.setBackground( self.bg_brushes[ SYNTAX_DULL ] )
        self.qp.setBackgroundMode( QtCore.Qt.OpaqueMode )
        self.qp.setFont( self.font )
        self.qp.setPen( self.fg_colours[ SYNTAX_DULL ] )

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
                    self._debugTermCalls1( 'termUpdateEnd: V scroll hide %d' % (index,) )

                else:
                    win_id, x, y, width, height, pos, total = bar_info
                    self._debugTermScroll( 'termUpdateEnd set V scroll id %r value %r' % (win_id, pos) )
                    bar.setWindowId( win_id )
                    bar.setMaximum( total )
                    bar.setValue( pos )

                    x, y = self.__pixelPoint( x+1, y+1 )

                    self.qp.fillRect( x, y, self.char_width * width, self.char_height * height, QtGui.QColor( 192, 192, 192 ) )

                    bar.resize( self.char_width * width, self.char_height * height )
                    bar.move( x, y )

                    bar.show()
                    self._debugTermCalls1( 'termUpdateEnd: V scroll show %d' % (index,) )

            index += 1
            while index < len( self.all_vert_scroll_bars ):
                self.all_vert_scroll_bars[ index ].hide()
                self._debugTermCalls1( 'termUpdateEnd: V scroll hide %d extra' % (index,) )
                index += 1

            #--- horz_scroll -----------------------------------------------------------
            self.all_horz_scroll_bar_info = all_horz_scroll_bars
            for index, bar_info in enumerate( all_horz_scroll_bars ):
                if len( self.all_horz_scroll_bars ) <= index:
                    self.all_horz_scroll_bars.append( BemacsHorizontalScrollBar( self ) )

                bar = self.all_horz_scroll_bars[ index ]
                if bar_info is None:
                    bar.hide()
                    self._debugTermCalls1( 'termUpdateEnd: H scroll hide %d' % (index,) )

                else:
                    win_id, x, y, width, height, pos = bar_info
                    self._debugTermScroll( 'termUpdateEnd set H scroll id %r value %r' % (win_id, pos) )
                    bar.setWindowId( win_id )
                    bar.setValue( pos )

                    x, y = self.__pixelPoint( x+1, y+1 )

                    self.qp.fillRect( x, y, self.char_width * width, self.char_height * height, QtGui.QColor( 192, 192, 192 ) )

                    bar.resize( self.char_width * width, self.char_height * height )
                    bar.move( x, y )

                    bar.show()
                    self._debugTermCalls1( 'termUpdateEnd: H scroll show %d' % (index,) )

            index += 1
            while index < len( self.all_horz_scroll_bars ):
                self.all_horz_scroll_bars[ index ].hide()
                self._debugTermCalls1( 'termUpdateEnd: H scroll hode %d extra' % (index,) )
                index += 1

        del self.qp

        self.__debug_save_image_index += 1
        #qqq#self.editor_pixmap.save( '/home/barry/tmpdir/image-%03d.png' % (self.__debug_save_image_index,), 'PNG' )

        if self.cursor_state != self.ST_CUR_STEADY:
            self.cursor_state = self.ST_CUR_BLINK_SHOW
            self.cursor_timer.start( self.cursor_blink_interval )

        self.update( 0, 0, self.pixel_width, self.pixel_height )

        self._debugTermCalls1( 'termUpdateEnd() done ---------------------------------------------------------' )
        self._debugSpeed( 'termUpdateEnd() end' )

    def cursorBlinkTimeout( self ):
        if self.cursor_state == self.ST_CUR_BLINK_SHOW:
            self.cursor_state = self.ST_CUR_BLINK_HIDE

        elif self.cursor_state == self.ST_CUR_BLINK_HIDE:
            self.cursor_state = self.ST_CUR_BLINK_SHOW

        self.cursor_timer.start( self.cursor_blink_interval )
        self.update( 0, 0, self.pixel_width, self.pixel_height )

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
            if self.__use_fast_drawtext:
                self.__all_term_ops.append( (self.__termUpdateLineFast, (old, new, row)) )
            else:
                self.__all_term_ops.append( (self.__termUpdateLineSlow, (old, new, row)) )

    def __termUpdateLineFast( self, old, new, row ):
        if old is not None:
            self._debugTermCalls2( '__termUpdateLineFast row=%d old %r' % (row, old[0].rstrip(),) )
        self._debugTermCalls2( '__termUpdateLineFast row=%d new %r' % (row, new[0].rstrip(),) )

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
                        self.qp.setPen( self.fg_pens[ cur_mode ] )
                        self.qp.setBackground( self.bg_brushes[ cur_mode ] )

                    x, y = self.__pixelPoint( draw_cols[ start ] + 1, row )

                    self.qp.fillRect( x, y, (end-start)*self.char_width, self.char_height, self.bg_brushes[ cur_mode ] )
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
            x, y = self.__pixelPoint( new_line_length+1, row )
            self.qp.fillRect( x, y, remaining_width*self.char_width, self.char_height, self.bg_brushes[ SYNTAX_DULL ] )

        self._debugTermCalls2( '__termUpdateLineFast %d done' % (row,) )

    def __termUpdateLineSlow( self, old, new, row ):
        if old is not None:
            self._debugTermCalls2( '__termUpdateLineSlow row=%d old %r' % (row, old[0].rstrip(),) )
            self._debugTermCalls2( '__termUpdateLineSlow row=%d old %r' % (row, old[1],) )
        self._debugTermCalls2( '__termUpdateLineSlow row=%d new %r' % (row, new[0].rstrip(),) )
        self._debugTermCalls2( '__termUpdateLineSlow row=%d new %r' % (row, new[1],) )

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

                if cur_mode != mode:
                    cur_mode = mode
                    self.qp.setPen( self.fg_pens[ cur_mode ] )
                    self.qp.setBackground( self.bg_brushes[ cur_mode ] )

            x, y = self.__pixelPoint( col+1, row )

            # draw text may not fill the rectange of the char leaving lines on the screen
            #self.qp.fillRect( x, y, self.char_width, self.char_height, self.bg_brushes[ cur_mode ] ) #qqq
            self.qp.drawText( x, y+self.char_ascent, new_ch )

        remaining_width = self.term_width - new_line_length
        if remaining_width > 0:
            x, y = self.__pixelPoint( new_line_length+1, row )
            self.qp.fillRect( x, y, remaining_width*self.char_width, self.char_height, self.bg_brushes[ SYNTAX_DULL ] )

        self._debugTermCalls2( '__termUpdateLineSlow %d done' % (row,) )

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
        height = self.char_height

        self._debugTermCalls2( '__termMoveLine dst_x %r, dst_y %r, width %r height %r src_x %r src_y %r' %
                (dst_x, dst_y
                ,width, height
                ,src_x, src_y) )

        self.qp.drawPixmap( dst_x, dst_y, width, height, self.editor_pixmap, src_x, src_y, width, height )

    def termDisplayActivity( self, ch ):
        self._debugTermCalls1( 'termDisplayActivity( %r )' % (ch,) )

class EmacsCursorHighlighter(QtWidgets.QWidget, be_debug.EmacsDebugMixin):
    ST_HIDDEN = 0
    ST_DRAW_BIGGER = 1
    ST_DRAW_SMALLER = 2

    def __init__( self, app, parent ):
        QtWidgets.QWidget.__init__( self, parent )
        self.log = parent.log
        self.panel = parent
        self.height = 100
        self.radius = None
        self.state = self.ST_HIDDEN

        self.timer = QtCore.QTimer( self )
        self.timer.setSingleShot( True )
        self.timer.timeout.connect( self.pulseTimeout )

        be_debug.EmacsDebugMixin.__init__( self )

        self.resize( self.height, self.height )

        self.hide()

    def setHeight( self, height ):
        self.height = height
        self.resize( self.height, self.height )

    def pulseCursor( self, x, y ):
        self.radius = 0
        self.state = self.ST_DRAW_BIGGER

        x -= self.height//2
        self.move( x, y )
        self.show()
        self.pulseTimeout()

    def pulseTimeout( self ):
        if self.state == self.ST_DRAW_BIGGER:
            self.radius += 2
            if self.radius < (self.height // 2):
                self.update( 0, 0, self.height, self.height )
                self.timer.start( 17 )  # assume 60Hz, frame each 16.6ms

            else:
                self.state = self.ST_DRAW_SMALLER
                self.timer.start( 200 )

        elif self.state == self.ST_DRAW_SMALLER:
            self.radius -= 2
            if self.radius >= 2:
                self.update( 0, 0, self.height, self.height )
                self.timer.start( 33 )  # assume 60Hz, frame each 16.6ms

            else:
                self.state = self.ST_HIDDEN
                self.hide()


    def paintEvent(self, event):
        colour = QtGui.QColor( 192, 0, 0, 128 )
        pen = QtGui.QPen( colour )
        pen.setWidth( 1 )
        brush = QtGui.QBrush( colour )

        qp = QtGui.QPainter( self )
        qp.setBrush( brush )
        qp.setPen( pen )
        center = self.height // 2

        qp.drawEllipse( QtCore.QRect( center-self.radius, center-self.radius, self.radius*2, self.radius*2 ) )
        del qp

class BemacsVerticalScrollBar(QtWidgets.QScrollBar, be_debug.EmacsDebugMixin):
    def __init__( self, panel ):
        super().__init__( QtCore.Qt.Vertical, panel )
        self.log = panel.log
        be_debug.EmacsDebugMixin.__init__( self )

        self.editor = panel.app.editor
        self.window_id = None

        self.setMinimum( 1 )
        self.setMaximum( 2 )
        self.setSingleStep( 1 )
        self.setPageStep( 1 )
        self.setValue( 1 )

        self.actionTriggered.connect( self.handleActionTriggered )

    def setWindowId( self, window_id ):
        self.window_id = window_id

    def handleActionTriggered( self, action ):
        if action == self.SliderSingleStepAdd:
            self._debugTermScroll( 'guiScrollChangeVert id %r %r' % (self.window_id, 1) )
            self.editor.guiScrollChangeVert( self.window_id, +1 )

        elif action == self.SliderSingleStepSub:
            self._debugTermScroll( 'guiScrollChangeVert id %r %r' % (self.window_id, -1) )
            self.editor.guiScrollChangeVert( self.window_id, -1 )

        if action == self.SliderPageStepAdd:
            self._debugTermScroll( 'guiScrollChangeVert id %r %r' % (self.window_id, 2) )
            self.editor.guiScrollChangeVert( self.window_id, +2 )

        elif action == self.SliderPageStepSub:
            self._debugTermScroll( 'guiScrollChangeVert id %r %r' % (self.window_id, -2) )
            self.editor.guiScrollChangeVert( self.window_id, -2 )

        else:
            self._debugTermScroll( 'guiScrollSetVert id %r %r' % (self.window_id, self.value()) )
            self.editor.guiScrollSetVert( self.window_id, self.value() )

class BemacsHorizontalScrollBar(QtWidgets.QScrollBar, be_debug.EmacsDebugMixin):
    def __init__( self, panel ):
        super().__init__( QtCore.Qt.Horizontal, panel )
        self.log = panel.log
        be_debug.EmacsDebugMixin.__init__( self )

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
