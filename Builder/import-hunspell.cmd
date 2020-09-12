@echo on
rem
rem import-hunspell.cmd
rem
setlocal EnableDelayedExpansion
rem hunspell source code:
rem   https://github.com/hunspell/hunspell/tags
rem
rem dictionaries for hunspell:
rem   https://sourceforge.net/projects/wordlist/files/speller/

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


if "%2" == "" goto :error
for %%d in (%2 %3 %4 %5 %6 %7 %8 %9) do call :add_dict %%d
goto :eof

:add_dict
    echo %1
    colour-print "<>info Info:<> Adding dictionary from %1"
    if exist tmp.dict rmdir /s /q tmp.dict
    mkdir tmp.dict
    py -3 -m zipfile --extract %1 tmp.dict
    cd tmp.dict
    for %%f in (*.dic *.aff) do call :rename_dict %%f
    cd ..
    exit /b 0

:rename_dict
    set name=%1
    set newname=%name:-large=%
    dir /b
    move %name% ..\%newname%
    exit /b 0

:error
    echo Error!
endlocal
