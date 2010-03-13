#!/bin/sh
set -e
PKGNAME=BarrysEmacs-%(version)s
rm -rf tmp
mkdir tmp
mkdir tmp/${PKGNAME}

echo "Info: copy app"
cp -r "pkg/Barry's Emacs.app" "tmp/${PKGNAME}/Barry's Emacs.app"

echo "Info: change to org.barrys-emacs.bemacs"
python <<EOF
f = open( "tmp/${PKGNAME}/Barry's Emacs.app/Contents/Info.plist", 'r' )
text = f.read()
f.close()

text = text.replace( '<string>org.barrys-emacs.bemacs-devel</string>', '<string>org.barrys-emacs.bemacs</string>' )
f = open( "tmp/${PKGNAME}/Barry's Emacs.app/Contents/Info.plist", 'w' )
f.write( text )
f.close()


f = open( "${BUILDER_TOP_DIR}/Editor/PythonBemacs/be_client.py", 'r' )
text = f.read()
f.close()

text = text.replace( 'org.barrys-emacs.bemacs-devel', 'org.barrys-emacs.bemacs' )
f = open( "tmp/${PKGNAME}/bemacs", 'w' )
f.write( text )
f.close()
EOF

chmod +x "tmp/${PKGNAME}/bemacs"

echo "Info: Create DMG"
hdiutil create -srcfolder tmp/${PKGNAME} tmp/tmp.dmg
hdiutil convert tmp/tmp.dmg -format UDZO -imagekey zlib-level=9 -o tmp/${PKGNAME}.dmg
