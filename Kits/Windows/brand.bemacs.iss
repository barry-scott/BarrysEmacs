;
; -- bemacs.iss --
;

[Code]
function InitializeSetup() : Boolean;
var
    uninstall_string    : string;
    uninstall_image     : string;
    uninstall_params    : string;
    Error               : Integer;
    space_pos           : Integer;
    rc                  : Integer;
    rcb                 : Boolean;
begin
    Error := 0;
    rcb := RegQueryStringValue( HKLM,
        'SOFTWARE\Microsoft\Windows\CurrentVersion\Uninstall\Barry''s Emacs',
        'UninstallString', uninstall_string );
    if rcb then
    begin
        rc := MsgBox( 'An old version of Barry''s Emacs is installed.' #13 #13
                'It must be uninstalled before installing the this version' #13
                'Do you wish to uninstall it now?', mbConfirmation, MB_YESNO );
        if rc = idYes then
        begin
            space_pos := Pos( ' ', uninstall_string );
            uninstall_image := Copy( uninstall_string, 1, space_pos-1 );
            uninstall_params := Copy( uninstall_string, space_pos, 999 );
            rcb := Exec( uninstall_image, uninstall_params, ExpandConstant('{src}'), SW_SHOWNORMAL, ewWaitUntilTerminated, Error );
            if not rcb then
                MsgBox( 'Failed to run the uninstall procedure.' #13 #13
                    'Please uninstall the old Barry''s Emacs' #13
                    'and try this installation again.', mbError, MB_OK );
            if Error <> 0 then
                MsgBox( 'Failed to run the uninstall procedure.' #13 #13
                    'Please uninstall the old Barry''s Emacs' #13
                    'and try this installation again.', mbError, MB_OK );
        end;
    end;
    BringToFrontAndRestore;
    rcb := RegQueryStringValue( HKLM,
        'SOFTWARE\Microsoft\Windows\CurrentVersion\Uninstall\Barry''s Emacs',
        'UninstallString', uninstall_string );
    Result := not rcb;
    if not Result then
        rc := MsgBox(    'Quitting installation.' #13 #13
                'An old version of Barry''s Emacs is still installed.' #13
                'Run this installation again after the old kit has' #13
                'been uninstalled', mbInformation, MB_OK );
end;

[Setup]
AppName=Barry's Emacs 8
AppVerName=Barry's Emacs %(maturity)s%(major)s.%(minor)s
AppCopyright=Copyright (C) 1991-%(year)s Barry A. Scott
DefaultDirName={pf}\Barry Scott\Barry's Emacs 8
DefaultGroupName=Barry's Emacs 8
UninstallDisplayIcon={app}\bemacs.exe
ChangesAssociations=yes
DisableStartupPrompt=yes
InfoBeforeFile=info_before.txt
Compression=bzip/9

[Tasks]
Name: "option_register_emacs_open_ml"; Description: "Barry's Emacs will open .ML and .MLP files"
Name: "option_register_emacs_open_c_dont"; Description: "No association"; GroupDescription: "How should Barry's Emacs be associated with Cand C++ Source Files"; Flags: exclusive
Name: "option_register_emacs_open_c_one_type"; Description: "Associate using one file type"; GroupDescription: "How should Barry's Emacs be associated with Cand C++ Source Files"; Flags: exclusive
Name: "option_register_emacs_open_c_many_types"; Description: "Associate using multiple file types"; GroupDescription: "How should Barry's Emacs be associated with Cand C++ Source Files"; Flags: exclusive
Name: "option_desktop_icon"; Description: "Place Barry's Emacs icon on the Desktop"
Name: "option_start_menu_icon"; Description: "Place Barry's Emacs on the Start menu"
Name: "option_edit_with_bemacs"; Description: "Place Edit with Barry's Emacs on the Context menu"

[Run]
Filename: "{app}\bemacs.EXE"; Parameters: """{app}\readme.txt"""; Flags: nowait postinstall skipifsilent; Description: "View README.TXT"

[Files]

#include "msvc_system_files.iss"

Source: "..\Readme.txt"; DestDir: "{app}";

Source: "kitfiles\dbadd.exe"; DestDir: "{app}"
Source: "kitfiles\dbcreate.exe"; DestDir: "{app}"
Source: "kitfiles\dbdel.exe"; DestDir: "{app}"
Source: "kitfiles\dblist.exe"; DestDir: "{app}"
Source: "kitfiles\dbprint.exe"; DestDir: "{app}"
Source: "kitfiles\mll2db.exe"; DestDir: "{app}"

Source: "kitfiles\emacs_library\*"; DestDir: "{app}\emacs_library"

Source: "..\..\HTML\*.css";  DestDir: "{app}\Documentation";
Source: "..\..\HTML\*.html"; DestDir: "{app}\Documentation";
Source: "..\..\HTML\*.gif";  DestDir: "{app}\Documentation";
Source: "..\..\HTML\*.js";   DestDir: "{app}\Documentation";

Source: "kitfiles\bemacs.exe"; DestDir: "{app}"
Source: "kitfiles\bemacs.exe.manifest"; DestDir: "{app}"
Source: "kitfiles\bemacs_server.exe"; DestDir: "{app}"
Source: "kitfiles\bemacs_server.exe.manifest"; DestDir: "{app}"
Source: "kitfiles\support\*"; DestDir: "{app}\support"

[Icons]
Name: "{group}\Barry's Emacs"; Filename: "{app}\bemacs.exe"
Name: "{group}\Barry's Emacs Server"; Filename: "{app}\BEmacs_Server.exe"
Name: "{group}\Documentation"; Filename: "{app}\Documentation\emacs-documentation.html"
Name: "{group}\FAQ"; Filename: "{app}\documentation\bemacs-faq.html"
Name: "{group}\Readme"; Filename: "{app}\bemacs.exe"; Parameters: """{app}\readme.txt"""
Name: "{group}\Barry's Emacs Web Site"; Filename: "http://www.barrys-emacs.org";

;
;    Add an Emacs icon to the Desktop
;
Name: "{commondesktop}\Barry's Emacs 8"; Filename: "{app}\bemacs.exe"; Tasks: "option_desktop_icon"

;
;    Add an Emacs icon to the Start menu
;
Name: "{commonstartmenu}\Barry's Emacs 8"; Filename: "{app}\bemacs.exe"; Tasks: "option_start_menu_icon"

[Registry]
Root: HKCR; Subkey: "BarrysEmacs8Command"; ValueType: string; ValueData: "BEmacs Command"; Flags: uninsdeletekey
Root: HKCR; Subkey: "BarrysEmacs8Command\Shell\open\command"; ValueType: string; ValueData: """{app}\bemacs.exe"" /package=""%%1"""
Root: HKCR; Subkey: "BarrysEmacs8Command\DefaultIcon"; ValueType: string; ValueData: "{app}\bemacs.exe"

Root: HKCR; Subkey: "BarrysEmacs8MLisp"; ValueType: string; ValueData: "BEmacs MLisp"; Flags: uninsdeletekey
Root: HKCR; Subkey: "BarrysEmacs8MLisp\Shell\open\command"; ValueType: string; ValueData: """{app}\bemacs.exe"" ""%%1"""
Root: HKCR; Subkey: "BarrysEmacs8MLisp\DefaultIcon"; ValueType: string; ValueData: "{app}\bemacs.exe"

Root: HKCR; Subkey: "BarrysEmacs8Document"; ValueType: string; ValueData: "BEmacs"; Flags: uninsdeletekey
Root: HKCR; Subkey: "BarrysEmacs8Document\Shell\open\command"; ValueType: string; ValueData: """{app}\bemacs.exe"" ""%%1"""
Root: HKCR; Subkey: "BarrysEmacs8Document\DefaultIcon"; ValueType: string; ValueData: "{app}\bemacs.exe"

Root: HKCR; Subkey: "BarrysEmacs8DocumentII"; ValueType: string; ValueData: "BEmacs II"; Flags: uninsdeletekey
Root: HKCR; Subkey: "BarrysEmacs8DocumentII\Shell\open\command"; ValueType: string; ValueData: """{app}\bemacs.exe"" ""%%1"""
Root: HKCR; Subkey: "BarrysEmacs8DocumentII\DefaultIcon"; ValueType: string; ValueData: "{app}\bemacs.exe"

Root: HKCR; Subkey: "BarrysEmacs8DocumentIII"; ValueType: string; ValueData: "BEmacs III"; Flags: uninsdeletekey
Root: HKCR; Subkey: "BarrysEmacs8DocumentIII\Shell\open\command"; ValueType: string; ValueData: """{app}\bemacs.exe"" ""%%1"""
Root: HKCR; Subkey: "BarrysEmacs8DocumentIII\DefaultIcon"; ValueType: string; ValueData: "{app}\bemacs.exe"

Root: HKCR; Subkey: "BarrysEmacs8DocumentIV"; ValueType: string; ValueData: "BEmacs IV"; Flags: uninsdeletekey
Root: HKCR; Subkey: "BarrysEmacs8DocumentIV\Shell\open\command"; ValueType: string; ValueData: """{app}\bemacs.exe"" ""%%1"""
Root: HKCR; Subkey: "BarrysEmacs8DocumentIV\DefaultIcon"; ValueType: string; ValueData: "{app}\bemacs.exe"

Root: HKCR; Subkey: "BarrysEmacs8DocumentV"; ValueType: string; ValueData: "BEmacs V"; Flags: uninsdeletekey
Root: HKCR; Subkey: "BarrysEmacs8DocumentV\Shell\open\command"; ValueType: string; ValueData: """{app}\bemacs.exe"" ""%%1"""
Root: HKCR; Subkey: "BarrysEmacs8DocumentV\DefaultIcon"; ValueType: string; ValueData: "{app}\bemacs.exe"

;
;    Add the Edit with Barry's Emacs to the context menu
;

; option_edit_with_bemacs
Root: HKCR; Subkey: "*\shell\Edit with Barry's Emacs 8"; ValueType: string; ValueData: "Edit with &Barry's Emacs 8"; Flags: uninsdeletekey
Root: HKCR; Subkey: "*\shell\Edit with Barry's Emacs 8\command"; ValueType: string; ValueData: """{app}\bemacs.exe"" ""%%1"""

Root: HKCR; Subkey: "Drive\shell\Barry's Emacs Here 8"; ValueType: string; ValueData: "Barry's Emacs 8 &Here"; Flags: uninsdeletekey
Root: HKCR; Subkey: "Drive\shell\Barry's Emacs Here 8\command"; ValueType: string; ValueData: """{app}\bemacs.exe"" -cd ""%%1\.."""

Root: HKCR; Subkey: "Directory\shell\Barry's Emacs Here 8"; ValueType: string; ValueData: "Barry's Emacs 8 &Here"; Flags: uninsdeletekey
Root: HKCR; Subkey: "Directory\shell\Barry's Emacs Here 8\command"; ValueType: string; ValueData: """{app}\bemacs.exe"" -cd ""%%1"""

;
; have emacs open .ML files and .MLP files
;
Root: HKCR; SubKey: ".ml";  ValueType: string; ValueData: "BarrysEmacs8MLisp"; Tasks: "option_register_emacs_open_ml"; Flags: uninsdeletekey
Root: HKCR; SubKey: ".mlp"; ValueType: string; ValueData: "BarrysEmacs8Command"; Tasks: "option_register_emacs_open_ml"; Flags: uninsdeletekey

;
; register all the C and C++ file types for emacs to open
; either using one type or multiple
;
Root: HKCR; Subkey: ".h";   ValueType: string; ValueData: "BarrysEmacs8Document"; Tasks: "option_register_emacs_open_c_one_type"
Root: HKCR; Subkey: ".hh";  ValueType: string; ValueData: "BarrysEmacs8Document"; Tasks: "option_register_emacs_open_c_one_type"
Root: HKCR; Subkey: ".hpp"; ValueType: string; ValueData: "BarrysEmacs8Document"; Tasks: "option_register_emacs_open_c_one_type"
Root: HKCR; Subkey: ".hxx"; ValueType: string; ValueData: "BarrysEmacs8Document"; Tasks: "option_register_emacs_open_c_one_type"
Root: HKCR; Subkey: ".c";   ValueType: string; ValueData: "BarrysEmacs8Document"; Tasks: "option_register_emacs_open_c_one_type"
Root: HKCR; Subkey: ".cc";  ValueType: string; ValueData: "BarrysEmacs8Document"; Tasks: "option_register_emacs_open_c_one_type"
Root: HKCR; Subkey: ".cpp"; ValueType: string; ValueData: "BarrysEmacs8Document"; Tasks: "option_register_emacs_open_c_one_type"
Root: HKCR; Subkey: ".cxx"; ValueType: string; ValueData: "BarrysEmacs8Document"; Tasks: "option_register_emacs_open_c_one_type"

Root: HKCR; Subkey: ".h";   ValueType: string; ValueData: "BarrysEmacs8DocumentII"; Tasks: "option_register_emacs_open_c_many_types"
Root: HKCR; Subkey: ".hh";  ValueType: string; ValueData: "BarrysEmacs8DocumentII"; Tasks: "option_register_emacs_open_c_many_types"
Root: HKCR; Subkey: ".hpp"; ValueType: string; ValueData: "BarrysEmacs8DocumentII"; Tasks: "option_register_emacs_open_c_many_types"
Root: HKCR; Subkey: ".hxx"; ValueType: string; ValueData: "BarrysEmacs8DocumentII"; Tasks: "option_register_emacs_open_c_many_types"

Root: HKCR; Subkey: ".c";   ValueType: string; ValueData: "BarrysEmacs8DocumentIII"; Tasks: "option_register_emacs_open_c_many_types"
Root: HKCR; Subkey: ".cc";  ValueType: string; ValueData: "BarrysEmacs8DocumentIII"; Tasks: "option_register_emacs_open_c_many_types"
Root: HKCR; Subkey: ".cpp"; ValueType: string; ValueData: "BarrysEmacs8DocumentIII"; Tasks: "option_register_emacs_open_c_many_types"
Root: HKCR; Subkey: ".cxx"; ValueType: string; ValueData: "BarrysEmacs8DocumentIII"; Tasks: "option_register_emacs_open_c_many_types"
