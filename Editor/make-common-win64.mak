#
#	make_common.mak common unix make file
#
CXX_OBJECTS=$(edit_obj)cxxsupport.obj \
$(edit_obj)cxx_extensions.obj \
$(edit_obj)cxxextensions.obj \
$(edit_obj)IndirectPythonInterface.obj

pybemacs_specific_obj_files=$(edit_obj)pybemacs.obj \
$(edit_obj)bemacs_python.obj \
$(edit_obj)python_thread_control.obj \
$(CXX_OBJECTS)

obj_files = $(os_specific_obj_files) \
 $(edit_obj)abbrev.obj \
 $(edit_obj)abspath.obj \
 $(edit_obj)arith.obj \
 $(edit_obj)buf_man.obj \
 $(edit_obj)buffer.obj \
 $(edit_obj)caseconv.obj \
 $(edit_obj)columns.obj \
 $(edit_obj)CommandLine.obj \
 $(edit_obj)dbman.obj \
 $(edit_obj)display.obj \
 $(edit_obj)display_insert_delete.obj \
 $(edit_obj)display_line.obj \
 $(edit_obj)doprint.obj \
 $(edit_obj)em_stat.obj \
 $(edit_obj)em_time.obj \
 $(edit_obj)emacs.obj \
 $(edit_obj)emacs_init.obj \
 $(edit_obj)emacs_proc.obj \
 $(edit_obj)emacsrtl.obj \
 $(edit_obj)emarray.obj \
 $(edit_obj)emstring.obj \
 $(edit_obj)emunicode.obj \
 $(edit_obj)emstrtab.obj \
 $(edit_obj)errlog.obj \
 $(edit_obj)ext_func.obj \
 $(edit_obj)file_name_compare.obj \
 $(edit_obj)fileio.obj \
 $(edit_obj)fio.obj \
 $(edit_obj)function.obj \
 $(edit_obj)getdb.obj \
 $(edit_obj)getdirectory.obj \
 $(edit_obj)getfile.obj \
 $(edit_obj)glob_var.obj \
 $(edit_obj)gui_input_mode.obj \
 $(edit_obj)journal.obj \
 $(edit_obj)key_names.obj \
 $(edit_obj)keyboard.obj \
 $(edit_obj)lispfunc.obj \
 $(edit_obj)macros.obj \
 $(edit_obj)mem_man.obj \
 $(edit_obj)metacomm.obj \
 $(edit_obj)minibuf.obj \
 $(edit_obj)mlispars.obj \
 $(edit_obj)mlispexp.obj \
 $(edit_obj)mlisproc.obj \
 $(edit_obj)mlprintf.obj \
 $(edit_obj)ndbm.obj \
 $(edit_obj)options.obj \
 $(edit_obj)queue.obj \
 $(edit_obj)save_env.obj \
 $(edit_obj)search.obj \
 $(edit_obj)search_extended_algorithm.obj \
 $(edit_obj)search_extended_parser.obj \
 $(edit_obj)search_interface.obj \
 $(edit_obj)search_simple_algorithm.obj \
 $(edit_obj)search_simple_engine.obj \
 $(edit_obj)simpcomm.obj \
 $(edit_obj)string_map.obj \
 $(edit_obj)subproc.obj \
 $(edit_obj)syntax.obj \
 $(edit_obj)term.obj \
 $(edit_obj)timer.obj \
 $(edit_obj)undo.obj \
 $(edit_obj)unix_ext_func.obj \
 $(edit_obj)unixfile.obj \
 $(edit_obj)variable.obj \
 $(edit_obj)varthunk.obj \
 $(edit_obj)windman.obj \
 $(edit_obj)window.obj \


##################################################################################
#                                                                                #
#                                                                                #
#          Main targets                                                          #
#                                                                                #
#                                                                                #
##################################################################################

pybemacs: $(edit_obj)_bemacs.so be_main bintools

bintools: $(edit_exe)bemacs $(edit_exe)mll-2-db $(edit_exe)dbadd \
	$(edit_exe)dbcreate $(edit_exe)dblist $(edit_exe)dbdel $(edit_exe)dbprint

##################################################################################
#                                                                                #
#                                                                                #
#          Install                                                               #
#                                                                                #
#                                                                                #
##################################################################################

install_pybemacs_images: $(edit_obj)_bemacs.so be_main bintools
	for file in _bemacs.so mll-2-db dbadd dbcreate dblist dbdel dbprint; \
	do \
		cp -f $(edit_exe)$$file $(BUILD_KIT_DIR); \
		chmod ugo=rx $(BUILD_KIT_DIR)/$$file; \
	done

install_bitmaps:
	cp -f bitmaps/*.xpm $(BUILD_KIT_DIR)
	chmod ugo=r $(BUILD_KIT_DIR)/*.xpm

install_xkeys:
	cp -f X11_keymaps/*.xkeys $(BUILD_KIT_DIR)
	chmod ugo=r $(BUILD_KIT_DIR)/*.xkeys

##################################################################################
#                                                                                #
#                                                                                #
#          be_main                                                               #
#                                                                                #
#                                                                                #
##################################################################################
be_main:
	cd PythonBEmacs && make -f macosx.mak clean all

##################################################################################
#                                                                                #
#                                                                                #
#          bintools                                                              #
#                                                                                #
#                                                                                #
##################################################################################

db_obj_files= \
 $(edit_obj)db_rtl.obj \
 $(edit_obj)doprint.obj \
 $(edit_obj)em_stat.obj \
 $(edit_obj)emstring.obj \
 $(edit_obj)emunicode.obj \
 $(edit_obj)file_name_compare.obj \
 $(edit_obj)ndbm.obj \
 $(edit_obj)stub_rtl.obj \
 $(edit_obj)unixfile.obj \

$(edit_exe)dbadd : $(edit_obj)dbadd.obj $(db_obj_files)
	@ echo Info: Link $(edit_exe)dbadd
	$(ld_bintools) -o $(edit_obj)dbadd -g $(edit_obj)dbadd.obj $(db_obj_files)
	cp $(edit_obj)dbadd $(edit_exe)dbadd
	strip $(edit_exe)dbadd

$(edit_obj)dbadd.obj : Utilities/dbadd/dbadd.cpp
	@ echo Info: Compile Utilities/dbadd/dbadd.cpp
	@ $(cpp) $(cc_flags) -o $(edit_obj)dbadd.obj Utilities/dbadd/dbadd.cpp

$(edit_exe)bemacs : $(edit_obj)emclient.obj $(edit_obj)em_stat.obj $(edit_obj)emstring.obj $(edit_obj)emunicode.obj $(edit_obj)stub_rtl.obj $(os_specific_client_obj_files) $(edit_obj)doprint.obj
	@ echo Info: Link $(edit_exe)bemacs
	$(ld_bintools) -o $(edit_obj)bemacs -g $(os_specific_client_ld_options) $(edit_obj)emclient.obj $(edit_obj)em_stat.obj $(edit_obj)emstring.obj $(edit_obj)emunicode.obj $(edit_obj)stub_rtl.obj $(os_specific_client_obj_files) $(edit_obj)doprint.obj
	cp $(edit_obj)bemacs $(edit_exe)bemacs
	strip $(edit_exe)bemacs

$(edit_obj)emclient.obj : Source/Unix/emclient.cpp
	@ echo Info: Compile Source/Unix/emclient.cpp
	@ $(cpp) $(cc_flags) -o $(edit_obj)emclient.obj Source/Unix/emclient.cpp

$(edit_exe)mll-2-db : $(edit_obj)mll-2-db.obj $(db_obj_files)
	@ echo Info: Link $(edit_exe)mll-2-db
	$(ld_bintools) -o $(edit_obj)mll-2-db -g $(edit_obj)mll-2-db.obj $(db_obj_files)
	cp $(edit_obj)mll-2-db $(edit_exe)mll-2-db
	strip $(edit_exe)mll-2-db

$(edit_obj)mll-2-db.obj : Utilities/mll2db/mll-2-db.cpp
	@ echo Info: Compile Utilities/mll2db/mll-2-db.cpp
	@ $(cpp) $(cc_flags) -o $(edit_obj)mll-2-db.obj Utilities/mll2db/mll-2-db.cpp

$(edit_exe)dbcreate : $(edit_obj)dbcreate.obj $(db_obj_files)
	@ echo Info: Link $(edit_exe)dbcreate
	$(ld_bintools) -o $(edit_obj)dbcreate -g $(edit_obj)dbcreate.obj $(db_obj_files)
	cp $(edit_obj)dbcreate $(edit_exe)dbcreate
	strip $(edit_exe)dbcreate

$(edit_obj)dbcreate.obj : Utilities/dbcreate/dbcreate.cpp
	@ echo Info: Compile Utilities/dbcreate/dbcreate.cpp
	@ $(cpp) $(cc_flags) -o $(edit_obj)dbcreate.obj Utilities/dbcreate/dbcreate.cpp

$(edit_obj)db_rtl.obj : Utilities/db_rtl/db_rtl.cpp
	@ echo Info: Compile Utilities/db_rtl/db_rtl.cpp
	@ $(cpp) $(cc_flags) -o $(edit_obj)db_rtl.obj Utilities/db_rtl/db_rtl.cpp

$(edit_obj)stub_rtl.obj : Utilities/db_rtl/stub_rtl.cpp
	@ echo Info: Compile Utilities/db_rtl/stub_rtl.cpp
	@ $(cpp) $(cc_flags) -o $(edit_obj)stub_rtl.obj Utilities/db_rtl/stub_rtl.cpp

$(edit_exe)dblist : $(edit_obj)dblist.obj $(db_obj_files)
	@ echo Info: Link $(edit_exe)dblist
	$(ld_bintools) -o $(edit_obj)dblist -g $(edit_obj)dblist.obj $(db_obj_files)
	cp $(edit_obj)dblist $(edit_exe)dblist
	strip $(edit_exe)dblist

$(edit_obj)dblist.obj : Utilities/dblist/dblist.cpp
	@ echo Info: Compile Utilities/dblist/dblist.cpp
	@ $(cpp) $(cc_flags) -o $(edit_obj)dblist.obj Utilities/dblist/dblist.cpp

$(edit_exe)dbprint : $(edit_obj)dbprint.obj $(db_obj_files)
	@ echo Info: Link $(edit_exe)dbprint
	$(ld_bintools) -o $(edit_obj)dbprint -g $(edit_obj)dbprint.obj $(db_obj_files)
	cp $(edit_obj)dbprint $(edit_exe)dbprint
	strip $(edit_exe)dbprint

$(edit_obj)dbprint.obj : Utilities/dbprint/dbprint.cpp
	@ echo Info: Compile Utilities/dbprint/dbprint.cpp
	@ $(cpp) $(cc_flags) -o $(edit_obj)dbprint.obj Utilities/dbprint/dbprint.cpp

$(edit_exe)dbdel : $(edit_obj)dbdel.obj $(db_obj_files)
	@ echo Info: Link $(edit_exe)dbdel
	$(ld_bintools) -o $(edit_obj)dbdel -g $(edit_obj)dbdel.obj $(db_obj_files)
	cp $(edit_obj)dbdel $(edit_exe)dbdel
	strip $(edit_exe)dbdel

$(edit_obj)dbdel.obj : Utilities/dbdel/dbdel.cpp
	@ echo Info: Compile Utilities/dbdel/dbdel.cpp
	@ $(cpp) $(cc_flags) -o $(edit_obj)dbdel.obj Utilities/dbdel/dbdel.cpp

##################################################################################
#                                                                                #
#                                                                                #
#          _bemacs.so                                                            #
#                                                                                #
#                                                                                #
##################################################################################
$(edit_obj)_bemacs.so: $(pybemacs_specific_obj_files) $(obj_files)
	@echo Info: Compile $@
	@$(LDSHARED) -o $@ $(pybemacs_specific_obj_files) $(obj_files) $(LDLIBS)

$(edit_obj)pybemacs.obj: Source/pybemacs/pybemacs.cpp Source/pybemacs/bemacs_python.hpp
	@echo Info: Compile $<
	@$(cpp) $(cc_flags) -o $@ $<

$(edit_obj)bemacs_python.obj: Source/pybemacs/bemacs_python.cpp Source/pybemacs/bemacs_python.hpp
	@echo Info: Compile $<
	@$(cpp) $(cc_flags) -o $@ $<

$(edit_obj)python_thread_control.obj: Source/pybemacs/python_thread_control.cpp Source/pybemacs/bemacs_python.hpp
	@echo Info: Compile $<
	@$(cpp) $(cc_flags) -o $@ $<

$(edit_obj)cxxsupport.obj: $(PYCXXSRC)/cxxsupport.cxx
	@echo Info: Compile $<
	@$(CCC) -c $(CCCFLAGS) -o $@ $<

$(edit_obj)cxx_extensions.obj: $(PYCXXSRC)/cxx_extensions.cxx
	@echo Info: Compile $<
	@$(CCC) -c $(CCCFLAGS) -o $@ $<

$(edit_obj)cxxextensions.obj: $(PYCXXSRC)/cxxextensions.c
	@echo Info: Compile $<
	@$(CC) -c $(CCFLAGS) -o $@ $<

$(edit_obj)IndirectPythonInterface.obj: $(PYCXXSRC)/IndirectPythonInterface.cxx
	@echo Info: Compile $<
	@$(CCC) -c $(CCCFLAGS) -o $@ $< 

clean::
	rm -f $(edit_exe)_bemacs.so

##################################################################################
#                                                                                #
#                                                                                #
#          bemacs_server                                                         #
#                                                                                #
#                                                                                #
##################################################################################
$(edit_obj)linux_ld_fix.obj : Source/Unix/linux_ld_fix.cpp
	@ echo Info: Compile Source/Unix/linux_ld_fix.cpp
	@ $(cpp) $(cc_flags) -o $(edit_obj)linux_ld_fix.obj Source/Unix/linux_ld_fix.cpp

$(edit_obj)unix_rtl.obj : Source/Unix/unix_rtl.cpp
	@ echo Info: Compile Source/Unix/unix_rtl.cpp
	@ $(cpp) $(cc_flags) -o $(edit_obj)unix_rtl.obj Source/Unix/unix_rtl.cpp

$(edit_obj)abbrev.obj : Source/Common/abbrev.cpp
	@ echo Info: Compile Source/Common/abbrev.cpp
	@ $(cpp) $(cc_flags) -o $(edit_obj)abbrev.obj Source/Common/abbrev.cpp

$(edit_obj)abspath.obj : Source/Common/abspath.cpp
	@ echo Info: Compile Source/Common/abspath.cpp
	@ $(cpp) $(cc_flags) -o $(edit_obj)abspath.obj Source/Common/abspath.cpp

$(edit_obj)arith.obj : Source/Common/arith.cpp
	@ echo Info: Compile Source/Common/arith.cpp
	@ $(cpp) $(cc_flags) -o $(edit_obj)arith.obj Source/Common/arith.cpp

$(edit_obj)timer.obj : Source/Common/timer.cpp
	@ echo Info: Compile Source/Common/timer.cpp
	@ $(cpp) $(cc_flags) -o $(edit_obj)timer.obj Source/Common/timer.cpp

$(edit_obj)em_time.obj : Source/Common/em_time.cpp
	@ echo Info: Compile Source/Common/em_time.cpp
	@ $(cpp) $(cc_flags) -o $(edit_obj)em_time.obj Source/Common/em_time.cpp

$(edit_obj)em_stat.obj : Source/Common/em_stat.cpp
	@ echo Info: Compile Source/Common/em_stat.cpp
	@ $(cpp) $(cc_flags) -o $(edit_obj)em_stat.obj Source/Common/em_stat.cpp

$(edit_obj)buffer.obj : Source/Common/buffer.cpp
	@ echo Info: Compile Source/Common/buffer.cpp
	@ $(cpp) $(cc_flags) -o $(edit_obj)buffer.obj Source/Common/buffer.cpp

$(edit_obj)buf_man.obj : Source/Common/buf_man.cpp
	@ echo Info: Compile Source/Common/buf_man.cpp
	@ $(cpp) $(cc_flags) -o $(edit_obj)buf_man.obj Source/Common/buf_man.cpp

$(edit_obj)caseconv.obj : Source/Common/caseconv.cpp
	@ echo Info: Compile Source/Common/caseconv.cpp
	@ $(cpp) $(cc_flags) -o $(edit_obj)caseconv.obj Source/Common/caseconv.cpp

$(edit_obj)columns.obj : Source/Common/columns.cpp
	@ echo Info: Compile Source/Common/columns.cpp
	@ $(cpp) $(cc_flags) -o $(edit_obj)columns.obj Source/Common/columns.cpp

$(edit_obj)CommandLine.obj : Source/Common/CommandLine.cpp
	@ echo Info: Compile Source/Common/CommandLine.cpp
	@ $(cpp) $(cc_flags) -o $(edit_obj)CommandLine.obj Source/Common/CommandLine.cpp

$(edit_obj)dbman.obj : Source/Common/dbman.cpp
	@ echo Info: Compile Source/Common/dbman.cpp
	@ $(cpp) $(cc_flags) -o $(edit_obj)dbman.obj Source/Common/dbman.cpp

$(edit_obj)display.obj : Source/Common/display.cpp
	@ echo Info: Compile Source/Common/display.cpp
	@ $(cpp) $(cc_flags) -o $(edit_obj)display.obj Source/Common/display.cpp

$(edit_obj)display_line.obj : Source/Common/display_line.cpp
	@ echo Info: Compile Source/Common/display_line.cpp
	@ $(cpp) $(cc_flags) -o $(edit_obj)display_line.obj Source/Common/display_line.cpp

$(edit_obj)display_insert_delete.obj : Source/Common/display_insert_delete.cpp
	@ echo Info: Compile Source/Common/display_insert_delete.cpp
	@ $(cpp) $(cc_flags) -o $(edit_obj)display_insert_delete.obj Source/Common/display_insert_delete.cpp

$(edit_obj)doprint.obj : Source/Common/doprint.cpp
	@ echo Info: Compile Source/Common/doprint.cpp
	@ $(cpp) $(cc_flags) -o $(edit_obj)doprint.obj Source/Common/doprint.cpp

$(edit_obj)emacs.obj : Source/Common/emacs.cpp
	@ echo Info: Compile Source/Common/emacs.cpp
	@ $(cpp) $(cc_flags) -o $(edit_obj)emacs.obj Source/Common/emacs.cpp

$(edit_obj)emacsrtl.obj : Source/Common/emacsrtl.cpp
	@ echo Info: Compile Source/Common/emacsrtl.cpp
	@ $(cpp) $(cc_flags) -o $(edit_obj)emacsrtl.obj Source/Common/emacsrtl.cpp

$(edit_obj)emacs_signal.obj : Source/Unix/emacs_signal.cpp
	@ echo Info: Compile Source/Unix/emacs_signal.cpp
	@ $(cpp) $(cc_flags) -o $(edit_obj)emacs_signal.obj Source/Unix/emacs_signal.cpp

$(edit_obj)emarray.obj : Source/Common/emarray.cpp
	@ echo Info: Compile Source/Common/emarray.cpp
	@ $(cpp) $(cc_flags) -o $(edit_obj)emarray.obj Source/Common/emarray.cpp

$(edit_obj)emstring.obj : Source/Common/emstring.cpp
	@ echo Info: Compile Source/Common/emstring.cpp
	@ $(cpp) $(cc_flags) -o $(edit_obj)emstring.obj Source/Common/emstring.cpp

$(edit_obj)emunicode.obj : Source/Common/emunicode.cpp Include/Common/em_unicode_data.h
	@ echo Info: Compile Source/Common/emunicode.cpp
	@ $(cpp) $(cc_flags) -o $(edit_obj)emunicode.obj Source/Common/emunicode.cpp

Include/Common/em_unicode_data.h : make_unicode_data.py
	@ echo Info: Make Include/Common/em_unicode_data.h
	@ $(PYTHON) make_unicode_data.py $(UCDDIR)/UnicodeData.txt $(UCDDIR)/CaseFolding.txt Include/Common/em_unicode_data.h

$(edit_obj)emstrtab.obj : Source/Common/emstrtab.cpp
	@ echo Info: Compile Source/Common/emstrtab.cpp
	@ $(cpp) $(cc_flags) -o $(edit_obj)emstrtab.obj Source/Common/emstrtab.cpp

$(edit_obj)errlog.obj : Source/Common/errlog.cpp
	@ echo Info: Compile Source/Common/errlog.cpp
	@ $(cpp) $(cc_flags) -o $(edit_obj)errlog.obj Source/Common/errlog.cpp

$(edit_obj)ext_func.obj : Source/Common/ext_func.cpp
	@ echo Info: Compile Source/Common/ext_func.cpp
	@ $(cpp) $(cc_flags) -o $(edit_obj)ext_func.obj Source/Common/ext_func.cpp

$(edit_obj)file_name_compare.obj : Source/Common/file_name_compare.cpp
	@ echo Info: Compile Source/Common/file_name_compare.cpp
	@ $(cpp) $(cc_flags) -o $(edit_obj)file_name_compare.obj Source/Common/file_name_compare.cpp

$(edit_obj)fileio.obj : Source/Common/fileio.cpp
	@ echo Info: Compile Source/Common/fileio.cpp
	@ $(cpp) $(cc_flags) -o $(edit_obj)fileio.obj Source/Common/fileio.cpp

$(edit_obj)fio.obj : Source/Common/fio.cpp
	@ echo Info: Compile Source/Common/fio.cpp
	@ $(cpp) $(cc_flags) -o $(edit_obj)fio.obj Source/Common/fio.cpp

$(edit_obj)function.obj : Source/Common/function.cpp
	@ echo Info: Compile Source/Common/function.cpp
	@ $(cpp) $(cc_flags) -o $(edit_obj)function.obj Source/Common/function.cpp

$(edit_obj)getdb.obj : Source/Common/getdb.cpp
	@ echo Info: Compile Source/Common/getdb.cpp
	@ $(cpp) $(cc_flags) -o $(edit_obj)getdb.obj Source/Common/getdb.cpp

$(edit_obj)getfile.obj : Source/Common/getfile.cpp
	@ echo Info: Compile Source/Common/getfile.cpp
	@ $(cpp) $(cc_flags) -o $(edit_obj)getfile.obj Source/Common/getfile.cpp

$(edit_obj)getdirectory.obj : Source/Common/getdirectory.cpp
	@ echo Info: Compile Source/Common/getdirectory.cpp
	@ $(cpp) $(cc_flags) -o $(edit_obj)getdirectory.obj Source/Common/getdirectory.cpp

$(edit_obj)glob_var.obj : Source/Common/glob_var.cpp
	@ echo Info: Compile Source/Common/glob_var.cpp
	@ $(cpp) $(cc_flags) -o $(edit_obj)glob_var.obj Source/Common/glob_var.cpp

$(edit_obj)gui_input_mode.obj : Source/Common/gui_input_mode.cpp
	@ echo Info: Compile Source/Common/gui_input_mode.cpp
	@ $(cpp) $(cc_flags) -o $(edit_obj)gui_input_mode.obj Source/Common/gui_input_mode.cpp

$(edit_obj)journal.obj : Source/Common/journal.cpp
	@ echo Info: Compile Source/Common/journal.cpp
	@ $(cpp) $(cc_flags) -o $(edit_obj)journal.obj Source/Common/journal.cpp

$(edit_obj)keyboard.obj : Source/Common/keyboard.cpp
	@ echo Info: Compile Source/Common/keyboard.cpp
	@ $(cpp) $(cc_flags) -o $(edit_obj)keyboard.obj Source/Common/keyboard.cpp

$(edit_obj)key_names.obj : Source/Common/key_names.cpp
	@ echo Info: Compile Source/Common/key_names.cpp
	@ $(cpp) $(cc_flags) -o $(edit_obj)key_names.obj Source/Common/key_names.cpp

$(edit_obj)lispfunc.obj : Source/Common/lispfunc.cpp
	@ echo Info: Compile Source/Common/lispfunc.cpp
	@ $(cpp) $(cc_flags) -o $(edit_obj)lispfunc.obj Source/Common/lispfunc.cpp

$(edit_obj)macros.obj : Source/Common/macros.cpp
	@ echo Info: Compile Source/Common/macros.cpp
	@ $(cpp) $(cc_flags) -o $(edit_obj)macros.obj Source/Common/macros.cpp

$(edit_obj)mem_man.obj : Source/Common/mem_man.cpp
	@ echo Info: Compile Source/Common/mem_man.cpp
	@ $(cpp) $(cc_flags) -o $(edit_obj)mem_man.obj Source/Common/mem_man.cpp

$(edit_obj)emacs_init.obj : Source/Common/emacs_init.cpp
	@ echo Info: Compile Source/Common/emacs_init.cpp
	@ $(cpp) $(cc_flags) -o $(edit_obj)emacs_init.obj Source/Common/emacs_init.cpp

$(edit_obj)metacomm.obj : Source/Common/metacomm.cpp
	@ echo Info: Compile Source/Common/metacomm.cpp
	@ $(cpp) $(cc_flags) -o $(edit_obj)metacomm.obj Source/Common/metacomm.cpp

$(edit_obj)minibuf.obj : Source/Common/minibuf.cpp
	@ echo Info: Compile Source/Common/minibuf.cpp
	@ $(cpp) $(cc_flags) -o $(edit_obj)minibuf.obj Source/Common/minibuf.cpp

$(edit_obj)mlispars.obj : Source/Common/mlispars.cpp
	@ echo Info: Compile Source/Common/mlispars.cpp
	@ $(cpp) $(cc_flags) -o $(edit_obj)mlispars.obj Source/Common/mlispars.cpp

$(edit_obj)mlispexp.obj : Source/Common/mlispexp.cpp
	@ echo Info: Compile Source/Common/mlispexp.cpp
	@ $(cpp) $(cc_flags) -o $(edit_obj)mlispexp.obj Source/Common/mlispexp.cpp

$(edit_obj)mlisproc.obj : Source/Common/mlisproc.cpp
	@ echo Info: Compile Source/Common/mlisproc.cpp
	@ $(cpp) $(cc_flags) -o $(edit_obj)mlisproc.obj Source/Common/mlisproc.cpp

$(edit_obj)mlprintf.obj : Source/Common/mlprintf.cpp
	@ echo Info: Compile Source/Common/mlprintf.cpp
	@ $(cpp) $(cc_flags) -o $(edit_obj)mlprintf.obj Source/Common/mlprintf.cpp

$(edit_obj)ndbm.obj : Source/Common/ndbm.cpp
	@ echo Info: Compile Source/Common/ndbm.cpp
	@ $(cpp) $(cc_flags) -o $(edit_obj)ndbm.obj Source/Common/ndbm.cpp

$(edit_obj)options.obj : Source/Common/options.cpp
	@ echo Info: Compile Source/Common/options.cpp
	@ $(cpp) $(cc_flags) -o $(edit_obj)options.obj Source/Common/options.cpp

$(edit_obj)osfile.obj : Source/Common/osfile.cpp
	@ echo Info: Compile Source/Common/osfile.cpp
	@ $(cpp) $(cc_flags) -o $(edit_obj)osfile.obj Source/Common/osfile.cpp

$(edit_obj)osinter.obj : Source/Common/osinter.cpp
	@ echo Info: Compile Source/Common/osinter.cpp
	@ $(cpp) $(cc_flags) -o $(edit_obj)osinter.obj Source/Common/osinter.cpp

$(edit_obj)profile.obj : Source/Common/profile.cpp
	@ echo Info: Compile Source/Common/profile.cpp
	@ $(cpp) $(cc_flags) -o $(edit_obj)profile.obj Source/Common/profile.cpp

$(edit_obj)ptyopen_linux.obj : Source/Unix/ptyopen_linux.cpp
	@ echo Info: Compile Source/Unix/ptyopen_linux.cpp
	@ $(cpp) $(cc_flags) -o $(edit_obj)ptyopen_linux.obj Source/Unix/ptyopen_linux.cpp

$(edit_obj)ptyopen_svr4.obj : Source/Unix/ptyopen_svr4.cpp
	@ echo Info: Compile Source/Unix/ptyopen_svr4.cpp
	@ $(cpp) $(cc_flags) -o $(edit_obj)ptyopen_svr4.obj Source/Unix/ptyopen_svr4.cpp

$(edit_obj)ptyopen_bsd.obj : Source/Unix/ptyopen_bsd.cpp
	@ echo Info: Compile Source/Unix/ptyopen_bsd.cpp
	@ $(cpp) $(cc_flags) -o $(edit_obj)ptyopen_bsd.obj Source/Unix/ptyopen_bsd.cpp

$(edit_obj)emacs_proc.obj : Source/Common/emacs_proc.cpp
	@ echo Info: Compile Source/Common/emacs_proc.cpp
	@ $(cpp) $(cc_flags) -o $(edit_obj)emacs_proc.obj Source/Common/emacs_proc.cpp

$(edit_obj)queue.obj : Source/Common/queue.cpp
	@ echo Info: Compile Source/Common/queue.cpp
	@ $(cpp) $(cc_flags) -o $(edit_obj)queue.obj Source/Common/queue.cpp

$(edit_obj)save_env.obj : Source/Common/save_env.cpp
	@ echo Info: Compile Source/Common/save_env.cpp
	@ $(cpp) $(cc_flags) -o $(edit_obj)save_env.obj Source/Common/save_env.cpp

$(edit_obj)search.obj : Source/Common/search.cpp
	@ echo Info: Compile Source/Common/search.cpp
	@ $(cpp) $(cc_flags) -o $(edit_obj)search.obj Source/Common/search.cpp

$(edit_obj)search_extended_algorithm.obj : Source/Common/search_extended_algorithm.cpp
	@ echo Info: Compile Source/Common/search_extended_algorithm.cpp
	@ $(cpp) $(cc_flags) -o $(edit_obj)search_extended_algorithm.obj Source/Common/search_extended_algorithm.cpp

$(edit_obj)search_extended_parser.obj : Source/Common/search_extended_parser.cpp
	@ echo Info: Compile Source/Common/search_extended_parser.cpp
	@ $(cpp) $(cc_flags) -o $(edit_obj)search_extended_parser.obj Source/Common/search_extended_parser.cpp

$(edit_obj)search_interface.obj : Source/Common/search_interface.cpp
	@ echo Info: Compile Source/Common/search_interface.cpp
	@ $(cpp) $(cc_flags) -o $(edit_obj)search_interface.obj Source/Common/search_interface.cpp

$(edit_obj)search_simple_algorithm.obj : Source/Common/search_simple_algorithm.cpp
	@ echo Info: Compile Source/Common/search_simple_algorithm.cpp
	@ $(cpp) $(cc_flags) -o $(edit_obj)search_simple_algorithm.obj Source/Common/search_simple_algorithm.cpp

$(edit_obj)search_simple_engine.obj : Source/Common/search_simple_engine.cpp
	@ echo Info: Compile Source/Common/search_simple_engine.cpp
	@ $(cpp) $(cc_flags) -o $(edit_obj)search_simple_engine.obj Source/Common/search_simple_engine.cpp

$(edit_obj)simpcomm.obj : Source/Common/simpcomm.cpp
	@ echo Info: Compile Source/Common/simpcomm.cpp
	@ $(cpp) $(cc_flags) -o $(edit_obj)simpcomm.obj Source/Common/simpcomm.cpp

$(edit_obj)subproc.obj : Source/Common/subproc.cpp
	@ echo Info: Compile Source/Common/subproc.cpp
	@ $(cpp) $(cc_flags) -o $(edit_obj)subproc.obj Source/Common/subproc.cpp

$(edit_obj)string_map.obj : Source/Common/string_map.cpp
	@ echo Info: Compile Source/Common/string_map.cpp
	@ $(cpp) $(cc_flags) -o $(edit_obj)string_map.obj Source/Common/string_map.cpp

$(edit_obj)syntax.obj : Source/Common/syntax.cpp
	@ echo Info: Compile Source/Common/syntax.cpp
	@ $(cpp) $(cc_flags) -o $(edit_obj)syntax.obj Source/Common/syntax.cpp

$(edit_obj)term.obj : Source/Common/term.cpp Include/Common/tt.h
	@ echo Info: Compile Source/Common/term.cpp
	@ $(cpp) $(cc_flags) -o $(edit_obj)term.obj Source/Common/term.cpp

$(edit_obj)trm_ansi.obj : Source/Unix/trm_ansi.cpp Include/Common/tt.h
	@ echo Info: Compile Source/Unix/trm_ansi.cpp
	@ $(cpp) $(cc_flags) -o $(edit_obj)trm_ansi.obj Source/Unix/trm_ansi.cpp

$(edit_obj)undo.obj : Source/Common/undo.cpp
	@ echo Info: Compile Source/Common/undo.cpp
	@ $(cpp) $(cc_flags) -o $(edit_obj)undo.obj Source/Common/undo.cpp

$(edit_obj)variable.obj : Source/Common/variable.cpp
	@ echo Info: Compile Source/Common/variable.cpp
	@ $(cpp) $(cc_flags) -o $(edit_obj)variable.obj Source/Common/variable.cpp

$(edit_obj)varthunk.obj : Source/Common/varthunk.cpp
	@ echo Info: Compile Source/Common/varthunk.cpp
	@ $(cpp) $(cc_flags) -o $(edit_obj)varthunk.obj Source/Common/varthunk.cpp

$(edit_obj)unixcomm.obj : Source/Unix/unixcomm.cpp
	@ echo Info: Compile Source/Unix/unixcomm.cpp
	@ $(cpp) $(cc_flags) -o $(edit_obj)unixcomm.obj Source/Unix/unixcomm.cpp

$(edit_obj)unixfile.obj : Source/Unix/unixfile.cpp
	@ echo Info: Compile Source/Unix/unixfile.cpp
	@ $(cpp) $(cc_flags) -o $(edit_obj)unixfile.obj Source/Unix/unixfile.cpp

$(edit_obj)unix_ext_func.obj : Source/Unix/unix_ext_func.cpp
	@ echo Info: Compile Source/Unix/unix_ext_func.cpp
	@ $(cpp) $(cc_flags) -o $(edit_obj)unix_ext_func.obj Source/Unix/unix_ext_func.cpp

$(edit_obj)windman.obj : Source/Common/windman.cpp
	@ echo Info: Compile Source/Common/windman.cpp
	@ $(cpp) $(cc_flags) -o $(edit_obj)windman.obj Source/Common/windman.cpp

$(edit_obj)window.obj : Source/Common/window.cpp
	@ echo Info: Compile Source/Common/window.cpp
	@ $(cpp) $(cc_flags) -o $(edit_obj)window.obj Source/Common/window.cpp

##################################################################################
#                                                                                #
#                                                                                #
#          Clean                                                                 #
#                                                                                #
#                                                                                #
##################################################################################
clean::
	rm -f $(edit_obj)*.obj
	rm -f $(edit_exe)mll-2-db
	rm -f $(edit_exe)dbadd $(edit_exe)dbcreate $(edit_exe)dblist $(edit_exe)dbdel $(edit_exe)dbprint
