# Microsoft Developer Studio Project File - Name="example" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Dynamic-Link Library" 0x0102

CFG=example - Win32 Release
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "example_py21.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "example_py21.mak" CFG="example - Win32 Release"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "example - Win32 Debug" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "example - Win32 Release" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "example - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Release"
# PROP BASE Intermediate_Dir "Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "pyds21"
# PROP Intermediate_Dir "Debug21"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /YX /FD /c
# ADD CPP /nologo /MDd /W3 /GR /GX /Zi /Od /I "C:\Python21\include" /I "." /I "Demo" /D "WIN32" /D "_WINDOWS" /D "DEBUG" /D "_DEBUG" /FR /YX /FD /c /Tp
# ADD BASE MTL /nologo /mktyplib203 /o "NUL" /win32
# ADD MTL /nologo /mktyplib203 /o "NUL" /win32
# ADD BASE RSC /l 0x409
# ADD RSC /l 0x409
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /dll /machine:I386
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib python21_d.lib /nologo /subsystem:windows /dll /pdb:"sept" /debug /machine:I386 /out:"pyds21\example_d.pyd" /libpath:"c:\python21\libs" /export:initexample
# SUBTRACT LINK32 /pdb:none

!ELSEIF  "$(CFG)" == "example - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Release"
# PROP BASE Intermediate_Dir "Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "pyds21"
# PROP Intermediate_Dir "Release21"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /YX /FD /c
# ADD CPP /nologo /MD /W3 /GR /GX /Zi /Od /Ob2 /I "c:\python21\include" /I "." /I "Demo" /D "WIN32" /D "_WINDOWS" /D "NDEBUG" /D "noPY_WIN32_DELAYLOAD_PYTHON_DLL" /FR /YX /FD /c /Tp
# ADD BASE MTL /nologo /mktyplib203 /o "NUL" /win32
# ADD MTL /nologo /mktyplib203 /o "NUL" /win32
# ADD BASE RSC /l 0x409
# ADD RSC /l 0x409
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /dll /machine:I386
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib python21.lib /nologo /subsystem:windows /dll /debug /machine:I386 /out:"pyds21\example.pyd" /libpath:"c:\python21\libs" /export:initexample

!ENDIF 

# Begin Target

# Name "example - Win32 Debug"
# Name "example - Win32 Release"
# Begin Source File

SOURCE=.\Src\cxx_extensions.cxx
# End Source File
# Begin Source File

SOURCE=Src\cxxextensions.c
# End Source File
# Begin Source File

SOURCE=Src\cxxsupport.cxx
# End Source File
# Begin Source File

SOURCE=Demo\example.cxx
# End Source File
# Begin Source File

SOURCE=.\Src\IndirectPythonInterface.cxx
# End Source File
# Begin Source File

SOURCE=Demo\range.cxx
# End Source File
# Begin Source File

SOURCE=Demo\rangetest.cxx
# End Source File
# End Target
# End Project
