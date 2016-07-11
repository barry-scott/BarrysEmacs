'''
 ====================================================================
 Copyright (c) 2003-2015 Barry A Scott.  All rights reserved.

 This software is licensed as described in the file LICENSE.txt,
 which you should have received as part of this distribution.

 ====================================================================

    be_preferences_dialog.py

'''
import os

from PyQt5 import QtWidgets
from PyQt5 import QtGui
from PyQt5 import QtCore

import be_exceptions
import be_emacs_panel

class PreferencesDialog( QtWidgets.QDialog ):
    def __init__( self, parent, app ):
        super().__init__( parent )

        self.app = app

        self.all_tabs = []

        # useful for debugging new pages
        try:
            self.initControls()

        except RuntimeError:
            app.log.exception( T_('PreferencesDialog') )

        self.resize( 600, 400 )

    def initControls( self ):
        tabs = QtWidgets.QTabWidget()
        self.all_tabs.append( FontTab( self.app, T_('Font') ) )
        self.all_tabs.append( ColoursTab( self.app, T_('Colours') ) )

        for tab in self.all_tabs:
            tabs.addTab( tab, tab.title )

        buttons = QtWidgets.QDialogButtonBox( QtWidgets.QDialogButtonBox.Ok | QtWidgets.QDialogButtonBox.Cancel )

        buttons.accepted.connect( self.validateAndSave )
        buttons.rejected.connect( self.reject )

        layout = QtWidgets.QVBoxLayout()
        layout.addWidget( tabs )
        layout.addWidget( buttons )
        self.setLayout( layout )

    def validateAndSave( self ):
        for tab in self.all_tabs:
            if not tab.validate():
                return

        for tab in self.all_tabs:
            tab.savePreferences()

        self.accept()

class FontTab(QtWidgets.QWidget):
    def __init__( self, app, title ):
        super().__init__()

        self.title = title
        self.app = app

        self.initControls()

    def initControls( self ):
        p = self.app.getPrefs().window.font

        self.face = p.face
        self.point_size = p.point_size

        self.static_text1 = QtWidgets.QLabel( T_('Font:') )
        self.static_text2 = QtWidgets.QLabel( '%s %dpt ' % (self.face, self.point_size) )
        self.static_text2.sizePolicy().setHorizontalPolicy( QtWidgets.QSizePolicy.Fixed )
        self.static_text2.setFrameStyle( QtWidgets.QFrame.Panel|QtWidgets.QFrame.Sunken )

        self.btn_select_font = QtWidgets.QPushButton( T_(' Select Font... ') )

        self.grid_sizer = QtWidgets.QGridLayout()
        self.grid_sizer.addWidget( self.static_text1, 0, 0 )
        self.grid_sizer.addWidget( self.static_text2, 0, 1 )
        self.grid_sizer.addWidget( self.btn_select_font, 0, 2 )
        self.grid_sizer.setColumnStretch( 1, 2 )
        self.grid_sizer.setRowStretch( 1, 2 )

        self.btn_select_font.clicked.connect( self.onSelectFont )

        self.setLayout( self.grid_sizer )

    def savePreferences( self ):
        p = self.app.getPrefs().window.font

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
        font = QtGui.QFont( self.face, self.point_size )
        font, ok = QtWidgets.QFontDialog.getFont( font, self, T_('Choose font'), QtWidgets.QFontDialog.MonospacedFonts )

        if ok:
            self.face = font.family()
            self.point_size = font.pointSize()

            self.app.log.info( 'Face: %r' % (self.face,) )
            self.app.log.info( 'PointSize: %r' % (self.point_size,) )

            self.static_text2.setText( '%s %dpt ' % (self.face, self.point_size) )

class ColoursTab(QtWidgets.QWidget):
    def __init__( self, app, title ):
        super().__init__()

        self.title = title
        self.app = app

        self.initControls()

    def initControls( self ):
        p = self.app.getPrefs().window

        self.model = ColourTableModel( p.all_colours )
        self.view = ColourTableView()
        self.view.setModel( self.model )
        self.view.resizeColumnsToContents()

        self.grid_sizer = QtWidgets.QGridLayout()
        self.grid_sizer.addWidget( self.view, 0, 0 )

        self.setLayout( self.grid_sizer )

        self.view.doubleClicked.connect( self.model.tableDoubleClicked )


    def savePreferences( self ):
        p = self.app.getPrefs().window


    def validate( self ):
        return True

class ColourTableView(QtWidgets.QTableView):
    def __init__( self ):
        super().__init__()

class ColourTableModel(QtCore.QAbstractTableModel):
    col_name = 0
    col_example = 1
    col_fg = 2
    col_bg = 3

    def __init__( self, all_colours ):
        super().__init__()

        self.column_titles = [U_('Name'), U_('Example'), U_('Foreground'), U_('Background')]

        self.all_colours = all_colours

        self.all_names = [colour_info[0] for colour_info in be_emacs_panel.all_colour_defaults]

        self.__all_brushes = {}

    def tableDoubleClicked( self, index ):
        row = index.row()
        colour = self.all_colours[ self.all_names[ row ] ]

        col = index.column()
        if col == self.col_fg:
            fg = self.__pickColour( colour.fg, 'Foreground for %s' % (colour.name,) )
            if fg is not None:
                colour.fg = fg

                self.dataChanged.emit(
                    self.createIndex( row, self.col_fg ),
                    self.createIndex( row, self.col_example ) )

        if col == self.col_bg:
            bg = self.__pickColour( colour.bg, 'Background for %s' % (colour.name,) )
            if bg is not None:
                colour.bg = bg

                self.dataChanged.emit(
                    self.createIndex( row, self.col_fg ),
                    self.createIndex( row, self.col_example ) )

    def __pickColour( self, rgb, title ):
        colour = QtGui.QColor( *rgb )
        colour = QtWidgets.QColorDialog.getColor( colour, None, title )
        if colour.isValid():
            return (colour.red(), colour.green(), colour.blue())

        else:
            return None

    def __brush( self, rgb ):
        if rgb not in self.__all_brushes:
            self.__all_brushes[ rgb ] = QtGui.QBrush( QtGui.QColor( *rgb ) )

        return self.__all_brushes[ rgb ]

    def rowCount( self, parent ):
        return len( self.all_colours )

    def columnCount( self, parent ):
        return len( self.column_titles )

    def headerData( self, section, orientation, role ):
        if role == QtCore.Qt.DisplayRole:
            if orientation == QtCore.Qt.Horizontal:
                return T_( self.column_titles[section] )

            if orientation == QtCore.Qt.Vertical:
                return ''

        elif role == QtCore.Qt.TextAlignmentRole and orientation == QtCore.Qt.Horizontal:
            return QtCore.Qt.AlignLeft

        return None

    def data( self, index, role ):
        row = index.row()
        name = self.all_names[ row ]
        colour = self.all_colours[ name ]

        col = index.column()


        if role == QtCore.Qt.DisplayRole:
            if col == self.col_name:
                return T_( be_emacs_panel.all_colour_defaults[ row ][1] )

            elif col == self.col_fg:
                return '#%2.2x%2.2x%2.2x' % colour.fg

            elif col == self.col_bg:
                return '#%2.2x%2.2x%2.2x' % colour.bg

            elif col == self.col_example:
                return 'Example Text'

            assert False

        elif role == QtCore.Qt.ForegroundRole:
            if col == self.col_example:
                brush = self.__brush( colour.fg )
                return brush

        elif role == QtCore.Qt.BackgroundRole:
            if col == self.col_example:
                brush = self.__brush( colour.bg )
                return brush

        return None
