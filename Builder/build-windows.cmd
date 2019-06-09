@echo off
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

%PYTHON% build_bemacs.py --colour --vcredist=k:\subversion 2>&1 | py -3 -u build_tee.py build.log

if "%1" == "--install" for %%f in (tmp\bemacs-*-setup.exe) do start /wait %%f
