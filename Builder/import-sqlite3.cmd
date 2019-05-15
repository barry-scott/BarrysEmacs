setlocal
rmdir /s /q ..\Imports\sqlite
mkdir ..\Imports\sqlite
c:\unxutils\usr\local\wbin\unzip -j %1 -d ..\Imports\sqlite
endlocal
