#
#	unix.mak
#
#	build emacs for Mac OS X, FreeBSD, OpenBSD and Linux
#
PYTHON=python

ifeq (${BUILDER_CFG_PLATFORM},FreeBSD)
BEMACS_DOC_DIR=$(BUILDER_TOP_DIR)/Kits/FreeBSD/pkg
BEMACS_LIB_DIR=$(BUILDER_TOP_DIR)/Kits/FreeBSD/pkg
BUILD_KIT_DIR=$(BUILDER_TOP_DIR)/Kits/FreeBSD

else
ifeq (${BUILDER_CFG_PLATFORM},OpenBSD)
BEMACS_DOC_DIR=$(BUILDER_TOP_DIR)/Kits/OpenBSD/pkg
BEMACS_LIB_DIR=$(BUILDER_TOP_DIR)/Kits/OpenBSD/pkg
BUILD_KIT_DIR=$(BUILDER_TOP_DIR)/Kits/OpenBSD

else
ifeq (${BUILDER_CFG_PLATFORM},MacOSX)
BEMACS_DOC_DIR=$(BUILDER_TOP_DIR)/Kits/MacOSX/bemacs-kit
BEMACS_LIB_DIR=$(BUILDER_TOP_DIR)/Kits/MacOSX/bemacs-kit
BUILD_KIT_DIR=$(BUILDER_TOP_DIR)/Kits/MacOSX

else
ifeq (${BUILDER_CFG_PLATFORM},Linux-Fedora)
BEMACS_DOC_DIR=$(BUILDER_TOP_DIR)/Kits/Linux/RPM/BUILD
BEMACS_LIB_DIR=$(BUILDER_TOP_DIR)/Kits/Linux/RPM/ROOT/usr/local/bemacs
BUILD_KIT_DIR=$(BUILDER_TOP_DIR)/Kits/Linux/RPM

else
ifeq (${BUILDER_CFG_PLATFORM},Linux-Ubuntu)
BEMACS_DOC_DIR=$(BUILDER_TOP_DIR)/Kits/Linux/DPKG/tree/usr/local/share/doc/bemacs
BEMACS_LIB_DIR=$(BUILDER_TOP_DIR)/Kits/Linux/DPKG/tree/usr/local/bemacs
BUILD_KIT_DIR=$(BUILDER_TOP_DIR)/Kits/Linux/DPKG

endif
endif
endif
endif
endif

usage:
	@ echo "Usage: make -f unix.mak build"
	@ echo "Usage: make -f unix.mak clean"
	exit 1

build: build_$(BUILDER_CFG_PLATFORM)

brand:
	$(PYTHON) brand_version.py version_info.txt ..

build_Linux-Fedora: build_Linux Fedora_rpm

build_Linux-Ubuntu: build_Linux Debian_pkg

build_Linux: brand $(BEMACS_DOC_DIR) $(BEMACS_LIB_DIR) editor mlisp describe language quick_info docs
	@ echo Info: Linux kitting

build_FreeBSD: brand $(BEMACS_DOC_DIR) $(BEMACS_LIB_DIR) editor mlisp describe language quick_info docs FreeBSD_pkg
	@ echo Info: FreeBSD kitting

build_OpenBSD: brand $(BEMACS_DOC_DIR) $(BEMACS_LIB_DIR) editor mlisp describe language quick_info docs OpenBSD_pkg
	@ echo Info: OpenBSD kitting

build_MacOSX: brand $(BEMACS_DOC_DIR) $(BEMACS_LIB_DIR) editor mlisp describe language quick_info docs MacOSX_pkg

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
	cd ../MLisp; $(PYTHON) create_library.py common,unix,motif $(BEMACS_LIB_DIR)/emacslib $(BEMACS_LIB_DIR)

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
	cp -f ../Kits/readme.txt $(BEMACS_DOC_DIR);chmod ugo=r $(BEMACS_DOC_DIR)/readme.txt
	cp -f ../HTML/*.html $(BEMACS_DOC_DIR); chmod ugo=r $(BEMACS_DOC_DIR)/*.html
	cp -f ../HTML/*.gif $(BEMACS_DOC_DIR); chmod ugo=r $(BEMACS_DOC_DIR)/*.gif
	cp -f ../HTML/*.css $(BEMACS_DOC_DIR); chmod ugo=r $(BEMACS_DOC_DIR)/*.css
	cp -f ../HTML/*.js $(BEMACS_DOC_DIR); chmod ugo=r $(BEMACS_DOC_DIR)/*.js

EDIT_SCRIPT_EXPR=-e /%SRCDIR%/s!!$(BEMACS_LIB_DIR)! -e /%HTMLDIR%/s!!$(BEMACS_DOC_DIR)! -e /%KITDIR%/s!!$(BEMACS_DOC_DIR)/!
FreeBSD_pkg:	$(BEMACS_DOC_DIR) $(BEMACS_LIB_DIR) $(BUILD_KIT_DIR)/PLIST.template
	@ echo Info: FreeBSD package creation...
	sed $(EDIT_SCRIPT_EXPR) <$(BUILD_KIT_DIR)/PLIST.template >$(BEMACS_LIB_DIR)/PLIST
	cd $(BUILD_KIT_DIR); cp COMMENT DESC $(BEMACS_LIB_DIR)
	cd $(BEMACS_DOC_DIR); for docfile in *.css *.html *.js *.gif; do echo $$docfile >>PLIST; done
	cd $(BEMACS_LIB_DIR); /usr/sbin/pkg_create -o editors/bemacs -f PLIST -d DESC -c COMMENT bemacs.i386.tgz

OpenBSD_pkg:	$(BEMACS_DOC_DIR) $(BEMACS_LIB_DIR) $(BUILD_KIT_DIR)/PLIST.template
	@ echo Info: OpenBSD package creation...
	sed $(EDIT_SCRIPT_EXPR) <$(BUILD_KIT_DIR)/PLIST.template >$(BEMACS_LIB_DIR)/PLIST
	cd $(BUILD_KIT_DIR); cp -f COMMENT DESC $(BEMACS_LIB_DIR)
	cd $(BEMACS_DOC_DIR); for docfile in *.css *.html *.js *.gif; do echo $$docfile >>PLIST; done
	cd $(BEMACS_LIB_DIR); /usr/sbin/pkg_create -f PLIST -d DESC -c COMMENT bemacs.i386.tgz

MacOSX_pkg:	$(BEMACS_DOC_DIR) $(BEMACS_LIB_DIR)
	@ echo Info: MacOSX package creation...
	cd $(BUILD_KIT_DIR); chmod +x ./make-macosx-kit.sh; ./make-macosx-kit.sh

Fedora_rpm: Fedora_rpm_$(BUILDER_CFG_PLATFORM)

Fedora_rpm_Linux-Fedora: Fedora_rpm_Any

Fedora_rpm_Any:
	@ echo Info: ${BUILDER_CFG_PLATFORM} RPM creation...
	grep ^macrofiles: /usr/lib/rpm/rpmrc |sed -e s!~/.rpmmacros!$(BUILD_KIT_DIR)/rpmmacros! >$(BUILD_KIT_DIR)/rpmrc
	echo %_topdir $(BUILD_KIT_DIR) >$(BUILD_KIT_DIR)/rpmmacros
	echo BuildRoot: $(BUILD_KIT_DIR)/ROOT >$(BUILD_KIT_DIR)/SPECS/bemacs-with-build-root.spec
	cat $(BUILD_KIT_DIR)/SPECS/bemacs.spec >>$(BUILD_KIT_DIR)/SPECS/bemacs-with-build-root.spec
	cd $(BEMACS_DOC_DIR); for docfile in *.css *.html *.js *.gif; do echo %doc $$docfile >>$(BUILD_KIT_DIR)/SPECS/bemacs-with-build-root.spec; done
	cd $(BEMACS_LIB_DIR); chmod u+w *
	rpmbuild --rcfile=/usr/lib/rpm/rpmrc:$(BUILD_KIT_DIR)/rpmrc -bb $(BUILD_KIT_DIR)/SPECS/bemacs-with-build-root.spec

Debian_pkg:
	@ echo Info: ${BUILDER_CFG_PLATFORM} DPKG creation...
	cd ${BUILD_KIT_DIR}; chmod +x ./create-dpkg.sh; ./create-dpkg.sh

clean:
	cd ../Editor; PATH=.:$$PATH; export BUILD_KIT_DIR=$(BEMACS_LIB_DIR); ./build clean
	rm -rf $(BEMACS_DOC_DIR)
	rm -rf $(BEMACS_LIB_DIR)
