'''
 ====================================================================
 Copyright (c) 2003-2010 Barry A Scott.  All rights reserved.

 This software is licensed as described in the file LICENSE.txt,
 which you should have received as part of this distribution.

 ====================================================================

    be_emacs_panel.py

'''
import sys
import wx
import logging

import be_ids
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
    SYNTAX_DULL:            wx.Colour(  0,  0,  0),
    LINE_M_ATTR_HIGHLIGHT:  wx.Colour(  0,  0,  0),
    SYNTAX_WORD:            wx.Colour(  0,  0,  0),
    SYNTAX_TYPE_STRING1:    wx.Colour(  0,128,  0),
    SYNTAX_TYPE_STRING2:    wx.Colour(  0,128,  0),
    SYNTAX_TYPE_STRING3:    wx.Colour(  0,128,  0),
    SYNTAX_TYPE_COMMENT1:   wx.Colour(  0,128,  0),
    SYNTAX_TYPE_COMMENT2:   wx.Colour(  0,128,  0),
    SYNTAX_TYPE_COMMENT3:   wx.Colour(  0,128,  0),
    SYNTAX_TYPE_KEYWORD1:   wx.Colour(  0,  0,255),
    SYNTAX_TYPE_KEYWORD2:   wx.Colour(255,  0,  0),
    SYNTAX_TYPE_KEYWORD3:   wx.Colour(255,  0,  0),
    LINE_ATTR_MODELINE:     wx.Colour(255,255,128),
    LINE_ATTR_USER+1:       wx.Colour(255,  0,  0),
    LINE_ATTR_USER+2:       wx.Colour(  0,255,  0),
    LINE_ATTR_USER+3:       wx.Colour(  0,  0,255),
    LINE_ATTR_USER+4:       wx.Colour(255,255,  0),
    LINE_ATTR_USER+5:       wx.Colour(255,  0,255),
    LINE_ATTR_USER+6:       wx.Colour(  0,255,255),
    LINE_ATTR_USER+7:       wx.Colour(255,255,255),
    LINE_ATTR_USER+8:       wx.Colour(255,255,255),
    }

bg_colours = {
    SYNTAX_DULL:            wx.Colour(255,255,255),
    LINE_M_ATTR_HIGHLIGHT:  wx.Colour(255,204,102),
    SYNTAX_WORD:            wx.Colour(255,255,255),
    SYNTAX_TYPE_STRING1:    wx.Colour(255,255,255),
    SYNTAX_TYPE_STRING2:    wx.Colour(255,255,255),
    SYNTAX_TYPE_STRING3:    wx.Colour(255,255,255),
    SYNTAX_TYPE_COMMENT1:   wx.Colour(255,255,255),
    SYNTAX_TYPE_COMMENT2:   wx.Colour(255,255,255),
    SYNTAX_TYPE_COMMENT3:   wx.Colour(255,255,255),
    SYNTAX_TYPE_KEYWORD1:   wx.Colour(255,255,255),
    SYNTAX_TYPE_KEYWORD2:   wx.Colour(255,255,255),
    SYNTAX_TYPE_KEYWORD3:   wx.Colour(255,255,255),
    LINE_ATTR_MODELINE:     wx.Colour(  0,  0,255),
    LINE_ATTR_USER+1:       wx.Colour(255,255,255),
    LINE_ATTR_USER+2:       wx.Colour(255,255,255),
    LINE_ATTR_USER+3:       wx.Colour(255,255,255),
    LINE_ATTR_USER+4:       wx.Colour(255,255,255),
    LINE_ATTR_USER+5:       wx.Colour(255,255,255),
    LINE_ATTR_USER+6:       wx.Colour(255,255,255),
    LINE_ATTR_USER+7:       wx.Colour(192,192,192),
    LINE_ATTR_USER+8:       wx.Colour(255,255,255),
    }

default_binding =   u'\uef00'
prefix_key =        u'\uef01'
prefix_mouse =      u'\uef02'
prefix_menu =       u'\uef03'
key_base =          0xef20
mouse_base =        0xef20

def __nextKeyMapping():
    global key_base
    mapping = prefix_key + unichr( key_base )
    key_base += 1
    return mapping

def __nextMouseMapping():
    global mouse_base
    mapping = prefix_mouse + unichr( mouse_base )
    mouse_base += 1
    return mapping

keys_mapping = {
    u'default':                     default_binding,
    u'key-prefix':                  prefix_key,
    u'mouse-prefix':                prefix_mouse,
    u'menu-prefix':                 prefix_menu,
    u'menu':                        prefix_menu,

    u'ss3':                         u'\033O',
    u'csi':                         u'\033[',

    u'tab':                         u'\t',
    u'shift-tab':                   __nextKeyMapping(),

    u'backspace':                   u'\x7f',
    u'ctrl-backspace':              __nextKeyMapping(),

    u'mouse-1-down':                __nextMouseMapping(),
    u'mouse-1-up':                  __nextMouseMapping(),
    u'mouse-2-down':                __nextMouseMapping(),
    u'mouse-2-up':                  __nextMouseMapping(),
    u'mouse-3-down':                __nextMouseMapping(),
    u'mouse-3-up':                  __nextMouseMapping(),
    u'mouse-4-down':                __nextMouseMapping(),
    u'mouse-4-up':                  __nextMouseMapping(),

    u'mouse-motion':                __nextMouseMapping(),

    u'mouse-wheel-neg':             __nextMouseMapping(),
    u'ctrl-mouse-wheel-neg':        __nextMouseMapping(),
    u'shift-mouse-wheel-neg':       __nextMouseMapping(),
    u'ctrl-shift-mouse-wheel-neg':  __nextMouseMapping(),

    u'mouse-wheel-pos':             __nextMouseMapping(),
    u'ctrl-mouse-wheel-pos':        __nextMouseMapping(),
    u'shift-mouse-wheel-pos':       __nextMouseMapping(),
    u'ctrl-shift-mouse-wheel-pos':  __nextMouseMapping(),

    u'insert':                      __nextKeyMapping(),
    u'ctrl-insert':                 __nextKeyMapping(),
    u'shift-insert':                __nextKeyMapping(),
    u'ctrl-shift-insert':           __nextKeyMapping(),

    u'delete':                      __nextKeyMapping(),
    u'ctrl-delete':                 __nextKeyMapping(),
    u'shift-delete':                __nextKeyMapping(),
    u'ctrl-shift-delete':           __nextKeyMapping(),

    u'home':                        __nextKeyMapping(),
    u'ctrl-home':                   __nextKeyMapping(),
    u'shift-home':                  __nextKeyMapping(),
    u'ctrl-shift-home':             __nextKeyMapping(),

    u'end':                         __nextKeyMapping(),
    u'ctrl-end':                    __nextKeyMapping(),
    u'shift-end':                   __nextKeyMapping(),
    u'ctrl-shift-end':              __nextKeyMapping(),

    u'up':                          __nextKeyMapping(),
    u'ctrl-up':                     __nextKeyMapping(),
    u'shift-up':                    __nextKeyMapping(),
    u'ctrl-shift-up':               __nextKeyMapping(),

    u'down':                        __nextKeyMapping(),
    u'ctrl-down':                   __nextKeyMapping(),
    u'shift-down':                  __nextKeyMapping(),
    u'ctrl-shift-down':             __nextKeyMapping(),

    u'left':                        __nextKeyMapping(),
    u'ctrl-left':                   __nextKeyMapping(),
    u'shift-left':                  __nextKeyMapping(),
    u'ctrl-shift-left':             __nextKeyMapping(),

    u'right':                       __nextKeyMapping(),
    u'ctrl-right':                  __nextKeyMapping(),
    u'shift-right':                 __nextKeyMapping(),
    u'ctrl-shift-right':            __nextKeyMapping(),

    u'page-down':                   __nextKeyMapping(),
    u'ctrl-page-down':              __nextKeyMapping(),
    u'shift-page-down':             __nextKeyMapping(),
    u'ctrl-shift-page-down':        __nextKeyMapping(),

    u'page-up':                     __nextKeyMapping(),
    u'ctrl-page-up':                __nextKeyMapping(),
    u'shift-page-up':               __nextKeyMapping(),
    u'ctrl-shift-page-up':          __nextKeyMapping(),

    u'f1':                          __nextKeyMapping(),
    u'ctrl-f1':                     __nextKeyMapping(),
    u'shift-f1':                    __nextKeyMapping(),
    u'ctrl-shift-f1':               __nextKeyMapping(),

    u'f2':                          __nextKeyMapping(),
    u'ctrl-f2':                     __nextKeyMapping(),
    u'shift-f2':                    __nextKeyMapping(),
    u'ctrl-shift-f2':               __nextKeyMapping(),

    u'f3':                          __nextKeyMapping(),
    u'ctrl-f3':                     __nextKeyMapping(),
    u'shift-f3':                    __nextKeyMapping(),
    u'ctrl-shift-f3':               __nextKeyMapping(),

    u'f4':                          __nextKeyMapping(),
    u'ctrl-f4':                     __nextKeyMapping(),
    u'shift-f4':                    __nextKeyMapping(),
    u'ctrl-shift-f4':               __nextKeyMapping(),

    u'f5':                          __nextKeyMapping(),
    u'ctrl-f5':                     __nextKeyMapping(),
    u'shift-f5':                    __nextKeyMapping(),
    u'ctrl-shift-f5':               __nextKeyMapping(),

    u'f6':                          __nextKeyMapping(),
    u'ctrl-f6':                     __nextKeyMapping(),
    u'shift-f6':                    __nextKeyMapping(),
    u'ctrl-shift-f6':               __nextKeyMapping(),

    u'f7':                          __nextKeyMapping(),
    u'ctrl-f7':                     __nextKeyMapping(),
    u'shift-f7':                    __nextKeyMapping(),
    u'ctrl-shift-f7':               __nextKeyMapping(),

    u'f8':                          __nextKeyMapping(),
    u'ctrl-f8':                     __nextKeyMapping(),
    u'shift-f8':                    __nextKeyMapping(),
    u'ctrl-shift-f8':               __nextKeyMapping(),

    u'f9':                          __nextKeyMapping(),
    u'ctrl-f9':                     __nextKeyMapping(),
    u'shift-f9':                    __nextKeyMapping(),
    u'ctrl-shift-f9':               __nextKeyMapping(),

    u'f10':                         __nextKeyMapping(),
    u'ctrl-f10':                    __nextKeyMapping(),
    u'shift-f10':                   __nextKeyMapping(),
    u'ctrl-shift-f10':              __nextKeyMapping(),

    u'f11':                         __nextKeyMapping(),
    u'ctrl-f11':                    __nextKeyMapping(),
    u'shift-f11':                   __nextKeyMapping(),
    u'ctrl-shift-f11':              __nextKeyMapping(),

    u'f12':                         __nextKeyMapping(),
    u'ctrl-f12':                    __nextKeyMapping(),
    u'shift-f12':                   __nextKeyMapping(),
    u'ctrl-shift-f12':              __nextKeyMapping(),

    u'pause':                       __nextKeyMapping(),
    u'print-screen':                __nextKeyMapping(),
    u'scroll-lock':                 __nextKeyMapping(),

    u'kp-divide':                   __nextKeyMapping(),
    u'kp-dot':                      __nextKeyMapping(),
    u'kp-enter':                    __nextKeyMapping(),
    u'kp-minus':                    __nextKeyMapping(),
    u'kp-multiple':                 __nextKeyMapping(),
    u'kp-plus':                     __nextKeyMapping(),
    u'kp0':                         __nextKeyMapping(),
    u'kp1':                         __nextKeyMapping(),
    u'kp2':                         __nextKeyMapping(),
    u'kp3':                         __nextKeyMapping(),
    u'kp4':                         __nextKeyMapping(),
    u'kp5':                         __nextKeyMapping(),
    u'kp6':                         __nextKeyMapping(),
    u'kp7':                         __nextKeyMapping(),
    u'kp8':                         __nextKeyMapping(),
    u'kp9':                         __nextKeyMapping(),
    u'num-lock':                    __nextKeyMapping(),
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
    }

special_keys = {
#   Key code            trans           shift_trans         ctrl_trans          ctrl_shift_trans
    wx.WXK_BACK:        (u'backspace',  None,               u'ctrl-backspace',  None),
    wx.WXK_TAB:         (u'tab',        u'shift-tab',       u'tab',             u'shift-tab'),

    # function keys
    wx.WXK_F1:          (u'f1',         u'shift-f1',        u'ctrl-f1',         u'ctrl-shift-f1'),
    wx.WXK_F2:          (u'f2',         u'shift-f2',        u'ctrl-f2',         u'ctrl-shift-f2'),
    wx.WXK_F3:          (u'f3',         u'shift-f3',        u'ctrl-f3',         u'ctrl-shift-f3'),
    wx.WXK_F4:          (u'f4',         u'shift-f4',        u'ctrl-f4',         u'ctrl-shift-f4'),
    wx.WXK_F5:          (u'f5',         u'shift-f5',        u'ctrl-f5',         u'ctrl-shift-f5'),
    wx.WXK_F6:          (u'f6',         u'shift-f6',        u'ctrl-f6',         u'ctrl-shift-f6'),
    wx.WXK_F7:          (u'f7',         u'shift-f7',        u'ctrl-f7',         u'ctrl-shift-f7'),
    wx.WXK_F8:          (u'f8',         u'shift-f8',        u'ctrl-f8',         u'ctrl-shift-f8'),
    wx.WXK_F9:          (u'f9',         u'shift-f9',        u'ctrl-f9',         u'ctrl-shift-f9'),
    wx.WXK_F10:         (u'f10',        u'shift-f10',       u'ctrl-f10',        u'ctrl-shift-f10'),
    wx.WXK_F11:         (u'f11',        u'shift-f11',       u'ctrl-f11',        u'ctrl-shift-f11'),
    wx.WXK_F12:         (u'f12',        u'shift-f12',       u'ctrl-f12',        u'ctrl-shift-f12'),

    # enhanced keys
    wx.WXK_PAGEUP:      (u'page-up',    None,               u'ctrl-page-up',    None),
    wx.WXK_PAGEDOWN:    (u'page-down',  None,               u'ctrl-page-down',  None),
    wx.WXK_END:         (u'end',        None,               u'ctrl-end',        None),
    wx.WXK_HOME:        (u'home',       None,               u'ctrl-home',       None),

    wx.WXK_LEFT:        (u'left',       None,               u'ctrl-left',       None),
    wx.WXK_UP:          (u'up',         None,               u'ctrl-up',         None),
    wx.WXK_RIGHT:       (u'right',      None,               u'ctrl-right',      None),
    wx.WXK_DOWN:        (u'down',       None,               u'ctrl-down',       None),

    wx.WXK_INSERT:      (u'insert',     u'shift-insert',    u'ctrl-insert',     u'ctrl-shift-insert'),
    wx.WXK_DELETE:      (u'delete',     u'shift-delete',    u'ctrl-delete',     u'ctrl-shift-delete'),
    }

wx_key_names = {}
for name in dir(wx):
    if name.startswith( 'WXK_' ):
        wx_key_names[ getattr( wx, name ) ] = name

wx_evt_names = {}
for name in dir(wx):
    if name.startswith( 'wxEVT_' ):
        wx_evt_names[ getattr( wx, name ) ] = name

class EmacsPanel(wx.Panel, be_debug.EmacsDebugMixin):
    def __init__( self, app, parent ):
        wx.Panel.__init__( self, parent, -1, style=wx.WANTS_CHARS )
        be_debug.EmacsDebugMixin.__init__( self )

        self.app = app
        self.log = app.log

        self._debugPanel( '__init__()' )

        self.tw = be_exceptions.TryWrapperFactory( self.log )

        self.Bind( wx.EVT_PAINT, self.tw( self.OnPaint ) )
        self.Bind( wx.EVT_KEY_DOWN, self.tw( self.OnKeyDown ) )
        self.Bind( wx.EVT_KEY_UP, self.tw( self.OnKeyUp ) )
        self.Bind( wx.EVT_CHAR, self.tw( self.OnChar ) )

        self.Bind( wx.EVT_MOUSE_EVENTS, self.tw( self.OnMouse ) )

        self.Bind( wx.EVT_SCROLL, self.tw( self.OnScroll ) )

        self.dc = None
        self.first_paint = True
        self.__all_term_ops = []
        self.editor_bitmap = None

        self.eat_next_char = False
        self.cursor_x = 1
        self.cursor_y = 1
        self.window_size = 0

        self.all_vert_scroll_bars = []
        self.all_horz_scroll_bars = []

        self.map_vert_scroll_bar_to_window_id = {}
        self.map_horz_scroll_bar_to_window_id = {}

        self.__mouse_button_state = set()

        self.font = None

        self.client_padding = 3

        # the size of a char on the screen
        self.char_width = None
        self.char_length = None
        # the size of the window
        self.pixel_width = None
        self.pixel_length = None
        # the size in chars of the window
        self.term_width = None
        self.term_length = None

        self.Bind( wx.EVT_SIZE, self.tw( self.OnSize ) )

        self.__pending_geometryChanged = False

        self.__initFromPreferences()

    def newPreferences( self ):
        self.__initFromPreferences()

        self.dc = wx.MemoryDC()
        self.dc.SelectObject( self.editor_bitmap )

        self.dc.SetBackgroundMode( wx.SOLID )
        self.dc.SetFont( self.font )

        self.__initFont( self.dc )

        self.dc = None

        self.__geometryChanged()

        self.RefreshRect( (0, 0, self.pixel_width, self.pixel_length), True )

    def __initFromPreferences( self ):
        font_pref = self.app.prefs.getFont()

        self.font = wx.Font( font_pref.point_size, wx.FONTFAMILY_DEFAULT, wx.FONTSTYLE_NORMAL, wx.FONTWEIGHT_NORMAL, False, font_pref.face )
        self.log.info( 'Font face: %r' % (self.font.GetFaceName(),) )

    def __calculateWindowSize( self ):
        if self.char_width is None:
            return

        self.pixel_width, self.pixel_length = self.GetClientSizeTuple()
        self.term_width  = min( MSCREENWIDTH,  (self.pixel_width  - 2*self.client_padding) // self.char_width )
        self.term_length = min( MSCREENLENGTH, (self.pixel_length - 2*self.client_padding) // self.char_length )

        self._debugPanel( '__calculateWindowSize char: %dpx x %dpx window: %dpx X %dpx -> text window: %d X %d' %
                        (self.char_width, self.char_length
                        ,self.pixel_width, self.pixel_length
                        ,self.term_width, self.term_length) )

        self._debugPanel( '__calculateWindowSize create editor_bitmap %d x %d' % (self.pixel_width, self.pixel_length) )
        self.editor_bitmap = wx.EmptyBitmap( self.pixel_width, self.pixel_length, -1 )

        dc = wx.MemoryDC()
        dc.SelectObject( self.editor_bitmap )

        dc.SetBackgroundMode( wx.SOLID )
        dc.SetPen( wx.Pen( bg_colours[ SYNTAX_DULL ] ) )
        dc.SetBrush( wx.Brush( bg_colours[ SYNTAX_DULL ] ) )
        dc.DrawRectangle( 0, 0, self.pixel_width, self.pixel_length )
        dc = None

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
    def __initFont( self, dc ):
        dc.SetFont( self.font )

        self.char_width, self.char_length = dc.GetTextExtent( 'i' )
        self._debugPanel( 'OnPaint first_paint i %d.%d' % (self.char_width, self.char_length) )

        self.char_width, self.char_length = dc.GetTextExtent( 'M' )
        self._debugPanel( 'OnPaint first_paint M %d.%d' % (self.char_width, self.char_length) )

    def OnPaint( self, event ):
        self._debugSpeed( 'OnPaint() begin' )

        self.SetFocusIgnoringChildren()

        if self.first_paint:
            self._debugPanel( 'EmacsPanel.OnPaint() first paint' )
            self.first_paint = False

            dc = wx.PaintDC( self )

            dc.SetBackgroundMode( wx.SOLID )
            dc.SetBackground( wx.Brush( bg_colours[ SYNTAX_DULL ] ) )
            dc.Clear()

            self.__initFont( dc )

            dc = None

            self.__calculateWindowSize()

            # queue up this action until after the rest of GUI init has happend
            self.app.onGuiThread( self.app.onEmacsPanelReady, () )

        elif self.editor_bitmap is not None:
            self._debugPanel( 'EmacsPanel.OnPaint() editor_bitmap' )

            pdc = wx.PaintDC( self )

            pdc.SetBackgroundMode( wx.SOLID )

            self._debugSpeed( 'DrawBitmap() start' )
            pdc.DrawBitmap( self.editor_bitmap, 0, 0, False )
            self._debugSpeed( 'DrawBitmap() end %d x %d' % (self.pixel_width, self.pixel_length) )

            c_x, c_y = self.__pixelPoint( self.cursor_x, self.cursor_y )

            # Create a dc to alpha blend just the cursor as wx.GCDC is very slow on large bitmaps
            cursor_bitmap = self.editor_bitmap.GetSubBitmap( (c_x, c_y, self.char_width, self.char_length) )
            cur_dc_mem = wx.MemoryDC()
            cur_dc_mem.SelectObject( cursor_bitmap )
            cur_dc = wx.GCDC( cur_dc_mem )

            # alpha blend the cursor
            cur_dc.SetBackgroundMode( wx.TRANSPARENT )
            cursor_colour = wx.Colour( 0, 0, 0, 64 )
            cur_dc.SetPen( wx.Pen( cursor_colour ) )
            cur_dc.SetBrush( wx.Brush( cursor_colour ) )
            cur_dc.DrawRectangle( 0, 0, self.char_width, self.char_length )

            pdc.DrawBitmap( cursor_bitmap, c_x, c_y, False )

            pdc = None

        else:
            self._debugPanel( 'EmacsPanel.OnPaint() Nothing to do' )
            event.Skip()

        self._debugSpeed( 'OnPaint() end' )

    def getKeysMapping( self ):
        return keys_mapping

    def OnSize( self, event ):
        self._debugPanel( 'EmacsPanel.OnSize()' )

        if not self.__pending_geometryChanged:
            self.__pending_geometryChanged = True
            self.app.onGuiThread( self.__geometryChanged, () )

        event.Skip()

    def OnKeyDown( self, event ):
        key = event.GetKeyCode()
        shift = event.ShiftDown()
        ctrl = event.ControlDown()

        # map cmd into ctrl
        if wx.Platform == '__WXMAC__':
            cmd = event.ControlDown()
            if cmd:
                ctrl = True

        self._debugTermKey( 'OnKeyDown key %r name %r ctrl %s shift %s' % (key, wx_key_names.get( key, 'unknown' ), T( ctrl ), T( shift )) )

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

            self.eat_next_char = True

        event.Skip()

    def OnKeyUp( self, event ):
        key = event.GetKeyCode()
        shift = event.ShiftDown()
        ctrl = event.ControlDown()

        self._debugTermKey( 'OnKeyUp key %r name %r ctrl %s shift %s' % (key, wx_key_names.get( key, 'unknown' ), T( ctrl ), T( shift )) )

        event.Skip()

    def OnChar( self, event ):
        if self.eat_next_char:
            self._debugTermKey( 'OnChar eat_next_char' )

            self.eat_next_char = False
            return

        char = event.GetUnicodeKey()
        line_parts = ['OnChar "%r" %r' % (unichr( char ), char)]

        key = event.GetKeyCode()
        line_parts.append( ' key %r' % (key,) )

        alt = event.AltDown()
        line_parts.append( ' alt: %s' % T(alt) )

        cmd = event.CmdDown()
        line_parts.append( ' cmd: %s' % T(cmd) )

        ctrl = event.ControlDown()
        line_parts.append( ' control: %s' % T(ctrl) )

        meta = event.MetaDown()
        line_parts.append( ' meta: %s' % T(meta) )

        shift = event.ShiftDown()
        line_parts.append( ' shift: %s' % T(shift) )

        self._debugTermKey( (''.join( line_parts )).encode( 'utf-8' ) )

        if( wx.Platform == '__WXMAC__'
        and cmd
        and char in cmd_to_ctrl_map ):
            print 'mapped %d to %d' % (char, cmd_to_ctrl_map[ char ])
            char = cmd_to_ctrl_map[ char ]

        if( wx.Platform == '__WXGTK__'
        and ctrl
        and char in cmd_to_ctrl_map):
            char = cmd_to_ctrl_map[ char ]

        if ctrl and char == ord( ' ' ):
            char = 0

        self._debugSpeed( 'OnChar( %r )' % unichr( char ), True )
        self.app.editor.guiEventChar( unichr( char ), False )

    def OnMouse( self, event ):
        self._debugTermMouse( 'OnMouse() event_type %r %r' %
                                (event.GetEventType(), wx_evt_names.get( event.GetEventType(), 'unknown' )) )

        if self.char_width is None:
            event.Skip()
            return

        # Calculate character cell position
        x, y = event.GetPosition()
        column = (x - self.client_padding + (self.char_width/2)) // self.char_width + 1;
        line =   (y - self.client_padding ) // self.char_length + 1;

        shift = event.ShiftDown()
        control = event.ControlDown()

        translation = None

        if event.Dragging():
            self._debugTermMouse( 'Mouse drag shift %r line %r column %r' % (shift, line, column) )

            translation = keys_mapping["mouse-motion"]

            self.app.editor.guiEventMouse( translation, shift, [line, column] );

        elif event.IsButton():
            if event.LeftDown() or event.LeftDClick():
                translation = keys_mapping["mouse-1-down"]
                self.__mouse_button_state.add( 1 )

            elif event.LeftUp():
                if 1 in self.__mouse_button_state:
                    translation = keys_mapping["mouse-1-up"]
                    self.__mouse_button_state.remove( 1 )

            elif event.MiddleDown() or event.MiddleDClick():
                translation = keys_mapping["mouse-2-down"]
                self.__mouse_button_state.add( 2 )

            elif event.MiddleUp():
                if 2 in self.__mouse_button_state:
                    translation = keys_mapping["mouse-2-up"]
                    self.__mouse_button_state.remove( 2 )

            elif event.RightDown() or event.RightDClick():
                translation = keys_mapping["mouse-3-down"]
                self.__mouse_button_state.add( 3 )

            elif event.RightUp():
                if 3 in self.__mouse_button_state:
                    translation = keys_mapping["mouse-3-up"]
                    self.__mouse_button_state.remove( 3 )

            else:
                self.log.info( 'Unknown button event: %r' % (event.GetButton(),) )
                return

            self._debugTermMouse( 'Mouse shift %r line %r column %r' % (shift, line, column) )

            if translation is not None:
                self.app.editor.guiEventMouse( translation, shift, [line, column, shift] );

        elif event.GetEventType() == wx.wxEVT_MOUSEWHEEL:
            self._debugTermMouse( 'Mouse Wheel rotation %r delta %r' % (event.GetWheelRotation(), event.GetWheelDelta()) )

            rotation = event.GetWheelRotation() / event.GetWheelDelta()

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

            # on F8 with an ASUS USB mouse the wheel sends -120 or 120 not -1 or 1
            if wx.Platform == '__WXGTK__':
                rotation = 1

            self.app.editor.guiEventMouse( translation, shift, [abs(rotation), line, column] );

    def OnScroll( self, event ):
        bar_id = event.GetId()
        if bar_id in self.map_horz_scroll_bar_to_window_id:
            self.OnScrollHorz( event )

        elif bar_id in self.map_vert_scroll_bar_to_window_id:
            self.OnScrollVert( event )

        else:
            assert False, 'OnScroll called with bar that is not mapped'

    def OnScrollHorz( self, event ):
        win_id = self.map_horz_scroll_bar_to_window_id[ event.GetId() ]
        etype = event.GetEventType()
        self._debugTermScroll( 'OnScrollHorz win_id %d event_type %r %r' %
                                (win_id, etype, wx_evt_names.get( etype, 'unknown')) )

        if etype == wx.wxEVT_SCROLL_LINEDOWN:
            self.app.editor.guiScrollChangeHorz( win_id, +1 )

        elif etype == wx.wxEVT_SCROLL_LINEUP:
            self.app.editor.guiScrollChangeHorz( win_id, -1 )

        if etype == wx.wxEVT_SCROLL_PAGEDOWN:
            self.app.editor.guiScrollChangeHorz( win_id, +4 )

        elif etype == wx.wxEVT_SCROLL_PAGEUP:
            self.app.editor.guiScrollChangeHorz( win_id, -4 )

        elif( etype == wx.wxEVT_SCROLL_THUMBRELEASE
        or etype == wx.wxEVT_SCROLL_THUMBTRACK ):
            self.app.editor.guiScrollSetHorz( win_id, event.GetPosition() )

        event.Skip()

    def OnScrollVert( self, event ):
        win_id = self.map_vert_scroll_bar_to_window_id[ event.GetId() ]
        etype = event.GetEventType()
        self._debugTermScroll( 'OnScrollVert win_id %d event_type %r %r' %
                                (win_id, etype, wx_evt_names.get( etype, 'unknown')) )

        if etype == wx.wxEVT_SCROLL_LINEDOWN:
            self.app.editor.guiScrollChangeVert( win_id, +1 )

        elif etype == wx.wxEVT_SCROLL_LINEUP:
            self.app.editor.guiScrollChangeVert( win_id, -1 )

        if etype == wx.wxEVT_SCROLL_PAGEDOWN:
            self.app.editor.guiScrollChangeVert( win_id, +2 )

        elif etype == wx.wxEVT_SCROLL_PAGEUP:
            self.app.editor.guiScrollChangeVert( win_id, -2 )

        elif( etype == wx.wxEVT_SCROLL_THUMBRELEASE
        or etype == wx.wxEVT_SCROLL_THUMBTRACK ):
            self.app.editor.guiScrollSetVert( win_id, event.GetPosition() )


        event.Skip()

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
        self.dc.Clear()

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
        if self.editor_bitmap is None:
            self._debugTermCalls1( 'termUpdateEnd editor_bitmap is None' )
            return

        self._debugTermCalls1( 'termUpdateEnd() all_horz_scroll_bars %r' % (all_horz_scroll_bars,) )
        self._debugTermCalls1( 'termUpdateEnd() all_vert_scroll_bars %r' % (all_vert_scroll_bars,) )

        self.dc = wx.MemoryDC()
        self.dc.SelectObject( self.editor_bitmap )

        self.dc.SetBackgroundMode( wx.SOLID )
        self.dc.SetFont( self.font )

        self.__executeTermOperations()

        self.dc = None

        c_x, c_y = self.__pixelPoint( self.cursor_x, self.cursor_y )

        self.app.setStatus( all_status_bar_values )

        #--- vert_scroll -----------------------------------------------------------
        for index, bar_info in enumerate( all_vert_scroll_bars ):
            if len( self.all_vert_scroll_bars ) <= index:
                self.all_vert_scroll_bars.append( wx.ScrollBar( self, wx.NewId(), style=wx.SB_VERTICAL ) )

            bar = self.all_vert_scroll_bars[ index ]
            if bar_info is None:
                bar.Show( False )
                self._debugTermCalls1( 'termUpdateEnd: h scroll hode %d' % (index,) )

            else:
                win_id, x, y, width, height, pos, total = bar_info
                self.map_vert_scroll_bar_to_window_id[ bar.GetId() ] = win_id

                x, y = self.__pixelPoint( x+1, y+1 )

                bar.SetScrollbar( pos-1, 1, total, 10 )
                bar.SetSize( (self.char_width * width, self.char_length * height) )
                bar.SetPosition( (x, y) )
                bar.Show( True )
                self._debugTermCalls1( 'termUpdateEnd: h scroll show %d' % (index,) )

        index += 1
        while index < len( self.all_vert_scroll_bars ):
            self.all_vert_scroll_bars[ index ].Show( False )
            self._debugTermCalls1( 'termUpdateEnd: h scroll hode %d' % (index,) )
            index += 1

        #--- horz_scroll -----------------------------------------------------------
        for index, bar_info in enumerate( all_horz_scroll_bars ):
            if len( self.all_horz_scroll_bars ) <= index:
                self.all_horz_scroll_bars.append( wx.ScrollBar( self, wx.NewId(), style=wx.SB_HORIZONTAL ) )

            bar = self.all_horz_scroll_bars[ index ]
            if bar_info is None:
                bar.Show( False )
                self._debugTermCalls1( 'termUpdateEnd: v scroll hode %d' % (index,) )

            else:
                win_id, x, y, width, height, pos = bar_info
                self.map_horz_scroll_bar_to_window_id[ bar.GetId() ] = win_id

                x, y = self.__pixelPoint( x+1, y+1 )

                bar.SetScrollbar( pos-1, 1, 256, 10 )
                bar.SetSize( (self.char_width * width, self.char_length * height) )
                bar.SetPosition( (x, y) )
                bar.Show( True )
                self._debugTermCalls1( 'termUpdateEnd: v scroll show %d' % (index,) )

        index += 1
        while index < len( self.all_horz_scroll_bars ):
            self.all_horz_scroll_bars[ index ].Show( False )
            self._debugTermCalls1( 'termUpdateEnd: v scroll hode %d' % (index,) )
            index += 1

        self.RefreshRect( (0, 0, self.pixel_width, self.pixel_length), False )

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

        if True or wx.Platform == '__WXMAC__':
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
                        self.dc.SetTextForeground( fg_colours[ cur_mode ] ) 
                        self.dc.SetTextBackground( bg_colours[ cur_mode ] )
                        self.dc.SetPen( wx.Pen( bg_colours[ cur_mode ] ) )
                        self.dc.SetBrush( wx.Brush( bg_colours[ cur_mode ] ) )

                x, y = self.__pixelPoint( col+1, row )

                # draw text may not fill the rectange of the char leaving lines on the screen
                self.dc.DrawRectangle( x, y, self.char_width, self.char_length )
                self.dc.DrawText( new_ch, x, y )

        else:
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
            draw_last = len(draw_modes) - 1
            while start < draw_last:
                end = start + 1

                while end <= draw_last:
                    if( draw_modes[ start ] != draw_modes[ end ]
                    or draw_cols[ end-1 ]+1 != draw_cols[ end ] ):
                        if cur_mode != draw_modes[ start ]:
                            cur_mode = draw_modes[ start ]
                            self.dc.SetTextForeground( fg_colours[ cur_mode ] ) 
                            self.dc.SetTextBackground( bg_colours[ cur_mode ] )

                        x, y = self.__pixelPoint( draw_cols[ start ] + 1, row )

                        self.dc.DrawText( ''.join( draw_chars[ start:end ] ), x, y )
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
            self.dc.SetPen( wx.Pen( bg_colours[ SYNTAX_DULL ] ) )
            self.dc.SetBrush( wx.Brush( bg_colours[ SYNTAX_DULL ] ) )
            x, y = self.__pixelPoint( new_line_length+1, row )
            self.dc.DrawRectangle( x, y, remaining_width*self.char_width, self.char_length )

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

        self.dc.Blit(
                dst_x, dst_y,
                width, height,
                self.dc,
                src_x,  src_y,
                wx.COPY,
                False, -1, -1
                )

    def termDisplayActivity( self, ch ):
        self._debugTermCalls1( 'termDisplayActivity( %r )' % (ch,) )
