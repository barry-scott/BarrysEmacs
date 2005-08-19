	goto make_%1
:make_dos386
:make_win32
rem	call c800
rem	set nmake=%@search[nmake]
	call watcom
	e:\msvc\bin\nmaker /f emacs.mak appclass=%1 %2
	goto done
:make_
:make_winnt
	call nt %2
:done
pause
