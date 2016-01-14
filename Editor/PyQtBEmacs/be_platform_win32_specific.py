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

SHGFP_TYPE_CURRENT = 0
SHGFP_TYPE_DEFAULT = 1

CSIDL_APPDATA = 0x001A
CSIDL_PROFILE = 0x0028

MAX_PATH = 260

app_dir = None

def getUserDir():
    app_folder = ctypes.create_unicode_buffer( MAX_PATH + 1 )
    ctypes.windll.shell32.SHGetFolderPathW( 0, CSIDL_PROFILE, None, SHGFP_TYPE_CURRENT, ctypes.byref( app_folder ) )
    user_dir = os.environ.get( 'BEMACS_USER', os.path.join( app_folder.value, 'bemacs' ) )
    return user_dir

def getLibraryDir():
    lib_dir = os.environ.get( 'BEMACS_EMACS_LIBRARY', os.path.join( app_dir, 'emacs_library' ) )
    return lib_dir

def getDocUserGuide():
    return os.path.join( app_dir, 'documentation/emacs-documentation.html' )

def getLocalePath( app ):
    return os.path.join( app_dir, 'locale' )

def getNullDevice():
    return 'NUL'

def setupPlatformSpecific_( argv0 ):
    global app_dir

    if argv0[1:3] ==':\\':
        app_dir = os.path.dirname( argv0 )

    elif '\\' in argv0:
            app_dir = os.path.dirname( os.path.abspath( argv0 ) )

    else:
        for folder in [os.getcwd()] + [p.strip() for p in os.environ.get( 'PATH', '' ).split( ';' )]:
            app_path = os.path.join( folder, argv0 )
            if os.path.exists( app_path ):
                app_dir = os.path.dirname( app_path )
                break

    print( 'QQQ setupPlatformSpecific_ app_dir = %r' % (app_dir,) )
