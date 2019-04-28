#!/bin/bash
#
#   brand.build-fedora-rpms.sh
#
set -e
CMD="$1"
shift

case "$1" in
--revision=*)
    R="${1#*=}"
    shift
    ;;
*)
    R="1"
    ;;
esac

if [ "$1" != "" ]
then
    VERSION_ID="$1"
    shift
else
    . /etc/os-release
fi

if [ "$1" != "" ]
then
    ARCH="$3"
    shift
else
    ARCH=$( uname -m )
fi

rm -rf tmp
mkdir -p tmp

MOCK_ORIG_VERSION_NAME=fedora-${VERSION_ID}-${ARCH}
MOCK_COPR_REPO_FILE=/etc/yum.repos.d/_copr\:copr.fedorainfracloud.org\:barryascott\:tools.repo
MOCK_VERSION_NAME=tmp/bemacs-${MOCK_ORIG_VERSION_NAME}.cfg

echo "Info: Creating mock config ${MOCK_VERSION_NAME}"
python3 bemacs_make_mock_cfg.py \
    /etc/mock/${MOCK_ORIG_VERSION_NAME}.cfg ${MOCK_COPR_REPO_FILE} \
        ${MOCK_VERSION_NAME}

MOCK_ROOT=$( mock --root=${MOCK_VERSION_NAME} -p )

if [ -e "${MOCK_ROOT}" ]
then
    echo "Info: Using existing mock for ${MOCK_VERSION_NAME}"

else
    echo "Info: Init mock for ${MOCK_VERSION_NAME}"
    mock \
        --root=${MOCK_VERSION_NAME} \
        --init
fi

echo "Info: Creating source tarball"

KITNAME=bemacs

V=%(major)s.%(minor)s.%(patch)s
KIT_BASENAME=${KITNAME}-${V}

pushd tmp
echo "Info: Exporting source code"

(cd ${BUILDER_TOP_DIR}; git archive --format=tar --prefix=${KIT_BASENAME}/ master) | tar xf -

git show-ref --head --hash head >${KIT_BASENAME}/Builder/commit_id.txt

tar czf ${KIT_BASENAME}.tar.gz ${KIT_BASENAME}
popd

echo "Info: creating bemacs.spec"
python3 bemacs_make_spec_file.py gui ${V} ${R} tmp/bemacs.spec

echo "Info: Creating SRPM for ${KIT_BASENAME}"

mock \
    --root=${MOCK_VERSION_NAME} \
    --buildsrpm --dnf \
    --spec tmp/bemacs.spec \
    --sources tmp/${KIT_BASENAME}.tar.gz

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

SRPM_BASENAME="${KIT_BASENAME}-${R}.${DISTRO}"

cp -v "${MOCK_BUILD_DIR}/SRPMS/${SRPM_BASENAME}.src.rpm" tmp

case "${CMD}" in
"--srpm")
    echo "Info: SRPM is tmp/${SRPM_BASENAME}.src.rpm"
    ;;

"--copr")
    copr-cli build -r fedora-${VERSION_ID}-x86_64 tools "tmp/${SRPM_BASENAME}.src.rpm"
    ;;

*)
    echo "Info: Creating RPM"
    exit 1
    mock \
        --root=${MOCK_VERSION_NAME} \
        --enablerepo=barryascott-tools \
        --rebuild --dnf \
            "tmp/${SRPM_BASENAME}.src.rpm"

    ls -l ${MOCK_BUILD_DIR}/RPMS

    ALL_BIN_KITNAMES="
    ${KITNAME}
    ${KITNAME}-cli
    ${KITNAME}-gui
    ${KITNAME}-common
    ${KITNAME}-debuginfo"

    for BIN_KITNAME in ${ALL_BIN_KITNAMES}
    do
        cp -v "${MOCK_BUILD_DIR}/RPMS/${BIN_KITNAME}-${V}-${R}.${DISTRO}.x86_64.rpm" tmp
    done

    echo "Info: Results in ${PWD}/tmp:"
    ls -l tmp

    if [ "$CMD" = "--install" ]
    then
        echo "Info: Installing RPM"

        for BIN_KITNAME in ${ALL_BIN_KITNAMES}
        do
            if rpm -q ${BIN_KITNAME}
            then
                sudo dnf -y remove ${BIN_KITNAME}
            fi
        done
        sudo dnf -y install tmp/${KITNAME}*.x86_64.rpm
    fi
    ;;
esac
