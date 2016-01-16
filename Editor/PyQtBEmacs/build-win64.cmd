setlocal
echo on
rem ..\Source\Windows\Resources\win_emacs.ico
if "%1" == "" set DIST=dist
if not "%1" == "" set DIST=%1

set PYTHONPATH=..\obj-pybemacs
%PYTHON% -u make_be_images.py
%PYTHON% -m win_app_packager build be_main.py   --gui %DIST% --name bemacs_server
%PYTHON% -m win_app_packager build be_client.py --gui %DIST% --name bemacs_client --merge
endlocal
