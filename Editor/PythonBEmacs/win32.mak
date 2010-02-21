#
#	win32.mak bemacs
#
all: be.ico run build_app


APPNAME=be
APPTYPE=run_w		# run or run_w
PYTHONPATH=$(BEMACS_PYTHONPATH)

SOURCES= \
	be_app.py \
	be_exceptions.py \
	be_frame.py \
	be_ids.py \
	be_images.py \
	be_main.py \
	be_platform_specific.py \
	be_platform_win32_specific.py \
	be_preferences.py

IMAGES = \
	toolbar_images/editcopy.png \
	toolbar_images/editcut.png \
	toolbar_images/editpaste.png \
	bemacs.png


be.ico: ..\Source\Windows\Resources\win_emacs.ico
	copy ..\Source\Windows\Resources\win_emacs.ico be.ico >NUL

be_images.py: make_be_images.py $(IMAGES)
	$(PYTHON) -u make_be_images.py be_images.py $(IMAGES) 

check: checkstop

#
#	Make the run script
#
run: run_$(APPNAME).cmd

SCRIPT_NAME=run_$(APPNAME).cmd
$(SCRIPT_NAME): win32.mak
	echo setlocal > $(SCRIPT_NAME)
	echo set PYTHONPATH=$(PYTHONPATH) >> $(SCRIPT_NAME)
	echo python %CD%\$(APPNAME)_main.py %* >> $(SCRIPT_NAME)
	echo endlocal >> $(SCRIPT_NAME)

clean::
	if exist *.pyc del *.pyc
	if exist bin rmdir bin /s /q
	if exist be_images.py del be_images.py

!include <meinc_installer.mak>
