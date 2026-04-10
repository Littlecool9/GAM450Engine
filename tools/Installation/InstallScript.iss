;   TCR compliant install script for Aural Mischief.
;   Put this file in the root for what you want included in the installer,
;   then put everything into the subdirectories as listed in the installer below.
;   Based on the DigiPen sample script by Chris Onorati.
;
;   BEFORE COMPILING:
;   1. Populate GAMEDIRECTORY\ from the Release build output.
;   2. Ensure REDIST\vc_redist.x64.exe is present (not committed to git).
;   See /.claude/aural-mischief/plans/installer-and-release-stripping.md for full workflow.

[Setup]

#define ApplicationName 'AuralMischief'
#define ApplicationVersion '1.0'
#define CPlusPlusYearVersion '2022'

DisableWelcomePage=no
WizardImageFile=INSTALLERFILES\panel_image_100.bmp
WizardSmallImageFile=INSTALLERFILES\banner.bmp

AppId={{63628783-F830-446C-A0CA-0C7480DE9CBD}

; Standard app data stuff
AppName={#ApplicationName}
AppVerName={#ApplicationName}
VersionInfoVersion = {#ApplicationVersion}
AppVersion = {#ApplicationVersion}
AppPublisher=DigiPen Institute of Technology
AppPublisherURL=http://www.digipen.edu/
AppSupportURL=http://www.digipen.edu/

; Default path to the file storage directory.
; {pf} is the default program files directory set by Windows
DefaultDirName={autopf}\DigiPen\{#ApplicationName}

; Start menu directory
DefaultGroupName=DigiPen\{#ApplicationName}

; Output directory for the installer.
OutputDir=.\INSTALLER

; Setup executable installer
OutputBaseFilename={#ApplicationName}_Setup

; Path to the DigiPen EULA (Needed to pass TCRs)
LicenseFile=INSTALLERFILES\DigiPen_EULA.txt

; Compression scheme for the installer.
Compression=lzma
SolidCompression=yes

; Path to the icon for the installer (TCR check requires custom icon)
SetupIconFile=.\INSTALLERFILES\setup_icon.ico

; Allows installation without admin privileges (required for Sandbox)
PrivilegesRequired=none

; Prevent the installer from asking for a reboot
AlwaysRestart=no
RestartIfNeededByRun=no

[Languages]
Name: "english"; MessagesFile: "compiler:Default.isl"

[Tasks]
; Creates an installer option to allow/disallow desktop shortcut -- checked by default
Name: "desktopicon"; Description: "{cm:CreateDesktopIcon}"; GroupDescription: "{cm:AdditionalIcons}"

[Files]

; Load all DPI-scaled panel images into temp so the [Code] section can pick the right one
Source: ".\INSTALLERFILES\panel_image_*.bmp"; Flags: dontcopy

; The game directory is exactly what the install directory should look like
Source: .\GAMEDIRECTORY\*; DestDir: {app}; Flags: ignoreversion recursesubdirs createallsubdirs

; VC++ 2022 x64 redistributable -- deleted from temp after install
Source: ".\REDIST\vc_redist.x64.exe"; DestDir: {tmp}; Flags: deleteafterinstall

[Icons]
Name: {group}\{#ApplicationName}; Filename: {app}\{#ApplicationName}.exe; WorkingDir: {app}; IconFilename: "{app}\icon.ico"
Name: {group}\{cm:UninstallProgram,{#ApplicationName}}; Filename: {uninstallexe}
Name: {commondesktop}\{#ApplicationName}; Filename: {app}\{#ApplicationName}.exe; Tasks: desktopicon; WorkingDir: {app}; IconFilename: "{app}\icon.ico"

[Run]
Filename: {tmp}\vc_redist.x64.exe; Parameters: /q /Q /quiet /passive /silent /norestart /NORESTART /noreboot /NOREBOOT; StatusMsg: Installing Visual C++ {#CPlusPlusYearVersion} Redistributable...
Filename: {app}\{#ApplicationName}.exe; Description: {cm:LaunchProgram,{#ApplicationName}}; Flags: nowait postinstall skipifsilent

[UninstallDelete]

[Code]

// Scale the image based on DPI of monitor screen.
function GetScalingFactor: Integer;
begin
  if WizardForm.Font.PixelsPerInch >= 192 then Result := 200
    else
  if WizardForm.Font.PixelsPerInch >= 144 then Result := 150
    else
  if WizardForm.Font.PixelsPerInch >= 120 then Result := 125
    else Result := 100;
end;

// Set image of the far left side panel.
procedure LoadEmbededScaledBitmap(Image: TBitmapImage; NameBase: string);

var Name: String;
var FileName: String;

begin
  Name := Format('%s_%d.bmp', [NameBase, GetScalingFactor]);
  ExtractTemporaryFile(Name);
  FileName := ExpandConstant('{tmp}\' + Name);
  Image.Bitmap.LoadFromFile(FileName);
  DeleteFile(FileName);
end;

// Set images based on resolution.
procedure InitializeWizard;

begin
  { If using larger scaling, load the correct size of images }
  if GetScalingFactor > 100 then
  begin
    LoadEmbededScaledBitmap(WizardForm.WizardBitmapImage, 'panel_image');
  end;

end;
