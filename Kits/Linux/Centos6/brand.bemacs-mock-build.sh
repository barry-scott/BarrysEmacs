#!/bin/bash
V=%(major)s.%(minor)s.%(patch)s
KITNAME=bemacs
KIT_BASENAME=${KITNAME}-${V}

# assumes that tmp folder exists
if [ ! -d tmp ]
then
    echo "Error: tmp is not setup"
    exit 1
fi

MOCK_VERSION_NAME=${1? mock cfg name}
MOCK_ROOT=$( mock --root=${MOCK_VERSION_NAME} -p )
if [ ! -e "${MOCK_ROOT}" ]
then
    echo "Info: Init mock for ${MOCK_VERSION_NAME}"
     mock \
        --root=${MOCK_VERSION_NAME} \
        --init
fi

${PYTHON} ../bemacs_make_spec_file.py cli ${V} tmp/bemacs.spec

mock \
    --root=${MOCK_VERSION_NAME} \
    --buildsrpm \
    --spec tmp/bemacs.spec \
    --sources tmp/${KIT_BASENAME}.tar.gz

DISTRO=el6

MOCK_BUILD_DIR=${MOCK_ROOT}/builddir/build
ls -l ${MOCK_BUILD_DIR}/SRPMS

SRPM_BASENAME="${KIT_BASENAME}-1.${DISTRO}"

cp -v "${MOCK_BUILD_DIR}/SRPMS/${SRPM_BASENAME}.src.rpm" tmp

echo "Info: Creating RPM"
mock \
    --root=${MOCK_VERSION_NAME} \
    --rebuild --dnf \
        "tmp/${SRPM_BASENAME}.src.rpm"

ls -l ${MOCK_BUILD_DIR}/RPMS

ALL_BIN_KITNAMES="
${KITNAME}
${KITNAME}-cli
${KITNAME}-common
${KITNAME}-debuginfo"

for BIN_KITNAME in ${ALL_BIN_KITNAMES}
do
    cp -v "${MOCK_BUILD_DIR}/RPMS/${BIN_KITNAME}-${V}-1.${DISTRO}.x86_64.rpm" tmp
done
