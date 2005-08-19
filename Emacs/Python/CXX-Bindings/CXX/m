#!/bin/sh
VERSION=$1
shift

case `uname -s` in
Linux)		MAKEFILE=example_linux_py${VERSION}.mak;;
FreeBSD)	MAKEFILE=example_freebsd_py${VERSION}.mak;;
*)		echo Unsupported OS `uname -s`
		exit
esac

make -f ${MAKEFILE} $* 2>&1
