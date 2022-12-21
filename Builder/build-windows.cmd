@echo off
setlocal
if "%BUILDER_TOP_DIR%" == "" (
    echo Error: BUILDER_TOP_DIR is not set. Hint: run builder_custom_init.cmd
    goto :eof
)

if "%PYTHON%" == "" (
    echo Error: PYTHON is not set. Hint: fix builder_custom_init.cmd to set it
    goto :eof
)

if "%BUILDER_CFG_PLATFORM%" == "" (
    echo Error: BUILDER_CFG_PLATFORM is not set. Hint: fix builder_custom_init.cmd to set it
    goto :eof
)

"%PYTHON%" -m pip install --user --upgrade colour-text

if "%1" == "--no-venv" (
    shift
) else (
    call build-venv.cmd windows
)

set VPYTHON=%CD%\venv.tmp\Scripts\python.exe

set APPMODE=--gui
if "%1" == "--cli" (
    set APPMODE=--cli
    shift
)
if "%1" == "--gui" (
    set APPMODE=--gui
    shift
)

if "%1" == "--enable-debug" (
    set BUILD_OPT=--enable-debug
    shift
)

%VPYTHON% build_bemacs.py gui --colour --vcredist=k:\subversion %APPMODE% %BUILD_OPT% 2>&1 | "%PYTHON%" -u build_tee.py build.log

if "%1" == "--install" for %%f in (tmp\bemacs-*-setup.exe) do start /wait %%f
endlocal
