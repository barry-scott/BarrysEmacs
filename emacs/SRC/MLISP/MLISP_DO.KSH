#/usr/bin/ksh -v

echo mlisp_do.ksh: creating $3
$(emacs_exe)dbcreate $(emacs_exe)$3 -c

case $1 in
mlisp)		$(emacs_exe)mll-2-db $(mlisp_src)$2 $(emacs_exe)$3;;
language)	$(emacs_exe)mll-2-db $(language_src)$2 $(emacs_exe)$3;;
describe)	$(emacs_exe)mll-2-db $(describe_src)$2 $(emacs_exe)$3;;
*)		print "mlisp_do.ksh: error $1 $2 $3"
		exit 1;;
esac
