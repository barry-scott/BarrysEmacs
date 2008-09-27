'''
 ====================================================================
 Copyright (c) 2008 Barry A Scott.  All rights reserved.

 This software is licensed as described in the file LICENSE.txt,
 which you should have received as part of this distribution.

 ====================================================================

    make_be_version.py

'''
import sys
sys.path.append( '../Builder' )
import be_brand_version

argv = [
        sys.argv[0],
        '../Builder/version.info',
        'be_version.py.template',
        ]

if __name__ == '__main__':
    sys.exit( be_brand_version.main( argv ) )
