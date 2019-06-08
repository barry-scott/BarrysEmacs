#!/bin/bash
python3 ./package_bemacs.py ${1:-copr-testing} \
    --colour \
    --system-ucd
