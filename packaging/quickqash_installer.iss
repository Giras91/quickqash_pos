; QuickQash Inno Setup script
; This script expects the following preprocessor variables passed via ISCC /D
;   MyAppVersion - version string (e.g. 0.1)
;   SourceDir - path to the installed application files (no trailing slash)
;   IconFile - path to the application icon (relative to repo root)

#define MyAppVersion "0.1"
#define SourceDir "installer\\install"
#define IconFile "resources\\icon.ico"

[Setup]
AppName=QuickQash
AppVersion={#MyAppVersion}
DefaultDirName={pf}\QuickQash
DefaultGroupName=QuickQash
OutputBaseFilename=QuickQashSetup-{#MyAppVersion}
Compression=lzma
SolidCompression=yes
LicenseFile=..\LICENSE
DisableProgramGroupPage=yes
SetupIconFile={#IconFile}

[Languages]
Name: "english"; MessagesFile: "compiler:Default.isl"

[Files]
Source: "{#SourceDir}\\*"; DestDir: "{app}"; Flags: ignoreversion recursesubdirs createallsubdirs

[Icons]
Name: "{group}\\QuickQash"; Filename: "{app}\\QuickQash.exe"; IconFilename: "{#IconFile}"

[Run]
Filename: "{app}\\QuickQash.exe"; Description: "Launch QuickQash"; Flags: nowait postinstall skipifsilent
