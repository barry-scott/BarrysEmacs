'''
 ====================================================================
 Copyright (c) 2003-2008 Barry A Scott.  All rights reserved.

 This software is licensed as described in the file LICENSE.txt,
 which you should have received as part of this distribution.

 ====================================================================

    be_frame.py

    Based on code from pysvn WorkBench

'''
import wx

# point size and face need to choosen for platform
if wx.Platform == '__WXMSW__':
    face = 'Courier New'
    point_size = 8

elif wx.Platform == '__WXMAC__':
    face = 'Monaco'
    point_size = 12

else:
    face = 'Courier'
    point_size = 12
