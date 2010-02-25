@echo off
set PYTHONPATH=%BUILDER_TOP_DIR%\Editor\PythonBEmacs;%BUILDER_TOP_DIR%\Editor\obj-pybemacs
set BEMACS_EMACS_LIBRARY=%BUILDER_TOP_DIR%\Kit\win32\emacs_library

echo PYTHONPATH=%PYTHONPATH%
echo emacs_library=%emacs_library%

python -u be_main.py %*
