'''
 ====================================================================
 Copyright (c) 2003-2009 Barry A Scott.  All rights reserved.

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
_debug_term_calls2 = True

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

special_keys = {
#   Key code            trans         shift_trans ctrl_trans  ctrl_shift_trans
    wx.WXK_BACK:        ("\x7f",      None,       "\033[85~", None),
    wx.WXK_TAB:         ("\t",        "\033\t",   "\t",       "\033\t"),

    # function keys
    wx.WXK_F1:          ("\033[17~", "\033[117~", "\033[67~", "\033[167~"),
    wx.WXK_F2:          ("\033[18~", "\033[118~", "\033[68~", "\033[168~"),
    wx.WXK_F3:          ("\033[19~", "\033[119~", "\033[69~", "\033[169~"),
    wx.WXK_F4:          ("\033[20~", "\033[120~", "\033[70~", "\033[170~"),
    wx.WXK_F5:          ("\033[21~", "\033[121~", "\033[71~", "\033[171~"),
    wx.WXK_F6:          ("\033[23~", "\033[123~", "\033[73~", "\033[173~"),
    wx.WXK_F7:          ("\033[24~", "\033[124~", "\033[74~", "\033[174~"),
    wx.WXK_F8:          ("\033[25~", "\033[125~", "\033[75~", "\033[175~"),
    wx.WXK_F9:          ("\033[26~", "\033[126~", "\033[76~", "\033[176~"),
    wx.WXK_F10:         ("\033[28~", "\033[128~", "\033[78~", "\033[178~"),
    wx.WXK_F11:         ("\033[29~", "\033[129~", "\033[79~", "\033[179~"),
    wx.WXK_F12:         ("\033[31~", "\033[131~", "\033[81~", "\033[181~"),

    # enhanced keys
    wx.WXK_PAGEDOWN:    ("\033[5~",  "\033[105~", "\033[55~", "\033[155~"),
    wx.WXK_PAGEUP:      ("\033[6~",  "\033[106~", "\033[56~", "\033[156~"),
    wx.WXK_END:         ("\033[4~",  "\033[104~", "\033[54~", "\033[154~"),
    wx.WXK_HOME:        ("\033[1~",  "\033[101~", "\033[51~", "\033[151~"),

    wx.WXK_LEFT:        ("\033OD",   None,        "\033[60~", None,),
    wx.WXK_UP:          ("\033OA",   None,        "\033[57~", None,),
    wx.WXK_RIGHT:       ("\033OC",   None,        "\033[59~", None,),
    wx.WXK_DOWN:        ("\033OB",   None,        "\033[58~", None,),

    wx.WXK_INSERT:      ("\033[2~",  "\033[102~", "\033[52~", "\033[152~"),
    wx.WXK_DELETE:      ("\033[3~",  "\033[103~", "\033[53~", "\033[153~"),
    }

wx_key_names = {}
for name in dir(wx):
    if name.startswith( 'WXK_' ):
        wx_key_names[ getattr( wx, name ) ] = name

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

    def OnSize( self, event ):
        self.log.debug( 'EmacsPanel.OnSize()' )
        self.__geometryChanged()
        event.Skip()

    def OnKeyDown( self, event ):
        key = event.GetKeyCode()
        shift = event.ShiftDown()
        ctrl = event.ControlDown()
        self.log.debug( 'OnKeyDown key %r name %r shift %s' % (key, wx_key_names.get( key, 'unknown' ), T( shift )) )

        if key in special_keys:
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

        self.log.debug( (''.join( line_parts )).encode( 'utf-8' ) )

        self.app.editor.guiEventChar( unichr( char ), False )

    def OnMouse( self, event ):
        if event.Moving():
            pass

        elif event.IsButton():
            # Calculate character cell position
            x, y = event.GetPosition()
            column = (x - self.client_padding + (self.char_width/2)) // self.char_width + 1;
            line =   (y - self.client_padding ) // self.char_length + 1;

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

            mouse = "\x1b[%d;%d;%d;%d&w" % (button, 0, line, column)
            shift = event.ShiftDown()

            self.log.info( 'Mouse button %r line %r column %r shift %r' % (button, line, column, shift) )

            for ch in mouse:
                self.app.editor.guiEventChar( ch, shift )


            wheel = '''
        // Cygwin Xfree86 uses 4 and 5 to send the mouse wheel turns
        case Button4:
        case Button5:
        {
            if( event->type == ButtonPress )
            {
                int mode = 0;
                if( event->button == Button5 )
                    mode |= 1;    // -ive wheel motion
                if( (event->state & ShiftMask) != 0 )
                    mode |= 4;    // shift down
                if( (event->state & ControlMask) != 0 )
                    mode |= 8;    // ctrl down

                EmacsString mouse( FormatString("\x1b[%d;%d;%d;%d#w")
                    << mode << 1 << column << line );
                input_char_string( mouse, event->state & ShiftMask );
            }
            return;
        }
        default:
            _dbg_msg( FormatString( "Unexpected mouse button. event->button: %x" ) <<event->button );
            return;
        }
'''



    def __qqq__OnMouse( self, event ):
        if event.Moving():
            return


        button_down = []
        if event.LeftDown():
            button_down.append( 'l' )
        if event.MiddleDown():
            button_down.append( 'm' )
        if event.RightDown():
            button_down.append( 'r' )
        if event.LeftDClick():
            button_down.append( 'L' )
        if event.MiddleDClick():
            button_down.append( 'M' )
        if event.RightDClick():
            button_down.append( 'R' )

        button_up = []
        if event.LeftUp():
            button_up.append( 'l' )
        if event.MiddleUp():
            button_up.append( 'm' )
        if event.RightUp():
            button_up.append( 'r' )

        line_parts = ['Mouse %4d, %4d D: %3s U: %3s' % (event.GetX(), event.GetY(), ''.join( button_down ), ''.join( button_up ))]

        alt = event.AltDown()
        line_parts.append( ' alt: %s' % B(alt) )

        cmd = event.CmdDown()
        line_parts.append( ' cmd: %s' % B(cmd) )

        control = event.ControlDown()
        line_parts.append( ' control: %s' % B(control) )

        meta = event.MetaDown()
        line_parts.append( ' meta: %s' % B(meta) )

        shift = event.ShiftDown()
        line_parts.append( ' shift: %s' % B(shift) )

        self.log.debug( ''.join( line_parts ) )
        event.Skip()

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
