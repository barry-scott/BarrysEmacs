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

if exist c:\unxutils\usr\local\wbin\tee.exe (
    %PYTHON% build_bemacs.py --colour 2>&1 | c:\unxutils\usr\local\wbin\tee.exe build.log
) else (
    %PYTHON% build_bemacs.py --colour
)

if "%1" == "--install" for %%f in (tmp\bemacs-*-setup.exe) do start /wait %%f
