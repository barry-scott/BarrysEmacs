setlocal
set PYTHONPATH=%BUILDER_TOP_DIR%\Editor\PyQt6;%BUILDER_TOP_DIR%\Editor\exe-pybemacs
set BEMACS_EMACS_LIBRARY=%BUILDER_TOP_DIR%\Builder\tmp\kitfiles\emacs_library
set BEMACS_EMACS_DOC=%BUILDER_TOP_DIR%\Builder\tmp\kitfiles\Documentation

echo PYTHONPATH=%PYTHONPATH%
echo BEMACS_EMACS_LIBRARY=%BEMACS_EMACS_LIBRARY%

set BEMACS_STDOUT_LOG=bemacs-stdout.log
set BEMACS_GUI_LOG=bemacs-gui.log

%BUILDER_TOP_DIR%\Builder\venv.tmp\Scripts\python.exe -u %BUILDER_TOP_DIR%\Editor\PyQt6\be_main.py %*
endlocal
