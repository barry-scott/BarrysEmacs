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
import os
import wx
import logging

import be_ids
import be_exceptions
import be_version
import be_images
import be_emacs_panel
import be_preferences_dialog
import be_platform_specific

import be_config

class BemacsFrame(wx.Frame):
    status_spacer = 0
    status_readonly = 1
    status_overstrike = 2
    status_eol = 3
    status_line = 4
    status_column = 5
    status_num_fields = 6
    status_widths = [-1, 45, 45, 40, 80, 60]
    status_styles = [wx.SB_FLAT, wx.SB_RAISED, wx.SB_RAISED, wx.SB_RAISED, wx.SB_RAISED, wx.SB_RAISED]

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

        if wx.Platform == '__WXMAC__':
            # maps Cmd-Q to exit
            id_exit = wx.ID_EXIT

        else:
            id_exit = be_ids.id_exit

        self.menu_keys_to_id = {}
        self.menu_id_to_keys = {}

        self.menu_file = wx.Menu()
        self.menu_edit = wx.Menu()
        self.menu_help = wx.Menu()

        self.addEmacsMenu( self.menu_file, 'fo', T_('Open') )

        self.addEmacsMenu( self.menu_edit, 'ec', T_('Copy') )
        self.addEmacsMenu( self.menu_edit, 'ex', T_('Cut') )
        self.addEmacsMenu( self.menu_edit, 'ev', T_('Paste') )

        self.menu_file.Append( wx.ID_PREFERENCES, T_("&Preferences..."), T_("Preferences") )
        self.menu_file.Append( id_exit, T_("E&xit"), T_("Exit the application") )

        id_help_docs = wx.NewId()

        self.menu_help.Append( id_help_docs, T_('Documentation...') )
        self.menu_help.Append( wx.ID_ABOUT, T_("&About..."), T_("About the application") )

        self.menu_bar = wx.MenuBar()
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
        t.AddSimpleTool( self.getEmacsMenuId( 'ex' ),
            be_images.getBitmap( 'toolbar_images/editcut.png', bitmap_size ),
            T_('Cut Files and Folders'), T_('Cut Files and Folders') )
        t.AddSimpleTool( self.getEmacsMenuId( 'ec' ),
            be_images.getBitmap( 'toolbar_images/editcopy.png', bitmap_size ),
            T_('Copy Files and Folders'), T_('Copy Files and Folders') )
        t.AddSimpleTool( self.getEmacsMenuId( 'ev' ),
            be_images.getBitmap( 'toolbar_images/editpaste.png', bitmap_size ),
            T_('Paste Files and Folders'), T_('Paste Files and Folders') )

        t.Realize()

        # Add the status bar
        s = self.CreateStatusBar()
        s.SetFieldsCount( BemacsFrame.status_num_fields )
        s.SetStatusWidths( BemacsFrame.status_widths )
        s.SetStatusStyles( BemacsFrame.status_styles )

        s.SetStatusText( "", BemacsFrame.status_spacer )
        s.SetStatusText( "", BemacsFrame.status_readonly )
        s.SetStatusText( "", BemacsFrame.status_overstrike )
        s.SetStatusText( "", BemacsFrame.status_eol )
        s.SetStatusText( "", BemacsFrame.status_line )
        s.SetStatusText( "", BemacsFrame.status_column )

        # Create the main panel
        self.emacs_panel = be_emacs_panel.EmacsPanel( self.app, self )

        tw = be_exceptions.TryWrapperFactory( self.app.log )

        size = self.GetClientSize()

        # Set up the event handlers
        wx.EVT_MENU( self, wx.ID_ABOUT, tw( self.OnCmdAbout ) )
        wx.EVT_MENU( self, wx.ID_PREFERENCES, tw( self.OnCmdPreferences ) )
        wx.EVT_MENU( self, id_exit, tw( self.OnCmdExit ) )
        wx.EVT_MENU( self, id_help_docs, tw( self.OnHelpDocumentation ) )

        wx.EVT_SIZE( self, tw( self.OnSize ) )
        wx.EVT_MOVE( self, tw( self.OnMove ) )
        
        wx.EVT_CLOSE( self, tw( self.OnCloseWindow ) )

    def addEmacsMenu( self, menu, keys, title ):
        menu_id = wx.NewId()
        wx.EVT_MENU( self, menu_id, self.OnEmacsMenu )

        self.menu_id_to_keys[ menu_id ] = be_emacs_panel.prefix_menu + keys
        self.menu_keys_to_id[ keys ] = menu_id

        menu.Append( menu_id, title )

    def getEmacsMenuId( self, keys ):
        return self.menu_keys_to_id[ keys ]

    def OnEmacsMenu( self, event ):
        for ch in self.menu_id_to_keys[ event.GetId() ]:
            self.app.editor.guiEventChar( ch, False )

    # Status bar settings
    def setStatus( self, all_values ):
        s = self.GetStatusBar()
        s.SetStatusText( {True: 'READ', False: ''}[ all_values['readonly'] ], BemacsFrame.status_readonly )
        s.SetStatusText( {True: 'OVER', False: 'INS'}[ all_values['overstrike'] ], BemacsFrame.status_overstrike )
        s.SetStatusText( all_values['eol'].upper(), BemacsFrame.status_eol )
        s.SetStatusText( '%07d' % all_values['line'], BemacsFrame.status_line )
        s.SetStatusText( '%04d' % all_values['column'], BemacsFrame.status_column )

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
        pref_dialog = be_preferences_dialog.PreferencesDialog( self, self.app )
        rc = pref_dialog.ShowModal()
        if rc == wx.ID_OK:
            self.app.savePreferences()
            self.emacs_panel.newPreferences()

        self.log.error( 'Need to tell editor to change font' )

    def OnHelpDocumentation( self, event ):
        if '__WXMAC__' in wx.Platform:
            os.system( "open '%s'" % (be_platform_specific.getDocUserGuide(),) )

        elif '__WXGTK__' in wx.Platform:
            os.system( "gnome-open '%s'" % (be_platform_specific.getDocUserGuide(),) )

        elif '__WXMSW__' in wx.Platform:
            os.system( 'start "%s"' % (be_platform_specific.getDocUserGuide(),) )

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
