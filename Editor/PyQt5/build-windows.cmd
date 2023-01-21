setlocal
@echo on
if "%VPYTHON%" == "" (
    colour-print "<>error Error: VPYTHON is not defined"
    goto :error
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
%VPYTHON% -m win_app_packager build be_main.py   --gui %DIST_DIR% --version %VERSION% --icon ..\Source\Windows\Resources\win_emacs.ico --name bemacs_server --merge --modules-allowed-to-be-missing-file allowed-missing.txt
    if errorlevel 1 goto :error
%VPYTHON% -m win_app_packager build be_client.py --gui %DIST_DIR% --version %VERSION% --icon ..\Source\Windows\Resources\win_emacs.ico --name bemacs --merge --modules-allowed-to-be-missing-file allowed-missing.txt
    if errorlevel 1 goto :error

rem do not need the test folders
rmdir /s /q %DIST_DIR%\PyWinAppRes\Lib\lib2to3
rmdir /s /q %DIST_DIR%\PyWinAppRes\Lib\test
rmdir /s /q %DIST_DIR%\PyWinAppRes\Lib\unittest

rem do not need tkinter
rmdir /s /q %DIST_DIR%\PyWinAppRes\Lib\tkinter

rem do not need pydoc
rmdir /s /q %DIST_DIR%\PyWinAppRes\Lib\pydoc_data

pushd %DIST_DIR%\PyWinAppRes\Lib\site-packages\PyQt5
    if errorlevel 1 goto :error

echo Info: clean up Qt 1. move all pyd and dll into a tmp folder
mkdir tmp
    if errorlevel 1 goto :error
move Qt*.pyd tmp >NUL
    if errorlevel 1 goto :error
move tmp\Qt.pyd . >NUL
    if errorlevel 1 goto :error

mkdir Qt5\bin\tmp
    if errorlevel 1 goto :error
move Qt5\bin\Qt5*.dll Qt5\bin\tmp >NUL
    if errorlevel 1 goto :error

echo Info: clean up Qt 2. bring back only the ones we use
for %%x in (Core DBus Gui PrintSupport Svg Widgets) do call :qt_keep %%x

echo Info: clean up Qt 3. delete the Qt files we do not need
rmdir /s /q tmp
    if errorlevel 1 goto :error
rmdir /s /q Qt5\bin\tmp
    if errorlevel 1 goto :error

echo Info: clean up Qt 4. delete qml file
rmdir /s /q Qt5\qml
    if errorlevel 1 goto :error

echo Info: clean up Qt 5. delete translations file
rmdir /s /q Qt5\translations
    if errorlevel 1 goto :error

echo Info: clean up Qt 6. delete webengine files
if exist Qt5\bin\QtWebEngineProcess.exe (
    del Qt5\bin\QtWebEngineProcess.exe >NUL
        if errorlevel 1 goto :error
    del Qt5\resources\qtwebengine*.pak >NUL
        if errorlevel 1 goto :error
)

echo Info: clean up Qt 7. remove .sip files
del /s *.sip

echo on
echo Info: clean up python lib 1. delete test code
rmdir /s /q %DIST_DIR%\PyWinAppRes\Lib\ctypes\test
    if errorlevel 1 goto :error

popd

echo Info: build-windows completed successfully
exit /b 0
endlocal

:qt_keep
    echo Info: Keeping Qt%1
    move tmp\Qt%1.pyd . >NUL
        if errorlevel 1 goto :error
    move Qt5\bin\tmp\Qt5%1.dll Qt5\bin >NUL
        if errorlevel 1 goto :error
    goto :eof

:error
    echo Error: build-windows failed
    exit /b 1
