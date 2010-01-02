'''
 ====================================================================
 Copyright (c) 2008 Barry A Scott.  All rights reserved.

 This software is licensed as described in the file LICENSE.txt,
 which you should have received as part of this distribution.

 ====================================================================


    be_platform_win32_specific.py

'''
from win32com.shell import shell, shellcon
import os

SHGFP_TYPE_CURRENT = 0
SHGFP_TYPE_DEFAULT = 1

def getApplicationDir():
    app_folder = shell.SHGetFolderPath( 0, shellcon.CSIDL_APPDATA,
                0, SHGFP_TYPE_CURRENT )
    return os.path.join( app_folder, 'Bemacs' )

def getLocalePath( app ):
    return os.path.join( app.app_dir, 'locale' )

def getNullDevice():
    return 'NUL'
