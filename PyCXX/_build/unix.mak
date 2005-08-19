#
#	unix.mak
#
#	build emacs for FreeBSD and Linux
#

BUILD_KIT_DIR=$(BUILD_DIR)/Unix_Kit/$(BUILD_OS)

ifeq  ($(BUILD_OS),freebsd)
BEMACS_LIB_DIR=$(BUILD_DIR)/Unix_Kit/freebsd
else
BEMACS_DOC_DIR=$(BUILD_DIR)/Unix_Kit/linux/BUILD
BEMACS_LIB_DIR=$(BUILD_DIR)/Unix_Kit/linux/usr/local/bemacs
endif

usage:
	@ echo "Usage: make -f unix.mak [freebsd|linux] BUILD_CFG=cfg BUILD_OS=os BUILD_DIR=dir"
	exit 1

linux: editor mlisp describe language quick_info docs rpm
	@ echo Info: Linux kitting


freebsd: editor mlisp describe language quick_info pkg
	@ echo Info: FreeBSD kitting


editor:
	@ echo Info: Building BEmacs images...
	cd ../Editor; PATH=.:$$PATH; export BUILD_KIT_DIR=$(BEMACS_LIB_DIR); ./build install

mlisp:
	@ echo Info: Copying Mlisp files...
	cp -f ../MLisp/emacsinit.ml	$(BEMACS_LIB_DIR)
	chmod ugo=r $(BEMACS_LIB_DIR)/emacsinit.ml
	cp -f ../MLisp/emacs_profile.ml	$(BEMACS_LIB_DIR)
	chmod ugo=r $(BEMACS_LIB_DIR)/emacs_profile.ml
	cd ../MLisp; python create_library.py common,unix,motif $(BEMACS_LIB_DIR)/emacslib $(BEMACS_LIB_DIR)

describe:
	@ echo Info: Making describe...
	@ $(BEMACS_LIB_DIR)/dbcreate $(BEMACS_LIB_DIR)/emacsdesc -c
	@ $(BEMACS_LIB_DIR)/mll-2-db ../Describe/em_desc.mll $(BEMACS_LIB_DIR)/emacsdesc

language:
	@ echo Info: Making language...
	@ $(BEMACS_LIB_DIR)/dbcreate $(BEMACS_LIB_DIR)/emacslang -c
	@ $(BEMACS_LIB_DIR)/mll-2-db ../Language/language.mll $(BEMACS_LIB_DIR)/emacslang

quick_info:
	@ echo Info: Making quick info...
	@ $(BEMACS_LIB_DIR)/dbcreate $(BEMACS_LIB_DIR)/emacs_qinfo_c -c
	@ $(BEMACS_LIB_DIR)/mll-2-db ../Describe/qi_cc.mll $(BEMACS_LIB_DIR)/emacs_qinfo_c

docs:
	@ echo Info: Copying documentation...
	cp -f ../Kit/readme.txt $(BEMACS_DOC_DIR)
	cp -f ../HTML/*.htm $(BEMACS_DOC_DIR)
	cp -f ../HTML/*.gif $(BEMACS_DOC_DIR)
	cp -f ../HTML/*.css $(BEMACS_DOC_DIR)
	cp -f ../HTML/*.js $(BEMACS_DOC_DIR)
	chmod ugo=r $(BEMACS_DOC_DIR)/*

EDIT_SCRIPT_EXPR=-e /%SRCDIR%/s!!$(BUILD_KIT_DIR)! -e /%HTMLDIR%/s!!$(BUILD_DIR)/HTML/! -e /%KITDIR%/s!!$(BUILD_DIR)/Kit/!
pkg:
	@ echo Info: FreeBSD package creation...
	sed $(EDIT_SCRIPT_EXPR) <$(BEMACS_LIB_DIR)/PLIST.template >$(BEMACS_LIB_DIR)/PLIST
	cd $(BUILD_KIT_DIR); /usr/sbin/pkg_create -f PLIST -d DESC -c COMMENT bemacs.i386.tgz

rpm:
	@ echo Info: Linux RPM creation...
	grep ^macrofiles: /usr/lib/rpm/rpmrc |sed -e s!~/.rpmmacros!$(BUILD_KIT_DIR)/rpmmacros! >$(BUILD_KIT_DIR)/rpmrc
	echo %_topdir $(BUILD_KIT_DIR) >$(BUILD_KIT_DIR)/rpmmacros
	rpm --rcfile /usr/lib/rpm/rpmrc:$(BUILD_KIT_DIR)/rpmrc -bb --buildroot $(BUILD_KIT_DIR) $(BUILD_KIT_DIR)/SPECS/bemacs.spec
	cp -f $(BUILD_KIT_DIR)/RPMS/i386/bemacs-*.*-*.i386.rpm $(BUILD_KIT_DIR)/RPMS/i386/bemacs.i386.rpm
