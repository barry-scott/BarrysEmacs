'''
 ====================================================================
 Copyright (c) 2003-2010 Barry A Scott.  All rights reserved.

 This software is licensed as described in the file LICENSE.txt,
 which you should have received as part of this distribution.

 ====================================================================

    be_frame.py

    Based on code from pysvn WorkBench

'''
import sys
import wx
import logging

import be_ids
import be_exceptions
import be_version
import be_images
import be_emacs_panel

import be_config

class BemacsFrame(wx.Frame):
    status_general = 0
    status_progress = 1
    status_action = 2
    status_num_fields = 3
    status_widths = [-1, 150, -2]

    def __init__( self, app ):
        self.app = app
        self.log = self.app.log

        title = T_("Barry's Emacs")

        win_prefs = self.app.prefs.getWindow()

        extra_style = 0
        if win_prefs.maximized:
            extra_style = wx.MAXIMIZE

        wx.Frame.__init__(self, None, -1, title,
                (-1, -1),
                win_prefs.frame_size,
                wx.DEFAULT_FRAME_STYLE|extra_style )

        # Reset the size after startup to workaround a potential
        # problem on OSX with incorrect first size event saving the
        # wrong size in the preferences
        wx.CallAfter( self.SetSize, win_prefs.frame_size )

        self.menu_edit = wx.Menu()

        if wx.Platform != '__WXMAC__':
            self.menu_file = wx.Menu()
        else:
            self.menu_file = self.menu_edit

        self.menu_file.Append( wx.ID_PREFERENCES, T_("&Preferences..."), T_("Preferences") )
        self.menu_file.Append( wx.ID_EXIT, T_("E&xit"), T_("Exit the application") )

        self.menu_help = wx.Menu()
        self.menu_help.Append( wx.ID_ABOUT, T_("&About..."), T_("About the application") )

        self.menu_bar = wx.MenuBar()
        if wx.Platform != '__WXMAC__':
            self.menu_bar.Append( self.menu_file, T_("&File") )
        self.menu_bar.Append( self.menu_edit, T_("&Edit") )
        self.menu_bar.Append( self.menu_help, T_("&Help") )

        self.SetMenuBar( self.menu_bar )

        # Set the application icon
        self.SetIcon( be_images.getIcon( 'bemacs.png') )

        # Add tool bar
        t = self.CreateToolBar( name="main",
                                style=wx.TB_HORIZONTAL ) # | wx.NO_BORDER | wx.TB_TEXT )

        bitmap_size = (32, 32)
        t.SetToolBitmapSize( bitmap_size )
        t.AddSimpleTool( be_ids.id_SP_EditCut,
            be_images.getBitmap( 'toolbar_images/editcut.png', bitmap_size ),
            T_('Cut Files and Folders'), T_('Cut Files and Folders') )
        t.AddSimpleTool( be_ids.id_SP_EditCopy,
            be_images.getBitmap( 'toolbar_images/editcopy.png', bitmap_size ),
            T_('Copy Files and Folders'), T_('Copy Files and Folders') )
        t.AddSimpleTool( be_ids.id_SP_EditPaste,
            be_images.getBitmap( 'toolbar_images/editpaste.png', bitmap_size ),
            T_('Paste Files and Folders'), T_('Paste Files and Folders') )

        t.Realize()

        # Add the status bar
        s = self.CreateStatusBar()
        s.SetFieldsCount( BemacsFrame.status_num_fields )
        s.SetStatusWidths( BemacsFrame.status_widths )
        s.SetStatusText( T_("Barry's Emacs"), BemacsFrame.status_general )
        s.SetStatusText( "", BemacsFrame.status_progress )
        s.SetStatusText( T_("Ready"), BemacsFrame.status_action )

        # Create the main panel
        self.emacs_panel = be_emacs_panel.EmacsPanel( self.app, self )

        tw = be_exceptions.TryWrapperFactory( self.app.log )

        size = self.GetClientSize()

        # for some unknown reason MENU events get blocked by tree and list controls
        for event_source in [self]:
            # Set up the event handlers
            wx.EVT_MENU( event_source, wx.ID_ABOUT, tw( self.OnCmdAbout ) )
            wx.EVT_MENU( event_source, wx.ID_PREFERENCES, tw( self.OnCmdPreferences ) )
            wx.EVT_MENU( event_source, wx.ID_EXIT, tw( self.OnCmdExit ) )

        wx.EVT_SIZE( self, tw( self.OnSize ) )
        wx.EVT_MOVE( self, tw( self.OnMove ) )
        
        wx.EVT_CLOSE( self, tw( self.OnCloseWindow ) )

    # Status bar settings
    def setStatus( self, text ):
        self.GetStatusBar().SetStatusText( text, BemacsFrame.status_general )

    def setProgress( self, text ):
        self.GetStatusBar().SetStatusText( text, BemacsFrame.status_progress )

    def setAction( self, text ):
        self.GetStatusBar().SetStatusText( text, BemacsFrame.status_action )

    def savePreferences( self ):
        win_prefs = self.app.prefs.getWindow()
        # Size and Position are already saved
        win_prefs.maximized = self.IsMaximized()

    # Handler for the Exit menu command
    def OnCmdExit(self, event):
        self.Close()

    # Handler for the About menu command
    def OnCmdAbout(self, event):
        ver_str = ('%d.%d.%d-%d\n' %
                    (be_version.major, be_version.minor,
                     be_version.patch, be_version.build))
        str_message =    ((T_("Barry's Emacs version: %s") % ver_str) +
                '\nwxPython %d.%d.%d.%d %s' % wx.VERSION +
                '\nPython %d.%d.%d %s %d\n' % sys.version_info +
                T_('\nCopyright Barry Scott (c) 1980-2010. All rights reserved')
                )
        wx.LogMessage( str_message )

    def OnCmdPreferences( self, event ):
        pref_dialog = wb_preferences_dialog.PreferencesDialog( self, self.app )
        rc = pref_dialog.ShowModal()
        if rc == wx.ID_OK:
            self.app.savePreferences()

        self.list_panel.updateHandler()
        self.app.refreshFrame()

    def OnSize( self, event ):
        if not self.IsMaximized():
            self.app.prefs.getWindow().frame_size = self.GetSize()
        event.Skip()

    def OnMove( self, event ):
        if not self.IsMaximized() and not self.IsIconized():
            # don't use the event.GetPosition() as it
            # is off by the window frame thinkness
            pt = self.GetPosition()
            self.app.prefs.getWindow().frame_position = pt
        self.app.prefs.getWindow().is_maximized = self.IsMaximized()
        event.Skip()

    def OnZoom(self, evt):
        win_prefs = self.app.prefs.getWindow()
        win_prefs.zoom = self.log_panel.GetZoom()
        
    #------------------------------------------------------------------------
    def OnCloseWindow( self, event ):
        self.log.info( 'OnCloseWindow()' )
        self.app.savePreferences()
        self.app.onCloseEditor()
