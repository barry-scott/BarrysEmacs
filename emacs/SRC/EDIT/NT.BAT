@echo off
set cpu=AXP
if "%processor_architecture%" == "x86" set cpu=i386

set obj_root=\work\emacs
set src_root=\work\emacs\src\edit
if "%1" == "release" goto release
if "%1" == "profile" goto profile
if "%1" == "debug" goto debug
goto help

:release
echo Emacs NT Release build
cd %obj_root%\winnt.rel
rem if exist emwinnt.exe del emwinnt.exe
nmake CPU=%cpu% MFCNODEBUG=1 NODEBUG=1 -f %src_root%\ntemacs.mak 
cd %src_root%
goto exit

:profile
echo Emacs NT Profile build
cd %obj_root%\winnt.rel
rem if exist emwinnt.exe del emwinnt.exe
nmake CPU=%cpu% PROFILE=1 -f %src_root%\ntemacs.mak 
cd %src_root%
goto exit

:debug
echo Emacs NT Debug build
cd %obj_root%\winnt.dbg
rem if exist emwinnt.exe del emwinnt.exe
nmake CPU=%cpu% -f %src_root%\ntemacs.mak 
cd %src_root%
goto exit

:help
	echo "Usage: NT [release|debug|profile]"
:exit
