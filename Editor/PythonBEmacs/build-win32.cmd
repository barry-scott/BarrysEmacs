call build-client-win32.cmd %*
call build-server-win32.cmd %*

del %OUTPUTDIR%\pywintypes26.dll
del %OUTPUTDIR%\win32api.pyd
