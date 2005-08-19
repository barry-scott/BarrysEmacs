;
;	msvc60_system_files.iss
;

; begin VC system files
Source: "vc6redist\mfc42.dll";		DestDir: "{sys}"; Flags: restartreplace uninsneveruninstall regserver
Source: "vc6redist\msvcp60.dll";	DestDir: "{sys}"; Flags: restartreplace uninsneveruninstall
Source: "vc6redist\msvcirt.dll";	DestDir: "{sys}"; Flags: restartreplace uninsneveruninstall
Source: "vc6redist\msvcrt.dll";		DestDir: "{sys}"; Flags: restartreplace uninsneveruninstall
; end VC system files
