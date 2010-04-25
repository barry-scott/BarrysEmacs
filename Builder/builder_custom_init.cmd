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

set MEINC_INSTALLER_DIR=%BUILDER_TOP_DIR%\Imports\MEINC_Installer-7.1.1-py26-win32
set PYTHON=c:\python26\python
set PYTHON_VERSION=2.6
PATH c:\svn;%PATH%
