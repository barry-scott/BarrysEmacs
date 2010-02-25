'''
 ====================================================================
 Copyright (c) 2008-2010 Barry A Scott.  All rights reserved.

 This software is licensed as described in the file LICENSE.txt,
 which you should have received as part of this distribution.

 ====================================================================


    be_platform_win32_specific.py

'''
from win32com.shell import shell, shellcon
import os

SHGFP_TYPE_CURRENT = 0
SHGFP_TYPE_DEFAULT = 1

app_dir = None

def getUserDir():
    app_folder = shell.SHGetFolderPath( 0, shellcon.CSIDL_APPDATA, 0, SHGFP_TYPE_CURRENT )
    return os.environ.get( 'BEMACS_USER', os.path.join( app_folder, 'Bemacs' ) )

def getLibraryDir():
    return os.environ.get( 'BEMACS_LIBRARY', os.path.join( app_dir, 'emacs_library' ) )

def getLocalePath( app ):
    return os.path.join( app_dir, 'locale' )

def getNullDevice():
    return 'NUL'

def setupPlatformSpecific_( argv0 ):
    if argv0.startswith( '/' ):
        app_dir = os.path.dirname( argv0 )

    elif '/' in argv0:
            app_dir = os.path.dirname( os.path.abspath( argv0 ) )

    else:
        for folder in [p.strip() for s in os.environ.get( 'PATH', '' ).split( ';' )]:
            app_path = os.path.join( folder, argv0 )
            if os.path.exists( app_path ):
                app_dir = os.path.dirname( app_path )
                break

    if app_dir == '':
        app_dir = os.getcwd()
