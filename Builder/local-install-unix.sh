#!/bin/sh
cd ${BUILDER_TOP_DIR?run builder_init}/Builder/tmp/ROOT

case "$BUILDER_CFG_PLATFORM" in
NetBSD)
    tar_cmd=gtar
    ;;
*)
    tar_cmd=tar
    ;;
esac

# create tar ball as if files are owner by root:root
$tar_cmd czf ${TMPDIR:-/tmp}/bemacs-tmp.tar.gz --owner=root --group=root usr
# do not change meta data on existing directories
sudo_cmd="$(which sudo)"
$sudo_cmd $tar_cmd xzf ${TMPDIR:-/tmp}/bemacs-tmp.tar.gz --keep-directory-symlink --no-overwrite-dir -C /
