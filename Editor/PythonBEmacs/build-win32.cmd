setlocal
set INCLUDE=%MEINC_INSTALLER_DIR%
nmake -nologo -f win32.mak PYTHON=%PYTHON% BEMACS_PYTHONPATH=..\win32\obj-pybemacs OUTPUTDIR=..\..\Kit\win32 %*
endlocal
