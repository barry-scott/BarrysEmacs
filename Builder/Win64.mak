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
KITROOT=$(KITSRC)\kitfiles

#
build: brand kitdir editor mlisp describe quick_info language # inno_kit

brand:
	$(PYTHON) brand_version.py version_info.txt ..

kitdir:
	if not exist $(KITROOT) mkdir $(KITROOT)

inno_kit: $(KITROOT)\Output\setup.exe

#
#	Rule to run INNO's IDE to allow debugging of the installation
#
inno_debug:
	"c:\Program Files (x86)\Inno Setup 5\Compil32.exe" $(KITSRC)\bemacs.iss

$(KITROOT)\Output\setup.exe: $(KITSRC)\bemacs.iss
	cd $(KITSRC) && .\build-win64

editor: kitdir $(KITROOT)\bemacs_server.exe

$(KITROOT)\bemacs_server.exe:
	@echo Info: Build Editor $(BUILDER_CFG_BUILD_TYPE)
	cd $(BUILDER_TOP_DIR)\editor && .\build-win64
	cd $(BUILDER_TOP_DIR)\editor\PyQtBEmacs && .\build-win64 $(KITROOT) all
	echo Info: Copying images...
	copy $(BUILDER_TOP_DIR)\Editor\obj-utils\dbadd.exe $(KITROOT) >NUL
	copy $(BUILDER_TOP_DIR)\Editor\obj-utils\dbcreate.exe $(KITROOT) >NUL
	copy $(BUILDER_TOP_DIR)\Editor\obj-utils\dbdel.exe $(KITROOT) >NUL
	copy $(BUILDER_TOP_DIR)\Editor\obj-utils\dblist.exe $(KITROOT) >NUL
	copy $(BUILDER_TOP_DIR)\Editor\obj-utils\dbprint.exe $(KITROOT) >NUL
	copy $(BUILDER_TOP_DIR)\Editor\obj-utils\mll2db.exe $(KITROOT) >NUL

mlisp:
	echo Info: Copying Mlisp files...
	if not exist $(KITROOT)\emacs_library mkdir $(KITROOT)\emacs_library
	cd $(BUILDER_TOP_DIR)\mlisp && python create_library.py common $(KITROOT)\emacs_library\emacslib $(KITROOT)
	cd $(BUILDER_TOP_DIR)\mlisp && copy emacs_profile.ml $(KITROOT)\emacs_library
	cd $(BUILDER_TOP_DIR)\mlisp && copy emacsinit.ml $(KITROOT)\emacs_library

describe:
	echo Info: Making describe...
	if not exist $(KITROOT)\emacs_library mkdir $(KITROOT)\emacs_library
	$(KITROOT)\dbcreate $(KITROOT)\emacs_library\EmacsDesc -c
	$(KITROOT)\mll2db $(BUILDER_TOP_DIR)\describe\em_desc.mll $(KITROOT)\emacs_library\EmacsDesc

language:
	echo Info: Making language...
	if not exist $(KITROOT)\emacs_library mkdir $(KITROOT)\emacs_library
	$(KITROOT)\dbcreate $(KITROOT)\emacs_library\EmacsLang -c
	$(KITROOT)\mll2db $(BUILDER_TOP_DIR)\language\language.mll $(KITROOT)\emacs_library\EmacsLang

quick_info:
	echo Info: Making quick info...
	if not exist $(KITROOT)\emacs_library mkdir $(KITROOT)\emacs_library
	$(KITROOT)\dbcreate $(KITROOT)\emacs_library\emacs_qinfo_c -c
	$(KITROOT)\mll2db $(BUILDER_TOP_DIR)\describe\qi_cc.mll $(KITROOT)\emacs_library\emacs_qinfo_c

clean:
	cd $(BUILDER_TOP_DIR)\editor && .\build-win64 clean
	if exist $(KITROOT) rmdir /s /q  $(KITROOT)
