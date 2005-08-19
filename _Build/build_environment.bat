@prompt Info:
set PYTHON_VERSION=22
set MSVC_VERSION=60

if "%MSVC_VERSION%" == "60" call "C:\Program Files\Microsoft Visual Studio\VC98\Bin\vcvars32.bat"
if "%MSVC_VERSION%" == "71" call "C:\Program Files\Microsoft Visual Studio .NET 2003\Vc7\bin\vcvars32.bat"
PATH c:\Python%PYTHON_VERSION%;%PATH%
