#!/bin/sh
PKGNAME=BarrysEmacs-%(version)s
rm -rf tmp
mkdir tmp
mkdir tmp/${PKGNAME}
svn export BarrysEmacs.app tmp/${PKGNAME}/${PKGNAME}.app
KITDIR=tmp/${PKGNAME}/${PKGNAME}.app/Contents/Resources/bemacs-kit
DOCDIR=tmp/${PKGNAME}/Documentation
cp bemacs-kit/* ${KITDIR}
mkdir -p ${DOCDIR}/Files
mv -f ${KITDIR}/*.css ${DOCDIR}/Files
mv -f ${KITDIR}/*.htm ${DOCDIR}/Files
mv -f ${KITDIR}/*.js  ${DOCDIR}/Files
mv -f ${KITDIR}/*.gif ${DOCDIR}/Files

pushd ${DOCDIR}
ln -s Files/emacs-documentation.htm .
ln -s Files/bemacs-faq.htm .
popd

hdiutil create -srcfolder tmp/${PKGNAME} tmp/tmp.dmg
hdiutil convert tmp/tmp.dmg -format UDZO -imagekey zlib-level=9 -o tmp/${PKGNAME}.dmg
