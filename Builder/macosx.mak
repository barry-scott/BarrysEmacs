#
#	macosx.mak
#
#	build emacs for Mac OS X
#
PYTHON=python${PYTHON_VERSION}

BEMACS_BIN_DIR=$(BUILDER_TOP_DIR)/Kits/MacOSX/pkg/Barry's Emacs-Devel.app/Contents/MacOS
BEMACS_LIB_DIR=$(BUILDER_TOP_DIR)/Kits/MacOSX/pkg/Barry's Emacs-Devel.app/Contents/Resources/emacs_library
BEMACS_DOC_DIR=$(BUILDER_TOP_DIR)/Kits/MacOSX/pkg/Barry's Emacs-Devel.app/Contents/Resources/documentation

usage:
	@ echo "Usage: make -f macosx.mak build"
	@ echo "Usage: make -f macosx.mak clean"
	exit 1

build: brand editor mlisp describe language quick_info docs MacOSX_pkg

brand:
	$(PYTHON) brand_version.py version_info.txt ..

editor:
	@ echo Info: Building BEmacs images...
	cd ../Editor && ./build-macosx.sh all
	@ echo Info: Building BEmacs application...
	cd ../Editor/PyQtBEmacs && ./build-macosx.sh --package
	@ echo Info: Copy bemacs-cli...
	cp ../Editor/exe-cli-bemacs/bemacs-cli "$(BEMACS_BIN_DIR)"; chmod ugo=rx "$(BEMACS_BIN_DIR)/bemacs-cli"
	@ echo Info: Copy db utils...
	cp ../Editor/exe-utils/dbadd	"$(BEMACS_BIN_DIR)"; chmod ugo=rx "$(BEMACS_BIN_DIR)/dbadd"
	cp ../Editor/exe-utils/dbcreate	"$(BEMACS_BIN_DIR)"; chmod ugo=rx "$(BEMACS_BIN_DIR)/dbcreate"
	cp ../Editor/exe-utils/dbdel	"$(BEMACS_BIN_DIR)"; chmod ugo=rx "$(BEMACS_BIN_DIR)/dbdel"
	cp ../Editor/exe-utils/dbprint	"$(BEMACS_BIN_DIR)"; chmod ugo=rx "$(BEMACS_BIN_DIR)/dbprint"
	cp ../Editor/exe-utils/dblist	"$(BEMACS_BIN_DIR)"; chmod ugo=rx "$(BEMACS_BIN_DIR)/dblist"
	cp ../Editor/exe-utils/mll2db	"$(BEMACS_BIN_DIR)"; chmod ugo=rx "$(BEMACS_BIN_DIR)/mll2db"

mlisp:
	@ echo Info: Copying Mlisp files...
	cp -f ../MLisp/emacsinit.ml	"$(BEMACS_LIB_DIR)"; chmod ugo=r "$(BEMACS_LIB_DIR)/emacsinit.ml"
	cp -f ../MLisp/emacs_profile.ml	"$(BEMACS_LIB_DIR)"; chmod ugo=r "$(BEMACS_LIB_DIR)/emacs_profile.ml"
	cd ../MLisp; $(PYTHON) create_library.py common,unix "$(BEMACS_LIB_DIR)/emacslib" ../Editor/exe-utils
	chmod ugo=r "$(BEMACS_LIB_DIR)"/emacslib.*

describe:
	@ echo Info: Making describe...
	@ "../Editor/exe-utils/dbcreate" "$(BEMACS_LIB_DIR)/emacsdesc" -c
	@ "../Editor/exe-utils/mll2db" ../Describe/em_desc.mll "$(BEMACS_LIB_DIR)/emacsdesc"
	chmod ugo=r "$(BEMACS_LIB_DIR)"/emacsdesc.*

language:
	@ echo Info: Making language...
	@ "../Editor/exe-utils/dbcreate" "$(BEMACS_LIB_DIR)/emacslang" -c
	@ "../Editor/exe-utils/mll2db" ../Language/language.mll "$(BEMACS_LIB_DIR)/emacslang"
	chmod ugo=r "$(BEMACS_LIB_DIR)"/emacslang.*

quick_info:
	@ echo Info: Making quick info...
	@ "../Editor/exe-utils/dbcreate" "$(BEMACS_LIB_DIR)/emacs_qinfo_c" -c
	@ "../Editor/exe-utils/mll2db" ../Describe/qi_cc.mll "$(BEMACS_LIB_DIR)/emacs_qinfo_c"
	chmod ugo=r "$(BEMACS_LIB_DIR)"/emacs_qinfo_c.*

docs:
	@ echo Info: Copying documentation...
	cp -f ../Kits/readme.txt "$(BEMACS_DOC_DIR)";chmod ugo=r "$(BEMACS_DOC_DIR)/readme.txt"
	cp -f ../Editor/PyQtBEmacs/bemacs.png "$(BEMACS_DOC_DIR)"; chmod ugo=r "$(BEMACS_DOC_DIR)"/*.png
	cp -f ../HTML/*.html	"$(BEMACS_DOC_DIR)"; chmod ugo=r "$(BEMACS_DOC_DIR)"/*.html
	cp -f ../HTML/*.gif	"$(BEMACS_DOC_DIR)"; chmod ugo=r "$(BEMACS_DOC_DIR)"/*.gif
	cp -f ../HTML/*.css	"$(BEMACS_DOC_DIR)"; chmod ugo=r "$(BEMACS_DOC_DIR)"/*.css
	cp -f ../HTML/*.js	"$(BEMACS_DOC_DIR)"; chmod ugo=r "$(BEMACS_DOC_DIR)"/*.js

MacOSX_pkg:
	@ echo Info: MacOSX package creation...
	cd ../Kits/MacOSX; bash ./make-macosx-kit.sh

clean:
	cd ../Editor && export BUILD_KIT_DIR="$(BEMACS_LIB_DIR)"; ./build-macosx.sh clean
	rm -rf "$(BEMACS_DOC_DIR)"
	rm -rf "$(BEMACS_LIB_DIR)"
	rm -rf ../Kits/MacOSX/tmp
	rm -rf ../Kits/MacOSX/pkg
