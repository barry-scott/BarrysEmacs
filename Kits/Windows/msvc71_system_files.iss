;
;	msvc71_system_files.iss
;

; begin VC system files
Source: "vc71redist\mfc71.dll";		DestDir: "{sys}"; Flags: restartreplace uninsneveruninstall regserver
Source: "vc71redist\msvcp71.dll";	DestDir: "{sys}"; Flags: restartreplace uninsneveruninstall
Source: "vc71redist\msvcr71.dll";	DestDir: "{sys}"; Flags: restartreplace uninsneveruninstall
; end VC system files
