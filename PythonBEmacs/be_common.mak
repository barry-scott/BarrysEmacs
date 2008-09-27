#
#	be_common.mak
#
be_version.py: be_version.py.template ../Builder/brand_version.py ../Builder/version.info
	$(PYTHON) -u make_be_version.py

be_images.py: make_be_images.py
	$(PYTHON) -u make_be_images.py

run:
	$(PYTHON) -u be_main.py

check:
	$(PYTHON) -c "import be_pychecker;import be_main;be_pychecker.report()"

clean::
	rm -f be_version.py
	rm -f be_images.py
