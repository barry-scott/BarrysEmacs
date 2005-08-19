#!/bin/ksh
date_str=`date +%y-%m-%d`
tar_file=../em7src-${date_str}.tar
tar -cvf $tar_file *.ksh *.mak *.h *.cpp bitmaps/*.bm x11_keymaps/*.xkeys |xargs -n5 echo
gzip $tar_file
ls -l ${tar_file}.gz
