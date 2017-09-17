'''
 ====================================================================
 Copyright (c) 2003-2015 Barry A Scott.  All rights reserved.

 This software is licensed as described in the file LICENSE.txt,
 which you should have received as part of this distribution.

 ====================================================================

    be_platform_specific.py

'''
import sys

if sys.platform == 'win32':
    from be_platform_win32_specific import *

elif sys.platform == 'darwin':
    from be_platform_macosx_specific import *

else:
    from be_platform_unix_specific import *

def getPreferencesFilename():
    return getUserDir() / 'bemacs.xml'

def getSessionFilename():
    return getUserDir() / 'bemacs-session.xml'

def getLogFilename():
    return pathlib.Path( os.environ.get( 'BEMACS_GUI_LOG', getUserDir() / 'bemacs.log' ) )

def setupPlatform( argv0 ):
    setupPlatformSpecific_( argv0 )

    user_dir = getUserDir()
    if not user_dir.exists():
        user_dir.mkdir( parents=True )
