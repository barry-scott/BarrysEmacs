#!/bin/bash
#
#   make-devel-src-rpm.sh
#
set -e

CMD="$1"

echo "Info: Creating source tarball"

KITNAME=bemacs

V=%(major)s.%(minor)s.%(patch)s
rm -rf /tmp/${NAME}-${V}

KIT_BASENAME=${KITNAME}-${V}

rm -rf tmp
mkdir -p tmp
pushd tmp
echo "Info: Exporting source code"
svn export --quiet ${BUILDER_TOP_DIR} ${KIT_BASENAME}
svnversion ${BUILDER_TOP_DIR} >${KIT_BASENAME}/Builder/svn_version_override.dat

pushd ${KIT_BASENAME}/Imports
svn export --quiet https://svn.code.sf.net/p/cxx/code/tags/${PYCXX_VER} pycxx-${PYCXX_VER}
popd

tar czf ${KIT_BASENAME}.tar.gz ${KIT_BASENAME}
popd

echo "Info: creating bemacs.spec"
python3 bemacs_spec_set_version.py ${V}

echo "Info: Creating SRPM for ${KIT_BASENAME}"

sudo \
    mock \
        --buildsrpm --dnf \
        --spec bemacs.spec \
        --sources tmp/${KIT_BASENAME}.tar.gz

MOCK_ROOT=$( sudo mock -p )
MOCK_BUILD_DIR=${MOCK_ROOT}/builddir/build
ls -l ${MOCK_BUILD_DIR}/SRPMS

set $(tr : ' ' </etc/system-release-cpe)
case $4 in
fedora)
    DISTRO=fc$5
    ;;
*)
    echo "Error: need support for distro $4"
    exit 1
    ;;
esac

SRPM_BASENAME="${KIT_BASENAME}-1.${DISTRO}"

cp -v "${MOCK_BUILD_DIR}/SRPMS/${SRPM_BASENAME}.src.rpm" tmp

echo "Info: Creating RPM"
sudo \
    mock \
        --rebuild --dnf \
            "tmp/${SRPM_BASENAME}.src.rpm"

ls -l ${MOCK_BUILD_DIR}/RPMS

cp -v "${MOCK_BUILD_DIR}/RPMS/${SRPM_BASENAME}.x86_64.rpm" tmp

echo "Info: Results in ${PWD}/tmp:"
ls -l tmp

if [ "$CMD" = "--install" ]
then
    echo "Info: Installing RPM"
    sudo dnf -y remove ${KITNAME}
    sudo dnf -y install "tmp/${SRPM_BASENAME}.noarch.rpm"
fi
