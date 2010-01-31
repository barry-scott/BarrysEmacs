python setup.py >win32-tmp.mak
if not errorlevel 1 nmake -e -f win32-tmp.mak %*
