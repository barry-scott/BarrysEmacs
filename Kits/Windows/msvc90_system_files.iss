;
;	msvc90_system_files.iss
;

; begin VC system files
Source: "c:\Program Files\Microsoft Visual Studio 9.0\vc\redist\x86\Microsoft.VC90.CRT\Microsoft.VC90.CRT.manifest"; DestDir: "{app}"
Source: "c:\Program Files\Microsoft Visual Studio 9.0\vc\redist\x86\Microsoft.VC90.CRT\msvcm90.dll"; DestDir: "{app}"
Source: "c:\Program Files\Microsoft Visual Studio 9.0\vc\redist\x86\Microsoft.VC90.CRT\msvcp90.dll"; DestDir: "{app}"
Source: "c:\Program Files\Microsoft Visual Studio 9.0\vc\redist\x86\Microsoft.VC90.CRT\msvcr90.dll"; DestDir: "{app}"
; end VC system files
