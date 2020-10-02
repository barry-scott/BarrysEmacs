#!/usr/bin/env python3
import sys
import os
from pathlib import Path
import xml.dom.minidom
import re
import glob

try:
    import colour_text
    ct = colour_text.ColourText()
    ct.define( 'em', 'cyan' )

except ImportError:
    class ColourText:
        def initTerminal( self ):
            pass

        def __call__( self, text ):
            return text

#
# the docs use the following templete
#
html_head = '''<?xml version="1.0" encoding="UTF-8" standalone="no" ?>
<!DOCTYPE html>
<html>
<head>
<meta charset="UTF-8" />
<title>Barry's Emacs - %(title)s</title>
<link rel="stylesheet" href="emacs-docs.css" />
</head>
<body>
<div id="content">
<div id="index">
<div id="index-grid">
<h1>Barry's Emacs</h1>
<p>
'''
# index doc index as %(title)s<br/>
html_head_end = '''
</p>
'''
# insert optional index grid here
# insert index entries here
html_body = '''
</div> <!-- end id=index -->
<div id="body">
'''
# insert body here

html_foot = '''
</div>
</div>
</body>
</html>
'''

class BuildError(Exception):
    pass

class BuildDocs:
    def __init__( self, all_sections ):
        self.all_sections = all_sections

        self.opt_debug = False
        self.output_folder = None

        self.html_folder = Path( os.environ[ 'BUILDER_TOP_DIR' ] ) / 'HTML'

        self.doc_body_filenames = []

    def main( self, argv ):
        try:
            self.parseArgs( argv )
            self.buildHtmlDocs()
            return 0

        except BuildError as e:
            print( ct( '<>error Error:<> %s' % (e,) ), file=sys.stderr )
            return 1

    def buildHtmlDocs( self ):
        css_file_in = self.html_folder / 'emacs-docs.css'
        self.info( 'Asset %s' % (css_file_in.name,) )
        self.mustExist( css_file_in )

        css_file_out = self.output_folder / css_file_in

        with css_file_in.open( 'r' ) as f:
            css = f.read()

        with css_file_out.open( 'w' ) as f:
            f.write( css )

        for image in glob.glob( '%s/*.png' % (self.html_folder,) ):
            image_file_in = Path( image )
            image_file_out = self.output_folder / image_file_in.name

            self.info( 'Asset %s' % (image_file_in.name,) )
            with image_file_in.open( 'rb' ) as f:
                image = f.read()

            with image_file_out.open( 'wb' ) as f:
                f.write( image )

        for section in self.all_sections:
            self.buildSection( section )

    def info( self, msg ):
        print( ct( '<>info Info:<> %s' % (msg,) ) )

    def parseArgs( self, argv ):
        it_argv = iter( argv )
        self.prog_name = next(it_argv)

        try:
            while True:
                arg = next(it_argv)
                if arg == '--debug':
                    self.opt_debug = True

                elif arg.startswith( '--' ):
                    raise BuildError( 'Unknown option "%s"' % (arg,) )

                else:
                    self.output_folder = Path( arg )

        except StopIteration:
            pass

        if self.output_folder is None:
            raise BuildError( 'Required <output-folder> missing' )

        if not self.output_folder.exists():
            raise BuildError( 'Output folder does not exist - %s' % (self.output_folder,) )

    def buildSection( self, section ):
        section_file = self.output_folder / section.filename
        self.info( 'Creating <>info %s<> in <>em %s<>' % (section.title, section_file) )
        with section_file.open('w') as fo:
            fo.write( html_head % {'title': section.title} )
            self.buildDocIndex( fo, section )
            self.buildSectionGridIndex( fo, section.grid_index )
            self.buildSectionIndex( fo, section.index )
            fo.write( html_body )
            self.buildBody( fo )
            fo.write( html_foot )

        self.mustBeValidXml( section_file )

    def buildDocIndex( self, fo, section ):
        # add each of the sections titles
        for other in self.all_sections:
            if other is section:
                # no need to link this section
                fo.write( '%s<br />\n' % (other.title,) )

            else:
                fo.write( '<a href="%s">%s</a><br />\n' % (other.filename, other.title) )

        fo.write( html_head_end )

    def buildSectionGridIndex( self, fo, grid_index ):
        if grid_index is None:
            # end #index-grid
            fo.write( '</div>\n' )
            return

        grid_file = self.html_folder / grid_index

        self.info( 'Grid index from <>em %s<>' % (grid_file,) )

        self.mustExist( grid_file )
        self.mustBeValidXml( grid_file )

        # append to the index-grid div that
        # has the docs index in it
        in_index = False
        in_hide = False
        with grid_file.open( 'r' ) as fi:
            for line_no, line in enumerate( fi ):
                if not in_index:
                    if line == '<div class="index-grid">\n':
                        in_index = True
                    continue

                if in_index and line == '</body>\n':
                    break

                if line == '<!-- hide=on -->\n':
                    in_hide = True

                elif line == '<!-- hide=off -->\n':
                    in_hide = False
                    continue

                if in_hide:
                    continue

                fo.write( line )

        if not in_index:
            raise BuildError( '%s: missing <div class="index-grid">' % (grid_file,) )

        if in_hide:
            raise BuildError( '%s: missing <!-- hide=off -->' % (grid_file,) )


    def buildSectionIndex( self, fo, index ):
        self.doc_body_filenames = []

        index_file = self.html_folder / index

        self.info( 'Index from <>em %s<>' % (index_file,) )

        self.mustExist( index_file )
        self.mustBeValidXml( index_file )

        fo.write( '<!-- Source: %s -->\n' % (index_file,) )

        in_index = False
        in_hide = False
        with index_file.open( 'r' ) as fi:
            for line_no, line in enumerate( fi ):
                if not in_index:
                    # copy the index div but without its opening <div> line
                    # that has been already written out
                    if line == '<div class="index">\n':
                        in_index = True
                        fo.write( '<div id="index-entries">\n' )
                    continue

                if in_index and line == '</body>\n':
                    break

                if line == '<!-- hide=on -->\n':
                    in_hide = True

                elif line == '<!-- hide=off -->\n':
                    in_hide = False
                    continue

                if in_hide:
                    continue

                # copy the line after extacting filename of body file
                # and fixing the href to remove the filename
                if '<a href="' in line:
                    parts = line.split( '"', 2 )
                    href = parts[1]
                    if '#' not in href:
                        raise BuildError( '%s:%d: missing anchor in %r' % (index_file, line_no+1, line) )

                    filename, anchor = href.split( '#' )
                    line = '%s"#%s"%s' % (parts[0], anchor, parts[2])
                    if filename != '' and filename not in self.doc_body_filenames:
                        self.doc_body_filenames.append( filename )

                fo.write( line )

        if not in_index:
            raise BuildError( '%s: missing <div class="index">' % (index_file,) )

        if in_hide:
            raise BuildError( '%s: missing <!-- hide=off -->' % (grid_file,) )

        if len(self.doc_body_filenames) == 0:
            raise BuildError( 'No body files found in index' )


    def buildBody( self, fo ):
        for body_filename in self.doc_body_filenames:
            body_file = self.html_folder / body_filename
            self.info( 'Body from <>em %s<>' % (body_file.name,) )

            self.mustExist( body_file )
            self.mustBeValidXml( body_file )

            fo.write( '<!-- Source: %s -->\n' % (body_file.name,) )

            in_body = False
            in_hide = False
            with body_file.open( 'r' ) as fi:
                for line in fi:
                    if not in_body:
                        if line == '<body>\n':
                            in_body = True
                        continue

                    if in_body and line == '</body>\n':
                        break

                    if line == '<div class="contents">\n':
                        line = '<div>\n'

                    if line == '<!-- hide=on -->\n':
                        in_hide = True

                    elif line == '<!-- hide=off -->\n':
                        in_hide = False
                        continue

                    if in_hide:
                        continue

                    line = self.fixupAnchorHrefs( line )

                    fo.write( line )

            if in_hide:
                raise BuildError( '%s: missing <!-- hide=off -->' % (grid_file,) )


    def fixupAnchorHrefs( self, line ):
        all_href_spans = [m.span( 1 ) for m in re.finditer( '<a href="([^"]+)"', line )]
        all_href_spans.reverse()

        for span in all_href_spans:
            href = line[span[0]:span[1]]
            if href.startswith( '#' ) or  '#' not in href:
                continue

            filename, anchor = href.split( '#' )
            if filename in self.doc_body_filenames:
                line = '%s#%s%s' % (line[:span[0]], anchor, line[span[1]:])

        return line

    def mustExist( self, path ):
        if not path.exists():
            raise BuildError( 'Input file does not exist - %s' % (path,) )

    def mustBeValidXml( self, path ):
        with path.open( 'r' ) as fi:
            text = fi.read()

        try:
            dom = xml.dom.minidom.parseString( text )

        except xml.parsers.expat.ExpatError as e:
            raise BuildError( '%s: %s' % (path, e) )


class DocSection:
    def __init__( self, title, filename, index, grid_index=None ):
        self.title = title
        self.filename = filename
        self.index = index
        self.grid_index = grid_index

def main( argv ):
    build = BuildDocs( [
            DocSection( "User's Guide", 'users_guide.html', 'ug_index.html' ),
            DocSection( "Extensions Reference", 'extensions_reference.html', 'extn_index.html' ),
            DocSection( "MLisp Programmer's Guide", 'mlisp_programmers_guide.html', 'pg_index.html' ),
            DocSection( "MLisp Reference", 'mlisp_reference.html', 'mlisp_ref_index.html', 'mlisp_ref_grid.html' )] )

    return build.main( argv )

if __name__ == '__main__':
    sys.exit( main( sys.argv ) )
