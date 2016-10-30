#
#	Mac OS X makefile for BEmacs
#
PYTHON=python${PYTHON_VERSION}
PYTHONPATH=${BUILDER_TOP_DIR}/Editor/PyQtBEmacs:${BUILDER_TOP_DIR}/Editor/exe-pybemacs 

all: dist/be_main.app 

dist/be_main.app: be_images.py be_version.py
	PYTHONPATH=$(PYTHONPATH) $(PYTHON) setup.py py2app --xref

clean::	
	rm -rf build
	rm -rf dist

locale/en/LC_MESSAGES/pysvn_workbench.mo:
	./make-pot-file.sh
	./make-po-file.sh en
	./make-mo-files.sh locale

clean::	
	rm -rf locale/*

include be_common.mak
