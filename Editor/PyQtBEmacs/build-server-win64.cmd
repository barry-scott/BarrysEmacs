setlocal
set INCLUDE=%MEINC_INSTALLER_DIR%
set OUTPUTDIR=%1
shift
nmake -nologo -f win64-server.mak PYTHON=%PYTHON% BEMACS_PYTHONPATH=..\obj-pybemacs OUTPUTDIR=%OUTPUTDIR% %*
endlocal