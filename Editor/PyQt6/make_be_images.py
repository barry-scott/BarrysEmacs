'''
 ====================================================================
 Copyright (c) 2003-2015 Barry A Scott.  All rights reserved.

 This software is licensed as described in the file LICENSE.txt,
 which you should have received as part of this distribution.

 ====================================================================

    make_be_images.py

'''
import sys

data_slice = 32

argv = [
    sys.argv[0],
    'be_images.py',
    'toolbar_images/editcopy.png',
    'toolbar_images/editcut.png',
    'toolbar_images/editpaste.png',
    'toolbar_images/close-curly-brace.png',
    'toolbar_images/editcopy.png',
    'toolbar_images/editcut.png',
    'toolbar_images/editpaste.png',
    'toolbar_images/edit.png',
    'toolbar_images/justify.png',
    'toolbar_images/open-curly-brace.png',
    'toolbar_images/stop.png',
    'toolbar_images/synchronize.png',
    'toolbar_images/build_compile.png',
    'toolbar_images/build_next_error.png',
    'toolbar_images/build_prev_error.png',
    'toolbar_images/case_capitalise.png',
    'toolbar_images/case_invert.png',
    'toolbar_images/case_lower.png',
    'toolbar_images/case_upper.png',
    'toolbar_images/edit_copy.png',
    'toolbar_images/edit_cut.png',
    'toolbar_images/edit_delete.png',
    'toolbar_images/edit_paste.png',
    'toolbar_images/file_open.png',
    'toolbar_images/file_print.png',
    'toolbar_images/file_save_all.png',
    'toolbar_images/file_save.png',
    'toolbar_images/macro_play.png',
    'toolbar_images/macro_record.png',
    'toolbar_images/macro_stop.png',
    'toolbar_images/region_indent.png',
    'toolbar_images/region_undent.png',
    'toolbar_images/search_find.png',
    'toolbar_images/search_fold_case.png',
    'toolbar_images/tools_grep.png',
    'toolbar_images/view_white_space.png',
    'toolbar_images/view_wrap_long.png',
    'toolbar_images/window_cascade.png',
    'toolbar_images/window_del_other.png',
    'toolbar_images/window_del_this.png',
    'toolbar_images/window_split_horiz.png',
    'toolbar_images/window_split_vert.png',
    'toolbar_images/window_tile_horiz.png',
    'toolbar_images/window_tile_vert.png',
    'org.barrys-emacs.editor.png',
    ]

def main( argv ):
    f = open( argv[1], 'w' )
    f.write( header )
    for filename in argv[2:]:
        f.write( 'images_by_filename["%s"] = (\n' % (filename,) )
        i = open( filename, 'rb' )
        data = i.read()
        i.close()

        for offset in range( 0, len(data), data_slice ):
            f.write( '    %r\n' % (data[offset:offset+data_slice],) )
        f.write( '    )\n' )
    f.write( footer )
    f.close()

header = '''
from PyQt6 import QtGui
import io

def getPixmap( name, size=None ):
    return pixmap.BitmapFromImage( getImage( name, size ) )

def getPixmap( name, size=None ):
    pixmap = QtGui.QPixmap()
    pixmap.loadFromData( images_by_filename[ name ] )
    return pixmap

def getIcon( name, size=None ):
    return QtGui.QIcon( getPixmap( name, size ) )

images_by_filename = {}
'''

footer = '''
'''

if __name__ == '__main__':
    sys.exit( main( argv ) )
