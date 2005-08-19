#!/usr/bin/ksh
echo Listing of Emacs KIT directory contents
cd $(emacs_exe)
pwd

while read a_file
do
	case $a_file in
	*.*)	chmod +r $a_file;;
	*)	chmod +xr $a_file;;
	esac
	ls -l $a_file
done <$(kit_src)ultrix_kit.files

echo Making Emacs KIT tar file
tar cfR $(emacs_exe)emacs_ultrix_mips_x060.tar $(kit_src)ultrix_kit.files

echo Listing of Emacs KIT tar file
ls -l $(emacs_exe)emacs_ultrix_mips_x060.tar
tar tf $(emacs_exe)emacs_ultrix_mips_x060.tar
