set PYTHONPATH=%BUILDER_TOP_DIR%\Editor\PyQtBEmacs;%BUILDER_TOP_DIR%\Editor\obj-pybemacs
set BEMACS_EMACS_LIBRARY=%BUILDER_TOP_DIR%\Kits\Windows\kitfiles\emacs_library

echo PYTHONPATH=%PYTHONPATH%
echo BEMACS_EMACS_LIBRARY=%BEMACS_EMACS_LIBRARY%

set BEMACS_STDOUT_LOG=bemacs-stdout.log
set BEMACS_DEBUG_LOG=bemacs-debug.log
set BEMACS_GUI_LOG=bemacs-gui.log

%PYTHON% -u be_main.py %*
