#!/usr/bin/env python3
#
#   make_kit_files_list.py
#
import sys
import pathlib
import os

kitfiles_folder = pathlib.Path( sys.argv[1] )

all_files = []

for dirpath, all_dirnames, all_filenames in os.walk( str( kitfiles_folder ) ):
    for filename in all_filenames:
        filepath = pathlib.Path( dirpath ) / filename
        filepath = filepath.relative_to( kitfiles_folder )
        if filepath.suffix != '.pyc':
            all_files.append( filepath )

for filepath in sorted( all_files ):
    src_path = kitfiles_folder / filepath
    dst_path = pathlib.Path( '{app}' ) / filepath
    print( 'Source: "%s"; DestDir: "%s"' % (src_path, dst_path.parent) )
