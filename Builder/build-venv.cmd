@echo off
setlocal

set requirements_file=windows-requirements.txt

"%PYTHON%" -m colour_text "<>info Info:<> Clean up"
if exist venv.tmp rmdir /s /q venv.tmp

"%PYTHON%" -m colour_text "<>info Info:<> Create venv for %PYTHON%"
"%PYTHON%" -m venv venv.tmp

"%PYTHON%" -m colour_text "<>info Info:<> Install requirements"

venv.tmp\Scripts\python.exe -m pip install --upgrade pip setuptools
venv.tmp\Scripts\python.exe -m pip install -r %requirements_file%
venv.tmp\Scripts\python.exe -m pip list
endlocal
