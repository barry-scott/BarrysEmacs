# Microsoft Developer Studio Project File - Name="db_rtl" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Static Library" 0x0104

CFG=db_rtl - Win32 Release
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "db_rtl.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "db_rtl.mak" CFG="db_rtl - Win32 Release"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "db_rtl - Win32 Release" (based on "Win32 (x86) Static Library")
!MESSAGE "db_rtl - Win32 Debug" (based on "Win32 (x86) Static Library")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""$/EmacsV7/Editor", ZWAAAAAA"
# PROP Scc_LocalPath ".."
CPP=cl.exe
RSC=rc.exe

!IF  "$(CFG)" == "db_rtl - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir ".\Release"
# PROP BASE Intermediate_Dir ".\Release"
# PROP BASE Target_Dir "."
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir ".\Release"
# PROP Intermediate_Dir ".\Release"
# PROP Target_Dir "."
# ADD BASE CPP /nologo /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /YX /c
# ADD CPP /nologo /W3 /GX /O2 /I "..\..\Editor" /D "NDEBUG" /D "WIN32" /D "_CONSOLE" /D "_NT" /D "__has_bool__" /YX /FD /c
# ADD BASE RSC /l 0x809
# ADD RSC /l 0x809
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo

!ELSEIF  "$(CFG)" == "db_rtl - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir ".\Debug"
# PROP BASE Intermediate_Dir ".\Debug"
# PROP BASE Target_Dir "."
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir ".\Debug"
# PROP Intermediate_Dir ".\Debug"
# PROP Target_Dir "."
# ADD BASE CPP /nologo /W3 /GX /Z7 /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /YX /c
# ADD CPP /nologo /W3 /GX /Z7 /Od /I "..\..\Editor" /D "_DEBUG" /D "WIN32" /D "_CONSOLE" /D "_NT" /D "__has_bool__" /FD /c
# SUBTRACT CPP /YX
# ADD BASE RSC /l 0x809
# ADD RSC /l 0x809
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo

!ENDIF 

# Begin Target

# Name "db_rtl - Win32 Release"
# Name "db_rtl - Win32 Debug"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;hpj;bat;for;f90"
# Begin Source File

SOURCE=..\DB_RTL.CPP
# End Source File
# Begin Source File

SOURCE=..\DOPRINT.CPP
# End Source File
# Begin Source File

SOURCE=..\EM_STAT.CPP
# End Source File
# Begin Source File

SOURCE=..\EMSTRING.CPP
# End Source File
# Begin Source File

SOURCE=..\file_name_compare.cpp
# End Source File
# Begin Source File

SOURCE=..\NDBM.CPP
# End Source File
# Begin Source File

SOURCE=..\stub_rtl.cpp
# End Source File
# Begin Source File

SOURCE=..\WIN_FILE.CPP
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl;fi;fd"
# Begin Source File

SOURCE=..\EM_GEN.H
# End Source File
# Begin Source File

SOURCE=..\em_stat.h
# End Source File
# Begin Source File

SOURCE=..\emacsutl.h
# End Source File
# Begin Source File

SOURCE=..\emexcept.h
# End Source File
# Begin Source File

SOURCE=..\EMOBJECT.H
# End Source File
# Begin Source File

SOURCE=..\emstring.h
# End Source File
# Begin Source File

SOURCE=..\emstrtab.h
# End Source File
# Begin Source File

SOURCE=..\FILESERV.H
# End Source File
# Begin Source File

SOURCE=..\ndbm.h
# End Source File
# Begin Source File

SOURCE=..\OS.H
# End Source File
# Begin Source File

SOURCE=..\OS_WIN.H
# End Source File
# Begin Source File

SOURCE=..\WIN_INCL.H
# End Source File
# End Group
# Begin Group "Resource Files"

# PROP Default_Filter "ico;cur;bmp;dlg;rc2;rct;bin;cnt;rtf;gif;jpg;jpeg;jpe"
# End Group
# End Target
# End Project
