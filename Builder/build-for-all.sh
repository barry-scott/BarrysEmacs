#!/bin/bash
cmd=${1?cmd}
./build-for-epel-7.sh "${cmd}"
./build-for-epel-8.sh "${cmd}"
./build-for-fedora.sh "${cmd}" 36
./build-for-fedora.sh "${cmd}" 37
