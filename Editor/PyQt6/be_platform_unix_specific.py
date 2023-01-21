'''
 ====================================================================
 Copyright (c) 2008-2015 Barry A Scott.  All rights reserved.

 This software is licensed as described in the file LICENSE.txt,
 which you should have received as part of this distribution.

 ====================================================================


    be_platform_unix_specific.py

'''
import os
import pathlib

app_dir = None
library_dir = None
doc_dir = None

def getAppDir():
    assert app_dir is not None, 'call setupPlatformSpecific_() first'
    return app_dir

def getUserDir():
    return pathlib.Path( os.environ.get( 'BEMACS_EMACS_USER', os.path.join( os.environ['HOME'], 'bemacs' ) ) )

def getLibraryDir():
    return pathlib.Path( os.environ.get( 'BEMACS_EMACS_LIBRARY', library_dir ) )

def getDocDir():
    return pathlib.Path( os.environ.get( 'BEMACS_EMACS_DOC', doc_dir ) )

def getLocalePath( app ):
    return app_dir / 'locale'

def getDocUserGuide():
    return getDocDir() / 'users_guide.html'

def getNullDevice():
    return '/dev/null'

def setupPlatformSpecific_( argv0 ):
    global app_dir

    if argv0.startswith( '/' ):
        app_dir = pathlib.Path( argv0 ).parent

    elif '/' in argv0:
        app_dir = pathlib.Path( argv0 ).resolve().parent

    else:
        for folder in [pathlib.Path( s.strip() ) for s in os.environ.get( 'PATH', '' ).split( ':' )]:
            app_path = folder / argv0
            if app_path.exists():
                app_dir = app_path.parent
                break

    if app_dir is None:
        app_dir = pathlib.Path( os.getcwd() )

# build will add definition of library_dir
# build will add definition of doc_dir
