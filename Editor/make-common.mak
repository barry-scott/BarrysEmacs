#
#	make_common.mak common unix make file
#


obj_files = $(os_specific_obj_files) \
 $(edit_obj)abbrev.o \
 $(edit_obj)abspath.o \
 $(edit_obj)arith.o \
 $(edit_obj)buf_man.o \
 $(edit_obj)buffer.o \
 $(edit_obj)caseconv.o \
 $(edit_obj)columns.o \
 $(edit_obj)CommandLine.o \
 $(edit_obj)dbman.o \
 $(edit_obj)display.o \
 $(edit_obj)display_insert_delete.o \
 $(edit_obj)display_line.o \
 $(edit_obj)doprint.o \
 $(edit_obj)em_stat.o \
 $(edit_obj)em_time.o \
 $(edit_obj)emacs.o \
 $(edit_obj)emacs_init.o \
 $(edit_obj)emacs_motif.o \
 $(edit_obj)emacs_proc.o \
 $(edit_obj)emacs_signal.o \
 $(edit_obj)emacsrtl.o \
 $(edit_obj)emarray.o \
 $(edit_obj)emstring.o \
 $(edit_obj)emstrtab.o \
 $(edit_obj)errlog.o \
 $(edit_obj)ext_func.o \
 $(edit_obj)file_name_compare.o \
 $(edit_obj)fileio.o \
 $(edit_obj)fio.o \
 $(edit_obj)function.o \
 $(edit_obj)getdb.o \
 $(edit_obj)getdirectory.o \
 $(edit_obj)getfile.o \
 $(edit_obj)glob_var.o \
 $(edit_obj)gui_input_mode.o \
 $(edit_obj)journal.o \
 $(edit_obj)key_names.o \
 $(edit_obj)keyboard.o \
 $(edit_obj)lispfunc.o \
 $(edit_obj)macros.o \
 $(edit_obj)mem_man.o \
 $(edit_obj)metacomm.o \
 $(edit_obj)minibuf.o \
 $(edit_obj)mlispars.o \
 $(edit_obj)mlispexp.o \
 $(edit_obj)mlisproc.o \
 $(edit_obj)mlprintf.o \
 $(edit_obj)motif_keys.o \
 $(edit_obj)motifcmd.o \
 $(edit_obj)motiftk.o \
 $(edit_obj)motiftrm.o \
 $(edit_obj)ndbm.o \
 $(edit_obj)options.o \
 $(edit_obj)queue.o \
 $(edit_obj)save_env.o \
 $(edit_obj)search.o \
 $(edit_obj)search_extended_algorithm.o \
 $(edit_obj)search_extended_parser.o \
 $(edit_obj)search_interface.o \
 $(edit_obj)search_simple_algorithm.o \
 $(edit_obj)search_simple_engine.o \
 $(edit_obj)simpcomm.o \
 $(edit_obj)string_map.o \
 $(edit_obj)subproc.o \
 $(edit_obj)syntax.o \
 $(edit_obj)term.o \
 $(edit_obj)timer.o \
 $(edit_obj)trm_ansi.o \
 $(edit_obj)undo.o \
 $(edit_obj)unix_ext_func.o \
 $(edit_obj)unix_rtl.o \
 $(edit_obj)unix_trm.o \
 $(edit_obj)unixcomm.o \
 $(edit_obj)unixfile.o \
 $(edit_obj)variable.o \
 $(edit_obj)varthunk.o \
 $(edit_obj)windman.o \
 $(edit_obj)window.o \


##################################################################################
#                                                                                #
#                                                                                #
#          Main targets                                                          #
#                                                                                #
#                                                                                #
##################################################################################

static: $(edit_obj)bemacs_server_static $(edit_exe)bemacs_server_static bintools

dynamic: $(edit_obj)bemacs_server_dynamic $(edit_exe)bemacs_server_dynamic bintools

bintools: $(edit_exe)bemacs $(edit_exe)mll-2-db $(edit_exe)dbadd \
	$(edit_exe)dbcreate $(edit_exe)dblist $(edit_exe)dbdel $(edit_exe)dbprint

##################################################################################
#                                                                                #
#                                                                                #
#          Install                                                               #
#                                                                                #
#                                                                                #
##################################################################################

install_static_images: static
	for file in bemacs mll-2-db dbadd dbcreate dblist dbdel dbprint; \
	do \
		cp -f $(edit_exe)$$file $(BUILD_KIT_DIR); \
		chmod ugo=rx $(BUILD_KIT_DIR)/$$file; \
	done
	cp -f $(edit_exe)bemacs_server_static $(BUILD_KIT_DIR)/bemacs_server
	chmod ugo=rx $(BUILD_KIT_DIR)/bemacs_server

install_dynamic_images: dynamic
	for file in bemacs mll-2-db dbadd dbcreate dblist dbdel dbprint; \
	do \
		cp -f $(edit_exe)$$file $(BUILD_KIT_DIR); \
		chmod ugo=rx $(BUILD_KIT_DIR)/$$file; \
	done
	cp -f $(edit_exe)bemacs_server_dynamic $(BUILD_KIT_DIR)/bemacs_server
	chmod ugo=rx $(BUILD_KIT_DIR)/bemacs_server

install_bitmaps:
	cp -f bitmaps/*.xpm $(BUILD_KIT_DIR)
	chmod ugo=r $(BUILD_KIT_DIR)/*.xpm

install_xkeys:
	cp -f X11_keymaps/*.xkeys $(BUILD_KIT_DIR)
	chmod ugo=r $(BUILD_KIT_DIR)/*.xkeys

##################################################################################
#                                                                                #
#                                                                                #
#          bintools                                                              #
#                                                                                #
#                                                                                #
##################################################################################

db_obj_files= \
 $(edit_obj)db_rtl.o \
 $(edit_obj)doprint.o \
 $(edit_obj)em_stat.o \
 $(edit_obj)emstring.o \
 $(edit_obj)file_name_compare.o \
 $(edit_obj)ndbm.o \
 $(edit_obj)stub_rtl.o \
 $(edit_obj)unixfile.o \

$(edit_exe)dbadd : $(edit_obj)dbadd.o $(db_obj_files)
	@ echo Info: Link $(edit_exe)dbadd
	$(ld_bintools) -o $(edit_obj)dbadd -g $(edit_obj)dbadd.o $(db_obj_files)
	cp $(edit_obj)dbadd $(edit_exe)dbadd
	strip $(edit_exe)dbadd

$(edit_obj)dbadd.o : dbadd/dbadd.cpp
	@ echo Info: Compile dbadd/dbadd.cpp
	@ $(cpp) $(cc_flags) -o $(edit_obj)dbadd.o dbadd/dbadd.cpp

$(edit_exe)bemacs : $(edit_obj)emclient.o $(edit_obj)em_stat.o $(edit_obj)emstring.o $(edit_obj)stub_rtl.o
	@ echo Info: Link $(edit_exe)bemacs
	$(ld_bintools) -o $(edit_obj)bemacs -g $(edit_obj)emclient.o $(edit_obj)em_stat.o $(edit_obj)emstring.o $(edit_obj)stub_rtl.o $(edit_obj)doprint.o
	cp $(edit_obj)bemacs $(edit_exe)bemacs
	strip $(edit_exe)bemacs

$(edit_obj)emclient.o : Source/Unix/emclient.cpp
	@ echo Info: Compile Source/Unix/emclient.cpp
	@ $(cpp) $(cc_flags) -o $(edit_obj)emclient.o Source/Unix/emclient.cpp

$(edit_exe)mll-2-db : $(edit_obj)mll-2-db.o $(db_obj_files)
	@ echo Info: Link $(edit_exe)mll-2-db
	$(ld_bintools) -o $(edit_obj)mll-2-db -g $(edit_obj)mll-2-db.o $(db_obj_files)
	cp $(edit_obj)mll-2-db $(edit_exe)mll-2-db
	strip $(edit_exe)mll-2-db

$(edit_obj)mll-2-db.o : mll2db/mll-2-db.cpp
	@ echo Info: Compile mll2db/mll-2-db.cpp
	@ $(cpp) $(cc_flags) -o $(edit_obj)mll-2-db.o mll2db/mll-2-db.cpp

$(edit_exe)dbcreate : $(edit_obj)dbcreate.o $(db_obj_files)
	@ echo Info: Link $(edit_exe)dbcreate
	$(ld_bintools) -o $(edit_obj)dbcreate -g $(edit_obj)dbcreate.o $(db_obj_files)
	cp $(edit_obj)dbcreate $(edit_exe)dbcreate
	strip $(edit_exe)dbcreate

$(edit_obj)dbcreate.o : dbcreate/dbcreate.cpp
	@ echo Info: Compile dbcreate/dbcreate.cpp
	@ $(cpp) $(cc_flags) -o $(edit_obj)dbcreate.o dbcreate/dbcreate.cpp

$(edit_obj)db_rtl.o : db_rtl/db_rtl.cpp
	@ echo Info: Compile db_rtl/db_rtl.cpp
	@ $(cpp) $(cc_flags) -o $(edit_obj)db_rtl.o db_rtl/db_rtl.cpp

$(edit_obj)stub_rtl.o : db_rtl/stub_rtl.cpp
	@ echo Info: Compile db_rtl/stub_rtl.cpp
	@ $(cpp) $(cc_flags) -o $(edit_obj)stub_rtl.o db_rtl/stub_rtl.cpp

$(edit_exe)dblist : $(edit_obj)dblist.o $(db_obj_files)
	@ echo Info: Link $(edit_exe)dblist
	$(ld_bintools) -o $(edit_obj)dblist -g $(edit_obj)dblist.o $(db_obj_files)
	cp $(edit_obj)dblist $(edit_exe)dblist
	strip $(edit_exe)dblist

$(edit_obj)dblist.o : dblist/dblist.cpp
	@ echo Info: Compile dblist/dblist.cpp
	@ $(cpp) $(cc_flags) -o $(edit_obj)dblist.o dblist/dblist.cpp

$(edit_exe)dbprint : $(edit_obj)dbprint.o $(db_obj_files)
	@ echo Info: Link $(edit_exe)dbprint
	$(ld_bintools) -o $(edit_obj)dbprint -g $(edit_obj)dbprint.o $(db_obj_files)
	cp $(edit_obj)dbprint $(edit_exe)dbprint
	strip $(edit_exe)dbprint

$(edit_obj)dbprint.o : dbprint/dbprint.cpp
	@ echo Info: Compile dbprint/dbprint.cpp
	@ $(cpp) $(cc_flags) -o $(edit_obj)dbprint.o dbprint/dbprint.cpp

$(edit_exe)dbdel : $(edit_obj)dbdel.o $(db_obj_files)
	@ echo Info: Link $(edit_exe)dbdel
	$(ld_bintools) -o $(edit_obj)dbdel -g $(edit_obj)dbdel.o $(db_obj_files)
	cp $(edit_obj)dbdel $(edit_exe)dbdel
	strip $(edit_exe)dbdel

$(edit_obj)dbdel.o : dbdel/dbdel.cpp
	@ echo Info: Compile dbdel/dbdel.cpp
	@ $(cpp) $(cc_flags) -o $(edit_obj)dbdel.o dbdel/dbdel.cpp

##################################################################################
#                                                                                #
#                                                                                #
#          bemacs_server                                                         #
#                                                                                #
#                                                                                #
##################################################################################
$(edit_obj)bemacs_server_dynamic : $(obj_files)
	@ echo Info: Link $(edit_obj)bemacs_server_dynamic
	$(lddynamic) -o $(edit_obj)bemacs_server_dynamic $(obj_files) -lXm -lXpm -lXp -lXt -lXmu -lSM -lICE -lXext -lX11

$(edit_obj)linux_ld_fix.o : Source/Unix/linux_ld_fix.cpp
	@ echo Info: Compile Source/Unix/linux_ld_fix.cpp
	@ $(cpp) $(cc_flags) -o $(edit_obj)linux_ld_fix.o Source/Unix/linux_ld_fix.cpp

$(edit_exe)bemacs_server_dynamic : $(edit_obj)bemacs_server_dynamic
	@ echo Info: Build $(edit_exe)bemacs_server_dynamic
	cp $(edit_obj)bemacs_server_dynamic $(edit_exe)bemacs_server_dynamic
	strip $(edit_exe)bemacs_server_dynamic

$(edit_obj)bemacs_server_static : $(obj_files)
	@ echo Info: Link $(edit_obj)bemacs_server_static
	$(ldstatic) -Wl,-Bstatic -o $(edit_obj)bemacs_server_static $(obj_files) $(bemacs_libs) 2>&1 | $(cppfilt)

$(edit_exe)bemacs_server_static : $(edit_obj)bemacs_server_static
	cp $(edit_obj)bemacs_server_static $(edit_exe)bemacs_server_static
	strip $(edit_exe)bemacs_server_static

test: $(edit_obj)motif_test

motif_test_objs=\
	$(edit_obj)motif_test.o \
	$(edit_obj)emstring.o \
	$(edit_obj)doprint.o

$(edit_obj)motif_test : $(motif_test_objs)
	@ echo Info: Link $(edit_obj)motif_test
	$(lddynamic) -o $(edit_obj)motif_test $(motif_test_objs) -lXm -lXt -lSM -lICE -lXext -lX11 2>&1 |c++filt

$(edit_obj)unix_rtl.o : Source/Unix/unix_rtl.cpp
	@ echo Info: Compile Source/Unix/unix_rtl.cpp
	$(cpp) $(cc_flags) -o $(edit_obj)unix_rtl.o Source/Unix/unix_rtl.cpp

$(edit_obj)motif_test.o : Source/Unix/motif_test.cpp
	@ echo Info: Compile Source/Unix/motif_test.cpp
	@ $(cpp) $(cc_flags) -o $(edit_obj)motif_test.o Source/Unix/motif_test.cpp

$(edit_obj)motiftrm_test.o : Source/Unix/motiftrm.cpp
	@ echo Info: Compile Source/Unix/motiftrm.cpp
	@ $(cpp) $(cc_flags) -DMOTIF_TEST -o $(edit_obj)motiftrm_test.o Source/Unix/motiftrm.cpp

$(edit_obj)motiftrm.o : Source/Unix/motiftrm.cpp Include/Unix/emacs_motif.h
	@ echo Info: Compile Source/Unix/motiftrm.cpp
	@ $(cpp) $(cc_flags) -o $(edit_obj)motiftrm.o Source/Unix/motiftrm.cpp

$(edit_obj)motif_keys.o : Source/Unix/motif_keys.cpp Include/Unix/motif_keys.h
	@ echo Info: Compile Source/Unix/motif_keys.cpp
	@ $(cpp) $(cc_flags) -o $(edit_obj)motif_keys.o Source/Unix/motif_keys.cpp

$(edit_obj)emacs_motif.o : Source/Unix/emacs_motif.cpp Include/Unix/emacs_motif.h
	@ echo Info: Compile Source/Unix/emacs_motif.cpp
	@ $(cpp) $(cc_flags) -o $(edit_obj)emacs_motif.o Source/Unix/emacs_motif.cpp

$(edit_obj)motiftk.o : Source/Unix/motiftk.cpp
	@ echo Info: Compile Source/Unix/motiftk.cpp
	@ $(cpp) $(cc_flags) -o $(edit_obj)motiftk.o Source/Unix/motiftk.cpp

$(edit_obj)motifcmd.o : Source/Unix/motifcmd.cpp
	@ echo Info: Compile Source/Unix/motifcmd.cpp
	@ $(cpp) $(cc_flags) -o $(edit_obj)motifcmd.o Source/Unix/motifcmd.cpp

$(edit_obj)unix_trm.o : Source/Unix/unix_trm.cpp
	@ echo Info: Compile Source/Unix/unix_trm.cpp
	@ $(cpp) $(cc_flags) -o $(edit_obj)unix_trm.o Source/Unix/unix_trm.cpp

$(edit_obj)abbrev.o : Source/Common/abbrev.cpp
	@ echo Info: Compile Source/Common/abbrev.cpp
	@ $(cpp) $(cc_flags) -o $(edit_obj)abbrev.o Source/Common/abbrev.cpp

$(edit_obj)abspath.o : Source/Common/abspath.cpp
	@ echo Info: Compile Source/Common/abspath.cpp
	@ $(cpp) $(cc_flags) -o $(edit_obj)abspath.o Source/Common/abspath.cpp

$(edit_obj)arith.o : Source/Common/arith.cpp
	@ echo Info: Compile Source/Common/arith.cpp
	@ $(cpp) $(cc_flags) -o $(edit_obj)arith.o Source/Common/arith.cpp

$(edit_obj)timer.o : Source/Common/timer.cpp
	@ echo Info: Compile Source/Common/timer.cpp
	@ $(cpp) $(cc_flags) -o $(edit_obj)timer.o Source/Common/timer.cpp

$(edit_obj)em_time.o : Source/Common/em_time.cpp
	@ echo Info: Compile Source/Common/em_time.cpp
	@ $(cpp) $(cc_flags) -o $(edit_obj)em_time.o Source/Common/em_time.cpp

$(edit_obj)em_stat.o : Source/Common/em_stat.cpp
	@ echo Info: Compile Source/Common/em_stat.cpp
	@ $(cpp) $(cc_flags) -o $(edit_obj)em_stat.o Source/Common/em_stat.cpp

$(edit_obj)buffer.o : Source/Common/buffer.cpp
	@ echo Info: Compile Source/Common/buffer.cpp
	@ $(cpp) $(cc_flags) -o $(edit_obj)buffer.o Source/Common/buffer.cpp

$(edit_obj)buf_man.o : Source/Common/buf_man.cpp
	@ echo Info: Compile Source/Common/buf_man.cpp
	@ $(cpp) $(cc_flags) -o $(edit_obj)buf_man.o Source/Common/buf_man.cpp

$(edit_obj)caseconv.o : Source/Common/caseconv.cpp
	@ echo Info: Compile Source/Common/caseconv.cpp
	@ $(cpp) $(cc_flags) -o $(edit_obj)caseconv.o Source/Common/caseconv.cpp

$(edit_obj)columns.o : Source/Common/columns.cpp
	@ echo Info: Compile Source/Common/columns.cpp
	@ $(cpp) $(cc_flags) -o $(edit_obj)columns.o Source/Common/columns.cpp

$(edit_obj)CommandLine.o : Source/Common/CommandLine.cpp
	@ echo Info: Compile Source/Common/CommandLine.cpp
	@ $(cpp) $(cc_flags) -o $(edit_obj)CommandLine.o Source/Common/CommandLine.cpp

$(edit_obj)dbman.o : Source/Common/dbman.cpp
	@ echo Info: Compile Source/Common/dbman.cpp
	@ $(cpp) $(cc_flags) -o $(edit_obj)dbman.o Source/Common/dbman.cpp

$(edit_obj)display.o : Source/Common/display.cpp
	@ echo Info: Compile Source/Common/display.cpp
	@ $(cpp) $(cc_flags) -o $(edit_obj)display.o Source/Common/display.cpp

$(edit_obj)display_line.o : Source/Common/display_line.cpp
	@ echo Info: Compile Source/Common/display_line.cpp
	@ $(cpp) $(cc_flags) -o $(edit_obj)display_line.o Source/Common/display_line.cpp

$(edit_obj)display_insert_delete.o : Source/Common/display_insert_delete.cpp
	@ echo Info: Compile Source/Common/display_insert_delete.cpp
	@ $(cpp) $(cc_flags) -o $(edit_obj)display_insert_delete.o Source/Common/display_insert_delete.cpp

$(edit_obj)doprint.o : Source/Common/doprint.cpp
	@ echo Info: Compile Source/Common/doprint.cpp
	@ $(cpp) $(cc_flags) -o $(edit_obj)doprint.o Source/Common/doprint.cpp

$(edit_obj)emacs.o : Source/Common/emacs.cpp
	@ echo Info: Compile Source/Common/emacs.cpp
	@ $(cpp) $(cc_flags) -o $(edit_obj)emacs.o Source/Common/emacs.cpp

$(edit_obj)emacsrtl.o : Source/Common/emacsrtl.cpp
	@ echo Info: Compile Source/Common/emacsrtl.cpp
	@ $(cpp) $(cc_flags) -o $(edit_obj)emacsrtl.o Source/Common/emacsrtl.cpp

$(edit_obj)emacs_signal.o : Source/Unix/emacs_signal.cpp
	@ echo Info: Compile Source/Unix/emacs_signal.cpp
	@ $(cpp) $(cc_flags) -o $(edit_obj)emacs_signal.o Source/Unix/emacs_signal.cpp

$(edit_obj)emarray.o : Source/Common/emarray.cpp
	@ echo Info: Compile Source/Common/emarray.cpp
	@ $(cpp) $(cc_flags) -o $(edit_obj)emarray.o Source/Common/emarray.cpp

$(edit_obj)emstring.o : Source/Common/emstring.cpp
	@ echo Info: Compile Source/Common/emstring.cpp
	@ $(cpp) $(cc_flags) -o $(edit_obj)emstring.o Source/Common/emstring.cpp

$(edit_obj)emstrtab.o : Source/Common/emstrtab.cpp
	@ echo Info: Compile Source/Common/emstrtab.cpp
	@ $(cpp) $(cc_flags) -o $(edit_obj)emstrtab.o Source/Common/emstrtab.cpp

$(edit_obj)errlog.o : Source/Common/errlog.cpp
	@ echo Info: Compile Source/Common/errlog.cpp
	@ $(cpp) $(cc_flags) -o $(edit_obj)errlog.o Source/Common/errlog.cpp

$(edit_obj)ext_func.o : Source/Common/ext_func.cpp
	@ echo Info: Compile Source/Common/ext_func.cpp
	@ $(cpp) $(cc_flags) -o $(edit_obj)ext_func.o Source/Common/ext_func.cpp

$(edit_obj)file_name_compare.o : Source/Common/file_name_compare.cpp
	@ echo Info: Compile Source/Common/file_name_compare.cpp
	@ $(cpp) $(cc_flags) -o $(edit_obj)file_name_compare.o Source/Common/file_name_compare.cpp

$(edit_obj)fileio.o : Source/Common/fileio.cpp
	@ echo Info: Compile Source/Common/fileio.cpp
	@ $(cpp) $(cc_flags) -o $(edit_obj)fileio.o Source/Common/fileio.cpp

$(edit_obj)fio.o : Source/Common/fio.cpp
	@ echo Info: Compile Source/Common/fio.cpp
	@ $(cpp) $(cc_flags) -o $(edit_obj)fio.o Source/Common/fio.cpp

$(edit_obj)function.o : Source/Common/function.cpp
	@ echo Info: Compile Source/Common/function.cpp
	@ $(cpp) $(cc_flags) -o $(edit_obj)function.o Source/Common/function.cpp

$(edit_obj)getdb.o : Source/Common/getdb.cpp
	@ echo Info: Compile Source/Common/getdb.cpp
	@ $(cpp) $(cc_flags) -o $(edit_obj)getdb.o Source/Common/getdb.cpp

$(edit_obj)getfile.o : Source/Common/getfile.cpp
	@ echo Info: Compile Source/Common/getfile.cpp
	@ $(cpp) $(cc_flags) -o $(edit_obj)getfile.o Source/Common/getfile.cpp

$(edit_obj)getdirectory.o : Source/Common/getdirectory.cpp
	@ echo Info: Compile Source/Common/getdirectory.cpp
	@ $(cpp) $(cc_flags) -o $(edit_obj)getdirectory.o Source/Common/getdirectory.cpp

$(edit_obj)glob_var.o : Source/Common/glob_var.cpp
	@ echo Info: Compile Source/Common/glob_var.cpp
	@ $(cpp) $(cc_flags) -o $(edit_obj)glob_var.o Source/Common/glob_var.cpp

$(edit_obj)gui_input_mode.o : Source/Common/gui_input_mode.cpp
	@ echo Info: Compile Source/Common/gui_input_mode.cpp
	@ $(cpp) $(cc_flags) -o $(edit_obj)gui_input_mode.o Source/Common/gui_input_mode.cpp

$(edit_obj)journal.o : Source/Common/journal.cpp
	@ echo Info: Compile Source/Common/journal.cpp
	@ $(cpp) $(cc_flags) -o $(edit_obj)journal.o Source/Common/journal.cpp

$(edit_obj)keyboard.o : Source/Common/keyboard.cpp
	@ echo Info: Compile Source/Common/keyboard.cpp
	@ $(cpp) $(cc_flags) -o $(edit_obj)keyboard.o Source/Common/keyboard.cpp

$(edit_obj)key_names.o : Source/Common/key_names.cpp
	@ echo Info: Compile Source/Common/key_names.cpp
	@ $(cpp) $(cc_flags) -o $(edit_obj)key_names.o Source/Common/key_names.cpp

$(edit_obj)lispfunc.o : Source/Common/lispfunc.cpp
	@ echo Info: Compile Source/Common/lispfunc.cpp
	@ $(cpp) $(cc_flags) -o $(edit_obj)lispfunc.o Source/Common/lispfunc.cpp

$(edit_obj)macros.o : Source/Common/macros.cpp
	@ echo Info: Compile Source/Common/macros.cpp
	@ $(cpp) $(cc_flags) -o $(edit_obj)macros.o Source/Common/macros.cpp

$(edit_obj)mem_man.o : Source/Common/mem_man.cpp
	@ echo Info: Compile Source/Common/mem_man.cpp
	@ $(cpp) $(cc_flags) -o $(edit_obj)mem_man.o Source/Common/mem_man.cpp

$(edit_obj)emacs_init.o : Source/Common/emacs_init.cpp
	@ echo Info: Compile Source/Common/emacs_init.cpp
	@ $(cpp) $(cc_flags) -o $(edit_obj)emacs_init.o Source/Common/emacs_init.cpp

$(edit_obj)metacomm.o : Source/Common/metacomm.cpp
	@ echo Info: Compile Source/Common/metacomm.cpp
	@ $(cpp) $(cc_flags) -o $(edit_obj)metacomm.o Source/Common/metacomm.cpp

$(edit_obj)minibuf.o : Source/Common/minibuf.cpp
	@ echo Info: Compile Source/Common/minibuf.cpp
	@ $(cpp) $(cc_flags) -o $(edit_obj)minibuf.o Source/Common/minibuf.cpp

$(edit_obj)mlispars.o : Source/Common/mlispars.cpp
	@ echo Info: Compile Source/Common/mlispars.cpp
	@ $(cpp) $(cc_flags) -o $(edit_obj)mlispars.o Source/Common/mlispars.cpp

$(edit_obj)mlispexp.o : Source/Common/mlispexp.cpp
	@ echo Info: Compile Source/Common/mlispexp.cpp
	@ $(cpp) $(cc_flags) -o $(edit_obj)mlispexp.o Source/Common/mlispexp.cpp

$(edit_obj)mlisproc.o : Source/Common/mlisproc.cpp
	@ echo Info: Compile Source/Common/mlisproc.cpp
	@ $(cpp) $(cc_flags) -o $(edit_obj)mlisproc.o Source/Common/mlisproc.cpp

$(edit_obj)mlprintf.o : Source/Common/mlprintf.cpp
	@ echo Info: Compile Source/Common/mlprintf.cpp
	@ $(cpp) $(cc_flags) -o $(edit_obj)mlprintf.o Source/Common/mlprintf.cpp

$(edit_obj)ndbm.o : Source/Common/ndbm.cpp
	@ echo Info: Compile Source/Common/ndbm.cpp
	@ $(cpp) $(cc_flags) -o $(edit_obj)ndbm.o Source/Common/ndbm.cpp

$(edit_obj)options.o : Source/Common/options.cpp
	@ echo Info: Compile Source/Common/options.cpp
	@ $(cpp) $(cc_flags) -o $(edit_obj)options.o Source/Common/options.cpp

$(edit_obj)osfile.o : Source/Common/osfile.cpp
	@ echo Info: Compile Source/Common/osfile.cpp
	@ $(cpp) $(cc_flags) -o $(edit_obj)osfile.o Source/Common/osfile.cpp

$(edit_obj)osinter.o : Source/Common/osinter.cpp
	@ echo Info: Compile Source/Common/osinter.cpp
	@ $(cpp) $(cc_flags) -o $(edit_obj)osinter.o Source/Common/osinter.cpp

$(edit_obj)profile.o : Source/Common/profile.cpp
	@ echo Info: Compile Source/Common/profile.cpp
	@ $(cpp) $(cc_flags) -o $(edit_obj)profile.o Source/Common/profile.cpp

$(edit_obj)ptyopen_svr4.o : Source/Unix/ptyopen_svr4.cpp
	@ echo Info: Compile Source/Unix/ptyopen_svr4.cpp
	@ $(cpp) $(cc_flags) -o $(edit_obj)ptyopen_svr4.o Source/Unix/ptyopen_svr4.cpp

$(edit_obj)ptyopen_bsd.o : Source/Unix/ptyopen_bsd.cpp
	@ echo Info: Compile Source/Unix/ptyopen_bsd.cpp
	@ $(cpp) $(cc_flags) -o $(edit_obj)ptyopen_bsd.o Source/Unix/ptyopen_bsd.cpp

$(edit_obj)emacs_proc.o : Source/Common/emacs_proc.cpp
	@ echo Info: Compile Source/Common/emacs_proc.cpp
	@ $(cpp) $(cc_flags) -o $(edit_obj)emacs_proc.o Source/Common/emacs_proc.cpp

$(edit_obj)queue.o : Source/Common/queue.cpp
	@ echo Info: Compile Source/Common/queue.cpp
	@ $(cpp) $(cc_flags) -o $(edit_obj)queue.o Source/Common/queue.cpp

$(edit_obj)save_env.o : Source/Common/save_env.cpp
	@ echo Info: Compile Source/Common/save_env.cpp
	@ $(cpp) $(cc_flags) -o $(edit_obj)save_env.o Source/Common/save_env.cpp

$(edit_obj)search.o : Source/Common/search.cpp
	@ echo Info: Compile Source/Common/search.cpp
	@ $(cpp) $(cc_flags) -o $(edit_obj)search.o Source/Common/search.cpp

$(edit_obj)search_extended_algorithm.o : Source/Common/search_extended_algorithm.cpp
	@ echo Info: Compile Source/Common/search_extended_algorithm.cpp
	@ $(cpp) $(cc_flags) -o $(edit_obj)search_extended_algorithm.o Source/Common/search_extended_algorithm.cpp

$(edit_obj)search_extended_parser.o : Source/Common/search_extended_parser.cpp
	@ echo Info: Compile Source/Common/search_extended_parser.cpp
	@ $(cpp) $(cc_flags) -o $(edit_obj)search_extended_parser.o Source/Common/search_extended_parser.cpp

$(edit_obj)search_interface.o : Source/Common/search_interface.cpp
	@ echo Info: Compile Source/Common/search_interface.cpp
	@ $(cpp) $(cc_flags) -o $(edit_obj)search_interface.o Source/Common/search_interface.cpp

$(edit_obj)search_simple_algorithm.o : Source/Common/search_simple_algorithm.cpp
	@ echo Info: Compile Source/Common/search_simple_algorithm.cpp
	@ $(cpp) $(cc_flags) -o $(edit_obj)search_simple_algorithm.o Source/Common/search_simple_algorithm.cpp

$(edit_obj)search_simple_engine.o : Source/Common/search_simple_engine.cpp
	@ echo Info: Compile Source/Common/search_simple_engine.cpp
	@ $(cpp) $(cc_flags) -o $(edit_obj)search_simple_engine.o Source/Common/search_simple_engine.cpp

$(edit_obj)simpcomm.o : Source/Common/simpcomm.cpp
	@ echo Info: Compile Source/Common/simpcomm.cpp
	@ $(cpp) $(cc_flags) -o $(edit_obj)simpcomm.o Source/Common/simpcomm.cpp

$(edit_obj)subproc.o : Source/Common/subproc.cpp
	@ echo Info: Compile Source/Common/subproc.cpp
	@ $(cpp) $(cc_flags) -o $(edit_obj)subproc.o Source/Common/subproc.cpp

$(edit_obj)string_map.o : Source/Common/string_map.cpp
	@ echo Info: Compile Source/Common/string_map.cpp
	@ $(cpp) $(cc_flags) -o $(edit_obj)string_map.o Source/Common/string_map.cpp

$(edit_obj)syntax.o : Source/Common/syntax.cpp
	@ echo Info: Compile Source/Common/syntax.cpp
	@ $(cpp) $(cc_flags) -o $(edit_obj)syntax.o Source/Common/syntax.cpp

$(edit_obj)term.o : Source/Common/term.cpp Include/Common/tt.h
	@ echo Info: Compile Source/Common/term.cpp
	@ $(cpp) $(cc_flags) -o $(edit_obj)term.o Source/Common/term.cpp

$(edit_obj)trm_ansi.o : Source/Unix/trm_ansi.cpp Include/Common/tt.h
	@ echo Info: Compile Source/Unix/trm_ansi.cpp
	@ $(cpp) $(cc_flags) -o $(edit_obj)trm_ansi.o Source/Unix/trm_ansi.cpp

$(edit_obj)undo.o : Source/Common/undo.cpp
	@ echo Info: Compile Source/Common/undo.cpp
	@ $(cpp) $(cc_flags) -o $(edit_obj)undo.o Source/Common/undo.cpp

$(edit_obj)variable.o : Source/Common/variable.cpp
	@ echo Info: Compile Source/Common/variable.cpp
	@ $(cpp) $(cc_flags) -o $(edit_obj)variable.o Source/Common/variable.cpp

$(edit_obj)varthunk.o : Source/Common/varthunk.cpp
	@ echo Info: Compile Source/Common/varthunk.cpp
	@ $(cpp) $(cc_flags) -o $(edit_obj)varthunk.o Source/Common/varthunk.cpp

$(edit_obj)unixcomm.o : Source/Unix/unixcomm.cpp
	@ echo Info: Compile Source/Unix/unixcomm.cpp
	@ $(cpp) $(cc_flags) -o $(edit_obj)unixcomm.o Source/Unix/unixcomm.cpp

$(edit_obj)unixfile.o : Source/Unix/unixfile.cpp
	@ echo Info: Compile Source/Unix/unixfile.cpp
	@ $(cpp) $(cc_flags) -o $(edit_obj)unixfile.o Source/Unix/unixfile.cpp

$(edit_obj)unix_ext_func.o : Source/Unix/unix_ext_func.cpp
	@ echo Info: Compile Source/Unix/unix_ext_func.cpp
	@ $(cpp) $(cc_flags) -o $(edit_obj)unix_ext_func.o Source/Unix/unix_ext_func.cpp

$(edit_obj)windman.o : Source/Common/windman.cpp
	@ echo Info: Compile Source/Common/windman.cpp
	@ $(cpp) $(cc_flags) -o $(edit_obj)windman.o Source/Common/windman.cpp

$(edit_obj)window.o : Source/Common/window.cpp
	@ echo Info: Compile Source/Common/window.cpp
	@ $(cpp) $(cc_flags) -o $(edit_obj)window.o Source/Common/window.cpp

##################################################################################
#                                                                                #
#                                                                                #
#          Clean                                                                 #
#                                                                                #
#                                                                                #
##################################################################################
clean:
	rm -f $(edit_obj)*.o
	rm -f $(edit_obj)bemacs $(edit_exe)bemacs
	rm -f $(edit_exe)bemacs_server_dynamic $(edit_obj)bemacs_server_dynamic
	rm -f $(edit_exe)bemacs_server_static $(edit_obj)bemacs_server_static
	rm -f $(edit_exe)mll-2-db
	rm -f $(edit_exe)dbadd $(edit_exe)dbcreate $(edit_exe)dblist $(edit_exe)dbdel $(edit_exe)dbprint

##################################################################################
#                                                                                #
#                                                                                #
#          Depend                                                                #
#                                                                                #
#                                                                                #
##################################################################################
depend:
	mkdep -D__unix__ $(cc_flags) \
emacs_init.cpp \
abbrev.cpp \
abspath.cpp \
arith.cpp \
buffer.cpp \
caseconv.cpp \
columns.cpp \
CommandLine.cpp \
db_rtl.cpp \
dbadd.cpp \
dbcreate.cpp \
dbdel.cpp \
dblist.cpp \
dbman.cpp \
dbprint.cpp \
display.cpp \
doprint.cpp \
dumpjnl.cpp \
em_stat.cpp \
em_time.cpp \
emacs.cpp \
emacsrtl.cpp \
emacs_signal.cpp \
emarray.cpp \
emstring.cpp \
emstrtab.cpp \
errlog.cpp \
fileio.cpp \
fio.cpp \
function.cpp \
getdb.cpp \
getfile.cpp \
getdirectory.cpp \
glob_var.cpp \
gui_input_mode.cpp \
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
emacs_proc.cpp \
queue.cpp \
save_env.cpp \
search.cpp \
search_extended_algorithm.cpp \
search_extended_parser.cpp \
search_interface.cpp \
search_simple_algorithm.cpp \
search_simple_engine.cpp \
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
unixfile.cpp \
variable.cpp \
varthunk.cpp \
windman.cpp \
window.cpp

# DO NOT DELETE THIS LINE
