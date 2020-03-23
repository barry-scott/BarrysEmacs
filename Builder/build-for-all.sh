#!/bin/bash
cmd=${1?cmd}
./build-for-epel-6.sh "${cmd}"
./build-for-epel-7.sh "${cmd}"
./build-for-fedora.sh "${cmd}" 31
./build-for-fedora.sh "${cmd}" 32
