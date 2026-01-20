function LinkPrebuilt(name, libs)
    includedirs { IncludeDir[name] }
    libdirs { LibDir[name] }
    links(libs)
end

function CopyRuntimeDLL(name, dllFileName)
    if BinDir[name] == nil then
        print("CopyRuntimeDLL: BinDir missing for dep: " .. name)
        return
    end

    local src = '%{wks.location}/' .. BinDir[name] .. '/' .. dllFileName
    local dst = '%{cfg.targetdir}/' .. dllFileName

    postbuildcommands {
        '{MKDIR} "%{cfg.targetdir}"',
        '{COPYFILE} "' .. src .. '" "' .. dst .. '"'
    }
end

function LinkInstalled(name, libs)
    includedirs { IncludeDir[name] }
    libdirs { LibDir[name] }
    links(libs)
end

-- =========================
-- FMOD (External SDK via FMOD_SDK env var)
-- =========================

local function _GetFMODRoot()
    local root = os.getenv("FMOD_SDK")
    if (not root) or (#root == 0) then
        return nil
    end
    return path.translate(root)
end

function AddFMODHeaders()
    local fmodRoot = _GetFMODRoot()
    if not fmodRoot then
        print("[FMOD] FMOD_SDK env var not set. FMOD headers disabled.")
        defines { "MTS_AUDIO_FMOD=0" }
        return false
    end

    local coreInc = path.join(fmodRoot, "api/core/inc")
    local studioInc = path.join(fmodRoot, "api/studio/inc")

    if not os.isdir(coreInc) or not os.isdir(studioInc) then
        print("[FMOD] FMOD_SDK invalid. Expected api/core/inc and api/studio/inc.")
        print("[FMOD] FMOD_SDK=" .. fmodRoot)
        defines { "MTS_AUDIO_FMOD=0" }
        return false
    end

    defines { "MTS_AUDIO_FMOD=1" }
    includedirs { coreInc,studioInc }

    print("[FMOD] Headers enabled via FMOD_SDK=" .. fmodRoot)
    return true
end

function LinkFMODStudio()
    local fmodRoot = _GetFMODRoot()
    if not fmodRoot then
        print("[FMOD] FMOD_SDK env var not set. FMOD link disabled.")
        defines { "MTS_AUDIO_FMOD=0" }
        return false
    end

    local coreLib64   = path.join(fmodRoot, "api/core/lib/x64")
    local studioLib64 = path.join(fmodRoot, "api/studio/lib/x64")

    if not os.isdir(coreLib64) or not os.isdir(studioLib64) then
        print("[FMOD] FMOD libs not found. Expected api/*/lib/x64.")
        print("[FMOD] FMOD_SDK=" .. fmodRoot)
        defines { "MTS_AUDIO_FMOD=0" }
        return false
    end

    AddFMODHeaders()

    libdirs { coreLib64, studioLib64 }

    filter "configurations:Debug"
        links { "fmodL_vc", "fmodstudioL_vc" }
        postbuildcommands {
            '{MKDIR} "%{cfg.targetdir}"',
            '{COPYFILE} "' .. path.join(coreLib64, "fmodL.dll") .. '" "%{cfg.targetdir}/fmodL.dll"',
            '{COPYFILE} "' .. path.join(studioLib64, "fmodstudioL.dll") .. '" "%{cfg.targetdir}/fmodstudioL.dll"'
        }
    
    filter "configurations:Release"
        links { "fmod_vc", "fmodstudio_vc" }
        postbuildcommands {
            '{MKDIR} "%{cfg.targetdir}"',
            '{COPYFILE} "' .. path.join(coreLib64, "fmod.dll") .. '" "%{cfg.targetdir}/fmod.dll"',
            '{COPYFILE} "' .. path.join(studioLib64, "fmodstudio.dll") .. '" "%{cfg.targetdir}/fmodstudio.dll"'
        }
        
    filter {}

    print("[FMOD] Linked Studio via FMOD_SDK=" .. fmodRoot)
    return true
end
