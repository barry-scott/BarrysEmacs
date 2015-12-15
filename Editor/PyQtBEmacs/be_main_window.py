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
        act_exit = menu_file.addAction( T_('E&xit') )
        act_exit.triggered.connect( self.close )

        menu_edit = mb.addMenu( T_('&Edit') )

        self.addBemacsMenu( menu_edit, 'Copy', 'ec' )
        self.addBemacsMenu( menu_edit, 'Cut', 'ex' )
        self.addBemacsMenu( menu_edit, 'Paste', 'ep' )

        menu_help = mb.addMenu( T_('&Help' ) )

    def addBemacsMenu( self, menu, title, code ):
        if code not in self.__all_actions:
            self.__all_actions[ code ] = BemacsAction( code )

        self.__all_actions[ code ].connect( menu.addAction( title ) )
