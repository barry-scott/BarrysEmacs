#!/bin/bash
#
#   make-devel-src-rpm.sh
#
set -e

CMD="$1"

if [ "$2" != "" ]
then
    VERSION_ID=$2
else
    . /etc/os-release
fi

if [ "$3" != "" ]
then
    ARCH=$3
else
    ARCH=$( uname -m )
fi

MOCK_VERSION_NAME=fedora-${VERSION_ID}-${ARCH}
MOCK_ROOT=$( sudo mock --root=${MOCK_VERSION_NAME} -p )

if [ ! -e "${MOCK_ROOT}" ]
then
    echo "Info: Init mock for ${MOCK_VERSION_NAME}"
    sudo \
         mock \
            --root=${MOCK_VERSION_NAME} \
            --init
fi

echo "Info: Creating source tarball"

KITNAME=bemacs

V=%(major)s.%(minor)s.%(patch)s
rm -rf ${TMPDIR:-/tmp}/${NAME}-${V}

KIT_BASENAME=${KITNAME}-${V}

rm -rf tmp
mkdir -p tmp
pushd tmp
echo "Info: Exporting source code"

(cd ${BUILDER_TOP_DIR}; git archive --format=tar --prefix=${KIT_BASENAME}/ master) | tar xf -

pushd ${KIT_BASENAME}/Imports
case "${PYCXX_VER%%:*}" in
trunk)
    svn export --quiet https://svn.code.sf.net/p/cxx/code/trunk/CXX pycxx-${PYCXX_VER#*:}
    ;;

tag)
    svn export --quiet https://svn.code.sf.net/p/cxx/code/tags/${PYCXX_VER#*:} pycxx-${PYCXX_VER#*:}
    ;;

*)
    echo "Error: PYCXX_VER unknown value ${PYCXX_VER}"
    exit 1
    ;;
esac
popd

git show-ref --head --hash head >${KIT_BASENAME}/Builder/commit_id.txt

# xml-preferences is not packaged on Fedora
XML_PREF_PATH=$( ${PYTHON} -c "import xml_preferences, os.path; print( os.path.dirname( xml_preferences.__file__ ) )" )
mkdir ${KIT_BASENAME}/Editor/PyQtBEmacs/xml_preferences
cp  ${XML_PREF_PATH}/*.py ${KIT_BASENAME}/Editor/PyQtBEmacs/xml_preferences

tar czf ${KIT_BASENAME}.tar.gz ${KIT_BASENAME}
popd

echo "Info: creating bemacs.spec"
python3 bemacs_spec_set_version.py ${V}

echo "Info: Creating SRPM for ${KIT_BASENAME}"

sudo \
    mock \
        --root=${MOCK_VERSION_NAME} \
        --buildsrpm --dnf \
        --spec bemacs.spec \
        --sources tmp/${KIT_BASENAME}.tar.gz

MOCK_BUILD_DIR=${MOCK_ROOT}/builddir/build
sudo ls -l ${MOCK_BUILD_DIR}/SRPMS

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

sudo cp -v "${MOCK_BUILD_DIR}/SRPMS/${SRPM_BASENAME}.src.rpm" tmp

echo "Info: Creating RPM"
sudo \
    mock \
        --root=${MOCK_VERSION_NAME} \
        --rebuild --dnf \
            "tmp/${SRPM_BASENAME}.src.rpm"

sudo ls -l ${MOCK_BUILD_DIR}/RPMS

sudo cp -v "${MOCK_BUILD_DIR}/RPMS/${SRPM_BASENAME}.x86_64.rpm" tmp
sudo chown ${USER}: tmp/*.rpm

echo "Info: Results in ${PWD}/tmp:"
ls -l tmp

if [ "$CMD" = "--install" ]
then
    echo "Info: Installing RPM"
    sudo dnf -y remove ${KITNAME}
    sudo dnf -y install "tmp/${SRPM_BASENAME}.x86_64.rpm"
fi
