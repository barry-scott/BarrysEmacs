@echo off
setlocal
for %%f in (tmp\bemacs-*-setup.exe) do set KIT=%%f
dir %KIT%
copy %KIT% k:\BEmacs\beta
endlocal
