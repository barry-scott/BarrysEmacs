nmake -f %BUILDER_CFG_PLATFORM%.mak PYTHON=%python% clean
if exist c:\unxutils\tee.exe (
    nmake -f %BUILDER_CFG_PLATFORM%.mak PYTHON=%python% build 2>&1 | c:\unxutils\tee build.log
) else (
    nmake -f %BUILDER_CFG_PLATFORM%.mak PYTHON=%python% build
)
