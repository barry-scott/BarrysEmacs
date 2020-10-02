'''
 ====================================================================
 Copyright (c) 2008-2015 Barry A Scott.  All rights reserved.

 This software is licensed as described in the file LICENSE.txt,
 which you should have received as part of this distribution.

 ====================================================================


    be_platform_win32_specific.py

'''
import os
import ctypes
import pathlib

SHGFP_TYPE_CURRENT = 0
SHGFP_TYPE_DEFAULT = 1

CSIDL_APPDATA = 0x001A
CSIDL_PROFILE = 0x0028

MAX_PATH = 260

app_dir = None

def getAppDir():
    assert app_dir is not None, 'call setupPlatformSpecific_() first'
    return app_dir

def getUserDir():
    app_folder = ctypes.create_unicode_buffer( MAX_PATH + 1 )
    ctypes.windll.shell32.SHGetFolderPathW( 0, CSIDL_PROFILE, None, SHGFP_TYPE_CURRENT, ctypes.byref( app_folder ) )
    user_dir = os.environ.get( 'BEMACS_USER', os.path.join( app_folder.value, 'bemacs' ) )
    return pathlib.Path( user_dir )

def getLibraryDir():
    return pathlib.Path(
            os.environ.get(
                'BEMACS_EMACS_LIBRARY',
                app_dir / 'emacs_library' ) )

def getDocUserGuide():
    return pathlib.Path(
                os.environ.get(
                    'BEMACS_EMACS_DOCUMENTATION',
                    app_dir / 'documentation' / 'users-guide.html' ) )

def getLocalePath( app ):
    return app_dir / 'locale'

def getNullDevice():
    return 'NUL'

def setupPlatformSpecific_( argv0 ):
    global app_dir

    if argv0[1:3] ==':\\':
        app_dir = pathlib.Path( argv0 ).parent

    elif '\\' in argv0:
        app_dir = pathlib.Path( argv0 ).resolve().parent

    else:
        for folder in [os.getcwd()] + [p.strip() for p in os.environ.get( 'PATH', '' ).split( ';' )]:
            app_path = pathlib.Path( folder ) / argv0
            if app_path.exists():
                app_dir = app_path.parent
                break
