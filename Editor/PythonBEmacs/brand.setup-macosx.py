# -*- coding: utf-8 -*-

#
#   brand.setup-macosx.py
#

from setuptools import setup

setup(
    app =           ['be_main.py'],
    data_files =    [],
    options =       {'py2app':
                        {
                        'argv_emulation':
                            True,
                        'iconfile':
                            'bemacs.icns',
                        'plist':
                            dict(
                                CFBundleDocumentTypes=
                                    [dict(  CFBundleTypeExtensions=["qqq"],
                                            CFBundleTypeName="Text Document",
                                            CFBundleTypeRole="Editor")
                                    ,dict(  CFBundleTypeExtensions=["txt"],
                                            CFBundleTypeName="Text Document",
                                            CFBundleTypeRole="Editor"),],
                                CFBundleIdentifier='org.barrys-emacs.bemacs',
                                CFBundleName="Barry's Emacs",
                                CFBundleVersion='%(mac_short_version)s',
                                CFBundleShortVersionString='%(mac_short_version)s',
                                CFBundleGetInfoString=u"Barry's Emacs %(version)s, © 2000-%(year)s Barry A. Scott. All Rights Reserved.",
                                ),
                        }},
    setup_requires =['py2app'],
)
