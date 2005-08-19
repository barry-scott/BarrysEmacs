rem Emacs for Windows post setup actions
rem copy emacs060.ini if it does not exist
if "%1" == "" goto exit
if "%2" == "" goto exit
if not exist %2\emacs060.ini copy %1\inifiles\emacs060.ini %2
rem copy emdos32.ini if emdos32.exe exists and emdos32.ini does not
if not exist %1\emdos32.ini copy %1\inifiles\emdos32.ini %1
:exit
