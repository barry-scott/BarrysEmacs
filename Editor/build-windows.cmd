if "%1" = "--enable-debug" (
    %PYTHON% setup.py win64 Makefile --enable-debug
) else (
    %PYTHON% setup.py win64 Makefile
)
    if errorlevel 1 goto :eof

nmake /nologo -e -f Makefile %*
