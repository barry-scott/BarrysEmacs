@echo off

set CFG=Release
set STEPS=/clean=delete-all /fetch=brand /build /deliver=add-new 
set PROJ=/proj=$/Stable/Emacsv7 /database=Emacs 
set MAIL=/mail=barry@freebsd /smtp=freebsd
set WIN32_WD=u:\build\emacsv7\%CFG%
set FREEBSD_WD=x:\Build\BEmacs\FreeBSD\%CFG%
set LINUX_WD=x:\Build\BEmacs\Linux\%CFG%
set STATE=/state=u:\build\bemacs-build.state

rem
rem	Configure to label the sources
rem
ridgebuild /configure %STATE% %PROJ%

rem
rem	Need to clean off samba disks twice
rem
ridgebuild /clean=delete-all /work=%FREEBSD_WD% /data=Emacs
ridgebuild /clean=delete-all /work=%LINUX_WD%   /data=Emacs

rem
rem	Build all the kits
rem
start ridgebuild %STATE%                   %STEPS% %PROJ% /work=%WIN32_WD%   /cfg=%CFG%+Win32   %MAIL%
start ridgebuild %STATE% /end-of-line=unix %STEPS% %PROJ% /work=%FREEBSD_WD% /cfg=%CFG%+FreeBSD %MAIL%
start ridgebuild %STATE% /end-of-line=unix %STEPS% %PROJ% /work=%LINUX_WD%   /cfg=%CFG%+Linux   %MAIL%
