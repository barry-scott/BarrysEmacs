'''
 ====================================================================
 Copyright (c) 2008-2015 Barry A Scott.  All rights reserved.

 This software is licensed as described in the file LICENSE.txt,
 which you should have received as part of this distribution.

 ====================================================================


    be_platform_macosx_specific.py

'''
import os
import pathlib

app_dir = None

def getAppDir():
    assert app_dir is not None, 'call setupPlatformSpecific_() first'
    return app_dir

def getUserDir():
    folder = os.environ.get( 'BEMACS_EMACS_USER', os.path.join( os.environ['HOME'], 'bemacs' ) )
    assert folder is not None
    return pathlib.Path( folder )

def getLibraryDir():
    assert app_dir is not None
    folder = pathlib.Path( os.environ.get( 'BEMACS_EMACS_LIBRARY', app_dir / 'emacs_library' ) )
    return folder

def getLocalePath( app ):
    return app_dir / 'locale'

def getDocDir():
    return app_dir / 'documentation'

def getDocUserGuide():
    return getDocDir() / 'users_guide.html'

def getNullDevice():
    return '/dev/null'

def setupPlatformSpecific_( argv0 ):
    global app_dir

    if argv0.startswith( '/' ):
        app_dir = pathlib.Path( argv0 ).parent.parent / 'Resources'

    elif '/' in argv0:
        app_dir = pathlib.Path( argv0 ).resolve().parent

    else:
        for folder in [pathlib.Path( p.strip() ) for p in ['.'] + os.environ.get( 'PATH', '' ).split( ':' )]:
            app_path = (folder / argv0).resolve()
            if app_path.exists():
                app_dir = app_path.parent
                break

    assert app_dir is not None

default_font_name = 'Monaco'
default_font_point_size = 14
