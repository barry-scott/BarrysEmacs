	echo on
	rem /fv - function coverage
	rem /ft - function timing
	rem /fc - function count
	rem /lv - line coverage
	rem /lc - line counting
	if "%1" == "" echo "Usage: coverage [fv|ft|fc|lv|lc]"
	if "%1" == "" exit
	
	set profile_type=%1

	set rootdir=%_CWD%
	set rootdir=d:\work\emacsv7\editor
	set emacs_library=d:\emacs070\library
	set emacs_user=%rootdir%\test_data
	set emacs_path="emacs_user: emacs_library:"
	set envfile=%emacs_user%\purify_coverage.emacs_environment.tmp

	mkdir %emacs_user%

	echo Starting up...
	echo "(message \"Testing emacs\")" >%emacs_user%\emacsinit.ml

	copy coverage.mlp %emacs_user%

	cd %emacs_user%


	if "%profile_type%" == "fv" set imagefile=%rootdir%\editor\Profile\editor
	if "%profile_type%" == "ft" set imagefile=%rootdir%\editor\Profile\editor
	if "%profile_type%" == "fc" set imagefile=%rootdir%\editor\Profile\editor
	if "%profile_type%" == "lv" set imagefile=%rootdir%\editor\Debug\editor
	if "%profile_type%" == "lc" set imagefile=%rootdir%\editor\Debug\editor

	if "%profile_type%" == "fv" prep /ot coverage /oi coverage /om /%profile_type% %imagefile%
	if "%profile_type%" == "ft" prep /ot coverage /oi coverage /om /%profile_type% %imagefile%
	if "%profile_type%" == "fc" prep /ot coverage /oi coverage /om /%profile_type% %imagefile%
	if "%profile_type%" == "lv" prep /ot coverage /oi coverage /om /%profile_type% /excall @..\coverage-inc.rsp %imagefile%
	if "%profile_type%" == "lc" prep /ot coverage /oi coverage /om /%profile_type% /excall @..\coverage-inc.rsp %imagefile%

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
	profile /nc /i coverage /o pass_1 %imagefile% /package=coverage /restore=no-such-file file.c
	dir %envfile%
	pause
:pass_2
	echo Pass 2
	d:\emacs070\nt_x86\dbcreate testdb1 -c
	d:\emacs070\nt_x86\dbcreate testdb2 -c
	set pass=2
	profile /nc /i coverage /o pass_2 %imagefile% /package=coverage /restore=%envfile%
	pause

:pass_3
	echo Pass 3
	set pass=3
	profile /nc /i coverage /o pass_3 %imagefile% /package=coverage /restore=%envfile%
:done
	prep  /io pass_1 /io pass_2 /io pass_3 /it coverage /ot result
	pause
	plist result >..\profile-%profile_type%.list
	pause
	list coverage.phase-2-results.tmp
	cd ..
	echo Done.
