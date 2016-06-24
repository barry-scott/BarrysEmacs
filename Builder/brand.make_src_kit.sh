#!/bin/bash
KITNAME=bemacs-src-%(version)s
KITDIR=${TMPDIR:-/tmp}/${KITNAME}

rm -rf ${KITDIR}

svn export ${BUILDER_TOP_DIR} ${KITDIR}
cp ~/bin/builder_init ${KITDIR}/Builder
svnversion -c ${BUILDER_TOP_DIR} > ${KITDIR}/Builder/svn_version.dat

cd ${KITDIR}
cd Imports
tar xzf ${TMPDIR:-/tmp}/pycxx-${PYCXX_VER#*:}.tar.gz 

cd ${KITDIR}
cd ..
tar czf ${KITNAME}.tar.gz ${KITNAME}
