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

class EmacsPanel(wx.Panel):
    def __init__( self, app, parent ):
        wx.Panel.__init__(self, parent, -1)
        app.log.info( 'EmacsPanel.__init__()' )

        self.app = app

        self.Bind( wx.EVT_PAINT, self.OnPaint )
        self.Bind( wx.EVT_KEY_DOWN, self.OnKeyDown )
        self.Bind( wx.EVT_KEY_UP, self.OnKeyUp )
        self.Bind( wx.EVT_CHAR, self.OnChar )

        self.Bind( wx.EVT_MOUSE_EVENTS, self.OnMouse )

        self.all_lines = ['Emacs Panel']

    def __addLine( self, line ):
        self.all_lines.append( line + '      ' )
        self.all_lines = self.all_lines[-6:]

    def OnPaint( self, event ):
        self.app.log.info( 'EmacsPanel.OnPaint()' )
        self.__drawPanel()

    def __drawPanel( self ):
        dc = wx.PaintDC( self )
        dc.BeginDrawing()

        dc.SetBackgroundMode( wx.SOLID )
        dc.Clear()

        for index, line in enumerate( self.all_lines ):
            dc.DrawText( line, 10, 10+(index*20) )

        dc.EndDrawing()

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

        self.__addLine( ''.join( line_parts ) )
        event.Skip()
        self.__drawPanel()

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

        self.__addLine( ''.join( line_parts ) )
        event.Skip()
        self.__drawPanel()
