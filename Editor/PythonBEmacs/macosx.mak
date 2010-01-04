#
#	Mac OS X makefile for WorkBench
#
PYTHON=python${PYTHON_VERSION}

all: be_images.py

locale/en/LC_MESSAGES/pysvn_workbench.mo:
	./make-pot-file.sh
	./make-po-file.sh en
	./make-mo-files.sh locale

clean::	
	rm -rf locale/*

include be_common.mak
