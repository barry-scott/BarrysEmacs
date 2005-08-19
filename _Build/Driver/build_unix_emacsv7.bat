@echo off

set CFG=None
set SRC=None
set UNIX=None

if "%1" == "release" set CFG=Release
if "%1" == "debug" set CFG=Debug
if "%1" == "Release" set CFG=Release
if "%1" == "Debug" set CFG=Debug

if "%2" == "linux" set UNIX=Linux
if "%2" == "freebsd" set UNIX=FreeBSD

if "%3" == "latest" set SRC=/latest=source /deliver=test-only
if "%3" == "config" set SRC=/configure /deliver=add-new
if "%3" == "label" set SRC=/source-label=Stable_EmacsV7_%4_SRC /result-label=Stable_EmacsV7_%4_BIN /deliver=add-new


if "%3" == "latest" set BRANCH=Latest
if "%3" == "config" set BRANCH=Stable
if "%3" == "label" set BRANCH=Stable

if "%CFG%" == "None" goto usage
if "%UNIX%" == "None" goto usage
if "%SRC%" == "None" goto usage

set WD=x:\Build\BEmacs\%BRANCH%\%UNIX%\%CFG%
echo on
if exist %WD% rmdir /s /q %WD%
pause
ridgebuild /clean=delete-all,nochecks /work=%WD% /data=Emacs
pause
ridgebuild %SRC% /clean=delete-all,nochecks /fetch=brand /end-of-line=unix /build /proj=$/%BRANCH%/emacsv7 /work=%WD% /database=Emacs /cfg=%CFG%+%UNIX% /mail=barry@freebsd.private /smtp=freebsd
@echo off
goto exit
:usage
echo Usage: %0 [release^|debug] [linux^|freebsd] [latest^|config^|label nnnn]

:exit
