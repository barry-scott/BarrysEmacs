;
; -- bemacs.iss --
;

;
; Notes:
;   Uninstall key has the _is1 added by inno
;   32 bit apps are have uninstall keys in HKLM32
;   64 bit apps are have uninstall keys in HKLM64
;

#define AppName "Barry's Emacs 8"
; need to double up the apostrophe for use inside of pascal strings
#define AppId   "Barry''s Emacs 8"

[Code]
procedure UninstallOldVersions( root_key : Integer );
var
    uninstall_image     : string;
    error               : Integer;
    rci                 : Integer;
    rcb                 : Boolean;
begin
    error := 0;
    rcb := RegQueryStringValue( root_key,
        'SOFTWARE\Microsoft\Windows\CurrentVersion\Uninstall\{#AppId}_is1',
        'UninstallString', uninstall_image );
    if rcb then
    begin
        rci := MsgBox( 'An old version of {#AppId} is installed.' #13 #13
                      'It must be uninstalled before installing the this version' #13
                      'Do you wish to uninstall it now?', mbConfirmation, MB_YESNO );
        if rci = idYes then
        begin
            rcb := Exec( RemoveQuotes( uninstall_image ), '',
                            ExpandConstant('{src}'), SW_SHOWNORMAL, ewWaitUntilTerminated, Error );
            if not rcb then
                MsgBox( 'Failed to run the uninstall procedure.' #13 #13
                    'Please uninstall the old Barry''s Emacs' #13
                    'and try this installation again.', mbError, MB_OK );
            if error <> 0 then
                MsgBox( 'Failed to run the uninstall procedure.' #13 #13
                        'Please uninstall the old Barry''s Emacs' #13
                        'and try this installation again.', mbError, MB_OK );
        end;
    end;
end;

function InitializeSetup() : Boolean;
var
    uninstall_string    : string;
    rc32                : Boolean;
    rc64                : Boolean;
begin
    UninstallOldVersions( HKLM32 );
    UninstallOldVersions( HKLM64 );

    BringToFrontAndRestore;
    rc32 := RegQueryStringValue( HKLM32,
        'SOFTWARE\Microsoft\Windows\CurrentVersion\Uninstall\{#AppId}_is1',
        'UninstallString', uninstall_string );
    rc64 := RegQueryStringValue( HKLM64,
        'SOFTWARE\Microsoft\Windows\CurrentVersion\Uninstall\{#AppId}_is1',
        'UninstallString', uninstall_string );
    Result := not (rc32 or rc64);
    if not Result then
        MsgBox( 'Quitting installation.' #13 #13
                'An old version of {#AppId} is still installed.' #13
                'Run this installation again after the old version has' #13
                'been uninstalled', mbInformation, MB_OK );
end;

[Setup]
AppName={#AppName}
AppVerName=Barry's Emacs %(maturity)s%(major)s.%(minor)s
AppCopyright=Copyright (C) 1991-%(year)s Barry A. Scott
DefaultDirName={pf}\Barry Scott\{#AppName}
DefaultGroupName={#AppName}
UninstallDisplayIcon={app}\bemacs.exe
ChangesAssociations=yes
DisableStartupPrompt=yes
InfoBeforeFile=info_before.txt
Compression=bzip/9
ArchitecturesAllowed=x64
ArchitecturesInstallIn64BitMode=x64

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

Source: "..\Readme.txt"; DestDir: "{app}";

Source: "..\..\HTML\*.css";  DestDir: "{app}\Documentation";
Source: "..\..\HTML\*.html"; DestDir: "{app}\Documentation";
Source: "..\..\HTML\*.gif";  DestDir: "{app}\Documentation";
Source: "..\..\HTML\*.js";   DestDir: "{app}\Documentation";

#include "bemacs-kitfiles.iss"

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
Name: "{commondesktop}\{#AppName}"; Filename: "{app}\bemacs.exe"; Tasks: "option_desktop_icon"

;
;    Add an Emacs icon to the Start menu
;
Name: "{commonstartmenu}\{#AppName}"; Filename: "{app}\bemacs.exe"; Tasks: "option_start_menu_icon"

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
Root: HKCR; Subkey: "*\shell\Edit with {#AppName}"; ValueType: string; ValueData: "Edit with &Barry's Emacs 8"; Flags: uninsdeletekey
Root: HKCR; Subkey: "*\shell\Edit with {#AppName}\command"; ValueType: string; ValueData: """{app}\bemacs.exe"" ""%%1"""

Root: HKCR; Subkey: "Drive\shell\{#AppName} Here"; ValueType: string; ValueData: "Barry's Emacs 8 &Here"; Flags: uninsdeletekey
Root: HKCR; Subkey: "Drive\shell\{#AppName} Here\command"; ValueType: string; ValueData: """{app}\bemacs.exe"" /package=cd-here ""%%1\.."""

Root: HKCR; Subkey: "Directory\shell\{#AppName} Here"; ValueType: string; ValueData: "Barry's Emacs 8 &Here"; Flags: uninsdeletekey
Root: HKCR; Subkey: "Directory\shell\{#AppName} Here\command"; ValueType: string; ValueData: """{app}\bemacs.exe"" /package=cd-here ""%%1"""

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
