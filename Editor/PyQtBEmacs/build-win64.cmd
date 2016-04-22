setlocal
echo on
rem 
if "%1" == "" set DIST=dist
if not "%1" == "" set DIST=%1

set PATH=%BUILDER_QTDIR%\\msvc2015_64\bin;%PATH%
set PYTHONPATH=..\obj-pybemacs
%PYTHON% -u make_be_images.py
%PYTHON% -m win_app_packager build be_main.py   --gui %DIST% --icon ..\Source\Windows\Resources\win_emacs.ico --name bemacs_server --verbose
%PYTHON% -m win_app_packager build be_client.py --gui %DIST% --icon ..\Source\Windows\Resources\win_emacs.ico --name bemacs --merge
endlocal
