#!/bin/bash
mkdir -p tree/DEBIAN

cat <<EOF >tree/DEBIAN/control
Package: bemacs
Version: %(wc_state)s%(maturity)s%(major)s.%(minor)s.%(revision)s
Maintainer: Barry Scott <barry@barrys-emacs.org>
Description: Barry's Emacs Text editor
 Barry's Emacs Text editor
 Home page http://www.barrys-emacs.org
Architecture: i386
Depends: libmotif3
EOF

cat <<EOF >tree/DEBIAN/postinst
#!/bin/bash
cd /usr/local/bin
ln -s ../bemacs/bemacs bemacs
cd /usr/local/bemacs
ln -s emacs-xfree86.xkeys emacs.xkeys
EOF
chmod +x tree/DEBIAN/postinst

cat <<EOF >tree/DEBIAN/prerm
#!/bin/bash
cd /usr/local/bin
rm -f bemacs
cd /usr/local/bemacs
rm -f emacs.xkeys
EOF
chmod +x tree/DEBIAN/prerm

dpkg-deb -b tree ./
