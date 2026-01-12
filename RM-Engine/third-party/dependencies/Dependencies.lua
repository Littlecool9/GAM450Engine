-- Central registry of third-party dependency paths

ThirdParty = {}
ThirdParty.Root       = "RM-Engine/third-party"
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

-- =========================
-- Header-only dependencies
-- =========================
IncludeDir["spdlog"] = ThirdParty.HeaderOnly .. "/spdlog/include"

-- =========================
-- Prebuilt dependencies
-- =========================
IncludeDir["glfw"] = ThirdParty.Prebuilt .. "/glfw/include"
LibDir["glfw"]     = ThirdParty.Prebuilt .. "/glfw/lib-vc2022"
BinDir["glfw"]     = ThirdParty.Prebuilt .. "/glfw/lib-vc2022"

-- =========================
-- CMake-built dependencies (installed output)
-- You get these after running scripts/ThirdParty/Build*.bat
-- =========================
-- Example: assimp installed to RM-Engine/third-party/install/assimp/...
-- IncludeDir["assimp"] = ThirdParty.Install .. "/assimp/include"
-- LibDir["assimp"]     = ThirdParty.Install .. "/assimp/lib"
-- BinDir["assimp"]     = ThirdParty.Install .. "/assimp/bin"