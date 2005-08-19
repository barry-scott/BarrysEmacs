#
#	Make file for Emacs EDIT component

edit_obj=obj/
edit_exe=exe/

cc_opt=-g1 -ptr$(edit_obj)/ptrepository
cc_flags = -c -z $(cc_opt) -D_DEBUG -I. -I/usr/include/X11R6 -I/usr/include/Motif1.2 -I/bnr/contrib/X11R6/include -D_HPUX_SOURCE -Dvolatile=
ld_flags = -z $(cc_opt)
cpp = CC +eh +p
cc = CC +p
ldstatic = CC +eh +A -L/usr/lib/Motif1.2 -L/usr/lib/X11R6 -L/bnr/contrib/X11R6/lib $(ld_flags)
lddynamic = CC +eh -L/usr/lib/Motif1.2 -L/usr/lib/X11R6 -L/bnr/contrib/X11R6/lib $(ld_flags)
#edit_src=
pure=purify purecov

obj_files = $(edit_obj)abbrev.o \
 $(edit_obj)abspath.o \
 $(edit_obj)arith.o \
 $(edit_obj)buffer.o \
 $(edit_obj)buf_man.o \
 $(edit_obj)caseconv.o \
 $(edit_obj)columns.o \
 $(edit_obj)commandline.o \
 $(edit_obj)dbman.o \
 $(edit_obj)display.o \
 $(edit_obj)doprint.o \
 $(edit_obj)em_time.o \
 $(edit_obj)em_stat.o \
 $(edit_obj)emacs.o \
 $(edit_obj)emacsrtl.o \
 $(edit_obj)emarray.o \
 $(edit_obj)emstring.o \
 $(edit_obj)emstrtab.o \
 $(edit_obj)errlog.o \
 $(edit_obj)ext_func.o \
 $(edit_obj)fileio.o \
 $(edit_obj)fio.o \
 $(edit_obj)function.o \
 $(edit_obj)getdb.o \
 $(edit_obj)getfile.o \
 $(edit_obj)getdirectory.o \
 $(edit_obj)glob_var.o \
 $(edit_obj)gui_input_mode.o \
 $(edit_obj)journal.o \
 $(edit_obj)keyboard.o \
 $(edit_obj)key_names.o \
 $(edit_obj)lispfunc.o \
 $(edit_obj)macros.o \
 $(edit_obj)mem_man.o \
 $(edit_obj)metacomm.o \
 $(edit_obj)minibuf.o \
 $(edit_obj)mlispars.o \
 $(edit_obj)mlisproc.o \
 $(edit_obj)mlispexp.o \
 $(edit_obj)mlprintf.o \
 $(edit_obj)ndbm.o \
 $(edit_obj)process.o \
 $(edit_obj)queue.o \
 $(edit_obj)save_env.o \
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
 $(edit_obj)unixfile.o \
 $(edit_obj)unix_ext_func.o \
 $(edit_obj)unix_rtl.o \
 $(edit_obj)unix_trm.o \
 $(edit_obj)variable.o \
 $(edit_obj)unixcomm.o \
 $(edit_obj)emacs_signal.o \
 $(edit_obj)motiftrm.o \
 $(edit_obj)motiftk.o \
 $(edit_obj)motifcmd.o \
 $(edit_obj)motif_keys.o \
 $(edit_obj)emacs_motif.o \
 $(edit_obj)search.o \
 $(edit_obj)options.o \
 $(edit_obj)emacs_init.o

all: editor dbutils

editor : $(edit_obj)bemacs_server $(edit_exe)bemacs_server $(edit_obj)bemacs $(edit_exe)bemacs

dbutils: $(edit_exe)mll-2-db $(edit_exe)dbadd $(edit_exe)dbcreate $(edit_exe)dblist $(edit_exe)dbdel $(edit_exe)dbprint

editor.pure: $(edit_obj)emacs.pure

db_obj_files= \
 $(edit_obj)ndbm.o \
 $(edit_obj)db_rtl.o \
 $(edit_obj)stub_rtl.o \
 $(edit_obj)emstring.o \
 $(edit_obj)em_stat.o \
 $(edit_obj)unixfile.o \
 $(edit_obj)doprint.o

$(edit_exe)dbadd : $(edit_obj)dbadd.o $(db_obj_files)
	@ echo Make: $@
	@ $(cpp) -o $(edit_obj)dbadd -g1 $(edit_obj)dbadd.o $(db_obj_files)
	cp $(edit_obj)dbadd $(edit_exe)dbadd
	strip $(edit_exe)dbadd

$(edit_obj)dbadd.o : $(edit_src)dbadd.cpp
	@ echo Make: $@
	@ $(cpp) $(cc_flags) -o $(edit_obj)dbadd.o $(edit_src)dbadd.cpp

$(edit_obj)bemacs : $(edit_obj)emclient.o $(edit_obj)em_stat.o $(edit_obj)emstring.o $(edit_obj)stub_rtl.o
	@ echo Make: $@
	@ $(cpp) -o $(edit_obj)bemacs -g1 $(edit_obj)emclient.o $(edit_obj)em_stat.o $(edit_obj)emstring.o $(edit_obj)stub_rtl.o

$(edit_exe)bemacs : $(edit_obj)bemacs
	cp $(edit_obj)bemacs $(edit_exe)bemacs
	strip $(edit_exe)bemacs

$(edit_obj)emclient.o : $(edit_src)emclient.cpp
	@ echo Make: $@
	@ $(cpp) $(cc_flags) -o $(edit_obj)emclient.o $(edit_src)emclient.cpp

$(edit_exe)mll-2-db : $(edit_obj)mll-2-db.o $(db_obj_files)
	@ echo Make: $@
	@ $(cpp) -o $(edit_obj)mll-2-db -g1 $(edit_obj)mll-2-db.o $(db_obj_files)
	cp $(edit_obj)mll-2-db $(edit_exe)mll-2-db
	strip $(edit_exe)mll-2-db

$(edit_obj)mll-2-db.o : $(edit_src)mll-2-db.cpp
	@ echo Make: $@
	@ $(cpp) $(cc_flags) -o $(edit_obj)mll-2-db.o $(edit_src)mll-2-db.cpp

$(edit_exe)dbcreate : $(edit_obj)dbcreate.o $(db_obj_files)
	@ echo Make: $@
	@ $(cpp) -o $(edit_obj)dbcreate -g1 $(edit_obj)dbcreate.o $(db_obj_files)
	cp $(edit_obj)dbcreate $(edit_exe)dbcreate
	strip $(edit_exe)dbcreate

$(edit_obj)dbcreate.o : $(edit_src)dbcreate.cpp
	@ echo Make: $@
	@ $(cpp) $(cc_flags) -o $(edit_obj)dbcreate.o $(edit_src)dbcreate.cpp

$(edit_obj)db_rtl.o : $(edit_src)db_rtl.cpp
	@ echo Make: $@
	@ $(cpp) $(cc_flags) -o $(edit_obj)db_rtl.o $(edit_src)db_rtl.cpp

$(edit_obj)stub_rtl.o : $(edit_src)stub_rtl.cpp
	@ echo Compile $(edit_src)stub_rtl.cpp
	@ $(cpp) $(cc_flags) -o $(edit_obj)stub_rtl.o $(edit_src)stub_rtl.cpp

$(edit_exe)dblist : $(edit_obj)dblist.o $(db_obj_files)
	@ echo Make: $@
	@ $(cpp) -o $(edit_obj)dblist -g1 $(edit_obj)dblist.o $(db_obj_files)
	cp $(edit_obj)dblist $(edit_exe)dblist
	strip $(edit_exe)dblist

$(edit_obj)dblist.o : $(edit_src)dblist.cpp
	@ echo Make: $@
	@ $(cpp) $(cc_flags) -o $(edit_obj)dblist.o $(edit_src)dblist.cpp

$(edit_exe)dbprint : $(edit_obj)dbprint.o $(db_obj_files)
	@ echo Make: $@
	@ $(cpp) -o $(edit_obj)dbprint -g1 $(edit_obj)dbprint.o $(db_obj_files)
	cp $(edit_obj)dbprint $(edit_exe)dbprint
	strip $(edit_exe)dbprint

$(edit_obj)dbprint.o : $(edit_src)dbprint.cpp
	@ echo Make: $@
	@ $(cpp) $(cc_flags) -o $(edit_obj)dbprint.o $(edit_src)dbprint.cpp

$(edit_exe)dbdel : $(edit_obj)dbdel.o $(db_obj_files)
	@ echo Make: $@
	@ $(cpp) -o $(edit_obj)dbdel -g1 $(edit_obj)dbdel.o $(db_obj_files)
	cp $(edit_obj)dbdel $(edit_exe)dbdel
	strip $(edit_exe)dbdel

$(edit_obj)dbdel.o : $(edit_src)dbdel.cpp
	@ echo Make: $@
	@ $(cpp) $(cc_flags) -o $(edit_obj)dbdel.o $(edit_src)dbdel.cpp

$(edit_obj)bemacs_server : $(obj_files) $(edit_obj)ptyopen_svr4.o
	@ echo Make: $@
	@ $(lddynamic) -o $(edit_obj)bemacs_server $(obj_files) $(edit_obj)ptyopen_svr4.o \
		-lXm -lXt -lX11 -lXext

#	$(ldstatic) -o $(edit_obj)emacs_static $(obj_files) -lXm -lXt -lXext -lX11

$(edit_obj)bemacs_server_9 : $(obj_files) $(edit_obj)ptyopen_bsd.o
	@ echo Make: $@
	@ $(lddynamic) -o $(edit_obj)bemacs_server $(obj_files) $(edit_obj)ptyopen_bsd.o \
		-lXm -lXmu -lXt -lX11 -lXext

#	$(ldstatic) -o $(edit_obj)emacs_static $(obj_files) -lXm -lXt -lXext -lX11

$(edit_obj)bemacs_server.pure : $(obj_files) dld_stub.cpp
	@ echo Make: $@
	$(cpp) $(cc_flags) -o $(edit_obj)dld_stub.o dld_stub.cpp
	$(pure) $(ldstatic) -o $(edit_obj)bemacs_server.pure $(obj_files) $(edit_obj)dld_stub.o -lXm -lXt -lXext -lX11


$(edit_exe)bemacs_server : $(edit_obj)bemacs_server
	cp $(edit_obj)bemacs_server $(edit_exe)bemacs_server
	strip $(edit_exe)bemacs_server

$(edit_obj)unix_rtl.o : $(edit_src)unix_rtl.cpp
	@ echo Make: $@
	@ $(cpp) $(cc_flags) -o $(edit_obj)unix_rtl.o $(edit_src)unix_rtl.cpp

$(edit_obj)unix_ext_func.o : $(edit_src)unix_ext_func.cpp
	@ echo Make: $@
	@ $(cpp) $(cc_flags) -o $(edit_obj)unix_ext_func.o $(edit_src)unix_ext_func.cpp

$(edit_obj)emacs_motif.o : $(edit_src)emacs_motif.cpp $(edit_src)emacs_motif.h
	@ echo Make: $@
	@ $(cpp) $(cc_flags) -o $(edit_obj)emacs_motif.o $(edit_src)emacs_motif.cpp

$(edit_obj)motif_keys.o : $(edit_src)motif_keys.cpp $(edit_src)motif_keys.h
	@ echo Make: $@
	@ $(cpp) $(cc_flags) -o $(edit_obj)motif_keys.o $(edit_src)motif_keys.cpp

$(edit_obj)motiftrm.o : $(edit_src)motiftrm.cpp $(edit_src)emacs_motif.h
	@ echo Make: $@
	@ $(cpp) $(cc_flags) -o $(edit_obj)motiftrm.o $(edit_src)motiftrm.cpp

$(edit_obj)motiftk.o : $(edit_src)motiftk.cpp $(edit_src)emacs_motif.h
	@ echo Make: $@
	@ $(cpp) $(cc_flags) -o $(edit_obj)motiftk.o $(edit_src)motiftk.cpp

$(edit_obj)motifcmd.o : $(edit_src)motifcmd.cpp $(edit_src)emacs_motif.h
	@ echo Make: $@
	@ $(cpp) $(cc_flags) -o $(edit_obj)motifcmd.o $(edit_src)motifcmd.cpp

$(edit_obj)unix_trm.o : $(edit_src)unix_trm.cpp
	@ echo Make: $@
	@ $(cpp) $(cc_flags) -o $(edit_obj)unix_trm.o $(edit_src)unix_trm.cpp

$(edit_obj)abbrev.o : $(edit_src)abbrev.cpp
	@ echo Make: $@
	@ $(cpp) $(cc_flags) -o $(edit_obj)abbrev.o $(edit_src)abbrev.cpp

$(edit_obj)abspath.o : $(edit_src)abspath.cpp
	@ echo Make: $@
	@ $(cpp) $(cc_flags) -o $(edit_obj)abspath.o $(edit_src)abspath.cpp

$(edit_obj)arith.o : $(edit_src)arith.cpp
	@ echo Make: $@
	@ $(cpp) $(cc_flags) -o $(edit_obj)arith.o $(edit_src)arith.cpp

$(edit_obj)timer.o : $(edit_src)timer.cpp
	@ echo Make: $@
	@ $(cpp) $(cc_flags) -o $(edit_obj)timer.o $(edit_src)timer.cpp

$(edit_obj)em_time.o : $(edit_src)em_time.cpp
	@ echo Make: $@
	@ $(cpp) $(cc_flags) -o $(edit_obj)em_time.o $(edit_src)em_time.cpp

$(edit_obj)em_stat.o : $(edit_src)em_stat.cpp
	@ echo Make: $@
	@ $(cpp) $(cc_flags) -o $(edit_obj)em_stat.o $(edit_src)em_stat.cpp

$(edit_obj)buffer.o : $(edit_src)buffer.cpp
	@ echo Make: $@
	@ $(cpp) $(cc_flags) -o $(edit_obj)buffer.o $(edit_src)buffer.cpp

$(edit_obj)buf_man.o : $(edit_src)buf_man.cpp
	@ echo Make: $@
	@ $(cpp) $(cc_flags) -o $(edit_obj)buf_man.o $(edit_src)buf_man.cpp

$(edit_obj)caseconv.o : $(edit_src)caseconv.cpp
	@ echo Make: $@
	@ $(cpp) $(cc_flags) -o $(edit_obj)caseconv.o $(edit_src)caseconv.cpp

$(edit_obj)columns.o : $(edit_src)columns.cpp
	@ echo Make: $@
	@ $(cpp) $(cc_flags) -o $(edit_obj)columns.o $(edit_src)columns.cpp

$(edit_obj)commandline.o : $(edit_src)commandline.cpp
	@ echo Make: $@
	@ $(cpp) $(cc_flags) -o $(edit_obj)commandline.o $(edit_src)commandline.cpp

$(edit_obj)dbman.o : $(edit_src)dbman.cpp
	@ echo Make: $@
	@ $(cpp) $(cc_flags) -o $(edit_obj)dbman.o $(edit_src)dbman.cpp

$(edit_obj)display.o : $(edit_src)display.cpp
	@ echo Make: $@
	@ $(cpp) $(cc_flags) -o $(edit_obj)display.o $(edit_src)display.cpp

$(edit_obj)doprint.o : $(edit_src)doprint.cpp
	@ echo Make: $@
	@ $(cpp) $(cc_flags) -o $(edit_obj)doprint.o $(edit_src)doprint.cpp

$(edit_obj)emacs.o : $(edit_src)emacs.cpp
	@ echo Make: $@
	@ $(cpp) $(cc_flags) -o $(edit_obj)emacs.o $(edit_src)emacs.cpp

$(edit_obj)emacsrtl.o : $(edit_src)emacsrtl.cpp
	@ echo Make: $@
	@ $(cpp) $(cc_flags) -o $(edit_obj)emacsrtl.o $(edit_src)emacsrtl.cpp

$(edit_obj)emarray.o : $(edit_src)emarray.cpp
	@ echo Make: $@
	@ $(cpp) $(cc_flags) -o $(edit_obj)emarray.o $(edit_src)emarray.cpp

$(edit_obj)emstring.o : $(edit_src)emstring.cpp
	@ echo Make: $@
	@ $(cpp) $(cc_flags) -o $(edit_obj)emstring.o $(edit_src)emstring.cpp

$(edit_obj)emstrtab.o : $(edit_src)emstrtab.cpp
	@ echo Make: $@
	@ $(cpp) $(cc_flags) -o $(edit_obj)emstrtab.o $(edit_src)emstrtab.cpp

$(edit_obj)errlog.o : $(edit_src)errlog.cpp
	@ echo Make: $@
	@ $(cpp) $(cc_flags) -o $(edit_obj)errlog.o $(edit_src)errlog.cpp

$(edit_obj)ext_func.o : $(edit_src)ext_func.cpp
	@ echo Make: $@
	@ $(cpp) $(cc_flags) -o $(edit_obj)ext_func.o $(edit_src)ext_func.cpp

$(edit_obj)fileio.o : $(edit_src)fileio.cpp
	@ echo Make: $@
	@ $(cpp) $(cc_flags) -o $(edit_obj)fileio.o $(edit_src)fileio.cpp

$(edit_obj)fio.o : $(edit_src)fio.cpp
	@ echo Make: $@
	@ $(cpp) $(cc_flags) -o $(edit_obj)fio.o $(edit_src)fio.cpp

$(edit_obj)function.o : $(edit_src)function.cpp
	@ echo Make: $@
	@ $(cpp) $(cc_flags) -o $(edit_obj)function.o $(edit_src)function.cpp

$(edit_obj)getdb.o : $(edit_src)getdb.cpp
	@ echo Make: $@
	@ $(cpp) $(cc_flags) -o $(edit_obj)getdb.o $(edit_src)getdb.cpp

$(edit_obj)getfile.o : $(edit_src)getfile.cpp
	@ echo Make: $@
	@ $(cpp) $(cc_flags) -o $(edit_obj)getfile.o $(edit_src)getfile.cpp

$(edit_obj)getdirectory.o : $(edit_src)getdirectory.cpp
	@ echo Make: $@
	@ $(cpp) $(cc_flags) -o $(edit_obj)getdirectory.o $(edit_src)getdirectory.cpp

$(edit_obj)glob_var.o : $(edit_src)glob_var.cpp
	@ echo Make: $@
	@ $(cpp) $(cc_flags) -o $(edit_obj)glob_var.o $(edit_src)glob_var.cpp

$(edit_obj)gui_input_mode.o : $(edit_src)gui_input_mode.cpp
	@ echo Make: $@
	@ $(cpp) $(cc_flags) -o $(edit_obj)gui_input_mode.o $(edit_src)gui_input_mode.cpp

$(edit_obj)journal.o : $(edit_src)journal.cpp
	@ echo Make: $@
	@ $(cpp) $(cc_flags) -o $(edit_obj)journal.o $(edit_src)journal.cpp

$(edit_obj)keyboard.o : $(edit_src)keyboard.cpp
	@ echo Make: $@
	@ $(cpp) $(cc_flags) -o $(edit_obj)keyboard.o $(edit_src)keyboard.cpp

$(edit_obj)key_names.o : $(edit_src)key_names.cpp
	@ echo Make: $@
	@ $(cpp) $(cc_flags) -o $(edit_obj)key_names.o $(edit_src)key_names.cpp

$(edit_obj)lispfunc.o : $(edit_src)lispfunc.cpp
	@ echo Make: $@
	@ $(cpp) $(cc_flags) -o $(edit_obj)lispfunc.o $(edit_src)lispfunc.cpp

$(edit_obj)macros.o : $(edit_src)macros.cpp
	@ echo Make: $@
	@ $(cpp) $(cc_flags) -o $(edit_obj)macros.o $(edit_src)macros.cpp

$(edit_obj)mem_man.o : $(edit_src)mem_man.cpp $(edit_src)mem_man.h
	@ echo Make: $@
	@ $(cpp) $(cc_flags) -o $(edit_obj)mem_man.o $(edit_src)mem_man.cpp

$(edit_obj)emacs_init.o : $(edit_src)emacs_init.cpp
	@ echo Make: $@
	@ $(cpp) $(cc_flags) -o $(edit_obj)emacs_init.o $(edit_src)emacs_init.cpp

$(edit_obj)metacomm.o : $(edit_src)metacomm.cpp
	@ echo Make: $@
	@ $(cpp) $(cc_flags) -o $(edit_obj)metacomm.o $(edit_src)metacomm.cpp

$(edit_obj)minibuf.o : $(edit_src)minibuf.cpp
	@ echo Make: $@
	@ $(cpp) $(cc_flags) -o $(edit_obj)minibuf.o $(edit_src)minibuf.cpp

$(edit_obj)mlispars.o : $(edit_src)mlispars.cpp
	@ echo Make: $@
	@ $(cpp) $(cc_flags) -o $(edit_obj)mlispars.o $(edit_src)mlispars.cpp

$(edit_obj)mlispexp.o : $(edit_src)mlispexp.cpp
	@ echo Make: $@
	@ $(cpp) $(cc_flags) -o $(edit_obj)mlispexp.o $(edit_src)mlispexp.cpp

$(edit_obj)mlisproc.o : $(edit_src)mlisproc.cpp
	@ echo Make: $@
	@ $(cpp) $(cc_flags) -o $(edit_obj)mlisproc.o $(edit_src)mlisproc.cpp

$(edit_obj)mlprintf.o : $(edit_src)mlprintf.cpp
	@ echo Make: $@
	@ $(cpp) $(cc_flags) -o $(edit_obj)mlprintf.o $(edit_src)mlprintf.cpp

$(edit_obj)ndbm.o : $(edit_src)ndbm.cpp
	@ echo Make: $@
	@ $(cpp) $(cc_flags) -o $(edit_obj)ndbm.o $(edit_src)ndbm.cpp

$(edit_obj)options.o : $(edit_src)options.cpp
	@ echo Make: $@
	@ $(cpp) $(cc_flags) -o $(edit_obj)options.o $(edit_src)options.cpp

$(edit_obj)osfile.o : $(edit_src)osfile.cpp
	@ echo Make: $@
	@ $(cpp) $(cc_flags) -o $(edit_obj)osfile.o $(edit_src)osfile.cpp

$(edit_obj)osinter.o : $(edit_src)osinter.cpp
	@ echo Make: $@
	@ $(cpp) $(cc_flags) -o $(edit_obj)osinter.o $(edit_src)osinter.cpp

$(edit_obj)profile.o : $(edit_src)profile.cpp
	@ echo Make: $@
	@ $(cpp) $(cc_flags) -o $(edit_obj)profile.o $(edit_src)profile.cpp

$(edit_obj)process.o : $(edit_src)process.cpp
	@ echo Make: $@
	@ $(cpp) $(cc_flags) -o $(edit_obj)process.o $(edit_src)process.cpp

$(edit_obj)ptyopen_svr4.o : $(edit_src)ptyopen_svr4.cpp
	@ echo Make: $@
	@ $(cpp) $(cc_flags) -o $(edit_obj)ptyopen_svr4.o $(edit_src)ptyopen_svr4.cpp

$(edit_obj)ptyopen_bsd.o : $(edit_src)ptyopen_bsd.cpp
	@ echo Make: $@
	@ $(cpp) $(cc_flags) -o $(edit_obj)ptyopen_bsd.o $(edit_src)ptyopen_bsd.cpp

$(edit_obj)queue.o : $(edit_src)queue.cpp
	@ echo Make: $@
	@ $(cpp) $(cc_flags) -o $(edit_obj)queue.o $(edit_src)queue.cpp

$(edit_obj)save_env.o : $(edit_src)save_env.cpp
	@ echo Make: $@
	@ $(cpp) $(cc_flags) -o $(edit_obj)save_env.o $(edit_src)save_env.cpp

$(edit_obj)search.o : $(edit_src)search.cpp
	@ echo Make: $@
	@ $(cpp) $(cc_flags) -o $(edit_obj)search.o $(edit_src)search.cpp

$(edit_obj)searchin.o : $(edit_src)searchin.cpp
	@ echo Make: $@
	@ $(cpp) $(cc_flags) -o $(edit_obj)searchin.o $(edit_src)searchin.cpp

$(edit_obj)simpcomm.o : $(edit_src)simpcomm.cpp
	@ echo Make: $@
	@ $(cpp) $(cc_flags) -o $(edit_obj)simpcomm.o $(edit_src)simpcomm.cpp

$(edit_obj)subproc.o : $(edit_src)subproc.cpp
	@ echo Make: $@
	@ $(cpp) $(cc_flags) -o $(edit_obj)subproc.o $(edit_src)subproc.cpp

$(edit_obj)syntax.o : $(edit_src)syntax.cpp
	@ echo Make: $@
	@ $(cpp) $(cc_flags) -o $(edit_obj)syntax.o $(edit_src)syntax.cpp

$(edit_obj)term.o : $(edit_src)term.cpp $(edit_src)tt.h
	@ echo Make: $@
	@ $(cpp) $(cc_flags) -o $(edit_obj)term.o $(edit_src)term.cpp

$(edit_obj)trm_ansi.o : $(edit_src)trm_ansi.cpp $(edit_src)tt.h
	@ echo Make: $@
	@ $(cpp) $(cc_flags) -o $(edit_obj)trm_ansi.o $(edit_src)trm_ansi.cpp

$(edit_obj)undo.o : $(edit_src)undo.cpp
	@ echo Make: $@
	@ $(cpp) $(cc_flags) -o $(edit_obj)undo.o $(edit_src)undo.cpp

$(edit_obj)variable.o : $(edit_src)variable.cpp
	@ echo Make: $@
	@ $(cpp) $(cc_flags) -o $(edit_obj)variable.o $(edit_src)variable.cpp

$(edit_obj)varthunk.o : $(edit_src)varthunk.cpp
	@ echo Make: $@
	@ $(cpp) $(cc_flags) -o $(edit_obj)varthunk.o $(edit_src)varthunk.cpp

$(edit_obj)unixcomm.o : $(edit_src)unixcomm.cpp
	@ echo Make: $@
	@ $(cpp) $(cc_flags) -o $(edit_obj)unixcomm.o $(edit_src)unixcomm.cpp

$(edit_obj)emacs_signal.o : $(edit_src)emacs_signal.cpp
	@ echo Make: $@
	@ $(cpp) $(cc_flags) -o $(edit_obj)emacs_signal.o $(edit_src)emacs_signal.cpp

$(edit_obj)unixfile.o : $(edit_src)unixfile.cpp
	@ echo Make: $@
	@ $(cpp) $(cc_flags) -o $(edit_obj)unixfile.o $(edit_src)unixfile.cpp

$(edit_obj)windman.o : $(edit_src)windman.cpp
	@ echo Make: $@
	@ $(cpp) $(cc_flags) -o $(edit_obj)windman.o $(edit_src)windman.cpp

$(edit_obj)window.o : $(edit_src)window.cpp
	@ echo Make: $@
	@ $(cpp) $(cc_flags) -o $(edit_obj)window.o $(edit_src)window.cpp

clean:
	rm -f $(edit_obj)*.o
	rm -rf $(edit_obj)/ptrepository
	rm -f $(edit_obj)bemacs_server $(edit_exe)bemacs_server $(edit_obj)bemacs $(edit_exe)bemacs
	rm -f $(edit_obj)mll-2-db $(edit_obj)dbdel $(edit_obj)dbprint
	rm -f $(edit_obj)dbadd $(edit_obj)dbcreate $(edit_obj)dblist
	rm -f $(edit_exe)mll-2-db $(edit_exe)dbdel $(edit_exe)dbprint
	rm -f $(edit_exe)dbadd $(edit_exe)dbcreate $(edit_exe)dblist

depend:
	echo "# DO NOT DELETE" > $(edit_obj).depend
	makedepend -D__unix__ -f $(edit_obj).depend -s "# DO NOT DELETE" \
abbrev.cpp \
abspath.cpp \
arith.cpp \
buffer.cpp \
caseconv.cpp \
columns.cpp \
commandline.cpp \
db_rtl.cpp \
stub_rtl.cpp \
dbadd.cpp \
dbcreate.cpp \
dbdel.cpp \
dblist.cpp \
dbman.cpp \
dbprint.cpp \
display.cpp \
doprint.cpp \
dumpjnl.cpp \
em_time.cpp \
em_stat.cpp \
emacs.cpp \
emacsrtl.cpp \
emarray.cpp \
emstring.cpp \
emstrtab.cpp \
errlog.cpp \
ext_func.cpp \
fileio.cpp \
fio.cpp \
function.cpp \
getdb.cpp \
getfile.cpp \
glob_var.cpp \
journal.cpp \
keyboard.cpp \
key_names.cpp \
lispfunc.cpp \
macros.cpp \
mem_man.cpp \
metacomm.cpp \
minibuf.cpp \
mlispars.cpp \
mlispexp.cpp \
mlisproc.cpp \
mll-2-db.cpp \
mlprintf.cpp \
motifcmd.cpp \
motiftk.cpp \
motiftrm.cpp \
ndbm.cpp \
options.cpp \
process.cpp \
ptyopen_svr4.cpp \
queue.cpp \
save_env.cpp \
search.cpp \
searchin.cpp \
simpcomm.cpp \
subproc.cpp \
syntax.cpp \
term.cpp \
timer.cpp \
trm_ansi.cpp \
undo.cpp \
unix_rtl.cpp \
unix_trm.cpp \
unixcomm.cpp \
emacs_signal.cpp \
unixfile.cpp \
variable.cpp \
varthunk.cpp \
windman.cpp \
window.cpp
 
include $(edit_obj).depend
