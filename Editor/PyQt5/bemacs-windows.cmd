setlocal
set PYTHONPATH=%BUILDER_TOP_DIR%\Editor\PyQtBEmacs;%BUILDER_TOP_DIR%\Editor\exe-pybemacs
set BEMACS_EMACS_LIBRARY=%BUILDER_TOP_DIR%\Builder\tmp\kitfiles\emacs_library
set BEMACS_EMACS_DOC=%BUILDER_TOP_DIR%\Builder\tmp\kitfiles\Documentation

echo PYTHONPATH=%PYTHONPATH%
echo BEMACS_EMACS_LIBRARY=%BEMACS_EMACS_LIBRARY%

set BEMACS_STDOUT_LOG=bemacs-stdout.log
set BEMACS_DEBUG_LOG=bemacs-debug.log
set BEMACS_GUI_LOG=bemacs-gui.log

%BUILDER_TOP_DIR%\Builder\venv.tmp\Scripts\pythonw.exe -u be_main.py %* 2>%BEMACS_DEBUG_LOG%
endlocal
