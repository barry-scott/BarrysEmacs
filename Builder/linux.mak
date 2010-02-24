#
#	linux.mak
#
#	build emacs for Linux
#
PYTHON=python${PYTHON_VERSION}

ifeq (${BUILDER_CFG_PLATFORM},Linux-Fedora)
BEMACS_DOC_DIR=$(BUILDER_TOP_DIR)/Kits/Linux/RPM/ROOT/usr/local/share/doc/bemacs
BEMACS_LIB_DIR=$(BUILDER_TOP_DIR)/Kits/Linux/RPM/ROOT/usr/local/lib/bemacs
BEMACS_BIN_DIR=$(BUILDER_TOP_DIR)/Kits/Linux/RPM/ROOT/usr/local/bin
BUILD_KIT_DIR=$(BUILDER_TOP_DIR)/Kits/Linux/RPM

else
ifeq (${BUILDER_CFG_PLATFORM},Linux-Ubuntu)
BEMACS_DOC_DIR=$(BUILDER_TOP_DIR)/Kits/Linux/DPKG/tree/usr/local/share/doc/bemacs
BEMACS_LIB_DIR=$(BUILDER_TOP_DIR)/Kits/Linux/DPKG/tree/usr/local/lib/bemacs
BUILD_KIT_DIR=$(BUILDER_TOP_DIR)/Kits/Linux/DPKG

endif
endif

usage:
	@ echo "Usage: make -f unix.mak build"
	@ echo "Usage: make -f unix.mak clean"
	exit 1

build: build_$(BUILDER_CFG_PLATFORM)

brand:
	$(PYTHON) brand_version.py version_info.txt ..

build_Linux-Fedora: build_Linux # Fedora_rpm

build_Linux-Ubuntu: build_Linux # Debian_pkg

build_Linux: brand $(BEMACS_DOC_DIR) $(BEMACS_LIB_DIR) $(BEMACS_BIN_DIR) editor bemacs mlisp describe language quick_info docs
	@ echo Info: Linux kitting

$(BEMACS_DOC_DIR)::
	 if [ ! -e $@ ]; then mkdir -p $@; fi

$(BEMACS_LIB_DIR)::
	 if [ ! -e $@ ]; then mkdir -p $@; fi

$(BEMACS_BIN_DIR)::
	 if [ ! -e $@ ]; then mkdir -p $@; fi

editor:
	@ echo Info: Building BEmacs images...
	cd ../Editor && ./build.sh all
	cp ../Editor/obj-pybemacs/_bemacs.so ${BEMACS_BIN_DIR}
	@ echo Info: Copy db utils...
	cp ../Editor/obj-utils/dbadd	$(BEMACS_BIN_DIR)
	cp ../Editor/obj-utils/dbcreate $(BEMACS_BIN_DIR)
	cp ../Editor/obj-utils/dbdel	$(BEMACS_BIN_DIR)
	cp ../Editor/obj-utils/dbprint	$(BEMACS_BIN_DIR)
	cp ../Editor/obj-utils/dblist	$(BEMACS_BIN_DIR)
	cp ../Editor/obj-utils/mll2db	$(BEMACS_BIN_DIR)


bemacs:
	@ echo Info: Copy PythonBEmacs...
	cd ../Editor/PythonBEmacs && ./build-linux.sh ${BEMACS_BIN_DIR}

mlisp:
	@ echo Info: Copying Mlisp files...
	cp -f ../MLisp/emacsinit.ml	$(BEMACS_LIB_DIR); chmod ugo=r $(BEMACS_LIB_DIR)/emacsinit.ml
	cp -f ../MLisp/emacs_profile.ml	$(BEMACS_LIB_DIR); chmod ugo=r $(BEMACS_LIB_DIR)/emacs_profile.ml
	cd ../MLisp; $(PYTHON) create_library.py common,unix $(BEMACS_LIB_DIR)/emacslib $(BEMACS_BIN_DIR)

describe:
	@ echo Info: Making describe...
	@ $(BEMACS_BIN_DIR)/dbcreate $(BEMACS_LIB_DIR)/emacsdesc -c
	@ $(BEMACS_BIN_DIR)/mll2db ../Describe/em_desc.mll $(BEMACS_LIB_DIR)/emacsdesc

language:
	@ echo Info: Making language...
	@ $(BEMACS_BIN_DIR)/dbcreate $(BEMACS_LIB_DIR)/emacslang -c
	@ $(BEMACS_BIN_DIR)/mll2db ../Language/language.mll $(BEMACS_LIB_DIR)/emacslang

quick_info:
	@ echo Info: Making quick info...
	@ $(BEMACS_BIN_DIR)/dbcreate $(BEMACS_LIB_DIR)/emacs_qinfo_c -c
	@ $(BEMACS_BIN_DIR)/mll2db ../Describe/qi_cc.mll $(BEMACS_LIB_DIR)/emacs_qinfo_c

docs:
	@ echo Info: Copying documentation...
	cp -f ../Kits/readme.txt $(BEMACS_DOC_DIR);chmod ugo=r $(BEMACS_DOC_DIR)/readme.txt
	cp -f ../HTML/*.html $(BEMACS_DOC_DIR); chmod ugo=r $(BEMACS_DOC_DIR)/*.html
	cp -f ../HTML/*.gif $(BEMACS_DOC_DIR); chmod ugo=r $(BEMACS_DOC_DIR)/*.gif
	cp -f ../HTML/*.css $(BEMACS_DOC_DIR); chmod ugo=r $(BEMACS_DOC_DIR)/*.css
	cp -f ../HTML/*.js $(BEMACS_DOC_DIR); chmod ugo=r $(BEMACS_DOC_DIR)/*.js

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
	cd ../Editor && ./build.sh clean
	rm -rf $(BEMACS_DOC_DIR)
	rm -rf $(BEMACS_LIB_DIR)
	rm -rf $(BEMACS_BIN_DIR)
