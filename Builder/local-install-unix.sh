#!/bin/bash
cd ${BUILDER_TOP_DIR?run builder_init}/Builder/tmp/ROOT

tar czf ${TMPDIR:-/tmp}/bemacs-tmp.tar.gz usr
sudo tar xzf ${TMPDIR:-/tmp}/bemacs-tmp.tar.gz -C /
