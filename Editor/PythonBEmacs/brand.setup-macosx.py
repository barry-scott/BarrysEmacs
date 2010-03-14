# -*- coding: utf-8 -*-

#
#   brand.setup-macosx.py
#

from setuptools import setup


bemacs_doc_types = ['ml', 'mlp']

editor_doc_types = [
    'txt',
    'c', 'h', 'cxx', 'hxx', 'cpp', 'hpp',
    'py',
    'html', 'htm', 'css', 'js',
    'java',
    'mak',
    'sh',
    'cmd', 'bat',
    ]

setup(
    app =
        ['be_main.py'],
    data_files =
        [],
    options =       
        {'py2app':
            {
            'argv_emulation':
                True,
            'iconfile':
                'bemacs.icns',
            'plist':
                dict(
                    CFBundleDocumentTypes=
                        [dict(  CFBundleTypeExtensions=bemacs_doc_types,
                                CFBundleTypeName="Barry's Emacs-Devel Document",
                                CFBundleTypeRole="Editor")
                        ,dict(  CFBundleTypeExtensions=editor_doc_types,
                                CFBundleTypeName="Document",
                                CFBundleTypeRole="Editor"),],
                    CFBundleIdentifier='org.barrys-emacs.bemacs-devel',
                    CFBundleName="Barry's Emacs-Devel",
                    CFBundleVersion='%(mac_short_version)s',
                    CFBundleShortVersionString='%(mac_short_version)s',
                    CFBundleGetInfoString=u"Barry's Emacs-Devel %(version)s, © 1981-%(year)s Barry A. Scott. All Rights Reserved.",
                    ),
            }},
    setup_requires =
        ['py2app'],
)
