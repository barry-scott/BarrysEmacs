#
#   package_macos_dmg_settings.py
#
#   settings file imported into dmgbuild to create the bemacs DMG file.
#
import os
import sys
import plistlib

sys.path.insert( 0, os.getcwd() )

import build_log
log = build_log.BuildLog()
log.setColour( True )

log.info( 'Change to org.barrys-emacs.bemacs' )

PKGNAME = 'dmg'

app_path = os.path.join( 'tmp', PKGNAME, "Barry's Emacs.app" )
app_name = os.path.basename( app_path )

path = os.path.join( app_path, 'Contents', 'Info.plist' )
with open( path, 'r' ) as f:
    log.info( 'Reading %s' % (path,) )
    __text = f.read()

__text = __text.replace( '<string>org.barrys-emacs.bemacs-devel</string>',
                         '<string>org.barrys-emacs.bemacs</string>' )
__text = __text.replace( 'Emacs-Devel', 'Emacs' )

path = os.path.join( app_path, 'Contents', 'Info.plist' )
with open( path, 'w' ) as f:
    log.info( 'Writing %s' % (path,) )
    f.write( __text )

client_name = 'bemacs'
client_path = os.path.join( 'tmp', PKGNAME, client_name )


for tool_name, tool_path in [(client_name, client_path)]:
    __text = '''#!/bin/bash
__APP_FOLDER=$(osascript -e  'tell application "Finder" to POSIX path of (get application file id "org.barrys-emacs.bemacs" as alias)')
exec "$(__APP_FOLDER)/Contents/Resources/bin/%s" "$@"
    ''' % (tool_name,)

    tool_path = os.path.join( 'tmp', PKGNAME, tool_name )

    with open( tool_path, 'w' )  as f:
        log.info( 'Writing %s' % (tool_path,) )
        f.write( __text )

    log.info( 'chmod 555 %s' % (tool_path,) )
    os.chmod( tool_path, 0o555 )


# .. Useful stuff ..............................................................

def __iconFromApp( app_path ):
    plist_path = os.path.join( app_path, 'Contents', 'Info.plist' )
    with open( plist_path, 'rb' ) as f:
        plist = plistlib.load( f )
    icon_name = plist[ 'CFBundleIconFile' ]
    icon_root, icon_ext = os.path.splitext( icon_name )
    if not icon_ext:
        icon_ext = '.icns'
    icon_name = icon_root + icon_ext
    return os.path.join( app_path, 'Contents', 'Resources', icon_name )

# .. Basics ....................................................................

# Volume format (see hdiutil create -help)
format = 'UDZO'

# Volume size (must be large enough for your files)
size = '400M'

# Files to include
files = [app_path
        ,client_path
        ,'../Kits/readme.txt'
        ,'../Kits/readme-macos.txt']

# Symlinks to create
symlinks = {
    'Applications': '/Applications',
    }

# Volume icon
#
# You can either define icon, in which case that icon file will be copied to the
# image, *or* you can define badge_icon, in which case the icon file you specify
# will be used to badge the system's Removable Disk icon
#
#icon = '/path/to/icon.icns'
badge_icon = __iconFromApp( app_path )

# Where to put the icons
icon_locations = {
    app_name:           (140, 120),
    'Applications':     (500, 120),

    'readme.txt':       (100, 320),
    'readme-macos.txt': (230, 350),
    client_name:        (380, 320),
    }

# .. Window configuration ......................................................

# Background
#
# This is a STRING containing any of the following:
#
#    #3344ff          - web-style RGB color
#    #34f             - web-style RGB color, short form (#34f == #3344ff)
#    rgb(1,0,0)       - RGB color, each value is between 0 and 1
#    hsl(120,1,.5)    - HSL (hue saturation lightness) color
#    hwb(300,0,0)     - HWB (hue whiteness blackness) color
#    cmyk(0,1,0,0)    - CMYK color
#    goldenrod        - X11/SVG named color
#    builtin-arrow    - A simple built-in background with a blue arrow
#    /foo/bar/baz.png - The path to an image file
#
# The hue component in hsl() and hwb() may include a unit; it defaults to
# degrees ('deg'), but also supports radians ('rad') and gradians ('grad'
# or 'gon').
#
# Other color components may be expressed either in the range 0 to 1, or
# as percentages (e.g. 60% is equivalent to 0.6).
background = 'builtin-arrow'

show_status_bar = False
show_tab_view = False
show_toolbar = False
show_pathbar = False
show_sidebar = False
sidebar_width = 180

# Window position in ((x, y), (w, h)) format
window_rect = ((100, 100), (640, 512))

# Select the default view; must be one of
#
#    'icon-view'
#    'list-view'
#    'column-view'
#    'coverflow'
#
default_view = 'icon-view'

# General view configuration
show_icon_preview = False

# Set these to True to force inclusion of icon/list view settings (otherwise
# we only include settings for the default view)
include_icon_view_settings = 'auto'
include_list_view_settings = 'auto'

# .. Icon view configuration ...................................................

arrange_by = None
grid_offset = (0, 0)
grid_spacing = 100
scroll_position = (0, 0)
label_pos = 'bottom' # or 'right'
__text_size = 16
icon_size = 128

# .. List view configuration ...................................................

# Column names are as follows:
#
#   name
#   date-modified
#   date-created
#   date-added
#   date-last-opened
#   size
#   kind
#   label
#   version
#   comments
#
list_icon_size = 16
list___text_size = 12
list_scroll_position = (0, 0)
list_sort_by = 'name'
list_use_relative_dates = True
list_calculate_all_sizes = False,
list_columns = ('name', 'date-modified', 'size', 'kind', 'date-added')
list_column_widths = {
    'name': 300,
    'date-modified': 181,
    'date-created': 181,
    'date-added': 181,
    'date-last-opened': 181,
    'size': 97,
    'kind': 115,
    'label': 100,
    'version': 75,
    'comments': 300,
    }
list_column_sort_directions = {
    'name': 'ascending',
    'date-modified': 'descending',
    'date-created': 'descending',
    'date-added': 'descending',
    'date-last-opened': 'descending',
    'size': 'descending',
    'kind': 'ascending',
    'label': 'ascending',
    'version': 'ascending',
    'comments': 'ascending',
    }
