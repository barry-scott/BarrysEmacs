@set __e=%1
@if "%1" == "" set __e=off
@echo %__e%
rem builder_custom_init
set VC_VER=14.0
set BUILDER_CFG_PLATFORM=Win64
set BUILDER_CFG_BUILD_TYPE=Release
set PYTHON_VERSION=3.7
set PYTHON_FILE_VERSION=37

if %BUILDER_CFG_PLATFORM% == Win64 (
    rem if Win64 then setup path to include the 64bit CL.exe
    rem when called in place this can fork bomb (lots of CMD.exe in task manager)
    if exist "C:\Program Files (x86)\Microsoft Visual Studio %VC_VER%\VC\bin\amd64\vcvars64.bat" (
        call "C:\Program Files (x86)\Microsoft Visual Studio %VC_VER%\VC\bin\amd64\vcvars64.bat"
    )
    if exist "c:\Program Files (x86)\Microsoft Visual Studio\2017\Community\VC\Auxiliary\Build\vcvars64.bat" (
        call "c:\Program Files (x86)\Microsoft Visual Studio\2017\Community\VC\Auxiliary\Build\vcvars64.bat"
    )
    @echo %__e%
)

for /f "usebackq" %%X in (`py -%PYTHON_VERSION%-64 -c "import sys;print(sys.executable)"`) do set PYTHON=%%X
echo Python %PYTHON_VERSION% found in %PYTHON%
%PYTHON% -c "import sys;print( 'Python:', sys.version )"
