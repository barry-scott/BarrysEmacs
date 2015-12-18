#
#	win64-server.mak bemacs
#
all: bemacs.ico build_app


APPNAME=bemacs_server
APPTYPE=run_w		# run or run_w
PYTHONPATH=$(BEMACS_PYTHONPATH)

SOURCES= \
	bemacs_server_main.py \
	be_images.py

IMAGES = \
	toolbar_images/editcopy.png \
	toolbar_images/editcut.png \
	toolbar_images/editpaste.png \
	bemacs.png


bemacs_server_main.py: be_main.py
	copy be_main.py bemacs_server_main.py

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
	if exist bemacs.ico del bemacs.ico

!include <meinc_installer.mak>
