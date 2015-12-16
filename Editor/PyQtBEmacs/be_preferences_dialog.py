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

    def initControls( self ):
        tabs = QtWidgets.QTabWidget()
        self.all_tabs.append( FontTab( self.app, T_('Font') ) )

        for tab in self.all_tabs:
            tabs.addTab( tab, tab.title )

        buttons = QtWidgets.QDialogButtonBox( QtWidgets.QDialogButtonBox.Ok | QtWidgets.QDialogButtonBox.Cancel )

        buttons.accepted.connect( self.OnOk )
        buttons.rejected.connect( self.reject )

        layout = QtWidgets.QVBoxLayout()
        layout.addWidget( tabs )
        layout.addWidget( buttons )
        self.setLayout( layout )

    def OnOk( self ):
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
        p = self.app.prefs.getFont()

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

        self.btn_select_font.clicked.connect( self.onSelectFont )

        self.setLayout( self.grid_sizer )

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
        font = QtGui.QFont( self.face, self.point_size )
        font, ok = QtWidgets.QFontDialog.getFont( font, self, T_('Choose font'), QtWidgets.QFontDialog.MonospacedFonts )

        if ok:
            self.face = font.family()
            self.point_size = font.pointSize()

            self.app.log.info( 'Face: %r' % (self.face,) )
            self.app.log.info( 'PointSize: %r' % (self.point_size,) )

            self.static_text2.setText( '%s %dpt ' % (self.face, self.point_size) )
