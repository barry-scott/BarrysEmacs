#!/bin/bash

# need to use the snap shots of centos 6
MOCK_CFG=~/ws/code_SaaS/head/build/cloud_tooling/configurations/mock/prx-c68

python3 ./package_bemacs.py mock-standalone \
    --release=100 \
    --colour \
    --mock-target=${MOCK_CFG} \
    --no-gui \
    --no-hunspell \
    --kit-sqlite=$HOME/Downloads/sqlite-amalgamation-3280000.zip
