#
#	makefile for bemacs
#	starts in .\Builder
#

#################################################################################
#
#
#	Win32 build
#
#
#################################################################################

KITROOT=$(BUILDER_TOP_DIR)\kit\win32

#
build: brand kitdir editor mlisp describe quick_info language inno_kit

brand:
	python brand_version.py version_info.txt ..

kitdir:
	if not exist $(KITROOT) mkdir $(KITROOT)
	if not exist $(KITROOT)\emacs_library mkdir $(KITROOT)\emacs_library
	if not exist $(KITROOT)\documentation mkdir $(KITROOT)\Documentation

inno_kit: $(KITROOT)\Output\setup.exe

#
#	Rule to run INNO's IDE to allow debugging of the installation
#
inno_debug:
	"c:\Program Files\Inno Setup 4\Compil32.exe" ..\kit\bemacs.iss

$(KITROOT)\Output\setup.exe: ..\kit\bemacs.iss
	copy ..\kit\msvc$(MSVC_VERSION)_system_files.iss ..\kit\msvc_system_files.iss
	"c:\Program Files\Inno Setup 4\ISCC.exe" ..\kit\bemacs.iss
	..\kit\copy-setup.cmd ..\kit\Output\setup.exe ..\kit

editor: kitdir "$(KITROOT)\be.exe"

"$(KITROOT)\be.exe":
	@echo Info: Build Editor $(BUILDER_CFG_BUILD_TYPE)
	cd $(BUILDER_TOP_DIR)\editor && .\build-win32
	cd $(BUILDER_TOP_DIR)\editor\PythonBEmacs && .\build-win32
	echo Info: Copying images...
	copy  "$(BUILDER_TOP_DIR)\Editor\obj-utils\dbadd.exe" "$(KITROOT)" >NUL
	copy  "$(BUILDER_TOP_DIR)\Editor\obj-utils\dbcreate.exe" "$(KITROOT)" >NUL
	copy  "$(BUILDER_TOP_DIR)\Editor\obj-utils\dbdel.exe" "$(KITROOT)" >NUL
	copy  "$(BUILDER_TOP_DIR)\Editor\obj-utils\dblist.exe" "$(KITROOT)" >NUL
	copy  "$(BUILDER_TOP_DIR)\Editor\obj-utils\dbprint.exe" "$(KITROOT)" >NUL
	copy  "$(BUILDER_TOP_DIR)\Editor\obj-utils\mll2db.exe" "$(KITROOT)" >NUL

mlisp:
	echo Info: Copying Mlisp files...
	cd $(BUILDER_TOP_DIR)\mlisp && python create_library.py common $(KITROOT)\emacs_library\emacslib $(KITROOT)
	cd $(BUILDER_TOP_DIR)\mlisp && copy emacs_profile.ml $(KITROOT)\emacs_library
	cd $(BUILDER_TOP_DIR)\mlisp && copy emacsinit.ml $(KITROOT)\emacs_library

describe:
	echo Info: Making describe...
	"$(KITROOT)\dbcreate" "$(KITROOT)\emacs_library\EmacsDesc" -c
	"$(KITROOT)\mll2db" "$(BUILDER_TOP_DIR)\describe\em_desc.mll" "$(KITROOT)\emacs_library\EmacsDesc"

language:
	echo Info: Making language...
	"$(KITROOT)\dbcreate" "$(KITROOT)\emacs_library\EmacsLang" -c
	"$(KITROOT)\mll2db" "$(BUILDER_TOP_DIR)\language\language.mll" "$(KITROOT)\emacs_library\EmacsLang"

quick_info:
	echo Info: Making quick info...
	"$(KITROOT)\dbcreate" "$(KITROOT)\emacs_library\emacs_qinfo_c" -c
	"$(KITROOT)\mll2db" "$(BUILDER_TOP_DIR)\describe\qi_cc.mll" "$(KITROOT)\emacs_library\emacs_qinfo_c"

clean:
	cd $(BUILDER_TOP_DIR)\editor && .\build-win32 clean
	if exist $(KITROOT) rmdir /s /q  $(KITROOT)
