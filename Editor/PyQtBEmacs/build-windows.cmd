setlocal
echo on
rem 
if "%1" == "" set DIST=dist
if not "%1" == "" set DIST=%1

set PYTHONPATH=..\exe-pybemacs
%PYTHON% -u make_be_images.py
%PYTHON% -m win_app_packager build be_main.py   --gui %DIST% --icon ..\Source\Windows\Resources\win_emacs.ico --name bemacs_server --verbose
%PYTHON% -m win_app_packager build be_client.py --gui %DIST% --icon ..\Source\Windows\Resources\win_emacs.ico --name bemacs --merge

endlocal
