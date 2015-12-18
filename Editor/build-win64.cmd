%PYTHON% setup.py win64 Makefile.mak
if not errorlevel 1 nmake /nologo -e -f Makefile.mak %*
