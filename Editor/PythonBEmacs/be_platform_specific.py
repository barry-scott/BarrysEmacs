'''
 ====================================================================
 Copyright (c) 2003-2008 Barry A Scott.  All rights reserved.

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
    return os.path.join( getUserDir(), 'Bemacs.xml' )

def getLogFilename():
    return os.environ.get( 'BEMACS_GUI_LOG', os.path.join( getUserDir(), 'Bemacs.log' ) )

def setupPlatform( argv0 ):
    setupPlatformSpecific_( argv0 )

    user_dir = getUserDir()
    if not os.path.exists( user_dir ):
        os.makedirs( user_dir )
