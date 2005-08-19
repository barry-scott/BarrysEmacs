#
#	Emacs make file
#
quick=1

!ifndef appclass
appclass=winnt
!endif

!if "$(appclass)" == "dos386"
CL=WCL386
link=WCL386 -l:dos4g
CPU=i386
ENVVAR=WCL386
OBJ=d:\work\emacs\msdos32
SRC=d:\work\emacs\src\edit
APP_OPT=/i=$(SRC)
!ifdef nodebug
CPU_OPT=
!else
CPU_OPT=-d2
!endif
WIN_EXE=emacs386

C_OPT=$(CPU_OPT) $(APP_OPT) -fo$(OBJ)\ -zq -w4
!endif

!ifndef OBJ
!error Unknown appclass type $(appclass) - use 86, 286, 386, winnt, win32, win16
!endif


!ifndef CL
ENVVAR=cl
CL=cl
!endif

!ifndef link
link=link
!endif

{$(SRC)\}.c{$(OBJ)\}.obj:
	set $(ENVVAR)=$(C_OPT)
	-$(CL) /c $(SRC)\%|fF.c >$(TMP)\cl.tmp
	erfilter $(OBJ)\%|fF.er2 <$(TMP)\cl.tmp

#############################################################
#
#
#	Figure out the images that need building
#
#
#############################################################
all :	$(OBJ)\dbadd.exe \
	$(OBJ)\dbdel.exe \
	$(OBJ)\dblist.exe \
	$(OBJ)\dbprint.exe \
	$(OBJ)\dbcreate.exe \
	$(OBJ)\mll-2-db.exe \
	$(OBJ)\emacs386.exe
		echo all done for $(WIN_EXE)

$(OBJ)\dbadd.exe : $(OBJ)\dbadd.obj $(OBJ)\ndbm.obj $(OBJ)\db_rtl.obj
	cd $(OBJ)
	echo Linking DDBADD.EXE
	wlink @<<
sys dos4g
option stack=50k
debug all
name ddbadd
option map=ddbadd.map
file dbadd.obj
file ndbm.obj 
file db_rtl.obj 
<<
	echo Linking DBADD.EXE
	wlink @<<
sys dos4g
option stack=50k
name dbadd
option map=dbadd.map
file dbadd.obj
file ndbm.obj 
file db_rtl.obj 
<<
	cd $(SRC)

$(OBJ)\dbadd.obj : dbadd.c

$(OBJ)\mll-2-db.exe : $(OBJ)\mll-2-db.obj $(OBJ)\ndbm.obj $(OBJ)\db_rtl.obj
	cd $(OBJ)
	echo Linking Dmll-2-db.EXE
	wlink @<<
sys dos4g
option stack=50k
debug all
name dmll-2-d
option map=dmll-2-d.map
file mll-2-db.obj
file ndbm.obj 
file db_rtl.obj 
<<
	echo Linking mll-2-db.EXE
	wlink @<<
sys dos4g
option stack=50k
name mll-2-db
option map=mll-2-db.map
file mll-2-db.obj
file ndbm.obj 
file db_rtl.obj 
<<
	cd $(SRC)

$(OBJ)\mll-2-db.obj : mll-2-db.c

$(OBJ)\dbdel.exe : $(OBJ)\dbdel.obj $(OBJ)\ndbm.obj $(OBJ)\db_rtl.obj
	cd $(OBJ)
	echo Linking DDBdel.EXE
	wlink @<<
sys dos4g
option stack=50k
debug all
name ddbdel
option map=ddbdel.map
file dbdel.obj
file ndbm.obj 
file db_rtl.obj 
<<
	echo Linking DBdel.EXE
	wlink @<<
sys dos4g
option stack=50k
name dbdel
option map=dbdel.map
file dbdel.obj
file ndbm.obj 
file db_rtl.obj 
<<
	cd $(SRC)

$(OBJ)\dbdel.obj : dbdel.c

$(OBJ)\dblist.exe : $(OBJ)\dblist.obj $(OBJ)\ndbm.obj $(OBJ)\db_rtl.obj
	cd $(OBJ)
	echo Linking DDBlist.EXE
	wlink @<<
sys dos4g
option stack=50k
debug all
name ddblist
option map=ddblist.map
file dblist.obj
file ndbm.obj 
file db_rtl.obj 
<<
	echo Linking DBlist.EXE
	wlink @<<
sys dos4g
option stack=50k
name dblist
option map=dblist.map
file dblist.obj
file ndbm.obj 
file db_rtl.obj 
<<
	cd $(SRC)

$(OBJ)\dblist.obj : dblist.c

$(OBJ)\dbprint.exe : $(OBJ)\dbprint.obj $(OBJ)\ndbm.obj $(OBJ)\db_rtl.obj
	cd $(OBJ)
	echo Linking DDBprint.EXE
	wlink @<<
sys dos4g
option stack=50k
debug all
name ddbprint
option map=ddbprint.map
file dbprint.obj
file ndbm.obj 
file db_rtl.obj 
<<
	echo Linking DBprint.EXE
	wlink @<<
sys dos4g
option stack=50k
name dbprint
option map=dbprint.map
file dbprint.obj
file ndbm.obj 
file db_rtl.obj 
<<
	cd $(SRC)

$(OBJ)\dbprint.obj : dbprint.c

$(OBJ)\dbcreate.exe : $(OBJ)\dbcreate.obj $(OBJ)\db_rtl.obj $(OBJ)\ndbm.obj
	cd $(OBJ)
	echo Linking Ddbcreate.EXE
	wlink @<<
sys dos4g
option stack=50k
debug all
name ddbcreat
option map=ddbcreat.map
file dbcreate.obj
file ndbm.obj
file db_rtl.obj 
<<
	echo Linking dbcreate.EXE
	wlink @<<
sys dos4g
option stack=50k
name dbcreate
option map=dbcreate.map
file dbcreate.obj
file ndbm.obj
file db_rtl.obj 
<<
	cd $(SRC)

$(OBJ)\dbcreate.obj : dbcreate.c

$(OBJ)\db_rtl.obj : db_rtl.c

$(OBJ)\emacs386.exe : \
		emacs.mak \
		$(OBJ)\emversio.exe \
		$(OBJ)\aaaa_mem.obj \
		$(OBJ)\dos_load.obj \
		$(OBJ)\ms_mouse.obj \
		$(OBJ)\trmibmpc.obj \
		$(OBJ)\em_time.obj \
		$(OBJ)\timer.obj \
		$(OBJ)\fileserv.obj \
		$(OBJ)\abbrev.obj \
		$(OBJ)\arith.obj \
		$(OBJ)\buffer.obj \
		$(OBJ)\caseconv.obj \
		$(OBJ)\columns.obj \
		$(OBJ)\display.obj \
		$(OBJ)\edoprint.obj \
		$(OBJ)\emacs.obj \
		$(OBJ)\emacsrtl.obj \
		$(OBJ)\errlog.obj \
		$(OBJ)\function.obj \
		$(OBJ)\glob_var.obj \
		$(OBJ)\keyboard.obj \
		$(OBJ)\mlispars.obj \
		$(OBJ)\mlisproc.obj \
		$(OBJ)\mlprintf.obj \
		$(OBJ)\options.obj \
		$(OBJ)\lispfunc.obj \
		$(OBJ)\macros.obj \
		$(OBJ)\metacomm.obj \
		$(OBJ)\minibuf.obj \
		$(OBJ)\prompt.obj \
		$(OBJ)\queue.obj \
		$(OBJ)\search.obj \
		$(OBJ)\searchin.obj \
		$(OBJ)\simpcomm.obj \
		$(OBJ)\subproc.obj \
		$(OBJ)\syntax.obj \
		$(OBJ)\undo.obj \
		$(OBJ)\term.obj \
		$(OBJ)\varthunk.obj \
		$(OBJ)\vms_comm.obj \
		$(OBJ)\windman.obj \
		$(OBJ)\window.obj \
		$(OBJ)\ndbm.obj \
		$(OBJ)\variable.obj \
		$(OBJ)\journal.obj \
		$(OBJ)\abspath.obj \
		$(OBJ)\fileio.obj \
		$(OBJ)\fio.obj \
		$(OBJ)\dbman.obj \
		$(OBJ)\mem_man.obj \
		$(OBJ)\zzzz_mem.obj
	echo Linking debug emacs386
	cd $(OBJ)
	emversio version.c
	$(CL) -c -I$(SRC) version.c
	wlink @<<
sys dos4g
option stack=300k
debug all
name emacs386
option map=emacs386.map
file aaaa_mem.obj
file dos_load.obj
file ms_mouse.obj
file trmibmpc.obj
file em_time.obj
file timer.obj
file fileserv.obj 
file abbrev.obj 
file arith.obj 
file buffer.obj 
file caseconv.obj 
file columns.obj 
file dbman.obj 
file display.obj 
file edoprint.obj 
file emacs.obj 
file emacsrtl.obj 
file errlog.obj 
file fileio.obj 
file fio.obj 
file function.obj 
file glob_var.obj 
file journal.obj 
file keyboard.obj 
file options.obj 
file lispfunc.obj 
file macros.obj 
file metacomm.obj 
file minibuf.obj 
file mlispars.obj 
file mlisproc.obj 
file mlprintf.obj 
file ndbm.obj 
file prompt.obj 
file queue.obj 
file search.obj 
file searchin.obj 
file simpcomm.obj 
file subproc.obj 
file syntax.obj 
file term.obj 
file undo.obj 
file variable.obj 
file varthunk.obj 
file vms_comm.obj 
file windman.obj 
file window.obj 
file mem_man.obj 
file abspath.obj 
file version.obj
file zzzz_mem.obj
<<
	echo Linking EMDOS32.EXE
	wlink @<<
sys dos4g
option stack=300k
name emdos32
option map=emdos32.map
file aaaa_mem.obj
file dos_load.obj
file ms_mouse.obj
file trmibmpc.obj
file em_time.obj
file timer.obj
file fileserv.obj 
file abbrev.obj 
file arith.obj 
file buffer.obj 
file caseconv.obj 
file columns.obj 
file dbman.obj 
file display.obj 
file edoprint.obj 
file emacs.obj 
file emacsrtl.obj 
file errlog.obj 
file fileio.obj 
file fio.obj 
file function.obj 
file glob_var.obj 
file journal.obj 
file keyboard.obj 
file options.obj 
file lispfunc.obj 
file macros.obj 
file metacomm.obj 
file minibuf.obj 
file mlispars.obj 
file mlisproc.obj 
file mlprintf.obj 
file ndbm.obj 
file prompt.obj 
file queue.obj 
file search.obj 
file searchin.obj 
file simpcomm.obj 
file subproc.obj 
file syntax.obj 
file term.obj 
file undo.obj 
file variable.obj 
file varthunk.obj 
file vms_comm.obj 
file windman.obj 
file window.obj 
file mem_man.obj 
file abspath.obj 
file version.obj
file zzzz_mem.obj
<<
	cd $(SRC)

$(OBJ)\ms_mouse.obj : ms_mouse.c

$(OBJ)\em_time.obj : em_time.c em_time.h

$(OBJ)\timer.obj : timer.c

$(OBJ)\dos_load.obj : dos_load.c

$(OBJ)\win_load.obj : win_load.c emacswin.h os.h os_win.h

$(OBJ)\win_term.obj : win_term.c emacswin.h os.h os_win.h display.h tt.h

$(OBJ)\win_cmd.obj : win_cmd.c emacswin.h os.h os_win.h

$(OBJ)\abbrev.obj : abbrev.c

$(OBJ)\abspath.obj : abspath.c

$(OBJ)\arith.obj : arith.c

$(OBJ)\buffer.obj : buffer.c

$(OBJ)\dbg_stub.obj : dbg_stub.c

$(OBJ)\caseconv.obj : caseconv.c

$(OBJ)\columns.obj : columns.c

$(OBJ)\dbman.obj : dbman.c ndbm.h

$(OBJ)\display.obj : display.c display.h

$(OBJ)\edoprint.obj : edoprint.c doprint.c

$(OBJ)\emacsrtl.obj : emacsrtl.c

$(OBJ)\errlog.obj : errlog.c

$(OBJ)\fileio.obj : fileio.c

$(OBJ)\fileserv.obj : fileserv.c

$(OBJ)\fio.obj : fio.c

$(OBJ)\function.obj : function.c funclist.h

$(OBJ)\glob_var.obj : glob_var.c

$(OBJ)\journal.obj : journal.c journal.h

$(OBJ)\options.obj : options.c

$(OBJ)\lispfunc.obj : lispfunc.c

$(OBJ)\macros.obj : macros.c

$(OBJ)\mem_man.obj : mem_man.c

$(OBJ)\aaaa_mem.obj : aaaa_mem.c

$(OBJ)\zzzz_mem.obj : zzzz_mem.c

$(OBJ)\metacomm.obj : metacomm.c

$(OBJ)\minibuf.obj : minibuf.c

$(OBJ)\mlispars.obj : mlispars.c

$(OBJ)\mlisproc.obj : mlisproc.c

$(OBJ)\mlprintf.obj : mlprintf.c

$(OBJ)\ndbm.obj : ndbm.c ndbm.h

$(OBJ)\profile.obj : profile.c

$(OBJ)\prompt.obj : prompt.c

$(OBJ)\queue.obj : queue.c

$(OBJ)\search.obj : search.c

$(OBJ)\searchin.obj : searchin.c

$(OBJ)\simpcomm.obj : simpcomm.c

$(OBJ)\start.obj : start.c

$(OBJ)\subpro.obj : subpro.c

$(OBJ)\syntax.obj : syntax.c

$(OBJ)\trm_ansi.obj : trm_ansi.c

$(OBJ)\trmibmpc.obj : trmibmpc.c

$(OBJ)\term.obj : term.c

$(OBJ)\undo.obj : undo.c

$(OBJ)\variable.obj : variable.c varlist.h

$(OBJ)\varthunk.obj : varthunk.c

$(OBJ)\vms_comm.obj : vms_comm.c

$(OBJ)\windman.obj : windman.c

$(OBJ)\emversio.obj : emversio.c
!if "$(appclass)" == "win386" || "$(appclass)" == "dos386"
	rem done in the link
!endif

$(OBJ)\emversio.exe : emacs.mak \
		$(OBJ)\emversio.obj
	cd $(OBJ)
!if "$(appclass)" == "winnt"
	$(link) $(link_debug) $(conflags) -out:emversio.exe -map:emversio.map emversio.obj \
$(LIB_C)\libcmt.lib \
$(LIB_C)\ADVAPI32.lib \
$(LIB_C)\KERNEL32.lib
!endif
!if "$(appclass)" == "win386" || "$(appclass)" == "dos386"
	wcl386 -l=dos4g -d2 $(SRC)\emversio.c
!endif
	cd $(SRC)

# [end]
