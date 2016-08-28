%PYTHON% setup.py win64 Makefile
    if errorlevel 1 goto :eof

nmake /nologo -e -f Makefile %*
