-- Central registry of third-party dependency paths

ThirdParty = {}
ThirdParty.Root       = "Mitosis-Engine/third-party"
ThirdParty.HeaderOnly = ThirdParty.Root .. "/header-only"
ThirdParty.Prebuilt   = ThirdParty.Root .. "/prebuilt"
ThirdParty.Source     = ThirdParty.Root .. "/source"
ThirdParty.Build      = ThirdParty.Root .. "/build"
ThirdParty.Install    = ThirdParty.Root .. "/install"

IncludeDir = {}
LibDir     = {}
BinDir     = {}

-- =========================
-- Source-built (compiled into engine)
-- =========================
IncludeDir["glad"] = ThirdParty.Source .. "/glad/include"
IncludeDir["imgui"] = ThirdParty.Source .. "/imgui"
IncludeDir["imgui_backends"] = ThirdParty.Source .. "/imgui/backends"

-- =========================
-- Header-only dependencies
-- =========================
IncludeDir["spdlog"] = ThirdParty.HeaderOnly .. "/spdlog/include"
IncludeDir["rapidjson"] = ThirdParty.HeaderOnly .. "/rapidjson/include"
IncludeDir["glm"] = ThirdParty.HeaderOnly .. "/glm"

-- =========================
-- Prebuilt dependencies
-- =========================
IncludeDir["glfw"] = ThirdParty.Prebuilt .. "/glfw/include"
LibDir["glfw"]     = ThirdParty.Prebuilt .. "/glfw/lib-vc2022"
BinDir["glfw"]     = ThirdParty.Prebuilt .. "/glfw/lib-vc2022"

-- =========================
-- vcpkg-managed dependencies
-- =========================
IncludeDir["boost"] = "vcpkg_installed/x64-windows/include"

-- =========================
-- CMake-built dependencies (installed output)
-- You get these after running scripts/ThirdParty/Build*.bat
-- =========================
-- Example: assimp installed to Mitosis-Engine/third-party/install/assimp/...
-- IncludeDir["assimp"] = ThirdParty.Install .. "/assimp/include"
-- LibDir["assimp"]     = ThirdParty.Install .. "/assimp/lib"
-- BinDir["assimp"]     = ThirdParty.Install .. "/assimp/bin"