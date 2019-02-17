#
#	makefile for bemacs
#	starts in .\Builder
#

#################################################################################
#
#
#	Win64 build
#
#
#################################################################################

KITSRC=$(BUILDER_TOP_DIR)\Kits\Windows
KITROOT=$(KITSRC)\tmp
KITFILES=$(KITSRC)\tmp\kitfiles

#
build: brand kitdir editor mlisp describe quick_info language inno_kit

brand:
	$(PYTHON) brand_version.py version_info.txt ..

kitdir:
	if not exist $(KITFILES) mkdir $(KITFILES)

inno_kit: $(KITROOT)\Output\setup.exe

#
#	Rule to run INNO's IDE to allow debugging of the installation
#
inno_debug:
	"c:\Program Files (x86)\Inno Setup 5\Compil32.exe" $(KITSRC)\bemacs.iss

$(KITROOT)\Output\setup.exe: $(KITSRC)\setup_kit_files.py
	cd $(KITSRC) && .\build-windows.cmd $(INSTALL)

editor: kitdir $(KITROOT)\bemacs_server.exe

$(KITROOT)\bemacs_server.exe:
	@echo Info: Build Editor $(BUILDER_CFG_BUILD_TYPE)
	cd $(BUILDER_TOP_DIR)\Editor && .\build-windows.cmd
	cd $(BUILDER_TOP_DIR)\Editor\PyQtBEmacs && .\build-windows.cmd $(KITFILES) all
	echo Info: Copying images...
	copy $(BUILDER_TOP_DIR)\Editor\exe-utils\dbadd.exe $(KITFILES) >NUL
	copy $(BUILDER_TOP_DIR)\Editor\exe-utils\dbcreate.exe $(KITFILES) >NUL
	copy $(BUILDER_TOP_DIR)\Editor\exe-utils\dbdel.exe $(KITFILES) >NUL
	copy $(BUILDER_TOP_DIR)\Editor\exe-utils\dblist.exe $(KITFILES) >NUL
	copy $(BUILDER_TOP_DIR)\Editor\exe-utils\dbprint.exe $(KITFILES) >NUL
	copy $(BUILDER_TOP_DIR)\Editor\exe-utils\mll2db.exe $(KITFILES) >NUL

mlisp:
	echo Info: Copying Mlisp files...
	if not exist $(KITFILES)\emacs_library mkdir $(KITFILES)\emacs_library
	cd $(BUILDER_TOP_DIR)\mlisp && %PYTHON% create_library.py common $(KITFILES)\emacs_library\emacslib $(KITFILES)
	cd $(BUILDER_TOP_DIR)\mlisp && copy emacs_profile.ml $(KITFILES)\emacs_library
	cd $(BUILDER_TOP_DIR)\mlisp && copy emacsinit.ml $(KITFILES)\emacs_library

describe:
	echo Info: Making describe...
	if not exist $(KITFILES)\emacs_library mkdir $(KITFILES)\emacs_library
	$(KITFILES)\dbcreate $(KITFILES)\emacs_library\EmacsDesc -c
	$(KITFILES)\mll2db $(BUILDER_TOP_DIR)\describe\em_desc.mll $(KITFILES)\emacs_library\EmacsDesc

language:
	echo Info: Making language...
	if not exist $(KITFILES)\emacs_library mkdir $(KITFILES)\emacs_library
	$(KITFILES)\dbcreate $(KITFILES)\emacs_library\EmacsLang -c
	$(KITFILES)\mll2db $(BUILDER_TOP_DIR)\language\language.mll $(KITFILES)\emacs_library\EmacsLang

quick_info:
	echo Info: Making quick info...
	if not exist $(KITFILES)\emacs_library mkdir $(KITFILES)\emacs_library
	$(KITFILES)\dbcreate $(KITFILES)\emacs_library\emacs_qinfo_c -c
	$(KITFILES)\mll2db $(BUILDER_TOP_DIR)\describe\qi_cc.mll $(KITFILES)\emacs_library\emacs_qinfo_c

clean:
	cd $(BUILDER_TOP_DIR)\editor && .\build-windows clean
	if exist $(KITROOT) rmdir /s /q  $(KITROOT)
