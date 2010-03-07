#!/bin/sh
cd ${BUILDER_TOP_DIR}/Kits/Linux/RPM/ROOT
tar czf /tmp/bemacs-tmp.tar.gz usr
tar xzf /tmp/bemacs-tmp.tar.gz -C /
