'''
 ====================================================================
 Copyright (c) 2003-2010 Barry A Scott.  All rights reserved.

 This software is licensed as described in the file LICENSE.txt,
 which you should have received as part of this distribution.

 ====================================================================


    be_preferences.py

'''
import pprint

import sys
import os
import types

import xml.parsers.expat
import xml.dom.minidom
import xml.sax.saxutils

class ParseError(Exception):
    def __init__( self, value ):
        self.value = value

    def __str__( self ):
        return str(self.value)

    def __repr__( self ):
        return repr(self.value)


class Preferences:
    def __init__( self, app, pref_filename ):
        self.app = app
        self.pref_filename = pref_filename

        self.pref_data = None

        # all the preference section handles get created here
        self.pref_handlers = {}
        self.pref_handlers['Window'] = WindowPreferences( self.app )
        self.pref_handlers['Font'] = FontPreferences( self.app )

        # read preferences into the handlers
        self.readPreferences()

    def readPreferences( self ):
        try:
            self.pref_data = PreferenceData( self.app.log, self.pref_filename )

        except ParseError as e:
            self.app.log.error( str(e) )
            return

        for handler in self.pref_handlers.values():
            if self.pref_data.has_section( handler.section_name ):
                handler.readPreferences( self.pref_data )

    def __getattr__( self, name ):
        # support getProjects(), getFoobars() etc.
        if name[0:3] == 'get':
            section_name = name[3:]
            if self.pref_handlers.has_key( section_name ):
                return self.pref_handlers[ section_name ]

        raise AttributeError( '%s has no attribute %s' % (self.__class__.__name__, name ) )

    def writePreferences( self ):
        try:
            for handler in self.pref_handlers.values():
                self.pref_data.remove_section( handler.section_name )
                self.pref_data.add_section( handler.section_name )
                handler.writePreferences( self.pref_data )

            # write the prefs so that a failure to write does not
            # destroy the original
            # also keep one backup copy
            new_name = self.pref_filename + '.tmp'
            old_name = self.pref_filename + '.old'

            f = file( new_name, 'w' )
            self.pref_data.write( f )
            f.close()
            if os.path.exists( self.pref_filename ):
                if os.path.exists( old_name ): # os.rename does not delete automatically on Windows.
                    os.remove( old_name )
                os.rename( self.pref_filename, old_name )
            os.rename( new_name, self.pref_filename )

            self.app.log.info( 'Wrote preferences to %s' % self.pref_filename )

        except EnvironmentError as e:
            self.app.log.error( 'write preferences: %s' % e )

class PreferenceData:
    def __init__( self, log, xml_pref_filename ):
        self.all_sections = {}

        if os.path.exists( xml_pref_filename ):
            log.info( 'Reading preferences from %s' % xml_pref_filename )
            self.__readXml( xml_pref_filename )

    def __readXml( self, xml_pref_filename ):
        try:
            f = open( xml_pref_filename, 'r' )
            text = f.read()
            f.close()

            dom = xml.dom.minidom.parseString( text )

        except IOError as e:
            raise ParseError( str(e) )

        except xml.parsers.expat.ExpatError as e:
            raise ParseError( str(e) )

        prefs = dom.getElementsByTagName( 'bemacs-preferences' )[0]

        self.__parseXmlChildren( prefs, self.all_sections )

    def __parseXmlChildren( self, parent, data_dict ):
        for child in parent.childNodes:
            if child.nodeType == xml.dom.minidom.Node.ELEMENT_NODE:

                if self.__hasChildElements( child ):
                    child_data_dict = {}
                    if child.nodeName in data_dict:
                        if type(data_dict[ child.nodeName ]) != types.ListType:
                            data_dict[ child.nodeName ] = [data_dict[ child.nodeName], child_data_dict]
                        else:
                            data_dict[ child.nodeName ].append( child_data_dict )
                    else:
                        data_dict[ child.nodeName ] = child_data_dict

                    self.__parseXmlChildren( child, child_data_dict )
                else:
                    data_dict[ child.nodeName ] = self.__getText( child )

    def __hasChildElements( self, parent ):
        for child in parent.childNodes:
            if child.nodeType == xml.dom.minidom.Node.ELEMENT_NODE:
                return True
        return False

    def __getText( self, parent ):
        all_text = []

        for child in parent.childNodes:
            if child.nodeType == xml.dom.minidom.Node.TEXT_NODE:
                all_text.append( child.nodeValue )

        return ''.join( all_text )

    def __getElem( self, element_path ):
        node = self._dom
        for element_name in element_path:
            children = node.childNodes
            node = None
            for child in children:
                if child.nodeType == xml.dom.minidom.Node.ELEMENT_NODE and child.nodeName == element_name:
                    node = child
                    break
            if node is None:
                break

        return node

    def __getAttr( self, element_path, attrib_name ):
        element = self.getElement( element_path )
        if element.hasAttributes() and element.attributes.has_key( attrib_name ):
            return element.attributes[ attrib_name ].value
        return default


    def has_section( self, section_name ):
        return section_name in self.all_sections

    def len_section( self, section_name, option_name ):
        if type(self.all_sections[ section_name ][ option_name ]) == types.ListType:
            length = len( self.all_sections[ section_name ][ option_name ] )
        else:
            length = 1
        return length

    def has_option( self, section_name, option_name ):
        return option_name in self.all_sections[ section_name ]

    def get( self, section_name, option_name ):
        return self.all_sections[ section_name ][ option_name ]

    def getint( self, section_name, option_name ):
        return int( self.get( section_name, option_name ).strip() )

    def getfloat( self, section_name, option_name ):
        return float( self.get( section_name, option_name ).strip() )

    def getboolean( self, section_name, option_name ):
        return self.get( section_name, option_name ).strip().lower() == 'true'

    def remove_section( self, section_name ):
        if section_name in self.all_sections:
            del self.all_sections[ section_name ]

    def add_section( self, section_name ):
        self.all_sections[ section_name ] = {}

    def append_dict( self, section_name, list_name, data ):
        item_list = self.all_sections[ section_name ].setdefault( list_name, [] )
        item_list.append( data )

    def set( self, section_name, option_name, value ):
        self.all_sections[ section_name ][ option_name ] = value

    def write( self, f ):
        f.write( '<?xml version="1.0" encoding="UTF-8" standalone="yes"?>\n' )
        f.write( '<bemacs-preferences>\n' )
        self.__writeDictionary( f, self.all_sections, 4 )
        f.write( '</bemacs-preferences>\n' )

    def __writeDictionary( self, f, d, indent ):
        all_key_names = d.keys()
        all_key_names.sort()

        for key_name in all_key_names:
            value = d[ key_name ]
            if type(value) == types.DictType:
                if len(value) > 0:
                    f.write( '%*s<%s>\n' % (indent, '', key_name) )
                    self.__writeDictionary( f, value, indent + 4 )
                    f.write( '%*s</%s>\n' % (indent, '', key_name) )
            elif type(value) == types.ListType:
                for item in value:
                    f.write( '%*s<%s>\n' % (indent, '', key_name) )
                    self.__writeDictionary( f, item, indent + 4 )
                    f.write( '%*s</%s>\n' % (indent, '', key_name) )
            else:
                quoted_value = xml.sax.saxutils.escape( unicode( value ) ).encode('utf-8')
                f.write( '%*s<%s>%s</%s>\n' % (indent, '', key_name, quoted_value, key_name) )

class PreferenceSection:
    def __init__( self, section_name ):
        self.section_name = section_name

    def readPreferences( self, pref_data ):
        pass

    def writePreferences( self, pref_data ):
        pass

    # support being returned by the __getattr__ above
    def __call__( self ):
        return self

class GetOption:
    def __init__( self, pref_data, section_name ):
        self.pref_data = pref_data
        self.section_name = section_name

    def has( self, name ):
        return self.pref_data.has_option( self.section_name, name )

    def getstr( self, name ):
        return self.pref_data.get( self.section_name, name ).strip()

    def getint( self, name ):
        return self.pref_data.getint( self.section_name, name )

    def getfloat( self, name ):
        return self.pref_data.getfloat( self.section_name, name )

    def getbool( self, name ):
        return self.pref_data.getboolean( self.section_name, name )

    def getstrlist( self, name, sep ):
        s = self.getstr( name )
        if len(s) == 0:
            return []
        return [p.strip() for p in s.split( sep )]

class SetOption:
    def __init__( self, pref_data, section_name ):
        self.pref_data = pref_data
        self.section_name = section_name

    def set( self, name, value, sep='' ):
        if type(value) == types.ListType:
            value = sep.join( value )

        self.pref_data.set( self.section_name, name, value )

class GetIndexedOption:
    def __init__( self, pref_data, section_name, index, index_name ):
        self.pref_list = pref_data.get( section_name, index_name )
        if type(self.pref_list) != types.ListType:
            self.pref_list = [self.pref_list]

        self.index = index

    def has( self, name ):
        return name in self.pref_list[ self.index ]

    def get( self, name ):
        return self.pref_list[ self.index ][ name ]

    def getstr( self, name ):
        return self.get( name ).strip()

    def getint( self, name ):
        return int( self.getstr( name ) )

    def getfloat( self, name ):
        return float( self.getstr( name ) )

    def getbool( self, name ):
        return self.getstr( name ) == 'true'

class WindowPreferences(PreferenceSection):
    def __init__( self, app ):
        PreferenceSection.__init__( self, 'Window' )
        self.app = app

        self.frame_size = (700, 500)
        self.frame_position = (-1, -1)
        self.maximized = False
        self.zoom = 0

    def readPreferences( self, pref_data ):
        get_option = GetOption( pref_data, self.section_name )
        x = get_option.getint( 'pos_x' )
        if x < 0:
            x = 0
        y = get_option.getint( 'pos_y' )
        if y < 0:
            y = 0
        self.frame_position = ( x, y )

        w = get_option.getint( 'width' )
        h = get_option.getint( 'height' )
        self.frame_size = ( w, h )

        self.maximized = get_option.getbool( 'maximized' )
        if get_option.has( 'zoom' ):
            self.zoom = get_option.getint( 'zoom' )

    def writePreferences( self, pref_data ):
        set_option = SetOption( pref_data, self.section_name )

        set_option.set( 'pos_x', self.frame_position.x )
        set_option.set( 'pos_y', self.frame_position.y )
        set_option.set( 'width', self.frame_size.GetWidth() )
        set_option.set( 'height', self.frame_size.GetHeight() )
        set_option.set( 'maximized', self.maximized )
        set_option.set( 'zoom', self.zoom )

class FontPreferences(PreferenceSection):
    def __init__( self, app ):
        PreferenceSection.__init__( self, 'Font' )
        self.app = app

        self.point_size = 14
        # point size and face need to choosen for platform
        if sys.platform.startswith( 'win' ):
            self.face = 'Courier New'

        elif sys.platform == 'darwin':
            self.face = 'Monaco'

        else:
            # Assuming linux/xxxBSD
            self.face = 'Liberation Mono'
            self.point_size = 11

    def readPreferences( self, pref_data ):
        get_option = GetOption( pref_data, self.section_name )
        self.face = get_option.getstr( 'face' )
        self.point_size = get_option.getint( 'point_size' )

    def writePreferences( self, pref_data ):
        set_option = SetOption( pref_data, self.section_name )

        set_option.set( 'face', self.face )
        set_option.set( 'point_size', self.point_size )
