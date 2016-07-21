if "%BUILDER_TOP_DIR%" == "" (
    echo Error: BUILDER_TOP_DIR is not set. Hint: run builder_custom_init.cmd
    goto :eof
)
if "%BUILDER_CFG_PLATFORM%" == "" (
    echo Error: BUILDER_CFG_PLATFORM is not set. Hint: fix builder_custom_init.cmd to set it.
    goto :eof
)

if "%PYTHON%" == "" (
    echo Error: PYTHON is not set. Hint: fix builder_custom_init.cmd to set it.
    goto :eof
)

%PYTHON% -c "from PyQt5 import QtWidgets, QtGui, QtCore" >nul
if errorlevel 1 (
    echo Error: PyQt5 is not installed for %PYTHON%. Hint: install windows PyQt5
    goto :eof
)

%PYTHON% -c "from PyQt5 import Qsci" 2>nul
if errorlevel 1 (
    echo Error: QScintilla is not installed for %PYTHON%. Hint: %PYTHON% -m pip isntall QScintilla
    goto :eof
)

nmake -f %BUILDER_CFG_PLATFORM%.mak PYTHON=%python% clean
if exist c:\unxutils\tee.exe (
    nmake -f %BUILDER_CFG_PLATFORM%.mak PYTHON=%python% build 2>&1 | c:\unxutils\tee build.log
) else (
    nmake -f %BUILDER_CFG_PLATFORM%.mak PYTHON=%python% build
)
