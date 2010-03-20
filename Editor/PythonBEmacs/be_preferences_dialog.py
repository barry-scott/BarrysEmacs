'''
 ====================================================================
 Copyright (c) 2003-2010 Barry A Scott.  All rights reserved.

 This software is licensed as described in the file LICENSE.txt,
 which you should have received as part of this distribution.

 ====================================================================

    be_preferences_dialog.py

'''
import wx
import be_exceptions
import os

class PreferencesDialog( wx.Dialog ):
    def __init__( self, parent, app ):
        wx.Dialog.__init__( self, parent, -1, T_('Preferences'), size=(400,200) )
        self.app = app
        self.v_sizer = None

        # useful for debugging new pages
        try:
            self.initControls()
        except:
            app.log.exception( T_('PreferencesDialog') )

        self.SetSizer( self.v_sizer )
        self.Layout()
        self.Fit()

        self.CentreOnParent()

    def initControls( self ):
        self.v_sizer = wx.BoxSizer( wx.VERTICAL )

        self.notebook = wx.Notebook( self )

        self.v_sizer.Add( self.notebook, 0, wx.EXPAND|wx.ALL, 5 )

        self.pages = []
        self.pages.append( FontPage( self.notebook, self.app ) )
        #self.pages.append( ToolbarPage( self.notebook, self.app ) )


        self.button_ok = wx.Button( self, wx.ID_OK, T_(' OK ') )
        self.button_ok.SetDefault()
        self.button_cancel = wx.Button( self, wx.ID_CANCEL, T_(' Cancel ') )

        self.h_sizer = wx.BoxSizer( wx.HORIZONTAL )
        self.h_sizer.Add( (1, 1), 1, wx.EXPAND )
        self.h_sizer.Add( self.button_ok, 0, wx.EXPAND|wx.EAST, 15 )
        self.h_sizer.Add( self.button_cancel, 0, wx.EXPAND|wx.EAST, 2 )

        self.v_sizer.Add( self.h_sizer, 0, wx.EXPAND|wx.ALL, 5 )

        wx.EVT_BUTTON( self, wx.ID_OK, self.OnOk )
        wx.EVT_BUTTON( self, wx.ID_CANCEL, self.OnCancel )

    def OnOk( self, event ):
        for page in self.pages:
            if not page.validate():
                return

        for page in self.pages:
            page.savePreferences()

        self.EndModal( wx.ID_OK )

    def OnCancel( self, event ):
        self.EndModal( wx.ID_CANCEL )
    

class PagePanel(wx.Panel):
    def __init__( self, notebook, title ):
        wx.Panel.__init__( self, notebook, -1, style = wx.NO_BORDER )

        self.page_v_sizer = wx.BoxSizer( wx.VERTICAL )
        self.page_v_sizer.Add( self.initControls(), 0, wx.EXPAND|wx.ALL, 5 )
        self.SetSizer( self.page_v_sizer )
        self.SetAutoLayout( True )
        self.page_v_sizer.Fit( self )
        self.Layout()

        notebook.AddPage( self, title )

    def initControls( self ):
        raise be_exceptions.InternalError('must override initControls')

class FontPage(PagePanel):
    def __init__( self, notebook, app ):
        self.app = app
        PagePanel.__init__( self, notebook, T_('Font') )

    def initControls( self ):
        p = self.app.prefs.getFont()

        self.face = p.face
        self.point_size = p.point_size

        self.static_text1 = wx.StaticText( self, -1, T_('Font: '), style=wx.ALIGN_RIGHT)
        self.static_text2 = wx.StaticText( self, -1, '%s %d' % (self.face, self.point_size), style=wx.ALIGN_RIGHT)

        self.btn_select_font = wx.Button( self, -1, T_(' Select Font... '))

        self.grid_sizer = wx.FlexGridSizer( 0, 3, 0, 0 )
        self.grid_sizer.AddGrowableCol( 1 )

        self.grid_sizer.Add( self.static_text1, 1, wx.EXPAND|wx.ALL, 3 )
        self.grid_sizer.Add( self.static_text2, 0, wx.EXPAND )
        self.grid_sizer.Add( self.btn_select_font, 1, wx.EXPAND|wx.ALL, 3 )

        wx.EVT_BUTTON( self, self.btn_select_font.GetId(), self.onSelectFont )

        return self.grid_sizer

    def savePreferences( self ):
        p = self.app.prefs.getFont()
        p.face = self.face
        p.point_size = self.point_size

    def validate( self ):
        valid = True

        if not valid:
            wx.MessageBox(
                T_('You must enter a valid something'),
                T_('Warning'),
                wx.OK | wx.ICON_EXCLAMATION,
                self )
            return False

        return True

    def onSelectFont( self, *args ):
        data = wx.FontData()
        font = wx.Font( self.point_size, wx.FONTFAMILY_DEFAULT, wx.FONTSTYLE_NORMAL, wx.FONTWEIGHT_NORMAL, False, self.face )

        data.SetInitialFont( font )

        dlg = wx.FontDialog( self, data )
        rc = dlg.ShowModal()
        if rc == wx.ID_OK:
            data = dlg.GetFontData()
            font = data.GetChosenFont()
            self.face = font.GetFaceName()
            self.point_size = font.GetPointSize()

            self.app.log.info( 'Face: %r' % (self.face,) )
            self.app.log.info( 'PointSize: %r' % (self.point_size,) )

            label = '%s %d' % (self.face, self.point_size)
            self.app.log.info( 'Label: %s' % (label,) )
            self.static_text2.SetLabel( label )

class ToolbarPage(PagePanel):
    id_exclude = wx.NewId()
    id_include = wx.NewId()
    id_move_up = wx.NewId()
    id_move_down = wx.NewId()
    id_excluded_list = wx.NewId()
    id_included_list = wx.NewId()

    def __init__( self, notebook, app ):
        self.app = app
        self.selected_name = None
        PagePanel.__init__( self, notebook, T_('Toolbar') )

    def initControls( self ):
        p = self.app.prefs.getToolbar()
        self.all_groups = be_toolbars.toolbar_main.getAllGroupNames()
        self.group_order = p.group_order

        self.excluded_list = wx.ListCtrl( self, ToolbarPage.id_excluded_list, wx.DefaultPosition,
                wx.Size( 200, 100 ), wx.LC_REPORT )
        self.excluded_list.InsertColumn( 0, T_('Toolbar Group') )
        self.excluded_list.SetColumnWidth( 0, 100 )

        self.included_list = wx.ListCtrl( self, ToolbarPage.id_included_list, wx.DefaultPosition,
                wx.Size( 200, 100 ), wx.LC_REPORT )
        self.included_list.InsertColumn( 0, T_('Toolbar Group') )
        self.included_list.SetColumnWidth( 0, 100 )

        for name in self.group_order:
            index = self.included_list.GetItemCount()

            self.included_list.InsertStringItem( index, name )
    
        for name in [name for name in self.all_groups if name not in self.group_order]:
            index = self.excluded_list.GetItemCount()

            self.excluded_list.InsertStringItem( index, name )

        self.button_include = wx.Button( self, ToolbarPage.id_include, T_(' Include --> ') )
        self.button_include.Enable( False )
        self.button_exclude = wx.Button( self, ToolbarPage.id_exclude, T_(' <-- Exclude ') )
        self.button_exclude.Enable( False )

        self.button_up = wx.Button( self, ToolbarPage.id_move_up, T_(' Move Up ') )
        self.button_up.Enable( False )
        self.button_down = wx.Button( self, ToolbarPage.id_move_down, T_(' Move Down ') )
        self.button_down.Enable( False )

        # qqq
        self.enable_label = wx.StaticText( self, -1, T_('Display toolbar: '), style=wx.ALIGN_RIGHT )
        self.enable_ctrl = wx.CheckBox( self, -1, T_('Enabled') )
        self.enable_ctrl.SetValue( p.toolbar_enable )
        self.horizontal_label = wx.StaticText( self, -1, T_('Orientation: '), style=wx.ALIGN_RIGHT )
        self.horizontal_ctrl = wx.Choice( self, -1, choices=[T_('Horizontal'), T_('Vertical')] )
        if p.horizontal_orientation:
            self.horizontal_ctrl.SetSelection( 0 )
        else:
            self.horizontal_ctrl.SetSelection( 1 )

        sizes = [T_('Small'), T_('Large'), T_('Huge')]
        pixtoidx = {64: 2, 32: 1, 16: 0}
        self.size_label = wx.StaticText( self, -1, T_('Icon size: '), style=wx.ALIGN_RIGHT )
        self.ctrl_image_size = wx.Choice( self, -1, choices=sizes )
        self.ctrl_image_size.SetSelection( pixtoidx.get( p.bitmap_size, 1 ) )

        # build the sizers
        self.v_sizer = wx.BoxSizer( wx.VERTICAL )
        self.v_sizer.Add( self.button_include, 0, wx.EXPAND|wx.EAST, 5 )
        self.v_sizer.Add( self.button_exclude, 0, wx.EXPAND|wx.EAST, 5 )
        self.v_sizer.Add( self.button_up, 0, wx.EXPAND|wx.EAST, 5 )
        self.v_sizer.Add( self.button_down, 0, wx.EXPAND|wx.EAST, 5 )

        self.h_sizer = wx.BoxSizer( wx.HORIZONTAL )
        self.h_sizer.Add( self.excluded_list, 0, wx.EXPAND|wx.WEST, 5 )
        self.h_sizer.Add( self.v_sizer, 0, wx.EXPAND|wx.EAST, 5 )
        self.h_sizer.Add( self.included_list, 0, wx.EXPAND|wx.EAST, 5 )

        self.grid_sizer = wx.FlexGridSizer( 0, 2, 0, 0 )
        self.grid_sizer.AddGrowableCol( 1 )

        self.grid_sizer.Add( self.enable_label, 1, wx.EXPAND|wx.ALL, 3 )
        self.grid_sizer.Add( self.enable_ctrl, 0, wx.EXPAND|wx.ALL, 5 )

        self.grid_sizer.Add( self.horizontal_label, 1, wx.EXPAND|wx.ALL, 3 )
        self.grid_sizer.Add( self.horizontal_ctrl, 0, wx.EXPAND|wx.ALL, 5 )

        self.grid_sizer.Add( self.size_label, 1, wx.EXPAND|wx.ALL, 3 )
        self.grid_sizer.Add( self.ctrl_image_size, 0, wx.EXPAND|wx.ALL, 5 )

        self.v_sizer2 = wx.BoxSizer( wx.VERTICAL )
        self.v_sizer2.Add( self.grid_sizer )
        self.v_sizer2.Add( self.h_sizer )

        wx.EVT_BUTTON( self, ToolbarPage.id_include, self.OnInclude )
        wx.EVT_BUTTON( self, ToolbarPage.id_exclude, self.OnExclude )
        wx.EVT_BUTTON( self, ToolbarPage.id_move_up, self.OnMoveUp )
        wx.EVT_BUTTON( self, ToolbarPage.id_move_down, self.OnMoveDown )

        wx.EVT_LIST_ITEM_SELECTED( self, ToolbarPage.id_excluded_list, self.OnExcludeSelected )
        wx.EVT_LIST_ITEM_DESELECTED( self, ToolbarPage.id_excluded_list, self.OnExcludeDeselected )

        wx.EVT_LIST_ITEM_SELECTED( self, ToolbarPage.id_included_list, self.OnIncludeSelected )
        wx.EVT_LIST_ITEM_DESELECTED( self, ToolbarPage.id_included_list, self.OnIncludeDeselected )

        return self.v_sizer2

    def OnInclude( self, event ):
        self.changeInclusionGroup( self.excluded_list, self.included_list )

    def OnExclude( self, event ):
        self.changeInclusionGroup( self.included_list, self.excluded_list )

    def changeInclusionGroup( self, from_list, to_list ):
        # remove from from_list
        from_index = from_list.FindItem( -1, self.selected_name )
        
        from_list.DeleteItem( from_index )

        # add to end of to_list
        index = to_list.GetItemCount()

        to_list.InsertStringItem( index, self.selected_name )

    def OnMoveUp( self, event ):
        self.moveColumn( -1 )

    def OnMoveDown( self, event ):
        self.moveColumn( 1 )

    def moveColumn( self, direction ):
        index = self.included_list.FindItem( -1, self.selected_name )

        self.included_list.DeleteItem( index )
        index += direction
        self.included_list.InsertStringItem( index, self.selected_name )
        self.included_list.SetItemState( index, wx.LIST_STATE_SELECTED, wx.LIST_STATE_SELECTED )

        # enable up and down if not at the ends
        item_count = self.included_list.GetItemCount()
        self.button_up.Enable( item_count > 1 and index != 0 )
        self.button_down.Enable( item_count > 1 and index != (item_count-1) )

    def OnExcludeSelected( self, event ):
        self.button_up.Enable( False )
        self.button_down.Enable( False )
        self.button_include.Enable( True )

        self.selected_name = self.excluded_list.GetItemText( event.m_itemIndex )

    def OnExcludeDeselected( self, event ):
        self.button_include.Enable( False )
        self.width_text_ctrl.Enable( False )

    def OnIncludeSelected( self, event ):
        self.button_exclude.Enable( True )
        self.button_include.Enable( False )

        self.selected_name = self.included_list.GetItemText( event.m_itemIndex )

        # enable up and down if no at the ends
        item_count = self.included_list.GetItemCount()
        self.button_up.Enable( item_count > 1 and event.m_itemIndex != 0 )
        self.button_down.Enable( item_count > 1 and event.m_itemIndex != (item_count-1) )

    def OnIncludeDeselected( self, event ):
        self.button_exclude.Enable( False )

    def savePreferences( self ):
        p = self.app.prefs.getToolbar()

        p.toolbar_enable = self.enable_ctrl.GetValue()

        p = self.app.prefs.getToolbar()
        group_order = []
        for index in range( self.included_list.GetItemCount() ):
            name = self.included_list.GetItemText( index )
            group_order.append( name )

        p.group_order = group_order

        p.horizontal_orientation = self.horizontal_ctrl.GetSelection() == 0

        size_to_pixels = {0:16, 1:32, 2:64}
        p.bitmap_size = size_to_pixels[ self.ctrl_image_size.GetSelection() ]

    def validate( self ):
        if self.included_list.GetItemCount() == 0:
            wx.MessageBox( T_('You must include at least one Toolbar group'),
                T_('Warning'),
                wx.OK | wx.ICON_EXCLAMATION,
                self )
            return False

        return True
