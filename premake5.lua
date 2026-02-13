workspace "Mitosis-Engine"
    configurations { "Debug", "Release" }
    architecture "x64"
    startproject "TechDemo"

-- ---------------------------------------------------------
-- Project names and directories
-- ---------------------------------------------------------

engineName = "Mitosis-Engine"
launcherName = "Mitosis-Launcher"

outputdir = "%{cfg.buildcfg}-%{cfg.system}-%{cfg.architecture}"

-- ---------------------------------------------------------
-- Third-party dependencies registry
-- ---------------------------------------------------------
dofile("Mitosis-Engine/third-party/dependencies/Dependencies.lua")
dofile("Mitosis-Engine/third-party/dependencies/ThirdPartyHelpers.lua")

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
        defines { "MTS_PLATFORM_WINDOWS" }

    filter "toolset:msc*"
        buildoptions { "/Zc:preprocessor" }

    filter "configurations:Debug"
        runtime "Debug"
        symbols "On"
        defines { "MTS_DEBUG" }

    filter "configurations:Release"
        runtime "Release"
        optimize "On"
        defines { "MTS_RELEASE" }

    filter {}
end

-- -----------------------------------------
-- App scaffolding helpers
-- -----------------------------------------
function EnsureDir(path)
    -- creates nested dirs fine in premake's os.mkdir
    os.mkdir(path)
end

function EnsureFile(path, contents)
    local f = io.open(path, "r")
    if f then f:close(); return end -- already exists
    f = io.open(path, "w")
    if not f then
        print("Failed to create file: " .. path)
        return
    end
    f:write(contents or "")
    f:close()
end

function ScaffoldApp(appDir, appName)
    EnsureDir(appDir)
    EnsureDir(appDir .. "/src")
    EnsureDir(appDir .. "/Assets")
    EnsureDir(appDir .. "/Data")

    -- -------------------------
    -- <AppName>Scene.h
    -- -------------------------
    EnsureFile(appDir .. "/src/" .. appName .. "Scene.h", string.format([[
#pragma once
#include <MitosisEngine.h>

class %sScene : public mts::Scene
{
public:
    ~%sScene() override;

protected:
    void OnInit() override {};
    void OnUpdate() override {};
};
]], appName, appName))
    -- -------------------------
    -- pch.h
    -- -------------------------
    EnsureFile(appDir .. "/src/pch.h", [[
#pragma once
#include <MitosisEngine.h>
]])

    -- -------------------------
    -- pch.cpp
    -- -------------------------
    EnsureFile(appDir .. "/src/pch.cpp", [[
#include "pch.h"
]])
    -- -------------------------
    -- <AppName>Scene.cpp
    -- -------------------------
    EnsureFile(appDir .. "/src/" .. appName .. "Scene.cpp", string.format([[
#include "pch.h"
#include "%sScene.h"

%sScene::~%sScene() = default;
]], appName, appName, appName))

    -- -------------------------
    -- <AppName>.h
    -- -------------------------
    EnsureFile(appDir .. "/src/" .. appName .. ".h", string.format([[
#pragma once
#include <MitosisEngine.h>

#include "%sScene.h"

class %s : public mts::Application
{
public:
    %s();
    ~%s();

private:
    mts::Scene* GetInitialScene() override { return exampleScene; }

private:
    %sScene* exampleScene;
};
]], appName, appName, appName, appName, appName))

    -- -------------------------
    -- <AppName>.cpp
    -- -------------------------
    EnsureFile(appDir .. "/src/" .. appName .. ".cpp", string.format([[
#include "pch.h"
#include "%s.h"

mts::Application* mts::CreateApplication()
{
    return new %s();
}

%s::%s()
{
    exampleScene = new %sScene();
}

%s::~%s()
{
    delete exampleScene;
}
]], appName, appName, appName, appName, appName, appName, appName))
end


-- ========================================================= 
-- App creation helper
-- =========================================================
function CreateApp(appName, appDir)
    project (appName)
        location (appDir)
        kind "ConsoleApp"

        ApplyCommonSettings()

        pchheader "pch.h"
        pchsource (appDir .. "/src/pch.cpp")

        files
        {
            appDir .. "/src/**.h",
            appDir .. "/src/**.cpp",
            appDir .. "/**.h",
            appDir .. "/**.cpp"
        }

        includedirs
        {
            engineName .. "/src",
            launcherName .. "/src",
            IncludeDir["glad"],
            IncludeDir["imgui"],
            IncludeDir["imgui_backends"],
            IncludeDir["boost"],
            IncludeDir["glm"],
        }

        links
        {
            launcherName,
            engineName,
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
        -- External dependency: FMOD (SDK via FMOD_SDK env var)
        -- Link + copy runtime DLLs for the executable
        -- -------------------------
        LinkFMODStudio()

        -- -------------------------
        -- Example: CMake-installed dependency (if you add one)
        -- -------------------------
        -- LinkInstalled("assimp", { "assimp-vc143-mt.lib" }) -- (name differs per build; set yours)
        -- CopyRuntimeDLL("assimp", "assimp-vc143-mt.dll") 
end

-- ========================================================= 
-- Mitosis-Engine (Static Library)
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

        "%{prj.name}/src/**.frag",
        "%{prj.name}/src/**.vert",
        
        -- Custom compilation for glad.c file (better design can be added later)
        "%{prj.name}/third-party/source/glad/src/glad.c",

        -- imgui core
        "%{prj.name}/third-party/source/imgui/imgui*.cpp",

        -- imgui backends
        "%{prj.name}/third-party/source/imgui/backends/imgui_impl_glfw.cpp",
        "%{prj.name}/third-party/source/imgui/backends/imgui_impl_opengl3.cpp",
    }

    includedirs
    {
        "%{prj.name}/src",

        -- Source deps (example; GLAD)
        IncludeDir["glad"],
        IncludeDir["imgui"],
        IncludeDir["imgui_backends"],

        -- Header-only deps (example: spdlog)
        IncludeDir["spdlog"],
        IncludeDir["rapidjson"],
        IncludeDir["glm"],
		IncludeDir["stb_image"],

        -- Prebuilt deps (example: GLFW)
        IncludeDir["glfw"],

        -- vcpkg-managed deps (example: Boost)
        IncludeDir["boost"],
    }

    -- -------------------------
    -- External dependency: FMOD (SDK via FMOD_SDK env var)
    -- -------------------------
    AddFMODHeaders()

    filter "files:Mitosis-Engine/third-party/source/glad/src/glad.c"
        language "C"
        flags { "NoPCH" }
    filter {}

    filter "files:Mitosis-Engine/third-party/source/imgui/**.cpp"
        flags { "NoPCH" }
    filter {}

    -- If you want engine itself to link to some libs, do it here.

-- =========================================================
-- Mitosis-Launcher (Static Library)
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
        IncludeDir["glm"],
        IncludeDir["boost"],
        IncludeDir["imgui"],
        IncludeDir["imgui_backends"],
    }

    links
    {
        engineName
    }

-- =========================================================
-- Sandbox (Console Application)
-- =========================================================
ScaffoldApp("Sandbox", "Sandbox")
CreateApp("Sandbox", "Sandbox")

-- =========================================================
-- TechDemo (Console Application)
-- =========================================================
ScaffoldApp("TechDemo", "TechDemo")
CreateApp("TechDemo", "TechDemo")