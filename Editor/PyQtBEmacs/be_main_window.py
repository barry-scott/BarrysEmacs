'''
 ====================================================================
 Copyright (c) 2003-2010 Barry A Scott.  All rights reserved.

 This software is licensed as described in the file LICENSE.txt,
 which you should have received as part of this distribution.

 ====================================================================

    be_main_window.py

    Based on code from pysvn WorkBench

'''
import sys
import os
import logging

from PyQt5 import QtWidgets
from PyQt5 import QtGui
from PyQt5 import QtCore

#import be_ids
import be_exceptions
import be_version
import be_images
#import be_emacs_panel
#import be_preferences_dialog
import be_platform_specific

import be_config

class BemacsAction:
    def __init__( self, code ):
        self.code = code

    def sendCode( self ):
        print( 'BemacsAction code=%r' % (self.code,) )

    def connect( self, action ):
        action.triggered.connect( self.sendCode )



class BemacsMainWindow(QtWidgets.QMainWindow):
    def __init__( self, app ):
        self.app = app
        self.log = self.app.log

        self.__all_actions = {}

        title = T_("Barry's Emacs")

        win_prefs = self.app.prefs.getWindow()

        super().__init__()
        self.setWindowTitle( title )
        self.setWindowIcon( be_images.getIcon( 'bemacs.png' ) )

        self.centre_widget = QtWidgets.QTextEdit( 'The centre widget' )
        self.setCentralWidget( self.centre_widget )

        mb = self.menuBar()

        menu_file = mb.addMenu( T_('&File') )
        self.addEmacsMenu( menu_file, 'fo', T_('&Open') )

        act_prefs = menu_file.addAction( T_('&Preferences') )
        act_prefs.triggered.connect( self.OnActPreferences )

        act_exit = menu_file.addAction( T_('E&xit') )
        act_exit.triggered.connect( self.close )

        menu_edit = mb.addMenu( T_('&Edit') )

        self.addEmacsMenu( menu_edit, 'ec', T_('Copy') )
        self.addEmacsMenu( menu_edit, 'ex', T_('Cut') )
        self.addEmacsMenu( menu_edit, 'ep', T_('Paste') )
        menu_edit.addSeparator()

        self.addEmacsMenu( menu_edit, 'ea', T_('Select All') )

        menu_edit.addSeparator()
        self.addEmacsMenu( menu_edit, 'eg', T_('Goto Line...') )

        menu_edit.addSeparator()
        menu_edit_advanced = menu_edit.addMenu( T_('Advanced...') )

        self.addEmacsMenu( menu_edit_advanced, 'cu', T_('Case Upper') )
        self.addEmacsMenu( menu_edit_advanced, 'cl', T_('Case Lower') )
        self.addEmacsMenu( menu_edit_advanced, 'cc', T_('Case Capitalise') )
        self.addEmacsMenu( menu_edit_advanced, 'ci', T_('Case Invert') )
        menu_edit_advanced.addSeparator()
        self.addEmacsMenu( menu_edit_advanced, 'ri', T_('Indent Region') )
        self.addEmacsMenu( menu_edit_advanced, 'rI', T_('Undent Region') )
        menu_edit_advanced.addSeparator()
        self.addEmacsMenu( menu_edit_advanced, 'rn', T_('Narrow Region') )
        self.addEmacsMenu( menu_edit_advanced, 'rw', T_('Widen Region') )

        menu_view = mb.addMenu( T_('&View') )
        self.addEmacsMenu( menu_view, 'vw', T_('View white space') )
        self.addEmacsMenu( menu_view, 'vl', T_('Wrap long lines') )

        menu_macro = mb.addMenu( T_('&Macro') )
        self.addEmacsMenu( menu_macro, 'mr', T_('Record') )
        self.addEmacsMenu( menu_macro, 'ms', T_('Stop Recording') )
        self.addEmacsMenu( menu_macro, 'mp', T_('Run') )

        menu_build = mb.addMenu( T_('Build') )
        self.addEmacsMenu( menu_build, 'bc', T_('Compile') )
        self.addEmacsMenu( menu_build, 'bn', T_('Next Error') )
        self.addEmacsMenu( menu_build, 'bp', T_('Previous Error') )

        menu_tool = mb.addMenu( T_('&Tool') )
        self.addEmacsMenu( menu_tool, 'tg', T_('Grep in files...') )
        self.addEmacsMenu( menu_tool, 'tb', T_('Grep in buffers...') )
        self.addEmacsMenu( menu_tool, 'tc', T_('Grep current buffer...') )
        self.addEmacsMenu( menu_tool, 'rf', T_('Filter region...') )
        self.addEmacsMenu( menu_tool, 'rs', T_('Sort region') )

        menu_buffer = mb.addMenu( T_('&Buffer') )
        self.addEmacsMenu( menu_buffer, 'bs', T_('Switch to buffer...') )
        self.addEmacsMenu( menu_buffer, 'bl', T_('List buffers') )

        menu_window = mb.addMenu( T_('&Window') )
        self.addEmacsMenu( menu_window, 'wh', T_('Split Horizontal') )
        self.addEmacsMenu( menu_window, 'wv', T_('Delete Vertical') )
        self.addEmacsMenu( menu_window, 'wo', T_('Delete Other') )
        self.addEmacsMenu( menu_window, 'wt', T_('Delete This') )

        menu_help = mb.addMenu( T_('&Help' ) )
        act = menu_help.addAction( T_('Documentation...') )
        act.triggered.connect( self.OnActDocumentation )
        act = menu_help.addAction( T_("&About...") )
        act.triggered.connect( self.OnActAbout )

    def addEmacsMenu( self, menu, code, title, icon=None ):
        if code not in self.__all_actions:
            self.__all_actions[ code ] = BemacsAction( code )

        if icon is None:
            action = menu.addAction( title )
        else:
            action = menu.addAction( icon, title )

        self.__all_actions[ code ].connect( action )

    def OnActPreferences( self ):
        return

        pref_dialog = be_preferences_dialog.PreferencesDialog( self, self.app )
        rc = pref_dialog.ShowModal()
        if rc == wx.ID_OK:
            self.app.savePreferences()
            self.emacs_panel.newPreferences()

    def OnActDocumentation( self ):
        user_guide = be_platform_specific.getDocUserGuide()
        if not os.path.exists( user_guide ):
            self.log.error( 'Expected user guide %r to exist' % (user_guide,) )
            return

        #qqq# URL open( 'file://%s' % (user_guide,) )

    def OnActAbout( self ):
        all_about_info = []
        all_about_info.append( T_("Barry's Emacs %d.%d.%d-%d") %
                                (be_version.major, be_version.minor
                                ,be_version.patch, be_version.build) )
        all_about_info.append( 'Python %d.%d.%d %s %d' %
                                (sys.version_info.major
                                ,sys.version_info.minor
                                ,sys.version_info.micro
                                ,sys.version_info.releaselevel
                                ,sys.version_info.serial) )
        all_about_info.append( T_('Copyright Barry Scott (c) 1980-%s. All rights reserved') % (be_version.year,) )

        QtWidgets.QMessageBox.information( self, T_('About'), '\n'.join( all_about_info ) )
