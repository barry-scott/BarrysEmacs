setlocal
set INCLUDE=%MEINC_INSTALLER_DIR%
nmake -nologo -f win32-server.mak PYTHON=%PYTHON% BEMACS_PYTHONPATH=..\obj-pybemacs OUTPUTDIR=..\..\Kit\win32 %*
nmake -nologo -f win32-client.mak PYTHON=%PYTHON% BEMACS_PYTHONPATH=..\obj-pybemacs OUTPUTDIR=..\..\Kit\win32 %*
endlocal
