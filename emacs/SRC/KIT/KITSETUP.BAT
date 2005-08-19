rem
rem
rem	Make the emacs kit
rem		%1	- "zip" to do the zip
rem		%2	- "dos" if building on MSDOS
rem

set emacs_ver=6.1.23
set emacs_zip=be6_1_23.zip

set dbtools=winnt.rel
if "%2" == "dos" set dbtools=msdos32 

set kitroot=\work\emacs\kit
set root=\work\emacs

set IS3=q:\InstallShield3

attrib -r %kitroot%\emkit060\*.*
del /s %kitroot%\*.*
cd %kitroot%
md %kitroot%\emkit060

rem the Windows 3.1 and Windows 95 install
copy d:\osetup21\redist\setup.exe %kitroot\emkit060\setup3_1.exe
copy d:\osetup21\redist\_osetup.exe %kitroot\emkit060
copy %root%\src\kit\_osetup.inf %kitroot\emkit060
copy %root%\src\kit\_osetup.bmp %kitroot\emkit060

copy %root%\src\kit\postsetu.bat %kitroot\emkit060

rem the DOS install
copy %root%\src\kit\dossetup.bat %kitroot\emkit060

rem the Windows NT install
copy %root%\src\kit\setup.rul .
%IS3%\program\compile setup
copy setup.ins %kitroot%\emkit060
copy %IS3%\program\setup.exe %kitroot%\emkit060\setup.exe
copy %IS3%\program\_setup.lib %kitroot%\emkit060
copy %IS3%\program\_isdel.exe %kitroot%\emkit060
copy %IS3%\program\_setup.dll %kitroot%\emkit060
copy %IS3%\program\_inst32i.ex_ %kitroot%\emkit060

rem the kits files...
copy %root%\src\kit\readme.txt %kitroot%\emkit060

copy d:\bin\nmlk_tsr.com %kitroot%\emkit060
copy %root%\msdos32\emdos32.exe %kitroot%\emkit060
copy %root%\msdos32\dbcreate.exe %kitroot%\emkit060\dos_dbcr.exe
copy %root%\msdos32\dbadd.exe %kitroot%\emkit060\dos_dbad.exe
copy %root%\msdos32\dbdel.exe %kitroot%\emkit060\dos_dbde.exe
copy %root%\msdos32\dblist.exe %kitroot%\emkit060\dos_dbli.exe
copy %root%\msdos32\dbprint.exe %kitroot%\emkit060\dos_dbpr.exe
copy %root%\msdos32\mll-2-db.exe %kitroot%\emkit060\dos_m2db.exe
rem copy d:\watcom\bin\pminfo.exe %kitroot%\emkit060
rem copy d:\watcom\bin\rminfo.exe %kitroot%\emkit060
copy d:\watcom\bin\dos4gw.exe %kitroot%\emkit060

copy %root%\winnt.rel\emwinnt.hlp %kitroot%\emkit060
copy %root%\winnt.rel\emwinnt.exe %kitroot%\emkit060
copy %root%\winnt.rel\dbcreate.exe %kitroot%\emkit060\x86_dbcr.exe
copy %root%\winnt.rel\dbadd.exe %kitroot%\emkit060\x86_dbad.exe
copy %root%\winnt.rel\dbdel.exe %kitroot%\emkit060\x86_dbde.exe
copy %root%\winnt.rel\dblist.exe %kitroot%\emkit060\x86_dbli.exe
copy %root%\winnt.rel\dbprint.exe %kitroot%\emkit060\x86_dbpr.exe
copy %root%\winnt.rel\mll-2-db.exe %kitroot%\emkit060\x86_m2db.exe

copy %root%\src\kit\emacs.cbi %kitroot%\emkit060\Emacs.cbi

copy %root%\src\help\ug.hlp %kitroot%\emkit060\bemacsug.hlp
copy %root%\src\help\pg.hlp %kitroot%\emkit060\bemacspg.hlp
copy %root%\src\help\er.hlp %kitroot%\emkit060\bemacser.hlp

set filename=%kitroot%\emkit060\library.dat
copy nul %filename%

copy %root%\src\mlisp\emacsini.ml %kitroot%\emkit060
echo emacsini.ml emacsinit.ml >>%filename%

copy %root%\src\mlisp\profile.ml %kitroot%\emkit060
echo profile.ml emacs_profile.ml >>%filename%

%root%\%dbtools%\dbcreate %kitroot%\emkit060\emacslib -c
%root%\%dbtools%\mll-2-db %root%\src\mlisp\emacslib.mll %kitroot%\emkit060\emacslib

echo em_desc.dir emacsdesc.dir >>%filename%
echo em_desc.pag emacsdesc.pag >>%filename%
echo em_desc.dat emacsdesc.dat >>%filename%
echo em_desc emacsdesc >>%filename%
%root%\%dbtools%\dbcreate %kitroot%\emkit060\em_desc -c
%root%\%dbtools%\mll-2-db %root%\src\describe\em_desc.mll %kitroot%\emkit060\em_desc

echo language.dir emacslang.dir >>%filename%
echo language.pag emacslang.pag >>%filename%
echo language.dat emacslang.dat >>%filename%
echo language emacslang >>%filename%
%root%\%dbtools%\dbcreate %kitroot%\emkit060\language -c
%root%\%dbtools%\mll-2-db %root%\src\language\language.mll %kitroot%\emkit060\language

echo qi_cc.dir emacs_qinfo_vaxc.dir >>%filename%
echo qi_cc.pag emacs_qinfo_vaxc.pag >>%filename%
echo qi_cc.dat emacs_qinfo_vaxc.dat >>%filename%
echo qi_cc emacs_qinfo_vaxc >>%filename%
%root%\%dbtools%\dbcreate %kitroot%\emkit060\qi_cc -c
%root%\%dbtools%\mll-2-db %root%\src\describe\qi_cc.mll %kitroot%\emkit060\qi_cc

echo qi_bliss.dir emacs_qinfo_bliss.dir >>%filename%
echo qi_bliss.pag emacs_qinfo_bliss.pag >>%filename%
echo qi_bliss.dat emacs_qinfo_bliss.dat >>%filename%
echo qi_bliss emacs_qinfo_bliss >>%filename%
%root%\%dbtools%\dbcreate %kitroot%\emkit060\qi_bliss -c
%root%\%dbtools%\mll-2-db %root%\src\describe\qi_bliss.mll %kitroot%\emkit060\qi_bliss

echo qi_dcl.dir emacs_qinfo_dcl.dir >>%filename%
echo qi_dcl.pag emacs_qinfo_dcl.pag >>%filename%
echo qi_dcl.dat emacs_qinfo_dcl.dat >>%filename%
echo qi_dcl emacs_qinfo_dcl >>%filename%
%root%\%dbtools%\dbcreate %kitroot%\emkit060\qi_dcl -c
%root%\%dbtools%\mll-2-db %root%\src\describe\qi_dcl.mll %kitroot%\emkit060\qi_dcl

echo qi_vms.dir emacs_qinfo_system_services.dir >>%filename%
echo qi_vms.pag emacs_qinfo_system_services.pag >>%filename%
echo qi_vms.dat emacs_qinfo_system_services.dat >>%filename%
echo qi_vms emacs_qinfo_system_services >>%filename%
%root%\%dbtools%\dbcreate %kitroot%\emkit060\qi_vms -c
%root%\%dbtools%\mll-2-db %root%\src\describe\qi_vms.mll %kitroot%\emkit060\qi_vms

rem md %kitroot%\emkit060\inifiles
copy %root%\src\kit\emacs060.reg %kitroot%\emkit060

if exist %emacs_zip% del %emacs_zip%
echo _ >em_zip.txt
echo Barry's Emacs X%emacs_ver% for Windows/NT, Windows and DOS >>em_zip.txt
echo Copyright (c) 1991-1995 Barry A. Scott >>em_zip.txt
echo Internet mail address: barry@scottb.demon.co.uk >>em_zip.txt
echo Unzip this kit using PKUNZIP %emacs_zip% c:\kittmp >>em_zip.txt
echo Run SETUP.EXE for Windows installation or >>em_zip.txt
echo DOSSETUP.BAT for DOS installations >>em_zip.txt
echo _ >>em_zip.txt

attrib -r %kitroot%\emkit060\*.*

if "%1" == "zip" cd emkit060
if "%1" == "zip" pkzip -a -z -ex ..\%emacs_zip% *.* <..\em_zip.txt
cd %root%\src\kit
