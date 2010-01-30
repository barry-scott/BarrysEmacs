@set __e=%1
@if "%1" == "" set __e=on
@echo %__e%
rem builder_custom_init
set BUILDER_CFG_PLATFORM=Win32
set BUILDER_CFG_BUILD_TYPE=Release
set MSVC_VERSION=90

if %MSVC_VERSION% == 90 (
    call "%VS90COMNTOOLS%vsvars32.bat"
    @echo %__e%
)
PATH c:\svn;%PATH%
