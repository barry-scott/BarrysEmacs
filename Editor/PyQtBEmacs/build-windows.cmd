setlocal
echo on
rem 
if "%1" == "" set DIST=dist
if not "%1" == "" set DIST=%1

set PATH=%BUILDER_QTDIR%\msvc2015_64\bin;%PATH%
set PYTHONPATH=..\exe-pybemacs
%PYTHON% -u make_be_images.py
%PYTHON% -m win_app_packager build be_main.py   --gui %DIST% --icon ..\Source\Windows\Resources\win_emacs.ico --name bemacs_server --verbose
%PYTHON% -m win_app_packager build be_client.py --gui %DIST% --icon ..\Source\Windows\Resources\win_emacs.ico --name bemacs --merge

mkdir %DIST%\plugins\platforms
xcopy /q %BUILDER_QTDIR%\msvc2015_64\plugins\platforms\qwindows.dll %DIST%\plugins\platforms

mkdir %DIST%\plugins\imageformats
xcopy /q %BUILDER_QTDIR%\msvc2015_64\plugins\imageformats\*.dll %DIST%\plugins\imageformats
del /q %DIST%\plugins\imageformats\*d.dll

mkdir %DIST%\plugins\iconengins
xcopy /q %BUILDER_QTDIR%\msvc2015_64\plugins\iconengins\*.dll %DIST%\plugins\iconengins

endlocal
