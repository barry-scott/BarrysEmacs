# Microsoft Visual C++ Generated NMAKE File, Format Version 2.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Application" 0x0101

!IF "$(CFG)" == ""
CFG=Win32 Release
!MESSAGE No configuration specified.  Defaulting to Win32 Release.
!ENDIF 

!IF "$(CFG)" != "Win32 Release" && "$(CFG)" != "Win32 Debug"
!MESSAGE Invalid configuration "$(CFG)" specified.
!MESSAGE You can specify a configuration when running NMAKE on this makefile
!MESSAGE by defining the macro CFG on the command line.  For example:
!MESSAGE 
!MESSAGE NMAKE /f "EMWINNT.MAK" CFG="Win32 Release"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "Win32 Release" (based on "Win32 (x86) Application")
!MESSAGE "Win32 Debug" (based on "Win32 (x86) Application")
!MESSAGE 
!ERROR An invalid configuration is specified.
!ENDIF 

################################################################################
# Begin Project
# PROP Target_Last_Scanned "Win32 Release"
MTL=MkTypLib.exe
CPP=cl.exe
RSC=rc.exe

!IF  "$(CFG)" == "Win32 Release"

# PROP BASE Use_MFC 1
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "WinRel"
# PROP BASE Intermediate_Dir "WinRel"
# PROP Use_MFC 1
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "u:\work\emacs\winnt.rel"
# PROP Intermediate_Dir "u:\work\emacs\winnt.rel"
OUTDIR=u:\work\emacs\winnt.rel
INTDIR=u:\work\emacs\winnt.rel

ALL : $(OUTDIR)/emwinnt.exe $(OUTDIR)/emwinnt.map $(OUTDIR)/emwinnt.bsc

$(OUTDIR) : 
    if not exist $(OUTDIR)/nul mkdir $(OUTDIR)

# ADD BASE MTL /nologo /D "NDEBUG" /win32
# ADD MTL /nologo /D "NDEBUG" /win32
# ADD BASE CPP /nologo /G3 /Zp4 /MT /W4 /GX /Zi /YX /O2 /I "u:\work\emacs\src\edit" /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_X86_" /D "_NT" /D "_MBCS" /FR /c /Tp
# ADD CPP /nologo /G3 /Zp4 /MT /W4 /GX /Zi /O2 /I "u:\work\emacs\src\edit" /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_X86_" /D "_NT" /D "_MBCS" /FR /c /Tp
CPP_PROJ=/nologo /G3 /Zp4 /MT /W4 /GX /Zi /O2 /I "u:\work\emacs\src\edit" /D\
 "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_X86_" /D "_NT" /D "_MBCS" /FR$(INTDIR)/\
 /Fo$(INTDIR)/ /Fd$(OUTDIR)/"emwinnt.pdb" /c /Tp 
CPP_OBJS=u:\work\emacs\winnt.rel/
# ADD BASE RSC /l 0x409 /d "NDEBUG" /d "_NT"
# ADD RSC /l 0x409 /d "NDEBUG" /d "_NT"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
BSC32_FLAGS=/nologo /o$(OUTDIR)/"emwinnt.bsc" 
BSC32_SBRS= \
	$(INTDIR)/FINDDLG.SBR \
	$(INTDIR)/mainfrm.sbr \
	$(INTDIR)/STDAFX.SBR \
	$(INTDIR)/SWITCHDL.SBR \
	$(INTDIR)/WIN_MENU.SBR \
	$(INTDIR)/win_opt.sbr \
	$(INTDIR)/WIN_RTL.SBR \
	$(INTDIR)/WIN_UI.SBR \
	$(INTDIR)/WINEMACS.SBR \
	$(INTDIR)/WINEMDOC.SBR \
	$(INTDIR)/winemvw.sbr \
	$(INTDIR)/ABBREV.SBR \
	$(INTDIR)/ABSPATH.SBR \
	$(INTDIR)/ARITH.SBR \
	$(INTDIR)/BUFFER.SBR \
	$(INTDIR)/CASECONV.SBR \
	$(INTDIR)/COLUMNS.SBR \
	$(INTDIR)/DBMAN.SBR \
	$(INTDIR)/DISPLAY.SBR \
	$(INTDIR)/EDOPRINT.SBR \
	$(INTDIR)/EM_TIME.SBR \
	$(INTDIR)/EMACS.SBR \
	$(INTDIR)/EMACSRTL.SBR \
	$(INTDIR)/ERRLOG.SBR \
	$(INTDIR)/FILEIO.SBR \
	$(INTDIR)/FILESERV.SBR \
	$(INTDIR)/FIO.SBR \
	$(INTDIR)/FUNCTION.SBR \
	$(INTDIR)/GLOB_VAR.SBR \
	$(INTDIR)/JOURNAL.SBR \
	$(INTDIR)/KEYBOARD.SBR \
	$(INTDIR)/LISPFUNC.SBR \
	$(INTDIR)/MACROS.SBR \
	$(INTDIR)/MEM_MAN.SBR \
	$(INTDIR)/METACOMM.SBR \
	$(INTDIR)/MINIBUF.SBR \
	$(INTDIR)/MLISPARS.SBR \
	$(INTDIR)/MLISPROC.SBR \
	$(INTDIR)/NDBM.SBR \
	$(INTDIR)/OPTIONS.SBR \
	$(INTDIR)/PROMPT.SBR \
	$(INTDIR)/QUEUE.SBR \
	$(INTDIR)/SEARCH.SBR \
	$(INTDIR)/SEARCHIN.SBR \
	$(INTDIR)/SIMPCOMM.SBR \
	$(INTDIR)/SUBPROC.SBR \
	$(INTDIR)/SYNTAX.SBR \
	$(INTDIR)/TERM.SBR \
	$(INTDIR)/TIMER.SBR \
	$(INTDIR)/UNDO.SBR \
	$(INTDIR)/VARIABLE.SBR \
	$(INTDIR)/VARTHUNK.SBR \
	$(INTDIR)/WINDMAN.SBR \
	$(INTDIR)/WINDOW.SBR \
	$(INTDIR)/NT_PROC.SBR \
	$(INTDIR)/NT_COMM.SBR \
	$(INTDIR)/NT_ASYNC.SBR \
	$(INTDIR)/NT_PIPE.SBR \
	$(INTDIR)/AAAA_MEM.SBR \
	$(INTDIR)/ZZZZ_MEM.SBR \
	$(INTDIR)/MLPRINTF.SBR \
	$(INTDIR)/version.sbr

$(OUTDIR)/emwinnt.bsc : $(OUTDIR)  $(BSC32_SBRS)
    $(BSC32) @<<
  $(BSC32_FLAGS) $(BSC32_SBRS)
<<

LINK32=link.exe
# ADD BASE LINK32 nafxcw.lib olecli32.lib olesvr32.lib /NOLOGO /VERSION:6,0 /STACK:0x100000 /SUBSYSTEM:windows /MAP /MACHINE:IX86 /OUT:"emwinnt"
# ADD LINK32 nafxcw.lib ole32.lib oleaut32.lib /NOLOGO /VERSION:6,0 /STACK:0x100000 /SUBSYSTEM:windows /MAP /MACHINE:IX86
# SUBTRACT LINK32 /INCREMENTAL:yes
LINK32_FLAGS=nafxcw.lib ole32.lib oleaut32.lib /NOLOGO /VERSION:6,0\
 /STACK:0x100000 /SUBSYSTEM:windows /INCREMENTAL:no /PDB:$(OUTDIR)/"emwinnt.pdb"\
 /MAP:$(INTDIR)/"emwinnt.map" /MACHINE:IX86 /OUT:$(OUTDIR)/"emwinnt.exe" 
DEF_FILE=
LINK32_OBJS= \
	$(INTDIR)/FINDDLG.OBJ \
	$(INTDIR)/mainfrm.obj \
	$(INTDIR)/STDAFX.OBJ \
	$(INTDIR)/SWITCHDL.OBJ \
	$(INTDIR)/WIN_MENU.OBJ \
	$(INTDIR)/win_opt.obj \
	$(INTDIR)/WIN_RTL.OBJ \
	$(INTDIR)/WIN_UI.OBJ \
	$(INTDIR)/WINEMACS.OBJ \
	$(INTDIR)/WINEMACS.res \
	$(INTDIR)/WINEMDOC.OBJ \
	$(INTDIR)/winemvw.obj \
	$(INTDIR)/ABBREV.OBJ \
	$(INTDIR)/ABSPATH.OBJ \
	$(INTDIR)/ARITH.OBJ \
	$(INTDIR)/BUFFER.OBJ \
	$(INTDIR)/CASECONV.OBJ \
	$(INTDIR)/COLUMNS.OBJ \
	$(INTDIR)/DBMAN.OBJ \
	$(INTDIR)/DISPLAY.OBJ \
	$(INTDIR)/EDOPRINT.OBJ \
	$(INTDIR)/EM_TIME.OBJ \
	$(INTDIR)/EMACS.OBJ \
	$(INTDIR)/EMACSRTL.OBJ \
	$(INTDIR)/ERRLOG.OBJ \
	$(INTDIR)/FILEIO.OBJ \
	$(INTDIR)/FILESERV.OBJ \
	$(INTDIR)/FIO.OBJ \
	$(INTDIR)/FUNCTION.OBJ \
	$(INTDIR)/GLOB_VAR.OBJ \
	$(INTDIR)/JOURNAL.OBJ \
	$(INTDIR)/KEYBOARD.OBJ \
	$(INTDIR)/LISPFUNC.OBJ \
	$(INTDIR)/MACROS.OBJ \
	$(INTDIR)/MEM_MAN.OBJ \
	$(INTDIR)/METACOMM.OBJ \
	$(INTDIR)/MINIBUF.OBJ \
	$(INTDIR)/MLISPARS.OBJ \
	$(INTDIR)/MLISPROC.OBJ \
	$(INTDIR)/NDBM.OBJ \
	$(INTDIR)/OPTIONS.OBJ \
	$(INTDIR)/PROMPT.OBJ \
	$(INTDIR)/QUEUE.OBJ \
	$(INTDIR)/SEARCH.OBJ \
	$(INTDIR)/SEARCHIN.OBJ \
	$(INTDIR)/SIMPCOMM.OBJ \
	$(INTDIR)/SUBPROC.OBJ \
	$(INTDIR)/SYNTAX.OBJ \
	$(INTDIR)/TERM.OBJ \
	$(INTDIR)/TIMER.OBJ \
	$(INTDIR)/UNDO.OBJ \
	$(INTDIR)/VARIABLE.OBJ \
	$(INTDIR)/VARTHUNK.OBJ \
	$(INTDIR)/WINDMAN.OBJ \
	$(INTDIR)/WINDOW.OBJ \
	$(INTDIR)/NT_PROC.OBJ \
	$(INTDIR)/NT_COMM.OBJ \
	$(INTDIR)/NT_ASYNC.OBJ \
	$(INTDIR)/NT_PIPE.OBJ \
	$(INTDIR)/AAAA_MEM.OBJ \
	$(INTDIR)/ZZZZ_MEM.OBJ \
	$(INTDIR)/MLPRINTF.OBJ \
	$(INTDIR)/version.obj

$(OUTDIR)/emwinnt.exe : $(OUTDIR)  $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

!ELSEIF  "$(CFG)" == "Win32 Debug"

# PROP BASE Use_MFC 1
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Win32_De"
# PROP BASE Intermediate_Dir "Win32_De"
# PROP Use_MFC 1
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "Win32_De"
# PROP Intermediate_Dir "Win32_De"
OUTDIR=.\Win32_De
INTDIR=.\Win32_De

ALL : \work\emacs\winnt.dbg\emwinnt.exe $(OUTDIR)/emwinnt.map\
 $(OUTDIR)/emwinnt.bsc

$(OUTDIR) : 
    if not exist $(OUTDIR)/nul mkdir $(OUTDIR)

# ADD BASE MTL /nologo /D "NDEBUG" /win32
# ADD MTL /nologo /D "NDEBUG" /win32
# ADD BASE CPP /nologo /G3 /Zp4 /MT /W4 /GX /Zi /O2 /I "u:\work\emacs\src\edit" /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_X86_" /D "_NT" /D "_MBCS" /FR /c /Tp
# ADD CPP /nologo /G3 /Zp4 /MT /W4 /GX /Zi /O2 /I "u:\work\emacs\src\edit" /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_X86_" /D "_NT" /D "_MBCS" /FR"u:\work\emacs\winnt.dbg/" /c /Tp
CPP_PROJ=/nologo /G3 /Zp4 /MT /W4 /GX /Zi /O2 /I "u:\work\emacs\src\edit" /D\
 "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_X86_" /D "_NT" /D "_MBCS"\
 /FR"u:\work\emacs\winnt.dbg/" /Fo$(INTDIR)/ /Fd$(OUTDIR)/"emwinnt.pdb" /c /Tp 
CPP_OBJS=.\Win32_De/
# ADD BASE RSC /l 0x409 /d "NDEBUG" /d "_NT"
# ADD RSC /l 0x409 /d "NDEBUG" /d "_NT"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
BSC32_FLAGS=/nologo /o$(OUTDIR)/"emwinnt.bsc" 
BSC32_SBRS= \
	\work\emacs\winnt.dbg\FINDDLG.SBR \
	\work\emacs\winnt.dbg\mainfrm.sbr \
	\work\emacs\winnt.dbg\STDAFX.SBR \
	\work\emacs\winnt.dbg\SWITCHDL.SBR \
	\work\emacs\winnt.dbg\WIN_MENU.SBR \
	\work\emacs\winnt.dbg\win_opt.sbr \
	\work\emacs\winnt.dbg\WIN_RTL.SBR \
	\work\emacs\winnt.dbg\WIN_UI.SBR \
	\work\emacs\winnt.dbg\WINEMACS.SBR \
	\work\emacs\winnt.dbg\WINEMDOC.SBR \
	\work\emacs\winnt.dbg\winemvw.sbr \
	\work\emacs\winnt.dbg\ABBREV.SBR \
	\work\emacs\winnt.dbg\ABSPATH.SBR \
	\work\emacs\winnt.dbg\ARITH.SBR \
	\work\emacs\winnt.dbg\BUFFER.SBR \
	\work\emacs\winnt.dbg\CASECONV.SBR \
	\work\emacs\winnt.dbg\COLUMNS.SBR \
	\work\emacs\winnt.dbg\DBMAN.SBR \
	\work\emacs\winnt.dbg\DISPLAY.SBR \
	\work\emacs\winnt.dbg\EDOPRINT.SBR \
	\work\emacs\winnt.dbg\EM_TIME.SBR \
	\work\emacs\winnt.dbg\EMACS.SBR \
	\work\emacs\winnt.dbg\EMACSRTL.SBR \
	\work\emacs\winnt.dbg\ERRLOG.SBR \
	\work\emacs\winnt.dbg\FILEIO.SBR \
	\work\emacs\winnt.dbg\FILESERV.SBR \
	\work\emacs\winnt.dbg\FIO.SBR \
	\work\emacs\winnt.dbg\FUNCTION.SBR \
	\work\emacs\winnt.dbg\GLOB_VAR.SBR \
	\work\emacs\winnt.dbg\JOURNAL.SBR \
	\work\emacs\winnt.dbg\KEYBOARD.SBR \
	\work\emacs\winnt.dbg\LISPFUNC.SBR \
	\work\emacs\winnt.dbg\MACROS.SBR \
	\work\emacs\winnt.dbg\MEM_MAN.SBR \
	\work\emacs\winnt.dbg\METACOMM.SBR \
	\work\emacs\winnt.dbg\MINIBUF.SBR \
	\work\emacs\winnt.dbg\MLISPARS.SBR \
	\work\emacs\winnt.dbg\MLISPROC.SBR \
	\work\emacs\winnt.dbg\NDBM.SBR \
	\work\emacs\winnt.dbg\OPTIONS.SBR \
	\work\emacs\winnt.dbg\PROMPT.SBR \
	\work\emacs\winnt.dbg\QUEUE.SBR \
	\work\emacs\winnt.dbg\SEARCH.SBR \
	\work\emacs\winnt.dbg\SEARCHIN.SBR \
	\work\emacs\winnt.dbg\SIMPCOMM.SBR \
	\work\emacs\winnt.dbg\SUBPROC.SBR \
	\work\emacs\winnt.dbg\SYNTAX.SBR \
	\work\emacs\winnt.dbg\TERM.SBR \
	\work\emacs\winnt.dbg\TIMER.SBR \
	\work\emacs\winnt.dbg\UNDO.SBR \
	\work\emacs\winnt.dbg\VARIABLE.SBR \
	\work\emacs\winnt.dbg\VARTHUNK.SBR \
	\work\emacs\winnt.dbg\WINDMAN.SBR \
	\work\emacs\winnt.dbg\WINDOW.SBR \
	\work\emacs\winnt.dbg\NT_PROC.SBR \
	\work\emacs\winnt.dbg\NT_COMM.SBR \
	\work\emacs\winnt.dbg\NT_ASYNC.SBR \
	\work\emacs\winnt.dbg\NT_PIPE.SBR \
	\work\emacs\winnt.dbg\AAAA_MEM.SBR \
	\work\emacs\winnt.dbg\ZZZZ_MEM.SBR \
	\work\emacs\winnt.dbg\MLPRINTF.SBR \
	\work\emacs\winnt.dbg\version.sbr

$(OUTDIR)/emwinnt.bsc : $(OUTDIR)  $(BSC32_SBRS)
    $(BSC32) @<<
  $(BSC32_FLAGS) $(BSC32_SBRS)
<<

LINK32=link.exe
# ADD BASE LINK32 nafxcw.lib ole32.lib oleaut32.lib /NOLOGO /VERSION:6,0 /STACK:0x100000 /SUBSYSTEM:windows /MAP /MACHINE:IX86
# SUBTRACT BASE LINK32 /INCREMENTAL:yes
# ADD LINK32 nafxcw.lib ole32.lib oleaut32.lib /NOLOGO /VERSION:6,0 /STACK:0x100000 /SUBSYSTEM:windows /MAP /MACHINE:IX86 /OUT:"u:\work\emacs\winnt.dbg/emwinnt.exe"
# SUBTRACT LINK32 /INCREMENTAL:yes
LINK32_FLAGS=nafxcw.lib ole32.lib oleaut32.lib /NOLOGO /VERSION:6,0\
 /STACK:0x100000 /SUBSYSTEM:windows /INCREMENTAL:no /PDB:$(OUTDIR)/"emwinnt.pdb"\
 /MAP:$(INTDIR)/"emwinnt.map" /MACHINE:IX86\
 /OUT:"u:\work\emacs\winnt.dbg/emwinnt.exe" 
DEF_FILE=
LINK32_OBJS= \
	$(INTDIR)/FINDDLG.OBJ \
	$(INTDIR)/mainfrm.obj \
	$(INTDIR)/STDAFX.OBJ \
	$(INTDIR)/SWITCHDL.OBJ \
	$(INTDIR)/WIN_MENU.OBJ \
	$(INTDIR)/win_opt.obj \
	$(INTDIR)/WIN_RTL.OBJ \
	$(INTDIR)/WIN_UI.OBJ \
	$(INTDIR)/WINEMACS.OBJ \
	$(INTDIR)/WINEMACS.res \
	$(INTDIR)/WINEMDOC.OBJ \
	$(INTDIR)/winemvw.obj \
	$(INTDIR)/ABBREV.OBJ \
	$(INTDIR)/ABSPATH.OBJ \
	$(INTDIR)/ARITH.OBJ \
	$(INTDIR)/BUFFER.OBJ \
	$(INTDIR)/CASECONV.OBJ \
	$(INTDIR)/COLUMNS.OBJ \
	$(INTDIR)/DBMAN.OBJ \
	$(INTDIR)/DISPLAY.OBJ \
	$(INTDIR)/EDOPRINT.OBJ \
	$(INTDIR)/EM_TIME.OBJ \
	$(INTDIR)/EMACS.OBJ \
	$(INTDIR)/EMACSRTL.OBJ \
	$(INTDIR)/ERRLOG.OBJ \
	$(INTDIR)/FILEIO.OBJ \
	$(INTDIR)/FILESERV.OBJ \
	$(INTDIR)/FIO.OBJ \
	$(INTDIR)/FUNCTION.OBJ \
	$(INTDIR)/GLOB_VAR.OBJ \
	$(INTDIR)/JOURNAL.OBJ \
	$(INTDIR)/KEYBOARD.OBJ \
	$(INTDIR)/LISPFUNC.OBJ \
	$(INTDIR)/MACROS.OBJ \
	$(INTDIR)/MEM_MAN.OBJ \
	$(INTDIR)/METACOMM.OBJ \
	$(INTDIR)/MINIBUF.OBJ \
	$(INTDIR)/MLISPARS.OBJ \
	$(INTDIR)/MLISPROC.OBJ \
	$(INTDIR)/NDBM.OBJ \
	$(INTDIR)/OPTIONS.OBJ \
	$(INTDIR)/PROMPT.OBJ \
	$(INTDIR)/QUEUE.OBJ \
	$(INTDIR)/SEARCH.OBJ \
	$(INTDIR)/SEARCHIN.OBJ \
	$(INTDIR)/SIMPCOMM.OBJ \
	$(INTDIR)/SUBPROC.OBJ \
	$(INTDIR)/SYNTAX.OBJ \
	$(INTDIR)/TERM.OBJ \
	$(INTDIR)/TIMER.OBJ \
	$(INTDIR)/UNDO.OBJ \
	$(INTDIR)/VARIABLE.OBJ \
	$(INTDIR)/VARTHUNK.OBJ \
	$(INTDIR)/WINDMAN.OBJ \
	$(INTDIR)/WINDOW.OBJ \
	$(INTDIR)/NT_PROC.OBJ \
	$(INTDIR)/NT_COMM.OBJ \
	$(INTDIR)/NT_ASYNC.OBJ \
	$(INTDIR)/NT_PIPE.OBJ \
	$(INTDIR)/AAAA_MEM.OBJ \
	$(INTDIR)/ZZZZ_MEM.OBJ \
	$(INTDIR)/MLPRINTF.OBJ \
	$(INTDIR)/version.obj

\work\emacs\winnt.dbg\emwinnt.exe : $(OUTDIR)  $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

!ENDIF 

MTL_PROJ=/nologo /D "NDEBUG" /win32 

.c{$(CPP_OBJS)}.obj:
   $(CPP) $(CPP_PROJ) $<  

.cpp{$(CPP_OBJS)}.obj:
   $(CPP) $(CPP_PROJ) $<  

.cxx{$(CPP_OBJS)}.obj:
   $(CPP) $(CPP_PROJ) $<  

RSC_PROJ=/l 0x409 /fo$(INTDIR)/"WINEMACS.res" /d "NDEBUG" /d "_NT" 
################################################################################
# Begin Group "Interface files"

################################################################################
# Begin Source File

SOURCE=.\FINDDLG.CPP
DEP_FINDD=\
	.\STDAFX.H\
	.\WINEMACS.H\
	.\FINDDLG.H\
	.\EMACS.H\
	.\OS.H\
	.\EM_GEN.H\
	.\EM_MAC.H\
	.\QUEUE.H\
	.\EM_TIME.H\
	.\SYNTAX.H\
	.\DISPLAY.H\
	.\MLISP.H\
	.\FILEIO.H\
	.\KEYBOARD.H\
	.\BUFFER.H\
	.\ABBREV.H\
	.\NDBM.H\
	.\TT.H\
	.\WINDOW.H\
	.\UNDO.H\
	.\JOURNAL.H\
	.\FILESERV.H\
	.\EM_RTN.H\
	.\EM_VAR.H\
	.\OS_WIN.H\
	.\OS_MAC.H

!IF  "$(CFG)" == "Win32 Release"

# ADD BASE CPP /Yu"STDAFX.H"
# ADD CPP /Yu"STDAFX.H"

$(INTDIR)/FINDDLG.OBJ :  $(SOURCE)  $(DEP_FINDD) $(INTDIR) $(INTDIR)/STDAFX.OBJ
   $(CPP) /nologo /G3 /Zp4 /MT /W4 /GX /Zi /O2 /I "u:\work\emacs\src\edit" /D\
 "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_X86_" /D "_NT" /D "_MBCS" /FR$(INTDIR)/\
 /Fp$(OUTDIR)/"emwinnt.pch" /Yu"STDAFX.H" /Fo$(INTDIR)/\
 /Fd$(OUTDIR)/"emwinnt.pdb" /c /Tp  $(SOURCE) 

!ELSEIF  "$(CFG)" == "Win32 Debug"

# ADD BASE CPP /Yu"STDAFX.H"
# ADD CPP /Yu"STDAFX.H"

$(INTDIR)/FINDDLG.OBJ :  $(SOURCE)  $(DEP_FINDD) $(INTDIR) $(INTDIR)/STDAFX.OBJ
   $(CPP) /nologo /G3 /Zp4 /MT /W4 /GX /Zi /O2 /I "u:\work\emacs\src\edit" /D\
 "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_X86_" /D "_NT" /D "_MBCS"\
 /FR"u:\work\emacs\winnt.dbg/" /Fp$(OUTDIR)/"emwinnt.pch" /Yu"STDAFX.H"\
 /Fo$(INTDIR)/ /Fd$(OUTDIR)/"emwinnt.pdb" /c /Tp  $(SOURCE) 

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\mainfrm.cpp
DEP_MAINF=\
	.\STDAFX.H\
	.\WINEMACS.H\
	.\mainfrm.h\
	.\WINEMDOC.H\
	.\winemvw.h\
	.\EMACS.H\
	.\OS.H\
	.\EM_GEN.H\
	.\EM_MAC.H\
	.\QUEUE.H\
	.\EM_TIME.H\
	.\SYNTAX.H\
	.\DISPLAY.H\
	.\MLISP.H\
	.\FILEIO.H\
	.\KEYBOARD.H\
	.\BUFFER.H\
	.\ABBREV.H\
	.\NDBM.H\
	.\TT.H\
	.\WINDOW.H\
	.\UNDO.H\
	.\JOURNAL.H\
	.\FILESERV.H\
	.\EM_RTN.H\
	.\EM_VAR.H\
	.\OS_WIN.H\
	.\OS_MAC.H

!IF  "$(CFG)" == "Win32 Release"

# ADD BASE CPP /Yu"STDAFX.H"
# ADD CPP /Yu"STDAFX.H"

$(INTDIR)/mainfrm.obj :  $(SOURCE)  $(DEP_MAINF) $(INTDIR) $(INTDIR)/STDAFX.OBJ
   $(CPP) /nologo /G3 /Zp4 /MT /W4 /GX /Zi /O2 /I "u:\work\emacs\src\edit" /D\
 "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_X86_" /D "_NT" /D "_MBCS" /FR$(INTDIR)/\
 /Fp$(OUTDIR)/"emwinnt.pch" /Yu"STDAFX.H" /Fo$(INTDIR)/\
 /Fd$(OUTDIR)/"emwinnt.pdb" /c /Tp  $(SOURCE) 

!ELSEIF  "$(CFG)" == "Win32 Debug"

# ADD BASE CPP /Yu"STDAFX.H"
# ADD CPP /Yu"STDAFX.H"

$(INTDIR)/mainfrm.obj :  $(SOURCE)  $(DEP_MAINF) $(INTDIR) $(INTDIR)/STDAFX.OBJ
   $(CPP) /nologo /G3 /Zp4 /MT /W4 /GX /Zi /O2 /I "u:\work\emacs\src\edit" /D\
 "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_X86_" /D "_NT" /D "_MBCS"\
 /FR"u:\work\emacs\winnt.dbg/" /Fp$(OUTDIR)/"emwinnt.pch" /Yu"STDAFX.H"\
 /Fo$(INTDIR)/ /Fd$(OUTDIR)/"emwinnt.pdb" /c /Tp  $(SOURCE) 

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\STDAFX.CPP
DEP_STDAF=\
	.\STDAFX.H\
	.\EMACS.H\
	.\OS.H\
	.\EM_GEN.H\
	.\EM_MAC.H\
	.\QUEUE.H\
	.\EM_TIME.H\
	.\SYNTAX.H\
	.\DISPLAY.H\
	.\MLISP.H\
	.\FILEIO.H\
	.\KEYBOARD.H\
	.\BUFFER.H\
	.\ABBREV.H\
	.\NDBM.H\
	.\TT.H\
	.\WINDOW.H\
	.\UNDO.H\
	.\JOURNAL.H\
	.\FILESERV.H\
	.\EM_RTN.H\
	.\EM_VAR.H\
	.\OS_WIN.H\
	.\OS_MAC.H

!IF  "$(CFG)" == "Win32 Release"

# ADD BASE CPP /Yc"STDAFX.H"
# ADD CPP /Yc"STDAFX.H"

$(INTDIR)/STDAFX.OBJ :  $(SOURCE)  $(DEP_STDAF) $(INTDIR)
   $(CPP) /nologo /G3 /Zp4 /MT /W4 /GX /Zi /O2 /I "u:\work\emacs\src\edit" /D\
 "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_X86_" /D "_NT" /D "_MBCS" /FR$(INTDIR)/\
 /Fp$(OUTDIR)/"emwinnt.pch" /Yc"STDAFX.H" /Fo$(INTDIR)/\
 /Fd$(OUTDIR)/"emwinnt.pdb" /c /Tp  $(SOURCE) 

!ELSEIF  "$(CFG)" == "Win32 Debug"

# ADD BASE CPP /Yc"STDAFX.H"
# ADD CPP /Yc"STDAFX.H"

$(INTDIR)/STDAFX.OBJ :  $(SOURCE)  $(DEP_STDAF) $(INTDIR)
   $(CPP) /nologo /G3 /Zp4 /MT /W4 /GX /Zi /O2 /I "u:\work\emacs\src\edit" /D\
 "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_X86_" /D "_NT" /D "_MBCS"\
 /FR"u:\work\emacs\winnt.dbg/" /Fp$(OUTDIR)/"emwinnt.pch" /Yc"STDAFX.H"\
 /Fo$(INTDIR)/ /Fd$(OUTDIR)/"emwinnt.pdb" /c /Tp  $(SOURCE) 

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\SWITCHDL.CPP
DEP_SWITC=\
	.\STDAFX.H\
	.\WINEMACS.H\
	.\SWITCHDL.H\
	.\EMACS.H\
	.\OS.H\
	.\EM_GEN.H\
	.\EM_MAC.H\
	.\QUEUE.H\
	.\EM_TIME.H\
	.\SYNTAX.H\
	.\DISPLAY.H\
	.\MLISP.H\
	.\FILEIO.H\
	.\KEYBOARD.H\
	.\BUFFER.H\
	.\ABBREV.H\
	.\NDBM.H\
	.\TT.H\
	.\WINDOW.H\
	.\UNDO.H\
	.\JOURNAL.H\
	.\FILESERV.H\
	.\EM_RTN.H\
	.\EM_VAR.H\
	.\OS_WIN.H\
	.\OS_MAC.H

!IF  "$(CFG)" == "Win32 Release"

# ADD BASE CPP /Yu"STDAFX.H"
# ADD CPP /Yu"STDAFX.H"

$(INTDIR)/SWITCHDL.OBJ :  $(SOURCE)  $(DEP_SWITC) $(INTDIR)\
 $(INTDIR)/STDAFX.OBJ
   $(CPP) /nologo /G3 /Zp4 /MT /W4 /GX /Zi /O2 /I "u:\work\emacs\src\edit" /D\
 "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_X86_" /D "_NT" /D "_MBCS" /FR$(INTDIR)/\
 /Fp$(OUTDIR)/"emwinnt.pch" /Yu"STDAFX.H" /Fo$(INTDIR)/\
 /Fd$(OUTDIR)/"emwinnt.pdb" /c /Tp  $(SOURCE) 

!ELSEIF  "$(CFG)" == "Win32 Debug"

# ADD BASE CPP /Yu"STDAFX.H"
# ADD CPP /Yu"STDAFX.H"

$(INTDIR)/SWITCHDL.OBJ :  $(SOURCE)  $(DEP_SWITC) $(INTDIR)\
 $(INTDIR)/STDAFX.OBJ
   $(CPP) /nologo /G3 /Zp4 /MT /W4 /GX /Zi /O2 /I "u:\work\emacs\src\edit" /D\
 "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_X86_" /D "_NT" /D "_MBCS"\
 /FR"u:\work\emacs\winnt.dbg/" /Fp$(OUTDIR)/"emwinnt.pch" /Yu"STDAFX.H"\
 /Fo$(INTDIR)/ /Fd$(OUTDIR)/"emwinnt.pdb" /c /Tp  $(SOURCE) 

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\WIN_MENU.CPP
DEP_WIN_M=\
	.\STDAFX.H\
	.\WINEMDOC.H\
	.\winemvw.h\
	.\EMACS.H\
	.\OS.H\
	.\EM_GEN.H\
	.\EM_MAC.H\
	.\QUEUE.H\
	.\EM_TIME.H\
	.\SYNTAX.H\
	.\DISPLAY.H\
	.\MLISP.H\
	.\FILEIO.H\
	.\KEYBOARD.H\
	.\BUFFER.H\
	.\ABBREV.H\
	.\NDBM.H\
	.\TT.H\
	.\WINDOW.H\
	.\UNDO.H\
	.\JOURNAL.H\
	.\FILESERV.H\
	.\EM_RTN.H\
	.\EM_VAR.H\
	.\OS_WIN.H\
	.\OS_MAC.H

!IF  "$(CFG)" == "Win32 Release"

# ADD BASE CPP /Yu"STDAFX.H"
# ADD CPP /Yu"STDAFX.H"

$(INTDIR)/WIN_MENU.OBJ :  $(SOURCE)  $(DEP_WIN_M) $(INTDIR)\
 $(INTDIR)/STDAFX.OBJ
   $(CPP) /nologo /G3 /Zp4 /MT /W4 /GX /Zi /O2 /I "u:\work\emacs\src\edit" /D\
 "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_X86_" /D "_NT" /D "_MBCS" /FR$(INTDIR)/\
 /Fp$(OUTDIR)/"emwinnt.pch" /Yu"STDAFX.H" /Fo$(INTDIR)/\
 /Fd$(OUTDIR)/"emwinnt.pdb" /c /Tp  $(SOURCE) 

!ELSEIF  "$(CFG)" == "Win32 Debug"

# ADD BASE CPP /Yu"STDAFX.H"
# ADD CPP /Yu"STDAFX.H"

$(INTDIR)/WIN_MENU.OBJ :  $(SOURCE)  $(DEP_WIN_M) $(INTDIR)\
 $(INTDIR)/STDAFX.OBJ
   $(CPP) /nologo /G3 /Zp4 /MT /W4 /GX /Zi /O2 /I "u:\work\emacs\src\edit" /D\
 "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_X86_" /D "_NT" /D "_MBCS"\
 /FR"u:\work\emacs\winnt.dbg/" /Fp$(OUTDIR)/"emwinnt.pch" /Yu"STDAFX.H"\
 /Fo$(INTDIR)/ /Fd$(OUTDIR)/"emwinnt.pdb" /c /Tp  $(SOURCE) 

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\win_opt.cpp
DEP_WIN_O=\
	.\STDAFX.H\
	.\WINEMACS.H\
	.\WINEMDOC.H\
	.\winemvw.h\
	.\win_opt.h\
	.\EMACS.H\
	.\OS.H\
	.\EM_GEN.H\
	.\EM_MAC.H\
	.\QUEUE.H\
	.\EM_TIME.H\
	.\SYNTAX.H\
	.\DISPLAY.H\
	.\MLISP.H\
	.\FILEIO.H\
	.\KEYBOARD.H\
	.\BUFFER.H\
	.\ABBREV.H\
	.\NDBM.H\
	.\TT.H\
	.\WINDOW.H\
	.\UNDO.H\
	.\JOURNAL.H\
	.\FILESERV.H\
	.\EM_RTN.H\
	.\EM_VAR.H\
	.\OS_WIN.H\
	.\OS_MAC.H

!IF  "$(CFG)" == "Win32 Release"

# ADD BASE CPP /Yu"STDAFX.H"
# ADD CPP /Yu"STDAFX.H"

$(INTDIR)/win_opt.obj :  $(SOURCE)  $(DEP_WIN_O) $(INTDIR) $(INTDIR)/STDAFX.OBJ
   $(CPP) /nologo /G3 /Zp4 /MT /W4 /GX /Zi /O2 /I "u:\work\emacs\src\edit" /D\
 "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_X86_" /D "_NT" /D "_MBCS" /FR$(INTDIR)/\
 /Fp$(OUTDIR)/"emwinnt.pch" /Yu"STDAFX.H" /Fo$(INTDIR)/\
 /Fd$(OUTDIR)/"emwinnt.pdb" /c /Tp  $(SOURCE) 

!ELSEIF  "$(CFG)" == "Win32 Debug"

# ADD BASE CPP /Yu"STDAFX.H"
# ADD CPP /Yu"STDAFX.H"

$(INTDIR)/win_opt.obj :  $(SOURCE)  $(DEP_WIN_O) $(INTDIR) $(INTDIR)/STDAFX.OBJ
   $(CPP) /nologo /G3 /Zp4 /MT /W4 /GX /Zi /O2 /I "u:\work\emacs\src\edit" /D\
 "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_X86_" /D "_NT" /D "_MBCS"\
 /FR"u:\work\emacs\winnt.dbg/" /Fp$(OUTDIR)/"emwinnt.pch" /Yu"STDAFX.H"\
 /Fo$(INTDIR)/ /Fd$(OUTDIR)/"emwinnt.pdb" /c /Tp  $(SOURCE) 

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\WIN_RTL.CPP
DEP_WIN_R=\
	.\STDAFX.H\
	.\WINEMACS.H\
	.\mainfrm.h\
	.\WINEMDOC.H\
	.\winemvw.h\
	.\EMACS.H\
	.\OS.H\
	.\EM_GEN.H\
	.\EM_MAC.H\
	.\QUEUE.H\
	.\EM_TIME.H\
	.\SYNTAX.H\
	.\DISPLAY.H\
	.\MLISP.H\
	.\FILEIO.H\
	.\KEYBOARD.H\
	.\BUFFER.H\
	.\ABBREV.H\
	.\NDBM.H\
	.\TT.H\
	.\WINDOW.H\
	.\UNDO.H\
	.\JOURNAL.H\
	.\FILESERV.H\
	.\EM_RTN.H\
	.\EM_VAR.H\
	.\OS_WIN.H\
	.\OS_MAC.H

!IF  "$(CFG)" == "Win32 Release"

# ADD BASE CPP /Yu"STDAFX.H"
# ADD CPP /Yu"STDAFX.H"

$(INTDIR)/WIN_RTL.OBJ :  $(SOURCE)  $(DEP_WIN_R) $(INTDIR) $(INTDIR)/STDAFX.OBJ
   $(CPP) /nologo /G3 /Zp4 /MT /W4 /GX /Zi /O2 /I "u:\work\emacs\src\edit" /D\
 "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_X86_" /D "_NT" /D "_MBCS" /FR$(INTDIR)/\
 /Fp$(OUTDIR)/"emwinnt.pch" /Yu"STDAFX.H" /Fo$(INTDIR)/\
 /Fd$(OUTDIR)/"emwinnt.pdb" /c /Tp  $(SOURCE) 

!ELSEIF  "$(CFG)" == "Win32 Debug"

# ADD BASE CPP /Yu"STDAFX.H"
# ADD CPP /Yu"STDAFX.H"

$(INTDIR)/WIN_RTL.OBJ :  $(SOURCE)  $(DEP_WIN_R) $(INTDIR) $(INTDIR)/STDAFX.OBJ
   $(CPP) /nologo /G3 /Zp4 /MT /W4 /GX /Zi /O2 /I "u:\work\emacs\src\edit" /D\
 "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_X86_" /D "_NT" /D "_MBCS"\
 /FR"u:\work\emacs\winnt.dbg/" /Fp$(OUTDIR)/"emwinnt.pch" /Yu"STDAFX.H"\
 /Fo$(INTDIR)/ /Fd$(OUTDIR)/"emwinnt.pdb" /c /Tp  $(SOURCE) 

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\WIN_UI.CPP
DEP_WIN_U=\
	.\STDAFX.H\
	.\WINEMACS.H\
	.\WINEMDOC.H\
	.\winemvw.h\
	.\FINDDLG.H\
	.\SWITCHDL.H\
	.\win_opt.h\
	.\EMACS.H\
	.\OS.H\
	.\EM_GEN.H\
	.\EM_MAC.H\
	.\QUEUE.H\
	.\EM_TIME.H\
	.\SYNTAX.H\
	.\DISPLAY.H\
	.\MLISP.H\
	.\FILEIO.H\
	.\KEYBOARD.H\
	.\BUFFER.H\
	.\ABBREV.H\
	.\NDBM.H\
	.\TT.H\
	.\WINDOW.H\
	.\UNDO.H\
	.\JOURNAL.H\
	.\FILESERV.H\
	.\EM_RTN.H\
	.\EM_VAR.H\
	.\OS_WIN.H\
	.\OS_MAC.H

!IF  "$(CFG)" == "Win32 Release"

# ADD BASE CPP /Yu"STDAFX.H"
# ADD CPP /Yu"STDAFX.H"

$(INTDIR)/WIN_UI.OBJ :  $(SOURCE)  $(DEP_WIN_U) $(INTDIR) $(INTDIR)/STDAFX.OBJ
   $(CPP) /nologo /G3 /Zp4 /MT /W4 /GX /Zi /O2 /I "u:\work\emacs\src\edit" /D\
 "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_X86_" /D "_NT" /D "_MBCS" /FR$(INTDIR)/\
 /Fp$(OUTDIR)/"emwinnt.pch" /Yu"STDAFX.H" /Fo$(INTDIR)/\
 /Fd$(OUTDIR)/"emwinnt.pdb" /c /Tp  $(SOURCE) 

!ELSEIF  "$(CFG)" == "Win32 Debug"

# ADD BASE CPP /Yu"STDAFX.H"
# ADD CPP /Yu"STDAFX.H"

$(INTDIR)/WIN_UI.OBJ :  $(SOURCE)  $(DEP_WIN_U) $(INTDIR) $(INTDIR)/STDAFX.OBJ
   $(CPP) /nologo /G3 /Zp4 /MT /W4 /GX /Zi /O2 /I "u:\work\emacs\src\edit" /D\
 "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_X86_" /D "_NT" /D "_MBCS"\
 /FR"u:\work\emacs\winnt.dbg/" /Fp$(OUTDIR)/"emwinnt.pch" /Yu"STDAFX.H"\
 /Fo$(INTDIR)/ /Fd$(OUTDIR)/"emwinnt.pdb" /c /Tp  $(SOURCE) 

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\WINEMACS.CPP
DEP_WINEM=\
	.\STDAFX.H\
	.\WINEMACS.H\
	.\mainfrm.h\
	.\WINEMDOC.H\
	.\winemvw.h\
	.\win_opt.h\
	.\EMACS.H\
	.\OS.H\
	.\EM_GEN.H\
	.\EM_MAC.H\
	.\QUEUE.H\
	.\EM_TIME.H\
	.\SYNTAX.H\
	.\DISPLAY.H\
	.\MLISP.H\
	.\FILEIO.H\
	.\KEYBOARD.H\
	.\BUFFER.H\
	.\ABBREV.H\
	.\NDBM.H\
	.\TT.H\
	.\WINDOW.H\
	.\UNDO.H\
	.\JOURNAL.H\
	.\FILESERV.H\
	.\EM_RTN.H\
	.\EM_VAR.H\
	.\OS_WIN.H\
	.\OS_MAC.H

!IF  "$(CFG)" == "Win32 Release"

# ADD BASE CPP /Yu"STDAFX.H"
# ADD CPP /Yu"STDAFX.H"

$(INTDIR)/WINEMACS.OBJ :  $(SOURCE)  $(DEP_WINEM) $(INTDIR)\
 $(INTDIR)/STDAFX.OBJ
   $(CPP) /nologo /G3 /Zp4 /MT /W4 /GX /Zi /O2 /I "u:\work\emacs\src\edit" /D\
 "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_X86_" /D "_NT" /D "_MBCS" /FR$(INTDIR)/\
 /Fp$(OUTDIR)/"emwinnt.pch" /Yu"STDAFX.H" /Fo$(INTDIR)/\
 /Fd$(OUTDIR)/"emwinnt.pdb" /c /Tp  $(SOURCE) 

!ELSEIF  "$(CFG)" == "Win32 Debug"

# ADD BASE CPP /Yu"STDAFX.H"
# ADD CPP /Yu"STDAFX.H"

$(INTDIR)/WINEMACS.OBJ :  $(SOURCE)  $(DEP_WINEM) $(INTDIR)\
 $(INTDIR)/STDAFX.OBJ
   $(CPP) /nologo /G3 /Zp4 /MT /W4 /GX /Zi /O2 /I "u:\work\emacs\src\edit" /D\
 "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_X86_" /D "_NT" /D "_MBCS"\
 /FR"u:\work\emacs\winnt.dbg/" /Fp$(OUTDIR)/"emwinnt.pch" /Yu"STDAFX.H"\
 /Fo$(INTDIR)/ /Fd$(OUTDIR)/"emwinnt.pdb" /c /Tp  $(SOURCE) 

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\WINEMACS.RC
DEP_WINEMA=\
	.\RES\WINEMACS.ICO\
	.\RES\WINEMDOC.ICO\
	.\RES\TOOLBAR.BMP\
	.\RES\WINEMACS.RC2

$(INTDIR)/WINEMACS.res :  $(SOURCE)  $(DEP_WINEMA) $(INTDIR)
   $(RSC) $(RSC_PROJ)  $(SOURCE) 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\WINEMDOC.CPP
DEP_WINEMD=\
	.\STDAFX.H\
	.\WINEMACS.H\
	.\WINEMDOC.H\
	.\EMACS.H\
	.\OS.H\
	.\EM_GEN.H\
	.\EM_MAC.H\
	.\QUEUE.H\
	.\EM_TIME.H\
	.\SYNTAX.H\
	.\DISPLAY.H\
	.\MLISP.H\
	.\FILEIO.H\
	.\KEYBOARD.H\
	.\BUFFER.H\
	.\ABBREV.H\
	.\NDBM.H\
	.\TT.H\
	.\WINDOW.H\
	.\UNDO.H\
	.\JOURNAL.H\
	.\FILESERV.H\
	.\EM_RTN.H\
	.\EM_VAR.H\
	.\OS_WIN.H\
	.\OS_MAC.H

!IF  "$(CFG)" == "Win32 Release"

# ADD BASE CPP /Yu"STDAFX.H"
# ADD CPP /Yu"STDAFX.H"

$(INTDIR)/WINEMDOC.OBJ :  $(SOURCE)  $(DEP_WINEMD) $(INTDIR)\
 $(INTDIR)/STDAFX.OBJ
   $(CPP) /nologo /G3 /Zp4 /MT /W4 /GX /Zi /O2 /I "u:\work\emacs\src\edit" /D\
 "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_X86_" /D "_NT" /D "_MBCS" /FR$(INTDIR)/\
 /Fp$(OUTDIR)/"emwinnt.pch" /Yu"STDAFX.H" /Fo$(INTDIR)/\
 /Fd$(OUTDIR)/"emwinnt.pdb" /c /Tp  $(SOURCE) 

!ELSEIF  "$(CFG)" == "Win32 Debug"

# ADD BASE CPP /Yu"STDAFX.H"
# ADD CPP /Yu"STDAFX.H"

$(INTDIR)/WINEMDOC.OBJ :  $(SOURCE)  $(DEP_WINEMD) $(INTDIR)\
 $(INTDIR)/STDAFX.OBJ
   $(CPP) /nologo /G3 /Zp4 /MT /W4 /GX /Zi /O2 /I "u:\work\emacs\src\edit" /D\
 "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_X86_" /D "_NT" /D "_MBCS"\
 /FR"u:\work\emacs\winnt.dbg/" /Fp$(OUTDIR)/"emwinnt.pch" /Yu"STDAFX.H"\
 /Fo$(INTDIR)/ /Fd$(OUTDIR)/"emwinnt.pdb" /c /Tp  $(SOURCE) 

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\winemvw.cpp
DEP_WINEMV=\
	.\STDAFX.H\
	.\WINEMACS.H\
	.\WINEMDOC.H\
	.\winemvw.h\
	.\FINDDLG.H\
	.\SWITCHDL.H\
	.\win_opt.h\
	.\EMACS.H\
	.\OS.H\
	.\EM_GEN.H\
	.\EM_MAC.H\
	.\QUEUE.H\
	.\EM_TIME.H\
	.\SYNTAX.H\
	.\DISPLAY.H\
	.\MLISP.H\
	.\FILEIO.H\
	.\KEYBOARD.H\
	.\BUFFER.H\
	.\ABBREV.H\
	.\NDBM.H\
	.\TT.H\
	.\WINDOW.H\
	.\UNDO.H\
	.\JOURNAL.H\
	.\FILESERV.H\
	.\EM_RTN.H\
	.\EM_VAR.H\
	.\OS_WIN.H\
	.\OS_MAC.H

!IF  "$(CFG)" == "Win32 Release"

# ADD BASE CPP /Yu"STDAFX.H"
# ADD CPP /Yu"STDAFX.H"

$(INTDIR)/winemvw.obj :  $(SOURCE)  $(DEP_WINEMV) $(INTDIR)\
 $(INTDIR)/STDAFX.OBJ
   $(CPP) /nologo /G3 /Zp4 /MT /W4 /GX /Zi /O2 /I "u:\work\emacs\src\edit" /D\
 "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_X86_" /D "_NT" /D "_MBCS" /FR$(INTDIR)/\
 /Fp$(OUTDIR)/"emwinnt.pch" /Yu"STDAFX.H" /Fo$(INTDIR)/\
 /Fd$(OUTDIR)/"emwinnt.pdb" /c /Tp  $(SOURCE) 

!ELSEIF  "$(CFG)" == "Win32 Debug"

# ADD BASE CPP /Yu"STDAFX.H"
# ADD CPP /Yu"STDAFX.H"

$(INTDIR)/winemvw.obj :  $(SOURCE)  $(DEP_WINEMV) $(INTDIR)\
 $(INTDIR)/STDAFX.OBJ
   $(CPP) /nologo /G3 /Zp4 /MT /W4 /GX /Zi /O2 /I "u:\work\emacs\src\edit" /D\
 "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_X86_" /D "_NT" /D "_MBCS"\
 /FR"u:\work\emacs\winnt.dbg/" /Fp$(OUTDIR)/"emwinnt.pch" /Yu"STDAFX.H"\
 /Fo$(INTDIR)/ /Fd$(OUTDIR)/"emwinnt.pdb" /c /Tp  $(SOURCE) 

!ENDIF 

# End Source File
# End Group
################################################################################
# Begin Group "Editor files"

################################################################################
# Begin Source File

SOURCE=.\ABBREV.C
DEP_ABBRE=\
	.\EMACS.H\
	.\OS.H\
	.\EM_GEN.H\
	.\EM_MAC.H\
	.\QUEUE.H\
	.\EM_TIME.H\
	.\SYNTAX.H\
	.\DISPLAY.H\
	.\MLISP.H\
	.\FILEIO.H\
	.\KEYBOARD.H\
	.\BUFFER.H\
	.\ABBREV.H\
	.\NDBM.H\
	.\TT.H\
	.\WINDOW.H\
	.\UNDO.H\
	.\JOURNAL.H\
	.\FILESERV.H\
	.\EM_RTN.H\
	.\EM_VAR.H\
	.\OS_WIN.H\
	.\OS_MAC.H

!IF  "$(CFG)" == "Win32 Release"

# ADD BASE CPP /Yc"EMACS.H"
# SUBTRACT CPP /Yc /Yu

$(INTDIR)/ABBREV.OBJ :  $(SOURCE)  $(DEP_ABBRE) $(INTDIR)
   $(CPP) /nologo /G3 /Zp4 /MT /W4 /GX /Zi /O2 /I "u:\work\emacs\src\edit" /D\
 "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_X86_" /D "_NT" /D "_MBCS" /FR$(INTDIR)/\
 /Fo$(INTDIR)/ /Fd$(OUTDIR)/"emwinnt.pdb" /c /Tp  $(SOURCE) 

!ELSEIF  "$(CFG)" == "Win32 Debug"

# ADD BASE CPP /Yc"EMACS.H"
# SUBTRACT CPP /Yc /Yu

$(INTDIR)/ABBREV.OBJ :  $(SOURCE)  $(DEP_ABBRE) $(INTDIR)
   $(CPP) /nologo /G3 /Zp4 /MT /W4 /GX /Zi /O2 /I "u:\work\emacs\src\edit" /D\
 "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_X86_" /D "_NT" /D "_MBCS"\
 /FR"u:\work\emacs\winnt.dbg/" /Fo$(INTDIR)/ /Fd$(OUTDIR)/"emwinnt.pdb" /c /Tp\
  $(SOURCE) 

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\ABSPATH.C
DEP_ABSPA=\
	.\WIN_INCL.H\
	.\EMACS.H\
	.\OS.H\
	.\EM_GEN.H\
	.\EM_MAC.H\
	.\QUEUE.H\
	.\EM_TIME.H\
	.\SYNTAX.H\
	.\DISPLAY.H\
	.\MLISP.H\
	.\FILEIO.H\
	.\KEYBOARD.H\
	.\BUFFER.H\
	.\ABBREV.H\
	.\NDBM.H\
	.\TT.H\
	.\WINDOW.H\
	.\UNDO.H\
	.\JOURNAL.H\
	.\FILESERV.H\
	.\EM_RTN.H\
	.\EM_VAR.H\
	.\OS_WIN.H\
	.\OS_MAC.H

!IF  "$(CFG)" == "Win32 Release"

# ADD BASE CPP /Yu"EMACS.H"
# SUBTRACT CPP /Yc /Yu

$(INTDIR)/ABSPATH.OBJ :  $(SOURCE)  $(DEP_ABSPA) $(INTDIR)
   $(CPP) /nologo /G3 /Zp4 /MT /W4 /GX /Zi /O2 /I "u:\work\emacs\src\edit" /D\
 "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_X86_" /D "_NT" /D "_MBCS" /FR$(INTDIR)/\
 /Fo$(INTDIR)/ /Fd$(OUTDIR)/"emwinnt.pdb" /c /Tp  $(SOURCE) 

!ELSEIF  "$(CFG)" == "Win32 Debug"

# ADD BASE CPP /Yu"EMACS.H"
# SUBTRACT CPP /Yc /Yu

$(INTDIR)/ABSPATH.OBJ :  $(SOURCE)  $(DEP_ABSPA) $(INTDIR)
   $(CPP) /nologo /G3 /Zp4 /MT /W4 /GX /Zi /O2 /I "u:\work\emacs\src\edit" /D\
 "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_X86_" /D "_NT" /D "_MBCS"\
 /FR"u:\work\emacs\winnt.dbg/" /Fo$(INTDIR)/ /Fd$(OUTDIR)/"emwinnt.pdb" /c /Tp\
  $(SOURCE) 

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\ARITH.C
DEP_ARITH=\
	.\EMACS.H\
	.\OS.H\
	.\EM_GEN.H\
	.\EM_MAC.H\
	.\QUEUE.H\
	.\EM_TIME.H\
	.\SYNTAX.H\
	.\DISPLAY.H\
	.\MLISP.H\
	.\FILEIO.H\
	.\KEYBOARD.H\
	.\BUFFER.H\
	.\ABBREV.H\
	.\NDBM.H\
	.\TT.H\
	.\WINDOW.H\
	.\UNDO.H\
	.\JOURNAL.H\
	.\FILESERV.H\
	.\EM_RTN.H\
	.\EM_VAR.H\
	.\OS_WIN.H\
	.\OS_MAC.H

!IF  "$(CFG)" == "Win32 Release"

# ADD BASE CPP /Yu"EMACS.H"
# SUBTRACT CPP /Yc /Yu

$(INTDIR)/ARITH.OBJ :  $(SOURCE)  $(DEP_ARITH) $(INTDIR)
   $(CPP) /nologo /G3 /Zp4 /MT /W4 /GX /Zi /O2 /I "u:\work\emacs\src\edit" /D\
 "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_X86_" /D "_NT" /D "_MBCS" /FR$(INTDIR)/\
 /Fo$(INTDIR)/ /Fd$(OUTDIR)/"emwinnt.pdb" /c /Tp  $(SOURCE) 

!ELSEIF  "$(CFG)" == "Win32 Debug"

# ADD BASE CPP /Yu"EMACS.H"
# SUBTRACT CPP /Yc /Yu

$(INTDIR)/ARITH.OBJ :  $(SOURCE)  $(DEP_ARITH) $(INTDIR)
   $(CPP) /nologo /G3 /Zp4 /MT /W4 /GX /Zi /O2 /I "u:\work\emacs\src\edit" /D\
 "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_X86_" /D "_NT" /D "_MBCS"\
 /FR"u:\work\emacs\winnt.dbg/" /Fo$(INTDIR)/ /Fd$(OUTDIR)/"emwinnt.pdb" /c /Tp\
  $(SOURCE) 

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\BUFFER.C
DEP_BUFFE=\
	.\EMACS.H\
	.\OS.H\
	.\EM_GEN.H\
	.\EM_MAC.H\
	.\QUEUE.H\
	.\EM_TIME.H\
	.\SYNTAX.H\
	.\DISPLAY.H\
	.\MLISP.H\
	.\FILEIO.H\
	.\KEYBOARD.H\
	.\BUFFER.H\
	.\ABBREV.H\
	.\NDBM.H\
	.\TT.H\
	.\WINDOW.H\
	.\UNDO.H\
	.\JOURNAL.H\
	.\FILESERV.H\
	.\EM_RTN.H\
	.\EM_VAR.H\
	.\OS_WIN.H\
	.\OS_MAC.H

!IF  "$(CFG)" == "Win32 Release"

# ADD BASE CPP /Yu"EMACS.H"
# SUBTRACT CPP /Yc /Yu

$(INTDIR)/BUFFER.OBJ :  $(SOURCE)  $(DEP_BUFFE) $(INTDIR)
   $(CPP) /nologo /G3 /Zp4 /MT /W4 /GX /Zi /O2 /I "u:\work\emacs\src\edit" /D\
 "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_X86_" /D "_NT" /D "_MBCS" /FR$(INTDIR)/\
 /Fo$(INTDIR)/ /Fd$(OUTDIR)/"emwinnt.pdb" /c /Tp  $(SOURCE) 

!ELSEIF  "$(CFG)" == "Win32 Debug"

# ADD BASE CPP /Yu"EMACS.H"
# SUBTRACT CPP /Yc /Yu

$(INTDIR)/BUFFER.OBJ :  $(SOURCE)  $(DEP_BUFFE) $(INTDIR)
   $(CPP) /nologo /G3 /Zp4 /MT /W4 /GX /Zi /O2 /I "u:\work\emacs\src\edit" /D\
 "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_X86_" /D "_NT" /D "_MBCS"\
 /FR"u:\work\emacs\winnt.dbg/" /Fo$(INTDIR)/ /Fd$(OUTDIR)/"emwinnt.pdb" /c /Tp\
  $(SOURCE) 

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\CASECONV.C
DEP_CASEC=\
	.\EMACS.H\
	.\OS.H\
	.\EM_GEN.H\
	.\EM_MAC.H\
	.\QUEUE.H\
	.\EM_TIME.H\
	.\SYNTAX.H\
	.\DISPLAY.H\
	.\MLISP.H\
	.\FILEIO.H\
	.\KEYBOARD.H\
	.\BUFFER.H\
	.\ABBREV.H\
	.\NDBM.H\
	.\TT.H\
	.\WINDOW.H\
	.\UNDO.H\
	.\JOURNAL.H\
	.\FILESERV.H\
	.\EM_RTN.H\
	.\EM_VAR.H\
	.\OS_WIN.H\
	.\OS_MAC.H

!IF  "$(CFG)" == "Win32 Release"

# ADD BASE CPP /Yu"EMACS.H"
# SUBTRACT CPP /Yc /Yu

$(INTDIR)/CASECONV.OBJ :  $(SOURCE)  $(DEP_CASEC) $(INTDIR)
   $(CPP) /nologo /G3 /Zp4 /MT /W4 /GX /Zi /O2 /I "u:\work\emacs\src\edit" /D\
 "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_X86_" /D "_NT" /D "_MBCS" /FR$(INTDIR)/\
 /Fo$(INTDIR)/ /Fd$(OUTDIR)/"emwinnt.pdb" /c /Tp  $(SOURCE) 

!ELSEIF  "$(CFG)" == "Win32 Debug"

# ADD BASE CPP /Yu"EMACS.H"
# SUBTRACT CPP /Yc /Yu

$(INTDIR)/CASECONV.OBJ :  $(SOURCE)  $(DEP_CASEC) $(INTDIR)
   $(CPP) /nologo /G3 /Zp4 /MT /W4 /GX /Zi /O2 /I "u:\work\emacs\src\edit" /D\
 "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_X86_" /D "_NT" /D "_MBCS"\
 /FR"u:\work\emacs\winnt.dbg/" /Fo$(INTDIR)/ /Fd$(OUTDIR)/"emwinnt.pdb" /c /Tp\
  $(SOURCE) 

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\COLUMNS.C
DEP_COLUM=\
	.\EMACS.H\
	.\OS.H\
	.\EM_GEN.H\
	.\EM_MAC.H\
	.\QUEUE.H\
	.\EM_TIME.H\
	.\SYNTAX.H\
	.\DISPLAY.H\
	.\MLISP.H\
	.\FILEIO.H\
	.\KEYBOARD.H\
	.\BUFFER.H\
	.\ABBREV.H\
	.\NDBM.H\
	.\TT.H\
	.\WINDOW.H\
	.\UNDO.H\
	.\JOURNAL.H\
	.\FILESERV.H\
	.\EM_RTN.H\
	.\EM_VAR.H\
	.\OS_WIN.H\
	.\OS_MAC.H

!IF  "$(CFG)" == "Win32 Release"

# ADD BASE CPP /Yu"EMACS.H"
# SUBTRACT CPP /Yc /Yu

$(INTDIR)/COLUMNS.OBJ :  $(SOURCE)  $(DEP_COLUM) $(INTDIR)
   $(CPP) /nologo /G3 /Zp4 /MT /W4 /GX /Zi /O2 /I "u:\work\emacs\src\edit" /D\
 "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_X86_" /D "_NT" /D "_MBCS" /FR$(INTDIR)/\
 /Fo$(INTDIR)/ /Fd$(OUTDIR)/"emwinnt.pdb" /c /Tp  $(SOURCE) 

!ELSEIF  "$(CFG)" == "Win32 Debug"

# ADD BASE CPP /Yu"EMACS.H"
# SUBTRACT CPP /Yc /Yu

$(INTDIR)/COLUMNS.OBJ :  $(SOURCE)  $(DEP_COLUM) $(INTDIR)
   $(CPP) /nologo /G3 /Zp4 /MT /W4 /GX /Zi /O2 /I "u:\work\emacs\src\edit" /D\
 "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_X86_" /D "_NT" /D "_MBCS"\
 /FR"u:\work\emacs\winnt.dbg/" /Fo$(INTDIR)/ /Fd$(OUTDIR)/"emwinnt.pdb" /c /Tp\
  $(SOURCE) 

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\DBMAN.C
DEP_DBMAN=\
	.\EMACS.H\
	.\OS.H\
	.\EM_GEN.H\
	.\EM_MAC.H\
	.\QUEUE.H\
	.\EM_TIME.H\
	.\SYNTAX.H\
	.\DISPLAY.H\
	.\MLISP.H\
	.\FILEIO.H\
	.\KEYBOARD.H\
	.\BUFFER.H\
	.\ABBREV.H\
	.\NDBM.H\
	.\TT.H\
	.\WINDOW.H\
	.\UNDO.H\
	.\JOURNAL.H\
	.\FILESERV.H\
	.\EM_RTN.H\
	.\EM_VAR.H\
	.\OS_WIN.H\
	.\OS_MAC.H

!IF  "$(CFG)" == "Win32 Release"

# ADD BASE CPP /Yu"EMACS.H"
# SUBTRACT CPP /Yc /Yu

$(INTDIR)/DBMAN.OBJ :  $(SOURCE)  $(DEP_DBMAN) $(INTDIR)
   $(CPP) /nologo /G3 /Zp4 /MT /W4 /GX /Zi /O2 /I "u:\work\emacs\src\edit" /D\
 "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_X86_" /D "_NT" /D "_MBCS" /FR$(INTDIR)/\
 /Fo$(INTDIR)/ /Fd$(OUTDIR)/"emwinnt.pdb" /c /Tp  $(SOURCE) 

!ELSEIF  "$(CFG)" == "Win32 Debug"

# ADD BASE CPP /Yu"EMACS.H"
# SUBTRACT CPP /Yc /Yu

$(INTDIR)/DBMAN.OBJ :  $(SOURCE)  $(DEP_DBMAN) $(INTDIR)
   $(CPP) /nologo /G3 /Zp4 /MT /W4 /GX /Zi /O2 /I "u:\work\emacs\src\edit" /D\
 "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_X86_" /D "_NT" /D "_MBCS"\
 /FR"u:\work\emacs\winnt.dbg/" /Fo$(INTDIR)/ /Fd$(OUTDIR)/"emwinnt.pdb" /c /Tp\
  $(SOURCE) 

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\DISPLAY.C
DEP_DISPL=\
	.\EMACS.H\
	.\OS.H\
	.\EM_GEN.H\
	.\EM_MAC.H\
	.\QUEUE.H\
	.\EM_TIME.H\
	.\SYNTAX.H\
	.\DISPLAY.H\
	.\MLISP.H\
	.\FILEIO.H\
	.\KEYBOARD.H\
	.\BUFFER.H\
	.\ABBREV.H\
	.\NDBM.H\
	.\TT.H\
	.\WINDOW.H\
	.\UNDO.H\
	.\JOURNAL.H\
	.\FILESERV.H\
	.\EM_RTN.H\
	.\EM_VAR.H\
	.\OS_WIN.H\
	.\OS_MAC.H

!IF  "$(CFG)" == "Win32 Release"

# ADD BASE CPP /Yu"EMACS.H"
# SUBTRACT CPP /Yc /Yu

$(INTDIR)/DISPLAY.OBJ :  $(SOURCE)  $(DEP_DISPL) $(INTDIR)
   $(CPP) /nologo /G3 /Zp4 /MT /W4 /GX /Zi /O2 /I "u:\work\emacs\src\edit" /D\
 "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_X86_" /D "_NT" /D "_MBCS" /FR$(INTDIR)/\
 /Fo$(INTDIR)/ /Fd$(OUTDIR)/"emwinnt.pdb" /c /Tp  $(SOURCE) 

!ELSEIF  "$(CFG)" == "Win32 Debug"

# ADD BASE CPP /Yu"EMACS.H"
# SUBTRACT CPP /Yc /Yu

$(INTDIR)/DISPLAY.OBJ :  $(SOURCE)  $(DEP_DISPL) $(INTDIR)
   $(CPP) /nologo /G3 /Zp4 /MT /W4 /GX /Zi /O2 /I "u:\work\emacs\src\edit" /D\
 "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_X86_" /D "_NT" /D "_MBCS"\
 /FR"u:\work\emacs\winnt.dbg/" /Fo$(INTDIR)/ /Fd$(OUTDIR)/"emwinnt.pdb" /c /Tp\
  $(SOURCE) 

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\EDOPRINT.C
DEP_EDOPR=\
	.\EMACS.H\
	.\DOPRINT.C\
	.\OS.H\
	.\EM_GEN.H\
	.\EM_MAC.H\
	.\QUEUE.H\
	.\EM_TIME.H\
	.\SYNTAX.H\
	.\DISPLAY.H\
	.\MLISP.H\
	.\FILEIO.H\
	.\KEYBOARD.H\
	.\BUFFER.H\
	.\ABBREV.H\
	.\NDBM.H\
	.\TT.H\
	.\WINDOW.H\
	.\UNDO.H\
	.\JOURNAL.H\
	.\FILESERV.H\
	.\EM_RTN.H\
	.\EM_VAR.H\
	.\OS_WIN.H\
	.\OS_MAC.H

!IF  "$(CFG)" == "Win32 Release"

# ADD BASE CPP /Yu"EMACS.H"
# SUBTRACT CPP /Yc /Yu

$(INTDIR)/EDOPRINT.OBJ :  $(SOURCE)  $(DEP_EDOPR) $(INTDIR)
   $(CPP) /nologo /G3 /Zp4 /MT /W4 /GX /Zi /O2 /I "u:\work\emacs\src\edit" /D\
 "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_X86_" /D "_NT" /D "_MBCS" /FR$(INTDIR)/\
 /Fo$(INTDIR)/ /Fd$(OUTDIR)/"emwinnt.pdb" /c /Tp  $(SOURCE) 

!ELSEIF  "$(CFG)" == "Win32 Debug"

# ADD BASE CPP /Yu"EMACS.H"
# SUBTRACT CPP /Yc /Yu

$(INTDIR)/EDOPRINT.OBJ :  $(SOURCE)  $(DEP_EDOPR) $(INTDIR)
   $(CPP) /nologo /G3 /Zp4 /MT /W4 /GX /Zi /O2 /I "u:\work\emacs\src\edit" /D\
 "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_X86_" /D "_NT" /D "_MBCS"\
 /FR"u:\work\emacs\winnt.dbg/" /Fo$(INTDIR)/ /Fd$(OUTDIR)/"emwinnt.pdb" /c /Tp\
  $(SOURCE) 

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\EM_TIME.C
DEP_EM_TI=\
	.\EMACS.H\
	.\OS.H\
	.\EM_GEN.H\
	.\EM_MAC.H\
	.\QUEUE.H\
	.\EM_TIME.H\
	.\SYNTAX.H\
	.\DISPLAY.H\
	.\MLISP.H\
	.\FILEIO.H\
	.\KEYBOARD.H\
	.\BUFFER.H\
	.\ABBREV.H\
	.\NDBM.H\
	.\TT.H\
	.\WINDOW.H\
	.\UNDO.H\
	.\JOURNAL.H\
	.\FILESERV.H\
	.\EM_RTN.H\
	.\EM_VAR.H\
	.\OS_WIN.H\
	.\OS_MAC.H

!IF  "$(CFG)" == "Win32 Release"

# ADD BASE CPP /Yu"EMACS.H"
# SUBTRACT CPP /Yc /Yu

$(INTDIR)/EM_TIME.OBJ :  $(SOURCE)  $(DEP_EM_TI) $(INTDIR)
   $(CPP) /nologo /G3 /Zp4 /MT /W4 /GX /Zi /O2 /I "u:\work\emacs\src\edit" /D\
 "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_X86_" /D "_NT" /D "_MBCS" /FR$(INTDIR)/\
 /Fo$(INTDIR)/ /Fd$(OUTDIR)/"emwinnt.pdb" /c /Tp  $(SOURCE) 

!ELSEIF  "$(CFG)" == "Win32 Debug"

# ADD BASE CPP /Yu"EMACS.H"
# SUBTRACT CPP /Yc /Yu

$(INTDIR)/EM_TIME.OBJ :  $(SOURCE)  $(DEP_EM_TI) $(INTDIR)
   $(CPP) /nologo /G3 /Zp4 /MT /W4 /GX /Zi /O2 /I "u:\work\emacs\src\edit" /D\
 "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_X86_" /D "_NT" /D "_MBCS"\
 /FR"u:\work\emacs\winnt.dbg/" /Fo$(INTDIR)/ /Fd$(OUTDIR)/"emwinnt.pdb" /c /Tp\
  $(SOURCE) 

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\EMACS.C
DEP_EMACS=\
	.\EMACS.H\
	.\OS.H\
	.\EM_GEN.H\
	.\EM_MAC.H\
	.\QUEUE.H\
	.\EM_TIME.H\
	.\SYNTAX.H\
	.\DISPLAY.H\
	.\MLISP.H\
	.\FILEIO.H\
	.\KEYBOARD.H\
	.\BUFFER.H\
	.\ABBREV.H\
	.\NDBM.H\
	.\TT.H\
	.\WINDOW.H\
	.\UNDO.H\
	.\JOURNAL.H\
	.\FILESERV.H\
	.\EM_RTN.H\
	.\EM_VAR.H\
	.\OS_WIN.H\
	.\OS_MAC.H

!IF  "$(CFG)" == "Win32 Release"

# ADD BASE CPP /Yu"EMACS.H"
# SUBTRACT CPP /Yc /Yu

$(INTDIR)/EMACS.OBJ :  $(SOURCE)  $(DEP_EMACS) $(INTDIR)
   $(CPP) /nologo /G3 /Zp4 /MT /W4 /GX /Zi /O2 /I "u:\work\emacs\src\edit" /D\
 "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_X86_" /D "_NT" /D "_MBCS" /FR$(INTDIR)/\
 /Fo$(INTDIR)/ /Fd$(OUTDIR)/"emwinnt.pdb" /c /Tp  $(SOURCE) 

!ELSEIF  "$(CFG)" == "Win32 Debug"

# ADD BASE CPP /Yu"EMACS.H"
# SUBTRACT CPP /Yc /Yu

$(INTDIR)/EMACS.OBJ :  $(SOURCE)  $(DEP_EMACS) $(INTDIR)
   $(CPP) /nologo /G3 /Zp4 /MT /W4 /GX /Zi /O2 /I "u:\work\emacs\src\edit" /D\
 "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_X86_" /D "_NT" /D "_MBCS"\
 /FR"u:\work\emacs\winnt.dbg/" /Fo$(INTDIR)/ /Fd$(OUTDIR)/"emwinnt.pdb" /c /Tp\
  $(SOURCE) 

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\EMACSRTL.C
DEP_EMACSR=\
	.\EMACS.H\
	.\OS.H\
	.\EM_GEN.H\
	.\EM_MAC.H\
	.\QUEUE.H\
	.\EM_TIME.H\
	.\SYNTAX.H\
	.\DISPLAY.H\
	.\MLISP.H\
	.\FILEIO.H\
	.\KEYBOARD.H\
	.\BUFFER.H\
	.\ABBREV.H\
	.\NDBM.H\
	.\TT.H\
	.\WINDOW.H\
	.\UNDO.H\
	.\JOURNAL.H\
	.\FILESERV.H\
	.\EM_RTN.H\
	.\EM_VAR.H\
	.\OS_WIN.H\
	.\OS_MAC.H

!IF  "$(CFG)" == "Win32 Release"

# ADD BASE CPP /Yu"EMACS.H"
# SUBTRACT CPP /Yc /Yu

$(INTDIR)/EMACSRTL.OBJ :  $(SOURCE)  $(DEP_EMACSR) $(INTDIR)
   $(CPP) /nologo /G3 /Zp4 /MT /W4 /GX /Zi /O2 /I "u:\work\emacs\src\edit" /D\
 "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_X86_" /D "_NT" /D "_MBCS" /FR$(INTDIR)/\
 /Fo$(INTDIR)/ /Fd$(OUTDIR)/"emwinnt.pdb" /c /Tp  $(SOURCE) 

!ELSEIF  "$(CFG)" == "Win32 Debug"

# ADD BASE CPP /Yu"EMACS.H"
# SUBTRACT CPP /Yc /Yu

$(INTDIR)/EMACSRTL.OBJ :  $(SOURCE)  $(DEP_EMACSR) $(INTDIR)
   $(CPP) /nologo /G3 /Zp4 /MT /W4 /GX /Zi /O2 /I "u:\work\emacs\src\edit" /D\
 "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_X86_" /D "_NT" /D "_MBCS"\
 /FR"u:\work\emacs\winnt.dbg/" /Fo$(INTDIR)/ /Fd$(OUTDIR)/"emwinnt.pdb" /c /Tp\
  $(SOURCE) 

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\ERRLOG.C
DEP_ERRLO=\
	.\EMACS.H\
	.\ERRLOG.H\
	.\OS.H\
	.\EM_GEN.H\
	.\EM_MAC.H\
	.\QUEUE.H\
	.\EM_TIME.H\
	.\SYNTAX.H\
	.\DISPLAY.H\
	.\MLISP.H\
	.\FILEIO.H\
	.\KEYBOARD.H\
	.\BUFFER.H\
	.\ABBREV.H\
	.\NDBM.H\
	.\TT.H\
	.\WINDOW.H\
	.\UNDO.H\
	.\JOURNAL.H\
	.\FILESERV.H\
	.\EM_RTN.H\
	.\EM_VAR.H\
	.\OS_WIN.H\
	.\OS_MAC.H

!IF  "$(CFG)" == "Win32 Release"

# ADD BASE CPP /Yu"EMACS.H"
# SUBTRACT CPP /Yc /Yu

$(INTDIR)/ERRLOG.OBJ :  $(SOURCE)  $(DEP_ERRLO) $(INTDIR)
   $(CPP) /nologo /G3 /Zp4 /MT /W4 /GX /Zi /O2 /I "u:\work\emacs\src\edit" /D\
 "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_X86_" /D "_NT" /D "_MBCS" /FR$(INTDIR)/\
 /Fo$(INTDIR)/ /Fd$(OUTDIR)/"emwinnt.pdb" /c /Tp  $(SOURCE) 

!ELSEIF  "$(CFG)" == "Win32 Debug"

# ADD BASE CPP /Yu"EMACS.H"
# SUBTRACT CPP /Yc /Yu

$(INTDIR)/ERRLOG.OBJ :  $(SOURCE)  $(DEP_ERRLO) $(INTDIR)
   $(CPP) /nologo /G3 /Zp4 /MT /W4 /GX /Zi /O2 /I "u:\work\emacs\src\edit" /D\
 "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_X86_" /D "_NT" /D "_MBCS"\
 /FR"u:\work\emacs\winnt.dbg/" /Fo$(INTDIR)/ /Fd$(OUTDIR)/"emwinnt.pdb" /c /Tp\
  $(SOURCE) 

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\FILEIO.C
DEP_FILEI=\
	.\EMACS.H\
	.\OS.H\
	.\EM_GEN.H\
	.\EM_MAC.H\
	.\QUEUE.H\
	.\EM_TIME.H\
	.\SYNTAX.H\
	.\DISPLAY.H\
	.\MLISP.H\
	.\FILEIO.H\
	.\KEYBOARD.H\
	.\BUFFER.H\
	.\ABBREV.H\
	.\NDBM.H\
	.\TT.H\
	.\WINDOW.H\
	.\UNDO.H\
	.\JOURNAL.H\
	.\FILESERV.H\
	.\EM_RTN.H\
	.\EM_VAR.H\
	.\OS_WIN.H\
	.\OS_MAC.H

!IF  "$(CFG)" == "Win32 Release"

# ADD BASE CPP /Yu"EMACS.H"
# SUBTRACT CPP /Yc /Yu

$(INTDIR)/FILEIO.OBJ :  $(SOURCE)  $(DEP_FILEI) $(INTDIR)
   $(CPP) /nologo /G3 /Zp4 /MT /W4 /GX /Zi /O2 /I "u:\work\emacs\src\edit" /D\
 "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_X86_" /D "_NT" /D "_MBCS" /FR$(INTDIR)/\
 /Fo$(INTDIR)/ /Fd$(OUTDIR)/"emwinnt.pdb" /c /Tp  $(SOURCE) 

!ELSEIF  "$(CFG)" == "Win32 Debug"

# ADD BASE CPP /Yu"EMACS.H"
# SUBTRACT CPP /Yc /Yu

$(INTDIR)/FILEIO.OBJ :  $(SOURCE)  $(DEP_FILEI) $(INTDIR)
   $(CPP) /nologo /G3 /Zp4 /MT /W4 /GX /Zi /O2 /I "u:\work\emacs\src\edit" /D\
 "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_X86_" /D "_NT" /D "_MBCS"\
 /FR"u:\work\emacs\winnt.dbg/" /Fo$(INTDIR)/ /Fd$(OUTDIR)/"emwinnt.pdb" /c /Tp\
  $(SOURCE) 

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\FILESERV.C
DEP_FILES=\
	.\EMACS.H\
	.\WIN_INCL.H\
	.\OS.H\
	.\EM_GEN.H\
	.\EM_MAC.H\
	.\QUEUE.H\
	.\EM_TIME.H\
	.\SYNTAX.H\
	.\DISPLAY.H\
	.\MLISP.H\
	.\FILEIO.H\
	.\KEYBOARD.H\
	.\BUFFER.H\
	.\ABBREV.H\
	.\NDBM.H\
	.\TT.H\
	.\WINDOW.H\
	.\UNDO.H\
	.\JOURNAL.H\
	.\FILESERV.H\
	.\EM_RTN.H\
	.\EM_VAR.H\
	.\OS_WIN.H\
	.\OS_MAC.H

!IF  "$(CFG)" == "Win32 Release"

# ADD BASE CPP /Yu"EMACS.H"
# SUBTRACT CPP /Yc /Yu

$(INTDIR)/FILESERV.OBJ :  $(SOURCE)  $(DEP_FILES) $(INTDIR)
   $(CPP) /nologo /G3 /Zp4 /MT /W4 /GX /Zi /O2 /I "u:\work\emacs\src\edit" /D\
 "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_X86_" /D "_NT" /D "_MBCS" /FR$(INTDIR)/\
 /Fo$(INTDIR)/ /Fd$(OUTDIR)/"emwinnt.pdb" /c /Tp  $(SOURCE) 

!ELSEIF  "$(CFG)" == "Win32 Debug"

# ADD BASE CPP /Yu"EMACS.H"
# SUBTRACT CPP /Yc /Yu

$(INTDIR)/FILESERV.OBJ :  $(SOURCE)  $(DEP_FILES) $(INTDIR)
   $(CPP) /nologo /G3 /Zp4 /MT /W4 /GX /Zi /O2 /I "u:\work\emacs\src\edit" /D\
 "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_X86_" /D "_NT" /D "_MBCS"\
 /FR"u:\work\emacs\winnt.dbg/" /Fo$(INTDIR)/ /Fd$(OUTDIR)/"emwinnt.pdb" /c /Tp\
  $(SOURCE) 

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\FIO.C
DEP_FIO_C=\
	.\EMACS.H\
	.\OS.H\
	.\EM_GEN.H\
	.\EM_MAC.H\
	.\QUEUE.H\
	.\EM_TIME.H\
	.\SYNTAX.H\
	.\DISPLAY.H\
	.\MLISP.H\
	.\FILEIO.H\
	.\KEYBOARD.H\
	.\BUFFER.H\
	.\ABBREV.H\
	.\NDBM.H\
	.\TT.H\
	.\WINDOW.H\
	.\UNDO.H\
	.\JOURNAL.H\
	.\FILESERV.H\
	.\EM_RTN.H\
	.\EM_VAR.H\
	.\OS_WIN.H\
	.\OS_MAC.H

!IF  "$(CFG)" == "Win32 Release"

# ADD BASE CPP /Yu"EMACS.H"
# SUBTRACT CPP /Yc /Yu

$(INTDIR)/FIO.OBJ :  $(SOURCE)  $(DEP_FIO_C) $(INTDIR)
   $(CPP) /nologo /G3 /Zp4 /MT /W4 /GX /Zi /O2 /I "u:\work\emacs\src\edit" /D\
 "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_X86_" /D "_NT" /D "_MBCS" /FR$(INTDIR)/\
 /Fo$(INTDIR)/ /Fd$(OUTDIR)/"emwinnt.pdb" /c /Tp  $(SOURCE) 

!ELSEIF  "$(CFG)" == "Win32 Debug"

# ADD BASE CPP /Yu"EMACS.H"
# SUBTRACT CPP /Yc /Yu

$(INTDIR)/FIO.OBJ :  $(SOURCE)  $(DEP_FIO_C) $(INTDIR)
   $(CPP) /nologo /G3 /Zp4 /MT /W4 /GX /Zi /O2 /I "u:\work\emacs\src\edit" /D\
 "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_X86_" /D "_NT" /D "_MBCS"\
 /FR"u:\work\emacs\winnt.dbg/" /Fo$(INTDIR)/ /Fd$(OUTDIR)/"emwinnt.pdb" /c /Tp\
  $(SOURCE) 

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\FUNCTION.C
DEP_FUNCT=\
	.\EMACS.H\
	.\FUNCLIST.H\
	.\OS.H\
	.\EM_GEN.H\
	.\EM_MAC.H\
	.\QUEUE.H\
	.\EM_TIME.H\
	.\SYNTAX.H\
	.\DISPLAY.H\
	.\MLISP.H\
	.\FILEIO.H\
	.\KEYBOARD.H\
	.\BUFFER.H\
	.\ABBREV.H\
	.\NDBM.H\
	.\TT.H\
	.\WINDOW.H\
	.\UNDO.H\
	.\JOURNAL.H\
	.\FILESERV.H\
	.\EM_RTN.H\
	.\EM_VAR.H\
	.\OS_WIN.H\
	.\OS_MAC.H

!IF  "$(CFG)" == "Win32 Release"

# ADD BASE CPP /Yu"EMACS.H"
# SUBTRACT CPP /Yc /Yu

$(INTDIR)/FUNCTION.OBJ :  $(SOURCE)  $(DEP_FUNCT) $(INTDIR)
   $(CPP) /nologo /G3 /Zp4 /MT /W4 /GX /Zi /O2 /I "u:\work\emacs\src\edit" /D\
 "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_X86_" /D "_NT" /D "_MBCS" /FR$(INTDIR)/\
 /Fo$(INTDIR)/ /Fd$(OUTDIR)/"emwinnt.pdb" /c /Tp  $(SOURCE) 

!ELSEIF  "$(CFG)" == "Win32 Debug"

# ADD BASE CPP /Yu"EMACS.H"
# SUBTRACT CPP /Yc /Yu

$(INTDIR)/FUNCTION.OBJ :  $(SOURCE)  $(DEP_FUNCT) $(INTDIR)
   $(CPP) /nologo /G3 /Zp4 /MT /W4 /GX /Zi /O2 /I "u:\work\emacs\src\edit" /D\
 "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_X86_" /D "_NT" /D "_MBCS"\
 /FR"u:\work\emacs\winnt.dbg/" /Fo$(INTDIR)/ /Fd$(OUTDIR)/"emwinnt.pdb" /c /Tp\
  $(SOURCE) 

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\GLOB_VAR.C

!IF  "$(CFG)" == "Win32 Release"

# ADD BASE CPP /Yu"EMACS.H"
# SUBTRACT CPP /Yc /Yu

$(INTDIR)/GLOB_VAR.OBJ :  $(SOURCE)  $(INTDIR)
   $(CPP) /nologo /G3 /Zp4 /MT /W4 /GX /Zi /O2 /I "u:\work\emacs\src\edit" /D\
 "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_X86_" /D "_NT" /D "_MBCS" /FR$(INTDIR)/\
 /Fo$(INTDIR)/ /Fd$(OUTDIR)/"emwinnt.pdb" /c /Tp  $(SOURCE) 

!ELSEIF  "$(CFG)" == "Win32 Debug"

# ADD BASE CPP /Yu"EMACS.H"
# SUBTRACT CPP /Yc /Yu

$(INTDIR)/GLOB_VAR.OBJ :  $(SOURCE)  $(INTDIR)
   $(CPP) /nologo /G3 /Zp4 /MT /W4 /GX /Zi /O2 /I "u:\work\emacs\src\edit" /D\
 "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_X86_" /D "_NT" /D "_MBCS"\
 /FR"u:\work\emacs\winnt.dbg/" /Fo$(INTDIR)/ /Fd$(OUTDIR)/"emwinnt.pdb" /c /Tp\
  $(SOURCE) 

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\JOURNAL.C

!IF  "$(CFG)" == "Win32 Release"

# ADD BASE CPP /Yu"EMACS.H"
# SUBTRACT CPP /Yc /Yu

$(INTDIR)/JOURNAL.OBJ :  $(SOURCE)  $(INTDIR)
   $(CPP) /nologo /G3 /Zp4 /MT /W4 /GX /Zi /O2 /I "u:\work\emacs\src\edit" /D\
 "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_X86_" /D "_NT" /D "_MBCS" /FR$(INTDIR)/\
 /Fo$(INTDIR)/ /Fd$(OUTDIR)/"emwinnt.pdb" /c /Tp  $(SOURCE) 

!ELSEIF  "$(CFG)" == "Win32 Debug"

# ADD BASE CPP /Yu"EMACS.H"
# SUBTRACT CPP /Yc /Yu

$(INTDIR)/JOURNAL.OBJ :  $(SOURCE)  $(INTDIR)
   $(CPP) /nologo /G3 /Zp4 /MT /W4 /GX /Zi /O2 /I "u:\work\emacs\src\edit" /D\
 "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_X86_" /D "_NT" /D "_MBCS"\
 /FR"u:\work\emacs\winnt.dbg/" /Fo$(INTDIR)/ /Fd$(OUTDIR)/"emwinnt.pdb" /c /Tp\
  $(SOURCE) 

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\KEYBOARD.C

!IF  "$(CFG)" == "Win32 Release"

# ADD BASE CPP /Yu"EMACS.H"
# SUBTRACT CPP /Yc /Yu

$(INTDIR)/KEYBOARD.OBJ :  $(SOURCE)  $(INTDIR)
   $(CPP) /nologo /G3 /Zp4 /MT /W4 /GX /Zi /O2 /I "u:\work\emacs\src\edit" /D\
 "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_X86_" /D "_NT" /D "_MBCS" /FR$(INTDIR)/\
 /Fo$(INTDIR)/ /Fd$(OUTDIR)/"emwinnt.pdb" /c /Tp  $(SOURCE) 

!ELSEIF  "$(CFG)" == "Win32 Debug"

# ADD BASE CPP /Yu"EMACS.H"
# SUBTRACT CPP /Yc /Yu

$(INTDIR)/KEYBOARD.OBJ :  $(SOURCE)  $(INTDIR)
   $(CPP) /nologo /G3 /Zp4 /MT /W4 /GX /Zi /O2 /I "u:\work\emacs\src\edit" /D\
 "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_X86_" /D "_NT" /D "_MBCS"\
 /FR"u:\work\emacs\winnt.dbg/" /Fo$(INTDIR)/ /Fd$(OUTDIR)/"emwinnt.pdb" /c /Tp\
  $(SOURCE) 

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\LISPFUNC.C

!IF  "$(CFG)" == "Win32 Release"

# ADD BASE CPP /Yu"EMACS.H"
# SUBTRACT CPP /Yc /Yu

$(INTDIR)/LISPFUNC.OBJ :  $(SOURCE)  $(INTDIR)
   $(CPP) /nologo /G3 /Zp4 /MT /W4 /GX /Zi /O2 /I "u:\work\emacs\src\edit" /D\
 "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_X86_" /D "_NT" /D "_MBCS" /FR$(INTDIR)/\
 /Fo$(INTDIR)/ /Fd$(OUTDIR)/"emwinnt.pdb" /c /Tp  $(SOURCE) 

!ELSEIF  "$(CFG)" == "Win32 Debug"

# ADD BASE CPP /Yu"EMACS.H"
# SUBTRACT CPP /Yc /Yu

$(INTDIR)/LISPFUNC.OBJ :  $(SOURCE)  $(INTDIR)
   $(CPP) /nologo /G3 /Zp4 /MT /W4 /GX /Zi /O2 /I "u:\work\emacs\src\edit" /D\
 "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_X86_" /D "_NT" /D "_MBCS"\
 /FR"u:\work\emacs\winnt.dbg/" /Fo$(INTDIR)/ /Fd$(OUTDIR)/"emwinnt.pdb" /c /Tp\
  $(SOURCE) 

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\MACROS.C
DEP_MACRO=\
	.\EMACS.H\
	.\OS.H\
	.\EM_GEN.H\
	.\EM_MAC.H\
	.\QUEUE.H\
	.\EM_TIME.H\
	.\SYNTAX.H\
	.\DISPLAY.H\
	.\MLISP.H\
	.\FILEIO.H\
	.\KEYBOARD.H\
	.\BUFFER.H\
	.\ABBREV.H\
	.\NDBM.H\
	.\TT.H\
	.\WINDOW.H\
	.\UNDO.H\
	.\JOURNAL.H\
	.\FILESERV.H\
	.\EM_RTN.H\
	.\EM_VAR.H\
	.\OS_WIN.H\
	.\OS_MAC.H

!IF  "$(CFG)" == "Win32 Release"

# ADD BASE CPP /Yu"EMACS.H"
# SUBTRACT CPP /Yc /Yu

$(INTDIR)/MACROS.OBJ :  $(SOURCE)  $(DEP_MACRO) $(INTDIR)
   $(CPP) /nologo /G3 /Zp4 /MT /W4 /GX /Zi /O2 /I "u:\work\emacs\src\edit" /D\
 "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_X86_" /D "_NT" /D "_MBCS" /FR$(INTDIR)/\
 /Fo$(INTDIR)/ /Fd$(OUTDIR)/"emwinnt.pdb" /c /Tp  $(SOURCE) 

!ELSEIF  "$(CFG)" == "Win32 Debug"

# ADD BASE CPP /Yu"EMACS.H"
# SUBTRACT CPP /Yc /Yu

$(INTDIR)/MACROS.OBJ :  $(SOURCE)  $(DEP_MACRO) $(INTDIR)
   $(CPP) /nologo /G3 /Zp4 /MT /W4 /GX /Zi /O2 /I "u:\work\emacs\src\edit" /D\
 "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_X86_" /D "_NT" /D "_MBCS"\
 /FR"u:\work\emacs\winnt.dbg/" /Fo$(INTDIR)/ /Fd$(OUTDIR)/"emwinnt.pdb" /c /Tp\
  $(SOURCE) 

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\MEM_MAN.C
DEP_MEM_M=\
	.\EMACS.H\
	.\ERRLOG.H\
	.\VMS_COMM.H\
	.\UNIXCOMM.H\
	.\NT_COMM.H\
	.\OS.H\
	.\EM_GEN.H\
	.\EM_MAC.H\
	.\QUEUE.H\
	.\EM_TIME.H\
	.\SYNTAX.H\
	.\DISPLAY.H\
	.\MLISP.H\
	.\FILEIO.H\
	.\KEYBOARD.H\
	.\BUFFER.H\
	.\ABBREV.H\
	.\NDBM.H\
	.\TT.H\
	.\WINDOW.H\
	.\UNDO.H\
	.\JOURNAL.H\
	.\FILESERV.H\
	.\EM_RTN.H\
	.\EM_VAR.H\
	.\NT_ASYNC.H\
	.\NT_PIPE.H\
	.\NT_PROC.H\
	.\OS_WIN.H\
	.\OS_MAC.H

!IF  "$(CFG)" == "Win32 Release"

# ADD BASE CPP /Yu"EMACS.H"
# SUBTRACT CPP /Yc /Yu

$(INTDIR)/MEM_MAN.OBJ :  $(SOURCE)  $(DEP_MEM_M) $(INTDIR)
   $(CPP) /nologo /G3 /Zp4 /MT /W4 /GX /Zi /O2 /I "u:\work\emacs\src\edit" /D\
 "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_X86_" /D "_NT" /D "_MBCS" /FR$(INTDIR)/\
 /Fo$(INTDIR)/ /Fd$(OUTDIR)/"emwinnt.pdb" /c /Tp  $(SOURCE) 

!ELSEIF  "$(CFG)" == "Win32 Debug"

# ADD BASE CPP /Yu"EMACS.H"
# SUBTRACT CPP /Yc /Yu

$(INTDIR)/MEM_MAN.OBJ :  $(SOURCE)  $(DEP_MEM_M) $(INTDIR)
   $(CPP) /nologo /G3 /Zp4 /MT /W4 /GX /Zi /O2 /I "u:\work\emacs\src\edit" /D\
 "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_X86_" /D "_NT" /D "_MBCS"\
 /FR"u:\work\emacs\winnt.dbg/" /Fo$(INTDIR)/ /Fd$(OUTDIR)/"emwinnt.pdb" /c /Tp\
  $(SOURCE) 

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\METACOMM.C
DEP_METAC=\
	.\EMACS.H\
	.\OS.H\
	.\EM_GEN.H\
	.\EM_MAC.H\
	.\QUEUE.H\
	.\EM_TIME.H\
	.\SYNTAX.H\
	.\DISPLAY.H\
	.\MLISP.H\
	.\FILEIO.H\
	.\KEYBOARD.H\
	.\BUFFER.H\
	.\ABBREV.H\
	.\NDBM.H\
	.\TT.H\
	.\WINDOW.H\
	.\UNDO.H\
	.\JOURNAL.H\
	.\FILESERV.H\
	.\EM_RTN.H\
	.\EM_VAR.H\
	.\OS_WIN.H\
	.\OS_MAC.H

!IF  "$(CFG)" == "Win32 Release"

# ADD BASE CPP /Yu"EMACS.H"
# SUBTRACT CPP /Yc /Yu

$(INTDIR)/METACOMM.OBJ :  $(SOURCE)  $(DEP_METAC) $(INTDIR)
   $(CPP) /nologo /G3 /Zp4 /MT /W4 /GX /Zi /O2 /I "u:\work\emacs\src\edit" /D\
 "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_X86_" /D "_NT" /D "_MBCS" /FR$(INTDIR)/\
 /Fo$(INTDIR)/ /Fd$(OUTDIR)/"emwinnt.pdb" /c /Tp  $(SOURCE) 

!ELSEIF  "$(CFG)" == "Win32 Debug"

# ADD BASE CPP /Yu"EMACS.H"
# SUBTRACT CPP /Yc /Yu

$(INTDIR)/METACOMM.OBJ :  $(SOURCE)  $(DEP_METAC) $(INTDIR)
   $(CPP) /nologo /G3 /Zp4 /MT /W4 /GX /Zi /O2 /I "u:\work\emacs\src\edit" /D\
 "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_X86_" /D "_NT" /D "_MBCS"\
 /FR"u:\work\emacs\winnt.dbg/" /Fo$(INTDIR)/ /Fd$(OUTDIR)/"emwinnt.pdb" /c /Tp\
  $(SOURCE) 

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\MINIBUF.C
DEP_MINIB=\
	.\EMACS.H\
	.\OS.H\
	.\EM_GEN.H\
	.\EM_MAC.H\
	.\QUEUE.H\
	.\EM_TIME.H\
	.\SYNTAX.H\
	.\DISPLAY.H\
	.\MLISP.H\
	.\FILEIO.H\
	.\KEYBOARD.H\
	.\BUFFER.H\
	.\ABBREV.H\
	.\NDBM.H\
	.\TT.H\
	.\WINDOW.H\
	.\UNDO.H\
	.\JOURNAL.H\
	.\FILESERV.H\
	.\EM_RTN.H\
	.\EM_VAR.H\
	.\OS_WIN.H\
	.\OS_MAC.H

!IF  "$(CFG)" == "Win32 Release"

# ADD BASE CPP /Yu"EMACS.H"
# SUBTRACT CPP /Yc /Yu

$(INTDIR)/MINIBUF.OBJ :  $(SOURCE)  $(DEP_MINIB) $(INTDIR)
   $(CPP) /nologo /G3 /Zp4 /MT /W4 /GX /Zi /O2 /I "u:\work\emacs\src\edit" /D\
 "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_X86_" /D "_NT" /D "_MBCS" /FR$(INTDIR)/\
 /Fo$(INTDIR)/ /Fd$(OUTDIR)/"emwinnt.pdb" /c /Tp  $(SOURCE) 

!ELSEIF  "$(CFG)" == "Win32 Debug"

# ADD BASE CPP /Yu"EMACS.H"
# SUBTRACT CPP /Yc /Yu

$(INTDIR)/MINIBUF.OBJ :  $(SOURCE)  $(DEP_MINIB) $(INTDIR)
   $(CPP) /nologo /G3 /Zp4 /MT /W4 /GX /Zi /O2 /I "u:\work\emacs\src\edit" /D\
 "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_X86_" /D "_NT" /D "_MBCS"\
 /FR"u:\work\emacs\winnt.dbg/" /Fo$(INTDIR)/ /Fd$(OUTDIR)/"emwinnt.pdb" /c /Tp\
  $(SOURCE) 

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\MLISPARS.C
DEP_MLISP=\
	.\EMACS.H\
	.\OS.H\
	.\EM_GEN.H\
	.\EM_MAC.H\
	.\QUEUE.H\
	.\EM_TIME.H\
	.\SYNTAX.H\
	.\DISPLAY.H\
	.\MLISP.H\
	.\FILEIO.H\
	.\KEYBOARD.H\
	.\BUFFER.H\
	.\ABBREV.H\
	.\NDBM.H\
	.\TT.H\
	.\WINDOW.H\
	.\UNDO.H\
	.\JOURNAL.H\
	.\FILESERV.H\
	.\EM_RTN.H\
	.\EM_VAR.H\
	.\OS_WIN.H\
	.\OS_MAC.H

!IF  "$(CFG)" == "Win32 Release"

# ADD BASE CPP /Yu"EMACS.H"
# SUBTRACT CPP /Yc /Yu

$(INTDIR)/MLISPARS.OBJ :  $(SOURCE)  $(DEP_MLISP) $(INTDIR)
   $(CPP) /nologo /G3 /Zp4 /MT /W4 /GX /Zi /O2 /I "u:\work\emacs\src\edit" /D\
 "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_X86_" /D "_NT" /D "_MBCS" /FR$(INTDIR)/\
 /Fo$(INTDIR)/ /Fd$(OUTDIR)/"emwinnt.pdb" /c /Tp  $(SOURCE) 

!ELSEIF  "$(CFG)" == "Win32 Debug"

# ADD BASE CPP /Yu"EMACS.H"
# SUBTRACT CPP /Yc /Yu

$(INTDIR)/MLISPARS.OBJ :  $(SOURCE)  $(DEP_MLISP) $(INTDIR)
   $(CPP) /nologo /G3 /Zp4 /MT /W4 /GX /Zi /O2 /I "u:\work\emacs\src\edit" /D\
 "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_X86_" /D "_NT" /D "_MBCS"\
 /FR"u:\work\emacs\winnt.dbg/" /Fo$(INTDIR)/ /Fd$(OUTDIR)/"emwinnt.pdb" /c /Tp\
  $(SOURCE) 

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\MLISPROC.C

!IF  "$(CFG)" == "Win32 Release"

# ADD BASE CPP /Yu"EMACS.H"
# SUBTRACT CPP /Yc /Yu

$(INTDIR)/MLISPROC.OBJ :  $(SOURCE)  $(INTDIR)
   $(CPP) /nologo /G3 /Zp4 /MT /W4 /GX /Zi /O2 /I "u:\work\emacs\src\edit" /D\
 "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_X86_" /D "_NT" /D "_MBCS" /FR$(INTDIR)/\
 /Fo$(INTDIR)/ /Fd$(OUTDIR)/"emwinnt.pdb" /c /Tp  $(SOURCE) 

!ELSEIF  "$(CFG)" == "Win32 Debug"

# ADD BASE CPP /Yu"EMACS.H"
# SUBTRACT CPP /Yc /Yu

$(INTDIR)/MLISPROC.OBJ :  $(SOURCE)  $(INTDIR)
   $(CPP) /nologo /G3 /Zp4 /MT /W4 /GX /Zi /O2 /I "u:\work\emacs\src\edit" /D\
 "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_X86_" /D "_NT" /D "_MBCS"\
 /FR"u:\work\emacs\winnt.dbg/" /Fo$(INTDIR)/ /Fd$(OUTDIR)/"emwinnt.pdb" /c /Tp\
  $(SOURCE) 

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\NDBM.C
DEP_NDBM_=\
	.\EMACS.H\
	.\OS.H\
	.\EM_GEN.H\
	.\EM_MAC.H\
	.\QUEUE.H\
	.\EM_TIME.H\
	.\SYNTAX.H\
	.\DISPLAY.H\
	.\MLISP.H\
	.\FILEIO.H\
	.\KEYBOARD.H\
	.\BUFFER.H\
	.\ABBREV.H\
	.\NDBM.H\
	.\TT.H\
	.\WINDOW.H\
	.\UNDO.H\
	.\JOURNAL.H\
	.\FILESERV.H\
	.\EM_RTN.H\
	.\EM_VAR.H\
	.\OS_WIN.H\
	.\OS_MAC.H

!IF  "$(CFG)" == "Win32 Release"

# ADD BASE CPP /Yu"EMACS.H"
# SUBTRACT CPP /Yc /Yu

$(INTDIR)/NDBM.OBJ :  $(SOURCE)  $(DEP_NDBM_) $(INTDIR)
   $(CPP) /nologo /G3 /Zp4 /MT /W4 /GX /Zi /O2 /I "u:\work\emacs\src\edit" /D\
 "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_X86_" /D "_NT" /D "_MBCS" /FR$(INTDIR)/\
 /Fo$(INTDIR)/ /Fd$(OUTDIR)/"emwinnt.pdb" /c /Tp  $(SOURCE) 

!ELSEIF  "$(CFG)" == "Win32 Debug"

# ADD BASE CPP /Yu"EMACS.H"
# SUBTRACT CPP /Yc /Yu

$(INTDIR)/NDBM.OBJ :  $(SOURCE)  $(DEP_NDBM_) $(INTDIR)
   $(CPP) /nologo /G3 /Zp4 /MT /W4 /GX /Zi /O2 /I "u:\work\emacs\src\edit" /D\
 "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_X86_" /D "_NT" /D "_MBCS"\
 /FR"u:\work\emacs\winnt.dbg/" /Fo$(INTDIR)/ /Fd$(OUTDIR)/"emwinnt.pdb" /c /Tp\
  $(SOURCE) 

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\OPTIONS.C
DEP_OPTIO=\
	.\EMACS.H\
	.\OS.H\
	.\EM_GEN.H\
	.\EM_MAC.H\
	.\QUEUE.H\
	.\EM_TIME.H\
	.\SYNTAX.H\
	.\DISPLAY.H\
	.\MLISP.H\
	.\FILEIO.H\
	.\KEYBOARD.H\
	.\BUFFER.H\
	.\ABBREV.H\
	.\NDBM.H\
	.\TT.H\
	.\WINDOW.H\
	.\UNDO.H\
	.\JOURNAL.H\
	.\FILESERV.H\
	.\EM_RTN.H\
	.\EM_VAR.H\
	.\OS_WIN.H\
	.\OS_MAC.H

!IF  "$(CFG)" == "Win32 Release"

# ADD BASE CPP /Yu"EMACS.H"
# SUBTRACT CPP /Yc /Yu

$(INTDIR)/OPTIONS.OBJ :  $(SOURCE)  $(DEP_OPTIO) $(INTDIR)
   $(CPP) /nologo /G3 /Zp4 /MT /W4 /GX /Zi /O2 /I "u:\work\emacs\src\edit" /D\
 "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_X86_" /D "_NT" /D "_MBCS" /FR$(INTDIR)/\
 /Fo$(INTDIR)/ /Fd$(OUTDIR)/"emwinnt.pdb" /c /Tp  $(SOURCE) 

!ELSEIF  "$(CFG)" == "Win32 Debug"

# ADD BASE CPP /Yu"EMACS.H"
# SUBTRACT CPP /Yc /Yu

$(INTDIR)/OPTIONS.OBJ :  $(SOURCE)  $(DEP_OPTIO) $(INTDIR)
   $(CPP) /nologo /G3 /Zp4 /MT /W4 /GX /Zi /O2 /I "u:\work\emacs\src\edit" /D\
 "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_X86_" /D "_NT" /D "_MBCS"\
 /FR"u:\work\emacs\winnt.dbg/" /Fo$(INTDIR)/ /Fd$(OUTDIR)/"emwinnt.pdb" /c /Tp\
  $(SOURCE) 

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\PROMPT.C
DEP_PROMP=\
	.\EMACS.H\
	.\OS.H\
	.\EM_GEN.H\
	.\EM_MAC.H\
	.\QUEUE.H\
	.\EM_TIME.H\
	.\SYNTAX.H\
	.\DISPLAY.H\
	.\MLISP.H\
	.\FILEIO.H\
	.\KEYBOARD.H\
	.\BUFFER.H\
	.\ABBREV.H\
	.\NDBM.H\
	.\TT.H\
	.\WINDOW.H\
	.\UNDO.H\
	.\JOURNAL.H\
	.\FILESERV.H\
	.\EM_RTN.H\
	.\EM_VAR.H\
	.\OS_WIN.H\
	.\OS_MAC.H

!IF  "$(CFG)" == "Win32 Release"

# ADD BASE CPP /Yu"EMACS.H"
# SUBTRACT CPP /Yc /Yu

$(INTDIR)/PROMPT.OBJ :  $(SOURCE)  $(DEP_PROMP) $(INTDIR)
   $(CPP) /nologo /G3 /Zp4 /MT /W4 /GX /Zi /O2 /I "u:\work\emacs\src\edit" /D\
 "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_X86_" /D "_NT" /D "_MBCS" /FR$(INTDIR)/\
 /Fo$(INTDIR)/ /Fd$(OUTDIR)/"emwinnt.pdb" /c /Tp  $(SOURCE) 

!ELSEIF  "$(CFG)" == "Win32 Debug"

# ADD BASE CPP /Yu"EMACS.H"
# SUBTRACT CPP /Yc /Yu

$(INTDIR)/PROMPT.OBJ :  $(SOURCE)  $(DEP_PROMP) $(INTDIR)
   $(CPP) /nologo /G3 /Zp4 /MT /W4 /GX /Zi /O2 /I "u:\work\emacs\src\edit" /D\
 "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_X86_" /D "_NT" /D "_MBCS"\
 /FR"u:\work\emacs\winnt.dbg/" /Fo$(INTDIR)/ /Fd$(OUTDIR)/"emwinnt.pdb" /c /Tp\
  $(SOURCE) 

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\QUEUE.C
DEP_QUEUE=\
	.\EMACS.H\
	.\OS.H\
	.\EM_GEN.H\
	.\EM_MAC.H\
	.\QUEUE.H\
	.\EM_TIME.H\
	.\SYNTAX.H\
	.\DISPLAY.H\
	.\MLISP.H\
	.\FILEIO.H\
	.\KEYBOARD.H\
	.\BUFFER.H\
	.\ABBREV.H\
	.\NDBM.H\
	.\TT.H\
	.\WINDOW.H\
	.\UNDO.H\
	.\JOURNAL.H\
	.\FILESERV.H\
	.\EM_RTN.H\
	.\EM_VAR.H\
	.\OS_WIN.H\
	.\OS_MAC.H

!IF  "$(CFG)" == "Win32 Release"

# ADD BASE CPP /Yu"EMACS.H"
# SUBTRACT CPP /Yc /Yu

$(INTDIR)/QUEUE.OBJ :  $(SOURCE)  $(DEP_QUEUE) $(INTDIR)
   $(CPP) /nologo /G3 /Zp4 /MT /W4 /GX /Zi /O2 /I "u:\work\emacs\src\edit" /D\
 "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_X86_" /D "_NT" /D "_MBCS" /FR$(INTDIR)/\
 /Fo$(INTDIR)/ /Fd$(OUTDIR)/"emwinnt.pdb" /c /Tp  $(SOURCE) 

!ELSEIF  "$(CFG)" == "Win32 Debug"

# ADD BASE CPP /Yu"EMACS.H"
# SUBTRACT CPP /Yc /Yu

$(INTDIR)/QUEUE.OBJ :  $(SOURCE)  $(DEP_QUEUE) $(INTDIR)
   $(CPP) /nologo /G3 /Zp4 /MT /W4 /GX /Zi /O2 /I "u:\work\emacs\src\edit" /D\
 "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_X86_" /D "_NT" /D "_MBCS"\
 /FR"u:\work\emacs\winnt.dbg/" /Fo$(INTDIR)/ /Fd$(OUTDIR)/"emwinnt.pdb" /c /Tp\
  $(SOURCE) 

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\SEARCH.C
DEP_SEARC=\
	.\EMACS.H\
	.\OS.H\
	.\EM_GEN.H\
	.\EM_MAC.H\
	.\QUEUE.H\
	.\EM_TIME.H\
	.\SYNTAX.H\
	.\DISPLAY.H\
	.\MLISP.H\
	.\FILEIO.H\
	.\KEYBOARD.H\
	.\BUFFER.H\
	.\ABBREV.H\
	.\NDBM.H\
	.\TT.H\
	.\WINDOW.H\
	.\UNDO.H\
	.\JOURNAL.H\
	.\FILESERV.H\
	.\EM_RTN.H\
	.\EM_VAR.H\
	.\OS_WIN.H\
	.\OS_MAC.H

!IF  "$(CFG)" == "Win32 Release"

# ADD BASE CPP /Yu"EMACS.H"
# SUBTRACT CPP /Yc /Yu

$(INTDIR)/SEARCH.OBJ :  $(SOURCE)  $(DEP_SEARC) $(INTDIR)
   $(CPP) /nologo /G3 /Zp4 /MT /W4 /GX /Zi /O2 /I "u:\work\emacs\src\edit" /D\
 "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_X86_" /D "_NT" /D "_MBCS" /FR$(INTDIR)/\
 /Fo$(INTDIR)/ /Fd$(OUTDIR)/"emwinnt.pdb" /c /Tp  $(SOURCE) 

!ELSEIF  "$(CFG)" == "Win32 Debug"

# ADD BASE CPP /Yu"EMACS.H"
# SUBTRACT CPP /Yc /Yu

$(INTDIR)/SEARCH.OBJ :  $(SOURCE)  $(DEP_SEARC) $(INTDIR)
   $(CPP) /nologo /G3 /Zp4 /MT /W4 /GX /Zi /O2 /I "u:\work\emacs\src\edit" /D\
 "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_X86_" /D "_NT" /D "_MBCS"\
 /FR"u:\work\emacs\winnt.dbg/" /Fo$(INTDIR)/ /Fd$(OUTDIR)/"emwinnt.pdb" /c /Tp\
  $(SOURCE) 

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\SEARCHIN.C

!IF  "$(CFG)" == "Win32 Release"

# ADD BASE CPP /Yu"EMACS.H"
# SUBTRACT CPP /Yc /Yu

$(INTDIR)/SEARCHIN.OBJ :  $(SOURCE)  $(INTDIR)
   $(CPP) /nologo /G3 /Zp4 /MT /W4 /GX /Zi /O2 /I "u:\work\emacs\src\edit" /D\
 "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_X86_" /D "_NT" /D "_MBCS" /FR$(INTDIR)/\
 /Fo$(INTDIR)/ /Fd$(OUTDIR)/"emwinnt.pdb" /c /Tp  $(SOURCE) 

!ELSEIF  "$(CFG)" == "Win32 Debug"

# ADD BASE CPP /Yu"EMACS.H"
# SUBTRACT CPP /Yc /Yu

$(INTDIR)/SEARCHIN.OBJ :  $(SOURCE)  $(INTDIR)
   $(CPP) /nologo /G3 /Zp4 /MT /W4 /GX /Zi /O2 /I "u:\work\emacs\src\edit" /D\
 "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_X86_" /D "_NT" /D "_MBCS"\
 /FR"u:\work\emacs\winnt.dbg/" /Fo$(INTDIR)/ /Fd$(OUTDIR)/"emwinnt.pdb" /c /Tp\
  $(SOURCE) 

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\SIMPCOMM.C
DEP_SIMPC=\
	.\EMACS.H\
	.\OS.H\
	.\EM_GEN.H\
	.\EM_MAC.H\
	.\QUEUE.H\
	.\EM_TIME.H\
	.\SYNTAX.H\
	.\DISPLAY.H\
	.\MLISP.H\
	.\FILEIO.H\
	.\KEYBOARD.H\
	.\BUFFER.H\
	.\ABBREV.H\
	.\NDBM.H\
	.\TT.H\
	.\WINDOW.H\
	.\UNDO.H\
	.\JOURNAL.H\
	.\FILESERV.H\
	.\EM_RTN.H\
	.\EM_VAR.H\
	.\OS_WIN.H\
	.\OS_MAC.H

!IF  "$(CFG)" == "Win32 Release"

# ADD BASE CPP /Yu"EMACS.H"
# SUBTRACT CPP /Yc /Yu

$(INTDIR)/SIMPCOMM.OBJ :  $(SOURCE)  $(DEP_SIMPC) $(INTDIR)
   $(CPP) /nologo /G3 /Zp4 /MT /W4 /GX /Zi /O2 /I "u:\work\emacs\src\edit" /D\
 "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_X86_" /D "_NT" /D "_MBCS" /FR$(INTDIR)/\
 /Fo$(INTDIR)/ /Fd$(OUTDIR)/"emwinnt.pdb" /c /Tp  $(SOURCE) 

!ELSEIF  "$(CFG)" == "Win32 Debug"

# ADD BASE CPP /Yu"EMACS.H"
# SUBTRACT CPP /Yc /Yu

$(INTDIR)/SIMPCOMM.OBJ :  $(SOURCE)  $(DEP_SIMPC) $(INTDIR)
   $(CPP) /nologo /G3 /Zp4 /MT /W4 /GX /Zi /O2 /I "u:\work\emacs\src\edit" /D\
 "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_X86_" /D "_NT" /D "_MBCS"\
 /FR"u:\work\emacs\winnt.dbg/" /Fo$(INTDIR)/ /Fd$(OUTDIR)/"emwinnt.pdb" /c /Tp\
  $(SOURCE) 

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\SUBPROC.C
DEP_SUBPR=\
	.\EMACS.H\
	.\WIN_INCL.H\
	.\OS.H\
	.\EM_GEN.H\
	.\EM_MAC.H\
	.\QUEUE.H\
	.\EM_TIME.H\
	.\SYNTAX.H\
	.\DISPLAY.H\
	.\MLISP.H\
	.\FILEIO.H\
	.\KEYBOARD.H\
	.\BUFFER.H\
	.\ABBREV.H\
	.\NDBM.H\
	.\TT.H\
	.\WINDOW.H\
	.\UNDO.H\
	.\JOURNAL.H\
	.\FILESERV.H\
	.\EM_RTN.H\
	.\EM_VAR.H\
	.\OS_WIN.H\
	.\OS_MAC.H

!IF  "$(CFG)" == "Win32 Release"

# ADD BASE CPP /Yu"EMACS.H"
# SUBTRACT CPP /Yc /Yu

$(INTDIR)/SUBPROC.OBJ :  $(SOURCE)  $(DEP_SUBPR) $(INTDIR)
   $(CPP) /nologo /G3 /Zp4 /MT /W4 /GX /Zi /O2 /I "u:\work\emacs\src\edit" /D\
 "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_X86_" /D "_NT" /D "_MBCS" /FR$(INTDIR)/\
 /Fo$(INTDIR)/ /Fd$(OUTDIR)/"emwinnt.pdb" /c /Tp  $(SOURCE) 

!ELSEIF  "$(CFG)" == "Win32 Debug"

# ADD BASE CPP /Yu"EMACS.H"
# SUBTRACT CPP /Yc /Yu

$(INTDIR)/SUBPROC.OBJ :  $(SOURCE)  $(DEP_SUBPR) $(INTDIR)
   $(CPP) /nologo /G3 /Zp4 /MT /W4 /GX /Zi /O2 /I "u:\work\emacs\src\edit" /D\
 "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_X86_" /D "_NT" /D "_MBCS"\
 /FR"u:\work\emacs\winnt.dbg/" /Fo$(INTDIR)/ /Fd$(OUTDIR)/"emwinnt.pdb" /c /Tp\
  $(SOURCE) 

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\SYNTAX.C
DEP_SYNTA=\
	.\EMACS.H\
	.\OS.H\
	.\EM_GEN.H\
	.\EM_MAC.H\
	.\QUEUE.H\
	.\EM_TIME.H\
	.\SYNTAX.H\
	.\DISPLAY.H\
	.\MLISP.H\
	.\FILEIO.H\
	.\KEYBOARD.H\
	.\BUFFER.H\
	.\ABBREV.H\
	.\NDBM.H\
	.\TT.H\
	.\WINDOW.H\
	.\UNDO.H\
	.\JOURNAL.H\
	.\FILESERV.H\
	.\EM_RTN.H\
	.\EM_VAR.H\
	.\OS_WIN.H\
	.\OS_MAC.H

!IF  "$(CFG)" == "Win32 Release"

# ADD BASE CPP /Yu"EMACS.H"
# SUBTRACT CPP /Yc /Yu

$(INTDIR)/SYNTAX.OBJ :  $(SOURCE)  $(DEP_SYNTA) $(INTDIR)
   $(CPP) /nologo /G3 /Zp4 /MT /W4 /GX /Zi /O2 /I "u:\work\emacs\src\edit" /D\
 "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_X86_" /D "_NT" /D "_MBCS" /FR$(INTDIR)/\
 /Fo$(INTDIR)/ /Fd$(OUTDIR)/"emwinnt.pdb" /c /Tp  $(SOURCE) 

!ELSEIF  "$(CFG)" == "Win32 Debug"

# ADD BASE CPP /Yu"EMACS.H"
# SUBTRACT CPP /Yc /Yu

$(INTDIR)/SYNTAX.OBJ :  $(SOURCE)  $(DEP_SYNTA) $(INTDIR)
   $(CPP) /nologo /G3 /Zp4 /MT /W4 /GX /Zi /O2 /I "u:\work\emacs\src\edit" /D\
 "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_X86_" /D "_NT" /D "_MBCS"\
 /FR"u:\work\emacs\winnt.dbg/" /Fo$(INTDIR)/ /Fd$(OUTDIR)/"emwinnt.pdb" /c /Tp\
  $(SOURCE) 

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\TERM.C
DEP_TERM_=\
	.\EMACS.H\
	.\OS.H\
	.\EM_GEN.H\
	.\EM_MAC.H\
	.\QUEUE.H\
	.\EM_TIME.H\
	.\SYNTAX.H\
	.\DISPLAY.H\
	.\MLISP.H\
	.\FILEIO.H\
	.\KEYBOARD.H\
	.\BUFFER.H\
	.\ABBREV.H\
	.\NDBM.H\
	.\TT.H\
	.\WINDOW.H\
	.\UNDO.H\
	.\JOURNAL.H\
	.\FILESERV.H\
	.\EM_RTN.H\
	.\EM_VAR.H\
	.\OS_WIN.H\
	.\OS_MAC.H

!IF  "$(CFG)" == "Win32 Release"

# ADD BASE CPP /Yu"EMACS.H"
# SUBTRACT CPP /Yc /Yu

$(INTDIR)/TERM.OBJ :  $(SOURCE)  $(DEP_TERM_) $(INTDIR)
   $(CPP) /nologo /G3 /Zp4 /MT /W4 /GX /Zi /O2 /I "u:\work\emacs\src\edit" /D\
 "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_X86_" /D "_NT" /D "_MBCS" /FR$(INTDIR)/\
 /Fo$(INTDIR)/ /Fd$(OUTDIR)/"emwinnt.pdb" /c /Tp  $(SOURCE) 

!ELSEIF  "$(CFG)" == "Win32 Debug"

# ADD BASE CPP /Yu"EMACS.H"
# SUBTRACT CPP /Yc /Yu

$(INTDIR)/TERM.OBJ :  $(SOURCE)  $(DEP_TERM_) $(INTDIR)
   $(CPP) /nologo /G3 /Zp4 /MT /W4 /GX /Zi /O2 /I "u:\work\emacs\src\edit" /D\
 "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_X86_" /D "_NT" /D "_MBCS"\
 /FR"u:\work\emacs\winnt.dbg/" /Fo$(INTDIR)/ /Fd$(OUTDIR)/"emwinnt.pdb" /c /Tp\
  $(SOURCE) 

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\TIMER.C
DEP_TIMER=\
	.\EMACS.H\
	.\OS.H\
	.\EM_GEN.H\
	.\EM_MAC.H\
	.\QUEUE.H\
	.\EM_TIME.H\
	.\SYNTAX.H\
	.\DISPLAY.H\
	.\MLISP.H\
	.\FILEIO.H\
	.\KEYBOARD.H\
	.\BUFFER.H\
	.\ABBREV.H\
	.\NDBM.H\
	.\TT.H\
	.\WINDOW.H\
	.\UNDO.H\
	.\JOURNAL.H\
	.\FILESERV.H\
	.\EM_RTN.H\
	.\EM_VAR.H\
	.\OS_WIN.H\
	.\OS_MAC.H

!IF  "$(CFG)" == "Win32 Release"

# ADD BASE CPP /Yu"EMACS.H"
# SUBTRACT CPP /Yc /Yu

$(INTDIR)/TIMER.OBJ :  $(SOURCE)  $(DEP_TIMER) $(INTDIR)
   $(CPP) /nologo /G3 /Zp4 /MT /W4 /GX /Zi /O2 /I "u:\work\emacs\src\edit" /D\
 "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_X86_" /D "_NT" /D "_MBCS" /FR$(INTDIR)/\
 /Fo$(INTDIR)/ /Fd$(OUTDIR)/"emwinnt.pdb" /c /Tp  $(SOURCE) 

!ELSEIF  "$(CFG)" == "Win32 Debug"

# ADD BASE CPP /Yu"EMACS.H"
# SUBTRACT CPP /Yc /Yu

$(INTDIR)/TIMER.OBJ :  $(SOURCE)  $(DEP_TIMER) $(INTDIR)
   $(CPP) /nologo /G3 /Zp4 /MT /W4 /GX /Zi /O2 /I "u:\work\emacs\src\edit" /D\
 "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_X86_" /D "_NT" /D "_MBCS"\
 /FR"u:\work\emacs\winnt.dbg/" /Fo$(INTDIR)/ /Fd$(OUTDIR)/"emwinnt.pdb" /c /Tp\
  $(SOURCE) 

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\UNDO.C
DEP_UNDO_=\
	.\EMACS.H\
	.\OS.H\
	.\EM_GEN.H\
	.\EM_MAC.H\
	.\QUEUE.H\
	.\EM_TIME.H\
	.\SYNTAX.H\
	.\DISPLAY.H\
	.\MLISP.H\
	.\FILEIO.H\
	.\KEYBOARD.H\
	.\BUFFER.H\
	.\ABBREV.H\
	.\NDBM.H\
	.\TT.H\
	.\WINDOW.H\
	.\UNDO.H\
	.\JOURNAL.H\
	.\FILESERV.H\
	.\EM_RTN.H\
	.\EM_VAR.H\
	.\OS_WIN.H\
	.\OS_MAC.H

!IF  "$(CFG)" == "Win32 Release"

# ADD BASE CPP /Yu"EMACS.H"
# SUBTRACT CPP /Yc /Yu

$(INTDIR)/UNDO.OBJ :  $(SOURCE)  $(DEP_UNDO_) $(INTDIR)
   $(CPP) /nologo /G3 /Zp4 /MT /W4 /GX /Zi /O2 /I "u:\work\emacs\src\edit" /D\
 "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_X86_" /D "_NT" /D "_MBCS" /FR$(INTDIR)/\
 /Fo$(INTDIR)/ /Fd$(OUTDIR)/"emwinnt.pdb" /c /Tp  $(SOURCE) 

!ELSEIF  "$(CFG)" == "Win32 Debug"

# ADD BASE CPP /Yu"EMACS.H"
# SUBTRACT CPP /Yc /Yu

$(INTDIR)/UNDO.OBJ :  $(SOURCE)  $(DEP_UNDO_) $(INTDIR)
   $(CPP) /nologo /G3 /Zp4 /MT /W4 /GX /Zi /O2 /I "u:\work\emacs\src\edit" /D\
 "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_X86_" /D "_NT" /D "_MBCS"\
 /FR"u:\work\emacs\winnt.dbg/" /Fo$(INTDIR)/ /Fd$(OUTDIR)/"emwinnt.pdb" /c /Tp\
  $(SOURCE) 

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\VARIABLE.C
DEP_VARIA=\
	.\EMACS.H\
	.\VARLIST.H\
	.\OS.H\
	.\EM_GEN.H\
	.\EM_MAC.H\
	.\QUEUE.H\
	.\EM_TIME.H\
	.\SYNTAX.H\
	.\DISPLAY.H\
	.\MLISP.H\
	.\FILEIO.H\
	.\KEYBOARD.H\
	.\BUFFER.H\
	.\ABBREV.H\
	.\NDBM.H\
	.\TT.H\
	.\WINDOW.H\
	.\UNDO.H\
	.\JOURNAL.H\
	.\FILESERV.H\
	.\EM_RTN.H\
	.\EM_VAR.H\
	.\OS_WIN.H\
	.\OS_MAC.H

!IF  "$(CFG)" == "Win32 Release"

# ADD BASE CPP /Yu"EMACS.H"
# SUBTRACT CPP /Yc /Yu

$(INTDIR)/VARIABLE.OBJ :  $(SOURCE)  $(DEP_VARIA) $(INTDIR)
   $(CPP) /nologo /G3 /Zp4 /MT /W4 /GX /Zi /O2 /I "u:\work\emacs\src\edit" /D\
 "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_X86_" /D "_NT" /D "_MBCS" /FR$(INTDIR)/\
 /Fo$(INTDIR)/ /Fd$(OUTDIR)/"emwinnt.pdb" /c /Tp  $(SOURCE) 

!ELSEIF  "$(CFG)" == "Win32 Debug"

# ADD BASE CPP /Yu"EMACS.H"
# SUBTRACT CPP /Yc /Yu

$(INTDIR)/VARIABLE.OBJ :  $(SOURCE)  $(DEP_VARIA) $(INTDIR)
   $(CPP) /nologo /G3 /Zp4 /MT /W4 /GX /Zi /O2 /I "u:\work\emacs\src\edit" /D\
 "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_X86_" /D "_NT" /D "_MBCS"\
 /FR"u:\work\emacs\winnt.dbg/" /Fo$(INTDIR)/ /Fd$(OUTDIR)/"emwinnt.pdb" /c /Tp\
  $(SOURCE) 

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\VARTHUNK.C
DEP_VARTH=\
	.\EMACS.H\
	.\OS.H\
	.\EM_GEN.H\
	.\EM_MAC.H\
	.\QUEUE.H\
	.\EM_TIME.H\
	.\SYNTAX.H\
	.\DISPLAY.H\
	.\MLISP.H\
	.\FILEIO.H\
	.\KEYBOARD.H\
	.\BUFFER.H\
	.\ABBREV.H\
	.\NDBM.H\
	.\TT.H\
	.\WINDOW.H\
	.\UNDO.H\
	.\JOURNAL.H\
	.\FILESERV.H\
	.\EM_RTN.H\
	.\EM_VAR.H\
	.\OS_WIN.H\
	.\OS_MAC.H

!IF  "$(CFG)" == "Win32 Release"

# ADD BASE CPP /Yu"EMACS.H"
# SUBTRACT CPP /Yc /Yu

$(INTDIR)/VARTHUNK.OBJ :  $(SOURCE)  $(DEP_VARTH) $(INTDIR)
   $(CPP) /nologo /G3 /Zp4 /MT /W4 /GX /Zi /O2 /I "u:\work\emacs\src\edit" /D\
 "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_X86_" /D "_NT" /D "_MBCS" /FR$(INTDIR)/\
 /Fo$(INTDIR)/ /Fd$(OUTDIR)/"emwinnt.pdb" /c /Tp  $(SOURCE) 

!ELSEIF  "$(CFG)" == "Win32 Debug"

# ADD BASE CPP /Yu"EMACS.H"
# SUBTRACT CPP /Yc /Yu

$(INTDIR)/VARTHUNK.OBJ :  $(SOURCE)  $(DEP_VARTH) $(INTDIR)
   $(CPP) /nologo /G3 /Zp4 /MT /W4 /GX /Zi /O2 /I "u:\work\emacs\src\edit" /D\
 "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_X86_" /D "_NT" /D "_MBCS"\
 /FR"u:\work\emacs\winnt.dbg/" /Fo$(INTDIR)/ /Fd$(OUTDIR)/"emwinnt.pdb" /c /Tp\
  $(SOURCE) 

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\WINDMAN.C
DEP_WINDM=\
	.\EMACS.H\
	.\OS.H\
	.\EM_GEN.H\
	.\EM_MAC.H\
	.\QUEUE.H\
	.\EM_TIME.H\
	.\SYNTAX.H\
	.\DISPLAY.H\
	.\MLISP.H\
	.\FILEIO.H\
	.\KEYBOARD.H\
	.\BUFFER.H\
	.\ABBREV.H\
	.\NDBM.H\
	.\TT.H\
	.\WINDOW.H\
	.\UNDO.H\
	.\JOURNAL.H\
	.\FILESERV.H\
	.\EM_RTN.H\
	.\EM_VAR.H\
	.\OS_WIN.H\
	.\OS_MAC.H

!IF  "$(CFG)" == "Win32 Release"

# ADD BASE CPP /Yu"EMACS.H"
# SUBTRACT CPP /Yc /Yu

$(INTDIR)/WINDMAN.OBJ :  $(SOURCE)  $(DEP_WINDM) $(INTDIR)
   $(CPP) /nologo /G3 /Zp4 /MT /W4 /GX /Zi /O2 /I "u:\work\emacs\src\edit" /D\
 "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_X86_" /D "_NT" /D "_MBCS" /FR$(INTDIR)/\
 /Fo$(INTDIR)/ /Fd$(OUTDIR)/"emwinnt.pdb" /c /Tp  $(SOURCE) 

!ELSEIF  "$(CFG)" == "Win32 Debug"

# ADD BASE CPP /Yu"EMACS.H"
# SUBTRACT CPP /Yc /Yu

$(INTDIR)/WINDMAN.OBJ :  $(SOURCE)  $(DEP_WINDM) $(INTDIR)
   $(CPP) /nologo /G3 /Zp4 /MT /W4 /GX /Zi /O2 /I "u:\work\emacs\src\edit" /D\
 "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_X86_" /D "_NT" /D "_MBCS"\
 /FR"u:\work\emacs\winnt.dbg/" /Fo$(INTDIR)/ /Fd$(OUTDIR)/"emwinnt.pdb" /c /Tp\
  $(SOURCE) 

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\WINDOW.C
DEP_WINDO=\
	.\EMACS.H\
	.\OS.H\
	.\EM_GEN.H\
	.\EM_MAC.H\
	.\QUEUE.H\
	.\EM_TIME.H\
	.\SYNTAX.H\
	.\DISPLAY.H\
	.\MLISP.H\
	.\FILEIO.H\
	.\KEYBOARD.H\
	.\BUFFER.H\
	.\ABBREV.H\
	.\NDBM.H\
	.\TT.H\
	.\WINDOW.H\
	.\UNDO.H\
	.\JOURNAL.H\
	.\FILESERV.H\
	.\EM_RTN.H\
	.\EM_VAR.H\
	.\OS_WIN.H\
	.\OS_MAC.H

!IF  "$(CFG)" == "Win32 Release"

# ADD BASE CPP /Yu"EMACS.H"
# SUBTRACT CPP /Yc /Yu

$(INTDIR)/WINDOW.OBJ :  $(SOURCE)  $(DEP_WINDO) $(INTDIR)
   $(CPP) /nologo /G3 /Zp4 /MT /W4 /GX /Zi /O2 /I "u:\work\emacs\src\edit" /D\
 "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_X86_" /D "_NT" /D "_MBCS" /FR$(INTDIR)/\
 /Fo$(INTDIR)/ /Fd$(OUTDIR)/"emwinnt.pdb" /c /Tp  $(SOURCE) 

!ELSEIF  "$(CFG)" == "Win32 Debug"

# ADD BASE CPP /Yu"EMACS.H"
# SUBTRACT CPP /Yc /Yu

$(INTDIR)/WINDOW.OBJ :  $(SOURCE)  $(DEP_WINDO) $(INTDIR)
   $(CPP) /nologo /G3 /Zp4 /MT /W4 /GX /Zi /O2 /I "u:\work\emacs\src\edit" /D\
 "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_X86_" /D "_NT" /D "_MBCS"\
 /FR"u:\work\emacs\winnt.dbg/" /Fo$(INTDIR)/ /Fd$(OUTDIR)/"emwinnt.pdb" /c /Tp\
  $(SOURCE) 

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\NT_PROC.C
DEP_NT_PR=\
	.\EMACS.H\
	.\NT_COMM.H\
	.\OS.H\
	.\EM_GEN.H\
	.\EM_MAC.H\
	.\QUEUE.H\
	.\EM_TIME.H\
	.\SYNTAX.H\
	.\DISPLAY.H\
	.\MLISP.H\
	.\FILEIO.H\
	.\KEYBOARD.H\
	.\BUFFER.H\
	.\ABBREV.H\
	.\NDBM.H\
	.\TT.H\
	.\WINDOW.H\
	.\UNDO.H\
	.\JOURNAL.H\
	.\FILESERV.H\
	.\EM_RTN.H\
	.\EM_VAR.H\
	.\NT_ASYNC.H\
	.\NT_PIPE.H\
	.\NT_PROC.H\
	.\OS_WIN.H\
	.\OS_MAC.H

!IF  "$(CFG)" == "Win32 Release"

# SUBTRACT CPP /Yc /Yu

$(INTDIR)/NT_PROC.OBJ :  $(SOURCE)  $(DEP_NT_PR) $(INTDIR)
   $(CPP) /nologo /G3 /Zp4 /MT /W4 /GX /Zi /O2 /I "u:\work\emacs\src\edit" /D\
 "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_X86_" /D "_NT" /D "_MBCS" /FR$(INTDIR)/\
 /Fo$(INTDIR)/ /Fd$(OUTDIR)/"emwinnt.pdb" /c /Tp  $(SOURCE) 

!ELSEIF  "$(CFG)" == "Win32 Debug"

# SUBTRACT CPP /Yc /Yu

$(INTDIR)/NT_PROC.OBJ :  $(SOURCE)  $(DEP_NT_PR) $(INTDIR)
   $(CPP) /nologo /G3 /Zp4 /MT /W4 /GX /Zi /O2 /I "u:\work\emacs\src\edit" /D\
 "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_X86_" /D "_NT" /D "_MBCS"\
 /FR"u:\work\emacs\winnt.dbg/" /Fo$(INTDIR)/ /Fd$(OUTDIR)/"emwinnt.pdb" /c /Tp\
  $(SOURCE) 

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\NT_COMM.C
DEP_NT_CO=\
	.\EMACS.H\
	.\NT_COMM.H\
	.\OS.H\
	.\EM_GEN.H\
	.\EM_MAC.H\
	.\QUEUE.H\
	.\EM_TIME.H\
	.\SYNTAX.H\
	.\DISPLAY.H\
	.\MLISP.H\
	.\FILEIO.H\
	.\KEYBOARD.H\
	.\BUFFER.H\
	.\ABBREV.H\
	.\NDBM.H\
	.\TT.H\
	.\WINDOW.H\
	.\UNDO.H\
	.\JOURNAL.H\
	.\FILESERV.H\
	.\EM_RTN.H\
	.\EM_VAR.H\
	.\NT_ASYNC.H\
	.\NT_PIPE.H\
	.\NT_PROC.H\
	.\OS_WIN.H\
	.\OS_MAC.H

!IF  "$(CFG)" == "Win32 Release"

# SUBTRACT CPP /Yc /Yu

$(INTDIR)/NT_COMM.OBJ :  $(SOURCE)  $(DEP_NT_CO) $(INTDIR)
   $(CPP) /nologo /G3 /Zp4 /MT /W4 /GX /Zi /O2 /I "u:\work\emacs\src\edit" /D\
 "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_X86_" /D "_NT" /D "_MBCS" /FR$(INTDIR)/\
 /Fo$(INTDIR)/ /Fd$(OUTDIR)/"emwinnt.pdb" /c /Tp  $(SOURCE) 

!ELSEIF  "$(CFG)" == "Win32 Debug"

# SUBTRACT CPP /Yc /Yu

$(INTDIR)/NT_COMM.OBJ :  $(SOURCE)  $(DEP_NT_CO) $(INTDIR)
   $(CPP) /nologo /G3 /Zp4 /MT /W4 /GX /Zi /O2 /I "u:\work\emacs\src\edit" /D\
 "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_X86_" /D "_NT" /D "_MBCS"\
 /FR"u:\work\emacs\winnt.dbg/" /Fo$(INTDIR)/ /Fd$(OUTDIR)/"emwinnt.pdb" /c /Tp\
  $(SOURCE) 

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\NT_ASYNC.C
DEP_NT_AS=\
	.\EMACS.H\
	.\NT_COMM.H\
	.\OS.H\
	.\EM_GEN.H\
	.\EM_MAC.H\
	.\QUEUE.H\
	.\EM_TIME.H\
	.\SYNTAX.H\
	.\DISPLAY.H\
	.\MLISP.H\
	.\FILEIO.H\
	.\KEYBOARD.H\
	.\BUFFER.H\
	.\ABBREV.H\
	.\NDBM.H\
	.\TT.H\
	.\WINDOW.H\
	.\UNDO.H\
	.\JOURNAL.H\
	.\FILESERV.H\
	.\EM_RTN.H\
	.\EM_VAR.H\
	.\NT_ASYNC.H\
	.\NT_PIPE.H\
	.\NT_PROC.H\
	.\OS_WIN.H\
	.\OS_MAC.H

!IF  "$(CFG)" == "Win32 Release"

# SUBTRACT CPP /Yc /Yu

$(INTDIR)/NT_ASYNC.OBJ :  $(SOURCE)  $(DEP_NT_AS) $(INTDIR)
   $(CPP) /nologo /G3 /Zp4 /MT /W4 /GX /Zi /O2 /I "u:\work\emacs\src\edit" /D\
 "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_X86_" /D "_NT" /D "_MBCS" /FR$(INTDIR)/\
 /Fo$(INTDIR)/ /Fd$(OUTDIR)/"emwinnt.pdb" /c /Tp  $(SOURCE) 

!ELSEIF  "$(CFG)" == "Win32 Debug"

# SUBTRACT CPP /Yc /Yu

$(INTDIR)/NT_ASYNC.OBJ :  $(SOURCE)  $(DEP_NT_AS) $(INTDIR)
   $(CPP) /nologo /G3 /Zp4 /MT /W4 /GX /Zi /O2 /I "u:\work\emacs\src\edit" /D\
 "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_X86_" /D "_NT" /D "_MBCS"\
 /FR"u:\work\emacs\winnt.dbg/" /Fo$(INTDIR)/ /Fd$(OUTDIR)/"emwinnt.pdb" /c /Tp\
  $(SOURCE) 

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\NT_PIPE.C
DEP_NT_PI=\
	.\EMACS.H\
	.\NT_COMM.H\
	.\OS.H\
	.\EM_GEN.H\
	.\EM_MAC.H\
	.\QUEUE.H\
	.\EM_TIME.H\
	.\SYNTAX.H\
	.\DISPLAY.H\
	.\MLISP.H\
	.\FILEIO.H\
	.\KEYBOARD.H\
	.\BUFFER.H\
	.\ABBREV.H\
	.\NDBM.H\
	.\TT.H\
	.\WINDOW.H\
	.\UNDO.H\
	.\JOURNAL.H\
	.\FILESERV.H\
	.\EM_RTN.H\
	.\EM_VAR.H\
	.\NT_ASYNC.H\
	.\NT_PIPE.H\
	.\NT_PROC.H\
	.\OS_WIN.H\
	.\OS_MAC.H

!IF  "$(CFG)" == "Win32 Release"

# SUBTRACT CPP /Yc /Yu

$(INTDIR)/NT_PIPE.OBJ :  $(SOURCE)  $(DEP_NT_PI) $(INTDIR)
   $(CPP) /nologo /G3 /Zp4 /MT /W4 /GX /Zi /O2 /I "u:\work\emacs\src\edit" /D\
 "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_X86_" /D "_NT" /D "_MBCS" /FR$(INTDIR)/\
 /Fo$(INTDIR)/ /Fd$(OUTDIR)/"emwinnt.pdb" /c /Tp  $(SOURCE) 

!ELSEIF  "$(CFG)" == "Win32 Debug"

# SUBTRACT CPP /Yc /Yu

$(INTDIR)/NT_PIPE.OBJ :  $(SOURCE)  $(DEP_NT_PI) $(INTDIR)
   $(CPP) /nologo /G3 /Zp4 /MT /W4 /GX /Zi /O2 /I "u:\work\emacs\src\edit" /D\
 "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_X86_" /D "_NT" /D "_MBCS"\
 /FR"u:\work\emacs\winnt.dbg/" /Fo$(INTDIR)/ /Fd$(OUTDIR)/"emwinnt.pdb" /c /Tp\
  $(SOURCE) 

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\AAAA_MEM.C

!IF  "$(CFG)" == "Win32 Release"

# SUBTRACT CPP /Yc /Yu

$(INTDIR)/AAAA_MEM.OBJ :  $(SOURCE)  $(INTDIR)
   $(CPP) /nologo /G3 /Zp4 /MT /W4 /GX /Zi /O2 /I "u:\work\emacs\src\edit" /D\
 "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_X86_" /D "_NT" /D "_MBCS" /FR$(INTDIR)/\
 /Fo$(INTDIR)/ /Fd$(OUTDIR)/"emwinnt.pdb" /c /Tp  $(SOURCE) 

!ELSEIF  "$(CFG)" == "Win32 Debug"

# SUBTRACT CPP /Yc /Yu

$(INTDIR)/AAAA_MEM.OBJ :  $(SOURCE)  $(INTDIR)
   $(CPP) /nologo /G3 /Zp4 /MT /W4 /GX /Zi /O2 /I "u:\work\emacs\src\edit" /D\
 "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_X86_" /D "_NT" /D "_MBCS"\
 /FR"u:\work\emacs\winnt.dbg/" /Fo$(INTDIR)/ /Fd$(OUTDIR)/"emwinnt.pdb" /c /Tp\
  $(SOURCE) 

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\ZZZZ_MEM.C

!IF  "$(CFG)" == "Win32 Release"

# SUBTRACT CPP /Yc /Yu

$(INTDIR)/ZZZZ_MEM.OBJ :  $(SOURCE)  $(INTDIR)
   $(CPP) /nologo /G3 /Zp4 /MT /W4 /GX /Zi /O2 /I "u:\work\emacs\src\edit" /D\
 "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_X86_" /D "_NT" /D "_MBCS" /FR$(INTDIR)/\
 /Fo$(INTDIR)/ /Fd$(OUTDIR)/"emwinnt.pdb" /c /Tp  $(SOURCE) 

!ELSEIF  "$(CFG)" == "Win32 Debug"

# SUBTRACT CPP /Yc /Yu

$(INTDIR)/ZZZZ_MEM.OBJ :  $(SOURCE)  $(INTDIR)
   $(CPP) /nologo /G3 /Zp4 /MT /W4 /GX /Zi /O2 /I "u:\work\emacs\src\edit" /D\
 "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_X86_" /D "_NT" /D "_MBCS"\
 /FR"u:\work\emacs\winnt.dbg/" /Fo$(INTDIR)/ /Fd$(OUTDIR)/"emwinnt.pdb" /c /Tp\
  $(SOURCE) 

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\MLPRINTF.C

$(INTDIR)/MLPRINTF.OBJ :  $(SOURCE)  $(INTDIR)

# End Source File
# End Group
################################################################################
# Begin Group "Created Sources"

################################################################################
# Begin Source File

SOURCE=\WORK\EMACS\WINNT.DBG\version.c
DEP_VERSI=\
	.\EMACS.H\
	.\OS.H\
	.\EM_GEN.H\
	.\EM_MAC.H\
	.\QUEUE.H\
	.\EM_TIME.H\
	.\SYNTAX.H\
	.\DISPLAY.H\
	.\MLISP.H\
	.\FILEIO.H\
	.\KEYBOARD.H\
	.\BUFFER.H\
	.\ABBREV.H\
	.\NDBM.H\
	.\TT.H\
	.\WINDOW.H\
	.\UNDO.H\
	.\JOURNAL.H\
	.\FILESERV.H\
	.\EM_RTN.H\
	.\EM_VAR.H\
	.\OS_WIN.H\
	.\OS_MAC.H

$(INTDIR)/version.obj :  $(SOURCE)  $(DEP_VERSI) $(INTDIR)
   $(CPP) $(CPP_PROJ)  $(SOURCE) 

# End Source File
# End Group
# End Project
################################################################################
