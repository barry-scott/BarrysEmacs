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

from PyQt5 import Qt
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

ellipsis = '…'

class BemacsAction:
    def __init__( self, app, code, check_state ):
        self.app = app
        self.code = code
        self.check_state = check_state
        self.action = None

    def __repr__( self ):
        return '<BemacsAction: code=%r check_state=%r>' % (self.code, self.check_state)

    def sendCode( self ):
        for ch in be_emacs_panel.prefix_menu + self.code:
            self.app.editor.guiEventChar( ch, False )

    def connect( self, action ):
        self.action = action
        if self.check_state is not None:
            action.setCheckable( True )

        if action.isCheckable():
            action.toggled.connect( self.sendCode )
        else:
            action.triggered.connect( self.sendCode )

    def setChecked( self, all_status ):
        # setting checked calls back into the handler, sendCode
        # block the signals to prevent this
        self.action.blockSignals( True )
        self.action.setChecked( all_status[ self.check_state ] )
        self.action.blockSignals( False )

class BemacsMainWindow(QtWidgets.QMainWindow):
    def __init__( self, app ):
        self.app = app
        self.log = self.app.log

        self.__all_actions = []
        self.__all_check_actions = []

        title = T_("Barry's Emacs")

        super().__init__()
        self.setWindowTitle( title )
        self.setWindowIcon( be_images.getIcon( 'bemacs.png' ) )

        # want to catch exceptions here
        self.setStatus = be_exceptions.TryWrapper( self.app.log, self._setStatus )

        self.emacs_panel = be_emacs_panel.EmacsPanel( self.app, self )
        self.setCentralWidget( self.emacs_panel )

        self.__setupMenuBar()
        self.__setupToolBar()
        self.__setupStatusBar( self.emacs_panel.font )

        geometry = self.app.getFrameGeometry()
        if geometry is not None:
            geometry = QtCore.QByteArray( geometry.encode('utf-8') )
            self.restoreGeometry( QtCore.QByteArray.fromHex( geometry ) )
            if( self.size().width() < 100
            or self.size().height() < 100 ):
                self.resize( 800, 600 )

        else:
            self.resize( 800, 600 )

        self.setAcceptDrops( True )

    def closeEvent( self, event ):
        if self.app.may_quit:
            self.log.info( 'closeEvent()' )

            self.app.setFrameGeometry( self.saveGeometry().toHex().data() )
            self.app.writePreferences()
            event.accept()

        else:
            # let the editor decide
            self.app.onCloseEditor()
            event.ignore()

    def __setupMenuBar( self ):
        mb = self.menuBar()

        menu_file = mb.addMenu( T_('&File') )
        self.addEmacsMenu( menu_file, 'fo', T_('&Open…'), 'toolbar_images/file_open.png' )
        menu_file.addSeparator()
        self.addEmacsMenu( menu_file, 'fs', T_('&Save'), 'toolbar_images/file_save.png' )
        self.addEmacsMenu( menu_file, 'fa', T_('Save &As…'), 'toolbar_images/file_save.png' )
        self.addEmacsMenu( menu_file, 'fl', T_('Save A&ll Modified Files'), 'toolbar_images/file_save_all.png' )

        if sys.platform != 'darwin':
            # on OS X Preferences and exit are not in the file menu
            # so no need for a separator
            menu_file.addSeparator()

        act_prefs = menu_file.addAction( T_('&Preferences') )
        act_prefs.triggered.connect( self.onActPreferences )

        act_exit = menu_file.addAction( T_('E&xit') )
        act_exit.triggered.connect( self.app.onCloseEditor )

        menu_edit = mb.addMenu( T_('&Edit') )

        # need to add the standard shotcuts to the edit menu items
        # otherwise cut/copy/paste does not work in dialogs like save as
        self.addEmacsMenu( menu_edit, 'ec', T_('Copy'), 'toolbar_images/editcopy.png' )
        self.addEmacsMenu( menu_edit, 'ex', T_('Cut'), 'toolbar_images/editcut.png' )
        self.addEmacsMenu( menu_edit, 'ev', T_('Paste'), 'toolbar_images/editpaste.png' )
        menu_edit.addSeparator()

        self.addEmacsMenu( menu_edit, 'ea', T_('Select All') )

        menu_edit.addSeparator()
        self.addEmacsMenu( menu_edit, 'eS', T_('case-fold-search'), check_state='case-fold-search' )
        self.addEmacsMenu( menu_edit, 'eR', T_('replace-case'), check_state='replace-case' )

        menu_edit.addSeparator()
        self.addEmacsMenu( menu_edit, 'eg', T_('Goto Line…') )

        menu_edit.addSeparator()
        menu_edit_advanced = menu_edit.addMenu( T_('Advanced…') )

        self.addEmacsMenu( menu_edit_advanced, 'cu', T_('Case UPPER'), 'toolbar_images/case_upper.png' )
        self.addEmacsMenu( menu_edit_advanced, 'cl', T_('Case lower'), 'toolbar_images/case_lower.png' )
        self.addEmacsMenu( menu_edit_advanced, 'cc', T_('Case Capitalise'), 'toolbar_images/case_capitalise.png' )
        self.addEmacsMenu( menu_edit_advanced, 'ci', T_('Case inVERT'), 'toolbar_images/case_invert.png' )
        menu_edit_advanced.addSeparator()
        self.addEmacsMenu( menu_edit_advanced, 'ri', T_('Indent Region') )
        self.addEmacsMenu( menu_edit_advanced, 'rI', T_('Undent Region') )
        menu_edit_advanced.addSeparator()
        self.addEmacsMenu( menu_edit_advanced, 'rn', T_('Narrow Region') )
        self.addEmacsMenu( menu_edit_advanced, 'rw', T_('Widen Region') )

        menu_view = mb.addMenu( T_('&View') )
        self.addEmacsMenu( menu_view, 'vw', T_('View white space'),
            'toolbar_images/view_white_space.png', check_state='display-non-printing-characters' )
        self.addEmacsMenu( menu_view, 'vl', T_('Wrap long lines'),
            'toolbar_images/view_wrap_long.png', check_state='wrap-long-lines' )

        menu_macro = mb.addMenu( T_('&Macro') )
        self.addEmacsMenu( menu_macro, 'mr', T_('Record'), 'toolbar_images/macro_record.png' )
        self.addEmacsMenu( menu_macro, 'ms', T_('Stop Recording'), 'toolbar_images/macro_stop.png' )
        self.addEmacsMenu( menu_macro, 'mp', T_('Run'), 'toolbar_images/macro_play.png' )

        menu_build = mb.addMenu( T_('Build') )
        self.addEmacsMenu( menu_build, 'bc', T_('Compile') )
        self.addEmacsMenu( menu_build, 'bn', T_('Next Error') )
        self.addEmacsMenu( menu_build, 'bp', T_('Previous Error') )

        menu_tool = mb.addMenu( T_('&Tool') )
        self.addEmacsMenu( menu_tool, 'tg', T_('Grep in files…'), 'toolbar_images/tools_grep.png' )
        self.addEmacsMenu( menu_tool, 'tb', T_('Grep in buffers…') )
        self.addEmacsMenu( menu_tool, 'tc', T_('Grep current buffer…') )
        self.addEmacsMenu( menu_tool, 'rf', T_('Filter region…') )
        self.addEmacsMenu( menu_tool, 'rs', T_('Sort region') )

        menu_buffer = mb.addMenu( T_('&Buffer') )
        self.addEmacsMenu( menu_buffer, 'bs', T_('Switch to buffer…') )
        self.addEmacsMenu( menu_buffer, 'bl', T_('List buffers') )

        menu_window = mb.addMenu( T_('&Window') )
        self.addEmacsMenu( menu_window, 'wh', T_('Split Horizontal'), 'toolbar_images/window_split_horiz.png' )
        self.addEmacsMenu( menu_window, 'wv', T_('Split Vertical'), 'toolbar_images/window_split_vert.png' )
        self.addEmacsMenu( menu_window, 'wo', T_('Delete Other'), 'toolbar_images/window_del_other.png' )
        self.addEmacsMenu( menu_window, 'wt', T_('Delete This'), 'toolbar_images/window_del_this.png' )

        menu_help = mb.addMenu( T_('&Help' ) )
        act = menu_help.addAction( T_('Documentation…') )
        act.triggered.connect( self.onActDocumentation )
        act = menu_help.addAction( T_("&About…") )
        act.triggered.connect( self.onActAbout )

    def addEmacsMenu( self, menu, code, title, icon_name=None, check_state=None, shortcut=None ):
        action = BemacsAction( self.app, code, check_state )
        self.__all_actions.append( action )
        if check_state is not None:
            self.__all_check_actions.append( action )

        if icon_name is None:
            qt_action = menu.addAction( title )
        else:
            icon = be_images.getIcon( icon_name )
            qt_action = menu.addAction( icon, title )

        if shortcut is not None:
            qt_action.setShortcuts( [QtGui.QKeySequence( shortcut )] )

        action.connect( qt_action )

    def __setupToolBar( self ):
        # Add tool bar
        t = self.addToolBar( 'main' )

        self.addEmacsToolbar( t, 'fo', T_('Open'), 'toolbar_images/file_open.png' )
        self.addEmacsToolbar( t, 'fs', T_('Save'), 'toolbar_images/file_save.png' )
        self.addEmacsToolbar( t, 'fl', T_('Save All Modified Files'), 'toolbar_images/file_save_all.png' )

        self.addEmacsToolbar( t, 'ex', T_('Cut'), 'toolbar_images/editcut.png' )
        self.addEmacsToolbar( t, 'ec', T_('Copy'), 'toolbar_images/editcopy.png' )
        self.addEmacsToolbar( t, 'ev', T_('Paste'), 'toolbar_images/editpaste.png' )

        t.addSeparator()
        self.addEmacsToolbar( t, 'eS', T_('Fold'), check_state='case-fold-search' )
        self.addEmacsToolbar( t, 'eR', T_('Replace'), check_state='replace-case' )

        t.addSeparator()
        self.addEmacsToolbar( t, 'wo', T_('Delete other window'), 'toolbar_images/window_del_other.png' )
        self.addEmacsToolbar( t, 'wt', T_('Delete this window'), 'toolbar_images/window_del_this.png' )
        self.addEmacsToolbar( t, 'wh', T_('Split Horizontal'), 'toolbar_images/window_split_horiz.png' )
        self.addEmacsToolbar( t, 'wv', T_('Split Vertical'), 'toolbar_images/window_split_vert.png' )

        t.addSeparator()
        self.addEmacsToolbar( t, 'cu', T_('UPPER'), 'toolbar_images/case_upper.png' )
        self.addEmacsToolbar( t, 'cl', T_('lower'), 'toolbar_images/case_lower.png' )
        self.addEmacsToolbar( t, 'cc', T_('Capitalise'), 'toolbar_images/case_capitalise.png' )
        self.addEmacsToolbar( t, 'ci', T_('inVERT'), 'toolbar_images/case_invert.png' )

        t.addSeparator()
        self.addEmacsToolbar( t, 'mr', T_('Record'), 'toolbar_images/macro_record.png' )
        self.addEmacsToolbar( t, 'ms', T_('Stop'), 'toolbar_images/macro_stop.png' )
        self.addEmacsToolbar( t, 'mp', T_('Run'), 'toolbar_images/macro_play.png' )

        t.addSeparator()
        self.addEmacsToolbar( t, 'vw', T_('White Space'),
            'toolbar_images/view_white_space.png', check_state='display-non-printing-characters' )
        self.addEmacsToolbar( t, 'vl', T_('Wrap Long'),
            'toolbar_images/view_wrap_long.png', check_state='wrap-long-lines' )

        t.addSeparator()
        self.addEmacsToolbar( t, 'tg', T_('Grep in files…'), 'toolbar_images/tools_grep.png' )

    def addEmacsToolbar( self, container, code, title, icon_name=None, check_state=None ):
        action = BemacsAction( self.app, code, check_state )
        self.__all_actions.append( action )
        if check_state is not None:
            self.__all_check_actions.append( action )

        if icon_name is not None:
            icon = be_images.getIcon( icon_name )
            qt_action = container.addAction( icon, title )

        else:
            qt_action = container.addAction( title )

        action.connect( qt_action )

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
        if not user_guide.exists():
            self.log.error( 'Expected user guide %r to exist' % (user_guide,) )
            return

        # replace \ on windows with / for a good URL.
        url = user_guide.as_uri()
        url = QtCore.QUrl( url )

        rc = QtGui.QDesktopServices.openUrl( url )
        if not rc:
            self.log.error( 'Failed to open documentation for URL %r' % (url,) )

    def onActAbout( self ):
        all_about_info = []
        all_about_info.append( T_("Barry's Emacs %d.%d.%d") %
                                (be_version.major, be_version.minor, be_version.patch) )
        all_about_info.append( 'Commit\xa0%s' % (be_version.commit,) )
        all_about_info.append( 'Python %d.%d.%d %s %d' %
                                (sys.version_info.major
                                ,sys.version_info.minor
                                ,sys.version_info.micro
                                ,sys.version_info.releaselevel
                                ,sys.version_info.serial) )
        all_about_info.append( 'PyQt %s, Qt %s' % (Qt.PYQT_VERSION_STR, QtCore.QT_VERSION_STR) )
        all_about_info.append( T_('Copyright Barry Scott (c) 1980-%s. All rights reserved') % (be_version.year,) )

        QtWidgets.QMessageBox.information( self, T_("About Barry's Emacs"), '\n'.join( all_about_info ) )

    def _setStatus( self, all_status ):
        self.status_read_only   .setText( {True: 'RO', False: ''}[all_status['readonly']] )
        self.status_insert_mode .setText( {True: 'Over', False: 'Ins '}[all_status['overstrike']] )
        self.status_eol         .setText( all_status['eol'].upper() )
        self.status_line_num    .setText( '%d' % (all_status['line'],) )
        self.status_col_num     .setText( '%d' % (all_status['column'],) )

        for action in self.__all_check_actions:
            action.setChecked( all_status )

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
