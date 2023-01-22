setlocal
@echo on
if "%VPYTHON%" == "" (
    colour-print "<>error Error: VPYTHON is not defined"
    goto :error
)

if "%1" == "--gui" (
    set APPMODE=--gui
    shift
)
if "%1" == "--cli" (
    set APPMODE=--cli
    shift
)

if "%1" == "" set DIST_DIR=dist
if not "%1" == "" set DIST_DIR=%1
echo Info: DIST_DIR=%DIST_DIR%

if "%2" == "" set VERSION=0.0.0.0
if not "%2" == "" set VERSION=%2


set PYTHONPATH=..\exe-pybemacs
"%PYTHON%" -u make_be_images.py
    if errorlevel 1 goto :error
rem always merge as the build system puts Docs and emacs lib files in first
%VPYTHON% -m win_app_packager build be_main.py   %APPMODE% %DIST_DIR% --version %VERSION% --icon ..\Source\Windows\Resources\win_emacs.ico --name bemacs_server --merge --modules-allowed-to-be-missing-file allowed-missing.txt
    if errorlevel 1 goto :error
%VPYTHON% -m win_app_packager build be_client.py %APPMODE% %DIST_DIR% --version %VERSION% --icon ..\Source\Windows\Resources\win_emacs.ico --name bemacs --merge --modules-allowed-to-be-missing-file allowed-missing.txt
    if errorlevel 1 goto :error

rem set the stack to 16MiB aka 16777216
dumpbin /nologo /headers %DIST_DIR%\bemacs_server.exe | findstr "stack"
editbin /nologo /stack:16777216 %DIST_DIR%\bemacs_server.exe
dumpbin /nologo /headers %DIST_DIR%\bemacs_server.exe | findstr "stack"

rem do not need the test folders
rmdir /s /q %DIST_DIR%\PyWinAppRes\Lib\lib2to3
rmdir /s /q %DIST_DIR%\PyWinAppRes\Lib\test
rmdir /s /q %DIST_DIR%\PyWinAppRes\Lib\unittest
rmdir /s /q %DIST_DIR%\PyWinAppRes\Lib\ctypes\test

rem do not need tkinter
rmdir /s /q %DIST_DIR%\PyWinAppRes\Lib\tkinter

rem do not need pydoc
rmdir /s /q %DIST_DIR%\PyWinAppRes\Lib\pydoc_data

rem do not need colour-text
rmdir /s /q %DIST_DIR%\PyWinAppRes\Lib\site-packages\colour_text
rem do not need the packager
rmdir /s /q %DIST_DIR%\PyWinAppRes\Lib\site-packages\win_app_packager

pushd %DIST_DIR%\PyWinAppRes\Lib\site-packages\PyQt6
    if errorlevel 1 goto :error

echo Info: clean up Qt 3. delete QtQuick file
del Qt6\bin\\Qt6Quick*
    if errorlevel 1 goto :error

echo Info: clean up Qt 4. delete qml file
rmdir /s /q Qt6\qml
    if errorlevel 1 goto :error

echo Info: clean up Qt 5. delete translations file
rmdir /s /q Qt6\translations
    if errorlevel 1 goto :error

echo Info: clean up Qt 6. delete webengine files
if exist Qt6\bin\QtWebEngineProcess.exe (
    del Qt6\bin\QtWebEngineProcess.exe >NUL
        if errorlevel 1 goto :error
    del Qt6\resources\qtwebengine*.pak >NUL
        if errorlevel 1 goto :error
)

echo Info: clean up Qt 7. remove .sip files
del /s *.sip

popd

echo Info: build-windows completed successfully
exit /b 0
endlocal

:qt_keep
    echo Info: Keeping Qt%1
    move tmp\Qt%1.pyd . >NUL
        if errorlevel 1 goto :error
    move Qt6\bin\tmp\Qt6%1.dll Qt6\bin >NUL
        if errorlevel 1 goto :error
    goto :eof

:error
    echo Error: build-windows failed
    exit /b 1
