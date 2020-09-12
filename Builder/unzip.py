#!/usr/bin/env python3
import sys
import zipfile

def main( argv ):
    with zipfile.ZipFile( argv[1], 'r' ) as t:
        t.extractall( argv[2] )

    return 0

if __name__ == '__main__':
    sys.exit( main( sys.argv ) )
