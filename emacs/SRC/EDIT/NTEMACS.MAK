#
#	Emacs make files for X86 and Alpha NT systems
#
#
#	MFCDEBUG - set to 1 to compile in MFC debug code
#

!include <ntwin32.mak>

PROJ = EMWINNT
CFLAGS_D_WEXE32 = /G3 /Zp4 /W4 /Zi /D "_DEBUG" /D "_WINDOWS" /D "_NT" /I "$(SRC_ROOT)" /nologo /FR /Tp
CFLAGS_R_WEXE32 = /G3 /Zp4 /Gy /W4 /Zi /O2 /D "NDEBUG" /D "_WINDOWS" /D "_NT" /I "$(SRC_ROOT)" /nologo /FR /Tp
LFLAGS_D_WEXE32 = $(ldebug) /version:6.0 /map
LFLAGS_R_WEXE32 = /version:6.0 /map
D_RCDEFINES32 = /d_DEBUG /d_NT
R_RCDEFINES32 = /dNDEBUG /d_NT
CCREATEPCHFLAG = /YcEMACS.H
CPPCREATEPCHFLAG = /YcSTDAFX.H
CUSEPCHFLAG = /YuEMACS.H
CPPUSEPCHFLAG = /YuSTDAFX.H

!ifdef MFCNODEBUG
user_cflags = $(CFLAGS_R_WEXE32)
user_lflags = $(LFLAGS_R_WEXE32)
user_rcflags = $(R_RCDEFINES32)
mfclib = nafxcw.lib shell32.lib advapi32.lib
!else
user_cflags = $(CFLAGS_D_WEXE32)
user_lflags = $(LFLAGS_D_WEXE32)
user_rcflags = $(D_RCDEFINES32)
mfclib = nafxcwd.lib shell32.lib advapi32.lib
!endif

cpp = $(cc)

all:	$(PROJ).EXE DBADD.EXE DBLIST.EXE DBPRINT.EXE DBDEL.EXE DBCREATE.EXE mll-2-db.EXE $(PROJ).bsc

WINEMACS.RES:	$(SRC_ROOT)\WINEMACS.RC $(WINEMACS_RCDEP)
!ifdef rescvt
	$(rc) $(rcvars) $(user_rcflags) -r -FoWINEMACS.TMP $(SRC_ROOT)\WINEMACS.RC
	$(rescvt) -o WINEMACS.res WINEMACS.TMP
!else
	$(rc) $(rcvars) $(user_rcflags) -r -FoWINEMACS.res $(SRC_ROOT)\WINEMACS.RC
!endif

membegin.OBJ:	$(SRC_ROOT)\membegin.C $(ARITH_DEP)
	$(cc) $(cflags) $(user_cflags) $(SRC_ROOT)\membegin.C

memend.OBJ:	$(SRC_ROOT)\memend.C $(ARITH_DEP)
	$(cc) $(cflags) $(user_cflags) $(SRC_ROOT)\memend.C

ABBREV.OBJ:	$(SRC_ROOT)\ABBREV.C $(ABBREV_DEP) $(SRC_ROOT)\os.h
	$(cc) $(cflags) $(CCREATEPCHFLAG) $(user_cflags) $(SRC_ROOT)\ABBREV.C

ABSPATH.OBJ:	$(SRC_ROOT)\ABSPATH.C $(ABSPATH_DEP)
	$(cc) $(cflags) $(CUSEPCHFLAG) $(user_cflags) $(SRC_ROOT)\ABSPATH.C

ARITH.OBJ:	$(SRC_ROOT)\ARITH.C $(ARITH_DEP)
	$(cc) $(cflags) $(CUSEPCHFLAG) $(user_cflags) $(SRC_ROOT)\ARITH.C

BUFFER.OBJ:	$(SRC_ROOT)\BUFFER.C $(BUFFER_DEP)
	$(cc) $(cflags) $(CUSEPCHFLAG) $(user_cflags) $(SRC_ROOT)\BUFFER.C

CASECONV.OBJ:	$(SRC_ROOT)\CASECONV.C $(CASECONV_DEP)
	$(cc) $(cflags) $(CUSEPCHFLAG) $(user_cflags) $(SRC_ROOT)\CASECONV.C

COLUMNS.OBJ:	$(SRC_ROOT)\COLUMNS.C $(COLUMNS_DEP)
	$(cc) $(cflags) $(CUSEPCHFLAG) $(user_cflags) $(SRC_ROOT)\COLUMNS.C

DISPLAY.OBJ:	$(SRC_ROOT)\DISPLAY.C $(DISPLAY_DEP)
	$(cc) $(cflags) $(CUSEPCHFLAG) $(user_cflags) $(SRC_ROOT)\DISPLAY.C

EMACS.OBJ:	$(SRC_ROOT)\EMACS.C $(EMACS_DEP)
	$(cc) $(cflags) $(CUSEPCHFLAG) $(user_cflags) $(SRC_ROOT)\EMACS.C

EMACSRTL.OBJ:	$(SRC_ROOT)\EMACSRTL.C $(EMACSRTL_DEP)
	$(cc) $(cflags) $(CUSEPCHFLAG) $(user_cflags) $(SRC_ROOT)\EMACSRTL.C

ERRLOG.OBJ:	$(SRC_ROOT)\ERRLOG.C $(ERRLOG_DEP)
	$(cc) $(cflags) $(CUSEPCHFLAG) $(user_cflags) $(SRC_ROOT)\ERRLOG.C

FILEIO.OBJ:	$(SRC_ROOT)\FILEIO.C $(FILEIO_DEP)
	$(cc) $(cflags) $(CUSEPCHFLAG) $(user_cflags) $(SRC_ROOT)\FILEIO.C

FILESERV.OBJ:	$(SRC_ROOT)\FILESERV.C $(FILESERV_DEP)
	$(cc) $(cflags) $(CUSEPCHFLAG) $(user_cflags) $(SRC_ROOT)\FILESERV.C

FIO.OBJ:	$(SRC_ROOT)\FIO.C $(FIO_DEP)
	$(cc) $(cflags) $(CUSEPCHFLAG) $(user_cflags) $(SRC_ROOT)\FIO.C

FUNCTION.OBJ:	$(SRC_ROOT)\FUNCTION.C $(FUNCTION_DEP) $(SRC_ROOT)\funclist.h
	$(cc) $(cflags) $(CUSEPCHFLAG) $(user_cflags) $(SRC_ROOT)\FUNCTION.C

GLOB_VAR.OBJ:	$(SRC_ROOT)\GLOB_VAR.C $(GLOB_VAR_DEP)
	$(cc) $(cflags) $(CUSEPCHFLAG) $(user_cflags) $(SRC_ROOT)\GLOB_VAR.C

JOURNAL.OBJ:	$(SRC_ROOT)\JOURNAL.C $(JOURNAL_DEP)
	$(cc) $(cflags) $(CUSEPCHFLAG) $(user_cflags) $(SRC_ROOT)\JOURNAL.C

KEYBOARD.OBJ:	$(SRC_ROOT)\KEYBOARD.C $(KEYBOARD_DEP)
	$(cc) $(cflags) $(CUSEPCHFLAG) $(user_cflags) $(SRC_ROOT)\KEYBOARD.C

LISPFUNC.OBJ:	$(SRC_ROOT)\LISPFUNC.C $(LISPFUNC_DEP)
	$(cc) $(cflags) $(CUSEPCHFLAG) $(user_cflags) $(SRC_ROOT)\LISPFUNC.C

MACROS.OBJ:	$(SRC_ROOT)\MACROS.C $(MACROS_DEP)
	$(cc) $(cflags) $(CUSEPCHFLAG) $(user_cflags) $(SRC_ROOT)\MACROS.C

MEM_MAN.OBJ:	$(SRC_ROOT)\MEM_MAN.C $(MEM_MAN_DEP)
	$(cc) $(cflags) $(CUSEPCHFLAG) $(user_cflags) $(SRC_ROOT)\MEM_MAN.C

METACOMM.OBJ:	$(SRC_ROOT)\METACOMM.C $(METACOMM_DEP)
	$(cc) $(cflags) $(CUSEPCHFLAG) $(user_cflags) $(SRC_ROOT)\METACOMM.C

MINIBUF.OBJ:	$(SRC_ROOT)\MINIBUF.C $(MINIBUF_DEP)
	$(cc) $(cflags) $(CUSEPCHFLAG) $(user_cflags) $(SRC_ROOT)\MINIBUF.C

MLISPARS.OBJ:	$(SRC_ROOT)\MLISPARS.C $(MLISPARS_DEP)
	$(cc) $(cflags) $(CUSEPCHFLAG) $(user_cflags) $(SRC_ROOT)\MLISPARS.C

MLISPROC.OBJ:	$(SRC_ROOT)\MLISPROC.C $(MLISPROC_DEP)
	$(cc) $(cflags) $(CUSEPCHFLAG) $(user_cflags) $(SRC_ROOT)\MLISPROC.C

OPTIONS.OBJ:	$(SRC_ROOT)\OPTIONS.C $(OPTIONS_DEP)
	$(cc) $(cflags) $(CUSEPCHFLAG) $(user_cflags) $(SRC_ROOT)\OPTIONS.C

PROMPT.OBJ:	$(SRC_ROOT)\PROMPT.C $(PROMPT_DEP)
	$(cc) $(cflags) $(CUSEPCHFLAG) $(user_cflags) $(SRC_ROOT)\PROMPT.C

QUEUE.OBJ:	$(SRC_ROOT)\QUEUE.C $(QUEUE_DEP)
	$(cc) $(cflags) $(CUSEPCHFLAG) $(user_cflags) $(SRC_ROOT)\QUEUE.C

SEARCH.OBJ:	$(SRC_ROOT)\SEARCH.C $(SEARCH_DEP)
	$(cc) $(cflags) $(CUSEPCHFLAG) $(user_cflags) $(SRC_ROOT)\SEARCH.C

SEARCHIN.OBJ:	$(SRC_ROOT)\SEARCHIN.C $(SEARCHIN_DEP)
	$(cc) $(cflags) $(CUSEPCHFLAG) $(user_cflags) $(SRC_ROOT)\SEARCHIN.C

SIMPCOMM.OBJ:	$(SRC_ROOT)\SIMPCOMM.C $(SIMPCOMM_DEP)
	$(cc) $(cflags) $(CUSEPCHFLAG) $(user_cflags) $(SRC_ROOT)\SIMPCOMM.C

SUBPROC.OBJ:	$(SRC_ROOT)\SUBPROC.C $(SUBPROC_DEP)
	$(cc) $(cflags) $(CUSEPCHFLAG) $(user_cflags) $(SRC_ROOT)\SUBPROC.C

SYNTAX.OBJ:	$(SRC_ROOT)\SYNTAX.C $(SYNTAX_DEP)
	$(cc) $(cflags) $(CUSEPCHFLAG) $(user_cflags) $(SRC_ROOT)\SYNTAX.C

TERM.OBJ:	$(SRC_ROOT)\TERM.C $(TERM_DEP)
	$(cc) $(cflags) $(CUSEPCHFLAG) $(user_cflags) $(SRC_ROOT)\TERM.C

UNDO.OBJ:	$(SRC_ROOT)\UNDO.C $(UNDO_DEP)
	$(cc) $(cflags) $(CUSEPCHFLAG) $(user_cflags) $(SRC_ROOT)\UNDO.C

VARIABLE.OBJ:	$(SRC_ROOT)\VARIABLE.C $(VARIABLE_DEP) $(SRC_ROOT)\varlist.h
	$(cc) $(cflags) $(CUSEPCHFLAG) $(user_cflags) $(SRC_ROOT)\VARIABLE.C

VARTHUNK.OBJ:	$(SRC_ROOT)\VARTHUNK.C $(VARTHUNK_DEP)
	$(cc) $(cflags) $(CUSEPCHFLAG) $(user_cflags) $(SRC_ROOT)\VARTHUNK.C

NT_COMM.OBJ:	$(SRC_ROOT)\NT_COMM.C $(NT_COMM_DEP)
	$(cc) $(cflags) $(CUSEPCHFLAG) $(user_cflags) $(SRC_ROOT)\NT_COMM.C

NT_ASYNC.OBJ:	$(SRC_ROOT)\NT_ASYNC.C $(NT_ASYNC_DEP)
	$(cc) $(cflags) $(CUSEPCHFLAG) $(user_cflags) $(SRC_ROOT)\NT_ASYNC.C

NT_PIPE.OBJ:	$(SRC_ROOT)\NT_PIPE.C $(NT_PIPE_DEP)
	$(cc) $(cflags) $(CUSEPCHFLAG) $(user_cflags) $(SRC_ROOT)\NT_PIPE.C

NT_PROC.OBJ:	$(SRC_ROOT)\NT_PROC.C $(NT_PROC_DEP)
	$(cc) $(cflags) $(CUSEPCHFLAG) $(user_cflags) $(SRC_ROOT)\NT_PROC.C

WINDMAN.OBJ:	$(SRC_ROOT)\WINDMAN.C $(WINDMAN_DEP)
	$(cc) $(cflags) $(CUSEPCHFLAG) $(user_cflags) $(SRC_ROOT)\WINDMAN.C

WINDOW.OBJ:	$(SRC_ROOT)\WINDOW.C $(WINDOW_DEP)
	$(cc) $(cflags) $(CUSEPCHFLAG) $(user_cflags) $(SRC_ROOT)\WINDOW.C

NDBM.OBJ:	$(SRC_ROOT)\NDBM.C $(NDBM_DEP)
	$(cc) $(cflags) $(CUSEPCHFLAG) $(user_cflags) $(SRC_ROOT)\NDBM.C

DBMAN.OBJ:	$(SRC_ROOT)\DBMAN.C $(DBMAN_DEP)
	$(cc) $(cflags) $(CUSEPCHFLAG) $(user_cflags) $(SRC_ROOT)\DBMAN.C

STDAFX.OBJ:	$(SRC_ROOT)\STDAFX.CPP $(STDAFX_DEP) $(SRC_ROOT)\os.h
	$(cpp) $(cflags) $(CPPCREATEPCHFLAG) $(user_cflags) $(SRC_ROOT)\STDAFX.CPP

MAINFRM.OBJ:	$(SRC_ROOT)\MAINFRM.CPP $(MAINFRM_DEP)
	$(cpp) $(cflags) $(CPPUSEPCHFLAG) $(user_cflags) $(SRC_ROOT)\MAINFRM.CPP

FINDDLG.OBJ:	$(SRC_ROOT)\FINDDLG.CPP $(FINDDLG_DEP)
	$(cpp) $(cflags) $(CPPUSEPCHFLAG) $(user_cflags) $(SRC_ROOT)\FINDDLG.CPP

SWITCHDL.OBJ:	$(SRC_ROOT)\SWITCHDL.CPP $(SWITCHDL_DEP)
	$(cpp) $(cflags) $(CPPUSEPCHFLAG) $(user_cflags) $(SRC_ROOT)\SWITCHDL.CPP

WINEMACS.OBJ:	$(SRC_ROOT)\WINEMACS.CPP $(WINEMACS_DEP) $(SRC_ROOT)\os.h
	$(cpp) $(cflags) $(CPPUSEPCHFLAG) $(user_cflags) $(SRC_ROOT)\WINEMACS.CPP

WINEMDOC.OBJ:	$(SRC_ROOT)\WINEMDOC.CPP $(WINEMDOC_DEP)
	$(cpp) $(cflags) $(CPPUSEPCHFLAG) $(user_cflags) $(SRC_ROOT)\WINEMDOC.CPP

WINEMVW.OBJ:	$(SRC_ROOT)\WINEMVW.CPP $(WINEMVW_DEP)
	$(cpp) $(cflags) $(CPPUSEPCHFLAG) $(user_cflags) $(SRC_ROOT)\WINEMVW.CPP

EDOPRINT.OBJ:	$(SRC_ROOT)\EDOPRINT.C $(EDOPRINT_DEP)
	$(cc) $(cflags) $(CUSEPCHFLAG) $(user_cflags) $(SRC_ROOT)\EDOPRINT.C

WIN_OPT.OBJ:	$(SRC_ROOT)\WIN_OPT.CPP $(WIN_OPT_DEP)
	$(cpp) $(cflags) $(CPPUSEPCHFLAG) $(user_cflags) $(SRC_ROOT)\WIN_OPT.CPP

WIN_MENU.OBJ:	$(SRC_ROOT)\WIN_MENU.CPP $(WIN_MENU_DEP)
	$(cpp) $(cflags) $(CPPUSEPCHFLAG) $(user_cflags) $(SRC_ROOT)\WIN_MENU.CPP

WIN_UI.OBJ:	$(SRC_ROOT)\WIN_UI.CPP $(WIN_UI_DEP)
	$(cpp) $(cflags) $(CPPUSEPCHFLAG) $(user_cflags) $(SRC_ROOT)\WIN_UI.CPP

WIN_RTL.OBJ:	$(SRC_ROOT)\WIN_RTL.CPP $(WIN_RTL_DEP)
	$(cpp) $(cflags) $(CPPUSEPCHFLAG) $(user_cflags) $(SRC_ROOT)\WIN_RTL.CPP

TIMER.OBJ:	$(SRC_ROOT)\TIMER.C $(TIMER_DEP)
	$(cc) $(cflags) $(CUSEPCHFLAG) $(user_cflags) $(SRC_ROOT)\TIMER.C

EM_TIME.OBJ:	$(SRC_ROOT)\EM_TIME.C $(EM_TIME_DEP)
	$(cc) $(cflags) $(CUSEPCHFLAG) $(user_cflags) $(SRC_ROOT)\EM_TIME.C


$(PROJ).EXE:	WINEMACS.RES

$(PROJ).EXE: \
	membegin.obj memend.obj \
	ABBREV.OBJ ABSPATH.OBJ ARITH.OBJ BUFFER.OBJ CASECONV.OBJ COLUMNS.OBJ \
	DISPLAY.OBJ EMACS.OBJ EMACSRTL.OBJ ERRLOG.OBJ FILEIO.OBJ FILESERV.OBJ FIO.OBJ FUNCTION.OBJ \
	GLOB_VAR.OBJ JOURNAL.OBJ KEYBOARD.OBJ LISPFUNC.OBJ MACROS.OBJ MEM_MAN.OBJ METACOMM.OBJ \
	MINIBUF.OBJ MLISPARS.OBJ MLISPROC.OBJ OPTIONS.OBJ PROMPT.OBJ QUEUE.OBJ SEARCH.OBJ \
	SEARCHIN.OBJ SIMPCOMM.OBJ SUBPROC.OBJ SYNTAX.OBJ TERM.OBJ UNDO.OBJ VARIABLE.OBJ VARTHUNK.OBJ \
	NT_COMM.OBJ WINDMAN.OBJ WINDOW.OBJ NDBM.OBJ DBMAN.OBJ STDAFX.OBJ MAINFRM.OBJ FINDDLG.OBJ \
	NT_ASYNC.OBJ NT_PIPE.OBJ NT_PROC.OBJ \
	SWITCHDL.OBJ WINEMACS.OBJ WINEMDOC.OBJ WINEMVW.OBJ EDOPRINT.OBJ WIN_OPT.OBJ WIN_MENU.OBJ \
	WIN_UI.OBJ WIN_RTL.OBJ TIMER.OBJ EM_TIME.OBJ WINEMACS.RES \
	$(SRC_ROOT)\os.h emversio.exe
	emversio version.c
	$(cc) $(cflags) $(user_cflags) VERSION.C
	echo >NUL @<<$(PROJ).CRF
membegin.obj
ABBREV.OBJ ABSPATH.OBJ ARITH.OBJ BUFFER.OBJ CASECONV.OBJ COLUMNS.OBJ EMACS.OBJ
DISPLAY.OBJ EMACSRTL.OBJ ERRLOG.OBJ FILEIO.OBJ FILESERV.OBJ FIO.OBJ UNDO.OBJ
GLOB_VAR.OBJ JOURNAL.OBJ KEYBOARD.OBJ LISPFUNC.OBJ MACROS.OBJ function.OBJ
METACOMM.OBJ MINIBUF.OBJ MLISPARS.OBJ MLISPROC.OBJ OPTIONS.OBJ PROMPT.OBJ
QUEUE.OBJ SEARCH.OBJ SEARCHIN.OBJ SIMPCOMM.OBJ SUBPROC.OBJ SYNTAX.OBJ TERM.OBJ
VARIABLE.OBJ VARTHUNK.OBJ NT_COMM.OBJ WINDMAN.OBJ WINDOW.OBJ NDBM.OBJ
NT_ASYNC.OBJ NT_PIPE.OBJ NT_PROC.OBJ
DBMAN.OBJ STDAFX.OBJ MAINFRM.OBJ FINDDLG.OBJ SWITCHDL.OBJ WINEMACS.OBJ
WINEMDOC.OBJ WINEMVW.OBJ EDOPRINT.OBJ WIN_OPT.OBJ WIN_MENU.OBJ WIN_UI.OBJ
mem_man.OBJ WIN_RTL.OBJ VERSION.OBJ TIMER.OBJ EM_TIME.OBJ WINEMACS.RES
$(mfclib) $(guilibs)
memend.obj
<<
	$(link) $(guilflags) $(user_lflags) /STACK:200000 /OUT:"emwinnt.exe" @$(PROJ).CRF


#
#	dbadd ultility
#
DBADD.OBJ:	$(SRC_ROOT)\DBADD.C
	$(cc) $(cflags) $(user_cflags) $(SRC_ROOT)\DBADD.C

DB_rtl.OBJ:	$(SRC_ROOT)\DB_rtl.C
	$(cc) $(cflags) $(user_cflags) $(SRC_ROOT)\DB_rtl.C

DBADD.EXE:	DBADD.OBJ NDBM.OBJ db_rtl.obj
	echo >NUL @<<dbadd.CRF
DBADD.OBJ 
NDBM.OBJ db_rtl.obj
$(conlibs)
<<
	$(link) $(conlflags) $(user_lflags) /out:dbadd.exe @dbadd.CRF

#
#	dbdel ultility
#
DBdel.OBJ:	$(SRC_ROOT)\DBdel.C
	$(cc) $(cflags) $(user_cflags) $(SRC_ROOT)\DBdel.C

DBdel.EXE:	DBdel.OBJ NDBM.OBJ db_rtl.obj
	echo >NUL @<<dbdel.CRF
DBdel.OBJ 
NDBM.OBJ db_rtl.obj
$(conlibs)
<<
	$(link) $(conlflags) $(user_lflags) /out:dbdel.exe @dbdel.CRF

#
#	dblist ultility
#
DBlist.OBJ:	$(SRC_ROOT)\DBlist.C
	$(cc) $(cflags) $(user_cflags) $(SRC_ROOT)\DBlist.C

DBlist.EXE:	DBlist.OBJ NDBM.OBJ db_rtl.obj
	echo >NUL @<<dblist.CRF
DBlist.OBJ 
NDBM.OBJ
db_rtl.obj
$(conlibs)
<<
	$(link) $(conlflags) $(user_lflags) /out:dblist.exe @dblist.CRF

#
#	dbprint ultility
#
DBprint.OBJ:	$(SRC_ROOT)\DBprint.C
	$(cc) $(cflags) $(user_cflags) $(SRC_ROOT)\DBprint.C

DBprint.EXE:	DBprint.OBJ NDBM.OBJ db_rtl.obj
	echo >NUL @<<dbprint.CRF
DBprint.OBJ 
NDBM.OBJ
db_rtl.obj
$(conlibs)
<<
	$(link) $(conlflags) $(user_lflags) /out:DBprint.exe @dbprint.CRF

#
#	dbcreate ultility
#
DBcreate.OBJ:	$(SRC_ROOT)\DBcreate.C
	$(cc) $(cflags) $(user_cflags) $(SRC_ROOT)\DBcreate.C

DBcreate.EXE:	DBcreate.OBJ NDBM.OBJ db_rtl.obj
	echo >NUL @<<dbcreate.CRF
DBcreate.OBJ 
NDBM.OBJ
db_rtl.obj
$(conlibs)
<<
	$(link) $(conlflags) $(user_lflags) /out:DBcreate.exe @dbcreate.CRF

#
#	mll-2-db ultility
#
mll-2-db.OBJ:	$(SRC_ROOT)\mll-2-db.C
	$(cc) $(cflags) $(user_cflags) $(SRC_ROOT)\mll-2-db.C

mll-2-db.EXE:	mll-2-db.OBJ NDBM.OBJ db_rtl.obj
	echo >NUL @<<mll-2-db.CRF
mll-2-db.OBJ 
NDBM.OBJ
db_rtl.obj
$(conlibs)
<<
	$(link) $(conlflags) $(user_lflags) /out:mll-2-db.exe @mll-2-db.CRF

#
#	emversio ultility
#
emversio.OBJ:	$(SRC_ROOT)\emversio.C $(SRC_ROOT)\os.h
	$(cc) $(cflags) $(user_cflags) $(SRC_ROOT)\emversio.C

emversio.EXE:	emversio.OBJ NDBM.OBJ db_rtl.obj
	echo >NUL @<<emversio.CRF
emversio.OBJ 
$(conlibs)
<<
	$(link) $(conlflags) $(user_lflags) /out:emversio.exe @emversio.CRF

SBRS = ABBREV.SBR \
		ABSPATH.SBR \
		ARITH.SBR \
		BUFFER.SBR \
		CASECONV.SBR \
		COLUMNS.SBR \
		DISPLAY.SBR \
		EMACS.SBR \
		EMACSRTL.SBR \
		ERRLOG.SBR \
		FILEIO.SBR \
		FILESERV.SBR \
		FIO.SBR \
		FUNCTION.SBR \
		GLOB_VAR.SBR \
		JOURNAL.SBR \
		KEYBOARD.SBR \
		LISPFUNC.SBR \
		MACROS.SBR \
		MEM_MAN.SBR \
		METACOMM.SBR \
		MINIBUF.SBR \
		MLISPARS.SBR \
		MLISPROC.SBR \
		OPTIONS.SBR \
		PROMPT.SBR \
		QUEUE.SBR \
		SEARCH.SBR \
		SEARCHIN.SBR \
		SIMPCOMM.SBR \
		SUBPROC.SBR \
		SYNTAX.SBR \
		TERM.SBR \
		UNDO.SBR \
		VARIABLE.SBR \
		VARTHUNK.SBR \
		NT_COMM.SBR \
		WINDMAN.SBR \
		WINDOW.SBR \
		NDBM.SBR \
		DBMAN.SBR \
		STDAFX.SBR \
		MAINFRM.SBR \
		FINDDLG.SBR \
		SWITCHDL.SBR \
		WINEMACS.SBR \
		WINEMDOC.SBR \
		WINEMVW.SBR \
		EDOPRINT.SBR \
		NT_ASYNC.SBR \
		NT_COMM.SBR \
		NT_PIPE.SBR \
		NT_PROC.SBR \
		WIN_OPT.SBR \
		WIN_MENU.SBR \
		WIN_UI.SBR \
		WIN_RTL.SBR \
		VERSION.SBR \
		TIMER.SBR \
		EM_TIME.SBR

$(PROJ).BSC: $(SBRS)
	bscmake @<<
/o$@ $(SBRS)
<<
