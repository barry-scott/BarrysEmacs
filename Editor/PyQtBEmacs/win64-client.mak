#
#	win64.mak bemacs
#
all: bemacs.ico build_app

APPNAME=bemacs
APPTYPE=run_w		# run or run_w
PYTHONPATH=$(BEMACS_PYTHONPATH)

SOURCES= \
	bemacs_main.py

bemacs_main.py: be_client.py
	copy be_client.py bemacs_main.py

bemacs.ico: ..\Source\Windows\Resources\win_emacs.ico
	copy ..\Source\Windows\Resources\win_emacs.ico bemacs.ico >NUL

check: checkstop

clean::
	if exist *.pyc del *.pyc
	if exist bin rmdir bin /s /q
	if exist bemacs_main.py del bemacs_main.py
	if exist bemacs.rc del bemacs.rc
	if exist bemacs.ico del bemacs.ico

!include <meinc_installer.mak>
