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
#import be_images
#import be_emacs_panel
#import be_preferences_dialog
import be_platform_specific

import be_config

class BemacsMainWindow(QtWidgets.QMainWindow):
    def __init__( self, app ):
        self.app = app
        self.log = self.app.log

        title = T_("Barry's Emacs")

        win_prefs = self.app.prefs.getWindow()

        super().__init__( app )
        self.setTitle( title )

        self.centre_widget = QtWidgets.QTextEdit( 'The centre widget' )
        self.setCentralWidget( self.centre_widget )
