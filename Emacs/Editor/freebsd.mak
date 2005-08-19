#
#	Make file for FreeBSD BEmacs editor component
#

##################################################################################
#                                                                                #
#                                                                                #
#          Settings                                                              #
#                                                                                #
#                                                                                #
##################################################################################
CC=/usr/bin/gcc -pipe
CXX=/usr/bin/g++ -pipe
cc_flags = -g -D_DEBUG -I. -I/usr/X11R6/include -c
cpp = $(CXX) -ansi -pedantic -Wall -Wno-long-long -frtti -D__std_cpp__ -D__has_bool__
cc = $(CC) -ansi -pedantic -Wall=pthread
ldstatic = $(cpp) -g -static -L/usr/X11R6/lib
lddynamic = $(cpp) -g -dynamic -L/usr/X11R6/lib
edit_obj=freebsd/obj/
#edit_src=
edit_exe=freebsd/exe/

obj_files = \
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
 $(edit_obj)ptyopen_bsd.o \
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
install: install_images install_bitmaps install_xkeys

install_images: static
	for file in bemacs mll-2-db dbadd dbcreate dblist dbdel dbprint; \
	do \
		cp -f $(edit_exe)$$file $(BUILD_KIT_DIR); \
		chmod ugo=rx $(BUILD_KIT_DIR)/$$file; \
	done
	cp -f $(edit_exe)bemacs_server_static $(BUILD_KIT_DIR)/bemacs_server
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
 $(edit_obj)ndbm.o \
 $(edit_obj)db_rtl.o \
 $(edit_obj)stub_rtl.o \
 $(edit_obj)emstring.o \
 $(edit_obj)file_name_compare.o \
 $(edit_obj)unixfile.o \
 $(edit_obj)em_stat.o \
 $(edit_obj)doprint.o

$(edit_exe)dbadd : $(edit_obj)dbadd.o $(db_obj_files)
	@ echo Info: Link $(edit_exe)dbadd
	$(cpp) -o $(edit_obj)dbadd -g $(edit_obj)dbadd.o $(db_obj_files)
	cp $(edit_obj)dbadd $(edit_exe)dbadd
	strip $(edit_exe)dbadd

$(edit_obj)dbadd.o : $(edit_src)dbadd.cpp
	@ echo Info: Compile $(edit_src)dbadd.cpp
	@ $(cpp) $(cc_flags) -o $(edit_obj)dbadd.o $(edit_src)dbadd.cpp

$(edit_exe)bemacs : $(edit_obj)emclient.o $(edit_obj)em_stat.o $(edit_obj)emstring.o $(edit_obj)stub_rtl.o
	@ echo Info: Link $(edit_exe)bemacs
	$(cpp) -o $(edit_obj)bemacs -g $(edit_obj)emclient.o $(edit_obj)em_stat.o $(edit_obj)emstring.o $(edit_obj)stub_rtl.o $(edit_obj)doprint.o
	cp $(edit_obj)bemacs $(edit_exe)bemacs
	strip $(edit_exe)bemacs

$(edit_obj)emclient.o : $(edit_src)emclient.cpp
	@ echo Info: Compile $(edit_src)emclient.cpp
	@ $(cpp) $(cc_flags) -o $(edit_obj)emclient.o $(edit_src)emclient.cpp

$(edit_exe)mll-2-db : $(edit_obj)mll-2-db.o $(db_obj_files)
	@ echo Info: Link $(edit_exe)mll-2-db
	$(cpp) -o $(edit_obj)mll-2-db -g $(edit_obj)mll-2-db.o $(db_obj_files)
	cp $(edit_obj)mll-2-db $(edit_exe)mll-2-db
	strip $(edit_exe)mll-2-db

$(edit_obj)mll-2-db.o : $(edit_src)mll-2-db.cpp
	@ echo Info: Compile $(edit_src)mll-2-db.cpp
	@ $(cpp) $(cc_flags) -o $(edit_obj)mll-2-db.o $(edit_src)mll-2-db.cpp

$(edit_exe)dbcreate : $(edit_obj)dbcreate.o $(db_obj_files)
	@ echo Info: Link $(edit_exe)dbcreate
	$(cpp) -o $(edit_obj)dbcreate -g $(edit_obj)dbcreate.o $(db_obj_files)
	cp $(edit_obj)dbcreate $(edit_exe)dbcreate
	strip $(edit_exe)dbcreate

$(edit_obj)dbcreate.o : $(edit_src)dbcreate.cpp
	@ echo Info: Compile $(edit_src)dbcreate.cpp
	@ $(cpp) $(cc_flags) -o $(edit_obj)dbcreate.o $(edit_src)dbcreate.cpp

$(edit_obj)db_rtl.o : $(edit_src)db_rtl.cpp
	@ echo Info: Compile $(edit_src)db_rtl.cpp
	@ $(cpp) $(cc_flags) -o $(edit_obj)db_rtl.o $(edit_src)db_rtl.cpp

$(edit_obj)stub_rtl.o : $(edit_src)stub_rtl.cpp
	@ echo Info: Compile $(edit_src)stub_rtl.cpp
	@ $(cpp) $(cc_flags) -o $(edit_obj)stub_rtl.o $(edit_src)stub_rtl.cpp

$(edit_exe)dblist : $(edit_obj)dblist.o $(db_obj_files)
	@ echo Info: Link $(edit_exe)dblist
	$(cpp) -o $(edit_obj)dblist -g $(edit_obj)dblist.o $(db_obj_files)
	cp $(edit_obj)dblist $(edit_exe)dblist
	strip $(edit_exe)dblist

$(edit_obj)dblist.o : $(edit_src)dblist.cpp
	@ echo Info: Compile $(edit_src)dblist.cpp
	@ $(cpp) $(cc_flags) -o $(edit_obj)dblist.o $(edit_src)dblist.cpp

$(edit_exe)dbprint : $(edit_obj)dbprint.o $(db_obj_files)
	@ echo Info: Link $(edit_exe)dbprint
	$(cpp) -o $(edit_obj)dbprint -g $(edit_obj)dbprint.o $(db_obj_files)
	cp $(edit_obj)dbprint $(edit_exe)dbprint
	strip $(edit_exe)dbprint

$(edit_obj)dbprint.o : $(edit_src)dbprint.cpp
	@ echo Info: Compile $(edit_src)dbprint.cpp
	@ $(cpp) $(cc_flags) -o $(edit_obj)dbprint.o $(edit_src)dbprint.cpp

$(edit_exe)dbdel : $(edit_obj)dbdel.o $(db_obj_files)
	@ echo Info: Link $(edit_exe)dbdel
	$(cpp) -o $(edit_obj)dbdel -g $(edit_obj)dbdel.o $(db_obj_files)
	cp $(edit_obj)dbdel $(edit_exe)dbdel
	strip $(edit_exe)dbdel

$(edit_obj)dbdel.o : $(edit_src)dbdel.cpp
	@ echo Info: Compile $(edit_src)dbdel.cpp
	@ $(cpp) $(cc_flags) -o $(edit_obj)dbdel.o $(edit_src)dbdel.cpp

##################################################################################
#                                                                                #
#                                                                                #
#          bemacs_server                                                         #
#                                                                                #
#                                                                                #
##################################################################################
$(edit_obj)bemacs_server_dynamic : $(obj_files)
	@ echo Info: Link $(edit_obj)bemacs_server_dynamic
	$(lddynamic) -o $(edit_obj)bemacs_server_dynamic $(obj_files) -lXm -lXt -lXmu -lSM -lICE -lXext -lX11 $(EXTRALIBS) 2>&1 | c++filt

$(edit_exe)bemacs_server_dynamic : $(edit_obj)bemacs_server_dynamic
	@ echo Info: Build $(edit_exe)bemacs_server_dynamic
	cp $(edit_obj)bemacs_server_dynamic $(edit_exe)bemacs_server_dynamic
	strip $(edit_exe)bemacs_server_dynamic

$(edit_obj)bemacs_server_static : $(obj_files)
	@ echo Info: Link $(edit_obj)bemacs_server_static
	$(ldstatic) -o $(edit_obj)bemacs_server_static $(obj_files) -lXm -lXp -lXt -lSM -lICE -lXext -lX11 -lc_r $(EXTRALIBS) 2>&1 | c++filt

$(edit_exe)bemacs_server_static : $(edit_obj)bemacs_server_static
	cp $(edit_obj)bemacs_server_static $(edit_exe)bemacs_server_static
	strip $(edit_exe)bemacs_server_static

motif_test_objs=\
	$(edit_obj)motif_test.o \
	$(edit_obj)emstring.o \
	$(edit_obj)doprint.o

$(edit_obj)motif_test : $(motif_test_objs)
	@ echo Info: Link $(edit_obj)motif
	$(lddynamic) -o $(edit_obj)motif_test $(motif_test_objs) -lXm -lXt -lSM -lICE -lXext -lX11 2>&1 |c++filt

$(edit_obj)unix_rtl.o : $(edit_src)unix_rtl.cpp
	@ echo Info: Compile $(edit_src)unix_rtl.cpp
	@ $(cpp) $(cc_flags) -o $(edit_obj)unix_rtl.o $(edit_src)unix_rtl.cpp

$(edit_obj)motif_test.o : $(edit_src)motif_test.cpp
	@ echo Info: Compile $(edit_src)motif_test.cpp
	@ $(cpp) $(cc_flags) -o $(edit_obj)motif_test.o $(edit_src)motif_test.cpp

$(edit_obj)motiftrm_test.o : $(edit_src)motiftrm.cpp
	@ echo Info: Compile $(edit_src)motiftrm.cpp
	@ $(cpp) $(cc_flags) -DMOTIF_TEST -o $(edit_obj)motiftrm_test.o $(edit_src)motiftrm.cpp

$(edit_obj)motiftrm.o : $(edit_src)motiftrm.cpp $(edit_src)emacs_motif.h
	@ echo Info: Compile $(edit_src)motiftrm.cpp $(edit_src)emacs_motif.h
	@ $(cpp) $(cc_flags) -o $(edit_obj)motiftrm.o $(edit_src)motiftrm.cpp

$(edit_obj)motif_keys.o : $(edit_src)motif_keys.cpp $(edit_src)motif_keys.h
	@ echo Info: Compile $(edit_src)motif_keys.cpp
	@ $(cpp) $(cc_flags) -o $(edit_obj)motif_keys.o $(edit_src)motif_keys.cpp

$(edit_obj)emacs_motif.o : $(edit_src)emacs_motif.cpp $(edit_src)emacs_motif.h
	@ echo Info: Compile $(edit_src)emacs_motif.cpp $(edit_src)emacs_motif.h
	@ $(cpp) $(cc_flags) -o $(edit_obj)emacs_motif.o $(edit_src)emacs_motif.cpp

$(edit_obj)motiftk.o : $(edit_src)motiftk.cpp
	@ echo Info: Compile $(edit_src)motiftk.cpp
	@ $(cpp) $(cc_flags) -o $(edit_obj)motiftk.o $(edit_src)motiftk.cpp

$(edit_obj)motifcmd.o : $(edit_src)motifcmd.cpp
	@ echo Info: Compile $(edit_src)motifcmd.cpp
	@ $(cpp) $(cc_flags) -o $(edit_obj)motifcmd.o $(edit_src)motifcmd.cpp

$(edit_obj)unix_trm.o : $(edit_src)unix_trm.cpp
	@ echo Info: Compile $(edit_src)unix_trm.cpp
	@ $(cpp) $(cc_flags) -o $(edit_obj)unix_trm.o $(edit_src)unix_trm.cpp

$(edit_obj)abbrev.o : $(edit_src)abbrev.cpp
	@ echo Info: Compile $(edit_src)abbrev.cpp
	@ $(cpp) $(cc_flags) -o $(edit_obj)abbrev.o $(edit_src)abbrev.cpp

$(edit_obj)abspath.o : $(edit_src)abspath.cpp
	@ echo Info: Compile $(edit_src)abspath.cpp
	@ $(cpp) $(cc_flags) -o $(edit_obj)abspath.o $(edit_src)abspath.cpp

$(edit_obj)arith.o : $(edit_src)arith.cpp
	@ echo Info: Compile $(edit_src)arith.cpp
	@ $(cpp) $(cc_flags) -o $(edit_obj)arith.o $(edit_src)arith.cpp

$(edit_obj)timer.o : $(edit_src)timer.cpp
	@ echo Info: Compile $(edit_src)timer.cpp
	@ $(cpp) $(cc_flags) -o $(edit_obj)timer.o $(edit_src)timer.cpp

$(edit_obj)em_time.o : $(edit_src)em_time.cpp
	@ echo Info: Compile $(edit_src)em_time.cpp
	@ $(cpp) $(cc_flags) -o $(edit_obj)em_time.o $(edit_src)em_time.cpp

$(edit_obj)em_stat.o : $(edit_src)em_stat.cpp
	@ echo Info: Compile $(edit_src)em_stat.cpp
	@ $(cpp) $(cc_flags) -o $(edit_obj)em_stat.o $(edit_src)em_stat.cpp

$(edit_obj)buffer.o : $(edit_src)buffer.cpp
	@ echo Info: Compile $(edit_src)buffer.cpp
	@ $(cpp) $(cc_flags) -o $(edit_obj)buffer.o $(edit_src)buffer.cpp

$(edit_obj)buf_man.o : $(edit_src)buf_man.cpp
	@ echo Info: Compile $(edit_src)buf_man.cpp
	@ $(cpp) $(cc_flags) -o $(edit_obj)buf_man.o $(edit_src)buf_man.cpp

$(edit_obj)caseconv.o : $(edit_src)caseconv.cpp
	@ echo Info: Compile $(edit_src)caseconv.cpp
	@ $(cpp) $(cc_flags) -o $(edit_obj)caseconv.o $(edit_src)caseconv.cpp

$(edit_obj)columns.o : $(edit_src)columns.cpp
	@ echo Info: Compile $(edit_src)columns.cpp
	@ $(cpp) $(cc_flags) -o $(edit_obj)columns.o $(edit_src)columns.cpp

$(edit_obj)CommandLine.o : $(edit_src)CommandLine.cpp
	@ echo Info: Compile $(edit_src)CommandLine.cpp
	@ $(cpp) $(cc_flags) -o $(edit_obj)CommandLine.o $(edit_src)CommandLine.cpp

$(edit_obj)dbman.o : $(edit_src)dbman.cpp
	@ echo Info: Compile $(edit_src)dbman.cpp
	@ $(cpp) $(cc_flags) -o $(edit_obj)dbman.o $(edit_src)dbman.cpp

$(edit_obj)display.o : $(edit_src)display.cpp
	@ echo Info: Compile $(edit_src)display.cpp
	@ $(cpp) $(cc_flags) -o $(edit_obj)display.o $(edit_src)display.cpp

$(edit_obj)display_line.o : $(edit_src)display_line.cpp
	@ echo Info: Compile $(edit_src)display_line.cpp
	@ $(cpp) $(cc_flags) -o $(edit_obj)display_line.o $(edit_src)display_line.cpp

$(edit_obj)display_insert_delete.o : $(edit_src)display_insert_delete.cpp
	@ echo Info: Compile $(edit_src)display_insert_delete.cpp
	@ $(cpp) $(cc_flags) -o $(edit_obj)display_insert_delete.o $(edit_src)display_insert_delete.cpp

$(edit_obj)doprint.o : $(edit_src)doprint.cpp
	@ echo Info: Compile $(edit_src)doprint.cpp
	@ $(cpp) $(cc_flags) -o $(edit_obj)doprint.o $(edit_src)doprint.cpp

$(edit_obj)emacs.o : $(edit_src)emacs.cpp
	@ echo Info: Compile $(edit_src)emacs.cpp
	@ $(cpp) $(cc_flags) -o $(edit_obj)emacs.o $(edit_src)emacs.cpp

$(edit_obj)emacsrtl.o : $(edit_src)emacsrtl.cpp
	@ echo Info: Compile $(edit_src)emacsrtl.cpp
	@ $(cpp) $(cc_flags) -o $(edit_obj)emacsrtl.o $(edit_src)emacsrtl.cpp

$(edit_obj)emacs_signal.o : $(edit_src)emacs_signal.cpp
	@ echo Info: Compile $(edit_src)emacs_signal.cpp
	@ $(cpp) $(cc_flags) -o $(edit_obj)emacs_signal.o $(edit_src)emacs_signal.cpp

$(edit_obj)emarray.o : $(edit_src)emarray.cpp
	@ echo Info: Compile $(edit_src)emarray.cpp
	@ $(cpp) $(cc_flags) -o $(edit_obj)emarray.o $(edit_src)emarray.cpp

$(edit_obj)emstring.o : $(edit_src)emstring.cpp
	@ echo Info: Compile $(edit_src)emstring.cpp
	@ $(cpp) $(cc_flags) -o $(edit_obj)emstring.o $(edit_src)emstring.cpp

$(edit_obj)emstrtab.o : $(edit_src)emstrtab.cpp
	@ echo Info: Compile $(edit_src)emstrtab.cpp
	@ $(cpp) $(cc_flags) -o $(edit_obj)emstrtab.o $(edit_src)emstrtab.cpp

$(edit_obj)errlog.o : $(edit_src)errlog.cpp
	@ echo Info: Compile $(edit_src)errlog.cpp
	@ $(cpp) $(cc_flags) -o $(edit_obj)errlog.o $(edit_src)errlog.cpp

$(edit_obj)ext_func.o : $(edit_src)ext_func.cpp
	@ echo Info: Compile $(edit_src)ext_func.cpp
	@ $(cpp) $(cc_flags) -o $(edit_obj)ext_func.o $(edit_src)ext_func.cpp

$(edit_obj)file_name_compare.o : $(edit_src)file_name_compare.cpp
	@ echo Info: Compile $(edit_src)file_name_compare.cpp
	@ $(cpp) $(cc_flags) -o $(edit_obj)file_name_compare.o $(edit_src)file_name_compare.cpp

$(edit_obj)fileio.o : $(edit_src)fileio.cpp
	@ echo Info: Compile $(edit_src)fileio.cpp
	@ $(cpp) $(cc_flags) -o $(edit_obj)fileio.o $(edit_src)fileio.cpp

$(edit_obj)fio.o : $(edit_src)fio.cpp
	@ echo Info: Compile $(edit_src)fio.cpp
	@ $(cpp) $(cc_flags) -o $(edit_obj)fio.o $(edit_src)fio.cpp

$(edit_obj)function.o : $(edit_src)function.cpp
	@ echo Info: Compile $(edit_src)function.cpp
	@ $(cpp) $(cc_flags) -o $(edit_obj)function.o $(edit_src)function.cpp

$(edit_obj)getdb.o : $(edit_src)getdb.cpp
	@ echo Info: Compile $(edit_src)getdb.cpp
	@ $(cpp) $(cc_flags) -o $(edit_obj)getdb.o $(edit_src)getdb.cpp

$(edit_obj)getfile.o : $(edit_src)getfile.cpp
	@ echo Info: Compile $(edit_src)getfile.cpp
	@ $(cpp) $(cc_flags) -o $(edit_obj)getfile.o $(edit_src)getfile.cpp

$(edit_obj)getdirectory.o : $(edit_src)getdirectory.cpp
	@ echo Info: Compile $(edit_src)getdirectory.cpp
	@ $(cpp) $(cc_flags) -o $(edit_obj)getdirectory.o $(edit_src)getdirectory.cpp

$(edit_obj)glob_var.o : $(edit_src)glob_var.cpp
	@ echo Info: Compile $(edit_src)glob_var.cpp
	@ $(cpp) $(cc_flags) -o $(edit_obj)glob_var.o $(edit_src)glob_var.cpp

$(edit_obj)gui_input_mode.o : $(edit_src)gui_input_mode.cpp
	@ echo Info: Compile $(edit_src)gui_input_mode.cpp
	@ $(cpp) $(cc_flags) -o $(edit_obj)gui_input_mode.o $(edit_src)gui_input_mode.cpp

$(edit_obj)journal.o : $(edit_src)journal.cpp
	@ echo Info: Compile $(edit_src)journal.cpp
	@ $(cpp) $(cc_flags) -o $(edit_obj)journal.o $(edit_src)journal.cpp

$(edit_obj)keyboard.o : $(edit_src)keyboard.cpp
	@ echo Info: Compile $(edit_src)keyboard.cpp
	@ $(cpp) $(cc_flags) -o $(edit_obj)keyboard.o $(edit_src)keyboard.cpp

$(edit_obj)key_names.o : $(edit_src)key_names.cpp
	@ echo Info: Compile $(edit_src)key_names.cpp
	@ $(cpp) $(cc_flags) -o $(edit_obj)key_names.o $(edit_src)key_names.cpp

$(edit_obj)lispfunc.o : $(edit_src)lispfunc.cpp
	@ echo Info: Compile $(edit_src)lispfunc.cpp
	@ $(cpp) $(cc_flags) -o $(edit_obj)lispfunc.o $(edit_src)lispfunc.cpp

$(edit_obj)macros.o : $(edit_src)macros.cpp
	@ echo Info: Compile $(edit_src)macros.cpp
	@ $(cpp) $(cc_flags) -o $(edit_obj)macros.o $(edit_src)macros.cpp

$(edit_obj)mem_man.o : $(edit_src)mem_man.cpp
	@ echo Info: Compile $(edit_src)mem_man.cpp
	@ $(cpp) $(cc_flags) -o $(edit_obj)mem_man.o $(edit_src)mem_man.cpp

$(edit_obj)emacs_init.o : $(edit_src)emacs_init.cpp
	@ echo Info: Compile $(edit_src)emacs_init.cpp
	@ $(cpp) $(cc_flags) -o $(edit_obj)emacs_init.o $(edit_src)emacs_init.cpp

$(edit_obj)metacomm.o : $(edit_src)metacomm.cpp
	@ echo Info: Compile $(edit_src)metacomm.cpp
	@ $(cpp) $(cc_flags) -o $(edit_obj)metacomm.o $(edit_src)metacomm.cpp

$(edit_obj)minibuf.o : $(edit_src)minibuf.cpp
	@ echo Info: Compile $(edit_src)minibuf.cpp
	@ $(cpp) $(cc_flags) -o $(edit_obj)minibuf.o $(edit_src)minibuf.cpp

$(edit_obj)mlispars.o : $(edit_src)mlispars.cpp
	@ echo Info: Compile $(edit_src)mlispars.cpp
	@ $(cpp) $(cc_flags) -o $(edit_obj)mlispars.o $(edit_src)mlispars.cpp

$(edit_obj)mlispexp.o : $(edit_src)mlispexp.cpp
	@ echo Info: Compile $(edit_src)mlispexp.cpp
	@ $(cpp) $(cc_flags) -o $(edit_obj)mlispexp.o $(edit_src)mlispexp.cpp

$(edit_obj)mlisproc.o : $(edit_src)mlisproc.cpp
	@ echo Info: Compile $(edit_src)mlisproc.cpp
	@ $(cpp) $(cc_flags) -o $(edit_obj)mlisproc.o $(edit_src)mlisproc.cpp

$(edit_obj)mlprintf.o : $(edit_src)mlprintf.cpp
	@ echo Info: Compile $(edit_src)mlprintf.cpp
	@ $(cpp) $(cc_flags) -o $(edit_obj)mlprintf.o $(edit_src)mlprintf.cpp

$(edit_obj)ndbm.o : $(edit_src)ndbm.cpp
	@ echo Info: Compile $(edit_src)ndbm.cpp
	@ $(cpp) $(cc_flags) -o $(edit_obj)ndbm.o $(edit_src)ndbm.cpp

$(edit_obj)options.o : $(edit_src)options.cpp
	@ echo Info: Compile $(edit_src)options.cpp
	@ $(cpp) $(cc_flags) -o $(edit_obj)options.o $(edit_src)options.cpp

$(edit_obj)osfile.o : $(edit_src)osfile.cpp
	@ echo Info: Compile $(edit_src)osfile.cpp
	@ $(cpp) $(cc_flags) -o $(edit_obj)osfile.o $(edit_src)osfile.cpp

$(edit_obj)osinter.o : $(edit_src)osinter.cpp
	@ echo Info: Compile $(edit_src)osinter.cpp
	@ $(cpp) $(cc_flags) -o $(edit_obj)osinter.o $(edit_src)osinter.cpp

$(edit_obj)profile.o : $(edit_src)profile.cpp
	@ echo Info: Compile $(edit_src)profile.cpp
	@ $(cpp) $(cc_flags) -o $(edit_obj)profile.o $(edit_src)profile.cpp

$(edit_obj)ptyopen_bsd.o : $(edit_src)ptyopen_bsd.cpp
	@ echo Info: Compile $(edit_src)ptyopen_bsd.cpp
	@ $(cpp) $(cc_flags) -o $(edit_obj)ptyopen_bsd.o $(edit_src)ptyopen_bsd.cpp

$(edit_obj)emacs_proc.o : $(edit_src)emacs_proc.cpp
	@ echo Info: Compile $(edit_src)emacs_proc.cpp
	@ $(cpp) $(cc_flags) -o $(edit_obj)emacs_proc.o $(edit_src)emacs_proc.cpp

$(edit_obj)queue.o : $(edit_src)queue.cpp
	@ echo Info: Compile $(edit_src)queue.cpp
	@ $(cpp) $(cc_flags) -o $(edit_obj)queue.o $(edit_src)queue.cpp

$(edit_obj)save_env.o : $(edit_src)save_env.cpp
	@ echo Info: Compile $(edit_src)save_env.cpp
	@ $(cpp) $(cc_flags) -o $(edit_obj)save_env.o $(edit_src)save_env.cpp

$(edit_obj)search.o : $(edit_src)search.cpp
	@ echo Info: Compile $(edit_src)search.cpp
	@ $(cpp) $(cc_flags) -o $(edit_obj)search.o $(edit_src)search.cpp

$(edit_obj)search_extended_algorithm.o : $(edit_src)search_extended_algorithm.cpp
	@ echo Info: Compile $(edit_src)search_extended_algorithm.cpp
	@ $(cpp) $(cc_flags) -o $(edit_obj)search_extended_algorithm.o $(edit_src)search_extended_algorithm.cpp

$(edit_obj)search_extended_parser.o : $(edit_src)search_extended_parser.cpp
	@ echo Info: Compile $(edit_src)search_extended_parser.cpp
	@ $(cpp) $(cc_flags) -o $(edit_obj)search_extended_parser.o $(edit_src)search_extended_parser.cpp

$(edit_obj)search_interface.o : $(edit_src)search_interface.cpp
	@ echo Info: Compile $(edit_src)search_interface.cpp
	@ $(cpp) $(cc_flags) -o $(edit_obj)search_interface.o $(edit_src)search_interface.cpp

$(edit_obj)search_simple_algorithm.o : $(edit_src)search_simple_algorithm.cpp
	@ echo Info: Compile $(edit_src)search_simple_algorithm.cpp
	@ $(cpp) $(cc_flags) -o $(edit_obj)search_simple_algorithm.o $(edit_src)search_simple_algorithm.cpp

$(edit_obj)search_simple_engine.o : $(edit_src)search_simple_engine.cpp
	@ echo Info: Compile $(edit_src)search_simple_engine.cpp
	@ $(cpp) $(cc_flags) -o $(edit_obj)search_simple_engine.o $(edit_src)search_simple_engine.cpp

$(edit_obj)simpcomm.o : $(edit_src)simpcomm.cpp
	@ echo Info: Compile $(edit_src)simpcomm.cpp
	@ $(cpp) $(cc_flags) -o $(edit_obj)simpcomm.o $(edit_src)simpcomm.cpp

$(edit_obj)subproc.o : $(edit_src)subproc.cpp
	@ echo Info: Compile $(edit_src)subproc.cpp
	@ $(cpp) $(cc_flags) -o $(edit_obj)subproc.o $(edit_src)subproc.cpp

$(edit_obj)string_map.o : $(edit_src)string_map.cpp
	@ echo Info: Compile $(edit_src)string_map.cpp
	@ $(cpp) $(cc_flags) -o $(edit_obj)string_map.o $(edit_src)string_map.cpp

$(edit_obj)syntax.o : $(edit_src)syntax.cpp
	@ echo Info: Compile $(edit_src)syntax.cpp
	@ $(cpp) $(cc_flags) -o $(edit_obj)syntax.o $(edit_src)syntax.cpp

$(edit_obj)term.o : $(edit_src)term.cpp $(edit_src)tt.h
	@ echo Info: Compile $(edit_src)term.cpp $(edit_src)tt.h
	@ $(cpp) $(cc_flags) -o $(edit_obj)term.o $(edit_src)term.cpp

$(edit_obj)trm_ansi.o : $(edit_src)trm_ansi.cpp $(edit_src)tt.h
	@ echo Info: Compile $(edit_src)trm_ansi.cpp $(edit_src)tt.h
	@ $(cpp) $(cc_flags) -o $(edit_obj)trm_ansi.o $(edit_src)trm_ansi.cpp

$(edit_obj)undo.o : $(edit_src)undo.cpp
	@ echo Info: Compile $(edit_src)undo.cpp
	@ $(cpp) $(cc_flags) -o $(edit_obj)undo.o $(edit_src)undo.cpp

$(edit_obj)variable.o : $(edit_src)variable.cpp
	@ echo Info: Compile $(edit_src)variable.cpp
	@ $(cpp) $(cc_flags) -o $(edit_obj)variable.o $(edit_src)variable.cpp

$(edit_obj)varthunk.o : $(edit_src)varthunk.cpp
	@ echo Info: Compile $(edit_src)varthunk.cpp
	@ $(cpp) $(cc_flags) -o $(edit_obj)varthunk.o $(edit_src)varthunk.cpp

$(edit_obj)unixcomm.o : $(edit_src)unixcomm.cpp
	@ echo Info: Compile $(edit_src)unixcomm.cpp
	@ $(cpp) $(cc_flags) -o $(edit_obj)unixcomm.o $(edit_src)unixcomm.cpp

$(edit_obj)unixfile.o : $(edit_src)unixfile.cpp
	@ echo Info: Compile $(edit_src)unixfile.cpp
	@ $(cpp) $(cc_flags) -o $(edit_obj)unixfile.o $(edit_src)unixfile.cpp

$(edit_obj)unix_ext_func.o : $(edit_src)unix_ext_func.cpp
	@ echo Info: Compile $(edit_src)unix_ext_func.cpp
	@ $(cpp) $(cc_flags) -o $(edit_obj)unix_ext_func.o $(edit_src)unix_ext_func.cpp

$(edit_obj)windman.o : $(edit_src)windman.cpp
	@ echo Info: Compile $(edit_src)windman.cpp
	@ $(cpp) $(cc_flags) -o $(edit_obj)windman.o $(edit_src)windman.cpp

$(edit_obj)window.o : $(edit_src)window.cpp
	@ echo Info: Compile $(edit_src)window.cpp
	@ $(cpp) $(cc_flags) -o $(edit_obj)window.o $(edit_src)window.cpp


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
