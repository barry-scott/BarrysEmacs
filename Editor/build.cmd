python setup.py win32 Makefile.mak
if not errorlevel 1 nmake -e -f Makefile.mak %*
