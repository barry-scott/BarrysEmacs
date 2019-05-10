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
	@ echo "Usage: make -f unix.mak build-gui"
	@ echo "Usage: make -f unix.mak clean-gui"
	@ echo "Usage: make -f unix.mak build-cli"
	@ echo "Usage: make -f unix.mak clean-cli"
	exit 1

build-gui: brand $(BUILD_BEMACS_DOC_DIR) $(BUILD_BEMACS_LIB_DIR) $(BUILD_BEMACS_BIN_DIR) bemacs-gui bemacs-cli utils mlisp describe quick_info docs
	@ echo Info: Linux kitting

build-cli: brand $(BUILD_BEMACS_DOC_DIR) $(BUILD_BEMACS_LIB_DIR) $(BUILD_BEMACS_BIN_DIR) bemacs-cli utils mlisp describe quick_info docs
	@ echo Info: Linux kitting

clean-gui:
	cd ../Editor && ./build-linux.sh $(SETUP_OPTIONS) gui clean
	rm -rf $(BEMACS_ROOT_DIR)

clean-cli:
	cd ../Editor && ./build-linux.sh $(SETUP_OPTIONS) cli clean
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
	cd ../Editor/PyQtBEmacs && ./build-linux.sh $(BEMACS_ROOT_DIR) $(INSTALL_BEMACS_BIN_DIR) $(INSTALL_BEMACS_LIB_DIR) $(INSTALL_BEMACS_DOC_DIR)

bemacs-so:
	@ echo Info: Building BEmacs GUI images...
	cd ../Editor && INSTALL_BEMACS_LIB_DIR=$(INSTALL_BEMACS_LIB_DIR) ./build-linux.sh $(SETUP_OPTIONS) gui all
	cp ../Editor/exe-pybemacs/_bemacs.so $(BUILD_BEMACS_LIB_DIR)

bemacs-cli: 
	@ echo Info: Building BEmacs CLI images...
	cd ../Editor && INSTALL_BEMACS_LIB_DIR=$(INSTALL_BEMACS_LIB_DIR) ./build-linux.sh $(SETUP_OPTIONS) cli all
	cp ../Editor/exe-cli-bemacs/bemacs-cli $(BUILD_BEMACS_BIN_DIR)

utils:
	@ echo Info: Copy db utils...
	cd ../Editor && INSTALL_BEMACS_LIB_DIR=$(INSTALL_BEMACS_LIB_DIR) ./build-linux.sh $(SETUP_OPTIONS) utils all
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
ifdef USE_SQLITE3
	cd ../MLisp; $(PYTHON) create_library.py common,unix $(BUILD_BEMACS_LIB_DIR)/emacslib --sqlite3 $(BUILD_BEMACS_BIN_DIR)
else
	cd ../MLisp; $(PYTHON) create_library.py common,unix $(BUILD_BEMACS_LIB_DIR)/emacslib $(BUILD_BEMACS_BIN_DIR)
endif
	chmod ugo=r $(BUILD_BEMACS_LIB_DIR)/emacslib.*

describe: utils
	@ echo Info: Making describe...
ifdef USE_SQLITE3
	@ $(PYTHON) ../Describe/create_describe_database.py ../Describe/em_desc.mll $(BUILD_BEMACS_LIB_DIR)/emacsdesc.db
else
	@ $(BUILD_BEMACS_BIN_DIR)/bemacs-dbcreate $(BUILD_BEMACS_LIB_DIR)/emacsdesc -c
	@ $(BUILD_BEMACS_BIN_DIR)/bemacs-mll2db ../Describe/em_desc.mll $(BUILD_BEMACS_LIB_DIR)/emacsdesc
endif
	chmod ugo=r $(BUILD_BEMACS_LIB_DIR)/emacsdesc.*

quick_info: utils
ifdef USE_SQLITE3
	@ $(PYTHON) ../Describe/create_describe_database.py ../Describe/qi_cc.mll $(BUILD_BEMACS_LIB_DIR)/emacs_qinfo_c.db
else
	@ echo Info: Making quick info...
	@ $(BUILD_BEMACS_BIN_DIR)/bemacs-dbcreate $(BUILD_BEMACS_LIB_DIR)/emacs_qinfo_c -c
	@ $(BUILD_BEMACS_BIN_DIR)/bemacs-mll2db ../Describe/qi_cc.mll $(BUILD_BEMACS_LIB_DIR)/emacs_qinfo_c
endif
	chmod ugo=r $(BUILD_BEMACS_LIB_DIR)/emacs_qinfo_c.*

docs:
	@ echo Info: Copying documentation...
	cp -f ../Kits/readme.txt $(BUILD_BEMACS_DOC_DIR);chmod ugo=r $(BUILD_BEMACS_DOC_DIR)/readme.txt
	cp -f ../Editor/PyQtBEmacs/bemacs.png "$(BUILD_BEMACS_DOC_DIR)"; chmod ugo=r "$(BUILD_BEMACS_DOC_DIR)"/*.png
	cp -f ../HTML/*.html $(BUILD_BEMACS_DOC_DIR); chmod ugo=r $(BUILD_BEMACS_DOC_DIR)/*.html
	cp -f ../HTML/*.gif $(BUILD_BEMACS_DOC_DIR); chmod ugo=r $(BUILD_BEMACS_DOC_DIR)/*.gif
	cp -f ../HTML/*.css $(BUILD_BEMACS_DOC_DIR); chmod ugo=r $(BUILD_BEMACS_DOC_DIR)/*.css
	cp -f ../HTML/*.js $(BUILD_BEMACS_DOC_DIR); chmod ugo=r $(BUILD_BEMACS_DOC_DIR)/*.js
