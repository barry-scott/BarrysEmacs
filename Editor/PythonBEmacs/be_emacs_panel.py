'''
 ====================================================================
 Copyright (c) 2003-2010 Barry A Scott.  All rights reserved.

 This software is licensed as described in the file LICENSE.txt,
 which you should have received as part of this distribution.

 ====================================================================

    be_emacs_panel.py

    Based on code from pysvn WorkBench

'''
import sys
import wx
import wx.stc
import logging

import be_ids
import be_exceptions
import be_version
import be_images
import be_platform_specific

import be_config

_debug_term_calls1 = False
_debug_term_calls2 = False
_debug_term_input = True

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
    LINE_M_ATTR_HIGHLIGHT:  wx.Colour(255,255,255),
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
    LINE_M_ATTR_HIGHLIGHT:  wx.Colour(  0,  0,  0),
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

keys_mapping = {
    u'default':                     default_binding,
    u'key-prefix':                  prefix_key,
    u'mouse-prefix':                prefix_mouse,
    u'menu-prefix':                 prefix_menu,

    u'csi':                         u'\033[',
    u'backspace':                   u'\x7f',
    u'menu':                        u'\uef03',
    u'mouse':                       u'\033[&w',
    u'mouse-1-down':                u'\033[2&w',
    u'mouse-1-up':                  u'\033[3&w',
    u'mouse-2-down':                u'\033[4&w',
    u'mouse-2-up':                  u'\033[5&w',
    u'mouse-3-down':                u'\033[6&w',
    u'mouse-3-up':                  u'\033[7&w',
    u'mouse-4-down':                u'\033[8&w',
    u'mouse-4-up':                  u'\033[9&w',
    u'mouse-wheel':                 u'\033[#w',
    u'mouse-wheel-neg':             u'\033[1#w',
    u'mouse-wheel-pos':             u'\033[0#w',
    u'ctrl-mouse-wheel-neg':        u'\033[9#w',
    u'ctrl-mouse-wheel-pos':        u'\033[8#w',
    u'ctrl-shift-mouse-wheel-neg':  u'\033[13#w',
    u'ctrl-shift-mouse-wheel-pos':  u'\033[12#w',
    u'shift-mouse-wheel-neg':       u'\033[5#w',
    u'shift-mouse-wheel-pos':       u'\033[4#w',
    u'ss3':                         u'\033O',
    u'tab':                         u'\t',

    u'ctrl-backspace':              prefix_key + u'\uef20',
    u'ctrl-delete':                 prefix_key + u'\uef21',
    u'ctrl-down':                   prefix_key + u'\uef22',
    u'ctrl-end':                    prefix_key + u'\uef23',
    u'ctrl-left':                   prefix_key + u'\uef32',
    u'ctrl-page-down':              prefix_key + u'\uef33',
    u'ctrl-page-up':                prefix_key + u'\uef34',
    u'ctrl-right':                  prefix_key + u'\uef37',
    u'ctrl-shift-delete':           prefix_key + u'\uef39',
    u'ctrl-shift-down':             prefix_key + u'\uef3a',
    u'ctrl-shift-end':              prefix_key + u'\uef3b',
    u'ctrl-shift-left':             prefix_key + u'\uef4a',
    u'ctrl-shift-right':            prefix_key + u'\uef4f',
    u'ctrl-shift-up':               prefix_key + u'\uef51',
    u'ctrl-up':                     prefix_key + u'\uef52',
    u'delete':                      prefix_key + u'\uef53',
    u'down':                        prefix_key + u'\uef54',
    u'end':                         prefix_key + u'\uef55',
    u'left':                        prefix_key + u'\uef74',
    u'right':                       prefix_key + u'\uef7a',
    u'shift-delete':                prefix_key + u'\uef7c',
    u'shift-down':                  prefix_key + u'\uef7d',
    u'shift-left':                  prefix_key + u'\uef8e',
    u'shift-page-down':             prefix_key + u'\uef8f',
    u'shift-page-up':               prefix_key + u'\uef90',
    u'shift-right':                 prefix_key + u'\uef93',
    u'shift-up':                    prefix_key + u'\uef95',
    u'up':                          prefix_key + u'\uef96',

    u'ctrl-f1':                     prefix_key + u'\uef24',
    u'ctrl-f12':                    prefix_key + u'\uef25',
    u'ctrl-f2':                     prefix_key + u'\uef26',
    u'ctrl-f3':                     prefix_key + u'\uef27',
    u'ctrl-f4':                     prefix_key + u'\uef28',
    u'ctrl-f5':                     prefix_key + u'\uef29',
    u'ctrl-f6':                     prefix_key + u'\uef2a',
    u'ctrl-f7':                     prefix_key + u'\uef2b',
    u'ctrl-f8':                     prefix_key + u'\uef2c',
    u'ctrl-f9':                     prefix_key + u'\uef2d',
    u'ctrl-f10':                    prefix_key + u'\uef2e',
    u'ctrl-f11':                    prefix_key + u'\uef2f',
    u'ctrl-home':                   prefix_key + u'\uef30',
    u'ctrl-insert':                 prefix_key + u'\uef31',
    u'ctrl-pause':                  prefix_key + u'\uef35',
    u'ctrl-print-screen':           prefix_key + u'\uef36',
    u'ctrl-scroll-lock':            prefix_key + u'\uef38',
    u'ctrl-shift-f1':               prefix_key + u'\uef3c',
    u'ctrl-shift-f2':               prefix_key + u'\uef3d',
    u'ctrl-shift-f3':               prefix_key + u'\uef3e',
    u'ctrl-shift-f4':               prefix_key + u'\uef3f',
    u'ctrl-shift-f5':               prefix_key + u'\uef40',
    u'ctrl-shift-f6':               prefix_key + u'\uef41',
    u'ctrl-shift-f7':               prefix_key + u'\uef42',
    u'ctrl-shift-f8':               prefix_key + u'\uef43',
    u'ctrl-shift-f9':               prefix_key + u'\uef44',
    u'ctrl-shift-f10':              prefix_key + u'\uef45',
    u'ctrl-shift-f11':              prefix_key + u'\uef46',
    u'ctrl-shift-f12':              prefix_key + u'\uef47',
    u'ctrl-shift-home':             prefix_key + u'\uef48',
    u'ctrl-shift-insert':           prefix_key + u'\uef49',
    u'ctrl-shift-page-down':        prefix_key + u'\uef4b',
    u'ctrl-shift-page-up':          prefix_key + u'\uef4c',
    u'ctrl-shift-pause':            prefix_key + u'\uef4d',
    u'ctrl-shift-print-screen':     prefix_key + u'\uef4e',
    u'ctrl-shift-scroll-lock':      prefix_key + u'\uef50',
    u'f1':                          prefix_key + u'\uef56',
    u'f2':                          prefix_key + u'\uef57',
    u'f3':                          prefix_key + u'\uef58',
    u'f4':                          prefix_key + u'\uef59',
    u'f5':                          prefix_key + u'\uef5a',
    u'f6':                          prefix_key + u'\uef5b',
    u'f7':                          prefix_key + u'\uef5c',
    u'f8':                          prefix_key + u'\uef5d',
    u'f9':                          prefix_key + u'\uef5e',
    u'f10':                         prefix_key + u'\uef5f',
    u'f11':                         prefix_key + u'\uef60',
    u'f12':                         prefix_key + u'\uef61',
    u'home':                        prefix_key + u'\uef62',
    u'insert':                      prefix_key + u'\uef63',
    u'page-down':                   prefix_key + u'\uef76',
    u'page-up':                     prefix_key + u'\uef77',
    u'pause':                       prefix_key + u'\uef78',
    u'print-screen':                prefix_key + u'\uef79',
    u'scroll-lock':                 prefix_key + u'\uef7b',
    u'shift-end':                   prefix_key + u'\uef7e',
    u'shift-f1':                    prefix_key + u'\uef7f',
    u'shift-f10':                   prefix_key + u'\uef80',
    u'shift-f11':                   prefix_key + u'\uef81',
    u'shift-f12':                   prefix_key + u'\uef82',
    u'shift-f2':                    prefix_key + u'\uef83',
    u'shift-f3':                    prefix_key + u'\uef84',
    u'shift-f4':                    prefix_key + u'\uef85',
    u'shift-f5':                    prefix_key + u'\uef86',
    u'shift-f6':                    prefix_key + u'\uef87',
    u'shift-f7':                    prefix_key + u'\uef88',
    u'shift-f8':                    prefix_key + u'\uef89',
    u'shift-f9':                    prefix_key + u'\uef8a',
    u'shift-home':                  prefix_key + u'\uef8b',
    u'shift-insert':                prefix_key + u'\uef8c',
    u'shift-kp-plus':               prefix_key + u'\uef8d',
    u'shift-pause':                 prefix_key + u'\uef91',
    u'shift-print-screen':          prefix_key + u'\uef92',
    u'shift-scroll-lock':           prefix_key + u'\uef94',
    u'shift-tab':                   prefix_key + u'\uef97',

    u'kp-divide':                   prefix_key + u'\uef64',
    u'kp-dot':                      prefix_key + u'\uef65',
    u'kp-enter':                    prefix_key + u'\uef66',
    u'kp-minus':                    prefix_key + u'\uef67',
    u'kp-multiple':                 prefix_key + u'\uef68',
    u'kp-plus':                     prefix_key + u'\uef69',
    u'kp0':                         prefix_key + u'\uef6a',
    u'kp1':                         prefix_key + u'\uef6b',
    u'kp2':                         prefix_key + u'\uef6c',
    u'kp3':                         prefix_key + u'\uef6d',
    u'kp4':                         prefix_key + u'\uef6e',
    u'kp5':                         prefix_key + u'\uef6f',
    u'kp6':                         prefix_key + u'\uef70',
    u'kp7':                         prefix_key + u'\uef71',
    u'kp8':                         prefix_key + u'\uef72',
    u'kp9':                         prefix_key + u'\uef73',
    u'num-lock':                    prefix_key + u'\uef75',
}

special_keys = {
#   Key code            trans           shift_trans         ctrl_trans          ctrl_shift_trans
    wx.WXK_BACK:        (u'backspace',  u'backspace',       u'ctrl-backspace',  u'ctrl-backspace'),
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
    wx.WXK_PAGEUP:      (u'page-up',    u'shift-page-up',   u'ctrl-page-up',    u'ctrl-shift-page-up'),
    wx.WXK_PAGEDOWN:    (u'page-down',  u'shift-page-down', u'ctrl-page-down',  u'ctrl-shift-page-down'),
    wx.WXK_END:         (u'end',        u'shift-end',       u'ctrl-end',        u'ctrl-shift-end'),
    wx.WXK_HOME:        (u'home',       u'shift-home',      u'ctrl-home',       u'ctrl-shift-home'),

    wx.WXK_LEFT:        (u'left',       u'left',            u'ctrl-left',       u'ctrl-left'),
    wx.WXK_UP:          (u'up',         u'up',              u'ctrl-up',         u'ctrl-up'),
    wx.WXK_RIGHT:       (u'right',      u'right',           u'ctrl-right',      u'ctrl-right'),
    wx.WXK_DOWN:        (u'down',       u'down',            u'ctrl-down',       u'ctrl-down'),

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

class EmacsPanel(wx.Panel):
    def __init__( self, app, parent ):
        wx.Panel.__init__( self, parent, -1 )
        app.log.debug( 'EmacsPanel.__init__()' )

        self.app = app
        self.log = app.log

        self.Bind( wx.EVT_PAINT, self.OnPaint )
        self.Bind( wx.EVT_KEY_DOWN, self.OnKeyDown )
        self.Bind( wx.EVT_KEY_UP, self.OnKeyUp )
        self.Bind( wx.EVT_CHAR, self.OnChar )

        self.Bind( wx.EVT_MOUSE_EVENTS, self.OnMouse )

        self.dc = None
        self.first_paint = True
        self.__all_term_ops = []
        self.editor_bitmap = None

        self.eat_next_char = False
        self.cursor_x = 1
        self.cursor_y = 1
        self.window_size = 0

        # point size and face need to choosen for platform
        if wx.Platform == '__WXMSW__':
            face = 'Courier New'
            point_size = 8

        elif wx.Platform == '__WXMAC__':
            face = 'Monaco'
            point_size = 12

        else:
            face = 'Courier'
            point_size = 12

        self.font = wx.Font( point_size, wx.FONTFAMILY_DEFAULT, wx.FONTSTYLE_NORMAL, wx.FONTWEIGHT_NORMAL, False, face )
        print 'Font face: %r' % (self.font.GetFaceName(),)

        self.client_padding = 3

        self.caret = wx.Caret( self, (1, 10) )
        self.caret.Move( (self.client_padding, self.client_padding) )
        self.caret.Hide()

        self.SetCaret( self.caret )

        # the size of a char on the screen
        self.char_width = None
        self.char_length = None
        # the size of the window
        self.pixel_width = None
        self.pixel_length = None
        # the size in chars of the window
        self.term_width = None
        self.term_length = None

        wx.EVT_SIZE( self, self.OnSize )

    def __debugTermCalls1( self, msg ):
        if _debug_term_calls1:
            self.log.debug( 'TERM %s' % (msg,) )

    def __debugTermCalls2( self, msg ):
        if _debug_term_calls2:
            self.log.debug( 'TERM %s' % (msg,) )

    def __debugTermInput( self, msg ):
        if _debug_term_input:
            self.log.debug( 'TERM %s' % (msg,) )

    def __calculateWindowSize( self ):
        if self.char_width is None:
            return

        self.pixel_width, self.pixel_length = self.GetClientSizeTuple()
        self.term_width  = min( MSCREENWIDTH,  (self.pixel_width  - 2*self.client_padding) // self.char_width )
        self.term_length = min( MSCREENLENGTH, (self.pixel_length - 2*self.client_padding) // self.char_length )

        self.log.debug( '__calculateWindowSize char: %dpx x %dpx window: %dpx X %dpx -> text window: %d X %d' %
                        (self.char_width, self.char_length
                        ,self.pixel_width, self.pixel_length
                        ,self.term_width, self.term_length) )

        self.log.debug( '__calculateWindowSize create editor_bitmap %d x %d' % (self.pixel_width, self.pixel_length) )
        self.editor_bitmap = wx.EmptyBitmap( self.pixel_width, self.pixel_length, -1 )

        dc = wx.MemoryDC()
        dc.SelectObject( self.editor_bitmap )

        dc.BeginDrawing()
        dc.SetBackgroundMode( wx.SOLID )
        dc.SetPen( wx.Pen( bg_colours[ SYNTAX_DULL ] ) )
        dc.SetBrush( wx.Brush( bg_colours[ SYNTAX_DULL ] ) )
        dc.DrawRectangle( 0, 0, self.pixel_width, self.pixel_length )
        dc.EndDrawing()
        dc = None

    def __pixelPoint( self, x, y ):
        return  (self.client_padding + self.char_width  * (x-1)
                ,self.client_padding + self.char_length * (y-1))
        

    def __geometryChanged( self ):
        self.__calculateWindowSize()

        if self.app.editor is None:
            self.log.debug( '__geometryChanged no self.app.editor' )
            return

        if self.char_width is None:
            self.log.debug( '__geometryChanged self.char_width is None' )
            return


        self.app.editor.guiGeometryChange( self.term_width, self.term_length )

    #--------------------------------------------------------------------------------
    #
    #   Event handlers
    #
    #--------------------------------------------------------------------------------
    def OnPaint( self, event ):
        if self.first_paint:
            self.log.debug( 'EmacsPanel.OnPaint() first paint' )
            self.first_paint = False

            dc = wx.PaintDC( self )
            dc.BeginDrawing()

            dc.SetBackgroundMode( wx.SOLID )
            dc.Clear()
            dc.SetFont( self.font )

            self.char_width, self.char_length = dc.GetTextExtent( 'M' )
            self.log.debug( 'OnPaint first_paint M %d.%d' % (self.char_width, self.char_length) )

            self.char_width, self.char_length = dc.GetTextExtent( 'i' )
            self.log.debug( 'OnPaint first_paint i %d.%d' % (self.char_width, self.char_length) )
            dc.EndDrawing()
            dc = None

            self.__calculateWindowSize()

            # queue up this action until after th rest of GUI init has happend
            self.app.onGuiThread( self.app.onEmacsPanelReady, () )

            # pass on?
            event.Skip()

        elif self.editor_bitmap is not None:
            self.log.debug( 'EmacsPanel.OnPaint() editor_bitmap' )
            dc  = wx.PaintDC( self )
            dc.BeginDrawing()
            dc.SetBackgroundMode( wx.SOLID )

            dc.DrawBitmap( self.editor_bitmap, 0, 0, False )
            dc.EndDrawing()

            dc = None

        else:
            self.log.debug( 'EmacsPanel.OnPaint() Nothing to do' )
            event.Skip()

    def getKeysMapping( self ):
        return keys_mapping

    def OnSize( self, event ):
        self.log.debug( 'EmacsPanel.OnSize()' )
        self.__geometryChanged()
        event.Skip()

    def OnKeyDown( self, event ):
        key = event.GetKeyCode()
        shift = event.ShiftDown()
        ctrl = event.ControlDown()
        self.__debugTermInput( 'OnKeyDown key %r name %r shift %s' % (key, wx_key_names.get( key, 'unknown' ), T( shift )) )

        if key in special_keys:
            trans, shift_trans, ctrl_trans, ctrl_shift_trans = special_keys[ key ]

            if ctrl and shift:
                translation = ctrl_shift_trans
                shift = False

            elif ctrl:
                translation = ctrl_trans

            elif shift:
                translation = shift_trans
                shift = False

            else:
                translation = trans

            print '1 translation %r' % (translation,)

            translation = keys_mapping[ translation ]
            print '2 translation %r' % (translation,)

            for ch in translation:
                self.app.editor.guiEventChar( ch, shift )

            self.eat_next_char = True

        event.Skip()

    def OnKeyUp( self, event ):
        event.Skip()

    def OnChar( self, event ):
        if self.eat_next_char:
            self.eat_next_char = False
            return

        char = event.GetUnicodeKey()
        line_parts = ['"%r" %r' % (unichr( char ), char)]

        key = event.GetKeyCode()
        line_parts.append( ' key %r' % (key,) )

        alt = event.AltDown()
        line_parts.append( ' alt: %s' % T(alt) )

        cmd = event.CmdDown()
        line_parts.append( ' cmd: %s' % T(cmd) )

        control = event.ControlDown()
        line_parts.append( ' control: %s' % T(control) )

        meta = event.MetaDown()
        line_parts.append( ' meta: %s' % T(meta) )

        shift = event.ShiftDown()
        line_parts.append( ' shift: %s' % T(shift) )

        self.__debugTermInput( (''.join( line_parts )).encode( 'utf-8' ) )

        self.app.editor.guiEventChar( unichr( char ), False )

    def OnMouse( self, event ):
        self.__debugTermInput( 'OnMouse() event_type %r %r' % (event.GetEventType(), wx_evt_names.get( event.GetEventType(), 'unknown' )) )

        # Calculate character cell position
        x, y = event.GetPosition()
        column = (x - self.client_padding + (self.char_width/2)) // self.char_width + 1;
        line =   (y - self.client_padding ) // self.char_length + 1;

        shift = event.ShiftDown()
        control = event.ControlDown()

        if event.Moving():
            pass

        elif event.IsButton():

            if event.LeftDown():
                button = 2
            elif event.LeftUp():
                button = 3
            elif event.MiddleDown():
                button = 4
            elif event.MiddleUp():
                button = 5
            elif event.RightDown():
                button = 6
            elif event.RightUp():
                button = 7
            else:
                self.log.info( 'Uknown button event' )
                return

            mouse = u'\x1b[%d;%d;%d;%d&w' % (button, 0, line, column)

            self.__debugTermInput( 'Mouse button %r line %r column %r shift %r' % (button, line, column, shift) )

            for ch in mouse:
                self.app.editor.guiEventChar( ch, shift )

        elif event.GetEventType() == wx.wxEVT_MOUSEWHEEL:
            self.__debugTermInput( 'Mouse Wheel rotation %r delta %r' % (event.GetWheelRotation(), event.GetWheelDelta()) )

            rotation = event.GetWheelRotation()
            if rotation > 0:
                mode = 0
            else:
                mode = 1

            if shift:
                mode |= 4

            if control:
                mode |= 8

            mouse = u'\x1b[%d;%d;%d;%d#w' % (mode, 1, line, column)

            for Q in range( abs( rotation ) ):
                for ch in mouse:
                    self.app.editor.guiEventChar( ch, False )

    #--------------------------------------------------------------------------------
    #
    #   terminal drawing API forwarded from bemacs editor
    #
    #--------------------------------------------------------------------------------
    def termTopos( self, y, x ):
        self.__debugTermCalls1( 'termTopos( y=%d, x=%d)' % (y, x) )
        self.cursor_x = x
        self.cursor_y = y

    def termReset( self ):
        self.__debugTermCalls1( 'termReset()' )
        #self.__all_term_ops = [(self.__termReset, ())]
        #self.RefreshRect( (0,0,self.pixel_width,self.pixel_length), True )

    def __termReset( self ):
        self.dc.Clear()

    def termInit( self ):
        self.__debugTermCalls1( 'termInit()' )

    def termBeep( self ):
        self.__debugTermCalls1( 'termBeep()' )

    def termUpdateBegin( self ):
        self.__debugTermCalls1( 'termUpdateBegin() ------------------------------------------------------------' )
        self.__all_term_ops.append( (self.__termUpdateBegin, ()) )

    def __termUpdateBegin( self ):
        self.__debugTermCalls2( '__termUpdateBegin() ----------------------------------------------------------' )

    def termUpdateEnd( self ):
        self.__debugTermCalls2( 'termUpdateEnd() --------------------------------------------------------------' )
        if self.editor_bitmap is None:
            self.__debugTermCalls2( 'termUpdateEnd editor_bitmap is None' )
            return

        self.dc = wx.MemoryDC()
        self.dc.SelectObject( self.editor_bitmap )

        self.dc.BeginDrawing()

        self.dc.SetBackgroundMode( wx.SOLID )
        self.dc.SetFont( self.font )

        self.__executeTermOperations()

        self.dc.EndDrawing()
        self.dc = None
        self.RefreshRect( (0, 0, self.pixel_width, self.pixel_length), False )

        self.caret.SetSize( (max( 1, self.char_width//5), self.char_length) )
        c_x, c_y = self.__pixelPoint( self.cursor_x, self.cursor_y )
        self.caret.Move( (c_x, c_y) )
        if not self.caret.IsVisible():
            self.caret.Show()

    def __executeTermOperations( self ):
        all_term_ops = self.__all_term_ops
        self.__all_term_ops = []

        for fn, args in all_term_ops:
            fn( *args )

    def termUpdateLine( self, old, new, row ):
        self.__debugTermCalls1( 'termUpdateLine row=%d' % (row,) )
        if old != new:
            self.__all_term_ops.append( (self.__termUpdateLine, (old, new, row)) )

    def __termUpdateLine( self, old, new, row ):
        if old is not None:
            self.__debugTermCalls2( '__termUpdateLine row=%d old %r' % (row, old[0].rstrip(),) )
        self.__debugTermCalls2( '__termUpdateLine row=%d new %r' % (row, new[0].rstrip(),) )

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
            x, y = self.__pixelPoint( col+1, row )

            new_attr = new_line_attrs[ col ]
            new_ch = new_line_contents[ col ]
            if( old_line_contents <= col
            and old_line_contents[ col ] == new_ch
            and old_line_attrs[ col ] == attr ):
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

            self.dc.DrawText( new_ch, x, y )

        remaining_width = self.term_width - new_line_length
        if remaining_width > 0:
            self.dc.SetPen( wx.Pen( bg_colours[ SYNTAX_DULL ] ) )
            self.dc.SetBrush( wx.Brush( bg_colours[ SYNTAX_DULL ] ) )
            x, y = self.__pixelPoint( new_line_length+1, row )
            self.dc.DrawRectangle( x, y, remaining_width*self.char_width, self.char_length )

    def termMoveLine( self, from_line, to_line ):
        self.__debugTermCalls1( 'termMoveLine( %r, %r )' % (from_line,to_line) )
        self.__all_term_ops.append( (self.__termMoveLine, (from_line, to_line)) )

    def __termMoveLine( self, from_line, to_line ):
        self.__debugTermCalls2( '__termMoveLine( %r, %r )' % (from_line,to_line) )
        if self.first_paint:
            # Need to init move of the window
            return

        dst_x, dst_y = self.__pixelPoint( 1, to_line )
        src_x, src_y = self.__pixelPoint( 1, from_line )
        width = self.char_width * self.term_width
        height = self.char_length

        self.__debugTermCalls2( '__termMoveLine dst_x %r, dst_y %r, width %r height %r src_x %r src_y %r' %
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
        self.__debugTermCalls1( 'termDisplayActivity( %r )' % (ch,) )
