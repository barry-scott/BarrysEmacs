#!/bin/bash
python3 ./package_bemacs.py ${1:-copr-testing} \
    --colour \
    --mock-target=epel-7-x86_64 \
    --kit-sqlite=/shared/Downloads/Subversion/sqlite-amalgamation-3280000.zip \
    --kit-xml-preferences=/home/barry/Downloads/xml-preferences-1.1.4.tar.gz \
    --kit-pycxx=/shared/Downloads/Subversion/pycxx-7.1.2.tar.gz