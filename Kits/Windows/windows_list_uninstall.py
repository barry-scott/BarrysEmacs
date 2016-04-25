#!/usr/bin/env python3
import winreg

for title, access in [
    ('32bit', winreg.KEY_WOW64_32KEY),
    ('64bit', winreg.KEY_WOW64_64KEY)]:
    with winreg.OpenKey( winreg.HKEY_LOCAL_MACHINE, r'SOFTWARE\Microsoft\Windows\CurrentVersion\Uninstall', access=winreg.KEY_READ|access ) as key:
        index = 0
        while True:
            try:
                name = winreg.EnumKey( key, index )
                index += 1
            except OSError:
                break

            if 'barry' in name.lower():
                print( title, name )

                with winreg.OpenKey( key, name ) as bkey:
                    bindex = 0
                    while True:

                        try:
                            name, value, type_ = winreg.EnumValue( bkey, bindex )
                            bindex += 1
                        except OSError:
                            break

                        print( '   %s: %r' % (name, value) )
