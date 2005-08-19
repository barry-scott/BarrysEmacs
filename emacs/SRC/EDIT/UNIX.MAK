#
#	Make file for EMacs EDIT component
#
cc_flags = -c -g -Dunix -I$(edit_src) -I$(edit__src) -I$(edit_obj)
cc_relaxed = $(CC_NOANSI)
cc = $(CC_ANSI)
obj_files = $(edit_obj)membegin.o \
 $(edit_obj)abbrev.o \
 $(edit_obj)motifcmd.o \
 $(edit_obj)unixcomm.o \
 $(edit_obj)motiftk.o \
 $(edit_obj)motiftrm.o \
 $(edit_obj)abspath.o \
 $(edit_obj)arith.o \
 $(edit_obj)buffer.o \
 $(edit_obj)caseconv.o \
 $(edit_obj)columns.o \
 $(edit_obj)dbman.o \
 $(edit_obj)display.o \
 $(edit_obj)edoprint.o \
 $(edit_obj)emacs.o \
 $(edit_obj)em_time.o \
 $(edit_obj)emacsrtl.o \
 $(edit_obj)errlog.o \
 $(edit_obj)fileio.o \
 $(edit_obj)fio.o \
 $(edit_obj)function.o \
 $(edit_obj)glob_var.o \
 $(edit_obj)keyboard.o \
 $(edit_obj)lispfunc.o \
 $(edit_obj)macros.o \
 $(edit_obj)mem_man.o \
 $(edit_obj)metacomm.o \
 $(edit_obj)minibuf.o \
 $(edit_obj)mlispars.o \
 $(edit_obj)mlisproc.o \
 $(edit_obj)ndbm.o \
 $(edit_obj)options.o \
 $(edit_obj)queue.o \
 $(edit_obj)search.o \
 $(edit_obj)searchin.o \
 $(edit_obj)simpcomm.o \
 $(edit_obj)subproc.o \
 $(edit_obj)syntax.o \
 $(edit_obj)term.o \
 $(edit_obj)timer.o \
 $(edit_obj)trm_ansi.o \
 $(edit_obj)undo.o \
 $(edit_obj)varthunk.o \
 $(edit_obj)windman.o \
 $(edit_obj)window.o \
 $(edit_obj)prompt.o \
 $(edit_obj)journal.o \
 $(edit_obj)fileserv.o \
 $(edit_obj)unix_rtl.o \
 $(edit_obj)unix_trm.o \
 $(edit_obj)variable.o \
 $(edit_obj)memend.o

editor : $(edit_obj)dbgemacs $(emacs_exe)emacs $(emacs_exe)mll-2-db $(emacs_exe)dbadd $(emacs_exe)dbcreate $(emacs_exe)dblist $(emacs_exe)dbdel $(emacs_exe)dbprint

$(emacs_exe)dbadd : $(edit_obj)dbadd.o $(edit_obj)ndbm.o $(edit_obj)db_rtl.o
	$(cc) -o $(edit_obj)dbadd -g $(edit_obj)dbadd.o $(edit_obj)ndbm.o $(edit_obj)db_rtl.o
	cp $(edit_obj)dbadd $(emacs_exe)dbadd
	strip $(emacs_exe)dbadd

$(edit_obj)dbadd.o : $(edit_src)dbadd.c
	$(cc) $(cc_flags) -o $(edit_obj)dbadd.o $(edit_src)dbadd.c

$(emacs_exe)mll-2-db : $(edit_obj)mll-2-db.o $(edit_obj)ndbm.o $(edit_obj)db_rtl.o
	$(cc) -o $(edit_obj)mll-2-db -g $(edit_obj)mll-2-db.o $(edit_obj)ndbm.o $(edit_obj)db_rtl.o
	cp $(edit_obj)mll-2-db $(emacs_exe)mll-2-db
	strip $(emacs_exe)mll-2-db

$(edit_obj)mll-2-db.o : $(edit_src)mll-2-db.c
	$(cc) $(cc_flags) -o $(edit_obj)mll-2-db.o $(edit_src)mll-2-db.c

$(emacs_exe)dbcreate : $(edit_obj)dbcreate.o $(edit_obj)ndbm.o $(edit_obj)db_rtl.o
	$(cc) -o $(edit_obj)dbcreate -g $(edit_obj)dbcreate.o $(edit_obj)ndbm.o $(edit_obj)db_rtl.o
	cp $(edit_obj)dbcreate $(emacs_exe)dbcreate
	strip $(emacs_exe)dbcreate

$(edit_obj)dbcreate.o : $(edit_src)dbcreate.c
	$(cc) $(cc_flags) -o $(edit_obj)dbcreate.o $(edit_src)dbcreate.c

$(edit_obj)db_rtl.o : $(edit_src)db_rtl.c
	$(cc) $(cc_flags) -o $(edit_obj)db_rtl.o $(edit_src)db_rtl.c

$(emacs_exe)dblist : $(edit_obj)dblist.o $(edit_obj)ndbm.o $(edit_obj)db_rtl.o
	$(cc) -o $(edit_obj)dblist -g $(edit_obj)dblist.o $(edit_obj)ndbm.o $(edit_obj)db_rtl.o
	cp $(edit_obj)dblist $(emacs_exe)dblist
	strip $(emacs_exe)dblist

$(edit_obj)dblist.o : $(edit_src)dblist.c
	$(cc) $(cc_flags) -o $(edit_obj)dblist.o $(edit_src)dblist.c

$(emacs_exe)dbprint : $(edit_obj)dbprint.o $(edit_obj)ndbm.o $(edit_obj)db_rtl.o
	$(cc) -o $(edit_obj)dbprint -g $(edit_obj)dbprint.o $(edit_obj)ndbm.o $(edit_obj)db_rtl.o
	cp $(edit_obj)dbprint $(emacs_exe)dbprint
	strip $(emacs_exe)dbprint

$(edit_obj)dbprint.o : $(edit_src)dbprint.c
	$(cc) $(cc_flags) -o $(edit_obj)dbprint.o $(edit_src)dbprint.c

$(emacs_exe)dbdel : $(edit_obj)dbdel.o $(edit_obj)ndbm.o $(edit_obj)db_rtl.o
	$(cc) -o $(edit_obj)dbdel -g $(edit_obj)dbdel.o $(edit_obj)ndbm.o $(edit_obj)db_rtl.o
	cp $(edit_obj)dbdel $(emacs_exe)dbdel
	strip $(emacs_exe)dbdel

$(edit_obj)dbdel.o : $(edit_src)dbdel.c
	$(cc) $(cc_flags) -o $(edit_obj)dbdel.o $(edit_src)dbdel.c

$(edit_obj)dbgemacs : $(edit_obj)emversio $(obj_files)
	$(edit_obj)emversio $(edit_obj)version.c
	$(cc) $(cc_flags) -o $(edit_obj)version.o $(edit_obj)version.c
	$(cc) -o $(edit_obj)dbgemacs -g $(obj_files) $(edit_obj)version.o -lXm -lXt -lX11 -ldnet

$(emacs_exe)emacs : $(edit_obj)dbgemacs
	cp $(edit_obj)dbgemacs $(emacs_exe)emacs
	strip $(emacs_exe)emacs

$(edit_obj)unix_rtl.o : $(edit_src)unix_rtl.c
	$(cc) $(cc_flags) -o $(edit_obj)unix_rtl.o $(edit_src)unix_rtl.c

$(edit_obj)motiftrm.o : $(edit_src)motiftrm.c
	$(cc_relaxed) $(cc_flags) -o $(edit_obj)motiftrm.o $(edit_src)motiftrm.c

$(edit_obj)motiftk.o : $(edit_src)motiftk.c
	$(cc_relaxed) $(cc_flags) -o $(edit_obj)motiftk.o $(edit_src)motiftk.c

$(edit_obj)motifcmd.o : $(edit_src)motifcmd.c
	$(cc_relaxed) $(cc_flags) -o $(edit_obj)motifcmd.o $(edit_src)motifcmd.c

$(edit_obj)unix_trm.o : $(edit_src)unix_trm.c
	$(cc) $(cc_flags) -o $(edit_obj)unix_trm.o $(edit_src)unix_trm.c

$(edit_obj)abbrev.o : $(edit_src)abbrev.c
	$(cc) $(cc_flags) -o $(edit_obj)abbrev.o $(edit_src)abbrev.c

$(edit_obj)abspath.o : $(edit_src)abspath.c
	$(cc) $(cc_flags) -o $(edit_obj)abspath.o $(edit_src)abspath.c

$(edit_obj)arith.o : $(edit_src)arith.c
	$(cc) $(cc_flags) -o $(edit_obj)arith.o $(edit_src)arith.c

$(edit_obj)timer.o : $(edit_src)timer.c
	$(cc) $(cc_flags) -o $(edit_obj)timer.o $(edit_src)timer.c

$(edit_obj)em_time.o : $(edit_src)em_time.c
	$(cc) $(cc_flags) -o $(edit_obj)em_time.o $(edit_src)em_time.c

$(edit_obj)buffer.o : $(edit_src)buffer.c
	$(cc) $(cc_flags) -o $(edit_obj)buffer.o $(edit_src)buffer.c

$(edit_obj)caseconv.o : $(edit_src)caseconv.c
	$(cc) $(cc_flags) -o $(edit_obj)caseconv.o $(edit_src)caseconv.c

$(edit_obj)columns.o : $(edit_src)columns.c
	$(cc) $(cc_flags) -o $(edit_obj)columns.o $(edit_src)columns.c

$(edit_obj)dbman.o : $(edit_src)dbman.c
	$(cc) $(cc_flags) -o $(edit_obj)dbman.o $(edit_src)dbman.c

$(edit_obj)display.o : $(edit_src)display.c
	$(cc) $(cc_flags) -o $(edit_obj)display.o $(edit_src)display.c

$(edit_obj)edoprint.o : $(edit_src)edoprint.c $(edit_src)doprint.c
	$(cc) $(cc_flags) -o $(edit_obj)edoprint.o $(edit_src)edoprint.c

$(edit_obj)emacs.o : $(edit_src)emacs.c
	$(cc) $(cc_flags) -o $(edit_obj)emacs.o $(edit_src)emacs.c

$(edit_obj)emacsrtl.o : $(edit_src)emacsrtl.c
	$(cc) $(cc_flags) -o $(edit_obj)emacsrtl.o $(edit_src)emacsrtl.c

$(edit_obj)errlog.o : $(edit_src)errlog.c
	$(cc) $(cc_flags) -o $(edit_obj)errlog.o $(edit_src)errlog.c

$(edit_obj)fileio.o : $(edit_src)fileio.c
	$(cc) $(cc_flags) -o $(edit_obj)fileio.o $(edit_src)fileio.c

$(edit_obj)fileserv.o : $(edit_src)fileserv.c
	$(cc) $(cc_flags) -o $(edit_obj)fileserv.o $(edit_src)fileserv.c

$(edit_obj)fio.o : $(edit_src)fio.c
	$(cc) $(cc_flags) -o $(edit_obj)fio.o $(edit_src)fio.c

$(edit_obj)function.o : $(edit_src)function.c $(edit_src)funclist.h
	$(cc) $(cc_flags) -o $(edit_obj)function.o $(edit_src)function.c

$(edit_obj)glob_var.o : $(edit_src)glob_var.c
	$(cc) $(cc_flags) -o $(edit_obj)glob_var.o $(edit_src)glob_var.c

$(edit_obj)journal.o : $(edit_src)journal.c
	$(cc) $(cc_flags) -o $(edit_obj)journal.o $(edit_src)journal.c

$(edit_obj)keyboard.o : $(edit_src)keyboard.c
	$(cc) $(cc_flags) -o $(edit_obj)keyboard.o $(edit_src)keyboard.c

$(edit_obj)lispfunc.o : $(edit_src)lispfunc.c
	$(cc) $(cc_flags) -o $(edit_obj)lispfunc.o $(edit_src)lispfunc.c

$(edit_obj)macros.o : $(edit_src)macros.c
	$(cc) $(cc_flags) -o $(edit_obj)macros.o $(edit_src)macros.c

$(edit_obj)mem_man.o : $(edit_src)mem_man.c
	$(cc) $(cc_flags) -o $(edit_obj)mem_man.o $(edit_src)mem_man.c

$(edit_obj)membegin.o : $(edit_src)membegin.c
	$(cc) $(cc_flags) -o $(edit_obj)membegin.o $(edit_src)membegin.c

$(edit_obj)memend.o : $(edit_src)memend.c
	$(cc) $(cc_flags) -o $(edit_obj)memend.o $(edit_src)memend.c

$(edit_obj)metacomm.o : $(edit_src)metacomm.c
	$(cc) $(cc_flags) -o $(edit_obj)metacomm.o $(edit_src)metacomm.c

$(edit_obj)minibuf.o : $(edit_src)minibuf.c
	$(cc) $(cc_flags) -o $(edit_obj)minibuf.o $(edit_src)minibuf.c

$(edit_obj)mlispars.o : $(edit_src)mlispars.c
	$(cc) $(cc_flags) -o $(edit_obj)mlispars.o $(edit_src)mlispars.c

$(edit_obj)mlisproc.o : $(edit_src)mlisproc.c
	$(cc) $(cc_flags) -o $(edit_obj)mlisproc.o $(edit_src)mlisproc.c

$(edit_obj)ndbm.o : $(edit_src)ndbm.c
	$(cc) $(cc_flags) -o $(edit_obj)ndbm.o $(edit_src)ndbm.c

$(edit_obj)options.o : $(edit_src)options.c
	$(cc) $(cc_flags) -o $(edit_obj)options.o $(edit_src)options.c

$(edit_obj)osfile.o : $(edit_src)osfile.c
	$(cc) $(cc_flags) -o $(edit_obj)osfile.o $(edit_src)osfile.c

$(edit_obj)osinter.o : $(edit_src)osinter.c
	$(cc) $(cc_flags) -o $(edit_obj)osinter.o $(edit_src)osinter.c

$(edit_obj)profile.o : $(edit_src)profile.c
	$(cc) $(cc_flags) -o $(edit_obj)profile.o $(edit_src)profile.c

$(edit_obj)prompt.o : $(edit_src)prompt.c
	$(cc) $(cc_flags) -o $(edit_obj)prompt.o $(edit_src)prompt.c

$(edit_obj)queue.o : $(edit_src)queue.c
	$(cc) $(cc_flags) -o $(edit_obj)queue.o $(edit_src)queue.c

$(edit_obj)search.o : $(edit_src)search.c
	$(cc) $(cc_flags) -o $(edit_obj)search.o $(edit_src)search.c

$(edit_obj)searchin.o : $(edit_src)searchin.c
	$(cc) $(cc_flags) -o $(edit_obj)searchin.o $(edit_src)searchin.c

$(edit_obj)simpcomm.o : $(edit_src)simpcomm.c
	$(cc) $(cc_flags) -o $(edit_obj)simpcomm.o $(edit_src)simpcomm.c

$(edit_obj)subproc.o : $(edit_src)subproc.c
	$(cc) $(cc_flags) -o $(edit_obj)subproc.o $(edit_src)subproc.c

$(edit_obj)syntax.o : $(edit_src)syntax.c
	$(cc) $(cc_flags) -o $(edit_obj)syntax.o $(edit_src)syntax.c

$(edit_obj)term.o : $(edit_src)term.c $(edit_src)term.h $(edit_src)tt.h
	$(cc) $(cc_flags) -o $(edit_obj)term.o $(edit_src)term.c

$(edit_obj)trm_ansi.o : $(edit_src)trm_ansi.c $(edit_src)term.h $(edit_src)tt.h
	$(cc) $(cc_flags) -o $(edit_obj)trm_ansi.o $(edit_src)trm_ansi.c

$(edit_obj)undo.o : $(edit_src)undo.c
	$(cc) $(cc_flags) -o $(edit_obj)undo.o $(edit_src)undo.c

$(edit_obj)variable.o : $(edit_src)variable.c $(edit_src)varlist.h
	$(cc) $(cc_flags) -o $(edit_obj)variable.o $(edit_src)variable.c

$(edit_obj)varthunk.o : $(edit_src)varthunk.c
	$(cc) $(cc_flags) -o $(edit_obj)varthunk.o $(edit_src)varthunk.c

$(edit_obj)vmsabs.o : $(edit_src)vmsabs.c
	$(cc) $(cc_flags) -o $(edit_obj)vmsabs.o $(edit_src)vmsabs.c

$(edit_obj)vms_comm.o : $(edit_src)vms_comm.c
	$(cc) $(cc_flags) -o $(edit_obj)vms_comm.o $(edit_src)vms_comm.c

$(edit_obj)unixcomm.o : $(edit_src)unixcomm.c
	$(cc_relaxed) $(cc_flags) -o $(edit_obj)unixcomm.o $(edit_src)unixcomm.c

$(edit_obj)windman.o : $(edit_src)windman.c
	$(cc) $(cc_flags) -o $(edit_obj)windman.o $(edit_src)windman.c

$(edit_obj)window.o : $(edit_src)window.c
	$(cc) $(cc_flags) -o $(edit_obj)window.o $(edit_src)window.c

$(edit_obj)emversio	: $(edit_obj)emversio.o
	$(cc) -o $(edit_obj)emversio $(edit_obj)emversio.o

$(edit_obj)emversio.o : $(edit_src)emversio.c
	$(cc) $(cc_flags) -o $(edit_obj)emversio.o $(edit_src)emversio.c

