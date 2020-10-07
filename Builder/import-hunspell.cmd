@echo off
rem
rem import-hunspell.cmd
rem
setlocal EnableDelayedExpansion
rem hunspell source code:
rem   https://github.com/hunspell/hunspell/tags
rem
rem dictionaries for hunspell:
rem   https://sourceforge.net/projects/wordlist/files/speller/

if "%1" == "" (
    colour-print "<>error Error:<> too few args"
    echo "Example: import-hunspell.cmd k:\BEmacs\dependencies\hunspell-1.7.0.tar.gz k:\BEmacs\dependencies\*.zip"
    exit /b 1
)

colour-print "<>info Info:<> install hunspell source"
if not exist ..\Imports mkdir ..\Imports
if exist ..\Imports\hunspell rmdir /s /q ..\Imports\hunspell

if "%1" == "" goto error

cd ../Imports
py -3 -m tarfile --extract %1 .
move hunspell-* hunspell

cd hunspell
copy src\hunspell\hunvisapi.h.in src\hunspell\hunvisapi.h

colour-print "<>info Info:<> installing dictionary kits"

if exist tmp.dict rmdir /s /q tmp.dict
mkdir tmp.dict

if "%2" == "" goto :error
for %%d in (%2 %3 %4 %5 %6 %7 %8 %9) do call :add_dict %%d
cd tmp.dict
for %%f in (*.dic *.aff) do call :rename_dict %%f
cd ..
dir /b tmp.dict
goto :eof


:add_dict
    colour-print "<>info Info:<> Adding dictionary from %1"
    py -3 -m zipfile --extract %1 tmp.dict
    exit /b 0

:rename_dict
    set name=%1
    set newname=%name:-large=%
    move %name% %newname% >nul
    exit /b 0

:error
    echo Error!
endlocal
