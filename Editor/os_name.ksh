#!/usr/bin/env bash
#
#	figure out a name for this system
#
name=`uname -s`
case $name in
AIX)
	echo aix-`uname -v`.`uname -r`;;
HP-UX)
	IFS="$IFS."
	set - `uname -r`
	IFS=" "
	echo hp-ux-$2.$3;;
FreeBSD)
	echo freebsd;;
OpenBSD)
	echo openbsd;;
Linux)
	echo linux;;
Darwin)
	echo darwin;;
*)
	echo $name;;
esac
