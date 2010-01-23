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

keys_mapping = {
    u'csi':                         u'\033[',
    u'backspace':                   u'\x7f',
    u'ctrl-backspace':              u'\033[85~',
    u'ctrl-delete':                 u'\033[53~',
    u'ctrl-down':                   u'\033[58~',
    u'ctrl-end':                    u'\033[54~',
    u'ctrl-f1':                     u'\033[67~',
    u'ctrl-f10':                    u'\033[78~',
    u'ctrl-f11':                    u'\033[79~',
    u'ctrl-f12':                    u'\033[81~',
    u'ctrl-f2':                     u'\033[68~',
    u'ctrl-f3':                     u'\033[69~',
    u'ctrl-f4':                     u'\033[70~',
    u'ctrl-f5':                     u'\033[71~',
    u'ctrl-f6':                     u'\033[73~',
    u'ctrl-f7':                     u'\033[74~',
    u'ctrl-f8':                     u'\033[75~',
    u'ctrl-f9':                     u'\033[76~',
    u'ctrl-home':                   u'\033[51~',
    u'ctrl-insert':                 u'\033[52~',
    u'ctrl-left':                   u'\033[60~',
    u'ctrl-mouse-wheel-neg':        u'\033[9#w',
    u'ctrl-mouse-wheel-pos':        u'\033[8#w',
    u'ctrl-page-down':              u'\033[56~',
    u'ctrl-page-up':                u'\033[55~',
    u'ctrl-pause':                  u'\033[84~',
    u'ctrl-print-screen':           u'\033[82~',
    u'ctrl-right':                  u'\033[59~',
    u'ctrl-scroll-lock':            u'\033[83~',
    u'ctrl-shift-delete':           u'\033[153~',
    u'ctrl-shift-down':             u'\033[158~',
    u'ctrl-shift-end':              u'\033[154~',
    u'ctrl-shift-f1':               u'\033[167~',
    u'ctrl-shift-f10':              u'\033[178~',
    u'ctrl-shift-f11':              u'\033[179~',
    u'ctrl-shift-f12':              u'\033[181~',
    u'ctrl-shift-f2':               u'\033[168~',
    u'ctrl-shift-f3':               u'\033[169~',
    u'ctrl-shift-f4':               u'\033[170~',
    u'ctrl-shift-f5':               u'\033[171~',
    u'ctrl-shift-f6':               u'\033[173~',
    u'ctrl-shift-f7':               u'\033[174~',
    u'ctrl-shift-f8':               u'\033[175~',
    u'ctrl-shift-f9':               u'\033[176~',
    u'ctrl-shift-home':             u'\033[151~',
    u'ctrl-shift-insert':           u'\033[152~',
    u'ctrl-shift-left':             u'\033[160~',
    u'ctrl-shift-mouse-wheel-neg':  u'\033[13#w',
    u'ctrl-shift-mouse-wheel-pos':  u'\033[12#w',
    u'ctrl-shift-page-down':        u'\033[156~',
    u'ctrl-shift-page-up':          u'\033[155~',
    u'ctrl-shift-pause':            u'\033[184~',
    u'ctrl-shift-print-screen':     u'\033[182~',
    u'ctrl-shift-right':            u'\033[159~',
    u'ctrl-shift-scroll-lock':      u'\033[183~',
    u'ctrl-shift-up':               u'\033[157~',
    u'ctrl-up':                     u'\033[57~',
    u'delete':                      u'\033[3~',
    u'down':                        u'\033[B',
    u'end':                         u'\033[4~',
    u'f1':                          u'\033[17~',
    u'f10':                         u'\033[28~',
    u'f11':                         u'\033[29~',
    u'f12':                         u'\033[31~',
    u'f2':                          u'\033[18~',
    u'f3':                          u'\033[19~',
    u'f4':                          u'\033[20~',
    u'f5':                          u'\033[21~',
    u'f6':                          u'\033[23~',
    u'f7':                          u'\033[24~',
    u'f8':                          u'\033[25~',
    u'f9':                          u'\033[26~',
    u'home':                        u'\033[1~',
    u'insert':                      u'\033[2~',
    u'kp-divide':                   u'\033OQ',
    u'kp-dot':                      u'\033On',
    u'kp-enter':                    u'\033OM',
    u'kp-minus':                    u'\033OS',
    u'kp-multiple':                 u'\033OR',
    u'kp-plus':                     u'\033Ol',
    u'kp0':                         u'\033Op',
    u'kp1':                         u'\033Oq',
    u'kp2':                         u'\033Or',
    u'kp3':                         u'\033Os',
    u'kp4':                         u'\033Ot',
    u'kp5':                         u'\033Ou',
    u'kp6':                         u'\033Ov',
    u'kp7':                         u'\033Ow',
    u'kp8':                         u'\033Ox',
    u'kp9':                         u'\033Oy',
    u'left':                        u'\033[D',
    u'menu':                        u'\202',
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
    u'num-lock':                    u'\033OP',
    u'page-down':                   u'\033[6~',
    u'page-up':                     u'\033[5~',
    u'pause':                       u'\033[34~',
    u'print-screen':                u'\033[32~',
    u'right':                       u'\033[C',
    u'scroll-lock':                 u'\033[33~',
    u'shift-delete':                u'\033[103~',
    u'shift-down':                  u'\033[108~',
    u'shift-end':                   u'\033[104~',
    u'shift-f1':                    u'\033[117~',
    u'shift-f10':                   u'\033[128~',
    u'shift-f11':                   u'\033[129~',
    u'shift-f12':                   u'\033[131~',
    u'shift-f2':                    u'\033[118~',
    u'shift-f3':                    u'\033[119~',
    u'shift-f4':                    u'\033[120~',
    u'shift-f5':                    u'\033[121~',
    u'shift-f6':                    u'\033[123~',
    u'shift-f7':                    u'\033[124~',
    u'shift-f8':                    u'\033[125~',
    u'shift-f9':                    u'\033[126~',
    u'shift-home':                  u'\033[101~',
    u'shift-insert':                u'\033[102~',
    u'shift-kp-plus':               u'\033Om',
    u'shift-left':                  u'\033[110~',
    u'shift-mouse-wheel-neg':       u'\033[5#w',
    u'shift-mouse-wheel-pos':       u'\033[4#w',
    u'shift-page-down':             u'\033[106~',
    u'shift-page-up':               u'\033[105~',
    u'shift-pause':                 u'\033[134~',
    u'shift-print-screen':          u'\033[132~',
    u'shift-right':                 u'\033[109~',
    u'shift-scroll-lock':           u'\033[133~',
    u'shift-up':                    u'\033[107~',
    u'ss3':                         u'\033O',
    u'up':                          u'\033[A',
    u'tab':                         u'\t',
    u'shift-tab':                   u'\033\t',
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
