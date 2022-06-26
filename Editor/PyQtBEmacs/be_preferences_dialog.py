'''
 ====================================================================
 Copyright (c) 2003-2019 Barry A Scott.  All rights reserved.

 This software is licensed as described in the file LICENSE.txt,
 which you should have received as part of this distribution.

 ====================================================================

    be_preferences_dialog.py

'''
import os

from PyQt6 import QtWidgets
from PyQt6 import QtGui
from PyQt6 import QtCore

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
        self.all_tabs.append( CursorTab( self.app, T_('Cursor') ) )

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

class CursorTab(QtWidgets.QWidget):
    def __init__( self, app, title ):
        super().__init__()

        self.title = title
        self.app = app

        self.initControls()

    def initControls( self ):
        p = self.app.getPrefs().window.cursor_style

        self.shape_label = QtWidgets.QLabel( T_('Shape') )
        self.shape_group = QtWidgets.QButtonGroup()
        self.shape_line_radio = QtWidgets.QRadioButton( T_('Line cursor') )
        self.shape_group.addButton( self.shape_line_radio )
        self.shape_line_radio.setChecked( p.shape == 'line' )
        self.shape_block_radio = QtWidgets.QRadioButton( T_('Block cursor') )
        self.shape_group.addButton( self.shape_block_radio )
        self.shape_block_radio.setChecked( p.shape == 'block' )

        self.blink_label = QtWidgets.QLabel( T_('Style') )
        self.blink_value = QtWidgets.QCheckBox( T_('Use a blinking cursor') )
        self.blink_value.setChecked( p.blink )

        self.interval_label = QtWidgets.QLabel( T_('Blink Interval (milli-seconds)') )
        self.interval_value = QtWidgets.QLineEdit()
        self.interval_value.setText( '%s' % (p.interval,) )

        self.colour_lable = QtWidgets.QLabel( T_('Note: The cursor colour can be set on in the Colours tab') )

        self.grid_sizer = QtWidgets.QGridLayout()
        row = 0
        self.grid_sizer.addWidget( self.shape_label, row, 0, 3, 1 )
        row += 1
        self.grid_sizer.addWidget( self.shape_block_radio, row, 1 )
        row += 1
        self.grid_sizer.addWidget( self.shape_line_radio, row, 1 )
        row += 1
        self.grid_sizer.addWidget( self.blink_label, row, 0 )
        self.grid_sizer.addWidget( self.blink_value, row, 1 )
        row += 1
        self.grid_sizer.addWidget( self.interval_label, row, 0 )
        self.grid_sizer.addWidget( self.interval_value, row, 1 )
        row += 1
        self.grid_sizer.addWidget( self.colour_lable, row, 0, 1, 2 )

        self.grid_sizer.setRowStretch( row, 5 )
        self.grid_sizer.setColumnStretch( 1, 5 )

        self.setLayout( self.grid_sizer )

    def savePreferences( self ):
        p = self.app.getPrefs().window.cursor_style

        p.blink = self.blink_value.isChecked()
        p.interval = int(self.interval_value.text())

        if self.shape_line_radio.isChecked():
            p.shape = 'line'

        elif self.shape_block_radio.isChecked():
            p.shape = 'block'

    def validate( self ):
        try:
            size = int(self.interval_value.text())
            if size < 100 or size > 5000:
                raise ValueError('bad size')
            return True

        except ValueError:
            return False


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

        self.label_theme = QtWidgets.QLabel( T_('Theme') )
        self.theme = QtWidgets.QComboBox()
        self.theme.addItems( sorted(be_emacs_panel.all_themes.keys()) )
        self.theme.setCurrentText( p.theme.name )
        self.theme.currentTextChanged.connect( self.themeChanged )

        self.label_custom_colours = QtWidgets.QLabel( T_('Custom Colors') )
        self.custom_colours_model = ColourTableModel( p.cursor, p.all_colours, p.theme.name )
        self.custom_colours_view = ColourTableView()
        self.custom_colours_view.setModel( self.custom_colours_model )
        self.custom_colours_view.resizeColumnsToContents()

        self.reset_button = QtWidgets.QPushButton( T_("Reset all custom colours to the theme's colors") )
        self.reset_button.clicked.connect( self.resetColours )

        self.grid_sizer = QtWidgets.QGridLayout()
        self.grid_sizer.addWidget( self.label_theme, 0, 0 )
        self.grid_sizer.addWidget( self.theme, 0, 1 )
        self.grid_sizer.addWidget( self.label_custom_colours, 1, 0 )
        self.grid_sizer.addWidget( self.custom_colours_view, 1, 1 )
        self.grid_sizer.addWidget( self.reset_button, 2, 1 )

        self.setLayout( self.grid_sizer )

        self.custom_colours_view.doubleClicked.connect( self.custom_colours_model.tableDoubleClicked )

    def themeChanged( self, theme_name ):
        self.custom_colours_model.setTheme( theme_name )

    def resetColours( self, *args ):
        self.custom_colours_model.resetColours()

    def savePreferences( self ):
        p = self.app.getPrefs().window.theme
        p.name = self.theme.currentText()

    def validate( self ):
        return True

class ColourTableView(QtWidgets.QTableView):
    def __init__( self ):
        super().__init__()

        self.customContextMenuRequested.connect( self.tableContextMenu )
        self.setContextMenuPolicy( QtCore.Qt.CustomContextMenu )

        self.setSelectionBehavior( self.SelectItems )
        self.setSelectionMode( self.SingleSelection )

        self.current_selection = []

    def selectionChanged( self, selected, deselected ):
        # allow the table to redraw the selected row highlights
        super().selectionChanged( selected, deselected )

        model = self.model()
        self.current_selection = [index for index in self.selectedIndexes()
                                        if (index.row() > 0 and index.column() in (model.col_fg, model.col_bg))
                                        or (index.row() == 0 and index.column() in (model.col_fg,))]

    def tableContextMenu( self, pos ):
        if len(self.current_selection) == 0:
            return

        global_pos = self.viewport().mapToGlobal( pos )

        menu = QtWidgets.QMenu( self )

        if self.current_selection[0].row() == 0:
            menu.addSection( T_('Cursor') )

        else:
            menu.addSection( T_('Colour') )

        action = menu.addAction( T_('Edit…') )
        action.triggered.connect( self.tableEditColour )

        action = menu.addAction( T_('Set to default colour') )
        action.triggered.connect( self.tableSetToDefault )

        menu.exec( global_pos )

    def tableEditColour( self ):
        self.model().editColour( self.current_selection[0] )

    def tableSetToDefault( self ):
        self.model().setToDefault( self.current_selection[0] )

class ColourTableModel(QtCore.QAbstractTableModel):
    col_name = 0
    col_example = 1
    col_fg = 2
    col_bg = 3

    def __init__( self, cursor, all_colours, theme_name ):
        super().__init__()

        self.column_titles = [U_('Name'), U_('Example'), U_('Foreground'), U_('Background')]

        self.cursor = cursor
        self.all_colours = all_colours

        self.theme = None
        self.all_names = None
        self.all_presentation_names = None
        self.__all_brushes = {}

        self.setTheme( theme_name )

    def setTheme( self, theme_name ):
        self.theme = be_emacs_panel.all_themes[ theme_name ]
        self.all_names = [colour_info.name for colour_info in self.theme.all_colours]
        self.all_presentation_names = [colour_info.presentation_name for colour_info in self.theme.all_colours]

    def resetColours( self ):
        self.beginResetModel()

        self.cursor.fg.setTuple( self.theme.cursor_fg )
        for colour_info in self.theme.all_colours:
            self.all_colours[ colour_info.name ].fg.setTuple( colour_info.fg )
            self.all_colours[ colour_info.name ].bg.setTuple( colour_info.bg )

        self.endResetModel()

    def tableDoubleClicked( self, index ):
        self.editColour( index )

    def setToDefault( self, index ):
        row = index.row()
        col = index.column()

        if row == 0:
            if col == self.col_fg:
                self.cursor.fg.setTuple( self.theme.cursor_fg )

            return

        colour = self.theme.all_colours[ self.all_names[ row-1 ] ]

        if col == self.col_fg:
            fg = self.theme.all_colours[ row-1 ].fg
            if fg is not None:
                colour.fg.setTuple( fg )

                self.dataChanged.emit(
                    self.createIndex( row, self.col_example ),
                    self.createIndex( row, self.col_fg ) )

        if col == self.col_bg:
            bg = self.theme.all_colours[ row-1 ].bg
            if bg is not None:
                colour.bg.setTuple( bg )

                self.dataChanged.emit(
                    self.createIndex( row, self.col_example ),
                    self.createIndex( row, self.col_fg ) )

    def editColour( self, index ):
        row = index.row()
        col = index.column()

        if row == 0:
            if col == self.col_fg:
                if self.__pickColour( self.cursor.fg, T_('Cursor') ):
                    self.dataChanged.emit(
                        self.createIndex( row, self.col_example ),
                        self.createIndex( row, self.col_fg ) )

            return

        colour = self.all_colours[ self.all_names[ row-1 ] ]

        if col == self.col_fg:
            if self.__pickColour( colour.fg, T_('Foreground for %s') % (self.all_presentation_names[ row-1 ],) ):
                self.dataChanged.emit(
                    self.createIndex( row, self.col_example ),
                    self.createIndex( row, self.col_fg ) )

        if col == self.col_bg:
            if self.__pickColour( colour.bg, T_('Background for %s') % (self.all_presentation_names[ row-1 ],) ):
                self.dataChanged.emit(
                    self.createIndex( row, self.col_example ),
                    self.createIndex( row, self.col_fg ) )

    def __pickColour( self, rgb, title ):
        value = rgb.getTuple()
        colour = QtGui.QColor( *value )
        if len(value) == 3:
            colour = QtWidgets.QColorDialog.getColor( colour, None, title )
            if colour.isValid():
                rgb.setTuple( (colour.red(), colour.green(), colour.blue()) )
                return True

        else:
            colour = QtWidgets.QColorDialog.getColor( colour, None, title, QtWidgets.QColorDialog.ShowAlphaChannel )
            if colour.isValid():
                rgb.setTuple( (colour.red(), colour.green(), colour.blue(), colour.alpha()) )
                return True

        return False

    def __brush( self, rgb ):
        if rgb not in self.__all_brushes:
            self.__all_brushes[ rgb ] = QtGui.QBrush( QtGui.QColor( *rgb ) )

        return self.__all_brushes[ rgb ]

    def rowCount( self, parent ):
        return len( self.all_colours ) + 1

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
        col = index.column()
        row = index.row()
        if row == 0:
            # its the cursor
            if role == QtCore.Qt.DisplayRole:
                if col == self.col_name:
                    return T_('Cursor')

                elif col == self.col_fg:
                    return '#%2.2x%2.2x%2.2x%2.2x' % self.cursor.fg.getTuple()

                elif col == self.col_bg:
                    return ''

                elif col == self.col_example:
                    return ' X '

                assert False

            elif role == QtCore.Qt.ForegroundRole:
                if col == self.col_example:
                    brush = self.__brush( self.cursor.fg.getTuple() )
                    return brush

            elif role == QtCore.Qt.BackgroundRole:
                if col == self.col_example:
                    if self.theme.name == 'Dark':
                        brush = self.__brush( (  0,  0,  0) )
                    else:
                        brush = self.__brush( (255,255,255) )
                    return brush

        else:
            # skip first row
            row -= 1
            name = self.all_names[ row ]
            colour = self.all_colours[ name ]

            if role == QtCore.Qt.DisplayRole:
                if col == self.col_name:
                    return T_( self.all_presentation_names[ row ] )

                elif col == self.col_fg:
                    return '#%2.2x%2.2x%2.2x' % colour.fg.getTuple()

                elif col == self.col_bg:
                    return '#%2.2x%2.2x%2.2x' % colour.bg.getTuple()

                elif col == self.col_example:
                    return 'Example Text'

                assert False

            elif role == QtCore.Qt.ForegroundRole:
                if col == self.col_example:
                    brush = self.__brush( colour.fg.getTuple() )
                    return brush

            elif role == QtCore.Qt.BackgroundRole:
                if col == self.col_example:
                    brush = self.__brush( colour.bg.getTuple() )
                    return brush

        return None
