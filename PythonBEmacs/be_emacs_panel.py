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

attr_padding = [0]*MSCREENWIDTH
line_padding = ' '*MSCREENWIDTH


special_keys = {
    wx.WXK_UP:      '\033' '[A',
    wx.WXK_DOWN:    '\033' '[B',
    wx.WXK_RIGHT:   '\033' '[C',
    wx.WXK_LEFT:    '\033' '[D',
    wx.WXK_HOME:    '\033' '[H',
    wx.WXK_END:     '\033' '[F',
    wx.WXK_PAGEUP:  '\033' '[5~',
    wx.WXK_PAGEDOWN:'\033' '[6~',
    wx.WXK_F1:      '\033' '[11~',
    wx.WXK_F2:      '\033' '[12~',
    wx.WXK_F3:      '\033' '[13~',
    wx.WXK_F4:      '\033' '[14~',
    wx.WXK_F5:      '\033' '[15~',
    wx.WXK_F6:      '\033' '[17~',
    wx.WXK_F7:      '\033' '[18~',
    wx.WXK_F8:      '\033' '[19~',
    wx.WXK_F9:      '\033' '[20~',
    wx.WXK_F10:     '\033' '[21~',
    wx.WXK_F11:     '\033' '[23~',
    wx.WXK_F12:     '\033' '[24~',
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

        self.all_lines = [' '*MSCREENWIDTH]*MSCREENLENGTH
        self.all_attrs = [[0]*MSCREENWIDTH]*MSCREENLENGTH

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

        self.caret = wx.Caret( self, (1, 10) )
        self.caret.Move( (2, 2) )
        self.caret.Hide()

        self.SetCaret( self.caret )

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

            editor_dc = wx.MemoryDC()
            editor_dc.SelectObjectAsSource( self.editor_bitmap )

            dc.Blit( 0, 0, self.pixel_width, self.pixel_length, editor_dc, 0, 0 )
            dc.EndDrawing()

            editor_dc = None
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
        self.log.debug( 'OnKeyDown key %r name %r shift %s' % (key, wx_key_names.get( key, 'unknown' ), T( shift )) )

        if key == wx.WXK_F1:
            self.testF1()
            return

        if key == wx.WXK_F2:
            self.testF2()
            return

        if key == wx.WXK_F3:
            self.testF3()
            return

        if key in special_keys:
            for ch in special_keys.get( key, '' ):
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
    def __drawPanel( self, dc ):
        dc.BeginDrawing()

        dc.SetBackgroundMode( wx.SOLID )
        dc.SetFont( self.font )

        cur_mode = None

        for row in range( self.term_length ):
            line = self.all_lines[ row ]
            attrs = self.all_attrs[ row ]

            for col in range( self.term_width ):
                x, y = self.__pixelPoint( col+1, row+1 )

                attr = attrs[col]

                if (attr & LINE_M_ATTR_HIGHLIGHT) != 0:
                    mode = LINE_M_ATTR_HIGHLIGHT

                elif (attr & LINE_ATTR_MODELINE) != 0:
                    mode = LINE_ATTR_MODELINE

                elif (attr&LINE_ATTR_USER) != 0:
                    mode = attr&LINE_M_ATTR_USER

                else:
                    mode = attr

                if cur_mode != mode:
                    cur_mode = mode
                    dc.SetTextBackground( bg_colours[ cur_mode ] ) 
                    dc.SetTextForeground( fg_colours[ cur_mode ] ) 

                dc.DrawText( line[col], x, y )

        dc.EndDrawing()

    def __drawPanel2( self, dc ):
        dc.BeginDrawing()

        dc.SetBackgroundMode( wx.SOLID )
        dc.SetFont( self.font )

        cur_mode = None

        for row in range( self.term_length ):
            line = self.all_lines[ row ]
            attrs = self.all_attrs[ row ]

            all_text = []
            all_coord = []
            all_fg = []
            all_bg = []

            for col in range( self.term_width ):
                x, y = self.__pixelPoint( col+1, row+1 )

                attr = attrs[col]

                if (attr & LINE_M_ATTR_HIGHLIGHT) != 0:
                    mode = LINE_M_ATTR_HIGHLIGHT

                elif (attr & LINE_ATTR_MODELINE) != 0:
                    mode = LINE_ATTR_MODELINE

                elif (attr&LINE_ATTR_USER) != 0:
                    mode = attr&LINE_M_ATTR_USER

                else:
                    mode = attr

                if cur_mode != mode:
                    cur_mode = mode

                all_text.append( line[col] )
                all_coord.append( (x, y) )
                all_fg.append( fg_colours[ cur_mode ] )
                all_bg.append( bg_colours[ cur_mode ] )

            dc.DrawTextList( all_text, all_coord, all_fg, all_bg )
        dc.EndDrawing()

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

        row -= 1

        if new is None:
            new_line_contents = ' '*self.term_width
            new_line_attrs = attr_padding
        else:
            new_line_contents = new[0] + line_padding[ 0 : self.term_width-len(new[0]) ]
            new_line_attrs = new[1] + attr_padding[ 0 : self.term_width-len(new[0]) ]

        self.all_lines[ row ] = new_line_contents
        self.all_attrs[ row ] = new_line_attrs

        dc = self.dc

        cur_mode = None

        for col in range( self.term_width ):
            x, y = self.__pixelPoint( col+1, row+1 )

            attr = new_line_attrs[ col ]

            if (attr & LINE_M_ATTR_HIGHLIGHT) != 0:
                mode = LINE_M_ATTR_HIGHLIGHT

            elif (attr & LINE_ATTR_MODELINE) != 0:
                mode = LINE_ATTR_MODELINE

            elif (attr&LINE_ATTR_USER) != 0:
                mode = attr&LINE_M_ATTR_USER

            else:
                mode = attr

            if cur_mode != mode:
                cur_mode = mode
                dc.SetTextForeground( fg_colours[ cur_mode ] ) 
                dc.SetTextBackground( bg_colours[ cur_mode ] ) 

            dc.DrawText( new_line_contents[ col ], x, y )

    def termMoveLine( self, from_line, to_line ):
        self.__debugTermCalls1( 'termMoveLine( %r, %r )' % (from_line,to_line) )
        self.__all_term_ops.append( (self.__termMoveLine, (from_line, to_line)) )

    def __termMoveLine( self, from_line, to_line ):
        self.__debugTermCalls2( '__termMoveLine( %r, %r )' % (from_line,to_line) )
        if self.first_paint:
            # Need to init move of the window
            return

        self.all_lines[ to_line-1 ] = self.all_lines[ from_line-1 ]
        self.all_lines[ from_line-1 ] = line_padding 

        self.all_attrs[ to_line-1 ] = self.all_attrs[ from_line-1 ]
        self.all_attrs[ from_line-1 ] = attr_padding

        dst_x, dst_y = self.__pixelPoint( 1, to_line )
        src_x, src_y = self.__pixelPoint( 1, from_line )
        width = self.char_width * self.term_width
        height = self.char_length

        if wx.Platform == '__WXMAC__qqq':
            bottom_margin = self.pixel_length - (self.char_length*self.term_length) - self.client_padding
            src_y = (self.term_length-(from_line-1))*self.char_length + bottom_margin

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

    def testFont( self ):
        dc = wx.ClientDC( self )

        dc.BeginDrawing()

        text1 = 'III'
        text2 = 'MMM'
        folist = ['AndaleMono', 'Monaco', 'Courier', 'Courier New', 'Lucida Console']

        for e in folist:
            fo = wx.Font( 14, wx.FONTFAMILY_DEFAULT, wx.FONTSTYLE_NORMAL, wx.FONTWEIGHT_NORMAL, False, e )
            dc.SetFont( fo )
            print e, fo.GetFaceName()

            for i in range( 1, 5, 1 ):
                x1 = dc.GetTextExtent( text1 * i )
                x2 = dc.GetTextExtent( text2 * i )
                print float( x1[0] )/float( i*len( text1 ) ), float( x2[0] )/float( i*len( text2 ) )

        dc.EndDrawing()

    def testF1( self ):
        self.qqq = 0
        self.testBlit( self.qqq )

    def testF2( self ):
        self.qqq += 1
        self.testBlit( self.qqq )

    def testF3( self ):
        self.qqq -= 1
        self.testBlit( self.qqq )

    def testBlit( self, n ):

        dc = wx.ClientDC( self )

        dc.BeginDrawing()

        bottom_margin = self.pixel_length - (self.char_length*self.term_length) - self.client_padding

        dst_x, dst_y = 200, 2
        src_x, src_y = 2, self.char_length*n + bottom_margin
        width = 100
        height = self.char_length


        self.__debugTermCalls1( 'testF1 n %r bm %r dst_x %r, dst_y %r, width %r height %r src_x %r src_y %r' %
                (n, bottom_margin
                ,dst_x, dst_y
                ,width, height
                ,src_x, src_y) )

        dc.Blit(
            dst_x, dst_y,
            width, height,
            dc,
            src_x, src_y,
            wx.COPY,
            False, -1, -1
            )

        dc.EndDrawing()
