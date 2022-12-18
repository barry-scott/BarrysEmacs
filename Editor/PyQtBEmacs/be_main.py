#!/usr/bin/python3
'''
 ====================================================================
 Copyright (c) 2008-2015 Barry A Scott.  All rights reserved.

 This software is licensed as described in the file LICENSE.txt,
 which you should have received as part of this distribution.

 ====================================================================

    be_main.py

'''
import sys
import os
import locale

def prerequesitChecks():
    return 1

def main( argv ):

    #
    #   set a working STDOUT before loading most modules
    #
    # help debug when stdout goes nowhere useful
    # Mac OS X and Windows are the main problems
    if sys.platform == 'darwin':
        if '--noredirect' not in argv:
            sys.stdout = open( os.environ.get( 'BEMACS_STDOUT_LOG', '/dev/null' ), 'w', 1 )
            sys.stderr = sys.stdout

    elif sys.platform.startswith( 'win' ):
        if '--noredirect' not in argv:
            sys.stdout = open( os.environ.get( 'BEMACS_STDOUT_LOG', 'nul' ), 'w' )
            sys.stderr = sys.stdout

    # don't pollute any subprocesses with env vars
    # from packaging processing
    for envvar in ['PYTHONPATH', 'PYTHONHOME', 'PYTHONEXECUTABLE']:
        if envvar in os.environ:
            del os.environ[ envvar ]

    if sys.platform.startswith( 'win' ):
        # in windows when building from a venv its necessary to 
        # use add_dll_directory so that packaging will work

        # also at run time it is also necessary to add
        # these folders
        import PyQt6
        PyQt6_dir = os.path.dirname( PyQt6.__file__ )
        for folder in (PyQt6_dir, os.path.join( PyQt6_dir, 'Qt6', 'bin' )):
            os.add_dll_directory( folder )

    # Create the win application and start its message loop
    import be_app
    app = be_app.BemacsApp( argv )

    if not prerequesitChecks():
        return 1

    app.main_window.show()
    return app.exec()

if __name__ == '__main__':
    sys.exit( main( sys.argv ) )
