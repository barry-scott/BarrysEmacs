set emacs_ver=6.0.0
set emacs_zip=emacsv60.zip

set kitroot=d:\work\emacs\kit
set root=d:\work\emacs
del /s %kitroot%\*.*
cd %kitroot%
md %kitroot%\emacs060
md %kitroot%\emacs060\dos
md %kitroot%\emacs060\nt_x86
md %kitroot%\emacs060\nt_axp

copy %root%\src\kit\readme.txt %kitroot%\emacs060

copy c:\bin\nmlk_tsr.com %kitroot%\emacs060
copy %root%\msdos32\emdos32.exe %kitroot%\emacs060
copy %root%\msdos32\dbcreate.exe %kitroot%\emacs060\dos
copy %root%\msdos32\dbadd.exe %kitroot%\emacs060\dos
copy %root%\msdos32\dbdel.exe %kitroot%\emacs060\dos
copy %root%\msdos32\dblist.exe %kitroot%\emacs060\dos
copy %root%\msdos32\dbprint.exe %kitroot%\emacs060\dos
copy %root%\msdos32\mll-2-db.exe %kitroot%\emacs060\dos
copy d:\watcom\bin\pminfo.exe %kitroot%\emacs060
copy d:\watcom\bin\rminfo.exe %kitroot%\emacs060
copy d:\watcom\bin\dos4gw.exe %kitroot%\emacs060

copy %root%\winnt.rel\emwinnt.hlp %kitroot%\emacs060
copy %root%\winnt.rel\emwinnt.exe %kitroot%\emacs060
copy %root%\winnt.rel\dbcreate.exe %kitroot%\emacs060\nt_x86
copy %root%\winnt.rel\dbadd.exe %kitroot%\emacs060\nt_x86
copy %root%\winnt.rel\dbdel.exe %kitroot%\emacs060\nt_x86
copy %root%\winnt.rel\dblist.exe %kitroot%\emacs060\nt_x86
copy %root%\winnt.rel\dbprint.exe %kitroot%\emacs060\nt_x86
copy %root%\winnt.rel\mll-2-db.exe %kitroot%\emacs060\nt_x86

md %kitroot%\emacs060\library

set filename=%kitroot%\emacs060\library\filename.dat
copy nul %filename%

copy %root%\src\mlisp\emacsini.ml %kitroot%\emacs060\library
echo emacsini.ml emacsinit.ml >>%filename%

copy %root%\src\mlisp\profile.ml %kitroot%\emacs060\library
echo profile.ml emacs_profile.ml >>%filename%

%root%\msdos32\dbcreate %kitroot%\emacs060\library\emacslib -c
%root%\msdos32\mll-2-db %root%\src\mlisp\emacslib.mll %kitroot%\emacs060\library\emacslib

%root%\msdos32\dbcreate %kitroot%\emacs060\library\em_desc -c
%root%\msdos32\mll-2-db %root%\src\describe\em_desc.mll %kitroot%\emacs060\library\em_desc

echo language.dir emacslang.dir >>%filename%
echo language.pag emacslang.pag >>%filename%
echo language.dat emacslang.dat >>%filename%
echo language emacslang >>%filename%
%root%\msdos32\dbcreate %kitroot%\emacs060\library\language -c
%root%\msdos32\mll-2-db %root%\src\language\language.mll %kitroot%\emacs060\library\language

echo qi_cc.dir emacs_qinfo_vaxc.dir >>%filename%
echo qi_cc.pag emacs_qinfo_vaxc.pag >>%filename%
echo qi_cc.dat emacs_qinfo_vaxc.dat >>%filename%
echo qi_cc emacs_qinfo_vaxc >>%filename%
%root%\msdos32\dbcreate %kitroot%\emacs060\library\qi_cc -c
%root%\msdos32\mll-2-db %root%\src\describe\qi_cc.mll %kitroot%\emacs060\library\qi_cc

rem %root%\msdos32\dbcreate %kitroot%\emacs060\library\qi_bliss -c
rem %root%\msdos32\mll-2-db %root%\src\describe\qi_bliss.mll %kitroot%\emacs060\library\qi_bliss

rem %root%\msdos32\dbcreate %kitroot%\emacs060\library\qi_dcl -c
rem %root%\msdos32\mll-2-db %root%\src\describe\qi_dcl.mll %kitroot%\emacs060\library\qi_dcl

rem %root%\msdos32\dbcreate %kitroot%\emacs060\library\qi_vms -c
rem %root%\msdos32\mll-2-db %root%\src\describe\qi_vms.mll %kitroot%\emacs060\library\qi_vms

md %kitroot%\emacs060\inifiles
copy %root%\src\kit\emdos32.ini %kitroot%\emacs060\inifiles
copy %root%\src\kit\emacs060.ini %kitroot%\emacs060\inifiles
copy %root%\src\kit\emacs060.reg %kitroot%\emacs060\inifiles

if exist %emacs_zip% del %emacs_zip%
echo _ >em_zip.txt
echo Emacs V%emacs_ver% for Windows and DOS >>em_zip.txt
echo Copyright (c) 1991,1994 Barry A. Scott >>em_zip.txt
echo Internet mail address: barry@scottb.demon.co.uk >>em_zip.txt
echo Unzip this kit using PKUNZIP -d %emacs_zip% c:\ >>em_zip.txt
echo _ >>em_zip.txt

pkzip -a -P -z -r %emacs_zip% emacs060\*.* <em_zip.txt
cd %root%\src\edit
