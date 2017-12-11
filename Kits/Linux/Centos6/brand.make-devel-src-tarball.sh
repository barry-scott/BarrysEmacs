#!/bin/bash
#
#   make-devel-src-rpm.sh for Centos 6 CLI only
#
set -e

GIT_TREEISH=${1:-master}

echo "Info: Creating source tarball from ${GIT_TREEISH}"

KITNAME=bemacs

V=%(major)s.%(minor)s.%(patch)s
rm -rf ${TMPDIR:-/tmp}/${NAME}-${V}

KIT_BASENAME=${KITNAME}-${V}

rm -rf tmp
mkdir -p tmp
pushd tmp
echo "Info: Exporting source code"

(cd ${BUILDER_TOP_DIR}; git archive --format=tar --prefix=${KIT_BASENAME}/ ${GIT_TREEISH}) | tar xf -

git show-ref --head --hash head >${KIT_BASENAME}/Builder/commit_id.txt

# xml-preferences is not packaged on Fedora
XML_PREF_PATH=$( ${PYTHON} -c "import xml_preferences, os.path; print( os.path.dirname( xml_preferences.__file__ ) )" )
mkdir ${KIT_BASENAME}/Editor/PyQtBEmacs/xml_preferences
cp  ${XML_PREF_PATH}/*.py ${KIT_BASENAME}/Editor/PyQtBEmacs/xml_preferences

tar czf ${KIT_BASENAME}.tar.gz ${KIT_BASENAME}

# clean up the tree of files
rm -rf ${KIT_BASENAME}
popd

echo "Info: creating bemacs.spec"
python3 ../bemacs_spec_set_version.py ${V}
mv bemacs.spec tmp

cp bemacs-mock-build.sh tmp

echo "Info: Creating asserts for ${KIT_BASENAME}"
ls -l $PWD/tmp
