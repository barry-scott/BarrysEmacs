#!/bin/bash
#
#   brand.build-fedora-rpms.sh
#
./build_fedora_rpms.py "$@" --version=%(major)s.%(minor)s.%(patch)s
