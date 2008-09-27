#
#	Mac OS X makefile for WorkBench
#
PYTHON=python2.5

all: be_version.py be_images.py

locale/en/LC_MESSAGES/pysvn_workbench.mo:
	./make-pot-file.sh
	./make-po-file.sh en
	./make-mo-files.sh locale

clean::	
	rm -rf locale/*

include be_common.mak
