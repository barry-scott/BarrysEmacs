	echo on
	set rootdir=%_CWD%
	set rootdir=u:\work\emacsv7\editor
	set emacs_kit=u:\work\emacsv7\kit\emkit070
	set emacs_library=u:\work\emacsv7\kit\emkit070
	set emacs_user=%rootdir%\test_data
	set emacs_journal=%emacs_user%
	set emacs_path="emacs_user: emacs_library:"
	set envfile=%emacs_user%\purify_coverage.emacs_environment.tmp

	mkdir %emacs_user%

	echo Starting up...
	echo "(message \"Testing emacs\")" >%emacs_user%\emacsinit.ml

	copy coverage.mlp %emacs_user%

	cd %emacs_user%


	set imagefile=%rootdir%\editor\debug\editor%1
	rem pause
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
	dir %envfile%
	pause
:pass_2
	echo Pass 2
	%emacs_kit%\dbcreate testdb1 -c
	%emacs_kit%\dbcreate testdb2 -c
	set pass=2
	%imagefile% /package=coverage /restore=%envfile%
	pause

:pass_3
	echo Pass 3
	set pass=3
	%imagefile% /package=coverage /restore=%envfile%
:done
	list coverage.phase-2-results.tmp
	cd ..
	echo Done.
