@set __e=%1
@if "%1" == "" set __e=on
@echo %__e%
rem builder_custom_init
set BUILDER_CFG_PLATFORM=Win32
set BUILDER_CFG_BUILD_TYPE=Release
set MSVC_VERSION=60

if %MSVC_VERSION% == 71 (
    call "C:\Program Files\Microsoft Visual Studio .NET 2003\Common7\Tools\vsvars32.bat"
    @echo %__e%
    call "C:\Program Files\Microsoft Visual C++ Toolkit 2003\vcvars32.bat"
    @echo %__e%
    call "c:\Program Files\Microsoft SDK\SetEnv.Bat" /XP32 /RETAIL
    @echo %__e%
)
if %MSVC_VERSION% == 60 (
    @call "c:\Program Files\Microsoft Visual Studio\VC98\Bin\VCVARS32.BAT"
    @echo %__e%
    @call "c:\Program Files\Microsoft SDK\SetEnv.Bat" /XP32 /RETAIL
    @echo %__e%
)
PATH c:\svn;%PATH%
