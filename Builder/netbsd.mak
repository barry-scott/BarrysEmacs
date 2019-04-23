#
#	netbsd.mak
#
#	build emacs for NetBSD
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
BEMACS_ROOT_DIR=$(BUILDER_TOP_DIR)/Kits/NetBSD/ROOT

INSTALL_BEMACS_DOC_DIR=/usr/local/share/bemacs/doc
INSTALL_BEMACS_LIB_DIR=/usr/local/lib/bemacs
INSTALL_BEMACS_BIN_DIR=/usr/local/bin

endif

BUILD_BEMACS_DOC_DIR=$(BEMACS_ROOT_DIR)$(INSTALL_BEMACS_DOC_DIR)
BUILD_BEMACS_LIB_DIR=$(BEMACS_ROOT_DIR)$(INSTALL_BEMACS_LIB_DIR)
BUILD_BEMACS_BIN_DIR=$(BEMACS_ROOT_DIR)$(INSTALL_BEMACS_BIN_DIR)

usage:
	@ echo "Usage: gmake -f netbsd.mak build-gui"
	@ echo "Usage: gmake -f netbsd.mak clean-gui"
	@ echo "Usage: gmake -f netbsd.mak build-cli"
	@ echo "Usage: gmake -f netbsd.mak clean-cli"
	exit 1

build-gui: brand $(BUILD_BEMACS_DOC_DIR) $(BUILD_BEMACS_LIB_DIR) $(BUILD_BEMACS_BIN_DIR) bemacs-gui bemacs-cli utils mlisp describe language quick_info docs
	@ echo Info: NetBSD kitting

build-cli: brand $(BUILD_BEMACS_DOC_DIR) $(BUILD_BEMACS_LIB_DIR) $(BUILD_BEMACS_BIN_DIR) bemacs-cli utils mlisp describe language quick_info docs
	@ echo Info: NetBSD kitting

clean-gui:
	cd ../Editor && ./build-netbsd.sh gui clean
	rm -rf $(BEMACS_ROOT_DIR)

clean-cli:
	cd ../Editor && ./build-netbsd.sh cli clean
	rm -rf $(BEMACS_ROOT_DIR)

brand:
	$(PYTHON) brand_version.py version_info.txt ..

$(BUILD_BEMACS_DOC_DIR)::
	 if [ ! -e $@ ]; then mkdir -p $@; fi

$(BUILD_BEMACS_LIB_DIR)::
	 if [ ! -e $@ ]; then mkdir -p $@; fi

$(BUILD_BEMACS_BIN_DIR)::
	 if [ ! -e $@ ]; then mkdir -p $@; fi

bemacs-gui: bemacs-so
	@ echo Info: Copy PyQtBEmacs...
	cd ../Editor/PyQtBEmacs && ./build-netbsd.sh $(BEMACS_ROOT_DIR) $(INSTALL_BEMACS_BIN_DIR) $(INSTALL_BEMACS_LIB_DIR) $(INSTALL_BEMACS_DOC_DIR)

bemacs-so:
	@ echo Info: Building BEmacs GUI images...
	cd ../Editor && INSTALL_BEMACS_LIB_DIR=$(INSTALL_BEMACS_LIB_DIR) ./build-netbsd.sh gui all
	cp ../Editor/exe-pybemacs/_bemacs.so $(BUILD_BEMACS_LIB_DIR)

bemacs-cli: 
	@ echo Info: Building BEmacs CLI images...
	cd ../Editor && INSTALL_BEMACS_LIB_DIR=$(INSTALL_BEMACS_LIB_DIR) ./build-netbsd.sh cli all
	cp ../Editor/exe-cli-bemacs/bemacs-cli $(BUILD_BEMACS_BIN_DIR)

utils:
	@ echo Info: Copy db utils...
	cd ../Editor && INSTALL_BEMACS_LIB_DIR=$(INSTALL_BEMACS_LIB_DIR) ./build-netbsd.sh utils all
	cp ../Editor/exe-utils/dbadd	$(BUILD_BEMACS_BIN_DIR)/bemacs-dbadd
	cp ../Editor/exe-utils/dbcreate $(BUILD_BEMACS_BIN_DIR)/bemacs-dbcreate
	cp ../Editor/exe-utils/dbdel	$(BUILD_BEMACS_BIN_DIR)/bemacs-dbdel
	cp ../Editor/exe-utils/dbprint	$(BUILD_BEMACS_BIN_DIR)/bemacs-dbprint
	cp ../Editor/exe-utils/dblist	$(BUILD_BEMACS_BIN_DIR)/bemacs-dblist
	cp ../Editor/exe-utils/mll2db	$(BUILD_BEMACS_BIN_DIR)/bemacs-mll2db

mlisp: utils
	@ echo Info: Copying Mlisp files...
	cp -f ../MLisp/emacsinit.ml	$(BUILD_BEMACS_LIB_DIR); chmod ugo=r $(BUILD_BEMACS_LIB_DIR)/emacsinit.ml
	cp -f ../MLisp/emacs_profile.ml	$(BUILD_BEMACS_LIB_DIR); chmod ugo=r $(BUILD_BEMACS_LIB_DIR)/emacs_profile.ml
	rm -f $(BUILD_BEMACS_LIB_DIR)/emacslib.*
	cd ../MLisp; $(PYTHON) create_library.py common,unix $(BUILD_BEMACS_LIB_DIR)/emacslib $(BUILD_BEMACS_BIN_DIR); chmod ugo=r $(BUILD_BEMACS_LIB_DIR)/emacslib.*

describe: utils
	@ echo Info: Making describe...
	@ $(BUILD_BEMACS_BIN_DIR)/bemacs-dbcreate $(BUILD_BEMACS_LIB_DIR)/emacsdesc -c
	@ $(BUILD_BEMACS_BIN_DIR)/bemacs-mll2db ../Describe/em_desc.mll $(BUILD_BEMACS_LIB_DIR)/emacsdesc; chmod ugo=r $(BUILD_BEMACS_LIB_DIR)/emacsdesc.*

language: utils
	@ echo Info: Making language...
	@ $(BUILD_BEMACS_BIN_DIR)/bemacs-dbcreate $(BUILD_BEMACS_LIB_DIR)/emacslang -c
	@ $(BUILD_BEMACS_BIN_DIR)/bemacs-mll2db ../Language/language.mll $(BUILD_BEMACS_LIB_DIR)/emacslang; chmod ugo=r $(BUILD_BEMACS_LIB_DIR)/emacslang.*

quick_info: utils
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
