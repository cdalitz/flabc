; -- setup.iss --
; InnoSetup script file for creating flabc installer package on Win32

[Setup]
AppName=flabc
AppVerName=flabc version 1.3.1
OutputDir=.
OutputBaseFilename=flabc-setup
DefaultDirName={pf}\flabc
DisableProgramGroupPage=yes
; ^ since no icons will be created in "{group}", we don't need the wizard
;   to ask for a group name.
UninstallDisplayIcon={app}\bin\flabc.exe
ChangesAssociations=yes

[Files]
Source: "bin\*"; DestDir: "{app}\bin"
Source: "doc\*"; DestDir: "{app}\doc"; Flags: recursesubdirs
Source: "fonts\*"; DestDir: "{app}\fonts"
Source: "templates\*"; DestDir: "{app}\templates"
Source: "examples\*"; DestDir: "{app}\examples"

[Icons]
Name: "{commonprograms}\flabc"; Filename: "{app}\bin\flabc.exe"
Name: "{userdesktop}\flabc"; Filename: "{app}\bin\flabc.exe"

; associate abc files with flabc
[Registry]
Root: HKCR; Subkey: ".abc"; ValueType: string; ValueName: ""; ValueData: "abcMusicFile"; Flags: uninsdeletevalue
Root: HKCR; Subkey: "abcMusicFile"; ValueType: string; ValueName: ""; ValueData: "abc Music File"; Flags: uninsdeletekey
Root: HKCR; Subkey: "abcMusicFile\DefaultIcon"; ValueType: string; ValueName: ""; ValueData: "{app}\bin\flabc.exe,0"
Root: HKCR; Subkey: "abcMusicFile\shell\open\command"; ValueType: string; ValueName: ""; ValueData: """{app}\bin\flabc.exe"" ""%1"""


