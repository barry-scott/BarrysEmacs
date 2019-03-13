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

tar czf ${KIT_BASENAME}.tar.gz ${KIT_BASENAME}

# clean up the tree of files
rm -rf ${KIT_BASENAME}
popd

echo "Info: creating bemacs.spec"
python3 ../bemacs_make_spec_file.py cli ${V} tmp/bemacs.spec
cp bemacs-mock-build.sh tmp

echo "Info: Creating asserts for ${KIT_BASENAME} in $PWD/tmp"
ls -l $PWD/tmp
