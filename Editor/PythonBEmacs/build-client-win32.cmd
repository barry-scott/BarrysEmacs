setlocal
set INCLUDE=%MEINC_INSTALLER_DIR%
set OUTPUTDIR=%1
nmake -nologo -f win32-client.mak PYTHON=%PYTHON% BEMACS_PYTHONPATH=..\obj-pybemacs OUTPUTDIR=%OUTPUTDIR% %2 %3 %4 %5 %6
endlocal
