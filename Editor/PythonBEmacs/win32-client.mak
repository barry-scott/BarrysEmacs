#
#	win32.mak bemacs
#
all: be.ico build_app


APPNAME=bemacs
APPTYPE=run		# run or run_w
PYTHONPATH=$(BEMACS_PYTHONPATH)

SOURCES= \
	bemacs_main.py \
	be_app.py \
	be_exceptions.py \
	be_frame.py \
	be_ids.py \
	be_images.py \
	be_platform_specific.py \
	be_platform_win32_specific.py \
	be_preferences.py

IMAGES = \
	toolbar_images/editcopy.png \
	toolbar_images/editcut.png \
	toolbar_images/editpaste.png \
	bemacs.png

bemacs_main.py: be_client.py
	copy be_client.py bemacs_main.py

bemacs.rc: be.rc bemacs.ico
	copy be.rc bemacs.rc

bemacs.ico: ..\Source\Windows\Resources\win_emacs.ico
	copy ..\Source\Windows\Resources\win_emacs.ico bemacs.ico >NUL

be_images.py: make_be_images.py $(IMAGES)
	$(PYTHON) -u make_be_images.py be_images.py $(IMAGES) 

check: checkstop

clean::
	if exist *.pyc del *.pyc
	if exist bin rmdir bin /s /q
	if exist be_images.py del be_images.py
	if exist bemacs_main.py del bemacs_main.py
	if exist bemacs.rc del bemacs.rc
	if exist bemacs.ico del bemacs.ico

!include <meinc_installer.mak>
