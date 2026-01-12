# Third-Party Dependencies (RM-Engine)

We support 3 kinds of dependencies:

## 1) Header-only (`third-party/header-only/`)

Examples: spdlog, glm (often)

- Stored as submodules or vendored source
- No build step required
- Premake uses `includedirs` only

## 2) Prebuilt (`third-party/prebuilt/`)

Examples: glfw (DLL + import lib)

- We store the ready-to-link `include/`, `*.lib`, and optional `*.dll`
- Premake uses:
  - `includedirs`
  - `libdirs`
  - `links`
  - and copies runtime DLLs into `bin/` after build (postbuild step)

## 3) Source (CMake) (`third-party/source/`)

Examples: assimp, freetype, etc.

- We keep the library source under `third-party/source/<LibName>`
- We build it with CMake into:
  - `third-party/build/<LibName>/...` (build tree)
  - `third-party/install/<LibName>/...` (clean install output used by Premake)

---

# First-time Setup

## Prerequisites

- Visual Studio 2022 (with C++ desktop workload)
- CMake (available in PATH)
- Git with submodules enabled

## Clone

```bash
git clone --recurse-submodules <repo>
```

## Update

If you have already cloned without submodules:

```bash
git submodule update --init --recursive
```

## Build third-party CMake libs

From third-party folder root:

```bat
scripts\ThirdParty\BuildAll.bat
```

Or build a single one by specifying:

```bat
scripts\ThirdParty\BuildOne.bat assimp
```

This will populate:

- `RM-Engine/third-party/install/<LibName>/include`
- `RM-Engine/third-party/install/<LibName>/lib`
- `RM-Engine/third-party/install/<LibName>/bin` (if DLLs)

## Generate Visual Studio solution

Finally, after all the initial libraries setup is completed, run:

```bat
scripts\GenerateProjects.bat
```

# Adding a new dependency

## Header-only

1. Put it in `third-party/header-only/<name>`
2. Add `IncludeDir["name"] = ...` in `third-party/dependencies/Dependencies.lua`
3. Add `includedirs { IncludeDir["name"] }` to the project that uses it

## Prebuilt

1. Put it in `third-party/prebuilt/<name>/{include,lib-...,bin...}`
2. Add `IncludeDir/LibDir/BinDir` entries in `Dependencies.lua`
3. In Premake:

    - `libdirs { LibDir["name"] }`
    - `links { "<libname>.lib" }`
    - copy DLLs via `postbuildcommands` (or `CopyRuntimeDLL` helper)

## CMake source

1. Put it in `third-party/source/<name>`
2. Add it to scripts/ThirdParty/BuildAll.bat list
3. Run `BuildAll.bat` (or `BuildOne.bat <name>`)
4. Add `IncludeDir/LibDir/BinDir` entries for installed output:

    - `third-party/install/<name>/...`

5. Link it in Premake via `libdirs/links`
