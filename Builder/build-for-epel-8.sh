#!/bin/bash
ARCH=$(arch)

# EPEL-8 specifics:
# no qt6 - use qt5
# g++ compiler error for std::pair in SFTP code - use --no-sftp
python3 ./package_bemacs.py ${1:-copr-testing} \
    --colour \
    --mock-target=epel-8-${ARCH} \
    --release=${2:-auto} \
    --pyqt-version=5 \
    --no-sftp \
    --system-hunspell \
    --default-font-name="DejaVu Sans Mono" \
    --default-font-package=dejavu-sans-mono-fonts \
    --kit-sqlite=/shared/Downloads/Subversion/sqlite-amalgamation-3280000.zip \
    --kit-pycxx=/shared/Downloads/Subversion/pycxx-7.1.7.tar.gz \
        |& tee build-for-epel-8-${ARCH}.log
