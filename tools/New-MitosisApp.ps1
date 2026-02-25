#Requires -Version 5.1
<#
.SYNOPSIS
    Scaffolds a new Mitosis Engine application project.

.DESCRIPTION
    Creates the directory structure, .vcxproj, C++ boilerplate, and patches
    Mitosis-Engine.sln so the new project is immediately buildable in Visual Studio.

    The script must live in the tools/ subdirectory of the repository root.

.PARAMETER ProjectName
    Name of the new application. Must start with a letter and contain only
    letters, digits, underscores, or hyphens (e.g. "MyGame", "Rhythm-Game").

.PARAMETER DryRun
    Print every action that would be taken without writing or modifying any files.

.EXAMPLE
    .\tools\New-MitosisApp.ps1 -ProjectName "MyGame"

.EXAMPLE
    .\tools\New-MitosisApp.ps1 -ProjectName "MyGame" -DryRun
#>

[CmdletBinding()]
param(
    [Parameter(Mandatory, HelpMessage = "Name of the new application project.")]
    [string]$ProjectName,

    [switch]$DryRun
)

Set-StrictMode -Version Latest
$ErrorActionPreference = "Stop"

# --- Known GUIDs (must stay in sync with the actual .vcxproj files) -----------
$VC_CPP_TYPE_GUID = "{8BC9CEB8-8B4A-11D0-8D11-00A0C91BC942}"   # standard C++ project type
$ENGINE_GUID      = "{90B97858-7C87-6C5E-651B-DD84518848C3}"   # Mitosis-Engine.vcxproj
$LAUNCHER_GUID    = "{4CDFA107-38D8-C059-610A-A6DE4D62A065}"   # Mitosis-Launcher.vcxproj

# --- Resolve paths ------------------------------------------------------------
$repoRoot     = Split-Path $PSScriptRoot -Parent
$slnPath      = Join-Path $repoRoot "Mitosis-Engine.sln"
$templatePath = Join-Path $PSScriptRoot "AppTemplate.vcxproj.template"
$projDir      = Join-Path $repoRoot $ProjectName
$srcDir       = Join-Path $projDir "src"

# --- Output helpers -----------------------------------------------------------
function Write-Step([string]$Msg) { Write-Host "  > $Msg" -ForegroundColor Cyan }
function Write-OK([string]$Msg)   { Write-Host "  + $Msg" -ForegroundColor Green }
function Write-Dry([string]$Msg)  { Write-Host "    [dry-run] $Msg" -ForegroundColor DarkYellow }

function Invoke-CreateDir([string]$Path) {
    if ($DryRun) { Write-Dry "mkdir  $Path"; return }
    New-Item -ItemType Directory -Path $Path -Force | Out-Null
    Write-OK "mkdir  $Path"
}

function Invoke-WriteFile([string]$Path, [string]$Content) {
    if ($DryRun) { Write-Dry "write  $Path"; return }
    $parent = Split-Path $Path -Parent
    if (-not (Test-Path $parent)) {
        New-Item -ItemType Directory -Path $parent -Force | Out-Null
    }
    [System.IO.File]::WriteAllText($Path, $Content, [System.Text.UTF8Encoding]::new($false))
    Write-OK "write  $Path"
}

# --- Banner -------------------------------------------------------------------
Write-Host ""
Write-Host "  Mitosis Engine - New Application Scaffolder" -ForegroundColor White
Write-Host "  ---------------------------------------------" -ForegroundColor DarkGray
if ($DryRun) { Write-Host "  (DRY RUN - no files will be written)" -ForegroundColor Yellow }
Write-Host ""

# ==============================================================================
# 1. Validate
# ==============================================================================
Write-Step "Validating..."

if ($ProjectName -notmatch '^[A-Za-z][A-Za-z0-9_\-]*$') {
    throw "ProjectName must start with a letter and contain only A-Z, a-z, 0-9, _ or -."
}

if (-not (Test-Path $slnPath)) {
    throw "Solution file not found: '$slnPath'`nEnsure this script lives in the tools/ subdirectory."
}

if (-not (Test-Path $templatePath)) {
    throw "vcxproj template not found: '$templatePath'`nEnsure AppTemplate.vcxproj.template is in the same tools/ directory."
}

if (Test-Path $projDir) {
    throw "Directory already exists: '$projDir'`nRemove it or choose a different project name."
}

# Guard against double-registration in the solution
$slnRaw = [System.IO.File]::ReadAllText($slnPath)
if ($slnRaw -match ('= "' + [regex]::Escape($ProjectName) + '"')) {
    throw "The solution already references a project named '$ProjectName'."
}

Write-OK "Validation passed."

# ==============================================================================
# 2. Generate project GUID
# ==============================================================================
$guid = [System.Guid]::NewGuid().ToString("B").ToUpper()
Write-OK "Generated GUID: $guid"

# ==============================================================================
# 3. Create directory structure
# ==============================================================================
Write-Host ""
Write-Step "Creating directory structure..."

Invoke-CreateDir $srcDir
Invoke-CreateDir (Join-Path $srcDir  "Script")
Invoke-CreateDir (Join-Path $projDir "Assets")
Invoke-CreateDir (Join-Path $projDir "Data")

# ==============================================================================
# 4. Write the .vcxproj (from template)
# ==============================================================================
Write-Host ""
Write-Step "Writing $ProjectName.vcxproj..."

$templateContent = [System.IO.File]::ReadAllText($templatePath)
$vcxprojContent  = $templateContent `
    -replace '__APPNAME__', $ProjectName `
    -replace '__GUID__',    $guid

Invoke-WriteFile (Join-Path $projDir "$ProjectName.vcxproj") $vcxprojContent

# ==============================================================================
# 5. Write C++ boilerplate
# ==============================================================================
Write-Host ""
Write-Step "Writing source files..."

# pch.h
Invoke-WriteFile (Join-Path $srcDir "pch.h") @"
#pragma once
#include <MitosisEngine.h>
"@

# pch.cpp
Invoke-WriteFile (Join-Path $srcDir "pch.cpp") @"
#include "pch.h"
"@

# <ProjectName>Scene.h
Invoke-WriteFile (Join-Path $srcDir "${ProjectName}Scene.h") @"
#pragma once
#include "pch.h"

class ${ProjectName}Scene : public mts::Scene
{
   public:
    ~${ProjectName}Scene() override;

   protected:
    void OnInit() override;
    void OnUpdate() override;
};
"@

# <ProjectName>Scene.cpp
Invoke-WriteFile (Join-Path $srcDir "${ProjectName}Scene.cpp") @"
#include "pch.h"
#include "${ProjectName}Scene.h"

${ProjectName}Scene::~${ProjectName}Scene()
{
}

void ${ProjectName}Scene::OnInit()
{
}

void ${ProjectName}Scene::OnUpdate()
{
}
"@

# <ProjectName>.h
Invoke-WriteFile (Join-Path $srcDir "${ProjectName}.h") @"
#pragma once
#include "pch.h"
#include "${ProjectName}Scene.h"

class ${ProjectName} : public mts::Application
{
   public:
    ${ProjectName}();
    ~${ProjectName}();

   private:
    mts::Scene* GetInitialScene() override;

   private:
    ${ProjectName}Scene* m_scene;
};
"@

# <ProjectName>.cpp
Invoke-WriteFile (Join-Path $srcDir "${ProjectName}.cpp") @"
#include "pch.h"
#include "${ProjectName}.h"

mts::Application* mts::CreateApplication()
{
    return new ${ProjectName}();
}

${ProjectName}::${ProjectName}()
{
    m_scene = new ${ProjectName}Scene();
}

${ProjectName}::~${ProjectName}()
{
    delete m_scene;
}

mts::Scene* ${ProjectName}::GetInitialScene()
{
    return m_scene;
}
"@

# ==============================================================================
# 6. Patch Mitosis-Engine.sln
# ==============================================================================
Write-Host ""
Write-Step "Patching Mitosis-Engine.sln..."

# Work line-by-line so we never disturb the existing content.
# The .sln file uses CRLF; split strips endings, rejoin restores them.
[string[]]$lines = $slnRaw -split '\r?\n'

# --- 6a. Insert Project() declaration before the "Global" line ---------------
$globalIdx = -1
for ($i = 0; $i -lt $lines.Count; $i++) {
    if ($lines[$i] -eq 'Global') { $globalIdx = $i; break }
}
if ($globalIdx -lt 0) { throw "Malformed solution: cannot find 'Global' keyword." }

[string[]]$projDecl = @(
    ('Project("{0}") = "{1}", "{1}\{1}.vcxproj", "{2}"' -f $VC_CPP_TYPE_GUID, $ProjectName, $guid),
    "EndProject"
)

$lines = @($lines[0..($globalIdx - 1)]) + $projDecl + @($lines[$globalIdx..($lines.Count - 1)])

# --- 6b. Insert config mappings before the closing EndGlobalSection ----------
# Scan forward until inside ProjectConfigurationPlatforms, then find its end.
$inPlatforms  = $false
$configEndIdx = -1
for ($i = 0; $i -lt $lines.Count; $i++) {
    if ($lines[$i] -match 'GlobalSection\(ProjectConfigurationPlatforms\)') {
        $inPlatforms = $true
        continue
    }
    if ($inPlatforms -and $lines[$i] -match '^\tEndGlobalSection') {
        $configEndIdx = $i
        break
    }
}
if ($configEndIdx -lt 0) { throw "Malformed solution: cannot find end of ProjectConfigurationPlatforms section." }

[string[]]$cfgLines = @(
    "`t`t$guid.Debug|x64.ActiveCfg = Debug|x64",
    "`t`t$guid.Debug|x64.Build.0 = Debug|x64",
    "`t`t$guid.Release|x64.ActiveCfg = Release|x64",
    "`t`t$guid.Release|x64.Build.0 = Release|x64"
)

$lines = @($lines[0..($configEndIdx - 1)]) + $cfgLines + @($lines[$configEndIdx..($lines.Count - 1)])

# --- Write back (UTF-8, no BOM, CRLF) ----------------------------------------
$newSlnContent = $lines -join "`r`n"

if ($DryRun) {
    Write-Dry "write  $slnPath"
    Write-Host ""
    Write-Host "  Diff preview (lines to be added):" -ForegroundColor DarkGray
    Write-Host "    + $($projDecl[0])" -ForegroundColor DarkGreen
    Write-Host "    + $($projDecl[1])" -ForegroundColor DarkGreen
    foreach ($cl in $cfgLines) { Write-Host "    + $cl" -ForegroundColor DarkGreen }
} else {
    [System.IO.File]::WriteAllText($slnPath, $newSlnContent, [System.Text.UTF8Encoding]::new($false))
    Write-OK "Patched $slnPath"
}

# ==============================================================================
# 7. Summary
# ==============================================================================
$sceneName = "${ProjectName}Scene"

Write-Host ""
Write-Host "  ---------------------------------------------" -ForegroundColor DarkGray
if ($DryRun) {
    Write-Host "  Dry run complete. No files were written." -ForegroundColor Yellow
} else {
    Write-Host "  Done!" -ForegroundColor Green
}
Write-Host ""
Write-Host "  Project : $ProjectName" -ForegroundColor White
Write-Host "  GUID    : $guid"        -ForegroundColor White
Write-Host "  Location: $projDir"     -ForegroundColor White
Write-Host ""
Write-Host "  Files created:" -ForegroundColor DarkGray
Write-Host "    $ProjectName\$ProjectName.vcxproj"   -ForegroundColor Gray
Write-Host "    $ProjectName\src\pch.h / pch.cpp"    -ForegroundColor Gray
Write-Host "    $ProjectName\src\$ProjectName.h / $ProjectName.cpp"           -ForegroundColor Gray
Write-Host "    $ProjectName\src\${sceneName}.h / ${sceneName}.cpp"           -ForegroundColor Gray
Write-Host ""
if (-not $DryRun) {
    Write-Host "  Next steps:" -ForegroundColor DarkGray
    Write-Host "    1. Reload Mitosis-Engine.sln in Visual Studio" -ForegroundColor Gray
    Write-Host "       (File > Reload Solution  or  close and reopen)" -ForegroundColor Gray
    Write-Host "    2. Build Solution (Ctrl+Shift+B)" -ForegroundColor Gray
    Write-Host "    3. Add game logic to ${sceneName}::OnInit() and OnUpdate()" -ForegroundColor Gray
    Write-Host ""
}
