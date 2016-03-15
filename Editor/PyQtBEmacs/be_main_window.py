'''
 ====================================================================
 Copyright (c) 2003-2015 Barry A Scott.  All rights reserved.

 This software is licensed as described in the file LICENSE.txt,
 which you should have received as part of this distribution.

 ====================================================================

    be_main_window.py

    Based on code from pysvn WorkBench

'''
import sys
import os
import time
import logging

# On OS X the packager missing this import
import sip

from PyQt5 import QtWidgets
from PyQt5 import QtGui
from PyQt5 import QtCore

#import be_ids
import be_exceptions
import be_version
import be_images
import be_emacs_panel
import be_preferences_dialog
import be_platform_specific

import be_config

class BemacsAction:
    def __init__( self, app, code ):
        self.app = app
        self.code = code

    def sendCode( self ):
        for ch in be_emacs_panel.prefix_menu + self.code:
            self.app.editor.guiEventChar( ch, False )

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

        self.emacs_panel = be_emacs_panel.EmacsPanel( self.app, self )
        self.setCentralWidget( self.emacs_panel )

        self.__setupMenuBar()
        self.__setupToolBar()
        self.__setupStatusBar( self.emacs_panel.font )

        print( '-'*60 )
        geometry = win_prefs.getFrameGeometry()
        if geometry is not None:
            geometry = QtCore.QByteArray( geometry.encode('utf-8') )
            self.restoreGeometry( QtCore.QByteArray.fromHex( geometry ) )

        else:
            self.resize( 700, 500 )

        self.setAcceptDrops( True )

        self.move_event_count = 0
        self.main_window_start_time = time.time()

    def closeEvent( self, event ):
        if self.app.may_quit:
            self.log.info( 'closeEvent()' )

            win_prefs = self.app.prefs.getWindow()
            win_prefs.setFrameGeometry( self.saveGeometry().toHex().data() )
            self.app.writePreferences()
            event.accept()

        else:
            # set the editor decide
            self.app.onCloseEditor()
            event.ignore()

    def __setupMenuBar( self ):
        mb = self.menuBar()

        menu_file = mb.addMenu( T_('&File') )
        self.addEmacsMenu( menu_file, 'fo', T_('&Open') )

        act_prefs = menu_file.addAction( T_('&Preferences') )
        act_prefs.triggered.connect( self.onActPreferences )

        act_exit = menu_file.addAction( T_('E&xit') )
        act_exit.triggered.connect( self.app.onCloseEditor )

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
        act.triggered.connect( self.onActDocumentation )
        act = menu_help.addAction( T_("&About...") )
        act.triggered.connect( self.onActAbout )

    def addEmacsMenu( self, container, code, title, icon=None ):
        if code not in self.__all_actions:
            self.__all_actions[ code ] = BemacsAction( self.app, code )

        if icon is None:
            action = container.addAction( title )
        else:
            action = container.addAction( icon, title )

        self.__all_actions[ code ].connect( action )

    def __setupToolBar( self ):
        # Add tool bar
        t = self.addToolBar( 'main' )

        self.addEmacsToolbar( t, 'ex', T_('Cut'), 'toolbar_images/editcut.png' )
        self.addEmacsToolbar( t, 'ec', T_('Copy'), 'toolbar_images/editcopy.png' )
        self.addEmacsToolbar( t, 'ev', T_('Paste'), 'toolbar_images/editpaste.png' )

        self.addEmacsToolbar( t, 'wo', T_('wo'), 'toolbar_images/window_del_other.png' )
        self.addEmacsToolbar( t, 'wt', T_('wt'), 'toolbar_images/window_del_this.png' )
        self.addEmacsToolbar( t, 'wh', T_('wh'), 'toolbar_images/window_split_horiz.png' )
        self.addEmacsToolbar( t, 'wv', T_('wv'), 'toolbar_images/window_split_vert.png' )

        t.addSeparator()
        self.addEmacsToolbar( t, 'cu', T_('UPPER'), 'toolbar_images/case_upper.png' )
        self.addEmacsToolbar( t, 'cl', T_('lower'), 'toolbar_images/case_lower.png' )
        self.addEmacsToolbar( t, 'cc', T_('Capitalise'), 'toolbar_images/case_capitalise.png' )
        self.addEmacsToolbar( t, 'ci', T_('inVERT'), 'toolbar_images/case_invert.png' )

        t.addSeparator()
        self.addEmacsToolbar( t, 'mr', T_('Record'), 'toolbar_images/macro_record.png' )
        self.addEmacsToolbar( t, 'ms', T_('Stop'), 'toolbar_images/macro_stop.png' )
        self.addEmacsToolbar( t, 'mp', T_('Play'), 'toolbar_images/macro_play.png' )

        t.addSeparator()
        self.addEmacsToolbar( t, 'vw', T_('White Space'), 'toolbar_images/view_white_space.png' )
        self.addEmacsToolbar( t, 'vl', T_('Wrap Long'), 'toolbar_images/view_wrap_long.png' )

        t.addSeparator()
        self.addEmacsToolbar( t, 'tg', T_('Grep Files'), 'toolbar_images/tools_grep.png' )

    def addEmacsToolbar( self, container, code, title, icon_filename ):
        icon = be_images.getIcon( icon_filename )

        if code not in self.__all_actions:
            self.__all_actions[ code ] = BemacsAction( self.app, code )

        action = container.addAction( icon, title )

        self.__all_actions[ code ].connect( action )

    def __setupStatusBar( self, font ):
        s = self.statusBar()

        self.status_message = QtWidgets.QLabel()
        self.status_read_only = QtWidgets.QLabel()
        self.status_insert_mode = QtWidgets.QLabel()
        self.status_eol = QtWidgets.QLabel()
        self.status_line_num = QtWidgets.QLabel()
        self.status_col_num = QtWidgets.QLabel()

        self.status_read_only.setFrameStyle( QtWidgets.QFrame.Panel|QtWidgets.QFrame.Sunken )
        self.status_insert_mode.setFrameStyle( QtWidgets.QFrame.Panel|QtWidgets.QFrame.Sunken )
        self.status_eol.setFrameStyle( QtWidgets.QFrame.Panel|QtWidgets.QFrame.Sunken )
        self.status_line_num.setFrameStyle( QtWidgets.QFrame.Panel|QtWidgets.QFrame.Sunken )
        self.status_col_num.setFrameStyle( QtWidgets.QFrame.Panel|QtWidgets.QFrame.Sunken )

        self.__setupStatusBarFont( font )

        self.status_line_num.setAlignment( QtCore.Qt.AlignRight )
        self.status_col_num.setAlignment( QtCore.Qt.AlignRight )

        self.status_read_only.setText( '' )
        self.status_insert_mode.setText( '' )
        self.status_eol.setText( '' )
        self.status_line_num.setText( '' )
        self.status_col_num.setText( '' )

        s.addWidget( self.status_message )
        s.addPermanentWidget( self.status_read_only )
        s.addPermanentWidget( self.status_insert_mode )
        s.addPermanentWidget( self.status_eol )
        s.addPermanentWidget( self.status_line_num )
        s.addPermanentWidget( self.status_col_num )

    def __setupStatusBarFont( self, font ):
        self.status_message.setFont( font )
        self.status_read_only.setFont( font )
        self.status_insert_mode.setFont( font )
        self.status_eol.setFont( font )
        self.status_line_num.setFont( font )
        self.status_col_num.setFont( font )

        metrics = self.status_col_num.fontMetrics()
        # QLabel min width is not used for the text. Its the text + somthing-magic
        # try width of a char + 25%
        fudge = metrics.width( 'M' ) * 125 // 100
        self.status_read_only.setMinimumWidth( fudge + max( [metrics.width( s ) for s in ('RO',)] ) )
        self.status_insert_mode.setMinimumWidth( fudge + max( [metrics.width( s ) for s in ('Ins', 'Over')] ) )
        self.status_eol.setMinimumWidth( fudge + max( [metrics.width( s ) for s in ('LF', 'CR', 'CRLF')] ) )
        self.status_line_num.setMinimumWidth( fudge + metrics.width( '9999999' ) )
        self.status_col_num.setMinimumWidth( fudge + metrics.width( '9999' ) )

    def newPreferences( self ):
        self.__setupStatusBarFont( self.emacs_panel.font )

    def onActPreferences( self ):
        pref_dialog = be_preferences_dialog.PreferencesDialog( self, self.app )
        rc = pref_dialog.exec_()
        if rc == QtWidgets.QDialog.Accepted:
            self.app.writePreferences()
            self.emacs_panel.newPreferences()
            self.newPreferences()

    def onActDocumentation( self ):
        user_guide = be_platform_specific.getDocUserGuide()
        if not os.path.exists( user_guide ):
            self.log.error( 'Expected user guide %r to exist' % (user_guide,) )
            return

        #qqq# URL open( 'file://%s' % (user_guide,) )

    def onActAbout( self ):
        from PyQt5 import Qt
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
        all_about_info.append( 'PyQt %s, Qt %s' % (Qt.PYQT_VERSION_STR, QtCore.QT_VERSION_STR) )
        all_about_info.append( T_('Copyright Barry Scott (c) 1980-%s. All rights reserved') % (be_version.year,) )

        QtWidgets.QMessageBox.information( self, T_("About Barry's Emacs"), '\n'.join( all_about_info ) )

    def setStatus( self, all_status ):
        self.status_read_only   .setText( {True: 'RO', False: ''}[all_status['readonly']] )
        self.status_insert_mode .setText( {True: 'Over', False: 'Ins '}[all_status['overstrike']] )
        self.status_eol         .setText( all_status['eol'].upper() )
        self.status_line_num    .setText( '%d' % (all_status['line'],) )
        self.status_col_num     .setText( '%d' % (all_status['column'],) )

    def dragEnterEvent( self, event ):
        if not event.mimeData().hasUrls():
            return

        for url in event.mimeData().urls():
            if url.scheme() != 'file':
                return

        event.acceptProposedAction()

    def dropEvent( self, event ):
        all_paths = [url.toLocalFile() for url in event.mimeData().urls()]

        if len(all_paths) == 1 and os.path.isdir( all_paths[0] ):
            self.app.guiClientCommandHandler( [os.getcwd(), 'cd-here'] + all_paths )

        else:
            all_paths = [path for path in all_paths if not os.path.isdir( path )]
            self.app.guiClientCommandHandler( [os.getcwd(), 'emacs'] + all_paths )

        event.acceptProposedAction()
