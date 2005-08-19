@echo off

set CFG=None
set SRC=None

if "%1" == "release" set CFG=Release
if "%1" == "debug" set CFG=Debug
if "%1" == "Release" set CFG=Release
if "%1" == "Debug" set CFG=Debug

if "%2" == "latest" set SRC=/latest=source
if "%2" == "config" set SRC=/configure /deliver=add-new

if "%2" == "latest" set BRANCH=Latest
if "%2" == "config" set BRANCH=Stable

if "%CFG%" == "None" goto usage
if "%SRC%" == "None" goto usage

ridgebuild %SRC% /clean=delete-all /fetch=brand /build /proj=$/%BRANCH%/Emacsv7 /work=u:\build\emacsv7\%BRANCH%\%CFG% /data=Emacs /cfg=%CFG%+Win32 /mail=barry@freebsd.private /smtp=freebsd %3

goto exit
:usage
echo Usage: %0 [release^|debug] [latest^|config]

:exit
