@set __e=%1
@if "%1" == "" set __e=off
@echo %__e%
rem builder_custom_init
set VC_VER=14.0
set BUILDER_CFG_PLATFORM=Win64
set BUILDER_CFG_BUILD_TYPE=Release
set PYTHON_VERSION=3.11

rem if Win64 then setup path to include the 64bit CL.exe
rem when called in place this can fork bomb (lots of CMD.exe in task manager)
if exist "C:\Program Files\Microsoft Visual Studio\2022\Community\VC\Auxiliary\Build\vcvars64.bat" (
    call "C:\Program Files\Microsoft Visual Studio\2022\Community\VC\Auxiliary\Build\vcvars64.bat"

) else if exist "c:\Program Files (x86)\Microsoft Visual Studio\2019\Community\VC\Auxiliary\Build\vcvars64.bat" (
    call "c:\Program Files (x86)\Microsoft Visual Studio\2019\Community\VC\Auxiliary\Build\vcvars64.bat"

) else if exist "c:\Program Files (x86)\Microsoft Visual Studio\2017\Community\VC\Auxiliary\Build\vcvars64.bat" (
    call "c:\Program Files (x86)\Microsoft Visual Studio\2017\Community\VC\Auxiliary\Build\vcvars64.bat"

) else if exist "C:\Program Files (x86)\Microsoft Visual Studio %VC_VER%\VC\bin\amd64\vcvars64.bat" (
    call "C:\Program Files (x86)\Microsoft Visual Studio %VC_VER%\VC\bin\amd64\vcvars64.bat"

) else (
    echo Error: Cannot find C++ compiler tools
    pause
)
@echo %__e%

py -%PYTHON_VERSION%-64 -c "import sys;print('set PYTHON=' + sys.executable)" >%TEMP%\__tmp__.cmd
call %TEMP%\__tmp__.cmd
del %TEMP%\__tmp__.cmd

echo Python %PYTHON_VERSION% found in %PYTHON%
"%PYTHON%" -c "import sys;print( 'Python:', sys.version )"

