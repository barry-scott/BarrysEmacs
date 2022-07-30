# -*- mode: python ; coding: utf-8 -*-

import PyInstaller.utils.osx as osxutils

def sign_binary(filename, identity=None, entitlements_file=None, deep=False):
    print('sign_binary(%%s) prevented from running' %% (filename,))
    return

osxutils.sign_binary = sign_binary

app_name = "Barry's Emacs"

block_cipher = None

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

a = Analysis(
        ['be_main.py'],
        pathex=['/Users/barry/Projects/BarrysEmacs/Editor/exe-pybemacs'],
        binaries=[],
        datas=[],
        hiddenimports=[],
        hookspath=[],
        hooksconfig={},
        runtime_hooks=[],
        excludes=[],
        win_no_prefer_redirects=False,
        win_private_assemblies=False,
        cipher=block_cipher,
        noarchive=False
        )

pyz = PYZ(
        a.pure,
        a.zipped_data,
        cipher=block_cipher
        )

exe = EXE(
        pyz,
        a.scripts,
        [],
        exclude_binaries=True,
        name=app_name,
        debug=False,
        bootloader_ignore_signals=False,
        strip=False,
        upx=True,
        console=False,
        disable_windowed_traceback=False,
        target_arch='universal2',
        codesign_identity=None,
        entitlements_file=None,
        icon='bemacs.icns'
        )

coll = COLLECT(
        exe,
        a.binaries,
        a.zipfiles,
        a.datas,
        strip=False,
        upx=False,
        upx_exclude=[],
        name=app_name
        )

app = BUNDLE(
        coll,
        name="%%s.app" %% (app_name,),
        icon='bemacs.icns',
        bundle_identifier='org.barrys-emacs.bemacs',
        info_plist=dict(
            CFBundleDocumentTypes=
                [dict(  CFBundleTypeExtensions=bemacs_doc_types,
                        CFBundleTypeName="%%s Document" %% (app_name,),
                        CFBundleTypeRole="Editor")
                ,dict(  CFBundleTypeExtensions=editor_doc_types,
                        CFBundleTypeName="Document",
                        CFBundleTypeRole="Editor"),],
            CFBundleIdentifier='org.barrys-emacs.bemacs',
            CFBundleName=app_name,
            CFBundleVersion='%(mac_short_version)s',
            CFBundleShortVersionString='%(mac_short_version)s',
            CFBundleGetInfoString="%%s %(version)s, © 1981-%(year)s Barry A. Scott. All Rights Reserved." %% (app_name,),
            # claim we know about dark mode
            NSRequiresAquaSystemAppearance=False,
            )
        )
