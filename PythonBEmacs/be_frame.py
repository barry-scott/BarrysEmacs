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

class BemacsFrame(wx.Frame):
    status_general = 0
    status_progress = 1
    status_action = 2
    status_num_fields = 3
    status_widths = [-1, 150, -2]

    def __init__( self, app ):
        self.app = app
        title = T_("Barry's Emacs")

        win_prefs = self.app.prefs.getWindow()

        extra_style = 0
        if win_prefs.maximized:
            extra_style = wx.MAXIMIZE

        wx.Frame.__init__(self, None, -1, title,
                win_prefs.frame_position,
                win_prefs.frame_size,
                wx.DEFAULT_FRAME_STYLE|extra_style )

        # Reset the size after startup to workaround a potential
        # problem on OSX with incorrect first size event saving the
        # wrong size in teh preferences
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
        self.SetIcon( be_images.getIcon( 'wb.png') )

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

        # Create the splitter windows
        if 'wxMac' in wx.PlatformInfo:
            style = wx.SP_LIVE_UPDATE | wx.SP_3DSASH
        else:
            style = wx.SP_LIVE_UPDATE
        self.h_split = wx.SplitterWindow( self, -1, style=style )
        self.v_split = wx.SplitterWindow( self.h_split, -1, style=style )

        # Make sure the splitters can't be removed by setting a minimum size
        self.v_split.SetMinimumPaneSize( 100 )
        self.h_split.SetMinimumPaneSize( 100 )

        # Create the main panels
        self.log_panel = LogCtrlPanel( self.app, self.h_split )

        try_wrapper = be_exceptions.TryWrapperFactory( self.app.log )

        size = self.GetClientSize()

        # for some unknown reason MENU events get blocked by tree and list controls
        for event_source in [self]:
            # Set up the event handlers
            wx.EVT_MENU( event_source, wx.ID_ABOUT, try_wrapper( self.OnCmdAbout ) )
            wx.EVT_MENU( event_source, wx.ID_PREFERENCES, try_wrapper( self.OnCmdPreferences ) )
            wx.EVT_MENU( event_source, wx.ID_EXIT, try_wrapper( self.OnCmdExit ) )

        wx.EVT_SIZE( self, self.OnSize )
        wx.EVT_MOVE( self, self.OnMove )

        wx.stc.EVT_STC_ZOOM( self, -1, self.OnZoom )
        
        wx.EVT_CLOSE( self, try_wrapper( self.OnCloseWindow ) )

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
                T_('\nCopyright Barry Scott (c) 2003-2008. All rights reserved')
                )
        wx.LogMessage( str_message )

    def OnCmdPreferences( self, event ):
        pref_dialog = wb_preferences_dialog.PreferencesDialog( self, self.app )
        rc = pref_dialog.ShowModal()
        if rc == wx.ID_OK:
            self.app.savePreferences()

        self.list_panel.updateHandler()
        self.refreshFrame()

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
    def OnActivateApp( self, is_active ):
        if is_active and self.app.prefs.getView().auto_refresh:
            self.refreshFrame()

    def OnRefresh( self, event ):
        self.app.log.debug( 'OnRefresh()' )
        self.refreshFrame()

    def refreshFrame( self ):
        self.app.log.debug( 'BemacsFrame.refreshFrame()' )

    #------------------------------------------------------------------------
    def OnCmdClearLog( self, event ):
        self.log_panel.ClearLog()

    def OnCloseWindow( self, event ):
        if self.app.exitAppNow():
            self.Destroy()


#--------------------------------------------------------------------------------
class LogCtrlPanel(wx.Panel):
    def __init__( self, app, parent ):
        wx.Panel.__init__(self, parent, -1)

        self.app = app
        self.text_ctrl = StyledLogCtrl( self.app, self )

        # Redirect the console IO to this panel
        sys.stdin = file( be_platform_specific.getNullDevice(), 'r' )
        if self.app.isStdIoRedirect():
            sys.stdout = self
            sys.stderr = self

        # Redirect log to the Log panel
        log_handler = LogHandler( self.text_ctrl )
        self.app.log.addHandler( log_handler )

        wx.EVT_SIZE( self, be_exceptions.TryWrapper( self.app.log, self.OnSize ) )


    #---------- Event handlers ------------------------------------------------------------
    def OnSize( self, event ):
        self.text_ctrl.SetWindowSize( self.GetSize() )

    #---------- Public methods ------------------------------------------------------------
    def write( self, string ):
        # only allowed to use GUI objects on the foreground thread
        if not self.app.isMainThread():
            self.app.foregroundProcess( self.write, (string,) )
            return

        if string[:6] == 'Error:':
            self.text_ctrl.WriteError(string)

        elif string[:5] == 'Info:':
            self.text_ctrl.WriteInfo(string)

        elif string[:8] == 'Warning:':
            self.text_ctrl.WriteWarning(string)

        elif string[:5] == 'Crit:':
            self.text_ctrl.WriteCritical(string)

        else:
            self.text_ctrl.WriteNormal(string)

        if not self.app.isStdIoRedirect():
            sys.__stdout__.write( string )

    def close( self ):
        pass

    def ClearLog( self ):
        self.text_ctrl.ClearText()

    def GetZoom(self):
        return self.text_ctrl.GetZoom()

    def SetZoom(self, zoom):
        self.text_ctrl.SetZoom(zoom)

#--------------------------------------------------------------------------------
class LogHandler(logging.Handler):
    def __init__( self, log_ctrl ):
        self.log_ctrl = log_ctrl
        logging.Handler.__init__( self )

    def emit( self, record ):
        try:
            msg = self.format(record) + '\n'
            level = record.levelno

            if level >= logging.CRITICAL:
                self.log_ctrl.WriteCritical( msg )

            elif level >= logging.ERROR:
                self.log_ctrl.WriteError( msg )

            elif level >= logging.WARNING:
                self.log_ctrl.WriteWarning( msg )

            elif level >= logging.INFO:
                self.log_ctrl.WriteInfo( msg )

            elif level >= logging.DEBUG:
                self.log_ctrl.WriteDebug( msg )

            else:
                self.log_ctrl.WriteError( msg )

        except Exception:
            self.handleError(record)

#--------------------------------------------------------------------------------
class StyledLogCtrl(wx.stc.StyledTextCtrl):
    'StyledLogCtrl'
    def __init__(self, app, parent):
        self.app = app

        wx.stc.StyledTextCtrl.__init__(self, parent)
        self.SetReadOnly( True )

        self.style_normal = 0
        self.style_error = 1
        self.style_info = 2
        self.style_warning = 3
        self.style_critical = 4
        self.style_debug = 4

        self.SetMarginWidth(0, 0)
        self.SetMarginWidth(1, 0)
        self.SetMarginWidth(2, 0)

        self.StyleSetSpec( wx.stc.STC_STYLE_DEFAULT, 
                "size:%d,face:%s,fore:#000000" % (be_config.point_size, be_config.face) )

        self.StyleSetSpec( self.style_normal,   "fore:#000000" )
        self.StyleSetSpec( self.style_error,    "fore:#DC143C" )    # Crimson
        self.StyleSetSpec( self.style_info,     "fore:#191970" )    # Midnight Blue
        self.StyleSetSpec( self.style_warning,  "fore:#008000" )    # Green
        self.StyleSetSpec( self.style_critical, "fore:#BA55D3" )    # Medium Orchid
        self.StyleSetSpec( self.style_debug,    "fore:#DC143C" )    # Crimson

        wx.EVT_KEY_DOWN( self, self.OnKeyDown )

    def OnKeyDown( self, event ):
        """
        Don't let the STC treat the TAB normally (insert a tab
        character.)  Turn it into a navigation event instead.
        """
        if event.GetKeyCode() == wx.WXK_TAB:
            flags = wx.NavigationKeyEvent.IsForward
            if event.ShiftDown():
                flags = wx.NavigationKeyEvent.IsBackward
            if event.ControlDown():
                flags |= wx.NavigationKeyEvent.WinChange
            self.Navigate(flags)            
        else:
            event.Skip()

    def SetWindowSize( self, size ):
        wx.stc.StyledTextCtrl.SetSize( self, size )
        self.EnsureCaretVisible()

    def WriteStyledText( self, text, style ):
        # only allowed to use GUI objects on the foreground thread
        if not self.app.isMainThread():
            self.app.foregroundProcess( self.WriteStyledText, (text, style) )
            return

        self.SetReadOnly(False)
        carot_pos = self.GetCurrentPos()
        insert_pos = self.GetLength()
        self.InsertText( insert_pos, text )
        self.StartStyling( insert_pos, 0xff )
        self.SetStyling( len(text), style )
        if carot_pos == insert_pos:
            new_carot_pos = self.GetLength()
            self.SetCurrentPos( new_carot_pos )
            self.SetSelectionStart( new_carot_pos )
            self.SetSelectionEnd( new_carot_pos )
            self.EnsureCaretVisible()
        self.SetReadOnly(True)

    def WriteNormal( self, text ):
        self.WriteStyledText( text, self.style_normal )

    def WriteError( self, text ):
        self.WriteStyledText( text, self.style_error )

    def WriteInfo( self, text ):
        self.WriteStyledText( text, self.style_info )

    def WriteWarning( self, text ):
        self.WriteStyledText( text, self.style_warning )

    def WriteCritical( self, text ):
        self.WriteStyledText( text, self.style_critical )

    def WriteDebug( self, text ):
        self.WriteStyledText( text, self.style_debug )

    def ClearText( self ):
        self.SetReadOnly(False)
        self.ClearAll()
        self.SetReadOnly(True)

