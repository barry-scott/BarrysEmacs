# Microsoft Developer Studio Generated NMAKE File, Format Version 4.20
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Dynamic-Link Library" 0x0102

!IF "$(CFG)" == ""
CFG=em_user_mfc_dll - Win32 Debug
!MESSAGE No configuration specified.  Defaulting to em_user_mfc_dll - Win32\
 Debug.
!ENDIF 

!IF "$(CFG)" != "em_user_mfc_dll - Win32 Release" && "$(CFG)" !=\
 "em_user_mfc_dll - Win32 Debug"
!MESSAGE Invalid configuration "$(CFG)" specified.
!MESSAGE You can specify a configuration when running NMAKE on this makefile
!MESSAGE by defining the macro CFG on the command line.  For example:
!MESSAGE 
!MESSAGE NMAKE /f "em_user_mfc_dll.mak" CFG="em_user_mfc_dll - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "em_user_mfc_dll - Win32 Release" (based on\
 "Win32 (x86) Dynamic-Link Library")
!MESSAGE "em_user_mfc_dll - Win32 Debug" (based on\
 "Win32 (x86) Dynamic-Link Library")
!MESSAGE 
!ERROR An invalid configuration is specified.
!ENDIF 

!IF "$(OS)" == "Windows_NT"
NULL=
!ELSE 
NULL=nul
!ENDIF 
################################################################################
# Begin Project
RSC=rc.exe
CPP=cl.exe
MTL=mktyplib.exe

!IF  "$(CFG)" == "em_user_mfc_dll - Win32 Release"

# PROP BASE Use_MFC 6
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Release"
# PROP BASE Intermediate_Dir "Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 6
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "Release"
# PROP Intermediate_Dir "Release"
# PROP Target_Dir ""
OUTDIR=.\Release
INTDIR=.\Release

ALL : "$(OUTDIR)\em_user_mfc_dll.dll"

CLEAN : 
	-@erase "$(INTDIR)\em_user_mfc_dll.obj"
	-@erase "$(INTDIR)\em_user_mfc_dll.pch"
	-@erase "$(INTDIR)\em_user_mfc_dll.res"
	-@erase "$(INTDIR)\StdAfx.obj"
	-@erase "$(OUTDIR)\em_user_mfc_dll.dll"
	-@erase "$(OUTDIR)\em_user_mfc_dll.exp"
	-@erase "$(OUTDIR)\em_user_mfc_dll.lib"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

# ADD BASE CPP /nologo /MD /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_WINDLL" /D "_AFXDLL" /D "_MBCS" /Yu"stdafx.h" /c
# ADD CPP /nologo /MD /W3 /GX /O2 /I "\work\emacsv7\editor" /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_WINDLL" /D "_AFXDLL" /D "_MBCS" /D "_USRDLL" /Yu"stdafx.h" /c
CPP_PROJ=/nologo /MD /W3 /GX /O2 /I "\work\emacsv7\editor" /D "WIN32" /D\
 "NDEBUG" /D "_WINDOWS" /D "_WINDLL" /D "_AFXDLL" /D "_MBCS" /D "_USRDLL"\
 /Fp"$(INTDIR)/em_user_mfc_dll.pch" /Yu"stdafx.h" /Fo"$(INTDIR)/" /c 
CPP_OBJS=.\Release/
CPP_SBRS=.\.
# ADD BASE MTL /nologo /D "NDEBUG" /win32
# ADD MTL /nologo /D "NDEBUG" /win32
MTL_PROJ=/nologo /D "NDEBUG" /win32 
# ADD BASE RSC /l 0x809 /d "NDEBUG" /d "_AFXDLL"
# ADD RSC /l 0x809 /d "NDEBUG" /d "_AFXDLL"
RSC_PROJ=/l 0x809 /fo"$(INTDIR)/em_user_mfc_dll.res" /d "NDEBUG" /d "_AFXDLL" 
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
BSC32_FLAGS=/nologo /o"$(OUTDIR)/em_user_mfc_dll.bsc" 
BSC32_SBRS= \
	
LINK32=link.exe
# ADD BASE LINK32 /nologo /subsystem:windows /dll /machine:I386
# ADD LINK32 /nologo /subsystem:windows /dll /machine:I386
LINK32_FLAGS=/nologo /subsystem:windows /dll /incremental:no\
 /pdb:"$(OUTDIR)/em_user_mfc_dll.pdb" /machine:I386 /def:".\em_user_mfc_dll.def"\
 /out:"$(OUTDIR)/em_user_mfc_dll.dll" /implib:"$(OUTDIR)/em_user_mfc_dll.lib" 
DEF_FILE= \
	".\em_user_mfc_dll.def"
LINK32_OBJS= \
	"$(INTDIR)\em_user_mfc_dll.obj" \
	"$(INTDIR)\em_user_mfc_dll.res" \
	"$(INTDIR)\StdAfx.obj"

"$(OUTDIR)\em_user_mfc_dll.dll" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

!ELSEIF  "$(CFG)" == "em_user_mfc_dll - Win32 Debug"

# PROP BASE Use_MFC 6
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Debug"
# PROP BASE Intermediate_Dir "Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 6
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "Debug"
# PROP Intermediate_Dir "Debug"
# PROP Target_Dir ""
OUTDIR=.\Debug
INTDIR=.\Debug

ALL : "$(OUTDIR)\em_user_mfc_dll.dll"

CLEAN : 
	-@erase "$(INTDIR)\em_user_mfc_dll.obj"
	-@erase "$(INTDIR)\em_user_mfc_dll.pch"
	-@erase "$(INTDIR)\em_user_mfc_dll.res"
	-@erase "$(INTDIR)\StdAfx.obj"
	-@erase "$(INTDIR)\vc40.idb"
	-@erase "$(INTDIR)\vc40.pdb"
	-@erase "$(OUTDIR)\em_user_mfc_dll.dll"
	-@erase "$(OUTDIR)\em_user_mfc_dll.exp"
	-@erase "$(OUTDIR)\em_user_mfc_dll.ilk"
	-@erase "$(OUTDIR)\em_user_mfc_dll.lib"
	-@erase "$(OUTDIR)\em_user_mfc_dll.pdb"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

# ADD BASE CPP /nologo /MDd /W3 /Gm /GX /Zi /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_WINDLL" /D "_AFXDLL" /D "_MBCS" /Yu"stdafx.h" /c
# ADD CPP /nologo /MDd /W3 /Gm /GX /Zi /Od /I "\work\emacsv7\editor" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_WINDLL" /D "_AFXDLL" /D "_MBCS" /D "_USRDLL" /Yu"stdafx.h" /c
CPP_PROJ=/nologo /MDd /W3 /Gm /GX /Zi /Od /I "\work\emacsv7\editor" /D "WIN32"\
 /D "_DEBUG" /D "_WINDOWS" /D "_WINDLL" /D "_AFXDLL" /D "_MBCS" /D "_USRDLL"\
 /Fp"$(INTDIR)/em_user_mfc_dll.pch" /Yu"stdafx.h" /Fo"$(INTDIR)/"\
 /Fd"$(INTDIR)/" /c 
CPP_OBJS=.\Debug/
CPP_SBRS=.\.
# ADD BASE MTL /nologo /D "_DEBUG" /win32
# ADD MTL /nologo /D "_DEBUG" /win32
MTL_PROJ=/nologo /D "_DEBUG" /win32 
# ADD BASE RSC /l 0x809 /d "_DEBUG" /d "_AFXDLL"
# ADD RSC /l 0x809 /d "_DEBUG" /d "_AFXDLL"
RSC_PROJ=/l 0x809 /fo"$(INTDIR)/em_user_mfc_dll.res" /d "_DEBUG" /d "_AFXDLL" 
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
BSC32_FLAGS=/nologo /o"$(OUTDIR)/em_user_mfc_dll.bsc" 
BSC32_SBRS= \
	
LINK32=link.exe
# ADD BASE LINK32 /nologo /subsystem:windows /dll /debug /machine:I386
# ADD LINK32 /nologo /subsystem:windows /dll /debug /machine:I386
LINK32_FLAGS=/nologo /subsystem:windows /dll /incremental:yes\
 /pdb:"$(OUTDIR)/em_user_mfc_dll.pdb" /debug /machine:I386\
 /def:".\em_user_mfc_dll.def" /out:"$(OUTDIR)/em_user_mfc_dll.dll"\
 /implib:"$(OUTDIR)/em_user_mfc_dll.lib" 
DEF_FILE= \
	".\em_user_mfc_dll.def"
LINK32_OBJS= \
	"$(INTDIR)\em_user_mfc_dll.obj" \
	"$(INTDIR)\em_user_mfc_dll.res" \
	"$(INTDIR)\StdAfx.obj"

"$(OUTDIR)\em_user_mfc_dll.dll" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

!ENDIF 

.c{$(CPP_OBJS)}.obj:
   $(CPP) $(CPP_PROJ) $<  

.cpp{$(CPP_OBJS)}.obj:
   $(CPP) $(CPP_PROJ) $<  

.cxx{$(CPP_OBJS)}.obj:
   $(CPP) $(CPP_PROJ) $<  

.c{$(CPP_SBRS)}.sbr:
   $(CPP) $(CPP_PROJ) $<  

.cpp{$(CPP_SBRS)}.sbr:
   $(CPP) $(CPP_PROJ) $<  

.cxx{$(CPP_SBRS)}.sbr:
   $(CPP) $(CPP_PROJ) $<  

################################################################################
# Begin Target

# Name "em_user_mfc_dll - Win32 Release"
# Name "em_user_mfc_dll - Win32 Debug"

!IF  "$(CFG)" == "em_user_mfc_dll - Win32 Release"

!ELSEIF  "$(CFG)" == "em_user_mfc_dll - Win32 Debug"

!ENDIF 

################################################################################
# Begin Source File

SOURCE=.\ReadMe.txt

!IF  "$(CFG)" == "em_user_mfc_dll - Win32 Release"

!ELSEIF  "$(CFG)" == "em_user_mfc_dll - Win32 Debug"

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\em_user_mfc_dll.cpp
DEP_CPP_EM_US=\
	".\em_user_mfc_dll.h"\
	".\StdAfx.h"\
	"\work\emacsv7\editor\em_user.h"\
	

"$(INTDIR)\em_user_mfc_dll.obj" : $(SOURCE) $(DEP_CPP_EM_US) "$(INTDIR)"\
 "$(INTDIR)\em_user_mfc_dll.pch"


# End Source File
################################################################################
# Begin Source File

SOURCE=.\em_user_mfc_dll.def

!IF  "$(CFG)" == "em_user_mfc_dll - Win32 Release"

!ELSEIF  "$(CFG)" == "em_user_mfc_dll - Win32 Debug"

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\StdAfx.cpp
DEP_CPP_STDAF=\
	".\StdAfx.h"\
	

!IF  "$(CFG)" == "em_user_mfc_dll - Win32 Release"

# ADD CPP /Yc"stdafx.h"

BuildCmds= \
	$(CPP) /nologo /MD /W3 /GX /O2 /I "\work\emacsv7\editor" /D "WIN32" /D\
 "NDEBUG" /D "_WINDOWS" /D "_WINDLL" /D "_AFXDLL" /D "_MBCS" /D "_USRDLL"\
 /Fp"$(INTDIR)/em_user_mfc_dll.pch" /Yc"stdafx.h" /Fo"$(INTDIR)/" /c $(SOURCE) \
	

"$(INTDIR)\StdAfx.obj" : $(SOURCE) $(DEP_CPP_STDAF) "$(INTDIR)"
   $(BuildCmds)

"$(INTDIR)\em_user_mfc_dll.pch" : $(SOURCE) $(DEP_CPP_STDAF) "$(INTDIR)"
   $(BuildCmds)

!ELSEIF  "$(CFG)" == "em_user_mfc_dll - Win32 Debug"

# ADD CPP /Yc"stdafx.h"

BuildCmds= \
	$(CPP) /nologo /MDd /W3 /Gm /GX /Zi /Od /I "\work\emacsv7\editor" /D "WIN32"\
 /D "_DEBUG" /D "_WINDOWS" /D "_WINDLL" /D "_AFXDLL" /D "_MBCS" /D "_USRDLL"\
 /Fp"$(INTDIR)/em_user_mfc_dll.pch" /Yc"stdafx.h" /Fo"$(INTDIR)/"\
 /Fd"$(INTDIR)/" /c $(SOURCE) \
	

"$(INTDIR)\StdAfx.obj" : $(SOURCE) $(DEP_CPP_STDAF) "$(INTDIR)"
   $(BuildCmds)

"$(INTDIR)\em_user_mfc_dll.pch" : $(SOURCE) $(DEP_CPP_STDAF) "$(INTDIR)"
   $(BuildCmds)

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\em_user_mfc_dll.rc
DEP_RSC_EM_USE=\
	".\res\em_user_mfc_dll.rc2"\
	

"$(INTDIR)\em_user_mfc_dll.res" : $(SOURCE) $(DEP_RSC_EM_USE) "$(INTDIR)"
   $(RSC) $(RSC_PROJ) $(SOURCE)


# End Source File
# End Target
# End Project
################################################################################
