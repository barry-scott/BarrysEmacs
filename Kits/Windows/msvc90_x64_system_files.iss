;
;	msvc90_x64_system_files.iss
;

; begin VC system files
Source: "c:\Program Files (x86)\Microsoft Visual Studio 9.0\vc\redist\amd64\Microsoft.VC90.CRT\Microsoft.VC90.CRT.manifest"; DestDir: "{app}"
Source: "c:\Program Files (x86)\Microsoft Visual Studio 9.0\vc\redist\amd64\Microsoft.VC90.CRT\msvcm90.dll"; DestDir: "{app}"
Source: "c:\Program Files (x86)\Microsoft Visual Studio 9.0\vc\redist\amd64\Microsoft.VC90.CRT\msvcp90.dll"; DestDir: "{app}"
Source: "c:\Program Files (x86)\Microsoft Visual Studio 9.0\vc\redist\amd64\Microsoft.VC90.CRT\msvcr90.dll"; DestDir: "{app}"
; end VC system files
