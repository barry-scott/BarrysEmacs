#
#	makefile WorkBench
#
all: build_bin

locale/en/LC_MESSAGES/pysvn_workbench.mo:
	./make-pot-file.sh
	./make-po-file.sh en
	./make-mo-files.sh locale

APPNAME=be
APPTYPE=run

PYTHONPATH=$(PYSVNLIB)

SOURCES= \
	be_app.py \
	be_exceptions.py \
	be_frame.py \
	be_ids.py \
	be_images.py \
	be_main.py \
	be_platform_specific.py \
	be_platform_unix_specific.py \
	be_preferences.py \
	be_version.py

include be_common.mak

PYCHECKER_OPTIONS=--no-shadobeuiltin
INSTALLER_OPTIONS=--force-ld-library-path

build_bin: build_app build_fixup

build_fixup:
	rm -f bin/support/readline.so

#check: checkstop

clean::
	rm -f locale/en/LC_MESSAGES/pysvn_workbench.mo
	rm -f .pycheckrc
	rm -rf bin
	rm -rf *.pyc
	rm -rf be_version.py

#include $(PYCHECKER_DIR)/pychecker.mak
include $(MEINC_INSTALLER_DIR)/meinc_installer.mak
