'''
 ====================================================================
 Copyright (c) 2003-2008 Barry A Scott.  All rights reserved.

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
	'bemacs.png',
        ]

def main( argv ):
    f = file( argv[1], 'w' )
    f.write( header )
    for filename in argv[2:]:
        f.write( 'images_by_filename["%s"] = (\n' % filename )
        i = file( filename, 'rb' )
        data = i.read()
        i.close()

        for offset in range( 0, len(data), data_slice ):
            f.write( '    %r\n' % data[offset:offset+data_slice] )
        f.write( '    )\n' )
    f.write( footer )
    f.close()

header = '''
import wx
import cStringIO

def getBitmap( name, size=None ):
    return wx.BitmapFromImage( getImage( name, size ) )

def getImage( name, size=None ):
    stream = cStringIO.StringIO( images_by_filename[ name ] )
    image = wx.ImageFromStream( stream )
    if size is not None:
        w, h = size
        if image.GetWidth() != w or image.GetHeight() != h:
            image.Rescale( w, h )
    return image

def getIcon( name, size=None ):
    icon = wx.EmptyIcon()
    icon.CopyFromBitmap( getBitmap( name, size ) )
    return icon

images_by_filename = {}
'''

footer = '''
'''

if __name__ == '__main__':
    sys.exit( main( argv ) )
