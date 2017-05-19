#
#	linux.mak
#
#	build emacs for Linux
#
ifndef PYTHON
$(error PYTHON is not defined)
endif

ifdef DESTDIR
BEMACS_ROOT_DIR=$(DESTDIR)

INSTALL_BEMACS_DOC_DIR=/usr/share/bemacs/doc
INSTALL_BEMACS_LIB_DIR=/usr/share/bemacs/lib
INSTALL_BEMACS_BIN_DIR=/usr/bin

else
BEMACS_ROOT_DIR=$(BUILDER_TOP_DIR)/Kits/Linux/ROOT

INSTALL_BEMACS_DOC_DIR=/usr/local/share/bemacs/doc
INSTALL_BEMACS_LIB_DIR=/usr/local/lib/bemacs
INSTALL_BEMACS_BIN_DIR=/usr/local/bin

endif

BUILD_BEMACS_DOC_DIR=$(BEMACS_ROOT_DIR)$(INSTALL_BEMACS_DOC_DIR)
BUILD_BEMACS_LIB_DIR=$(BEMACS_ROOT_DIR)$(INSTALL_BEMACS_LIB_DIR)
BUILD_BEMACS_BIN_DIR=$(BEMACS_ROOT_DIR)$(INSTALL_BEMACS_BIN_DIR)

usage:
	@ echo "Usage: make -f unix.mak build"
	@ echo "Usage: make -f unix.mak build-cli"
	@ echo "Usage: make -f unix.mak clean"
	exit 1

build: build_$(BUILDER_CFG_PLATFORM)

build-cli: build_cli_$(BUILDER_CFG_PLATFORM)

brand:
	$(PYTHON) brand_version.py version_info.txt ..

build_Linux-Fedora: build_Linux # Fedora_rpm

build_Linux-Ubuntu: build_Linux # Debian_pkg

build_Linux-Debian: build_Linux # Debian_pkg

build_Linux: brand $(BUILD_BEMACS_DOC_DIR) $(BUILD_BEMACS_LIB_DIR) $(BUILD_BEMACS_BIN_DIR) bemacs-gui utils mlisp describe language quick_info docs
	@ echo Info: Linux kitting

build_cli_Linux-Fedora: build_cli_Linux # Fedora_rpm

build_cli_Linux-Ubuntu: build_cli_Linux # Debian_pkg

build_cli_Linux-Debian: build_cli_Linux # Debian_pkg

build_cli_Linux: brand $(BUILD_BEMACS_DOC_DIR) $(BUILD_BEMACS_LIB_DIR) $(BUILD_BEMACS_BIN_DIR)  ../Editor/exe-cli-bemacs/bemacs-cli utils mlisp describe language quick_info docs
	@ echo Info: Linux kitting

$(BUILD_BEMACS_DOC_DIR)::
	 if [ ! -e $@ ]; then mkdir -p $@; fi

$(BUILD_BEMACS_LIB_DIR)::
	 if [ ! -e $@ ]; then mkdir -p $@; fi

$(BUILD_BEMACS_BIN_DIR)::
	 if [ ! -e $@ ]; then mkdir -p $@; fi

bemacs-gui: bemacs-so bemacs-cli
	@ echo Info: Copy PyQtBEmacs...
	cd ../Editor/PyQtBEmacs && ./build-linux.sh $(BEMACS_ROOT_DIR) $(INSTALL_BEMACS_BIN_DIR) $(INSTALL_BEMACS_LIB_DIR) $(INSTALL_BEMACS_DOC_DIR)

bemacs-so:
	@ echo Info: Building BEmacs images...
	cd ../Editor && INSTALL_BEMACS_LIB_DIR=$(INSTALL_BEMACS_LIB_DIR) ./build-linux.sh all
	cp ../Editor/exe-pybemacs/_bemacs.so $(BUILD_BEMACS_LIB_DIR)
	cp ../Editor/exe-cli-bemacs/bemacs-cli $(BUILD_BEMACS_BIN_DIR)

 ../Editor/exe-cli-bemacs/bemacs-cli: 
	@ echo Info: Building BEmacs images...
	cd ../Editor && INSTALL_BEMACS_LIB_DIR=$(INSTALL_BEMACS_LIB_DIR) ./build-linux.sh --no-bemacs-gui all
	cp ../Editor/exe-cli-bemacs/bemacs-cli $(BUILD_BEMACS_BIN_DIR)

utils:
	@ echo Info: Copy db utils...
	cp ../Editor/exe-utils/dbadd	$(BUILD_BEMACS_BIN_DIR)/bemacs-dbadd
	cp ../Editor/exe-utils/dbcreate $(BUILD_BEMACS_BIN_DIR)/bemacs-dbcreate
	cp ../Editor/exe-utils/dbdel	$(BUILD_BEMACS_BIN_DIR)/bemacs-dbdel
	cp ../Editor/exe-utils/dbprint	$(BUILD_BEMACS_BIN_DIR)/bemacs-dbprint
	cp ../Editor/exe-utils/dblist	$(BUILD_BEMACS_BIN_DIR)/bemacs-dblist
	cp ../Editor/exe-utils/mll2db	$(BUILD_BEMACS_BIN_DIR)/bemacs-mll2db

mlisp:
	@ echo Info: Copying Mlisp files...
	cp -f ../MLisp/emacsinit.ml	$(BUILD_BEMACS_LIB_DIR); chmod ugo=r $(BUILD_BEMACS_LIB_DIR)/emacsinit.ml
	cp -f ../MLisp/emacs_profile.ml	$(BUILD_BEMACS_LIB_DIR); chmod ugo=r $(BUILD_BEMACS_LIB_DIR)/emacs_profile.ml
	cd ../MLisp; $(PYTHON) create_library.py common $(BUILD_BEMACS_LIB_DIR)/emacslib $(BUILD_BEMACS_BIN_DIR); chmod ugo=r $(BUILD_BEMACS_LIB_DIR)/emacslib.*

describe:
	@ echo Info: Making describe...
	@ $(BUILD_BEMACS_BIN_DIR)/bemacs-dbcreate $(BUILD_BEMACS_LIB_DIR)/emacsdesc -c
	@ $(BUILD_BEMACS_BIN_DIR)/bemacs-mll2db ../Describe/em_desc.mll $(BUILD_BEMACS_LIB_DIR)/emacsdesc; chmod ugo=r $(BUILD_BEMACS_LIB_DIR)/emacsdesc.*

language:
	@ echo Info: Making language...
	@ $(BUILD_BEMACS_BIN_DIR)/bemacs-dbcreate $(BUILD_BEMACS_LIB_DIR)/emacslang -c
	@ $(BUILD_BEMACS_BIN_DIR)/bemacs-mll2db ../Language/language.mll $(BUILD_BEMACS_LIB_DIR)/emacslang; chmod ugo=r $(BUILD_BEMACS_LIB_DIR)/emacslang.*

quick_info:
	@ echo Info: Making quick info...
	@ $(BUILD_BEMACS_BIN_DIR)/bemacs-dbcreate $(BUILD_BEMACS_LIB_DIR)/emacs_qinfo_c -c
	@ $(BUILD_BEMACS_BIN_DIR)/bemacs-mll2db ../Describe/qi_cc.mll $(BUILD_BEMACS_LIB_DIR)/emacs_qinfo_c; chmod ugo=r $(BUILD_BEMACS_LIB_DIR)/emacs_qinfo_c.*

docs:
	@ echo Info: Copying documentation...
	cp -f ../Kits/readme.txt $(BUILD_BEMACS_DOC_DIR);chmod ugo=r $(BUILD_BEMACS_DOC_DIR)/readme.txt
	cp -f ../Editor/PyQtBEmacs/bemacs.png "$(BUILD_BEMACS_DOC_DIR)"; chmod ugo=r "$(BUILD_BEMACS_DOC_DIR)"/*.png
	cp -f ../HTML/*.html $(BUILD_BEMACS_DOC_DIR); chmod ugo=r $(BUILD_BEMACS_DOC_DIR)/*.html
	cp -f ../HTML/*.gif $(BUILD_BEMACS_DOC_DIR); chmod ugo=r $(BUILD_BEMACS_DOC_DIR)/*.gif
	cp -f ../HTML/*.css $(BUILD_BEMACS_DOC_DIR); chmod ugo=r $(BUILD_BEMACS_DOC_DIR)/*.css
	cp -f ../HTML/*.js $(BUILD_BEMACS_DOC_DIR); chmod ugo=r $(BUILD_BEMACS_DOC_DIR)/*.js

Fedora_rpm: Fedora_rpm_$(BUILDER_CFG_PLATFORM)

Fedora_rpm_Linux-Fedora: Fedora_rpm_Any

Fedora_rpm_Any:
	@ echo Info: ${BUILDER_CFG_PLATFORM} RPM creation...
	grep ^macrofiles: /usr/lib/rpm/rpmrc |sed -e s!~/.rpmmacros!$(BUILD_KIT_DIR)/rpmmacros! >$(BUILD_KIT_DIR)/rpmrc
	echo %_topdir $(BUILD_KIT_DIR) >$(BUILD_KIT_DIR)/rpmmacros
	echo BuildRoot: $(BUILD_KIT_DIR)/ROOT >$(BUILD_KIT_DIR)/SPECS/bemacs-with-build-root.spec
	cat $(BUILD_KIT_DIR)/SPECS/bemacs.spec >>$(BUILD_KIT_DIR)/SPECS/bemacs-with-build-root.spec
	cd $(BUILD_BEMACS_DOC_DIR); for docfile in *.css *.html *.js *.gif; do echo %doc $$docfile >>$(BUILD_KIT_DIR)/SPECS/bemacs-with-build-root.spec; done
	cd $(BUILD_BEMACS_LIB_DIR); chmod u+w *
	rpmbuild --rcfile=/usr/lib/rpm/rpmrc:$(BUILD_KIT_DIR)/rpmrc -bb $(BUILD_KIT_DIR)/SPECS/bemacs-with-build-root.spec

Debian_pkg:
	@ echo Info: ${BUILDER_CFG_PLATFORM} DPKG creation...
	cd $(BUILD_KIT_DIR); chmod +x ./create-dpkg.sh; ./create-dpkg.sh

clean:
	cd ../Editor && ./build-linux.sh clean
	rm -rf $(BEMACS_ROOT_DIR)
