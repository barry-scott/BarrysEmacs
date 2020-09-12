#!/usr/bin/env python3
import sys
import tarfile

def main( argv ):
    with tarfile.open( argv[1], 'r:*' ) as t:
        t.extractall( argv[2] )

    return 0

if __name__ == '__main__':
    sys.exit( main( sys.argv ) )
