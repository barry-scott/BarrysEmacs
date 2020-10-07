if "%1" == "--enable-debug" (
    shift
    %PYTHON% setup.py win64 all Makefile --enable-debug
) else (
    %PYTHON% setup.py win64 all Makefile
)
    if errorlevel 1 goto :eof

nmake /nologo -e -f Makefile %1 %2 %3 %4 %5
