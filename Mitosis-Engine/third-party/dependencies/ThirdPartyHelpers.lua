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