#!/bin/bash
cd ${BUILDER_TOP_DIR?run builder_init}/Builder/tmp/ROOT

# create tar ball as if files are owner by root:root
tar czf ${TMPDIR:-/tmp}/bemacs-tmp.tar.gz --owner=root --group=root usr
# do not change meta data on existing directories
sudo tar xzf ${TMPDIR:-/tmp}/bemacs-tmp.tar.gz --keep-directory-symlink --no-overwrite-dir -C /
