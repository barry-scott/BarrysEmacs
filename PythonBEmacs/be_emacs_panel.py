'''
 ====================================================================
 Copyright (c) 2003-2008 Barry A Scott.  All rights reserved.

 This software is licensed as described in the file LICENSE.txt,
 which you should have received as part of this distribution.

 ====================================================================

    be_frame.py

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

def B( boolean ):
    if boolean:
        return 'T'
    else:
        return '_'

_debug_term_calls = True

class EmacsPanel(wx.Panel):
    def __init__( self, app, parent ):
        wx.Panel.__init__( self, parent, -1 )
        app.log.info( 'EmacsPanel.__init__()' )

        self.app = app
        self.log = app.log

        self.Bind( wx.EVT_PAINT, self.OnPaint )
        self.Bind( wx.EVT_KEY_DOWN, self.OnKeyDown )
        self.Bind( wx.EVT_KEY_UP, self.OnKeyUp )
        self.Bind( wx.EVT_CHAR, self.OnChar )

        self.Bind( wx.EVT_MOUSE_EVENTS, self.OnMouse )

        self.all_lines = ['']*100

        self.first_paint = True

        self.font = wx.Font( 14, wx.FONTFAMILY_DEFAULT, wx.FONTSTYLE_NORMAL, wx.FONTWEIGHT_NORMAL, False, 'AndaleMono' )
        print 'Font face: %r' % (self.font.GetFaceName(),)

        self.caret = wx.Caret( self, (1, 10) )
        self.caret.Move( (2, 2) )
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


    def __debugTermCalls( self, msg ):
        if _debug_term_calls:
            self.log.debug( 'Debug: Term call %s' % (msg,) )

    def __calculateWindowSize( self ):
        self.pixel_width, self.pixel_length = self.GetClientSizeTuple()
        self.term_width = self.pixel_width // self.char_width
        self.term_length = self.pixel_length // self.char_length

        self.log.debug( '__calculateWindowSize char: %dpx x %dpx window: %dpx X %dpx -> text window: %d X %d' %
                        (self.char_width, self.char_length
                        ,self.pixel_width, self.pixel_length
                        ,self.term_width, self.term_length) )

    def __pixelPoint( self, x, y ):
        return  (2 + self.char_width  * (x-1)
                ,2 + self.char_length * (y-1))
        

    def __geometryChanged( self ):
        if self.app.editor is None:
            self.log.debug( '__geometryChanged no self.app.editor' )
            return

        if self.char_width is None:
            self.log.debug( '__geometryChanged self.char_width is None' )
            return

        self.__calculateWindowSize()
        self.app.editor.guiGeometryChange( self.term_width, self.term_length )

    #--------------------------------------------------------------------------------
    #
    #   Event handlers
    #
    #--------------------------------------------------------------------------------
    def OnPaint( self, event ):
        self.log.info( 'EmacsPanel.OnPaint()' )
        if self.first_paint:
            self.first_paint = False

            self.testFont()

            dc = wx.PaintDC( self )
            dc.BeginDrawing()

            dc.SetBackgroundMode( wx.SOLID )
            dc.Clear()
            dc.SetFont( self.font )

            self.char_width, self.char_length = dc.GetTextExtent( 'M' )
            print 'OnPaint first_paint %d.%d' % (self.char_width, self.char_length)
            dc.EndDrawing()

            self.__calculateWindowSize()

            # queue up this action until after th rest of GUI init has happend
            self.app.onGuiThread( self.app.onEmacsPanelReady, () )



        else:
            self.__drawPanel( wx.PaintDC( self ) )

    def OnSize( self, event ):
        self.log.info( 'EmacsPanel.OnSize()' )
        self.__geometryChanged()
        event.Skip()

    def OnKeyDown( self, event ):
        event.Skip()

    def OnKeyUp( self, event ):
        event.Skip()

    def OnChar( self, event ):
        char = event.GetUnicodeKey()
        line_parts = ['"%s" %r' % (unichr( char ), char)]

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

        self.app.editor.guiEventChar( unichr( char ), shift )

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
        self.__drawPanel( wx.ClientDC( self ) )

    #--------------------------------------------------------------------------------
    #
    #   terminal drawing API forwarded from bemacs editor
    #
    #--------------------------------------------------------------------------------
    def __drawPanel( self, dc ):
        dc.BeginDrawing()

        dc.SetBackgroundMode( wx.SOLID )
        dc.SetFont( self.font )

        #for ch in ('M',):
        #    for num in range( 1, 16, 1 ):
        #        s = ch * num
        #        x, y = dc.GetTextExtent( s )
        #        print '__drawPanel %24s %3d %d %.2f' % (s, x, y, float(x)/num)

        for index, line in enumerate( self.all_lines ):
            x, y = self.__pixelPoint( 1, index+1 )
            dc.DrawText( line, x, y )

        dc.EndDrawing()

    def termTopos( self, y, x ):
        dc = wx.ClientDC( self )

        dc.BeginDrawing()
        dc.SetBackgroundMode( wx.SOLID )
        dc.SetPen( wx.RED_PEN )
        c_x, c_y = self.__pixelPoint( x, y )
        dc.DrawLine( c_x, 0, c_x, 200 )
        dc.EndDrawing()

        self.caret.SetSize( (max( 1, self.char_width//5), self.char_length) )
        c_x, c_y = self.__pixelPoint( x, y )
        self.caret.Move( (c_x, c_y) )
        if not self.caret.IsVisible():
            self.caret.Show()

    def termReset( self ):
        self.__debugTermCalls( 'termReset()' )

        dc = wx.ClientDC( self )

        dc.BeginDrawing()

        dc.SetBackgroundMode( wx.SOLID )
        dc.Clear()
        dc.EndDrawing()

    def termInit( self ):
        self.__debugTermCalls( 'termInit()' )

    def termBeep( self ):
        self.__debugTermCalls( 'termBeep()' )

    def termUpdateBegin( self ):
        self.__debugTermCalls( 'termUpdateBegin()' )

    def termUpdateEnd( self ):
        self.__debugTermCalls( 'termUpdateEnd()' )
        self.__drawPanel( wx.ClientDC( self ) )

    def termUpdateLine( self, old, new, line_num ):
        self.__debugTermCalls( 'termUpdateLine( ..., %r )' % (line_num,) )

        if self.first_paint:
            # Need to init move of the window
            return

        self.__debugTermCalls( 'old %r' % (old,) )
        self.__debugTermCalls( 'new %r' % (new,) )
        if new is None:
            new_line_contents = (' '*self.term_width)
        else:
            new_line_contents = new[0] + (' '*(self.term_width-len(new[0])))

        self.all_lines[ line_num-1 ] = new_line_contents

    def termWindow( self, size ):
        self.__debugTermCalls( 'termWindow( %r )' % (size,) )

    def termInsertMode( self, mode ):
        self.__debugTermCalls( 'termInsertMode( %r )' % (mode,) )

    def termHighlightMode( self, mode ):
        self.__debugTermCalls( 'termHighlightMode( %r )' % (mode,) )

    def termInsertLines( self, num_lines ):
        self.__debugTermCalls( 'termInsertLines( %r )' % (num_lines,) )

    def termDeleteLines( self, num_lines ):
        self.__debugTermCalls( 'termDeleteLines( %r )' % (num_lines,) )

    def termDisplayActivity( self, ch ):
        self.__debugTermCalls( 'termDisplayActivity( %r )' % (ch,) )


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
