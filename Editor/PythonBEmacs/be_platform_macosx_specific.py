'''
 ====================================================================
 Copyright (c) 2008-2010 Barry A Scott.  All rights reserved.

 This software is licensed as described in the file LICENSE.txt,
 which you should have received as part of this distribution.

 ====================================================================


    be_platform_macosx_specific.py

'''
import os

app_dir = None

def getUserDir():
    return os.environ.get( 'BEMACS_USER', os.path.join( os.environ['HOME'], 'bemacs' ) )

def getLibraryDir():
    return os.environ.get( 'BEMACS_LIBRARY', os.path.join( app_dir, 'emacs_library' ) )

def getLocalePath( app ):
    return os.path.join( app_dir, 'locale' )

def getNullDevice():
    return '/dev/null'

def setupPlatformSpecific_( argv0 ):
    global app_dir

    if argv0.startswith( '\\' ):
        app_dir = os.path.dirname( argv0 )

    elif '\\' in argv0:
            app_dir = os.path.dirname( os.path.abspath( argv0 ) )

    else:
        for folder in [p.strip() for p in ['.'] + os.environ.get( 'PATH', '' ).split( ':' )]:
            app_path = os.path.abspath( os.path.join( folder, argv0 ) )
            if os.path.exists( app_path ):
                app_dir = os.path.abspath( os.path.dirname( app_path ) )
                break
