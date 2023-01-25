'''
 ====================================================================
 Copyright (c) 2003-2019 Barry A Scott.  All rights reserved.

 This software is licensed as described in the file LICENSE.txt,
 which you should have received as part of this distribution.

 ====================================================================


    be_preferences.py

'''
from pathlib import Path
import sys

from  xml_preferences import XmlPreferences, Scheme, SchemeNode, PreferencesNode

import be_emacs_panel

# cannot use a class for bool as PyQt wll not use __bool__ to get its truth
def Bool( text ):
    if text.lower() == 'true':
        return True

    elif text.lower() == 'false':
        return False

    else:
        raise ValueError( 'Bool expects the string true or false' )

class RGB:
    def __init__( self, value=None ):
        if value is None:
            self.value = None
            return

        if type(value) is tuple:
            self.value = value

        else:
            self.value = tuple( [int(v) for v in value.split(',')] )

        if len(self.value) != 3:
            raise ValueError( 'RGB need 3 values: %r' % (value,) )

        for v in self.value:
            if not (0 <= v <= 255):
                raise ValueError( 'RGB values must be in the range 0..255: %r - %r' % (value, v) )

    def getTuple( self ):
        return self.value

    def setTuple( self, value ):
        assert type(value) == tuple, 'value %r' % (value,)
        assert len(value) == 3, 'value %r' % (value,)
        self.value = value

    def __str__( self ):
        return '%d,%d,%d' % self.value

    def __repr__( self ):
        return '<RGB R:%d G:%d B:%d>' % self.value

class RGBA:
    def __init__( self, value=None ):
        if value is None:
            self.value = None
            return

        if type(value) is tuple:
            self.value = value

        else:
            self.value = tuple( [int(v) for v in value.split(',')] )

        if len(self.value) != 4:
            raise ValueError( 'RGBA need 4 values: %r' % (value,) )

        for v in self.value:
            if not (0 <= v <= 255):
                raise ValueError( 'RGBA values must be in the range 0..255: %r - %r' % (value, v) )

    def getTuple( self ):
        return self.value

    def setTuple( self, value ):
        assert type(value) == tuple, 'value %r' % (value,)
        assert len(value) == 4, 'value %r' % (value,)
        self.value = value

    def __str__( self ):
        return '%d,%d,%d,%d' % self.value

    def __repr__( self ):
        return '<RGB R:%d G:%d B:%d A:%d>' % self.value

# ------------------------------------------------------------
class Preferences(PreferencesNode):
    def __init__( self ):
        super().__init__()
        self.window = None

    def finaliseNode( self ):
        if self.window is None:
            self.window = Window()
            self.window.finaliseNode()

# ------------------------------------------------------------
class Session(PreferencesNode):
    xml_attribute_info = (('geometry', str),)

    def __init__( self ):
        super().__init__()
        self.geometry = None

    def getFrameGeometry( self ):
        return self.geometry

    def setFrameGeometry( self, geometry ):
        self.geometry = geometry.decode('utf-8')

# ------------------------------------------------------------
class Window(PreferencesNode):
    xml_attribute_info = (('theme', str), ('geometry', str))

    def __init__( self ):
        super().__init__()
        self.theme = None
        self.geometry = None
        self.font = None
        self.all_colours = {}
        self.cursor = None
        self.cursor_style = None

    def getFrameGeometry( self ):
        return self.geometry

    def getColour( self, name ):
        return self.all_colours[ name ]

    def finaliseNode( self ):
        if self.theme is None:
            if app.user_window_preference_is_dark_mode:
                self.theme = Theme( name='Dark' )
            else:
                self.theme = Theme( name='Light' )

        if self.font is None:
            self.font = Font()
            self.font.finaliseNode()

        if self.cursor is None:
            self.cursor = CursorColour( be_emacs_panel.all_themes[self.theme.name].cursor_fg )

        for colour_info in be_emacs_panel.all_themes[self.theme.name].all_colours:
            if colour_info.name not in self.all_colours:
                self.all_colours[ colour_info.name ] = Colour( colour_info.name, RGB( colour_info.fg ), RGB( colour_info.bg ) )

        if self.cursor_style is None:
            self.cursor_style = CursorStyle()

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
    xml_attribute_info = (('face', str), ('point_size', int))

    def __init__( self ):
        super().__init__()
        # point size and face need to chosen by platform
        if sys.platform.startswith( 'win' ):
            self.face = 'Courier New'
            self.point_size = 14

        elif sys.platform == 'darwin':
            self.face = 'Monaco'
            self.point_size = 14

        elif sys.platform == 'linux':
            if Path( '/etc/os-release' ).exists():
                # assume this is debian/ubuntu
                self.face = 'Noto Mono'
                self.point_size = 12

            else:
                # assume fedora
                self.face = 'Fira Mono'
                self.point_size = 12

        else:
            # Assuming linux/xxxBSD
            self.face = 'Liberation Mono'
            self.point_size = 11


class CursorStyle(PreferencesNode):
    xml_attribute_info = (('blink', Bool), ('interval', int), ('shape', str))

    def __init__( self, blink=True, interval=600, shape='line' ):
        super().__init__()
        self.blink = blink
        self.interval = interval
        self.shape = shape

    def setAttr( self, name, value ):
        super().setAttr( name, value )

class Colour(PreferencesNode):
    xml_attribute_info = (('fg', RGB), ('bg', RGB))

    def __init__( self, name, fg=None, bg=None ):
        super().__init__()
        self.name = name
        self.fg = fg
        self.bg = bg

    def __lt__( self, other ):
        return self.name < other.name

class CursorColour(PreferencesNode):
    xml_attribute_info = (('fg', RGBA),)

    def __init__( self, fg=None ):
        super().__init__()
        if fg is None:
            self.fg = None

        else:
            self.fg = RGBA()
            self.fg.setTuple( fg )

    def finaliseNode( self ):
        if self.fg is None:
            self.fg = RGBA()
            self.fg.setTuple( be_emacs_panel.all_themes[be_emacs_panel.theme_name_default].cursor_fg )

class Theme(PreferencesNode):
    xml_attribute_info = (('name', str),)

    def __init__( self, name=None ):
        super().__init__()
        self.name = name

    def setAttr( self, name, value ):
        super().setAttr( name, value )

    def getAttr( self, name ):
        return super().getAttr( name )

    def finaliseNode( self ):
        if self.name is None:
            self.name = be_emacs_panel.theme_name_default

bemacs_preferences_scheme = (Scheme(
        (SchemeNode( Preferences, 'preferences',  )

        <<  (SchemeNode( Window, 'window', ('geometry',) )
            << SchemeNode( CursorStyle, 'cursor_style' )
            << SchemeNode( Font, 'font' )
            << SchemeNode( Theme, 'theme' )
            << SchemeNode( Colour, 'colour', key_attribute='name' )
            << SchemeNode( CursorColour, 'cursor' )
            )
        )
    ) )

bemacs_session_scheme = (Scheme(
        SchemeNode( Session, 'session', ('geometry',) )
    ) )

app = None

class BemacsPreferenceManager:
    def __init__( self, app_, prefs_filename, session_filename ):
        self.xml_prefs = XmlPreferences( bemacs_preferences_scheme )
        self.xml_session = XmlPreferences( bemacs_session_scheme )

        global app
        app = app_

        self.prefs_filename = prefs_filename
        self.session_filename = session_filename

        assert isinstance( prefs_filename, Path )
        assert isinstance( session_filename, Path )

        try:
            self.session = self.xml_session.load( self.session_filename )

        except IOError:
            self.session = Session()
            self.session.finaliseNode()

        try:
            self.prefs = self.xml_prefs.load( self.prefs_filename )

        except IOError:
            self.prefs = Preferences()
            self.prefs.finaliseNode()

    def getPrefs( self ):
        return self.prefs

    def writePreferences( self ):
        self.prefs.window.geometry = None

        for filename, xml_scheme, data in (
                (self.prefs_filename, self.xml_prefs, self.prefs),
                (self.session_filename, self.xml_session, self.session)):
            tmp_filename = filename.with_suffix( '.tmp' )

            xml_scheme.saveAs( data, tmp_filename )

            old_filename = filename.with_suffix( '.old.xml' )

            if filename.exists():
                if old_filename.exists():
                    old_filename.unlink()

                filename.rename( old_filename )

            tmp_filename.rename( filename )

    def getFrameGeometry( self ):
        geometry = self.session.getFrameGeometry()
        if geometry is not None:
            return geometry

        return self.prefs.window.getFrameGeometry()

    def setFrameGeometry( self, geometry ):
        self.session.setFrameGeometry( geometry )
