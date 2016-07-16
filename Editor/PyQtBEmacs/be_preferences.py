'''
 ====================================================================
 Copyright (c) 2003-2016 Barry A Scott.  All rights reserved.

 This software is licensed as described in the file LICENSE.txt,
 which you should have received as part of this distribution.

 ====================================================================


    be_preferences.py

'''
import pathlib
import sys

from  xml_preferences import XmlPreferences, Scheme, SchemeNode, PreferencesNode

import be_emacs_panel


class Preferences(PreferencesNode):
    def __init__( self ):
        super().__init__()
        self.window = None

    def finaliseNode( self ):
        if self.window is None:
            self.window = Window()
            self.window.finaliseNode()

class Window(PreferencesNode):
    def __init__( self ):
        super().__init__()
        self.geometry = None
        self.font = None
        self.all_colours = {}
        self.cursor = None

    def getFrameGeometry( self ):
        return self.geometry

    def setFrameGeometry( self, geometry ):
        self.geometry = geometry.decode('utf-8')

    def getColour( self, name ):
        return self.all_colours[ name ]

    def finaliseNode( self ):
        if self.font is None:
            self.font = Font()
            self.font.finaliseNode()

        for colour_info in be_emacs_panel.all_colour_defaults:
            if colour_info.name not in self.all_colours:
                self.all_colours[ colour_info.name ] = Colour( colour_info.name, colour_info.fg, colour_info.bg )

    def setChildNodeMap( self, name, key, node ):
        if name == 'colour':
            self.all_colours[ key ] = node

        else:
            raise RuntimeError( 'unknown name %r' % (name,) )

    def getChildNodeMap( self, name ):
        if name == 'colour':
            return sorted( self.all_colours.values() )

        else:
            raise RuntimeError( 'unknown name %r' % (name,) )

class Font(PreferencesNode):
    def __init__( self ):
        super().__init__()
        # point size and face need to chosen by platform
        if sys.platform.startswith( 'win' ):
            self.face = 'Courier New'
            self.point_size = 14

        elif sys.platform == 'darwin':
            self.face = 'Monaco'
            self.point_size = 14

        else:
            # Assuming linux/xxxBSD
            self.face = 'Liberation Mono'
            self.point_size = 11

    def setAttr( self, name, value ):
        if name == 'point_size':
            self.point_size = int(value)

        else:
            super().setAttr( name, value )

class Colour(PreferencesNode):
    def __init__( self, name, fg=None, bg=None ):
        super().__init__()
        self.name = name
        self.fg = fg
        self.bg = bg

    def __lt__( self, other ):
        return self.name < other.name

    def setAttr( self, name, value ):
        if name == 'fg':
            self.fg = tuple( [int(v) for v in value.split(',')] )

        elif name == 'bg':
            self.bg = tuple( [int(v) for v in value.split(',')] )

        else:
            super().setAttr( name, value )

    def getAttr( self, name ):
        if name == 'fg':
            return '%d,%d,%d' % self.fg

        elif name == 'bg':
            return '%d,%d,%d' % self.bg

        else:
            return super().getAttr( name )

class Cursor(PreferencesNode):
    def __init__( self, fg=None ):
        super().__init__()
        self.fg = fg

    def setAttr( self, name, value ):
        if name == 'fg':
            self.fg = tuple( [int(v) for v in value.split(',')] )

        else:
            super().setAttr( name, value )

    def getAttr( self, name ):
        if name == 'fg':
            return '%d,%d,%d,%d' % self.fg

        else:
            return super().getAttr( name )

    def finaliseNode( self ):
        if self.fg is None:
            self.fg = be_emacs_panel.cursor_fg_default

bemacs_preferences_scheme = (Scheme(
        (SchemeNode( Preferences, 'preferences',  )
        <<  (SchemeNode( Window, 'window', ('geometry',) )
            << SchemeNode( Font, 'font', ('point_size', 'face') )
            << SchemeNode( Colour, 'colour', ('fg', 'bg'), key_attribute='name' )
            << SchemeNode( Cursor, 'cursor', ('fg',), default_attributes={'fg': '%d,%d,%d,%d' % be_emacs_panel.cursor_fg_default} )
            )
        )
    ) )

class BemacsPreferenceManager:
    def __init__( self, app, filename ):
        self.xml_prefs = XmlPreferences( bemacs_preferences_scheme )

        self.app = app

        self.prefs_filename = filename

        assert isinstance( filename, pathlib.Path )

        try:
            self.prefs = self.xml_prefs.load( self.prefs_filename )

        except IOError:
            self.prefs = Preferences()
            self.prefs.finaliseNode()

    def getPrefs( self ):
        return self.prefs

    def writePreferences( self ):
        tmp_filename = self.prefs_filename.with_suffix( '.tmp' )

        self.xml_prefs.saveAs( self.prefs, tmp_filename )

        old_filename = self.prefs_filename.with_suffix( '.old.xml' )

        if self.prefs_filename.exists():
            if old_filename.exists():
                old_filename.unlink()

            self.prefs_filename.rename( old_filename )

        tmp_filename.rename( self.prefs_filename )
