#
#	unix.mak
#
#	build emacs for FreeBSD, OpenBSD and Linux
#

BUILD_KIT_DIR=$(RB_WORKINGDIR)/Unix_Kit/$(RB_CFG_PLATFORM)

ifeq  (${RB_CFG_PLATFORM},FreeBSD)
BEMACS_DOC_DIR=$(RB_WORKINGDIR)/Unix_Kit/FreeBSD/pkg
BEMACS_LIB_DIR=$(RB_WORKINGDIR)/Unix_Kit/FreeBSD/pkg

else
ifeq  (${RB_CFG_PLATFORM},OpenBSD)
BEMACS_DOC_DIR=$(RB_WORKINGDIR)/Unix_Kit/OpenBSD/pkg
BEMACS_LIB_DIR=$(RB_WORKINGDIR)/Unix_Kit/OpenBSD/pkg

else
BEMACS_DOC_DIR=$(RB_WORKINGDIR)/Unix_Kit/Linux/BUILD
BEMACS_LIB_DIR=$(RB_WORKINGDIR)/Unix_Kit/Linux/usr/local/bemacs

endif
endif

usage:
	@ echo "Usage: make -f unix.mak"
	exit 1

build: build_$(RB_CFG_PLATFORM)


build_Linux: $(BEMACS_DOC_DIR) $(BEMACS_LIB_DIR) editor mlisp describe language quick_info docs rpm
	@ echo Info: Linux kitting

build_FreeBSD: $(BEMACS_DOC_DIR) $(BEMACS_LIB_DIR) editor mlisp describe language quick_info docs FreeBSD_pkg
	@ echo Info: FreeBSD kitting

build_OpenBSD: $(BEMACS_DOC_DIR) $(BEMACS_LIB_DIR) editor mlisp describe language quick_info docs OpenBSD_pkg
	@ echo Info: OpenBSD kitting

$(BEMACS_DOC_DIR)::
	 if [ ! -e $@ ]; then mkdir -p $@; fi

$(BEMACS_LIB_DIR)::
	 if [ ! -e $@ ]; then mkdir -p $@; fi

editor:
	@ echo Info: Building BEmacs images...
	cd ../Editor; PATH=.:$$PATH; export BUILD_KIT_DIR=$(BEMACS_LIB_DIR); ./build install

mlisp:
	@ echo Info: Copying Mlisp files...
	cp -f ../MLisp/emacsinit.ml	$(BEMACS_LIB_DIR); chmod ugo=r $(BEMACS_LIB_DIR)/emacsinit.ml
	cp -f ../MLisp/emacs_profile.ml	$(BEMACS_LIB_DIR); chmod ugo=r $(BEMACS_LIB_DIR)/emacs_profile.ml
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
	cp -f ../Kit/readme.txt $(BEMACS_DOC_DIR);chmod ugo=r $(BEMACS_DOC_DIR)/readme.txt
	cp -f ../HTML/*.htm $(BEMACS_DOC_DIR); chmod ugo=r $(BEMACS_DOC_DIR)/*.htm
	cp -f ../HTML/*.gif $(BEMACS_DOC_DIR); chmod ugo=r $(BEMACS_DOC_DIR)/*.gif
	cp -f ../HTML/*.css $(BEMACS_DOC_DIR); chmod ugo=r $(BEMACS_DOC_DIR)/*.css
	cp -f ../HTML/*.js $(BEMACS_DOC_DIR); chmod ugo=r $(BEMACS_DOC_DIR)/*.js

EDIT_SCRIPT_EXPR=-e /%SRCDIR%/s!!$(BEMACS_LIB_DIR)! -e /%HTMLDIR%/s!!$(BEMACS_DOC_DIR)! -e /%KITDIR%/s!!$(BEMACS_DOC_DIR)/!
FreeBSD_pkg:	$(BEMACS_DOC_DIR) $(BEMACS_LIB_DIR) $(BUILD_KIT_DIR)/PLIST.template
	@ echo Info: FreeBSD package creation...
	sed $(EDIT_SCRIPT_EXPR) <$(BUILD_KIT_DIR)/PLIST.template >$(BEMACS_LIB_DIR)/PLIST
	cd $(BUILD_KIT_DIR); cp COMMENT DESC $(BEMACS_LIB_DIR)
	cd $(BEMACS_DOC_DIR); for docfile in *.css *.htm *.js *.gif; do echo $$docfile >>PLIST; done
	cd $(BEMACS_LIB_DIR); /usr/sbin/pkg_create -o editors/bemacs -f PLIST -d DESC -c COMMENT bemacs.i386.tgz

OpenBSD_pkg:	$(BEMACS_DOC_DIR) $(BEMACS_LIB_DIR) $(BUILD_KIT_DIR)/PLIST.template
	@ echo Info: OpenBSD package creation...
	sed $(EDIT_SCRIPT_EXPR) <$(BUILD_KIT_DIR)/PLIST.template >$(BEMACS_LIB_DIR)/PLIST
	cd $(BUILD_KIT_DIR); cp -f COMMENT DESC $(BEMACS_LIB_DIR)
	cd $(BEMACS_DOC_DIR); for docfile in *.css *.htm *.js *.gif; do echo $$docfile >>PLIST; done
	cd $(BEMACS_LIB_DIR); /usr/sbin/pkg_create -f PLIST -d DESC -c COMMENT bemacs.i386.tgz


rpm:
	@ echo Info: Linux RPM creation...
	grep ^macrofiles: /usr/lib/rpm/rpmrc |sed -e s!~/.rpmmacros!$(BUILD_KIT_DIR)/rpmmacros! >$(BUILD_KIT_DIR)/rpmrc
	echo %_topdir $(BUILD_KIT_DIR) >$(BUILD_KIT_DIR)/rpmmacros
	echo BuildRoot: $(BUILD_KIT_DIR) >$(BUILD_KIT_DIR)/SPECS/bemacs-with-build-root.spec
	cat $(BUILD_KIT_DIR)/SPECS/bemacs.spec >>$(BUILD_KIT_DIR)/SPECS/bemacs-with-build-root.spec
	cd $(BEMACS_DOC_DIR); for docfile in *.css *.htm *.js *.gif; do echo %doc $$docfile >>$(BUILD_KIT_DIR)/SPECS/bemacs-with-build-root.spec; done
	rpm --rcfile=/usr/lib/rpm/rpmrc:$(BUILD_KIT_DIR)/rpmrc -bb $(BUILD_KIT_DIR)/SPECS/bemacs-with-build-root.spec
	cp -f $(BUILD_KIT_DIR)/RPMS/i386/bemacs-*.*-*.i386.rpm $(BUILD_KIT_DIR)/RPMS/i386/bemacs.i386.rpm

clean:
	cd ../Editor; PATH=.:$$PATH; export BUILD_KIT_DIR=$(BEMACS_LIB_DIR); ./build clean
	rm -rf $(BEMACS_DOC_DIR)
	rm -rf $(BEMACS_LIB_DIR)
