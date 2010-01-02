	echo on
	set rootdir=%_CWD%
	set rootdir=d:\work\emacsv7\editor
	set emacs_library=d:\emacs070\library
	set emacs_user=%rootdir%\test_data
	set emacs_path="emacs_user: emacs_library:"
	set envfile=%emacs_user%\purify_coverage.emacs_environment.tmp
	set imagefile=%rootdir%\editor\Debug\editor
                                             
	mkdir %emacs_user%

	echo Starting up...
	echo "(message \"Testing emacs\")" >%emacs_user%\emacsinit.ml

	copy coverage.mlp %emacs_user%

	cd %emacs_user%

:pass_1
	echo Pass 1
	erase %envfile%
	erase *.ej*
	erase *.tmp

	echo int main()		 >file.c
	echo 	{		>>file.c
	echo 	return 0;	>>file.c
	echo 	}		>>file.c

	set EMACS_DEBUG=0x10000000	# unused debug value
	set pass=1
	%imagefile% /package=coverage /restore=no-such-file file.c
	echo pass 1 done
	pause
	dir %envfile%
:pass_2
	echo Pass 2
	d:\emacs070\nt_x86\dbcreate testdb1 -c
	d:\emacs070\nt_x86\dbcreate testdb2 -c
	set pass=2
	echo emacs /package=coverage /restore=%envfile%
	pause
	%imagefile% /package=coverage /restore=%envfile%
	echo pass 2 done
	pause

:pass_3
	echo Pass 3
	set pass=3
	%imagefile% /package=coverage /restore=%envfile%
	echo pass 3 done
	pause
:done
	more coverage.phase-2-results.tmp
	echo Done.
