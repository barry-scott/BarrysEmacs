@echo off
echo.
echo	Emacs X6.1.14 MS DOS installion procedure
echo.

if exist _osetup.inf goto install_check_done
echo.
echo	cd to the directory containing DOSSETUP.BAT
echo	then run DOSSETUP
echo.
goto	exit

:install_check_done

set install_dir=%1
if not "%1" == "" goto start
set install_dir=c:\emacs060
:start
echo.	Installing Emacs in %install_dir%.
echo.
echo.	To specify a different directory call DOSSETUP with the
echo.	directory name as its first paramter.
echo.
echo.	Type RETURN to preceed with installation or Ctrl-C to quit.
pause
echo.
echo	Creating directories
mkdir	%install_dir%
mkdir	%install_dir%\library
mkdir	%install_dir%\userlib
mkdir	%install_dir%\inifiles
mkdir	%install_dir%\dos

echo	Copying common files
copy README.TXT %install_dir%\README.TXT >nul
copy LIBRARY.DAT %install_dir%\LIBRARY\FILENAME.DAT >nul

copy EMACSINI.ML %install_dir%\LIBRARY\EMACSINI.ML >nul
copy PROFILE.ML %install_dir%\LIBRARY\PROFILE.ML >nul

xcopy/y EMACSLIB.DIR %install_dir%\LIBRARY\ >nul
copy EMACSLIB.PAG %install_dir%\LIBRARY\EMACSLIB.PAG >nul
copy EMACSLIB.DAT %install_dir%\LIBRARY\EMACSLIB.DAT >nul

xcopy/y EM_DESC.DIR %install_dir%\LIBRARY\ >nul
copy EM_DESC.PAG %install_dir%\LIBRARY\EM_DESC.PAG >nul
copy EM_DESC.DAT %install_dir%\LIBRARY\EM_DESC.DAT >nul

xcopy/y LANGUAGE.DIR %install_dir%\LIBRARY\ >nul
copy LANGUAGE.PAG %install_dir%\LIBRARY\LANGUAGE.PAG >nul
copy LANGUAGE.DAT %install_dir%\LIBRARY\LANGUAGE.DAT >nul

xcopy/y QI_CC.DIR %install_dir%\LIBRARY\ >nul
copy QI_CC.PAG %install_dir%\LIBRARY\QI_CC.PAG >nul
copy QI_CC.DAT %install_dir%\LIBRARY\QI_CC.DAT >nul

echo	Copying MS-DOS specific files
copy NMLK_TSR.COM %install_dir%\NMLK_TSR.COM >nul
copy EMDOS32.EXE %install_dir%\EMDOS32.EXE >nul
copy DOS_DBCR.EXE %install_dir%\DOS\DBCREATE.EXE >nul
copy DOS_DBAD.EXE %install_dir%\DOS\DBADD.EXE >nul
copy DOS_DBDE.EXE %install_dir%\DOS\DBDEL.EXE >nul
copy DOS_DBLI.EXE %install_dir%\DOS\DBLIST.EXE >nul
copy DOS_DBPR.EXE %install_dir%\DOS\DBPRINT.EXE >nul
copy DOS_M2DB.EXE %install_dir%\DOS\MLL-2-DB.EXE >nul
copy DOS4GW.EXE %install_dir%\DOS4GW.EXE >nul

echo	Creating template EMDOS32.INI
set install_ini_file=%install_dir%\inifiles\emdos32.ini
set install_tmp=.\
if not "%tmp%" == "" set install_tmp=%tmp%
if not "%temp%" == "" set install_tmp=%temp%

echo [DeviceNames] >%install_ini_file%
echo sys$login=%install_dir% >>%install_ini_file%
echo sys$scratch=%install_tmp% >>%install_ini_file%
echo emacs$memory=.\ >>%install_ini_file%
echo emacs$library=%install_dir%\library\ >>%install_ini_file%
echo emacs$user=%install_dir%\userlib\ >>%install_ini_file%
echo emacs$journal=%install_tmp% >>%install_ini_file%

echo [Colours] >>%install_ini_file%
echo WindowText=1000;2000;3000;4255;5255;6255 >>%install_ini_file%
echo HighlightText=4255;1255;2255;3255 >>%install_ini_file%
echo ModeLine=6255;1255;2255 >>%install_ini_file%
echo Syntax_keyword1=1000;2000;3255;4255;5255;6255 >>%install_ini_file%
echo Syntax_keyword2=1255;2000;3000;4255;5255;6255 >>%install_ini_file%
echo Syntax_word=1000;2000;3000;4255;5255;6255 >>%install_ini_file%
echo Syntax_string=1127;2000;3000;4255;5255;6255 >>%install_ini_file%
echo Syntax_quote=1000;2000;3000;4255;5255;6255 >>%install_ini_file%
echo Syntax_comment=1000;2127;3000;4255;5255;6255 >>%install_ini_file%

echo [Environment] >>%install_ini_file%
echo Emacs_Path=.;emacs$user:;emacs$library: >>%install_ini_file%
echo emacs_memory=emacs$memory:__emacs_.mem >>%install_ini_file%
echo ;video_mode [mode number] [width] [height] >>%install_ini_file%
echo ;video_mode 35 132 25 >>%install_ini_file%

if not exist %install_dir%\emdos32.ini copy %install_dir%\inifiles\emdos32.ini %install_dir%\ >nul

echo	Copying Quick info files
xcopy/y QI_BLISS.DIR %install_dir%\LIBRARY\ >nul
copy QI_BLISS.PAG %install_dir%\LIBRARY\QI_BLISS.PAG >nul
copy QI_BLISS.DAT %install_dir%\LIBRARY\QI_BLISS.DAT >nul

xcopy/y QI_DCL.DIR %install_dir%\LIBRARY\ >nul
copy QI_DCL.PAG %install_dir%\LIBRARY\QI_DCL.PAG >nul
copy QI_DCL.DAT %install_dir%\LIBRARY\QI_DCL.DAT >nul

xcopy/y QI_VMS.DIR %install_dir%\LIBRARY\ >nul
copy QI_VMS.PAG %install_dir%\LIBRARY\QI_VMS.PAG >nul
copy QI_VMS.DAT %install_dir%\LIBRARY\QI_VMS.DAT >nul

echo.
echo	Installion completed
echo.
:exit
set install_dir=
set install_ini_file=
set install_tmp=
