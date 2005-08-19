# Microsoft Visual C++ Generated NMAKE File, Format Version 2.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Console Application" 0x0103

!IF "$(CFG)" == ""
CFG=Win32 Debug
!MESSAGE No configuration specified.  Defaulting to Win32 Debug.
!ENDIF 

!IF "$(CFG)" != "Win32 Release" && "$(CFG)" != "Win32 Debug"
!MESSAGE Invalid configuration "$(CFG)" specified.
!MESSAGE You can specify a configuration when running NMAKE on this makefile
!MESSAGE by defining the macro CFG on the command line.  For example:
!MESSAGE 
!MESSAGE NMAKE /f "mll-2-db.mak" CFG="Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "Win32 Release" (based on "Win32 (x86) Console Application")
!MESSAGE "Win32 Debug" (based on "Win32 (x86) Console Application")
!MESSAGE 
!ERROR An invalid configuration is specified.
!ENDIF 

################################################################################
# Begin Project
# PROP Target_Last_Scanned "Win32 Debug"
CPP=cl.exe
RSC=rc.exe

!IF  "$(CFG)" == "Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "WinRel"
# PROP BASE Intermediate_Dir "WinRel"
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "..\..\winnt.rel"
# PROP Intermediate_Dir "..\..\winnt.rel"
OUTDIR=..\..\winnt.rel
INTDIR=..\..\winnt.rel

ALL : $(OUTDIR)\mll-2-db.exe $(OUTDIR)\mll-2-db.bsc

$(OUTDIR) : 
    if not exist $(OUTDIR)\nul mkdir $(OUTDIR)

# ADD BASE CPP /nologo /W3 /GX /YX /O2 /D "WIN32" /D "NDEBUG" /D "_CONSOLE" /FR /c
# ADD CPP /nologo /W4 /GX /O2 /I "." /D "NDEBUG" /D "WIN32" /D "_CONSOLE" /D "_NT" /FR /c /Tp
# SUBTRACT CPP /YX
CPP_PROJ=/nologo /W4 /GX /O2 /I "." /D "NDEBUG" /D "WIN32" /D "_CONSOLE" /D\
 "_NT" /FR$(INTDIR)\ /Fo$(INTDIR)\ /c /Tp 
CPP_OBJS=.\..\..\winnt.rel/
# ADD BASE RSC /l 0x409 /d "NDEBUG"
# ADD RSC /l 0x409 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
BSC32_FLAGS=/nologo /o$(OUTDIR)\mll-2-db.bsc 
BSC32_SBRS= \
	$(INTDIR)\ndbm.sbr \
	$(INTDIR)\db_rtl.sbr \
	$(INTDIR)\mll-2-db.sbr

$(OUTDIR)\mll-2-db.bsc : $(OUTDIR)  $(BSC32_SBRS)
    $(BSC32) @<<
  $(BSC32_FLAGS) $(BSC32_SBRS)
<<

LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib /NOLOGO /SUBSYSTEM:console /MACHINE:I386
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib /NOLOGO /SUBSYSTEM:console /MACHINE:I386
LINK32_FLAGS=kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib\
 advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib /NOLOGO\
 /SUBSYSTEM:console /INCREMENTAL:no /PDB:$(OUTDIR)\mll-2-db.pdb /MACHINE:I386\
 /OUT:$(OUTDIR)\mll-2-db.exe 
DEF_FILE=
LINK32_OBJS= \
	$(INTDIR)\NDBM.OBJ \
	$(INTDIR)\DB_RTL.OBJ \
	$(INTDIR)\mll-2-db.OBJ

$(OUTDIR)\mll-2-db.exe : $(OUTDIR)  $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

!ELSEIF  "$(CFG)" == "Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "WinDebug"
# PROP BASE Intermediate_Dir "WinDebug"
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "..\..\winnt.dbg"
# PROP Intermediate_Dir "..\..\winnt.dbg"
OUTDIR=..\..\winnt.dbg
INTDIR=..\..\winnt.dbg

ALL : $(OUTDIR)\mll-2-db.exe $(OUTDIR)\mll-2-db.bsc

$(OUTDIR) : 
    if not exist $(OUTDIR)\nul mkdir $(OUTDIR)

# ADD BASE CPP /nologo /W3 /GX /Zi /YX /Od /D "WIN32" /D "_DEBUG" /D "_CONSOLE" /FR /c
# ADD CPP /nologo /W4 /GX /Zi /Od /I "." /D "_DEBUG" /D "WIN32" /D "_CONSOLE" /D "_NT" /FR /c /Tp
# SUBTRACT CPP /YX
CPP_PROJ=/nologo /W4 /GX /Zi /Od /I "." /D "_DEBUG" /D "WIN32" /D "_CONSOLE" /D\
 "_NT" /FR$(INTDIR)\ /Fo$(INTDIR)\ /Fd$(OUTDIR)\mll-2-db.pdb /c /Tp 
CPP_OBJS=.\..\..\winnt.dbg/
# ADD BASE RSC /l 0x409 /d "_DEBUG"
# ADD RSC /l 0x409 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
BSC32_FLAGS=/nologo /o$(OUTDIR)\mll-2-db.bsc
BSC32_SBRS= \
	$(INTDIR)\ndbm.sbr \
	$(INTDIR)\db_rtl.sbr \
	$(INTDIR)\mll-2-db.sbr

$(OUTDIR)\mll-2-db.bsc : $(OUTDIR)  $(BSC32_SBRS)
    $(BSC32) @<<
  $(BSC32_FLAGS) $(BSC32_SBRS)
<<

LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib /NOLOGO /SUBSYSTEM:console /DEBUG /MACHINE:I386
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib /NOLOGO /SUBSYSTEM:console /DEBUG /MACHINE:I386
LINK32_FLAGS=kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib\
 advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib /NOLOGO\
 /SUBSYSTEM:console /INCREMENTAL:yes /PDB:$(OUTDIR)\mll-2-db.pdb /DEBUG\
 /MACHINE:I386 /OUT:$(OUTDIR)\mll-2-db.exe 
DEF_FILE=
LINK32_OBJS= \
	$(INTDIR)\NDBM.OBJ \
	$(INTDIR)\DB_RTL.OBJ \
	$(INTDIR)\mll-2-db.obj

$(OUTDIR)\mll-2-db.exe : $(OUTDIR)  $(DEF_FILE) $(LINK32_OBJS)
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

################################################################################
# Begin Group "Source Files"

################################################################################
# Begin Source File

SOURCE=.\NDBM.C
DEP_NDBM_=\
	.\emacs.h\
	.\os.h\
	.\em_gen.h\
	.\em_mac.h\
	.\queue.h\
	.\em_time.h\
	.\syntax.h\
	.\display.h\
	.\mlisp.h\
	.\fileio.h\
	.\keyboard.h\
	.\buffer.h\
	.\abbrev.h\
	.\ndbm.h\
	.\tt.h\
	.\window.h\
	.\undo.h\
	.\journal.h\
	.\fileserv.h\
	.\em_rtn.h\
	.\em_var.h\
	.\os_win.h\
	.\os_mac.h

$(INTDIR)\NDBM.OBJ :  $(SOURCE)  $(DEP_NDBM_) $(INTDIR)

# End Source File
################################################################################
# Begin Source File

SOURCE=.\DB_RTL.C

$(INTDIR)\DB_RTL.OBJ :  $(SOURCE)  $(INTDIR)

# End Source File
################################################################################
# Begin Source File

SOURCE=.\mll-2-db.C
DEP_MLL_2=\
	.\ndbm.h

$(INTDIR)\mll-2-db.OBJ :  $(SOURCE)  $(DEP_MLL_2) $(INTDIR)

# End Source File
# End Group
# End Project
################################################################################
