#
#	win32.mak bemacs
#
all: be.ico build_app


APPNAME=bemacs_server
APPTYPE=run_w		# run or run_w
PYTHONPATH=$(BEMACS_PYTHONPATH)

SOURCES= \
	bemacs_server_main.py \
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


bemacs_server_main.py: be_main.py
	copy be_main.py bemacs_server_main.py

bemacs_server.rc: be.rc bemacs_server.ico
	copy be.rc bemacs_server.rc

bemacs_server.ico: ..\Source\Windows\Resources\win_emacs.ico
	copy ..\Source\Windows\Resources\win_emacs.ico bemacs_server.ico >NUL

be_images.py: make_be_images.py $(IMAGES)
	$(PYTHON) -u make_be_images.py be_images.py $(IMAGES) 

check: checkstop

clean::
	if exist *.pyc del *.pyc
	if exist bin rmdir bin /s /q
	if exist be_images.py del be_images.py
	if exist bemacs_server_main.py del bemacs_server_main.py
	if exist bemacs_server.rc del bemacs_server.rc
	if exist bemacs_server.ico del bemacs_server.ico

!include <meinc_installer.mak>
