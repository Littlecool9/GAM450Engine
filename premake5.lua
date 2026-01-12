workspace "RM-Engine"
    configurations { "Debug", "Release" }
    architecture "x64"
    startproject "Sandbox"

-- ---------------------------------------------------------
-- Project names and directories
-- ---------------------------------------------------------

engineName = "RM-Engine"
launcherName = "RM-Launcher"

outputdir = "%{cfg.buildcfg}-%{cfg.system}-%{cfg.architecture}"

-- ---------------------------------------------------------
-- Third-party dependencies registry
-- ---------------------------------------------------------
dofile("RM-Engine/third-party/dependencies/Dependencies.lua")
dofile("RM-Engine/third-party/dependencies/ThirdPartyHelpers.lua")

-- ---------------------------------------------------------
-- Common settings helper
-- ---------------------------------------------------------
function ApplyCommonSettings()
    language "C++"
    cppdialect "C++20"
    staticruntime "off"

    targetdir ("bin/" .. outputdir .. "/%{prj.name}")
    objdir    ("bin-int/" .. outputdir .. "/%{prj.name}")

    buildoptions { "/utf-8" }

    filter "system:windows"
        systemversion "latest"
        defines { "RM_PLATFORM_WINDOWS" }

    filter "toolset:msc*"
        buildoptions { "/Zc:preprocessor" }

    filter "configurations:Debug"
        runtime "Debug"
        symbols "On"
        defines { "RM_DEBUG" }

    filter "configurations:Release"
        runtime "Release"
        optimize "On"
        defines { "RM_RELEASE" }

    filter {}
end

-- =========================================================
-- RM-Engine (Static Library)
-- =========================================================

project (engineName)
    location (engineName)
    kind "StaticLib"

    ApplyCommonSettings()

    pchheader "pch.h"
    pchsource "%{prj.name}/src/pch.cpp"

    files
    {
        "%{prj.name}/src/**.h",
        "%{prj.name}/src/**.cpp",
        
        -- Custom compilation for glad.c file (better design can be added later)
        "%{prj.name}/third-party/source/glad/src/glad.c",
    }

    includedirs
    {
        "%{prj.name}/src",

        -- Source deps (example; GLAD)
        IncludeDir["glad"],

        -- Header-only deps (example: spdlog)
        IncludeDir["spdlog"],

        -- Prebuilt deps (example: GLFW)
        IncludeDir["glfw"],
    }

    filter "files:RM-Engine/third-party/source/glad/src/glad.c"
        language "C"
        flags { "NoPCH" }
    filter {}

    -- If you want engine itself to link to some libs, do it here.

-- =========================================================
-- RM-Launcher (Static Library)
-- =========================================================

project (launcherName)
    location (launcherName)
    kind "StaticLib"

    ApplyCommonSettings()

    pchheader "pch.h"
    pchsource "%{prj.name}/src/pch.cpp"

    files
    {
        "%{prj.name}/src/**.h",
        "%{prj.name}/src/**.cpp"
    }

    includedirs
    {
        engineName .. "/src",
        IncludeDir["glad"],
    }

    links
    {
        engineName
    }

-- =========================================================
-- Sandbox (Console Application)
-- =========================================================

project "Sandbox"
    location "Sandbox"
    kind "ConsoleApp"

    ApplyCommonSettings()

    pchheader "pch.h"
    pchsource "%{prj.name}/src/pch.cpp"

    files
    {
        "%{prj.name}/src/**.h",
        "%{prj.name}/src/**.cpp",
        "%{prj.name}/**.h",
        "%{prj.name}/**.cpp"
    }

    includedirs
    {
        engineName .. "/src",        
        IncludeDir["glad"],
    }

    links
    {
        launcherName,
        "opengl32.lib",
        "user32.lib",
        "gdi32.lib",
        "shell32.lib",
    }

    -- -------------------------
    -- Prebuilt dependency: GLFW
    -- -------------------------
    LinkPrebuilt("glfw", { "glfw3dll.lib" })
    CopyRuntimeDLL("glfw", "glfw3.dll")

    -- -------------------------
    -- Example: CMake-installed dependency (if you add one)
    -- -------------------------
    -- LinkInstalled("assimp", { "assimp-vc143-mt.lib" }) -- (name differs per build; set yours)
    -- CopyRuntimeDLL("assimp", "assimp-vc143-mt.dll")

    filter {}