'''
 ====================================================================
 Copyright (c) 2008 Barry A Scott.  All rights reserved.

 This software is licensed as described in the file LICENSE.txt,
 which you should have received as part of this distribution.

 ====================================================================


    be_platform_macosx_specific.py

'''
import os
import sys

def getApplicationDir():
    return os.path.join( os.environ['HOME'], 'Library/Preferences/org.barrys-emacs.bemacs' )

def getLocalePath( app ):
    return os.path.join( app.app_dir, 'locale' )

def getNullDevice():
    return '/dev/null'
