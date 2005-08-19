@echo off

pushd %0\..\..
for /f %%i in ('cd') do set __WD=%%i
cd _build

set SavedPrompt=%PROMPT%

echo Setting up EmacsV7 build environment
echo Working dir %__WD%
call build_environment.bat %__WD% Release Win32

@prompt %SavedPrompt%
@popd
