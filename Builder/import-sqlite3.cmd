@echo off
rem
rem import-sqlite3.cmd
rem
setlocal
if "%1" == "" (
    colour-print "<>error Error:<> too few args"
    echo "Example: import-sqlite3.cmd k:\BEmacs\dependencies\sqlite-amalgamation-3280000.zip"
    exit /b 1
)

colour-print "<>info Info:<> install sqlite3 source"

rmdir /s /q ..\Imports\sqlite
py -3 -m zipfile --extract %1 ..\Imports
cd ..\Imports
for /d %%d in (sqlite*) do move %%d sqlite
endlocal
