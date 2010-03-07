set PYTHONPATH=%BUILDER_TOP_DIR%\Editor\PythonBEmacs;%BUILDER_TOP_DIR%\Editor\obj-pybemacs
set BEMACS_EMACS_LIBRARY=%BUILDER_TOP_DIR%\Kit\win32\emacs_library

echo PYTHONPATH=%PYTHONPATH%
echo BEMACS_EMACS_LIBRARY=%BEMACS_EMACS_LIBRARY%

set BEMACS_STDOUT_LOG=bemacs-stdout.log
set BEMACS_DEBUG_LOG=bemacs-debug.log
set BEMACS_GUI_LOG=bemacs-gui.log

python -u be_main.py %*
