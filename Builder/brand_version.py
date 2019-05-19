#!/usr/bin/python3
from __future__ import print_function
import sys
import os
import time
import subprocess

template_file_prefix = 'brand.'

class Error(Exception):
    pass

if sys.version_info[0] == 3:
    def readFile( filename ):
        with open( filename, 'r', encoding='utf-8' ) as f:
            return f.read()

    def writeFile( filename, data ):
        with open( filename, 'w', encoding='utf-8' ) as f:
            f.write( data )

else:
    def readFile( filename ):
        with open( filename, 'rb' ) as f:
            return f.read().decode( 'utf-8' )

    def writeFile( filename, data ):
        with open( filename, 'wb' ) as f:
            f.write( data.encode( 'utf-8' ) )

def main( argv ):
    if len(argv) < 3:
        print( 'Usage: %s <version-info-file> <wc-path>' % argv[0] )
        return 1

    def info( msg ):
        print( 'Info: %s' % (msg,) )

    try:
        brandVersion( argv[1], argv[2], info )

    except Error as e:
        print( 'Error: %s' % str(e) )
        return 1

    return 0

def brandVersion( version_info_file, wc_path, log_info ):
    vi = VersionInfo( wc_path, log_info )
    vi.parseVersionInfo( version_info_file )

    finder = FileFinder( vi )
    finder.findAndBrandFiles( wc_path )


class FileFinder:
    def __init__( self, vi ):
        self.__vi = vi

    def findAndBrandFiles( self, path ):
        #print( 'findAndBrandFiles',path )
        all_files = os.listdir( path )

        for filename in all_files:
            #print( base )
            if( filename.startswith( template_file_prefix )
            and not filename.endswith( '~' )        # linux and mac edit file
            and not filename.split( '.' )[-1].startswith( '_' ) ):  # windows edit file
                self.__vi.brandOneFile( os.path.join( path, filename ) )

        for filename in all_files:
            full_path = os.path.join( path, filename )
            if( os.path.isdir( full_path )
            and not os.path.islink( full_path )
            and not full_path.endswith( '.git' )):
                self.findAndBrandFiles( full_path )

class VersionInfo:
    def __init__( self, wc_path, log_info ):
        self.log_info = log_info

        self.__info = {}
        now = time.localtime( time.time() )
        self.__info['year'] = time.strftime( '%Y', now )
        self.__info['date'] = time.strftime( '%Y-%m-%d', now )
        self.__info['time'] = time.strftime( '%H:%M:%S', now )

        commit_id_file = os.path.join( wc_path, 'Builder/commit_id.txt' )
        if os.path.exists( commit_id_file ):
            result = readFile( commit_id_file )

        else:
            result = subprocess.check_output( ['git', 'show-ref', '--head', '--hash', 'head'] )
            result = result.decode('utf-8')

        self.__info['commit'] = result.strip()

    def parseVersionInfo( self, filename ):
        contents = readFile( filename )
        for line in contents.split('\n'):
            line = line.strip()

            if line.startswith( '#' ):
                continue
            if line == '':
                continue

            key, value = line.split( ':', 1 )
            try:
                self.__info[ key.strip() ] = value.strip() % self.__info

            except (ValueError,TypeError,KeyError) as e:
                raise Error( 'Cannot format key %s with value "%s" because %s' % (key.strip(), value.strip(), str(e)) )

    def printInfo( self ):
        all_keys = self.__info.keys()
        all_keys.sort()
        for key in all_keys:
            self.log_info( '%10s: %s' % (key, self.__info[ key ]) )

    def brandOneFile( self, filename ):
        template_contents = readFile( filename ).split('\n')

        try:
            branded_contents = []
            for line_no, line in enumerate( template_contents ):
                branded_contents.append( line % self.__info )

        except (ValueError, TypeError, KeyError) as e:
            raise Error( 'Cannot format %s:%d because %s' % (filename, line_no+1, str(e)) )

        parent_dir = os.path.dirname( filename )
        base = os.path.basename( filename )
        new_filename = os.path.join( parent_dir, base[len(template_file_prefix):] )
        self.log_info( 'Creating %s' % (new_filename,) )

        writeFile( new_filename, '\n'.join( branded_contents ) )

if __name__ == '__main__':
    sys.exit( main( sys.argv ) )
