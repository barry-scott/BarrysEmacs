#!/bin/bash
V=%(major)s.%(minor)s.%(patch)s
KITNAME=bemacs
KIT_BASENAME=${KITNAME}-${V}

rm -rf tmp
mkdir -p tmp

MOCK_VERSION_NAME=${1? mock cfg name}
MOCK_ROOT=$( sudo mock --root=${MOCK_VERSION_NAME} -p )
if [ ! -e "${MOCK_ROOT}" ]
then
    echo "Info: Init mock for ${MOCK_VERSION_NAME}"
    sudo \
         mock \
            --root=${MOCK_VERSION_NAME} \
            --init
fi

../bemacs_make_spec_file.py cli ${V} tmp/bemacs.spec

sudo \
    mock \
        --root=${MOCK_VERSION_NAME} \
        --buildsrpm \
        --spec tmp/bemacs.spec \
        --sources ${KIT_BASENAME}.tar.gz

DISTRO=el6

MOCK_BUILD_DIR=${MOCK_ROOT}/builddir/build
sudo ls -l ${MOCK_BUILD_DIR}/SRPMS

SRPM_BASENAME="${KIT_BASENAME}-1.${DISTRO}"

sudo cp -v "${MOCK_BUILD_DIR}/SRPMS/${SRPM_BASENAME}.src.rpm" tmp

echo "Info: Creating RPM"
sudo \
    mock \
        --root=${MOCK_VERSION_NAME} \
        --rebuild --dnf \
            "tmp/${SRPM_BASENAME}.src.rpm"

sudo ls -l ${MOCK_BUILD_DIR}/RPMS

ALL_BIN_KITNAMES="
${KITNAME}
${KITNAME}-cli
${KITNAME}-common
${KITNAME}-debuginfo"

for BIN_KITNAME in ${ALL_BIN_KITNAMES}
do
    sudo cp -v "${MOCK_BUILD_DIR}/RPMS/${BIN_KITNAME}-${V}-1.${DISTRO}.x86_64.rpm" tmp
done
sudo chown ${USER}: tmp/*.rpm
